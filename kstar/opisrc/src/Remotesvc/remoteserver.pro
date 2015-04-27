######################################################################
# Automatically generated by qmake (2.01a) Thu Jan 25 08:27:10 2007
######################################################################
TEMPLATE = app

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
#DEPENDPATH  += include/
#INCLUDEPATH += include/

#EPICS lib
INCPATH     += ./include/ $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux
LIBS        += -L$(EPICS_BASE)/lib/linux-x86 -lasHost -lcas -ldbStaticHost -lgdd -lrecIoc -lsoftDevIoc -lasIoc -lCom -ldbStaticIoc -liocsh -lregistryIoc -ltestDevIoc -lca -ldbIoc -ldbtoolsIoc -lmiscIoc -lrsrvIoc

#XML-RPC
INCPATH		+= /usr/local/include/xmlrpc-c
#INCPATH		+= /usr/local/include/xercesc
#LIBS        += -L/usr/local/lib -lxerces-c -lxmlrpc -lxmlrpc++ -lxmlrpc_abyss -lxmlrpc_client -lxmlrpc_client++ -lxmlrpc_cpp -lxmlrpc_server -lxmlrpc_server++ -lxmlrpc_server_abyss -lxmlrpc_server_abyss++ -lxmlrpc_server_cgi -lxmlrpc_util -lxmlrpc_xmlparse -lxmlrpc_xmltok
LIBS        += -L/usr/local/lib -lxmlrpc -lxmlrpc++ -lxmlrpc_abyss -lxmlrpc_client -lxmlrpc_client++ -lxmlrpc_cpp -lxmlrpc_server -lxmlrpc_server++ -lxmlrpc_server_abyss -lxmlrpc_server_abyss++ -lxmlrpc_server_cgi -lxmlrpc_util -lxmlrpc_xmlparse -lxmlrpc_xmltok

#
#INCPATH		+= /usr/local/opisrc/include
#LIBS        += -L/usr/local/opi/lib -lvalueToStr

# Input
SOURCES += remoteserver.cpp

# Output
TARGET = remotesvr 
target.path = /usr/local/opi/bin
INSTALLS += target
