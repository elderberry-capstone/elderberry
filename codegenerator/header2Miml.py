#!/usr/bin/python

#	Assumptions:
#		- only parses extern functions
#		- init/final functions only detected with "initialize" and "finalize" in name
#		- any non-init/final functions with "initialize" or "finalize" aren't included
#		- only data types recognized are [const|unsigned] [int|char]
#		- variable names not defined are replaced by ARG# variable name

import sys, re

inputfile = sys.argv[1]

# Could check if .h and error if it isn't
rpos = inputfile.rfind(".");
if(rpos <=0):
	basefile = inputfile
else:
	basefile = inputfile[:rpos]

# Should check to see if a .Miml -- if not, add extension or error
outputfile = sys.argv[2]

try:
	f = open(inputfile, 'r')
except IOError as e:
	print "I/O error({0}): Input header file --> {1}".format(e.errno, e.strerror)
	sys.exit(-1)

codeLines = f.readlines()
f.close()

outputCodeHeader = "%YAML 1.2\n---\ninclude: " + inputfile + "\nobject: " + basefile + ".o"
outputCodeInit = "init: "
outputCodeFinal = "final: "
outputCodeSenders = "# Functions that handle outgoing data\nsenders:\n"
outputCodeReceivers = "# Functions that handle incoming data\nreceivers:\n"
outputCodeUnknown = "[unknown:]\n"
foundInit = 0
foundFinal = 0
foundUnknown = 0
foundSenders = 0
foundReceivers = 0

def xstr(s):
    if s is None:
        return ''
    return str(s)

for item in codeLines:
	match = re.match( r'extern[\s]+([\w]+[\s]+)([\w_-]+)[\s]*\((.*)\).*', item.strip('\n'), re.M|re.I)
	if match:
		strpos = match.group(2).find("initialize")
		if(strpos >=0):
			outputCodeInit += str(match.group(2)) + "()"
			foundInit += 1
			continue

		strpos = match.group(2).find("finalize")
		if(strpos >=0):
			outputCodeFinal += str(match.group(2)) + "()"
			foundFinal += 1
			continue

		content = match.group(3).split(',')
		counter = 0

		argVals = ""
		for item2 in content:
			counter += 1
			match2 = re.match( r'[\s]*((const|unsigned)[\s]+)?(int|char)[\s]*(\*)?([\s]*([\w_-]+)[\s]*)?', item2, re.M|re.I)
			if match2:
				argType = xstr(match2.group(1)) + xstr(match2.group(3)) + xstr(match2.group(4))
				argName = match2.group(6)
				if argName is None:
					argName = "ARG" + str(counter)
				
				argVals += "  - [" +argName + ", " + argType.strip() + "]\n"
		
		funcName = str(match.group(2))
		if (funcName[:3]=="get"):
			outputCodeReceivers += funcName  + ":\n" + argVals
			foundReceivers += 1
		elif (funcName[:4]=="send"):
			outputCodeSenders += funcName  + ":\n" + argVals
			foundSenders += 1
		else:
			outputCodeUnknown += funcName  + ":\n" + argVals
			foundUnknown += 1

try:
	fout = open(outputfile, 'w')
except IOError as e:
	print "I/O error({0}): Output Miml file --> {1}".format(e.errno, e.strerror)
	sys.exit(-1)
fout.write(outputCodeHeader + "\n")
fout.write(outputCodeInit + "\n")
fout.write(outputCodeFinal + "\n\n")
fout.write(outputCodeSenders + "\n")
fout.write(outputCodeReceivers + "\n")
if foundUnknown:
	fout.write("# Functions that have not been designated as\n")
	fout.write("# senders or receivers. Sort them accordingly\n")
	fout.write("# and delete the [unknown:] header.\n")
	fout.write(outputCodeUnknown + "\n")
fout.close()

print "\n " + inputfile
print "======================================================"
print " Init\tFinal\tSenders\t  Receivers   Unknown			 "
print "------------------------------------------------------"
print " " + str(foundInit) + "\t" + str(foundFinal) + "\t" + str(foundSenders) + "\t  " + str(foundReceivers) + "\t      " + str(foundUnknown)
print ""
print "Miml file '" + outputfile + "' successfully written.\n"