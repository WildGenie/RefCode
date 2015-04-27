#           ( DevType,  TaskName, DeviceName(createPhyChan),TriggerChannel, SigType, ClockChannel))
createDevice("NI6259", "Dev1", "Dev1", "/Dev1/PFI0", "ai", "/Dev1/PFI12")

#            ( TaskName, ChannelID, InputType, TagName, arg(MDS_Param or SCXI phyChannel))
createChannel("Dev1",     "0",      "FUEL",  "B_PKR251A",	"Dev1/ai0")
createChannel("Dev1",     "1",      "FUEL",  "B_IMR265", 	"Dev1/ai1")
createChannel("Dev1",     "2",      "FUEL",  "F_IMR265", 	"Dev1/ai2")
createChannel("Dev1",     "3",      "FUEL",  "B_IG307", 	"Dev1/ai3")
createChannel("Dev1",     "4",      "FUEL",  "K_GFLOW_IN", 	"Dev1/ai4")
createChannel("Dev1",     "5",      "FUEL",  "K_GFLOW_OUT", 	"Dev1/ai5")
createChannel("Dev1",     "6",      "FUEL",  "K_VALVE_DRI", 	"Dev1/ai6")
createChannel("Dev1",     "7",      "FUEL",  "I_GFLOW_IN", 	"Dev1/ai7")
createChannel("Dev1",     "8",      "FUEL",  "I_GFLOW_OUT", 	"Dev1/ai16")
createChannel("Dev1",     "9",      "FUEL",  "I_VALVE_DRI", 	"Dev1/ai17")
createChannel("Dev1",     "10",     "FUEL",  "K_HGFLOW_IN",	"Dev1/ai18")
createChannel("Dev1",     "11",     "FUEL",  "K_HGFLOW_OUT",	"Dev1/ai19")
createChannel("Dev1",     "12",     "FUEL",  "K_HVALVE_DRI",	"Dev1/ai20")
createChannel("Dev1",     "13",     "FUEL",  "G_GFLOW_IN",	"Dev1/ai21")
createChannel("Dev1",     "14",     "FUEL",  "G_GFLOW_OUT",	"Dev1/ai22")
createChannel("Dev1",     "15",     "FUEL",  "G_VALVE_DRI",	"Dev1/ai23")
