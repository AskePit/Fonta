import glob
import sys

stylesMap = {
'oldStyle' : 'old_style.dat',
'transitional' : 'transitional.dat',
'modern' : 'modern.dat',
'slab' : 'slab.dat',

'grotesque' : 'grotesque.dat',
'geometric' : 'geometric.dat',
'humanist' : 'humanist.dat',

'script' : 'script.dat',
'decorative' : 'decorative.dat',
'monospaced' : 'monospaced.dat',
}

path = "*.ini"
for fname in glob.glob(path):
    for line in open(fname, 'r').readlines():
        try:
            if '=' in line:
                key, value = line.split('=', 2)

                if key not in stylesMap:
                    continue
                if value == "0\n":
                    continue
                if key == "monospaced" and value == "false\n":
                    continue
                
                with open(stylesMap[key], "a") as outfile:
                    outfile.write("\n")
                    outfile.write(fname)
        except:
            continue
