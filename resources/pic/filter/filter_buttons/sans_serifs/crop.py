#!/usr/bin/python

import sys
from PIL import Image

if len(sys.argv) != 2:
	print ('Usage: test.py infile')
	sys.exit()
	
infilename = sys.argv[1]
infile = Image.open(infilename)
infile = infile.crop((0, 0, 419, 315))
infile.save(infilename)
	
infile.crop((0, 0, 419, 150)).save('normal_'    + infilename)
infile.crop((0, 150, 210, 315)).save('rounded_'   + infilename)
infile.crop((210, 150, 419, 315)).save('flarred_' + infilename)
