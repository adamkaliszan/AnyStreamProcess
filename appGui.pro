TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = core \
          gui

gui.depends = core
