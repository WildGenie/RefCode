#!../../bin/win32-x86/IRTV

< envPaths
< sfwEnv

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/IRTV.dbd"
IRTV_registerRecordDeviceDriver pdbbase

# use this after registerRecordDeviceDriver
< ${TOP}/iocBoot/${IOC}/IrtvEnv

## printing debug messages to console
setPrintLevel 3

# logging debug messages to file
setLogLevel 2
setLogFile("IRTV_D1.log")

# sleep times of DAQ task
var kDAQSleepMillis 		1000

# Create main task
createMainTask("IRTV_D1")

######################################################################
##           DevType,  TaskName,  DeviceName, TriggerChannel, SigType
######################################################################
createDevice("IRTV_D1", "IRTV_D1", "IRTV_D1", "",   "")

## Load record instances
dbLoadRecords("db/IRTV_Preset.db","SYS=IRTV_D1, PRESET=0")
dbLoadRecords("db/IRTV_Preset.db","SYS=IRTV_D1, PRESET=1")
dbLoadRecords("db/IRTV_Preset.db","SYS=IRTV_D1, PRESET=2")
dbLoadRecords("db/IRTV_Preset.db","SYS=IRTV_D1, PRESET=3")

dbLoadRecords("db/IRTV.db","SYS=IRTV_D1, LSYS=NMS, LPORT=P2")
dbLoadRecords("db/dbSFW.db","SYS=IRTV_D1")
dbLoadRecords("db/dbSubNet.db", "SYS=IRTV_D1")

cd ${TOP}/iocBoot/${IOC}
iocInit

#####################################
# IRMIS crawler
dbl > pvlist

cd ${TOP}/pvcrawler
system("pvcrawler.bat")
cd ${TOP}/iocBoot/${IOC}
#####################################

# Initial values
dbpf IRTV_D1_OP_MODE 2
dbpf IRTV_D1_AUTO_CREATE_LOCAL_TREE	1
dbpf IRTV_D1_CAM_MODEL_TYPE 1

# End of st.cmd