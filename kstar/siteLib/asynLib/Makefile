#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
#     National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
#     Operator of Los Alamos National Laboratory.
# EPICS BASE Versions 3.13.7
# and higher are distributed subject to a Software License Agreement found
# in file LICENSE that is included with this distribution. 
#*************************************************************************
TOP = ..
include $(TOP)/configure/CONFIG

ASYN = $(TOP)/asynLib
#USR_CFLAGS += -DDEBUG

LIBRARY_IOC += asyn
asyn_LIBS += $(EPICS_BASE_IOC_LIBS)

SRC_DIRS += $(ASYN)/asynDriver
INC += asynDriver.h
DBD += asyn.dbd
asyn_SRCS += asynManager.c
INC += epicsInterruptibleSyscall.h
asyn_SRCS += epicsInterruptibleSyscall.c

SRC_DIRS += $(ASYN)/asynGpib
INC += asynGpibDriver.h
asyn_SRCS += asynGpib.c

SRC_DIRS += $(ASYN)/devGpib
INC += devGpib.h
INC += devSupportGpib.h
INC += devCommonGpib.h
DBD += devGpib.dbd
asyn_SRCS += devCommonGpib.c
asyn_SRCS += devSupportGpib.c

SRC_DIRS += $(ASYN)/vxi11
asyn_SRCS += drvVxi11.c
asyn_SRCS += vxi11core_xdr.c
asyn_SRCS += E5810Reboot.c
asyn_SRCS += E2050Reboot.c
DBD += drvVxi11.dbd

SRC_DIRS += $(ASYN)/drvGenericSerial
asyn_SRCS += drvGenericSerial.c
DBD += drvGenericSerial.dbd

SRC_DIRS += $(ASYN)/drvAsynSerial
INC += drvAsynSerialPort.h
ifeq ($(OS_CLASS), WIN32)
asyn_SRCS += drvAsynSerialPortWin32.c
else
asyn_SRCS += drvAsynSerialPort.c
endif
DBD += drvAsynSerialPort.dbd
INC += drvAsynIPPort.h
asyn_SRCS += drvAsynIPPort.c
asyn_SRCS += drvAsynIPServerPort.c
DBD += drvAsynIPPort.dbd
INC += drvAsynIPServerPort.h

include $(TOP)/configure/RULES
