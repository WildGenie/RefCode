#ifndef _NB1A_DRV_NIDAQ_H
#define _NB1A_DRV_NIDAQ_H

#include "statCommon.h"

#include <NIDAQmx.h>

// --------------------------------------------------------
// Macro for compile
// --------------------------------------------------------
// There is no command for new tree
// When system is armed, tree is created automatically
#define	AUTO_CREATE_TREE			0			

// NBI only : if mode is local then SYS_RUN is not required
#define	AUTO_RUN				0			

// NBI only : SYS_ARMING (for trending), SYS_RUN(for calcuration & storing)
#define	TREND_ENABLE				1			

// converts scaling eu value to raw value of 16bits resolution
#define	USE_SCALING_VALUE			1			

#define BUFLEN  					0x400000    /* @@todo read from device */

// signal type
#define NBI_INPUT_TYPE_DT_STR			"Delta-T"	// Delta T
#define NBI_INPUT_TYPE_TC_STR			"T/C"		// Thermocouple
#define DENSITY_CALC_STR			"Density_Calc"	// Density calc
#define MDS_PARAM_PUT_STR			"MDS_Param"	// MDS Parameter value put in MDSplus DB
#define STRAIN_STR				"STRAIN"	// Strain Data

#define NBI_POWER_CONST				0.0697
#define	NBI_TEMP_PER_VOLT			4	// 0.25V/.C

#define	WAVE_RAW_VALUE_STR			"wave_raw"
#define	WAVE_VALUE_STR				"wave_calc"
// input PV
#define AI_READ_STATE_STR			"state"
#define	AI_RAW_VALUE_STR			"raw_value"
#define	AI_VALUE_STR				"value"
#define	AI_POWER_STR				"power"

#define	AI_DENSITY_STR				"density"
#define AI_DAQ_MIN_VOLT_R_STR                   "min_val_r"
#define AI_DAQ_MAX_VOLT_R_STR                   "max_val_r"
#define AI_DAQ_SAMPLING_TIME_R_STR              "smp_time_r"
#define AI_DAQ_SAMPLING_RATE_R_STR              "smp_rate_r"

#define AI_READ_STATE				1001
#define	AI_RAW_VALUE				1002
#define	AI_VALUE				1003
#define	AI_POWER				1004

#define AI_DAQ_MIN_VOLT_R                	1005
#define AI_DAQ_MAX_VOLT_R                	1006
#define AI_DAQ_SAMPLING_TIME_R                  1007
#define AI_DAQ_SAMPLING_RATE_R                  1008
#define	AI_DENSITY				1009

#define	WAVE_VALUE				1010
#define	WAVE_RAW_VALUE				1011

// output PV
#define AO_DAQ_BEAM_PLUSE_STR 			"beam_pulse"
#define AO_DAQ_BEAM_PLUSE_T0_STR        	"beam_pulse_t0"
#define AO_DAQ_BEAM_PLUSE_T1_STR        	"beam_pulse_t1"
#define AO_INTEGRAL_TIME_STR	 		"integral_time"
#define AO_FLOW_STR			 	"flow"
// TG.Lee
#define AO_DAQ_MIN_VOLT_STR                     "min_val"
#define AO_DAQ_MAX_VOLT_STR                     "max_val"
#define AO_DAQ_SAMPLING_RATE_STR                "smp_rate"
#define AO_DAQ_SAMPLING_TIME_STR                "smp_time"

#define AO_DENSITY_FRINGE_STR			"fringe"
#define AO_DENSITY_LIMITE_UP_STR		"dlimup"
#define AO_DENSITY_LIMITE_LOW_STR		"dlimlow"
#define AO_DENSITY_CONSTANT_A_STR		"cona"
#define AO_DENSITY_CONSTANT_B_STR		"conb"
#define AO_DENSITY_CONSTANT_C_STR		"conc"
#define AO_DENSITY_CONSTANT_D_STR		"cond"
// MDSplus Node parameter value setting each ECH Channel
#define AO_MDS_PARAM_VALUE1_STR			"gain_ch"
#define AO_MDS_PARAM_VALUE2_STR			"offset_ch"
#define AO_MDS_PARAM_VALUE3_STR			"a_ch"
#define AO_MDS_PARAM_VALUE4_STR			"b_ch"
#define AO_MDS_PARAM_VALUE5_STR			"c_ch"

#define BO_AUTO_RUN_STR                         "auto_run"
#define BO_TREND_RUN_STR			"trend_run"
#define BO_TREND_MODE_RUNSTOP_STR		"trend_runstop"
#define BO_CALC_RUN_STR				"calc_run"
#define BO_DAQ_STOP_STR				"daq_stop"

#define AO_DAQ_BEAM_PLUSE_T0                    1001
#define AO_DAQ_BEAM_PLUSE_T1                    1002
#define AO_DAQ_BEAM_PLUSE 			1003
#define AO_INTEGRAL_TIME	 		1101
#define AO_FLOW				 	1102
// TG.Lee
#define AO_DAQ_MIN_VOLT                         1103
#define AO_DAQ_MAX_VOLT                         1104
#define AO_DAQ_SAMPLING_RATE                    1105
#define AO_DAQ_SAMPLING_TIME                    1106

#define AO_DENSITY_FRINGE			1107	
#define AO_DENSITY_LIMITE_UP			1108
#define AO_DENSITY_LIMITE_LOW			1109
#define AO_DENSITY_CONSTANT_A			1110
#define AO_DENSITY_CONSTANT_B			1111
#define AO_DENSITY_CONSTANT_C			1112
#define AO_DENSITY_CONSTANT_D			1113
// MDSplus Node parameter value setting each ECH Channel
#define AO_MDS_PARAM_VALUE1			1114	
#define AO_MDS_PARAM_VALUE2			1115
#define AO_MDS_PARAM_VALUE3	   		1116
#define AO_MDS_PARAM_VALUE4			1117
#define AO_MDS_PARAM_VALUE5			1118

#define BO_AUTO_RUN                             3001
#define BO_TREND_RUN				3002
#define BO_CALC_RUN				3003
#define BO_DAQ_STOP				3004

// PVs
#define PV_LOCAL_SHOT_NUMBER_STR 	"VVS_SHOT_NUMBER"
#define PV_SYS_ARMING_STR 		"VVS_SYS_ARMING"
#define PV_SYS_RUN_STR			"VVS_SYS_RUN"
#define PV_LTU_TIG_T0_STR		"QDS_LTU_P6_SEC0_T0"
#define PV_START_TIG_T0_STR		"VVS_START_T0_SEC1"
#define PV_STOP_TIG_T1_STR		"VVS_STOP_T1_SEC1"
#define PV_RESET_STR 			"VVS_RESET"

#define PV_CALC_RUN_STR 		"VVS_CALC_RUN"
#define PV_TREND_RUN_STR 		"VVS_TREND_RUN"
#define PV_DAQ_STOP_STR 		"VVS_DAQ_STOP"

// PVs for ECH param
#define PV_ECH_POL_PH1_STR		"ECH_POL_ANG_PHI1_MDS"
#define PV_ECH_POL_PH2_STR		"ECH_POL_ANG_PHI2_MDS"
#define PV_ECH_ANT_POL_STR		"ECH_ANT_ANG_POL_MDS"
#define PV_ECH_ANT_TOR_STR		"ECH_ANT_ANG_TOR_MDS"

// initial value
#define INIT_CHECK_INTERVAL		1       /* Interval for Check the Configuration in run as second */
#define INIT_MAX_CHAN_NUM		32	/* 32ch - 6254, 8ch-DAQ Max Channel number of DAQ unit ,but mmwi used from 4ch to 2ch because Not Ready FIR , SCXI for STRAIN = 8x4=32  */	
#define INIT_SAMPLE_RATE		10000		// Hz
#define INIT_SAMPLE_RATE_LIMIT		10000		// Hz, for NI6220 Device. It's depends on device and channel 
#define INIT_MAX_CHAN_SAMPLES  		500000  /* first value : 100,000 but PXI-DAQ Max Rate : 500,000 Hz. so number of sampling data per channel  */
#define INIT_TOT_EVENT_SAMPLES		(INIT_MAX_CHAN_NUM * INIT_MAX_CHAN_SAMPLES)

#define INIT_LOW_LIMIT				-10.0
#define INIT_HIGH_LIMIT				10.0
#define INIT_LOW_LIMIT_STRAIN			-0.015
#define INIT_HIGH_LIMIT_STRAIN			0.015
#define READ_TIME_OUT				10.0

#define	I18_FULL_SCALE				262144.0
#define	I16_FULL_SCALE				65536.0
#define	I14_FULL_SCALE				16384.0
#define EU_GRADIENT				((INIT_HIGH_LIMIT - INIT_LOW_LIMIT) / I16_FULL_SCALE)
#define EU_GRADIENT14				((INIT_HIGH_LIMIT - INIT_LOW_LIMIT) / I14_FULL_SCALE)
#define EU_GRADIENT18				((INIT_HIGH_LIMIT_STRAIN - INIT_LOW_LIMIT_STRAIN) / I18_FULL_SCALE)
#define RAW_GRADIENT				(I16_FULL_SCALE / (INIT_HIGH_LIMIT - INIT_LOW_LIMIT))
#define RAW14_GRADIENT				(I14_FULL_SCALE / (INIT_HIGH_LIMIT - INIT_LOW_LIMIT))
#define RAW18_GRADIENT				(I18_FULL_SCALE / (INIT_HIGH_LIMIT_STRAIN - INIT_LOW_LIMIT_STRAIN))
#define GET_RAW_VALUE(x)			((x)*(RAW_GRADIENT))
#define GET_RAW14_VALUE(x)			((x)*(RAW14_GRADIENT))
#define GET_RAW18_VALUE(x)			((x)*(RAW18_GRADIENT))

#define NIERROR(errno)	NI_err_message(__FILE__,__LINE__,errno)
#define GET_CHID(idx)	(idx % INIT_MAX_CHAN_NUM)

typedef short	intype;					// input type

// content of inttype
enum {
	OTHER_INPUT_NONE	= 0,
	NBI_INPUT_DELTA_T	= 1,
	NBI_INPUT_TC		= 2,
	DENSITY_CALC		= 3,
	MDS_PARAM_PUT		= 4,
	STRAIN_DATA		= 5
};

typedef struct {
	int			used;					// 0 : Not used, 1 : Used

	// st.cmd
	int			inputType;				// type of input : 1(Delta-T), 2(T/C), 3(density Calc output Channel -if ch1 & ch2 -> only select mark ch1)
	char		physical_channel[40];		// Dev1/ai0
	char		physical_channelSCXI[40];	// MUX -MultiMode  SC1Mod1/ai0

	// for MDSplus
	char		inputTagName[40];		// createChannel() in st.cmd
	char		strTagName[40];			// \B0_CH00:FOO
	char		file_name[40];			// B0_CH00
	char		path_name[100];			// directory path and file name for raw data
	char		path_den_name[100];     // directory path and file name for density data
	FILE		*write_fp;

	// input from DAQ
	intype		rawValue;

	// with OPI
	float64		flow;					// ao : PV.FLOW
	float64		integral_time;			// ao : PV.TIME
	float64		integral_cnt;			// integral_time * sample_rate
	float64		euValue;			// ai : PV.VAL
	float64		power;				// ai : PV.POWER

	// Density
	FILE		*write_den_fp;			// file for density
	float64		selectCh;				// last event calc density selectCh for remember channel 
	float64		isFirst;				// Calc function is first time ? check. default value is 0; 
	float64		density;				// current density value
	float64		fringe;				    // fringe reference value
	float64		dLimUpper;				// density Upper limite value 
	float64		dLimLow;				// density Low limite value
	float64		conA;					// density Constant A value
	float64		conB;					// density Constant B value
	float64		conC;					// density Constant C value
	float64		conD;					// density Constant D value
	float64		avgNES1;				// voltage average ch#1 - 1:1000
	float64		avgNES2;				// vlotage average ch#2 - 1:1000
	float64		calcDensity[INIT_MAX_CHAN_SAMPLES];	        // buffer for density data
	float64		fScaleReadChannel[INIT_MAX_CHAN_SAMPLES];	// buffer for scaling sampling data of channel data
	// Gain and offset of HighVoltage Probe for ECH DAQ system 20120202
	float64		mdsParam1; // gain
	float64		mdsParam2; // offset
	float64		mdsParam3; // calA
	float64		mdsParam4; // calB
	float64		mdsParam5; // calC
	// compensation
	int16		offset;					// base offset for compensation

	// temporary
	float64		accum_cnt;				// accumulate of integral time
	float64		accum_temp;				// accumulate of Delta-T(offsetCompRawValue)
	float64		coefficient;			// Q(lpm) * C1 / T(Pulse length)
} ST_NIDAQ_channel;

typedef struct {
	TaskHandle	taskHandle;
	char		triggerSource[30];		// /Dev1/PFI0

	// SFW
	int32		smp_mode;
	float64		start_t0;				// $(SYS)_START_T0_SEC1
	float64		stop_t1;				// $(SYS)_STOP_T1_SEC1
	float64		sample_rate;
	float64		sample_rateLimit;
	float64		sample_time;			// stop_t1 - start_t0

	// NBI
	float64		beam_pulse;				// $(SYS)_BEAM_PULSE
	float64         beam_pulse_t0;                  // $(SYS)_BEAM_PULSE_T0
    float64         beam_pulse_t1;                  // $(SYS)_BEAM_PULSE_T1

	// DAQmx
	int32		terminal_config;		// DAQmx_Val_RSE, DAQmx_Val_Diff, DAQmx_Val_NRSE
	int32		units;
	float64		minVal;
	float64		maxVal;


	// Event
	int32		totalRead;

	int		auto_run_flag;			// any mode auto_run_flag enable/disable select. 
	int		protect_stop_flag;		// protect device stop from we don't know seqence. 
	int		trend_run_flag;			// want to trend mode - 1 , trend mode disable/shot mode - 0 in OPI, by user

	ST_NIDAQ_channel ST_Ch[INIT_MAX_CHAN_NUM];
	int32		readArray[INIT_TOT_EVENT_SAMPLES];		// 18bit for change tye from short to int buffer for raw sampling data
	float64		fScaleReadArray[INIT_TOT_EVENT_SAMPLES];	// buffer for scaling sampling data

	ELLLIST		*data_buf; 
} ST_NIDAQ;

/******* make for RING buffer   ************************************/
typedef struct {
	ELLNODE			node;
	/* IMPORTANT...!!!!    should be adjusted your local device!!!!!!!!! */
	char			data[ BUFLEN ];  
} ST_buf_node;

typedef struct {
	ST_buf_node		*pNode;
	unsigned int	size;
	char			opcode;
} ST_User_Buff_node;
/****************************************************/

extern float64 getTotSampsChan (ST_NIDAQ *pNIDAQ);
extern float64 getStartTime (ST_STD_device *pSTDdev);
extern float64 getEndTime (ST_STD_device *pSTDdev);

extern int stopDevice (ST_STD_device *pSTDdev);
extern int resetDevice (ST_STD_device *pSTDdev);
extern int processDensity (ST_NIDAQ *pNIDAQ, int chNo);
extern int resetDeviceAll ();

extern int processTrendRun ();
extern int processCalcRun ();

#endif	// _NB1A_DRV_NIDAQ_H
