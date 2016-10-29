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
	
infile.crop((0, 0, 210, 156)).save('oldstyle_'    + infilename)
infile.crop((210, 0, 210+209, 156)).save('transitional_'     + infilename)
infile.crop((0, 156, 210, 315)).save('modern_'   + infilename)
infile.crop((210, 156, 419, 315)).save('slab_' + infilename)
