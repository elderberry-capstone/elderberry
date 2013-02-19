#!/usr/bin/python3

import yaml

output_document = []

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

    def parse(self, s):
        if type(s).__name__=='dict':
            for key in s.keys():
                self.path.append(key)
                if self.matchpath(s[key]) == False:
                    self.parse (s[key])
                self.path.pop()
        elif type(s).__name__=='list':
            for element in s:
                if self.matchpath(element) == False:
                    self.parse (element)            
        else:
            self.matchpath(s)

    def matchpath(self, s):
        for key in self.parse_handlers.keys():
            if self.path.getPath() == self.parse_handlers[key]['path']:
                eval(key)(s)
                return True
        return False

def parse_error(s):
    print ("Func parse_error", s)
    
def parse_finalize(s):
    print ("Func parse_finalize", s)

def parse_functions(s):
    print ("Func parse_functions", s)

def parse_includes(s):
    print ("Func parse_includes", s)

def parse_initialize(s):
    print ("Func parse_initialize", s)

def parse_params(s):
    print ("Func parse_params", s)

parser = ParseRouter('./cg.conf')
parser.parse(yaml.load(open('./test.miml')))
