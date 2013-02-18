#!/usr/bin/python3

import yaml

parse_handlers = {}
path = ['']

def makepath():
    return '/'.join(path)

def build_router():
    return yaml.load(open('./cg.conf'))

def matchpath(s):
    for key in parse_handlers.keys():
        if makepath() == parse_handlers[key]['path']:
            eval(key)(s)

def parse(s):
    if type(s).__name__=='dict':
        for key in s.keys():
            path.append(key)
            matchpath(s)
            parse (s[key])
            path.pop()
    elif type(s).__name__=='list':
        for element in s:
            matchpath(s)
            parse (element)            
    else:
        matchpath(s)
    #    print (makepath(), s)

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

parse_handlers = build_router()
parse(yaml.load(open('./test.miml')))
