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
	
infile.crop((0, 0, 210, 144)).save('serif_'    + infilename)
infile.crop((210, 0, 210+209, 144)).save('sans_'     + infilename)
infile.crop((0, 144, 170, 315)).save('script_'   + infilename)
infile.crop((170, 144, 170+166, 315)).save('display_'  + infilename)
infile.crop((170+166, 144, 419, 315)).save('symbolic_' + infilename)
