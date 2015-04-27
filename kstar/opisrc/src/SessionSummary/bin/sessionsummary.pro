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

#MDSPlus lib
MDSPLUSHOME = /usr/local/mdsplus
INCPATH     += $$MDSPLUSHOME/include
LIBS        += -L$$MDSPLUSHOME/lib -lMdsLib

#EPICS lib
INCPATH     += ./include/ $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux
LIBS        += -L$(EPICS_BASE)/lib/linux-x86 -lasHost -lcas -ldbStaticHost -lgdd -lrecIoc -lsoftDevIoc -lasIoc

#accesskstardb lib
#INCPATH += ../../../include/
INCPATH += /usr/local/opisrc/include
LIBS += -L/usr/local/opi/lib -laccesskstardb


# Input
SOURCES += ../main.cpp ../mdsplusdata.cpp ../sessionsummary.cpp ../sessionsummarythr.cpp ../bestshotlist.cpp  ../shotresult.cpp
HEADERS += sessionsummary.h mdsplusdata.h sessionsummarythr.h bestshotlist.h shotresult.h

# Output
TARGET = SessionSummary
target.path = /usr/local/opi/bin
forms.files = ../ui/*.ui
forms.path = /usr/local/opi/ui
images.files = ../images/*.png
images.files += ../images/*.xpm
images.path = /usr/local/opi/images
INSTALLS += target forms images
