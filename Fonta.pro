TEMPLATE = subdirs

SUBDIRS += \
    fontadb \
    fonta \
    tools

fontadb.subdir = src/fontadb
fonta.subdir = src/fonta
tools.subdir = src/tools

fonta.depends = fontadb
tools.depends = fontadb

#CONFIG += ordered
