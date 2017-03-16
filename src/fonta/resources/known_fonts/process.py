import glob

to_trunc = ['std', 'pro', 'normal', 'regular', 'italic', 'bold', 'font', 'demo', 'condenced']

def trunc(s):
    for sub in to_trunc:
        if s.endswith(' ' + sub):
            s = s[:len(s) - (len(sub) + 1)]
    return s

def format_file(filename):
    fonts = open(filename, 'rb').readlines()

    fonts[:] = [x.decode('utf-8').lower() for x in fonts]
    fonts[:] = [' '.join(x.split()) for x in fonts]
    fonts[:] = [trunc(x) for x in fonts]
    fonts[:] = list(set(fonts))
    fonts.sort()

    out = open(filename, 'wb')
    for item in fonts:
        if not item:
            continue

        out.write(item.encode('utf-8'))
        out.write(b'\r\n')
    out.close()

for f in glob.glob('*.dat'):
    format_file(f)
