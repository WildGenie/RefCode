######################################################################
# Automatically generated by qmake (2.01a) Tue Jan 23 16:13:55 2007
######################################################################

CONFIG += uitools qt
TEMPLATE = app
TARGET = 
DEPENDPATH += ./ ../
INCLUDEPATH += ./ ../

# Input
SOURCES += ../main.cpp ../MainWindow.cpp
HEADERS += MainWindow.h
RESOURCES += ../rmp.qrc

INCPATH     += /usr/local/opisrc/kwt/include /usr/local/opisrc/include
LIBS        += -L/usr/local/opisrc/kwt/lib -lkwt

#EPICS lib
INCPATH     += ./include/ $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux

#QWT
INCPATH		+= /usr/local/qwt/include

# Output
TARGET = RMP
target.path = /usr/local/opi/bin
forms.files = ../ui/*.ui
forms.path = /usr/local/opi/ui
images.files = ../images/*.png
images.path = /usr/local/opi/images
resources.files = ../rmp.qrc
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.ui *.pro
sources.path = ./
# install
INSTALLS += target forms
