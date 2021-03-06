#           ( DevType,  TaskName, DeviceName(createPhyChan),TriggerChannel, SigType))
createDevice("NI6289", "Dev1", "Dev1", "/Dev1/PFI0", "ai")

#            ( TaskName, ChannelID, InputType, TagName, arg(MDS_Param or SCXI phyChannel))
createChannel("Dev1",     "0",      "STRAIN",  "VVS_AD_21", "SC1Mod1/ai0")
createChannel("Dev1",     "1",      "STRAIN",  "VVS_AD_22", "SC1Mod1/ai1")
createChannel("Dev1",     "2",      "STRAIN",  "VVS_AD_23", "SC1Mod1/ai2")
createChannel("Dev1",     "3",      "STRAIN",  "VVS_AD_24", "SC1Mod1/ai3")
createChannel("Dev1",     "4",      "STRAIN",  "VVS_AD_25", "SC1Mod1/ai4")
createChannel("Dev1",     "5",      "STRAIN",  "VVS_AD_26", "SC1Mod1/ai5")
createChannel("Dev1",     "6",      "STRAIN",  "VVS_AD_27", "SC1Mod1/ai6")
createChannel("Dev1",     "7",      "STRAIN",  "VVS_AD_28", "SC1Mod1/ai7")
createChannel("Dev1",     "8",      "STRAIN",  "VVS_AD_41", "SC1Mod2/ai0")
createChannel("Dev1",     "9",      "STRAIN",  "VVS_AD_42", "SC1Mod2/ai1")
createChannel("Dev1",     "10",      "STRAIN",  "VVS_AD_43","SC1Mod2/ai2")
createChannel("Dev1",     "11",      "STRAIN",  "VVS_AD_44","SC1Mod2/ai3")
createChannel("Dev1",     "12",      "STRAIN",  "VVS_AD_45","SC1Mod2/ai4")
createChannel("Dev1",     "13",      "STRAIN",  "VVS_AD_46","SC1Mod2/ai5")
createChannel("Dev1",     "14",      "STRAIN",  "VVS_AD_47","SC1Mod2/ai6")
createChannel("Dev1",     "15",      "STRAIN",  "VVS_AD_48","SC1Mod2/ai7")
createChannel("Dev1",     "16",      "STRAIN",  "VVS_AD_61","SC1Mod3/ai0")
createChannel("Dev1",     "17",      "STRAIN",  "VVS_AD_62","SC1Mod3/ai1")
createChannel("Dev1",     "18",      "STRAIN",  "VVS_AD_63","SC1Mod3/ai2")
createChannel("Dev1",     "19",      "STRAIN",  "VVS_AD_64","SC1Mod3/ai3")
createChannel("Dev1",     "20",      "STRAIN",  "VVS_AD_65","SC1Mod3/ai4")
createChannel("Dev1",     "21",      "STRAIN",  "VVS_AD_66","SC1Mod3/ai5")
createChannel("Dev1",     "22",      "STRAIN",  "VVS_AD_67","SC1Mod3/ai6")
createChannel("Dev1",     "23",      "STRAIN",  "VVS_AD_68","SC1Mod3/ai7")
createChannel("Dev1",     "24",      "STRAIN",  "VVS_AD_81","SC1Mod4/ai0")
createChannel("Dev1",     "25",      "STRAIN",  "VVS_AD_82","SC1Mod4/ai1")
createChannel("Dev1",     "26",      "STRAIN",  "VVS_AD_83","SC1Mod4/ai2")
createChannel("Dev1",     "27",      "STRAIN",  "VVS_AD_84","SC1Mod4/ai3")
createChannel("Dev1",     "28",      "STRAIN",  "VVS_AD_85","SC1Mod4/ai4")
createChannel("Dev1",     "29",      "STRAIN",  "VVS_AD_86","SC1Mod4/ai5")
createChannel("Dev1",     "30",      "STRAIN",  "VVS_AD_87","SC1Mod4/ai6")
createChannel("Dev1",     "31",      "STRAIN",  "VVS_AD_88","SC1Mod4/ai7")
