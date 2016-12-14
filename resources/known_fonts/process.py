import glob

def format_file(filename):
    fonts = open(filename, 'rb').readlines()

    fonts = list(map(lambda x:x.decode('utf-8').lower(), fonts))
    fonts = list(map(lambda x:' '.join(x.split()), fonts))
    fonts = list(set(fonts))
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
