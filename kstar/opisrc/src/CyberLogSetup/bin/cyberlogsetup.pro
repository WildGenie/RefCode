######################################################################
# Automatically generated by qmake (2.01a) Thu Jan 25 08:27:10 2007
######################################################################
CONFIG += uitools qt
TEMPLATE = app
MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ../include/
INCLUDEPATH += ../include/

#mysql
INCPATH += /usr/include/mysql

#mysql++ lib
INCPATH += /usr/local/include/mysql++
LIBS += -L/usr/local/lib -lmysqlpp

#accesskstardb lib
#INCPATH += ../../../include/
INCPATH += /usr/local/opisrc/include
LIBS += -L/usr/local/opi/lib -laccesskstardb

#QWT
INCPATH		+= /usr/local/qwt/include
LIBS        += -L/usr/local/qwt/lib -lqwt

# Input
SOURCES += ../main.cpp ../logsetup.cpp ../logsavethr.cpp
HEADERS += cyberlog.h logsetup.h logsavethr.h

# Output
TARGET = CyberLogSetup
target.path = /usr/local/opi/bin
forms.files = ../ui/*.ui
forms.path = /usr/local/opi/ui
images.files = ../images/*.png
images.files += ../images/*.xpm
images.path = /usr/local/opi/images
INSTALLS += target forms images
