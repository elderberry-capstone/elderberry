#!/usr/bin/python3

import sys
import re
import yaml
import copy
from os import path, access, R_OK
from collections import defaultdict

class ParserStates:
    # Since we have multiple MIML files now we need phases for processing.
    # Expansion allows handler functions that expect data in other files the opportunity
    #   to pull in that external data and place it in the parse tree.
    # Validation allows handler functions the opportunity to examine other data in the tree
    #   to ensure it is ready for use, including data pulled in by other functions.
    # Parsing is where the actual parsing work happens, where handler functions generate output.
    # Actual output writting happens when all these phases are complete and is not part of "parsing".
    Expand, Validate, Parse = range(3)

class ErrorLogger:
    # Class Implementation complete and tested - RA
    def __init__(self):
        self.errors = []
        self.warnings = []

    def new_error(self, message):
        self.errors.append(message)

    def new_warning(self, message):
        self.warnings.append(message)

    def append_error(self, message):
        if len(self.errors) > 0: # append to empty list just adds new error.
            message = self.errors.pop() + message 
        self.errors.append(message)

    def append_warnings(self, message):
        if len(self.warnings) > 0: # append to empty list just adds new error.
            message = self.warnings.pop() + message 
        self.warnings.append(message)

    def has_errors(self):
        if len(self.errors) > 0:
            return True
        return False

    def has_warnings(self):
        if len(self.warnings) > 0:
            return True
        return False

    def check_file(self, filename):
        if path.isfile(filename) == False:
            self.new_error("File: '" + filename + "' cannot be found!")
        elif access(filename, R_OK) == False:
            self.new_error("File: '" + filename + "' cannot be opened for reading!")
        else:
            return True
        return False

    def check(self):
        if self.has_warnings():
            print (len(self.warnings), " warning(s) encountered!")
            for warning in self.warnings:
                print (warning)
        if self.has_errors():
            print (len(self.errors), " error(s) encountered!")
            for error in self.errors:
                print (error)
            sys.exit(0)


class OutputGenerator:
    # Tested. Still needs to put data into files.

    # What we want here is:
    #    RootDictionary { ModeDictionary { LevelDictionary { OutputList [] }}}
    #
    # So OutputGen{Code}{1}{Include File1, Include File2}
    # or OutputGen{Header1}{1}{Function PrototypeA, Function PrototypeB}
    #
    # This way different Handlers can be invoked for different purposes 
    #   and order their output as they wish.

    def __init__(self, mode_flags):
        self.output = defaultdict(lambda: defaultdict(list))
        self.mode_flags = mode_flags

    def append(self, mode, level, data):
        self.output[mode][level].append(data)

    def display(self):
        for mode in self.output.keys():
            print ("\n")
            for level in self.output[mode].keys():
                for message in self.output[mode][level]:
                    print (mode, "->", level, "->", message)

class Parser:

    def __init__(self, filename):
        self.errors = ErrorLogger()
        if self.argument_check():
            self.errors.check_file(filename)
        self.errors.check()
        try:
            self.parse_handlers = yaml.load(open(filename, 'r'))
        except Exception as e:
            self.errors.new_error("YAML parsing error: " + str(e))
        self.errors.check()
        for handler in self.parse_handlers.keys():
            self.parse_handlers[handler]['path'] = self.parse_handlers[handler]['path'].split("/")
        print (yaml.dump(self.parse_handlers))
        self.path = ['']
        self.state = None
        self.output = OutputGenerator(self.modes)

    def parse(self):
        # top level 'public' function. Since we have external MIML docs we need to pull those in
        # before we crawl, so order of processing matters even though order of MIML elements does not.
        try:
            self.master = yaml.load(open(self.miml_file, 'r'))
        except Exception as e:
            self.errors.new_error("YAML parsing error: " + str(e))
            self.errors.check()
        # Do Expand, Validate, Parse
        self.handler_functions = ParseHandlers(self)
        while self.transition() == True:
            print ("Starting Mode: " + str(self.state))
            self.crawl(self.master)
        # Output
        self.output.display()

    def crawl(self, data):
        if type(data).__name__=='dict':
            for key in data.keys():
                self.path.append(key)
                if self.matchpath(data[key]) == False:
                    self.crawl (data[key])
                self.path.pop()
        elif type(data).__name__=='list':
            for element in data:
                self.path.append(element)
                if self.matchpath(element) == False:
                    self.crawl (element) 
                self.path.pop()           
        else:
            self.matchpath(data)

    def transition(self):
        return_value = True
        if self.state == None:
            # Set new buffer, copy master to unhandled.
            self.buffer = {}
            self.unhandled = copy.copy(self.master)
            self.state = ParserStates.Expand
        elif self.state == ParserStates.Expand:
            # Make buffer contents master. 
            self.master = self.buffer
            self.buffer = {}
            print(yaml.dump(self.unhandled))
            self.unhandled = copy.copy(self.master)
            self.state = ParserStates.Validate
        elif self.state == ParserStates.Validate:
            self.state = ParserStates.Parse
        else:
            return_value = False
        self.errors.check()
        return return_value
        

    def matchpath(self, data):
        # This method returns True if a handler decides no other parsing is required for
        # the data it handles, for the mode it is in.
        # 
        # Goal: Flexible not complicated!!! We could support all kinds of crazy, but we won't!
        #    '*' means single position wildcard, not arbitrary number of elements.
        #    handler paths that do not begin with '/' should not contain any '/'.
        #      They represent single token matches for the immediate point.
        #
        # Possible Match Cases: self.path = a / self.parse_handlers[key]['path'] = b
        # 1 - len(a) == len(b) and ( a[x] == b[x] or b[x] == '*' )
        # 2 - len(a) != len(b) && len(b) == 1
        #
        # Someone may put '*' for path, not sure if I want to make that illegal.

        return_value = False
        for key in self.parse_handlers.keys():
            if len(self.path) == len(self.parse_handlers[key]['path']):
                match = True
                for position in range(0, len(self.path)):
                    if (not self.path[position] == self.parse_handlers[key]['path'][position] and
                            not self.parse_handlers[key]['path'][position] == '*'):
                        match = False
                if match == True:
                    print ("Match: " + str(self.path) + " | " + str(self.parse_handlers[key]['path']))          
                    return_value = return_value | getattr(self.handler_functions, key)(data)
            if len(self.parse_handlers[key]['path']) == 1 and self.parse_handlers[key]['path'][0] == self.path[-1]:
                print ("Match: " + str(self.path) + " | " + str(self.parse_handlers[key]['path']))          
                return_value = return_value | getattr(self.handler_functions, key)(data)
        return return_value

    def argument_check(self):

        if len(sys.argv) > 3 or len(sys.argv) < 2:
            self.errors.new_error("Illegal number of arguments! Expected 1 or 2, received: "
            + str(len(sys.argv) -1))
            return False
        self.modes = {'code': False, 'make': False, 'header': False}
        self.miml_file = sys.argv[len(sys.argv) - 1]
        mode_flags = "-cmh" if len(sys.argv) == 2 else self.set_modes(sys.argv[1])
        match = re.match(r"^-[cmh]+$", mode_flags)
        if match:
            if re.match(r"(.*c)", mode_flags):
                self.modes['code'] = True
            if re.match(r"(.*m)", mode_flags):
                self.modes['make'] = True
            if re.match(r"(.*h)", mode_flags):
                self.modes['header'] = True
            return True
        else:
            self.errors.new_error("Illegal mode usage, expecting -[chm]. Given : " + mode_flags)
        return False

    def debug(self):
        print ("\n\n" + yaml.dump(self.buffer))

class ParseHandlers:

    def __init__(self, parser):
        self.parser = parser
        self.num_sources = 0

    def parse_sources(self, sources):
        if self.parser.state == ParserStates.Expand:
            # Pull in external file data, place in buffer
            del(self.parser.unhandled['sources'])
            self.parser.buffer['modules'] = {}
            for source in sources:
                self.num_sources += 1                
                if (self.parser.errors.check_file(source[1])):
                    try:
                        self.parser.buffer['modules'][source[0]] = yaml.load(open(source[1], 'r'))
                    except Exception as e:
                        self.parser.errors.new_error("YAML parsing error: " + str(e))

            return True
        self.parser.errors.new_error("Call to sources handler outside of Expansion state.")
        return False # Maybe we got here because of someone adding new features and another handler...

    def parse_messages(self, data):
        if self.parser.state == ParserStates.Expand:
            # Nothing to expand, but buffer messages for later passes.
            del(self.parser.unhandled['messages'])
            self.parser.buffer['messages'] = data
            return True
        

parser = Parser('./cg.conf')
parser.parse()
parser.debug()
