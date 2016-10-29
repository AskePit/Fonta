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
	
infile.crop((0, 0, 210, 148)).save('cove_'    + infilename)
infile.crop((210, 0, 210+209, 148)).save('square_'     + infilename)
infile.crop((0, 148, 140, 315)).save('bone_'   + infilename)
infile.crop((140, 148, 140+143, 315)).save('asymmetric_'  + infilename)
infile.crop((140+143, 148, 419, 315)).save('triangle_' + infilename)
