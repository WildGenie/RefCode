/******************************************************************************
 *  Copyright (c) 2007 ~ by NFRI, Woong. All Rights Reserved.                     *
 ******************************************************************************/


/*==============================================================================
                        EDIT HISTORY FOR MODULE


2007. 10. 16.
add "ioc_debug" routine


2007. 10. 22.
add "multi-trigger" routine


2007. 10. 25
change working flow for port config....


2007. 11. 5
add  " set calibration test "


2007. 11. 6
add printf for show offset value
add function " get LTU ID number  "
remove "sys/time.h"


2007. 11. 15
add function " devCLTU_AO_SET_CAL_VALUE() "


2007. 11. 20
changed in Func devCLTU_AO_mTrig()....
-	ptaskConfig->mPortConfig[pParam->nArg0].nTrigMode = xxxxxx;
+	ptaskConfig->mPortConfig[0].nTrigMode = xxxxxx;

2007. 11. 26
+ drvCLTU_set_rtc() call function.


2007. 12. 20
modify for mix cltu m6802

2009. 11. 8
+ stringin board time

2009. 11. 10
source architecture changed...

2013. 3. 19
FPGA major updated.


*/

/*
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "alarm.h"
#include "callback.h"
#include "cvtTable.h"
#include "dbDefs.h"
#include "dbAccess.h"
#include "recGbl.h"
#include "recSup.h"
#include "devSup.h"
#include "link.h"
#include "dbCommon.h"
*/

#include "dbAccess.h"  /*#include "dbAccessDefs.h" --> S_db_badField */

#include "boRecord.h"
#include "aiRecord.h"
#include "aoRecord.h"
#include "stringinRecord.h"

#include "epicsExport.h"


#include "drvCLTU.h"

#include "pvList.h"
#include "cltu_Def.h"

#include "oz_xtu2.h"
#include "oz_cltu.h"


#define WITH_TIMINGBOARD	1

/*
typedef struct
{
    IOSCANPVT      ioscanpvt;
} timeSCANst;
*/

void devCLTU_AO_ShotTerm(execParam *pParam);
void devCLTU_setup(execParam *pParam);
void devCLTU_set_shotStart(execParam *pParam);
void devCLTU_set_clockOnShot(execParam *pParam);
void devCLTU_AO_calibration(execParam *pParam);
void devCLTU_AO_SET_CAL_VALUE(execParam *pParam);

void devCLTU_AO_Trig(execParam *pParam);
void devCLTU_AO_Clock(execParam *pParam);
void devCLTU_AO_T0(execParam *pParam);
void devCLTU_AO_T1(execParam *pParam);
void devCLTU_AO_mTrig(execParam *pParam);
void devCLTU_AO_mClock(execParam *pParam);
void devCLTU_AO_mT0(execParam *pParam);
void devCLTU_AO_mT1(execParam *pParam);

typedef struct {
	char taskName[60];
	char arg1[60];
	char arg2[60];
	char arg3[60];
	char recordName[80];

	ST_drvCLTU *pCfg;

	unsigned	ind;
	int  nArg0;

} devCLTUdpvt;

typedef struct {
	char taskName[60];
	char arg1[60];
	char arg2[60];
	char arg3[60];
	char recordName[80];

	ST_drvCLTU *pCfg;

	unsigned	ind;
} devAiCLTURawReaddpvt;


LOCAL long devBoCLTUControl_init_record(boRecord *precord);
LOCAL long devBoCLTUControl_write_bo(boRecord *precord);

LOCAL long devAoCLTUControl_init_record(aoRecord *precord);
LOCAL long devAoCLTUControl_write_ao(aoRecord *precord);

LOCAL long devAiCLTURawRead_init_record(aiRecord *precord);
LOCAL long devAiCLTURawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devAiCLTURawRead_read_ai(aiRecord *precord);


LOCAL long devStringinCLTU_init_record(stringinRecord *precord);
LOCAL long devStringinCLTU_get_ioint_info(int cmd, stringinRecord *precord, IOSCANPVT *ioScanPvt);
LOCAL long devStringinCLTU_read_stringin(stringinRecord *precord);


struct {
    long     number;
    DEVSUPFUN    report;
    DEVSUPFUN    init;
    DEVSUPFUN    init_record;
    DEVSUPFUN    get_ioint_info;
    DEVSUPFUN    write_bo;
} devBoCLTUControl = {
    5,
    NULL,
    NULL,
    devBoCLTUControl_init_record,
    NULL,
    devBoCLTUControl_write_bo
};
epicsExportAddress(dset, devBoCLTUControl);


struct {
	long		number;
	DEVSUPFUN	dev_report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_stringin;
} devStringinCLTU = {
	6,
	NULL,
	NULL,
	devStringinCLTU_init_record,
	devStringinCLTU_get_ioint_info,
	devStringinCLTU_read_stringin
};
epicsExportAddress(dset, devStringinCLTU);



struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
} devAoCLTUControl = {
	6,
	NULL,
	NULL,
	devAoCLTUControl_init_record,
	NULL,
	devAoCLTUControl_write_ao,
	NULL
};
epicsExportAddress(dset, devAoCLTUControl);

struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioinit_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devAiCLTURawRead = {
	6,
	NULL,
	NULL,
	devAiCLTURawRead_init_record,
	devAiCLTURawRead_get_ioint_info,
	devAiCLTURawRead_read_ai,
	NULL
};

epicsExportAddress(dset, devAiCLTURawRead);

ULONG start, stop, _interval;
unsigned int		sysCltuStatus=0;
unsigned int gap_R1_R2 = 0;

void devCLTU_BO_ENABLE_CLK_PERM(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	int port = pParam->nArg0;

	if( pCfg->u8Version != VERSION_2 )	{
		return;
	}

	if( (unsigned int)pParam->setValue ) {
		printf("%s: port %d set 'Perm. clk on\n", pCfg->taskName, port);
		pCfg->permanentClk |= ( 0x1 << port);
	} else {
		printf("%s: port %d set 'Perm. clk off\n", pCfg->taskName, port);
		pCfg->permanentClk &= ~( 0x1 << port);
	}

	/* 1. set clear */
	memset(&pCfg->xtu_pcfg[port], 0, sizeof(ozPCONFIG_T));
	pCfg->cur_hli_number = 1;
	pCfg->cur_lli_number = 1;

/*	pCfg->xtu_pcfg[ port ].port_en = (unsigned int)(pCfg->xPortConfig[ port ].cOnOff) ? XTU_PORT_ENABLE : XTU_PORT_DISABLE; */
	pCfg->xtu_pcfg[ port ].port_en = XTU_PORT_ENABLE;
	pCfg->xtu_pcfg[port].nhli_num = 0;

	pCfg->xtu_pcfg[port].phli[0].intv.ns = XTU_MAX_INF_NS;  /* use only single HLI */
	pCfg->xtu_pcfg[port].phli[0].nhli_num = 0; /* don't care. cause unlimited clock..*/
	pCfg->xtu_pcfg[port].phli[0].nlli_num = 0;
	

	/* first valid section, first T0 event */
	if( pCfg->xPortConfig[port].r2Clock[0] == 100000000 ) { /* 100MHz */
		pCfg->xtu_pcfg[port].plli[0].clk_freq = 0;
		pCfg->xtu_pcfg[port].plli[0].option = XTU_PORT_CLK_OPT_100MHz | XTU_PORT_LLI_OPT_LOG_ON ;

	} else if (pCfg->xPortConfig[port].r2Clock[0] == 50000000 ) { 
		pCfg->xtu_pcfg[port].plli[0].clk_freq = 0;
		pCfg->xtu_pcfg[port].plli[0].option = XTU_PORT_CLK_OPT_50MHz | XTU_PORT_LLI_OPT_LOG_ON ;

	}
	else {
		pCfg->xtu_pcfg[port].plli[0].clk_freq = pCfg->xPortConfig[port].r2Clock[0];
		pCfg->xtu_pcfg[port].plli[0].option = XTU_PORT_CLK_OPT_VARFREQ | XTU_PORT_LLI_OPT_LOG_ON ;
	}
	pCfg->xtu_pcfg[port].plli[0].nlli_num = 0;
	pCfg->xtu_pcfg[port].plli[0].intv.ns = XTU_MAX_INF_NS;
	

	xtu_set_ConfigRegWrite(pCfg, port );
	
	drvCLTU_set_PermanentClk(pCfg);
	
}

void devCLTU_BO_PORT_ENABLE(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	int port = pParam->nArg0;
/*	struct dbCommon *precord = pParam->precord; */

	if( pCfg->u8Version != VERSION_2 )	{
/*		printf("CLTU_INFO! not new version. (%d)\n", pCfg->u8Version); */
		return;
	}
	pCfg->xPortConfig[ port ].cOnOff = (unsigned char)pParam->setValue;
	if( pCfg->xPortConfig[ port ].cOnOff ) 
		printf("%s: port %d 'Enabled'\n", pCfg->taskName, port);
	else
		printf("%s: port %d 'Disabled'\n", pCfg->taskName, port);

	pCfg->xtu_pcfg[ port ].port_en = (unsigned int)(pCfg->xPortConfig[ port ].cOnOff) ? XTU_PORT_ENABLE : XTU_PORT_DISABLE;
	
/*
	drvCLTU_set_PortEnable(pCfg, port);
*/	

/*	printf("Setup port %d : %s (%s)\n", pParam->nArg0, precord->name, epicsThreadGetNameSelf(), ); */
}

void devCLTU_BO_PORT_MODE(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;

	if( pCfg->u8Version != VERSION_2 ) return;

	pCfg->xPortConfig[ pParam->nArg0 ].cTrigOnOff = (unsigned char)pParam->setValue;

	if( pCfg->xPortConfig[ pParam->nArg0 ].cTrigOnOff ) 
		printf("%s: port %d set 'Trigger'\n", pCfg->taskName, pParam->nArg0);
	else
		printf("%s: port %d set 'Clock'\n", pCfg->taskName, pParam->nArg0);
	
}
void devCLTU_BO_PORT_ActiveLow(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;

	if( pCfg->u8Version != VERSION_2 ) return;
	
	pCfg->xPortConfig[ pParam->nArg0 ].cActiveLow = (unsigned char)pParam->setValue;

	if( pCfg->xPortConfig[ pParam->nArg0 ].cActiveLow ) {
		printf("%s: port %d set 'Active Low'\n", pCfg->taskName, pParam->nArg0);
		pCfg->portPolarity |= ( 0x1 << pParam->nArg0);
	} else {
		printf("%s: port %d set 'Active High'\n", pCfg->taskName, pParam->nArg0);
		pCfg->portPolarity &= ~( 0x1 << pParam->nArg0);
	}
	printf("%s: polarity bit: 0x%X\n", pCfg->taskName, pCfg->portPolarity);
	drvCLTU_set_PortActiveLevel(pCfg);
}



void devCLTU_BO_SETUP(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;

	if( pParam->nArg0 >= NUM_PORT_CNT) {
		printf("CLTU_ERROR! devCLTU_BO_SETUP (%d): Port must be 0~8\n", pParam->nArg0);
		return ;
	}

	if( drvCLTU_set_portConfig (pCfg, pParam->nArg0) == WR_ERROR )
		printf("\n>>> devCLTU_BO_SETUP : drvCLTU_set_portConfig(%d) ... error!\n", pParam->nArg0 );
	
	printf("Setup port %d : %s (%s)\n", pParam->nArg0, precord->name, epicsThreadGetNameSelf());
}

void devCLTU_BO_SHOW_INFO(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
/*	struct dbCommon *precord = pParam->precord; */
#if WITH_TIMINGBOARD
	drvCLTU_show_info(pCfg);
#endif
/*	printf("Setup port %d : %s (%s)\n", pParam->nArg0, precord->name, epicsThreadGetNameSelf()); */
}
void devCLTU_BO_SHOW_STATUS(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	drvCLTU_show_status(pCfg);
}
void devCLTU_BO_SHOW_TIME(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	drvCLTU_show_time(pCfg);
}
void devCLTU_BO_SHOW_TLK(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	drvCLTU_show_tlk(pCfg);
}
void devCLTU_BO_LOG_GSHOT(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	drvCLTU_show_log_gshot(pCfg);
}
void devCLTU_BO_LOG_PORT(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	drvCLTU_show_log_port(pCfg, pParam->nArg0);
} 

#if 0
void devCLTU_BO_SHOT_START(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	epicsTimeStamp epics_time;
	char buf[64];
#if WITH_TIMINGBOARD
	if( pCfg->u8Version == VERSION_2 && pCfg->u32Type == XTU_MODE_CTU )
	{
/*************************************************/
/*  first tried....
		pCfg = get_first_CLTUdev();
		while(pCfg) {
			if( drvCLTU_set_shotStart (pCfg) == WR_ERROR )
				printf("\n>>> devCLTU_BO_SHOT_START : drvCLTU_set_shotStart() ... error!\n" );
	  		pCfg = (ST_drvCLTU*) ellNext(&pCfg->node);
		}
		pCfg = pParam->pCfg;
*/

		ST_drvCLTU *pTSS=NULL, *pOld = NULL;
		PCI_COM_S* msg;
		unsigned int current_time=0, current_tick=0;
		unsigned int shot_start_time, shot_start_tick;  
		unsigned int shot_End_Time, shot_End_Tick;


		pTSS = get_first_CLTUdev();
  		pOld = (ST_drvCLTU*) ellNext(&pTSS->node);

		if(pOld->pciDevice.init != 0xFFFFFFFF)
		{
		    CLTU_ASSERT(); return;
		}

/*************************************************/
/*  second tried..
		if ( cltu_cmd_set_shot_start( pOld ) == WR_ERROR ) {
			printf("\n>>> drvCLTU_set_shotStart : R1, cltu_cmd_set_shot_start() ... error!\n" );
			return ;
		}

		if( drvCLTU_set_shotStart (pTSS) == WR_ERROR ){
			printf("\n>>> devCLTU_BO_SHOT_START : New version ... error!\n" );
			return ;
		}
*/

/*************************************************/
/* third tried.... */

		msg = (PCI_COM_S*) (pOld->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);


		shot_End_Time = 0xffffffff;
		shot_End_Tick = 0xffffffff;

#ifdef _CLTU_VXWORKS_ 
		taskLock();
#endif

/*		xtu_GetCurrentTime(pTSS, &current_time, &current_tick);*/
		current_time      = READ32(pTSS->pciDevice.base0 + XTUR010_);
		current_tick      = READ32(pTSS->pciDevice.base0 + XTUR011_);

/*		shot_start_tick = current_tick + 0x00030d40 + gap_R1_R2; */
		shot_start_tick = current_tick + gap_R1_R2;  /* included 1ms in gap_R1_R2 */

		if (shot_start_tick > 0x0BEBC1FF)
		{
			shot_start_tick = shot_start_tick % 0x0BEBC1FF;
			shot_start_time = current_time + 1;
		}
		else
		{
			shot_start_tick = shot_start_tick;
			shot_start_time = current_time;
		}

/*
		if( msg->status != 0xFFFFFFFF)	{
			printf("R1, status error in shot start!\n");
			return;
		}
*/
		msg->address = 0;
		msg->status  = 0;
		msg->data    = 0;
		msg->cmd     = CLTU_CMD_SHOT_START;

/*		xtu_SetShotTime(pTSS, shot_start_time, shot_start_tick, 0xffffffff, 0xffffffff);*/
		WRITE32(pTSS->pciDevice.base0 + XTUR008_TSSTM, shot_start_time);
		DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTUR009_TSSTL, shot_start_tick);
		DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTUR00A_TSETM, shot_End_Time);
		DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTUR00B_TSETL, shot_End_Tick);
		DELAY_WAIT(0);


	/* 2012. 9. 17 */
	WRITE32(pCfg->pciDevice.base0 + XTUR003_P_CNTRst, BIT31|BIT30);
	DELAY_WAIT(0);
/*		xtu_StartShotNow(pTSS);*/
		WRITE32(pTSS->pciDevice.base0 + XTUR00E_, BIT31);
		DELAY_WAIT(0);
	/* 2012. 9. 17 */
	WRITE32(pCfg->pciDevice.base0 + XTUR003_P_CNTRst, BIT31);
	DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTUR00E_, 0);
		DELAY_WAIT(0);

#ifdef _CLTU_VXWORKS_ 
		taskUnlock( ); 
#endif



	}
	else {
		if( drvCLTU_set_shotStart (pCfg) == WR_ERROR )
			printf("\n>>> devCLTU_BO_SHOT_START : drvCLTU_set_shotStart() ... error!\n" );
		}
#endif
	epicsTimeGetCurrent(&epics_time);
	epicsTimeToStrftime(buf, sizeof(buf), "%Y/%m/%d %H:%M:%S", &epics_time);

	printf("%s (%s): %s, %d\n", precord->name, epicsThreadGetNameSelf(), buf, pCfg->nCCS_shotNumber);
}
#endif

#if 0
void devCLTU_BO_SHOT_START(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	epicsTimeStamp epics_time;
	char buf[64];

	if( pCfg->u8Version == VERSION_2 && pCfg->u32Type == XTU_MODE_CTU )
	{

		ST_drvCLTU *pTSS=NULL, *pOld = NULL;
		PCI_COM_S* msg;

		/* 2013. 08. 06  last limit point. */
#if 0  /*2013. 11. 04 do not need anymore, FPGA updaged! */		
		drvCLTU_set_shotEnd(pCfg);
		epicsThreadSleep(0.015); 
		drvCLTU_set_shotEnd(pCfg);
		epicsThreadSleep(0.015); 
#endif

		pTSS = get_first_CLTUdev();
  		pOld = (ST_drvCLTU*) ellNext(&pTSS->node);

		if(pOld->pciDevice.init != 0xFFFFFFFF)
		{
		    CLTU_ASSERT(); return;
		}

		msg = (PCI_COM_S*) (pOld->pciDevice.base2 + PCI_BASE2PMC_BASEADDR_OFFSET);

#if 0
		shot_End_Time = 0xffffffff;
		shot_End_Tick = 0xffffffff;

		current_time      = READ32(pTSS->pciDevice.base0 + XTUR010_);
		current_tick      = READ32(pTSS->pciDevice.base0 + XTUR011_);

/*		shot_start_tick = current_tick + 0x00030d40 + gap_R1_R2; */
		shot_start_tick = current_tick + gap_R1_R2;  /* included 1ms in gap_R1_R2 */

		if (shot_start_tick > 0x0BEBC1FF)
		{
			shot_start_tick = shot_start_tick % 0x0BEBC1FF;
			shot_start_time = current_time + 1;
		}
		else
		{
			shot_start_tick = shot_start_tick;
			shot_start_time = current_time;
		}
		shot_start_time += 2; /* v2 start after two seconds */

/*		xtu_SetShotTime(pTSS, shot_start_time, shot_start_tick, 0xffffffff, 0xffffffff);*/
		WRITE32(pTSS->pciDevice.base0 + XTUR008_TSSTM, shot_start_time);
		DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTUR009_TSSTL, shot_start_tick);
		DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTUR00A_TSETM, shot_End_Time);
		DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTUR00B_TSETL, shot_End_Tick);
		DELAY_WAIT(0);
		
#ifdef _CLTU_VXWORKS_ 
		taskDelay( 2 * sysClkRateGet() ); /* sleep two seconds */
#endif
#endif

#ifdef _CLTU_VXWORKS_ 
		taskLock();
#endif

		WRITE32(pTSS->pciDevice.base0 + XTU_ADDR_CMD, XTU_CMD_IMM_START);
		DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTU_ADDR_SET_CMD, 1);
		DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTU_ADDR_SET_CMD, 0);


		/* at this time.. */
		/* need sleep time to synchronize old and new version */

#ifdef _CLTU_VXWORKS_ 
		taskDelay( gap_R1_R2 ); /* default: 4tick */
#endif
		
		msg->address = 0;
		msg->status  = 0;
		msg->data    = 0;
		msg->cmd     = CLTU_CMD_SHOT_START;

#ifdef _CLTU_VXWORKS_ 
		taskUnlock( ); 
#endif
/*
		epicsThreadSleep(0.007); 
		WRITE32(pTSS->pciDevice.base0 + XTU_ADDR_CMD, XTU_CMD_IMM_START);
		DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTU_ADDR_SET_CMD, 1);
		DELAY_WAIT(0);
		WRITE32(pTSS->pciDevice.base0 + XTU_ADDR_SET_CMD, 0);
*/
	}
	else {
		if( drvCLTU_set_shotStart (pCfg) == WR_ERROR )
			printf("\n>>> devCLTU_BO_SHOT_START : drvCLTU_set_shotStart() ... error!\n" );
	}

	pCfg->taskStatus |= TASK_STATE_GSHOT_ON;

	epicsTimeGetCurrent(&epics_time);
	epicsTimeToStrftime(buf, sizeof(buf), "%Y/%m/%d %H:%M:%S", &epics_time);

	printf("%s (%s): %s, %d\n", precord->name, epicsThreadGetNameSelf(), buf, pCfg->nCCS_shotNumber);
}

#endif

void devCLTU_BO_SHOT_START(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	epicsTimeStamp epics_time;
	char buf[64];

	if( drvCLTU_set_shotStart (pCfg) == WR_ERROR ){
		printf("\n>>> devCLTU_BO_SHOT_START : drvCLTU_set_shotStart() ... error!\n" );
		return;
	}

	pCfg->taskStatus |= TASK_STATE_GSHOT_ON;

	epicsTimeGetCurrent(&epics_time);
	epicsTimeToStrftime(buf, sizeof(buf), "%Y/%m/%d %H:%M:%S", &epics_time);

	printf("%s (%s): %s, %d\n", precord->name, epicsThreadGetNameSelf(), buf, pCfg->nCCS_shotNumber);
}



void devCLTU_BO_SET_FREE_RUN(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	char buf[64];
	
	if( pCfg->u8Version != VERSION_2 ) return;
	
	if( pCfg->enable_CTU_comp ) {
		printf("CLTU_INFO: delay compensation bit is enabled!\n");
		return;
	}
	/* not yet verified */
/*	memcpy(pCfg->xtuPrev_Mode, pCfg->xtuMode, sizeof(ozXTU_MODE_T) ); */

	pCfg->enable_LTU_freeRun = (unsigned char)pParam->setValue;
	if( pCfg->enable_LTU_freeRun ) 
	{
		sprintf(buf, "%s_IRIGB_SEL", pCfg->strPVhead);
		db_put(buf, "1"); /* XTU_IRIGB_GEN == 1 */

		pCfg->xtuMode.mode = XTU_MODE_CTU;
		drvCLTU_set_Mode(pCfg);

		sprintf(buf, "%s_TLKRX_ENABLE", pCfg->strPVhead);
		db_put(buf, "0"); /* XTU_TLKRX_CLK_DISABLE == 0*/

		sprintf(buf, "%s_TLKTX_SEL", pCfg->strPVhead);
		db_put(buf, "0"); /* XTU_TLKTX_CLK_ONBOARD == 0*/

		sprintf(buf, "%s_MCLK_SEL", pCfg->strPVhead);
		db_put(buf, "0"); /* XTU_MCLK_ONBOARD == 0*/
		
/*  don't need this,  alway stop when start time*/
		drvCLTU_set_shotEnd(pCfg);  /* 2013. 5. 13,  --> 2013. 11. 04 recover */
		pCfg->taskStatus &= ~TASK_STATE_GSHOT_ON;

		/* avoid abnormal shot starting.  2013. 12. 4 */ 
		pCfg->absShotTime.start.day = 0xffff;
		drvCLTU_set_shotTime(pCfg);

		printf("%s: XTU id(%d)  is standalone operation.\n", pCfg->taskName, pCfg->xtu_ID);		
	}
	else {
		drvCLTU_set_shotEnd(pCfg); /*don't need.  for safe.  2013. 5. 13 */
		/* 2013. 11. 04 recover */
		pCfg->taskStatus &= ~TASK_STATE_GSHOT_ON;
		epicsThreadSleep(0.3); /* 2013. 7. 29 */
		
		sprintf(buf, "%s_IRIGB_SEL", pCfg->strPVhead);
		db_put(buf, "0"); /* XTU_IRIGB_EXT == 0 */

		if( pCfg->u32Type == XTU_MODE_LTU ) {
			pCfg->xtuMode.mode = XTU_MODE_LTU;
			drvCLTU_set_Mode(pCfg);

			sprintf(buf, "%s_TLKRX_ENABLE", pCfg->strPVhead);
			db_put(buf, "1"); /* XTU_TLKRX_CLK_ENABLE  == BIT8*/

			sprintf(buf, "%s_TLKTX_SEL", pCfg->strPVhead);
			db_put(buf, "0"); /* XTU_TLKTX_CLK_ONBOARD == 0*/

			sprintf(buf, "%s_MCLK_SEL", pCfg->strPVhead);
			db_put(buf, "2"); /* XTU_MCLK_TLK_RX == 2*/
		} else {
			pCfg->xtuMode.mode = XTU_MODE_CTU;
			drvCLTU_set_Mode(pCfg);
			
			sprintf(buf, "%s_TLKRX_ENABLE", pCfg->strPVhead);
			db_put(buf, "0"); /* XTU_TLKRX_CLK_DISABLE  == 0*/

			sprintf(buf, "%s_TLKTX_SEL", pCfg->strPVhead);
			db_put(buf, "1"); /* XTU_TLKTX_CLK_REF == BIT4*/

			sprintf(buf, "%s_MCLK_SEL", pCfg->strPVhead);
			db_put(buf, "1"); /* XTU_MCLK_REF == 1*/
		}

		printf("%s: XTU id(%d)  is synchronized to CTU!\n", pCfg->taskName, pCfg->xtu_ID);
	}
	
}
/*
void devCLTU_BO_SET_COMP_RUN(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	
	if( pCfg->u8Version != VERSION_2 ){
		printf("CLTU_ERROR: Compensation control is only for R2 \n");
		return;
	}
	if( (pCfg->xtuMode.mode == XTU_MODE_LTU) || (pCfg->xtuMode.mode == XTU_MODE_LTU_FREE) ){
		printf("CLTU_ERROR: Compensation control is only for CTU\n");
		return;
	}

	pCfg->enable_CTU_comp = (unsigned char)pParam->setValue;
	if( pCfg->enable_CTU_comp ) {
		pCfg->xtuMode.mode = XTU_MODE_CTU_CMPST;
		printf("%s: CTU id(%d) set Compensation Mode\n", pCfg->taskName, pCfg->xtu_ID);
	} else {
		pCfg->xtuMode.mode = XTU_MODE_CTU;
		printf("%s: CTU id(%d) set Normal Operation.\n", pCfg->taskName, pCfg->xtu_ID);
	}
	xtu_SetMode(pCfg, pCfg->xtuMode.mode, pCfg->xtu_ID);
	xtu_reset(pCfg);
	
}
*/

void devCLTU_BO_SHOT_END(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	
/* 2013. 7. 29  'shot end' move to starting time , but CTU keep stay.  */
/* 2013. 11. 04  this function available again */
	if( pCfg->u8Version == VERSION_2 ) 
/*	if( pCfg->u8Version == VERSION_2 && pCfg->u32Type == XTU_MODE_CTU ) */
	{
		drvCLTU_set_shotEnd(pCfg);  
	}

	pCfg->taskStatus &= ~TASK_STATE_GSHOT_ON;
}

void devCLTU_BO_USE_REF_CLK(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;

	if( pCfg->u8Version == VERSION_1 && pCfg->u32Type == XTU_MODE_CTU)
	{
		if( (int)pParam->setValue ) {
			drvCLTU_set_RefClock(pCfg, 0xffffffff );
			printf("CTUV1 use external 50MHz clock!\n");
		}
		else {
			drvCLTU_set_RefClock(pCfg, 0x0 );
			printf("CTUV1 use internal clock!\n");
		}
	}
}

void devCLTU_BO_USE_MMAP_CTL(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;

	pCfg->use_mmap_ctl = (int)pParam->setValue;
	if( pCfg->use_mmap_ctl ) 
		printf("LTU use memory mapped io control.\n");
	else
		printf("LTU use pio control.\n");
}

void devCLTU_BO_RUN_CALIBRATION(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	char buf[64];

	if( pCfg->u8Version == VERSION_1 )
		pCfg->calib_target_ltu = (int)pParam->setValue;
	else if ( pCfg->u8Version == VERSION_2 )
	{
		if( pCfg->u32Type == XTU_MODE_LTU ){
			return;
		}

		pCfg->enable_CTU_comp = (unsigned char)pParam->setValue;
		if( pCfg->enable_CTU_comp ) {

			sprintf(buf, "%s_TLKRX_ENABLE", pCfg->strPVhead);
			db_put(buf, "1"); /* XTU_TLKRX_CLK_ENABLE  == BIT8*/
			printf("%s: CTU id(%d) set Compensation Mode\n", pCfg->taskName, pCfg->xtu_ID);
			pCfg->taskStatus |= TASK_STATE_CALIB_ON;
		} else {
			sprintf(buf, "%s_TLKRX_ENABLE", pCfg->strPVhead);
			db_put(buf, "0"); /* XTU_TLKRX_CLK_DISABLE  == 0*/
			printf("%s: CTU id(%d) set Normal Operation.\n", pCfg->taskName, pCfg->xtu_ID);
			pCfg->taskStatus &= ~TASK_STATE_CALIB_ON;
		}
	}
	else {
		printf("CLTU_ERROR! Wrong version. (%d)\n", pCfg->u8Version);
		return;
	}

	if( ioc_debug == 1)
		printf("control thread (calibarion for %f): %s (%s)\n", pParam->setValue, precord->name,
			                                            epicsThreadGetNameSelf());
}

void devCLTU_BO_IRIGB_SEL(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;

	if( pCfg->u8Version != VERSION_2 )	return;

/*	printf("Old irigb_sel value: %d\n", pCfg->xtuMode.irigb_sel);*/
	pCfg->xtuMode.irigb_sel = (unsigned int) pParam->setValue;
/*
	if( pCfg->xtuMode.irigb_sel == XTU_IRIGB_EXT ) printf("current IRIGB :  Ext(0)\n");
	else if ( pCfg->xtuMode.irigb_sel == XTU_IRIGB_GEN ) printf("current IRIGB :  Gen(1)\n");
	else printf("current IRIGB :  error!\n");
*/
	drvCLTU_set_IRIG_B_Src(pCfg );
}

void devCLTU_BO_TLKRX_ENABLE(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;

	if( pCfg->u8Version != VERSION_2 )	return;

/*	printf("Old tlkRx clk value: 0x%x\n", pCfg->xtuMode.tlkRxClk_en);*/
	pCfg->xtuMode.tlkRxClk_en = (unsigned int) pParam->setValue  ? XTU_TLKRX_CLK_ENABLE: XTU_TLKRX_CLK_DISABLE;
/*
	if( pCfg->xtuMode.tlkRxClk_en == XTU_TLKRX_CLK_ENABLE ) printf("current tlkRxClk :  enable\n");
	else if ( pCfg->xtuMode.tlkRxClk_en == XTU_TLKRX_CLK_DISABLE ) printf("current tlkRxClk :  disable\n");
	else printf("current tlkRxClk :  error!\n");
*/
	drvCLTU_set_clockConfig(pCfg);
}
void devCLTU_BO_TLKTX_SEL(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;

	if( pCfg->u8Version != VERSION_2 )	return;

/*	printf("Old tlkTx sel value: 0x%x\n", pCfg->xtuMode.tlkRxClk_en);*/
	pCfg->xtuMode.tlkTxClk_sel = (unsigned int) pParam->setValue  ? XTU_TLKTX_CLK_REF: XTU_TLKTX_CLK_ONBOARD;
/*
	if( pCfg->xtuMode.tlkTxClk_sel == XTU_TLKTX_CLK_ONBOARD ) printf("current tlkTxClk :  Onboard\n");
	else if ( pCfg->xtuMode.tlkTxClk_sel == XTU_TLKTX_CLK_REF ) printf("current tlkTxClk :  Ref\n");
	else printf("current tlkTxClk :  error!\n");
*/
	drvCLTU_set_clockConfig(pCfg);
}

static void devCLTU_BO_SET_SHOT_TIME(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	unsigned char value = (unsigned char)pParam->setValue;
	unsigned int cur_day;
	unsigned int rd_tmp;


	if( pCfg->u8Version != VERSION_2 )	return;
	if( pCfg->u32Type != XTU_MODE_CTU )	{
		printf(">>> ERROR!  not CTU mode!\n");
		return;
	}
	if( !value ) {
		return;		
	}


	if( is_mmap_ok(pCfg) ) {
		if(pCfg->pciDevice.init != 0xFFFFFFFF)
		{
			PRINT_ERR(); return;
		}   
		rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_TX_CURR_TIME      );
	} else {
#ifdef _CLTU_LINUX_	
		int status;
		status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_RX_CURR_TIME, &rd_tmp);
		if (status == -1) { printf("ioctl, IOCTL_XTU_GET_RX_CURR_TIME, error\n"); return; }
#endif
	}
	cur_day              = (rd_tmp & 0x0000ffff);
	
	
	pCfg->absShotTime.start.day = cur_day;
	pCfg->absShotTime.start.ms = 0x0;
	pCfg->absShotTime.start.ns = 0x0;	
	
	pCfg->absShotTime.stop.day = 0xffff;
	pCfg->absShotTime.stop.hour = 0xff;
	pCfg->absShotTime.stop.min = 0xff;
	pCfg->absShotTime.stop.sec = 0xff;
	pCfg->absShotTime.stop.ms = 0xffff;
	pCfg->absShotTime.stop.ns = 0xffff;
	
	drvCLTU_set_shotTime(pCfg);

}

static void devCLTU_AO_START_TIME_HH(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	unsigned char value = (unsigned char)pParam->setValue;

	if( pCfg->u8Version != VERSION_2 )	return;
	if( pCfg->u32Type != XTU_MODE_CTU )	{
		printf(">>> ERROR!  not CTU mode!\n");
		return;
	}
	if( 24 < value ) {
		printf(">>> ERROR!  input value is not valid.  (%d)\n", value);
		return;
	}
	pCfg->absShotTime.start.hour = value;

	printf("Global shot start time : %3dday/%3dh.%2dm.%2ds/%3dms.%6dns\n", 
												pCfg->absShotTime.start.day,
												pCfg->absShotTime.start.hour,
												pCfg->absShotTime.start.min,
												pCfg->absShotTime.start.sec, pCfg->absShotTime.start.ms, pCfg->absShotTime.start.ns);
}	
static void devCLTU_AO_START_TIME_MM(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	unsigned char value = (unsigned char)pParam->setValue;

	if( pCfg->u8Version != VERSION_2 )	return;
	if( pCfg->u32Type != XTU_MODE_CTU )	{
		printf(">>> ERROR!  not CTU mode!\n");
		return;
	}
	if( 60 < value ) {
		printf(">>> ERROR!  input value is not valid.  (%d)\n", value);
		return;
	}
	pCfg->absShotTime.start.min = value;

	printf("Global shot start time : %3dday/%3dh.%2dm.%2ds/%3dms.%6dns\n", 
												pCfg->absShotTime.start.day,
												pCfg->absShotTime.start.hour,
												pCfg->absShotTime.start.min,
												pCfg->absShotTime.start.sec, pCfg->absShotTime.start.ms, pCfg->absShotTime.start.ns);
}	
static void devCLTU_AO_START_TIME_SS(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	unsigned char value = (unsigned char)pParam->setValue;

	if( pCfg->u8Version != VERSION_2 )	return;
	if( pCfg->u32Type != XTU_MODE_CTU )	{
		printf(">>> ERROR!  not CTU mode!\n");
		return;
	}
	if( 60 < value ) {
		printf(">>> ERROR!  input value is not valid.  (%d)\n", value);
		return;
	}
	pCfg->absShotTime.start.sec = value;

	printf("Global shot start time : %3dday/%3dh.%2dm.%2ds/%3dms.%6dns\n", 
												pCfg->absShotTime.start.day,
												pCfg->absShotTime.start.hour,
												pCfg->absShotTime.start.min,
												pCfg->absShotTime.start.sec, pCfg->absShotTime.start.ms, pCfg->absShotTime.start.ns);
}	


void devCLTU_AO_READ_BUFFER(execParam *pParam)
{
#ifdef _CLTU_LINUX_

	unsigned int buff[128];
	int i, status, offset;
	ssize_t			ret;
	ST_drvCLTU *pCfg = pParam->pCfg;
	if( pCfg->u8Version != VERSION_2 ) return;

	if( ( unsigned int) pParam->setValue > 0xffffff ) {
		printf("limit line counter is 0xffffff : input line number is :%d\n", (int) pParam->setValue );
		return;
	}
	offset = (unsigned int) pParam->setValue;

	status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_SET_READ_OFFSET, &offset);
	if (status == -1) { printf("ioctl, IOCTL_XTU_SET_READ_OFFSET, error\n"); return; }

	ret = read(pCfg->pciDevice.fd, buff, 4*10);

	printf("----------------------\n");
	printf("offset: 0x%x\n", offset);
	for(i=0; i< 10; i++) {
		printf("%d(%04x): 0x%08x \n", i, offset + (i*4), buff[i]);
	}

#endif

	return;	
}

void devCLTU_AO_MCLK_SEL(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	if( pCfg->u8Version != VERSION_2 ) return;

/*	printf("Old mclk value: %d\n", pCfg->xtuMode.mclk_sel);*/
	pCfg->xtuMode.mclk_sel = (unsigned int) pParam->setValue;
/*
	if( pCfg->xtuMode.mclk_sel == XTU_MCLK_ONBOARD ) printf("current MCLK :  Onboard(0)\n");
	else if( pCfg->xtuMode.mclk_sel == XTU_MCLK_REF ) printf("current MCLK :  reference(1)\n");
	else if ( pCfg->xtuMode.mclk_sel == XTU_MCLK_TLK_RX ) printf("current MCLK :  tlk RX(2)\n");
	else {
		printf("current MCLK :  error! set to 'tlk RX'\n");
		pCfg->xtuMode.mclk_sel = XTU_MCLK_TLK_RX;
	}
*/	
	drvCLTU_set_clockConfig(pCfg);
}


void devCLTU_AO_GAP_R1R2(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
/*	struct dbCommon *precord = pParam->precord; */

	if( pCfg->u8Version != VERSION_2 )	{
/*		printf("CLTU_INFO! not new version. Gap supported by only R2.(%d)\n", pCfg->u8Version); */
		return;
	}
/*	drvCLTU_set_tGap((unsigned int) pParam->setValue); */

	printf("Old gap value: %d\n", gap_R1_R2);
	gap_R1_R2 = (unsigned int) pParam->setValue;
	printf("New gap value: %d\n", gap_R1_R2);

}

/* 100MHz / (value+1) */
void devCLTU_AO_SECTION_CLK(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;

	if( pCfg->u8Version != VERSION_2 )	{
/*		printf("CLTU_INFO! not new version. (%d)\n", pCfg->u8Version); */
		return;
	}
	if( (unsigned int)pParam->setValue == 0) {
		printf("CLTU_ERROR! 0Hz is invalid. (%dHz)  change to 1Hz\n", (unsigned int)pParam->setValue);
		pParam->setValue = 1.0;
	}
/*	
	if( pParam->setValue > 1000 ) {
		pCfg->xPortConfig[ pParam->nArg0 ].clock[pParam->nArg1] = (unsigned int)((EPICS_CLOCK_100M / (unsigned int)pParam->setValue) -2);
	} else if( pParam->setValue >= 1 )
		pCfg->xPortConfig[ pParam->nArg0 ].clock[pParam->nArg1] = (unsigned int)((EPICS_CLOCK_100M / (unsigned int)pParam->setValue) -2);
*/		

	
/*	pCfg->xPortConfig[ pParam->nArg0 ].clock[pParam->nArg1] = (unsigned int)((EPICS_CLOCK_100M / (unsigned int)pParam->setValue) -2); */
	pCfg->xPortConfig[ pParam->nArg0 ].r2Clock[pParam->nArg1] = (unsigned int)(pParam->setValue);
	if( ioc_debug == 2)
		printf("%s: port:%d, sec:%d, clock:%dHz\n", pCfg->taskName, pParam->nArg0, pParam->nArg1, (unsigned int)pParam->setValue );
#if 0
	if( drvCLTU_set_portConfig (pCfg, pParam->nArg0) == WR_ERROR )
		printf("\n>>> devCLTU_BO_SETUP : drvCLTU_set_portConfig(%d) ... error!\n", pParam->nArg0 );
#endif
	if( ioc_debug == 1)
		printf("control thread (ccs shot number : %d): %s (%s)\n", (unsigned int)pParam->setValue, precord->name,
			                                            epicsThreadGetNameSelf());
}

static void devCLTU_AO_SECTION_T0_BLIP(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	double value = pParam->setValue;
	char strName[60], strval[16];
	int len, port, sec;

	if( pCfg->u8Version != VERSION_2 ) return;

	port = pParam->nArg0;
	sec = pParam->nArg1;

	if( value < (-1*CCS_BLIP_TIME) ) value = (-1*CCS_BLIP_TIME);

	len = strlen(precord->name) - 5;
	strcpy( strName, "");
	strncpy(strName, precord->name, len );
	strName[len] = '\0';
/*	printf("len:%d, %s  --> %s\n", len,  precord->name, strName); */
	sprintf(strval, "%.10f", value + CCS_BLIP_TIME );

	if ( pCfg->xPortConfig[ port ].cFlag0[sec] != 74) {
/*		printf("call for normal T0 set\n"); */
		pCfg->xPortConfig[ port ].cFlag0[sec] = 74;
		db_put(strName, strval);		
/*		printf("release from normal T0 set\n"); */
	} 
	else 
		pCfg->xPortConfig[ port ].cFlag0[sec] = 0x0;

	pCfg->xPortConfig[ port ].dT0[sec] = value + CCS_BLIP_TIME;
/*	printf(" %s: %.09f\n", precord->name, pCfg->xPortConfig[ port ].dT0[sec]  ); */
	
}
static void devCLTU_AO_SECTION_T0(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	double value = pParam->setValue;
	char pvname[60], strval[16];
	int port, sec;

	if( pCfg->u8Version != VERSION_2 ) return;
	
	port = pParam->nArg0;
	sec = pParam->nArg1;
	
	if( value < 0 ) value = 0;
	
	sprintf(pvname, "%s_BLIP", precord->name);
	sprintf(strval, "%.10f", value - CCS_BLIP_TIME );

	if ( pCfg->xPortConfig[ port ].cFlag0[sec] != 74) {
		pCfg->xPortConfig[ port ].cFlag0[sec] = 74;
		db_put(pvname, strval);		
	}
	else 
		pCfg->xPortConfig[ port ].cFlag0[sec] = 0x0;
	
	pCfg->xPortConfig[ port ].dT0[ sec ] = value;
	
}

static void devCLTU_AO_SECTION_T1_BLIP(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	double value = pParam->setValue;
	char pvname[60], strval[16];
	int len, port, sec;

	if( pCfg->u8Version != VERSION_2 )	return;

	port = pParam->nArg0;
	sec = pParam->nArg1;

	if( value < (-1*CCS_BLIP_TIME) ) value = (-1*CCS_BLIP_TIME);	

	len = strlen(precord->name) - 5;
	strcpy( pvname, "");
	strncpy(pvname, precord->name, len );
	pvname[len] = '\0';
	sprintf(strval, "%.10f", value + CCS_BLIP_TIME );

	if ( pCfg->xPortConfig[ port ].cFlag1[sec] != 74 ) {
		pCfg->xPortConfig[ port ].cFlag1[sec] = 74;
		db_put(pvname, strval);		
	}
	else
		pCfg->xPortConfig[ port ].cFlag1[sec] = 0x0;
	
	pCfg->xPortConfig[ port ].dT1[ sec ] = value + CCS_BLIP_TIME;
}	


static void devCLTU_AO_SECTION_T1(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	double value = pParam->setValue;
	char pvname[60], strval[16];
	int port, sec;

	if( pCfg->u8Version != VERSION_2 )	return;

	port = pParam->nArg0;
	sec = pParam->nArg1;

	if( value < 0 ) value = 0;

	sprintf(pvname, "%s_BLIP", precord->name);
	sprintf(strval, "%.10f", value - CCS_BLIP_TIME );

	if ( pCfg->xPortConfig[ port ].cFlag1[sec] != 74 ) {
		pCfg->xPortConfig[ port ].cFlag1[sec] = 74;
		db_put(pvname, strval);		
	}
	else 
		pCfg->xPortConfig[ port ].cFlag1[sec] = 0x0;
	

/*	pCfg->xPortConfig[ pParam->nArg0 ].stopOffset[pParam->nArg1] = (unsigned long long int)(pParam->setValue * EPICS_CLOCK_200M + 1 ); */
	pCfg->xPortConfig[ port ].dT1[ sec ] = value;
}	


void devCLTU_AO_SHOT_NUMBER(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	

	pCfg->nCCS_shotNumber = (unsigned int)pParam->setValue;

	if( ioc_debug == 1)
		printf("control thread (ccs shot number : %d): %s (%s)\n", (unsigned int)pParam->setValue, precord->name,
			                                            epicsThreadGetNameSelf());
}

void devCLTU_AO_SET_CAL_VALUE(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	FILE *fp=NULL;
	char fname[128];
/*	static int isFirst = 1; */

/*	if( pCfg->u32Type == XTU_MODE_CTU ) return; */

	if( (unsigned int)pParam->setValue == 0 ) {
/*		isFirst = 0; */
	} else
		pCfg->compPutTime.ns= (unsigned int)pParam->setValue;


	if( drvCLTU_put_calibratedTick(pCfg ) == WR_ERROR )
		printf("\n>>> devCLTU_AO_SET_CAL_VALUE : drvCLTU_put_calibratedTick(%d) ... error!\n", pCfg->compPutTime.ns );
	else 
	{
		sprintf(fname, "%s/iocBoot/%s/%s%d", pCfg->strTOP, pCfg->strIOC, FILE_CALIB_VALUE, pCfg->xtu_ID);

		fp = fopen(fname, "w");
		if(!fp) {
			printf("CLTU_ERROR: can't open file \"%s\". \n", fname);
			return;
		}
		fprintf(fp, "%d\n", pCfg->compPutTime.ns);
		fclose(fp);
	}

	if( ioc_debug == 1)
		printf("control thread (calibrated value : %d): %s (%s)\n", pCfg->compPutTime.ns , precord->name,
			                                            epicsThreadGetNameSelf());
}

/*
void devCLTU_set_shotStart(execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
#if WITH_TIMINGBOARD
	if( drvCLTU_set_shotStart () == WR_ERROR )
		printf("\n>>> devCLTU_set_shotStart : drvCLTU_set_shotStart() ... error!\n" );
#endif

	if( ioc_debug == 1)
		printf("control thread (shot start): %s (%s)\n", precord->name,
			                                            epicsThreadGetNameSelf());
}
*/
void devCLTU_AO_ShotTerm(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;

	pCfg->nShotTerm_msb = (unsigned int) (time64>>32);
	pCfg->nShotTerm_lsb = (unsigned int) time64;

	if( ioc_debug == 1) {	
		printf(">>> shot term msb : 0x%x (%f sec)\n", pCfg->nShotTerm_msb, pParam->setValue );
		printf(">>> shot term lsb : 0x%x\n", pCfg->nShotTerm_lsb );
	}
#if WITH_TIMINGBOARD
	if( drvCLTU_set_shotTerm (pCfg) == WR_ERROR )
		printf("\n>>> devCLTU_AO_ShotTerm : drvCLTU_set_shotTerm() ... error!\n" );
#endif
	if( ioc_debug == 1)
		printf("control thread (shot period): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
}

/*  onoff: 1=on, 0=off */
void devCLTU_set_clockOnShot(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
#if WITH_TIMINGBOARD
	if( drvCLTU_set_clockOnShot( pCfg, pParam->nArg0,  pParam->setValue) == WR_ERROR )
		printf("\n>>> devCLTU_set_clockOnShot : drvCLTU_set_clockOnShot(%d, %d) ... error!\n", pParam->nArg0 , (int)pParam->setValue);
#endif
	if( ioc_debug == 1)
		printf("control thread (set clock on shot): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
}
/*
void devCLTU_setup(execParam *pParam)
{
	struct dbCommon *precord = pParam->precord;
#if WITH_TIMINGBOARD
	if( drvCLTU_set_portConfig (pParam->nArg0) == WR_ERROR )
		printf("\n>>> devCLTU_setup : drvCLTU_set_portConfig(%d) ... error!\n", pParam->nArg0 );
#endif
	if( ioc_debug == 1)
		printf("control thread (setup p%d): %s (%s)\n", pParam->nArg0, precord->name, epicsThreadGetNameSelf());
}
*/
void devCLTU_AO_Trig(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;


	if( pCfg->u8Version == VERSION_1)
	{
		pCfg->sPortConfig[pParam->nArg0].nTrigMode = (pParam->setValue == ACTIVE_HIGHT_VAL)? 0xffffffff : 0x0;
		pCfg->curPortConfigStatus[pParam->nArg0] |= PORT_TRIG_OK;

		if( ioc_debug == 1) {
			printf(">>> Trigger: port(%d), level(0x%x)\n", pParam->nArg0, pCfg->sPortConfig[pParam->nArg0].nTrigMode );
			printf("control thread : %s (%s)\n", precord->name, epicsThreadGetNameSelf());
		}
	}
	else if( pCfg->u8Version == VERSION_2)
	{
		pCfg->xPortConfig[pParam->nArg0].cActiveLow = (pParam->setValue == ACTIVE_HIGHT_VAL)? 0 : 1;
		pCfg->curPortConfigStatus[pParam->nArg0] |= PORT_TRIG_OK;

		if( ioc_debug == 1) {
			printf(">>> Trigger: port(%d), level(0x%x)\n", pParam->nArg0, pCfg->xPortConfig[pParam->nArg0].cActiveLow);
			printf("control thread : %s (%s)\n", precord->name, epicsThreadGetNameSelf());
		}
	}
	else
	{
		printf("ERROR_CLTU:  Wrong version (%d), 0x%x\n", pCfg->u8Version, pCfg->u8Version);
		printf("control thread : %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}

}

void devCLTU_AO_Clock(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	
	pCfg->sPortConfig[pParam->nArg0].nClock = pParam->setValue;
	pCfg->curPortConfigStatus[pParam->nArg0] |= PORT_CLOCK_OK;
	
	if( ioc_debug == 1) {
		printf(">>> Clock :p(%d) %d Hz\n",pParam->nArg0, pCfg->sPortConfig[pParam->nArg0].nClock );
		printf("control thread (clock rate): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}

void devCLTU_AO_T0(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;
	
	pCfg->sPortConfig[pParam->nArg0].nT0_msb = (unsigned int) (time64>>32);
	pCfg->sPortConfig[pParam->nArg0].nT0_lsb = (unsigned int) time64;
	pCfg->curPortConfigStatus[pParam->nArg0] |= PORT_T0_OK;

	if( ioc_debug == 1) {
		printf(">>> T0 msb : p(%d) 0x%x  (%f sec)\n", pParam->nArg0, pCfg->sPortConfig[pParam->nArg0].nT0_msb , pParam->setValue);
		printf(">>> T0 lsb : p(%d) 0x%x\n", pParam->nArg0, pCfg->sPortConfig[pParam->nArg0].nT0_lsb );
  
		printf("control thread (set t0): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}

void devCLTU_AO_T1(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;
	
	pCfg->sPortConfig[pParam->nArg0].nT1_msb = (unsigned int) (time64>>32);
	pCfg->sPortConfig[pParam->nArg0].nT1_lsb = (unsigned int) time64;
	pCfg->curPortConfigStatus[pParam->nArg0] |= PORT_T1_OK;

	if( ioc_debug == 1) {
		printf(">>> T1 msb : p(%d) 0x%x (%f sec)\n", pParam->nArg0, pCfg->sPortConfig[pParam->nArg0].nT1_msb , pParam->setValue);
		printf(">>> T1 lsb : p(%d) 0x%x\n", pParam->nArg0, pCfg->sPortConfig[pParam->nArg0].nT1_lsb );

		printf("control thread (set t1): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}

/* In this function(multi-trig mode),  we assume "nArg0" is "section number" */
void devCLTU_AO_mTrig(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
/*	pCfg->mPortConfig[pParam->nArg0].nTrigMode = (pParam->setValue == ACTIVE_HIGHT_VAL)? 0xffffffff : 0x0; */
	pCfg->mPortConfig[0].nTrigMode = (pParam->setValue == ACTIVE_HIGHT_VAL)? 0xffffffff : 0x0;
	pCfg->curPortConfigStatus[4] |= PORT_TRIG_OK;
	if( ioc_debug == 1) {
		printf(">>> mTrigger: level(0x%x)\n", pCfg->mPortConfig[0].nTrigMode );
		printf("control thread : %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}
void devCLTU_AO_mClock(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;


	if( pCfg->u8Version == VERSION_1)
	{
		struct dbCommon *precord = pParam->precord;

		pCfg->mPortConfig[pParam->nArg0].nClock = pParam->setValue;
		pCfg->curPortConfigStatus[4] |= PORT_CLOCK_OK;
		if( ioc_debug == 1) {
			printf(">>> mClock :section(%d) %d Hz\n", pParam->nArg0, pCfg->mPortConfig[pParam->nArg0].nClock );
			printf("control thread (clock rate): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
		}
	}
/*	else if( pCfg->u8Version == VERSION_2)
	{
		pCfg->xPortConfig[pParam->nArg0].clock[0] = (unsigned int) pParam->setValue;
		pCfg->curPortConfigStatus[pParam->nArg0] |= PORT_CLOCK_OK;
		if( ioc_debug == 1) {
			printf(">>> mClock :section(%d) %d Hz\n", pParam->nArg0, pCfg->xPortConfig[pParam->nArg0].clock[0]);
			printf("control thread (clock rate): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
		}

	}
	else
	{
		printf("ERROR_CLTU:  Wrong version (%d), 0x%x\n", pCfg->u8Version, pCfg->u8Version);
		printf("control thread : %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
*/
}
void devCLTU_AO_mT0(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;
	pCfg->mPortConfig[pParam->nArg0].nT0_msb = (unsigned int) (time64>>32);
	pCfg->mPortConfig[pParam->nArg0].nT0_lsb = (unsigned int) time64;
	pCfg->curPortConfigStatus[4] |= PORT_T0_OK;
	if( ioc_debug == 1) {
		printf(">>> mT0 msb : section(%d) 0x%x (%f sec)\n", pParam->nArg0, pCfg->mPortConfig[pParam->nArg0].nT0_msb, pParam->setValue);
		printf(">>> mT0 lsb : section(%d) 0x%x\n", pParam->nArg0, pCfg->mPortConfig[pParam->nArg0].nT0_lsb );
		printf("control thread (set t0): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}
void devCLTU_AO_mT1(execParam *pParam)
{
	ST_drvCLTU *pCfg = pParam->pCfg;
	struct dbCommon *precord = pParam->precord;
	unsigned long long  int time64;
	time64 = pParam->setValue * 100000000;
	pCfg->mPortConfig[pParam->nArg0].nT1_msb = (unsigned int) (time64>>32);
	pCfg->mPortConfig[pParam->nArg0].nT1_lsb = (unsigned int) time64;
	pCfg->curPortConfigStatus[4] |= PORT_T1_OK;
	if( ioc_debug == 1) {
		printf(">>> mT1 msb : section(%d) 0x%x (%f sec)\n", pParam->nArg0, pCfg->mPortConfig[pParam->nArg0].nT1_msb, pParam->setValue);
		printf(">>> mT1 lsb : section(%d) 0x%x\n", pParam->nArg0, pCfg->mPortConfig[pParam->nArg0].nT1_lsb );
		printf("control thread (set t1): %s (%s)\n", precord->name, epicsThreadGetNameSelf());
	}
}

LOCAL long devAoCLTUControl_init_record(aoRecord *precord)
{
	devCLTUdpvt *pdevCLTUdpvt = (devCLTUdpvt*) malloc(sizeof(devCLTUdpvt));
	int i;
	
	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevCLTUdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s %s %s",
				   pdevCLTUdpvt->taskName,
				   pdevCLTUdpvt->arg1,
			           pdevCLTUdpvt->arg2, 
			           pdevCLTUdpvt->arg3);

			if( ioc_debug == 2)
				printf("devAoCLTUControl arg num: %d: %s %s %s\n",i, pdevCLTUdpvt->taskName, 
								pdevCLTUdpvt->arg1,
								pdevCLTUdpvt->arg2);

/*			pdevCLTUdpvt->pCfg = drvCLTU_get_taskConfig(); */
			pdevCLTUdpvt->pCfg = drvCLTU_find_taskConfig(pdevCLTUdpvt->taskName);
			
			if(!pdevCLTUdpvt->pCfg) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devAoCLTUControl (init_record) Illegal INP field: task_name");
				free(pdevCLTUdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
		
				   
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord,
					  "devAoCLTUControl (init_record) Illegal OUT field");
			free(pdevCLTUdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
			
	}

	pdevCLTUdpvt->nArg0 = -1;
	
	if( i == 2 ) {
/*		if(!strcmp(pdevCLTUdpvt->arg1, AO_SHOT_START_STR)) {
			pdevCLTUdpvt->ind = AO_SHOT_START;
		} else */
		if (!strcmp(pdevCLTUdpvt->arg1, AO_SHOT_TERM_STR)) {
			pdevCLTUdpvt->ind = AO_SHOT_TERM;
		} else if (!strcmp(pdevCLTUdpvt->arg1, AO_PUT_CALED_VAL_STR)) {
			pdevCLTUdpvt->ind = AO_PUT_CALED_VAL;
		}  else if(!strcmp(pdevCLTUdpvt->arg1, AO_SHOT_NUMBER_STR)) {
			pdevCLTUdpvt->ind = AO_SHOT_NUMBER;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_GAP_R1R2_STR)) {
			pdevCLTUdpvt->ind = AO_GAP_R1R2;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_MCLK_SEL_STR)) {
			pdevCLTUdpvt->ind = AO_MCLK_SEL;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_READ_BUFFER_STR)) {
			pdevCLTUdpvt->ind = AO_READ_BUFFER;
		}
		 else if(!strcmp(pdevCLTUdpvt->arg1, AO_START_TIME_HH_STR)) {
			pdevCLTUdpvt->ind = AO_START_TIME_HH;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_START_TIME_MM_STR)) {
			pdevCLTUdpvt->ind = AO_START_TIME_MM;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_START_TIME_SS_STR)) {
			pdevCLTUdpvt->ind = AO_START_TIME_SS;
		}
		

	}
	else if (i ==3 ) 
	{
		
/*		if (!strcmp(pdevCLTUdpvt->arg1, AO_SETUP_STR)) {
			pdevCLTUdpvt->ind = AO_SETUP;
		} else */
		if(!strcmp(pdevCLTUdpvt->arg1, AO_CLOCK_ONSHOT_STR)) {
			pdevCLTUdpvt->ind = AO_CLOCK_ONSHOT;
		
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_TRIG_STR)) {
			pdevCLTUdpvt->ind = AO_TRIG;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_CLOCK_STR)) {
			pdevCLTUdpvt->ind = AO_CLOCK;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_T0_STR)) {
			pdevCLTUdpvt->ind = AO_T0;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_T1_STR)) {
			pdevCLTUdpvt->ind = AO_T1;

		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_MTRIG_STR)) {
			pdevCLTUdpvt->ind = AO_MTRIG;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_MCLOCK_STR)) {
			pdevCLTUdpvt->ind = AO_MCLOCK;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_MT0_STR)) {
			pdevCLTUdpvt->ind = AO_MT0;
		} else if(!strcmp(pdevCLTUdpvt->arg1, AO_MT1_STR)) {
			pdevCLTUdpvt->ind = AO_MT1;
		}

/*		pdevCLTUdpvt->nArg0 = atoi(pdevCLTUdpvt->arg2); */
	}
	else if (i ==4 ) 
	{
		if(!strcmp(pdevCLTUdpvt->arg1, AO_SECTION_CLK_STR))
			pdevCLTUdpvt->ind = AO_SECTION_CLK;
		else if(!strcmp(pdevCLTUdpvt->arg1, AO_SECTION_T0_STR))
			pdevCLTUdpvt->ind = AO_SECTION_T0;
		else if(!strcmp(pdevCLTUdpvt->arg1, AO_SECTION_T1_STR))
			pdevCLTUdpvt->ind = AO_SECTION_T1;
		else if(!strcmp(pdevCLTUdpvt->arg1, AO_SECTION_T0_BLIP_STR))
			pdevCLTUdpvt->ind = AO_SECTION_T0_BLIP;
		else if(!strcmp(pdevCLTUdpvt->arg1, AO_SECTION_T1_BLIP_STR))
			pdevCLTUdpvt->ind = AO_SECTION_T1_BLIP;

	}

	sysCltuStatus = 0; 

	precord->udf = FALSE;
	precord->dpvt = (void*) pdevCLTUdpvt;
	
	return 2;     /*returns: (0,2)=>(success,success no convert)*/
}

LOCAL long devAoCLTUControl_write_ao(aoRecord *precord)
{
	devCLTUdpvt        *pdevCLTUdpvt = (devCLTUdpvt*) precord->dpvt;
	ST_drvCLTU          *pCfg;
	drvCLTU_controlThreadConfig *pcontrolThreadConfig;
	controlThreadQueueData         qData;



	if(!pdevCLTUdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1;
	}

	pCfg			= pdevCLTUdpvt->pCfg;
	pcontrolThreadConfig		= pCfg->pcontrolThreadConfig;
	qData.param.pCfg		= pCfg;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= precord->val;
/*	qData.param.nArg0			= pdevCLTUdpvt->nArg0; */
	qData.param.nArg0 = atoi(pdevCLTUdpvt->arg2); /* port number */
	qData.param.nArg1 = atoi(pdevCLTUdpvt->arg3); /* section number */



        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

/*		start = drvCLTU_getCurrentTick(); */

#if PRINT_PHASE_INFO
		printf("db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif

		switch(pdevCLTUdpvt->ind) {
/*			case AO_SHOT_START:
				qData.pFunc = devCLTU_set_shotStart;
				break;
*/			case AO_SHOT_TERM:
				qData.pFunc = devCLTU_AO_ShotTerm;
				break;
			case AO_TRIG:
				qData.pFunc = devCLTU_AO_Trig;
				break;
			case AO_CLOCK:
				qData.pFunc = devCLTU_AO_Clock;
				break;
			case AO_T0:
				qData.pFunc = devCLTU_AO_T0;
				break;
			case AO_T1:
				qData.pFunc = devCLTU_AO_T1;
				break;
			case AO_MTRIG:
				qData.pFunc = devCLTU_AO_mTrig;
				break;
			case AO_MCLOCK:
				qData.pFunc = devCLTU_AO_mClock;
				break;
			case AO_MT0:
				qData.pFunc = devCLTU_AO_mT0;
				break;
			case AO_MT1:
				qData.pFunc = devCLTU_AO_mT1;
				break;
/*			case AO_SETUP:
				qData.pFunc = devCLTU_setup;
				break;
*/			case AO_CLOCK_ONSHOT:
				qData.pFunc = devCLTU_set_clockOnShot; /* not used 2009-11-16*/
				break;
			case AO_PUT_CALED_VAL:
				qData.pFunc = devCLTU_AO_SET_CAL_VALUE;
				break;
			case AO_SHOT_NUMBER:
				qData.pFunc = devCLTU_AO_SHOT_NUMBER;
				break;
/****************************************************************/
/*  this for CLTU version 2 */

			case AO_SECTION_CLK:
				qData.pFunc = devCLTU_AO_SECTION_CLK;
				break;
			case AO_SECTION_T0:
				qData.pFunc = devCLTU_AO_SECTION_T0;
				break;
			case AO_SECTION_T1:
				qData.pFunc = devCLTU_AO_SECTION_T1;
				break;
			case AO_GAP_R1R2:
				qData.pFunc = devCLTU_AO_GAP_R1R2;
				break;
			case AO_MCLK_SEL:
				qData.pFunc = devCLTU_AO_MCLK_SEL;
				break;
			case AO_READ_BUFFER:
				qData.pFunc = devCLTU_AO_READ_BUFFER;
				break;
			case AO_SECTION_T0_BLIP:
				qData.pFunc = devCLTU_AO_SECTION_T0_BLIP;
				break;
			case AO_SECTION_T1_BLIP:
				qData.pFunc = devCLTU_AO_SECTION_T1_BLIP;
				break;
			case AO_START_TIME_HH:
				qData.pFunc = devCLTU_AO_START_TIME_HH;
				break;
			case AO_START_TIME_MM:
				qData.pFunc = devCLTU_AO_START_TIME_MM;
				break;
			case AO_START_TIME_SS:
				qData.pFunc = devCLTU_AO_START_TIME_SS;
				break;
				
			default: break;
		}

		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(controlThreadQueueData));

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
		
#if PRINT_PHASE_INFO
		printf("db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());
#endif

		precord->pact = FALSE;
        	precord->udf = FALSE;
#if 1
		switch(pdevCLTUdpvt->ind) {
			case AO_PUT_CALED_VAL:
				precord->val = pCfg->compPutTime.ns;
				break;
			case AO_SECTION_T0:
				precord->val = pCfg->xPortConfig[ qData.param.nArg0 ].dT0[qData.param.nArg1];
				break;
			case AO_SECTION_T1:
				precord->val = pCfg->xPortConfig[ qData.param.nArg0 ].dT1[qData.param.nArg1];
				break;
			case AO_SECTION_T0_BLIP:
				precord->val = pCfg->xPortConfig[ qData.param.nArg0 ].dT0[qData.param.nArg1] - CCS_BLIP_TIME;
				break;
			case AO_SECTION_T1_BLIP:
				precord->val = pCfg->xPortConfig[ qData.param.nArg0 ].dT1[qData.param.nArg1] - CCS_BLIP_TIME;
				break;
			case AO_START_TIME_HH:
				precord->val = pCfg->absShotTime.start.hour;
				break;
			case AO_START_TIME_MM:
				precord->val = pCfg->absShotTime.start.min;
				break;
			case AO_START_TIME_SS:
				precord->val = pCfg->absShotTime.start.sec;
				break;
				
			
			default: break;
		}
#endif
		return 0;
	}

	return -1;
}



LOCAL long devAiCLTURawRead_init_record(aiRecord *precord)
{
	devAiCLTURawReaddpvt *pdevAiCLTURawReaddpvt = (devAiCLTURawReaddpvt*) malloc(sizeof(devAiCLTURawReaddpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevAiCLTURawReaddpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pdevAiCLTURawReaddpvt->taskName,
				   pdevAiCLTURawReaddpvt->arg1,
				   pdevAiCLTURawReaddpvt->arg2);
if( ioc_debug == 2)
	printf("devAiCLTURawRead arg num: %d: %s %s %s\n",i, pdevAiCLTURawReaddpvt->taskName, 
								pdevAiCLTURawReaddpvt->arg1,
								pdevAiCLTURawReaddpvt->arg2);

/*			pdevAiCLTURawReaddpvt->pCfg = drvCLTU_get_taskConfig(); */
			pdevAiCLTURawReaddpvt->pCfg = drvCLTU_find_taskConfig(pdevAiCLTURawReaddpvt->taskName);
			if(!pdevAiCLTURawReaddpvt->pCfg) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devAiCLTURawRead (init_record) Illegal INP field: task name");
				free(pdevAiCLTURawReaddpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devAiCLTURawRead (init_record) Illegal INP field");
			free(pdevAiCLTURawReaddpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if(!strcmp(pdevAiCLTURawReaddpvt->arg1, AI_CLTU_MCLOCK_STR))
		pdevAiCLTURawReaddpvt->ind = AI_CLTU_MCLOCK;
	else if(!strcmp(pdevAiCLTURawReaddpvt->arg1, AI_CLTU_TIME_STR)) {
		pdevAiCLTURawReaddpvt->ind = AI_CLTU_TIME;
		
	}
	else if(!strcmp(pdevAiCLTURawReaddpvt->arg1, AI_CLTU_OFFSET_STR))
		pdevAiCLTURawReaddpvt->ind = AI_CLTU_OFFSET;
	else if(!strcmp(pdevAiCLTURawReaddpvt->arg1, AI_CLTU_ID_STR))
		pdevAiCLTURawReaddpvt->ind = AI_CLTU_ID;
	else if(!strcmp(pdevAiCLTURawReaddpvt->arg1, AI_GET_COMP_NS_STR))
		pdevAiCLTURawReaddpvt->ind = AI_GET_COMP_NS;
	
	else {
		recGblRecordError(S_db_badField, (void*) precord,  "devAiCLTURawRead (init_record) error argment");
		free(pdevAiCLTURawReaddpvt);
		precord->udf = TRUE;
		precord->dpvt = NULL;
		return S_db_badField;
	}
	
	precord->udf = FALSE;
	precord->dpvt = (void*) pdevAiCLTURawReaddpvt;

	return 2;    /* don't convert */ 
}


LOCAL long devAiCLTURawRead_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	devAiCLTURawReaddpvt *pdevAiCLTURawReaddpvt = (devAiCLTURawReaddpvt*) precord->dpvt;
	ST_drvCLTU   *pCfg;

	if(!pdevAiCLTURawReaddpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pCfg = pdevAiCLTURawReaddpvt->pCfg;
	*ioScanPvt  = pCfg->ioScanPvt;
	return 0;
}

LOCAL long devAiCLTURawRead_read_ai(aiRecord *precord)
{ 
	devAiCLTURawReaddpvt	*pdevAiCLTURawReaddpvt = (devAiCLTURawReaddpvt*) precord->dpvt;
	ST_drvCLTU		*pCfg;

	System_Info* mSys;
	unsigned long long int curtime64, temp64;



	if(!pdevAiCLTURawReaddpvt) return 0;

	pCfg		= pdevAiCLTURawReaddpvt->pCfg;
#if WITH_TIMINGBOARD
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
/*		CLTU_ASSERT();  2012. 5. 23 alway print when exit. why??? */
		precord->val = 0; 
		return -1; /*(0,2)=> success and convert,don't convert)*/
	}
	mSys = (System_Info*) (pCfg->pciDevice.base2);
#else
	mSys = (System_Info *)malloc(sizeof(System_Info) ); /* temporary */
#endif

	switch(pdevAiCLTURawReaddpvt->ind) {
		case AI_CLTU_MCLOCK:
			precord->val = (double)mSys->ctu.dcm_locked;
			if(ioc_debug==3) printf("ctu.dcm_locked: 0x%x (%d)\n", (unsigned int)precord->val, (unsigned int)precord->val);
			break;
		case AI_CLTU_TIME:
#if !WITH_TIMINGBOARD
	mSys->CurrentTime_msb = sysCltuStatus++;
	mSys->CurrentTime_lsb = 0;
#endif
			if( pCfg->u8Version == VERSION_1 ) 
			{
				temp64 = (unsigned long long int)mSys->CurrentTime_msb << 32;
				curtime64 = temp64 + (unsigned long long int)mSys->CurrentTime_lsb;
				precord->val = (double)(curtime64/100000000); 
			} 
			else {

				}
/*			precord->val = (double)sysCltuStatus; */

#if 1
/*			drvCLTU_set_rtc((unsigned int)precord->val); */
			if(ioc_debug==3) printf("current time: %f sec\n", precord->val);
#endif
			
			break;
		case AI_CLTU_OFFSET:
#if !WITH_TIMINGBOARD
	mSys->ltu.compensation_offset1 = sysCltuStatus;
#endif
			precord->val = mSys->ltu.compensation_offset1 * 10; /* unit 10ns */
			if(ioc_debug==3) printf("current offset: %f\n", precord->val);
			break;
		case AI_CLTU_ID:
			if( pCfg->u8Version == VERSION_1 ) 
			{
				precord->val = mSys->ltu.id;
				printf("CLTU R1 id: 0x%X\n", (int)precord->val);
			} 
			else if( pCfg->u8Version == VERSION_2 ) 				
			{
				precord->val = pCfg->xtu_ID;
				printf("CLTU R2 id: 0x%X(%d)\n", (int)precord->val, (int)precord->val);
			}
			break;
		case AI_GET_COMP_NS:			
			if( pCfg->u8Version == VERSION_1 ) 
				precord->val = 0;
			else if( pCfg->u8Version == VERSION_2 )
			{			
				unsigned int rd_tmp;

				if( pCfg->u32Type == XTU_MODE_CTU ) {
					if( is_mmap_ok(pCfg) ) 
					{
						rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_TX_COMP_DELAY     );
						ms100MTick_HtoD(&rd_tmp, &(pCfg->compGetTime.ms), &(pCfg->compGetTime.ns) );
					} else {
#ifdef _CLTU_LINUX_
						int status;
						status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_TX_COMP_DELAY, &rd_tmp);
						if (status == -1) return (-1);
						ms100MTick_HtoD(&rd_tmp, &(pCfg->compGetTime.ms), &(pCfg->compGetTime.ns) );
#endif
					}
				} else {

					if( is_mmap_ok(pCfg) ) 
					{
						rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_RX_COMP_DELAY     );
						ms100MTick_HtoD(&rd_tmp, &(pCfg->compGetTime.ms), &(pCfg->compGetTime.ns) );
					} else {
#ifdef _CLTU_LINUX_
						int status;
						status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_RX_COMP_DELAY, &rd_tmp);
						if (status == -1) return (-1);
						ms100MTick_HtoD(&rd_tmp, &(pCfg->compGetTime.ms), &(pCfg->compGetTime.ns) );
#endif
					}
				}
				precord->val = pCfg->compGetTime.ns;
/*				printf("%s: CMPST_Tick: %d\n", pCfg->taskName, (unsigned int)precord->val); */
			}
			break;
	}
	return (2);
}


LOCAL long devStringinCLTU_init_record(stringinRecord *precord)
{
	devCLTUdpvt *pdevStringinCLTUdpvt = (devCLTUdpvt*) malloc(sizeof(devCLTUdpvt));
	int i;
	
	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pdevStringinCLTUdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s %s",
				   pdevStringinCLTUdpvt->taskName,
				   pdevStringinCLTUdpvt->arg1,
				   pdevStringinCLTUdpvt->arg2);
if( ioc_debug == 2)
	printf("devStringinCLTU arg num: %d: %s %s %s\n",i, pdevStringinCLTUdpvt->taskName, 
								pdevStringinCLTUdpvt->arg1,
								pdevStringinCLTUdpvt->arg2);

/*			pdevStringinCLTUdpvt->pCfg = drvCLTU_get_taskConfig(); */
			pdevStringinCLTUdpvt->pCfg = drvCLTU_find_taskConfig(pdevStringinCLTUdpvt->taskName);
			if(!pdevStringinCLTUdpvt->pCfg) {
				recGblRecordError(S_db_badField, (void*) precord,
						  "devStringinCLTU (init_record) Illegal INP field: task name");
				free(pdevStringinCLTUdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}
			

			break;
		default:
			recGblRecordError(S_db_badField, (void*) precord,
					  "devStringinCLTU (init_record) Illegal INP field");
			free(pdevStringinCLTUdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	if(!strcmp(pdevStringinCLTUdpvt->arg1, STRINGIN_CLTU_TIME_STR))
		pdevStringinCLTUdpvt->ind = STRINGIN_CLTU_TIME;
	else {
		recGblRecordError(S_db_badField, (void*) precord,  "devStringinCLTU (init_record) error argment");
		free(pdevStringinCLTUdpvt);
		precord->udf = TRUE;
		precord->dpvt = NULL;
		return S_db_badField;
	}
	
	precord->udf = FALSE;
	precord->dpvt = (void*) pdevStringinCLTUdpvt;

	return 2;    /* don't convert */ 
}


LOCAL long devStringinCLTU_get_ioint_info(int cmd, stringinRecord *precord, IOSCANPVT *ioScanPvt)
{
	devCLTUdpvt *pdevStringinCLTUdpvt = (devCLTUdpvt*) precord->dpvt;
	ST_drvCLTU   *pCfg;

	if(!pdevStringinCLTUdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pCfg = pdevStringinCLTUdpvt->pCfg;
	*ioScanPvt  = pCfg->ioScanPvt;
	return 0;
}

LOCAL long devStringinCLTU_read_stringin(stringinRecord *precord)
{ 
	devCLTUdpvt	*pdevStringinCLTUdpvt = (devCLTUdpvt*) precord->dpvt;
	ST_drvCLTU *pCfg;

	System_Info* mSys;
	unsigned long long int curtime64, temp64;

	unsigned int day, hour, min, sec;
	unsigned int timer_sec;


	if(!pdevStringinCLTUdpvt) {CLTU_ASSERT(); return -1; }/*returns: (-1,0)=>(failure,success)*/

	pCfg = pdevStringinCLTUdpvt->pCfg;
	
#if WITH_TIMINGBOARD
	if(pCfg->pciDevice.init != 0xFFFFFFFF)
	{
/*		CLTU_ASSERT();		2012. 5. 23 alway print when exit. why??? */
		sprintf(precord->val, "Error!");
		return -1; /*returns: (-1,0)=>(failure,success)*/
	}
	mSys = (System_Info*) (pCfg->pciDevice.base2);
#else
	mSys = (System_Info *)malloc(sizeof(System_Info) ); /* temporary */
#endif

	switch(pdevStringinCLTUdpvt->ind) {
	case STRINGIN_CLTU_TIME:
		if( pCfg->u8Version == VERSION_1 ) 
		{
			
			temp64 = (unsigned long long int)mSys->CurrentTime_msb << 32;
			curtime64 = temp64 + (unsigned long long int)mSys->CurrentTime_lsb;

			timer_sec = curtime64/100000000;
			
			day = (int) (timer_sec/T_DAY_DIVIDER);
			timer_sec = timer_sec - (day * T_DAY_DIVIDER);
			hour = (int) (timer_sec/T_HOUR_DIVIDER );
			timer_sec = timer_sec - (hour * T_HOUR_DIVIDER);
			min = (int) (timer_sec/T_MIN_DIVIDER );
			timer_sec = timer_sec - (min * T_MIN_DIVIDER);
			sec = (int) (timer_sec/T_SEC_DIVIDER );

			sprintf(precord->val, "%02d:%02d:%02d", hour, min, sec);
		}
		else
		{
			ozXTU_TIME_T pxtu_time;
			unsigned int rd_tmp;  

			if( is_mmap_ok(pCfg) ) {
				rd_tmp = READ32(pCfg->pciDevice.base0 + XTU_ADDR_TX_CURR_TIME + 4  );
				hms_HtoD(&rd_tmp,&(pxtu_time.curr.hour),&(pxtu_time.curr.min),&(pxtu_time.curr.sec));
			}
			else {
#ifdef _CLTU_LINUX_
				int status;
				status = ioctl(pCfg->pciDevice.fd, IOCTL_XTU_GET_TX_CURR_TIME_4, &rd_tmp);
				if (status == -1) return WR_ERROR;
				hms_HtoD(&rd_tmp,&(pxtu_time.curr.hour),&(pxtu_time.curr.min),&(pxtu_time.curr.sec));
#endif
				}
/*			xtu_time_print_new(pCfg, xtu_current_time, xtu_current_tick); */
			
			sprintf(precord->val, "%02d:%02d:%02d", pxtu_time.curr.hour, pxtu_time.curr.min, pxtu_time.curr.sec);
/*			sprintf(precord->rval, "%02d:%02d:%02d.%09d", hour, min, sec, xtu_current_tick); */
/*			printf("%s\n", precord->val); */
		}

#if 1
/*		drvCLTU_set_rtc((unsigned int)precord->val); */
		if(ioc_debug==3) printf("current time: %s\n", precord->val);
#endif
			
		break;
	default: 
		printf("\nERROR: %s: precord->dpvt(%d)\n", pCfg->taskName, pdevStringinCLTUdpvt->ind );
		break;
	}
	
	return (0); /*returns: (-1,0)=>(failure,success)*/
	
}

LOCAL long devBoCLTUControl_init_record(boRecord *precord)
{
	devCLTUdpvt *pdevCLTUControldpvt = (devCLTUdpvt*) malloc(sizeof(devCLTUdpvt));
	int cnt;

	precord->udf = TRUE;
	precord->dpvt = NULL;
	
	switch(precord->out.type) {
		case INST_IO:
			strcpy(pdevCLTUControldpvt->recordName, precord->name);
			cnt = sscanf(precord->out.value.instio.string, "%s %s %s", 
				pdevCLTUControldpvt->taskName, pdevCLTUControldpvt->arg1, pdevCLTUControldpvt->arg2);
	if( ioc_debug == 2)
		printf("devBoCLTUControldpvt arg num: %d: %s %s %s\n",cnt, pdevCLTUControldpvt->taskName, 
									pdevCLTUControldpvt->arg1, pdevCLTUControldpvt->arg2);

/*			pdevCLTUControldpvt->pCfg = drvCLTU_get_taskConfig(); */
			pdevCLTUControldpvt->pCfg = drvCLTU_find_taskConfig(pdevCLTUControldpvt->taskName);

			if(!pdevCLTUControldpvt->pCfg) {
				recGblRecordError(S_db_badField, (void*) precord, 
						  "devBoCLTUControl (init_record) Illegal OUT field: task_name");
				free(pdevCLTUControldpvt);
/*				precord->udf = TRUE;
				precord->dpvt = NULL; */
				return S_db_badField;
			}
			break;
		default:
			recGblRecordError(S_db_badField,(void*) precord, "devBoCLTUControl (init_record) Illegal OUT field");
			free(pdevCLTUControldpvt);
/*			precord->udf = TRUE;
			precord->dpvt = NULL; */
			return S_db_badField;
				
	}

	if( cnt == 2) {
		if(!strcmp(pdevCLTUControldpvt->arg1, BO_SHOT_START_STR)) 
			pdevCLTUControldpvt->ind = BO_SHOT_START;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_SHOW_INFO_STR)) 
			pdevCLTUControldpvt->ind = BO_SHOW_INFO;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_SHOT_END_STR)) 
			pdevCLTUControldpvt->ind = BO_SHOT_END;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_SET_FREE_RUN_STR)) 
			pdevCLTUControldpvt->ind = BO_SET_FREE_RUN;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_RUN_CALIBRATION_STR)) 
			pdevCLTUControldpvt->ind = BO_RUN_CALIBRATION;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_USE_REF_CLK_STR)) 
			pdevCLTUControldpvt->ind = BO_USE_REF_CLK;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_USE_MMAP_CTL_STR)) 
			pdevCLTUControldpvt->ind = BO_USE_MMAP_CTL;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_IRIGB_SEL_STR)) 
			pdevCLTUControldpvt->ind = BO_IRIGB_SEL;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_TLKRX_ENABLE_STR)) 
			pdevCLTUControldpvt->ind = BO_TLKRX_ENABLE;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_TLKTX_SEL_STR)) 
			pdevCLTUControldpvt->ind = BO_TLKTX_SEL;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_SHOW_STATUS_STR)) 
			pdevCLTUControldpvt->ind = BO_SHOW_STATUS;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_SHOW_TIME_STR)) 
			pdevCLTUControldpvt->ind = BO_SHOW_TIME;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_SHOW_TLK_STR)) 
			pdevCLTUControldpvt->ind = BO_SHOW_TLK;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_LOG_GSHOT_STR)) 
			pdevCLTUControldpvt->ind = BO_LOG_GSHOT;
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_SET_SHOT_TIME_STR)) 
			pdevCLTUControldpvt->ind = BO_SET_SHOT_TIME;
		
		
		else {
			recGblRecordError(S_db_badField,(void*) precord, "devBoCLTUControl (init_record) Illegal OUT field: arg1" );
			free(pdevCLTUControldpvt);
			return S_db_badField;
		}
	}
	else if( cnt == 3) 
	{
		if( !strcmp(pdevCLTUControldpvt->arg1, BO_SETUP_STR) )
			pdevCLTUControldpvt->ind = BO_SETUP;

		else if( !strcmp(pdevCLTUControldpvt->arg1, BO_PORT_ENABLE_STR) )
			pdevCLTUControldpvt->ind = BO_PORT_ENABLE;
		
		else if( !strcmp(pdevCLTUControldpvt->arg1, BO_PORT_MODE_STR) )
			pdevCLTUControldpvt->ind = BO_PORT_MODE;

		else if( !strcmp(pdevCLTUControldpvt->arg1, BO_PORT_ActiveLow_STR) )
			pdevCLTUControldpvt->ind = BO_PORT_ActiveLow;
		
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_LOG_PORT_STR)) 
			pdevCLTUControldpvt->ind = BO_LOG_PORT;
		
		else if(!strcmp(pdevCLTUControldpvt->arg1, BO_ENABLE_CLK_PERM_STR)) 
			pdevCLTUControldpvt->ind = BO_ENABLE_CLK_PERM;
		

		else {
			recGblRecordError(S_db_badField,(void*) precord, "devBoCLTUControl (init_record) Illegal INP field: arg1" );
			free(pdevCLTUControldpvt);
			return S_db_badField;
		}		
		pdevCLTUControldpvt->nArg0 = atoi(pdevCLTUControldpvt->arg2);
	}
	else{
		recGblRecordError(S_db_badField, (void*) precord,  "devBoCLTUControl (init_record) error argment");
		free(pdevCLTUControldpvt);
		return S_db_badField;
	}
	
	
	precord->udf = FALSE;
	precord->dpvt = (void*) pdevCLTUControldpvt;
		
	return 2;	  /* don't convert */
}


LOCAL long devBoCLTUControl_write_bo(boRecord *precord)
{
#if PRINT_PHASE_INFO
	static int   kkh_cnt;
#endif
	devCLTUdpvt		*pdevCLTUdpvt = (devCLTUdpvt*) precord->dpvt;
	ST_drvCLTU		*pCfg;
	drvCLTU_controlThreadConfig 		*pcontrolThreadConfig;
	controlThreadQueueData	qData;


	if(!pdevCLTUdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1; /*returns: (-1,0)=>(failure,success)*/
	}

	pCfg			= pdevCLTUdpvt->pCfg;
	pcontrolThreadConfig		= pCfg->pcontrolThreadConfig;
	qData.param.pCfg		= pCfg;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue		= (double)precord->val;

	qData.param.nArg0			= pdevCLTUdpvt->nArg0;


        /* db processing: phase I */
 	if(precord->pact == FALSE) {	
		precord->pact = TRUE;
		
#if PRINT_PHASE_INFO
		printf("db processing: phase I %s (%s)\n", precord->name,
				                                epicsThreadGetNameSelf());
#endif
		switch(pdevCLTUdpvt->ind) {
			case BO_SETUP:
				qData.pFunc = devCLTU_BO_SETUP;
				break;
			case BO_SHOT_START:
				qData.pFunc  = devCLTU_BO_SHOT_START;
				break;
			case BO_SHOW_INFO:
				qData.pFunc  = devCLTU_BO_SHOW_INFO;
				break;
				
			case BO_PORT_ENABLE:
				qData.pFunc  = devCLTU_BO_PORT_ENABLE;
				break;
			case BO_PORT_MODE:
				qData.pFunc  = devCLTU_BO_PORT_MODE;
				break;
			case BO_PORT_ActiveLow:
				qData.pFunc  = devCLTU_BO_PORT_ActiveLow;
				break;
			case BO_SHOT_END:
				qData.pFunc  = devCLTU_BO_SHOT_END;
				break;
			case BO_SET_FREE_RUN:
				qData.pFunc  = devCLTU_BO_SET_FREE_RUN;
				break;
			case BO_RUN_CALIBRATION:
				qData.pFunc  = devCLTU_BO_RUN_CALIBRATION;
				break;
			case BO_USE_REF_CLK:
				qData.pFunc  = devCLTU_BO_USE_REF_CLK;
				break;
			case BO_USE_MMAP_CTL:
				qData.pFunc  = devCLTU_BO_USE_MMAP_CTL;
				break;
			case BO_IRIGB_SEL:
				qData.pFunc = devCLTU_BO_IRIGB_SEL;
				break;
			case BO_TLKRX_ENABLE:
				qData.pFunc = devCLTU_BO_TLKRX_ENABLE;
				break;
			case BO_TLKTX_SEL:
				qData.pFunc = devCLTU_BO_TLKTX_SEL;
				break;
			case BO_SHOW_STATUS:
				qData.pFunc = devCLTU_BO_SHOW_STATUS;
				break;
			case BO_SHOW_TIME:
				qData.pFunc = devCLTU_BO_SHOW_TIME;
				break;
			case BO_SHOW_TLK:
				qData.pFunc = devCLTU_BO_SHOW_TLK;
				break;
			case BO_LOG_GSHOT:
				qData.pFunc = devCLTU_BO_LOG_GSHOT;
				break;
			case BO_LOG_PORT:
				qData.pFunc = devCLTU_BO_LOG_PORT;
				break;
			case BO_ENABLE_CLK_PERM:
				qData.pFunc = devCLTU_BO_ENABLE_CLK_PERM;
				break;
			case BO_SET_SHOT_TIME:
				qData.pFunc = devCLTU_BO_SET_SHOT_TIME;
				break;
				
			default: return -1; /*returns: (-1,0)=>(failure,success)*/
		}
		epicsMessageQueueSend(pcontrolThreadConfig->threadQueueId,
				      (void*) &qData,
				      sizeof(controlThreadQueueData));

		return 0; /*returns: (-1,0)=>(failure,success)*/
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {

#if PRINT_PHASE_INFO
		printf("db processing: phase II %s (%s) %d\n", precord->name,
				                                 epicsThreadGetNameSelf(),++kkh_cnt);
#endif
		precord->pact = FALSE;
		precord->udf = FALSE;
#if 0
		switch(pdevACQ196Controldpvt->ind) {
			case AO_SET_INTERCLK: precord->val = 0; break;
			case AO_SET_EXTERCLK: precord->val = 0; break;
			case AO_DAQ_START: precord->val = 0; break;
			case AO_DAQ_STOP: precord->val = 0; break;
			case AO_ADC_START: precord->val = 0; break;
			case AO_ADC_STOP: precord->val = 0; break;
			case AO_LOCAL_STORAGE: precord->val = 0; break;
			case AO_REMOTE_STORAGE: precord->val = 0; break;
			case AO_SET_CLEAR_STATUS: precord->val = 0; break;
		}
#endif
		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1; /*returns: (-1,0)=>(failure,success)*/
}


