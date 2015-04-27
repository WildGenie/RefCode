######################################################################
# Automatically generated by qmake (2.01a) Thu Jan 25 08:27:10 2007
######################################################################
CONFIG += uitools qt assistant
TEMPLATE = app

RCC_DIR      = .rcc
MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH += ../include
INCLUDEPATH += ../include

#EPICS lib
EPICS_DIR = /usr/local/epics/base
INCPATH     += $$EPICS_DIR/include $$EPICS_DIR/include/os/Linux
LIBS        += -L$$EPICS_DIR/lib/linux-x86 -lasHost -lcas -ldbStaticHost -lgdd -lrecIoc -lsoftDevIoc -lasIoc -lCom -ldbStaticIoc -liocsh -lregistryIoc -ltestDevIoc -lca -ldbIoc -ldbtoolsIoc -lmiscIoc -lrsrvIoc

#Shared lib
INCPATH += /usr/local/opisrc/include
LIBS += -L/usr/local/opi/lib -lcachedchannelaccess

# Input
SOURCES += CacheChannelRead.cpp
#HEADERS += MainWindow.h

# Output
TARGET = CachedChannelRead 
target.path = /usr/local/opi/bin
INSTALLS += target 
