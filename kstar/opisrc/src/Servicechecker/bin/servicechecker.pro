######################################################################
# Automatically generated by qmake (2.01a) Thu Jan 25 08:27:10 2007
######################################################################
CONFIG += uitools qt
TEMPLATE = app
MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ../include/
INCLUDEPATH += ../include/

#EPICS lib
INCPATH     += ./include/ $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux
LIBS        += -L$(EPICS_BASE)/lib/linux-x86 -lasHost -lcas -ldbStaticHost -lgdd -lrecIoc -lsoftDevIoc -lasIoc -lCom -ldbStaticIoc -liocsh -lregistryIoc -ltestDevIoc -lca -ldbIoc -ldbtoolsIoc -lmiscIoc -lrsrvIoc

#XML-RPC
INCPATH		+= /usr/local/include/xmlrpc-c
INCPATH		+= /usr/local/include/xercesc
LIBS        += -L/usr/local/lib -lxerces-c -lxmlrpc -lxmlrpc++ -lxmlrpc_abyss -lxmlrpc_client -lxmlrpc_client++ -lxmlrpc_cpp -lxmlrpc_server -lxmlrpc_server++ -lxmlrpc_server_abyss -lxmlrpc_server_abyss++ -lxmlrpc_server_cgi -lxmlrpc_util -lxmlrpc_xmlparse -lxmlrpc_xmltok

# Input
SOURCES += ../main.cpp ../servicechecker.cpp
HEADERS += servicechecker.h

# Output
TARGET = servicechecker 
target.path = /usr/local/opi/bin
forms.files = ../ui/*.ui
forms.path = /usr/local/opi/ui/
INSTALLS += target forms
