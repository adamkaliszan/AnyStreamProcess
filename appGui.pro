TEMPLATE = subdirs

DEFINES +="INC_AGENDA_PERFORMANCE=1"

CONFIG += ordered

SUBDIRS = core \
          gui

gui.depends = core
