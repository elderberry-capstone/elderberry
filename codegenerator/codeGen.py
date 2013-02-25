#!/usr/bin/python3

import sys
import re
import yaml
from os import path, access, R_OK
from collections import defaultdict

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

    def display(self):
        if self.has_errors():
            print (len(self.errors), " error(s) encountered!")
            for error in self.errors:
                print (error)
        if self.has_warnings():
            print (len(self.warnings), " warning(s) encountered!")
            for warning in self.warnings:
                print (warning)

class OutputGenerator:

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
        pass

class ArgumentChecker:

    def __init__(self, err):
        if len(sys.argv) > 3 or len(sys.argv) < 2:
            err.new_error(
              "Illegal number of arguments! Expected 1 or 2, received: ",
              len(sys.argv) -1)
        else:
            self.modes = {'code': False, 'make': False, 'header': False}
            if len(sys.argv) == 2:
                self.set_modes("-cmh", err)
            else:
                self.set_modes(sys.argv[1], err)
            self.check_config(sys.argv[len(sys.argv) - 1], err)
            
    def set_modes(self, modes, err):
        match = re.match(r"^-[cmh]+$", modes)
        if match:
            if re.match(r"(?=.*c)", modes):
                self.modes['code'] = True
            if re.match(r"(?=.*m)", modes):
                self.modes['make'] = True
            if re.match(r"(?=.*h)", modes):
                self.modes['header'] = True
        else:
            err.new_error("Illegal mode usage, expecting -[chm]. Given : " + modes)

    def check_config(self, filename, err):
        if path.isfile(filename) == False:
            err.new_error("Main MIML file: '" + filename + "' cannot be found!")
        elif access(filename, R_OK) == False:
            err.new_error("Main MIML file: '" + filename + "' cannot be opened for reading!")

class ParsePath:

    def __init__(self):
        self.path = ['']

    def append(self, token):
        self.path.append(token)

    def pop(self):
        self.path.pop()

    def getPath(self):
        return '/'.join(self.path)

class ParseRouter:

    def __init__(self, filename):
        self.parse_handlers = yaml.load(open(filename))
        self.path = ParsePath()
        self.output = OutputGenerator()

    def parse(self, data):
        if type(data).__name__=='dict':
            for key in data.keys():
                self.path.append(key)
                if self.matchpath(data[key]) == False:
                    self.parse (data[key])
                self.path.pop()
        elif type(data).__name__=='list':
            for element in data:
                if self.matchpath(element) == False:
                    self.parse (element)            
        else:
            self.matchpath(data)

    def matchpath(self, data):
        for key in self.parse_handlers.keys():
            if self.path.getPath() == self.parse_handlers[key]['path']:
                eval('self.output.' + key)(data)
                return True
        return False


errors = ErrorReporter()
args = ArgumentChecker(errors)

if errors.has_errors() == True:
    errors.display()
    sys.exit(0)
