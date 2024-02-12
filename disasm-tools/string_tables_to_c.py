#!/usr/bin/env python3

import re

strings = {}

currLabel = None

# pass 1: read strings
with open('asm/code_1.s') as f:
	for line in f:
		m = re.match('glabel (string_\\w+)', line)
		if m:
			currLabel = m.group(1)
		m = re.match('\\s+\\.asciz (".*)', line)
		if m:
			strings[currLabel] = m.group(1)

# pass 2: convert to C
with open('asm/code_1.s') as f:
	for line in f:
		m = re.match('glabel (lbl_\\w+)', line)
		if m:
			print('};\n\nchar *%s[] =\n{' % m.group(1))
		m = re.match('\\s+\\.4byte (string_\\w+)', line)
		if m:
			#print(m.group(1))
			print('    %s,' % strings[m.group(1)])
