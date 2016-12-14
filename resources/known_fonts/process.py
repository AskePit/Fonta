def format_file(filename):
    fonts = open(filename, 'rb').readlines()

    fonts = list(map(lambda x:x.decode('utf-8').lower().strip(), fonts))
    fonts = list(map(lambda x:' '.join(x.split()), fonts))
    fonts.sort()

    out = open(filename, 'wb')
    for item in fonts:
        if not item:
            continue

        out.write(item.encode('utf-8'))
        out.write(b'\r\n')
    out.close()

files = [
    'old_style',
    'decorative',
    'script',
    'transitional',
    'modern',
    'sans',
    'serif',
    'slab',
    'monospaced',
    'humanist',
    'grotesque',
    'geometric',
]

for f in files:
    format_file(f + '.dat')
