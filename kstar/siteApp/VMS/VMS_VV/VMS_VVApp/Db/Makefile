TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this


DB += dbVVPCRP.vdb
DB += dbLoadVVPCRP.template

DB += dbVVPGP307.vdb
DB += dbLoadVVPGP307.template

DB += dbVVPMBP.vdb
DB += dbLoadVVPMBP.template

DB += dbVVPTMP.vdb
DB += dbLoadVVPTMP.template

DB += dbVVPTPG262.vdb
DB += dbLoadVVPTPG262.template

DB += dbSysMon.db

DB += VMS_VV_BI.db
DB += VMS_VV_BI_READ.template

DB += VMS_VV_BO_SW.db
DB += VMS_VV_BO_SW_WRITE.template

DB += CRP_Rxx.db
DB += VMS_VV_CRP_Rxx.template

DB += VMS_VV_GAUGE_OUT.db
DB += VMS_VV_GAUGE_OUT.template

DB += VMS_VV_HEART.db

DB += VMS_VV_HTR_AI.db
DB += VMS_VV_HTR_AI.template
DB += VMS_VV_HTR_AO.db
DB += VMS_VV_HTR_AO.template
DB += VMS_VV_HTR_BI.db
DB += VMS_VV_HTR_BI.template
DB += VMS_VV_HTR_BO.db
DB += VMS_VV_HTR_BO.template

DB += VMS_VV_FL.db
DB += VMS_VV_INIT.db
DB += VMS_VV_READ.db
DB += VMS_VV_READ.template

DB += TMP_RSS.db
DB += VMS_VV_TMP_RSS.template


#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

