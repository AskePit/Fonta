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
	
infile.crop((0, 0, 419, 150)).save('grotesque_'    + infilename)
infile.crop((0, 150, 210, 315)).save('geometric_'   + infilename)
infile.crop((210, 150, 419, 315)).save('humanist_' + infilename)
