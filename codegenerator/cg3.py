#!/usr/bin/python3

import yaml
from collections import defaultdict

class ErrorReporter:
    def __init__(self):
        self.errors = []

    def new_error(self, message):
        self.errors.append[message]

    def append_error(self, message):
        self.errors.append(self.errors.pop() + message)

    def display(self):
        return self.errors


class OutputGenerator:

    def __init__(self):
        self.output = defaultdict(list)

    def append(self, level, data):
        self.output[level].append(data)

    def getOut(self):
        print (self.output)

    def parse_error(self, data):
        print ("Func parse_error:", data, "\n")
    
    def parse_finalize(self, data):
        print ("Func parse_finalize:", data, "\n")

    def parse_functions(self, data):
        print ("Func parse_functions:", data, "\n")

    def parse_includes(self, data):
        print ("Func parse_includes:", data, "\n")

    def parse_initialize(self, data):
        print ("Func parse_initialize:", data, "\n")

    def parse_params(self, data):
        print ("Func parse_params:", data, "\n")


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

parser = ParseRouter('./cg.conf')
parser.parse(yaml.load(open('./test.miml')))
parser.output.getOut()
