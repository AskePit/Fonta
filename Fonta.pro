TEMPLATE = subdirs

SUBDIRS += \
    fonta \
    fontadb \
    fonts_cleaner \
    fonta_classifier \
    cogwheel_gen \

fonta.subdir = src/fonta
fontadb.subdir = src/fontadb
fonts_cleaner.subdir = src/tools/fonts_cleaner
fonta_classifier.subdir = src/tools/fonta_classifier
cogwheel_gen.subdir = src/tools/cogwheel_gen

fonta.depends = fontadb
fonta_classifier.depends = fontadb
