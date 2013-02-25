#!/usr/bin/python3

import sys
import re
import yaml
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

class ErrorReporter:
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

    def __init__(self):
        self.output = defaultdict(lambda: defaultdict(list))

    def append(self, mode, level, data):
        self.output[mode][level].append(data)

    def display(self):
        for mode in self.output.keys():
            for level in self.output[mode].keys():
                for message in self.output[mode][level]:
                    print (mode, "->", level, "->", message)

class ArgumentChecker:

    def __init__(self, err):
        if len(sys.argv) > 3 or len(sys.argv) < 2:
            err.new_error("Illegal number of arguments! Expected 1 or 2, received: "
            + str(len(sys.argv) -1))
        else:
            self.modes = {'code': False, 'make': False, 'header': False}
            if len(sys.argv) == 2:
                self.set_modes("-cmh", err)
            else:
                self.set_modes(sys.argv[1], err)
            self.check_config(sys.argv[len(sys.argv) - 1], err)
        err.check()
            
    def set_modes(self, modes, err):
        match = re.match(r"^-[cmh]+$", modes)
        if match:
            if re.match(r"(.*c)", modes):
                self.modes['code'] = True
            if re.match(r"(.*m)", modes):
                self.modes['make'] = True
            if re.match(r"(.*h)", modes):
                self.modes['header'] = True
        else:
            err.new_error("Illegal mode usage, expecting -[chm]. Given : " + modes)

    def check_config(self, filename, err):
        if err.check_file(filename):
            self.miml_file = filename

class Parser:

    def __init__(self, filename, err):
        self.errors = err
        try:
            self.parse_handlers = yaml.load(open(filename, 'r'))
        except Exception as e:
            self.errors.new_error("YAML parsing error: " + str(e))
        self.errors.check()
        self.path = ['']
        self.output = OutputGenerator()
        self.handler_functions = ParseHandlers(self.output, self.errors)

    def parse(self, miml):
        # top level 'public' function. Since we have external MIML docs we need to pull those in
        # before we crawl, so order of processing matters even though order of MIML elements does not.
        try:
            self.root = yaml.load(open(miml, 'r'))
        except Exception as e:
            self.errors.new_error("YAML parsing error: " + str(e))
            self.errors.check()
        #  Expand
        self.mode = ParserStates.Expand
        self.crawl(self.root)
        self.errors.check()
        #  Validate
        self.mode = ParserStates.Validate
        self.crawl(self.root)
        self.errors.check()
        #  Expand
        self.mode = ParserStates.Parse
        self.crawl(self.root)
        self.errors.check()
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
            print ('/'.join(self.path))
            print (data)
            self.matchpath(data)

    def matchpath(self, data):
        # This method returns True if a handler decides no other parsing is required for
        # the data it handles, for the mode it is in.
        return_value = False
        for key in self.parse_handlers.keys():
            if '/'.join(self.path) == self.parse_handlers[key]['path']:
                return_value = return_value | getattr(self.handler_functions, key)(self.mode, data)
        return return_value

    def debug(self):
        print (yaml.dump(self.root))

class ParseHandlers:

    def __init__(self, output, err):
        self.output = output
        self.errors = err

    def parse_sources(self, mode, sources):
        if mode == ParserStates.Expand:
            for token in sources.keys():
                if (self.errors.check_file(sources[token])):
                    try:
                        sources[token] = yaml.load(open(sources[token], 'r'))
                    except Exception as e:
                        self.err.new_error("YAML parsing error: " + str(e))
        return True

    def parse_initialize(self, mode, data):
        if mode == ParserStates.Validate:
            # Check that each token in Initialize has a defined init function, and that is Cish....
            for token in data:
                
                print ("Validate Initialize")
        if mode == ParserStates.Parse:
            print ("Parsing Initialize")
        return True

    def parse_finalize(self, mode, data):
        print ("finalize")
        return True

    def parse_messages(self, mode, data):
        print ("messages")
        return True

    def parse_include(self, mode, data):
        print ("include")
        return True

    def parse_object(self, mode, data):
        print ("object")
        return True

    def parse_init(self, mode, data):
        print ("init")
        return True

    def parse_final(self, mode, data):
        print ("final")
        return True

    def parse_senders(self, mode, data):
        print ("senders")
        return True

    def parse_receivers(self, mode, data):
        print ("receivers")
        return True

errors = ErrorReporter()
args = ArgumentChecker(errors)

parser = Parser('./cg.conf', errors)
parser.parse(args.miml_file)
parser.debug()


