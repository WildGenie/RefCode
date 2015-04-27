#           ( Type,   TaskName, DeviceName,TriggerChannel, SigType))
createDevice("NI6123", "Dev1", "Dev1", "/Dev1/PFI0", "ai")

#            ( TaskName, ChannelID, InputType, TagName, arg
createChannel("Dev1",     "0",      "ECH",  "ECH_VC","MDS_Param")
createChannel("Dev1",     "1",      "ECH",  "ECH_VB","MDS_Param")
createChannel("Dev1",     "2",      "ECH",  "ECH_IB","MDS_Param")
createChannel("Dev1",     "3",      "ECH",  "ECH_IA","MDS_Param")
createChannel("Dev1",     "4",      "ECH",  "ECH_VFWD1","MDS_Param")
createChannel("Dev1",     "5",      "ECH",  "ECH_VREF1","MDS_Param")
createChannel("Dev1",     "6",      "ECH",  "ECH_VFWD2","MDS_Param")
createChannel("Dev1",     "7",      "ECH",  "ECH_VREF2","MDS_Param")
createChannel("Dev1",     "8",      "ECH",  "ECH_VFWD1C","MDS_Param")
createChannel("Dev1",     "9",      "ECH",  "ECH_VREF1C","MDS_Param")
createChannel("Dev1",     "10",      "ECH",  "ECH_PG1","MDS_Param")
createChannel("Dev1",     "11",      "ECH",  "ECH_PG2","MDS_Param")
createChannel("Dev1",     "12",      "ECH",  "ECH_PG3","MDS_Param")
createChannel("Dev1",     "13",      "ECH",  "ECH_PG4","MDS_Param")
createChannel("Dev1",     "14",      "ECH",  "ECH_PG5","MDS_Param")
createChannel("Dev1",     "15",      "ECH",  "ECH_PG6","MDS_Param")