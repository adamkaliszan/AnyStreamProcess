TEMPLATE = subdirs

DEFINES +="INC_AGENDA_PERFORMANCE=1"

CONFIG += ordered

SUBDIRS = core \
          console \
          gui

gui.depends = core
console.depends = core

