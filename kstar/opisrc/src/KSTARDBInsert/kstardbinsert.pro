######################################################################
# Automatically generated by qmake (2.01a) Tue Jan 23 16:13:55 2007
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

#mysql
INCPATH += /usr/include/mysql

#mysql++ lib 
INCPATH += /usr/local/include/mysql++
LIBS += -L/usr/local/lib -lmysqlpp

#accesskstardb lib 
INCPATH += /usr/local/opisrc/include/
LIBS += -L/usr/local/opi/lib -laccesskstardb

# Input
SOURCES += main.cpp

target.path = /usr/local/opi/bin 
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
sources.path = ./
# install
INSTALLS += target