#!/usr/bin/python3

import yaml

path = ['root']

def parse(s):
    if type(s).__name__=='dict':
#        print ('Its dict!', s, '\n\n')
        for key in s.keys():
#            print ("key: ", key)
            path.append(key)
#            print (path)
            parse (s[key])
            path.pop()
    elif type(s).__name__=='list':
#        print ('Its list!', s, '\n\n')
        for element in s:
#            path.append('element')
#            print (path, element)
            parse (element)
#            path.pop()
            
    else:
#        path.append(s)
        print (path, s)
#        print ('Its a scalar?', s, '\n\n')
#        path.pop()

rules = yaml.load(open('./cg.conf'))
struct = yaml.load(open('./test.miml'))
parse (struct)
print (yaml.dump(rules))
