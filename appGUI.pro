TEMPLATE = subdirs

DEFINES +="INC_AGENDA_PERFORMANCE=1"

CONFIG += ordered

SUBDIRS = core \
          gui

QMAKE_CXXFLAGS += -I/usr/include/python3.8 -I/usr/include/python3.8  -Wno-unused-result -Wsign-compare -g -fdebug-prefix-map=/build/python3.8-xhc1jt/python3.8-3.8.5=. -specs=/usr/share/dpkg/no-pie-compile.specs -fstack-protector -Wformat -Werror=format-security  -DNDEBUG -g -fwrapv -O3 -Wall

LIBS += -lpython3.8
LIBS += -L/usr/lib/python3.8/config-3.8-x86_64-linux-gnu -L/usr/lib -lcrypt -lpthread -ldl  -lutil -lm -lm

INCLUDEPATH += /usr/include/python3.8/

gui.depends = core

