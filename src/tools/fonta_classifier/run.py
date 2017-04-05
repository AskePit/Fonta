import lxml.html

base_url = 'http://www.1001fonts.com/sans-serif-fonts.html'
page = 1
end_page = 26
items = 50

f = open('out.txt', 'a')

while(page <= end_page):
    doc = lxml.html.parse(base_url+'?page='+str(page)+'&items='+str(items)).getroot()
    fonts = doc.find_class('fontName dimmed')

    for font in fonts:
        text = font.text
        if(text is not None):
            f.write(font.text.encode('utf-8', 'replace')+'\n')

    page = page+1

f.close()

