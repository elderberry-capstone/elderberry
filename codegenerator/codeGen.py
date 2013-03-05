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
        self.code_filename = "fcfmain.c"
        self.header_filename = "fcfmain.h" 
        self.make_filename = "miml.mk"

    def append(self, mode, level, data):
        self.output[mode][level].append(data)

    def display(self):
        for mode in self.output.keys():
            if self.mode_flags[mode] == True:
                print (mode + ":\n")
                for level in sorted(self.output[mode].keys()):
                    for message in self.output[mode][level]:
                    #    print (mode, "->", level, "->", message)
                        print (message)
                    print ("\n")

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
#            print (yaml.dump(self.master))
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
        # This function can be cleaned up a bit, its unfolded to let me figure out what should be same/different atm.
	# check for erros thrown during last phase.
        self.errors.check()
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
	    # Necessary to check unhandled from Expand or we will miss problems during buffer transfer
            if not self.unhandled == {}:
                self.errors.new_error("Unhandled MIML content at end of Expand state! " + str(self.unhandled))
            self.unhandled = copy.copy(self.master)
            self.state = ParserStates.Validate
        elif self.state == ParserStates.Validate:
            if not self.unhandled == {}:
                self.errors.new_error("Unhandled MIML content at end of Validate state! " + str(self.unhandled))
            # Make buffer contents master. 
            self.master = self.buffer
#            self.buffer = {}
            self.state = ParserStates.Parse
        else:
            # purge staged data.
            self.handler_functions.purge()
            return_value = False
	    # Check for errors thrown during transition
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
                    return_value = return_value | self.call_handler_function(key, data)
            if len(self.parse_handlers[key]['path']) == 1 and self.parse_handlers[key]['path'][0] == self.path[-1]:       
                return_value = return_value | self.call_handler_function(key, data)
        return return_value

    def call_handler_function(self, key, data):
        # NOTE: This is where the handler functions get called!!!
        if not type(data).__name__ == self.parse_handlers[key]['type']:
            self.errors.new_error("Handler type mismatch. " + key + " expects " + 
                self.handler_functions[key]['type'] + " received " + type(data).__name__)
            return False
        else:
            return getattr(self.handler_functions, key)(data)

    def argument_check(self):

        if len(sys.argv) > 3 or len(sys.argv) < 2:
            self.errors.new_error("Illegal number of arguments! Expected 1 or 2, received: "
            + str(len(sys.argv) -1))
            return False
        self.modes = {'code': False, 'make': False, 'header': False}
        self.miml_file = sys.argv[len(sys.argv) - 1]
        mode_flags = "-cmh" if len(sys.argv) == 2 else sys.argv[1]
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
        # dumps master, useful if you are messing with handler functions and can't find your way.
        print (yaml.dump(self.master))

class ParseHandlers:

    def __init__(self, parser):
        self.parser = parser
        # to support validate_params
        self.allowed_types = ['int', 'char*', 'int32_t']
        self.objects = []

    def purge(self):
        # Called after Parsing phase, allows handlers to stage data and then commit to OutputGenerator after parse stage.
        if len(self.objects) > 0:
            self.parser.output.append("make", 5, "OBJECTS += " + ', '.join(self.objects))

    def parse_sources(self, sources):
        if self.parser.state == ParserStates.Expand:
            # Pull in external file data, place in buffer
            del(self.parser.unhandled['sources'])
            self.parser.buffer['modules'] = {}
            for source in sources:             
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
        if self.parser.state == ParserStates.Validate:
            for message in data.keys():
                sender = message.split('.')
                if not len(sender) == 2:
                    self.parser.errors.new_error("Illegal Sender syntax: " + message)
                elif not sender[0] in self.parser.master['modules']:
                    self.parser.errors.new_error("Sending source " + sender[0] + " not loaded as module.")
                elif not sender[1] in self.parser.master['modules'][sender[0]]['senders']:
                    self.parser.errors.new_error("Sending message " + sender[1] + " not defined as sender for " + sender[0])
                else:
                    sender_params = self.parser.master['modules'][sender[0]]['senders'][sender[1]]
                    for rec in data[message]:
                        receiver = rec.split('.')
                        if not len(sender) == 2:
                            self.parser.errors.new_error("Illegal Receiver syntax: " + rec + " for message " + message)
                        elif not receiver[0] in self.parser.master['modules']:
                            self.parser.errors.new_error("Receiver: " + receiver[0] + " not loaded as module.")
                        elif not receiver[1] in self.parser.master['modules'][receiver[0]]['receivers']:
                            self.parser.errors.new_error("Receiving message " + receiver[1] + " not defined as receiver for " + receiver[0])
                        elif not len(sender_params) == len(self.parser.master['modules'][receiver[0]]['receivers'][receiver[1]]):
                            self.parser.errors.new_error("Message " + sender + " cannot send to receiver " + rec +
                            ". Number of arguments must be the same in both functions.")
                        else:
                            pos = 0
                            for param in sender_params:
                                if not param[1] == self.parser.master['modules'][receiver[0]]['receivers'][receiver[1]][pos][1]:
                                    self.parser.errors.new_error("Message " + message + " cannot send to receiver " + 
                                    rec + ". Type mismatch on argument " + str(pos + 1))
                                pos += 1
            del(self.parser.unhandled['messages'])
            self.parser.buffer['messages'] = data
            return True

        if self.parser.state == ParserStates.Parse:
            for message in data.keys(): # for each message
                (src, func) = message.split('.')
                args = []
                params = []
                types = []
                for caller_param in self.parser.master['modules'][src]['senders'][func]: # for each param in caller
                    args.append(caller_param[1] + " " + caller_param[0])
                    params.append(caller_param[0])
                    types.append(caller_param[1])
                self.parser.output.append("header", 10, "void " + func + "(" + ', '.join(types) + ');')
                self.parser.output.append("code", 20, "void " + func + "(" + ', '.join(args) + ') {')                    
                for receivers in data[message]: # for each receiver
                    (rsrc, rfunc) = receivers.split('.')
                    self.parser.output.append("code", 20, "    " + rfunc + "(" + ', '.join(params) + ');')
                self.parser.output.append("code", 20, "}\n")
            return True
        return False

    def parse_modules(self, data):
        # Must return False or other module matches will not happen.
        if self.parser.state == ParserStates.Validate:
            for source in data.keys():
                for key in data[source]:
                    if not key in ('include', 'object', 'init', 'final', 'senders', 'receivers'):
                        self.parser.errors.new_error("Module: " + source + " contains illegal component: " + key)
            del(self.parser.unhandled['modules'])
            self.parser.buffer['modules'] = data
        elif self.parser.state == ParserStates.Parse:
            # Initialize/Finalize Functions
            finals = []
            self.parser.output.append("code", 10, "void fcf_initialize() {")
            for source in data.keys():
                if "init" in data[source]:
                    self.parser.output.append("code", 10, "    " + data[source]['init'])
                if "final" in data[source]:
                    finals.append(data[source]['final'])
            self.parser.output.append("code", 10, "}")
            self.parser.output.append("code", 15, "void fcf_finalize() {")
            finals.reverse()
            for final in finals:
                self.parser.output.append("code", 15, "    " + final)  
            self.parser.output.append("code", 15, "}")
        else:
            self.parser.buffer['modules'] = data
        return False

    def parse_includes(self, data):
        if self.parser.state == ParserStates.Validate:
            if not re.match(r"\w+\.h", data):
                self.parser.errors.new_error("Illegal header file format: " + data + " in " + '/'.join(self.parser.path))
        elif self.parser.state == ParserStates.Parse:
            self.parser.output.append("code", 5, "#include \"" + data + "\"")
        return True

    def parse_objects(self, data):
        if self.parser.state == ParserStates.Validate:
            if not re.match(r"\w+\.o", data):
                self.parser.errors.new_error("Illegal object file format: " + data + " in " + '/'.join(self.parser.path))
        if self.parser.state == ParserStates.Parse:
            self.objects.append(data)
        return True

    def parse_inits(self, data):
        if self.parser.state == ParserStates.Validate:
            if not re.match(r"\w+\([^)]*\);", data):
                self.parser.errors.new_error("Illegal initialize function: " + data + " in " + '/'.join(self.parser.path))
        return True
    
    def parse_finals(self, data):
        if self.parser.state == ParserStates.Validate:
            if not re.match(r"\w+\([^)]*\);", data):
                self.parser.errors.new_error("Illegal finalize function: " + data + " in " + '/'.join(self.parser.path))
        return True

    def validate_senders(self, data):
        return self.validate_params(data)

    def validate_receivers(self, data):
        return self.validate_params(data)

    def validate_params(self, data):
        if self.parser.state == ParserStates.Validate:
            for param in data:
                if not len(param) == 2:
                    self.parser.errors.new_error("Illegal parameter definition: " + str(param) + " in " + '/'.join(self.parser.path))
                if not param[1] in self.allowed_types:
                    self.parser.errors.new_error("Illegal parameter type: " + str(param[1]) + " in " + '/'.join(self.parser.path))
        return True
            

parser = Parser('./cg.conf')
parser.parse()
# print ("Output:")
# parser.debug()
