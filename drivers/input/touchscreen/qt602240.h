/* file_name: qt602240.h
 *
 * description: Quantum TSP driver.
 *
 * Copyright (C) 2008-2010 Atmel Co. Ltd.
 *
 */

#ifndef _QT602240_H_
#define _QT602240_H_

#define __VER_2_0__

/* -------------------------------------------------------------------- */
/* qt602240 functional option */
/* -------------------------------------------------------------------- */
#define QT_FIRMUP_ENABLE		// qt602240 chipset firmware update enable
#define QT_MULTITOUCH_ENABLE	// qt602240 multi touch enable
#define OPTION_WRITE_CONFIG		// ?

#define SKY_PROCESS_CMD_KEY
#define TOUCH_IO
#ifdef TOUCH_IO
#define CHARGER_MODE
#endif
#define CHIP_NOINIT
#define RECHECK_AFTER_CALIBRATION
#define CHECK_FHE //frequency hopping errors
//#define INT_LEVEL //for froyo; edge trigger doesn't work well. why?
//#define USE_FACE_SUP_SW
//#define USE_FACE_SUP_HW
//#define WRONG_REF_LEVEL //for the wrong reference level of calibration

#if defined(CONFIG_MACH_MSM8X55_EF18K)
#include "qt602240_cfg.h"
#else
#include "qt602240_cfg_ef30s.h"
#endif

#define  U16    unsigned short int 
#define U8      __u8
#define u8      __u8
#define S16     signed short int
#define U16     unsigned short int
#define S32     signed long int
#define U32     unsigned long int
#define S64     signed long long int
#define U64     unsigned long long int
#define F32     float
#define F64     double


#define NUM_OF_TOUCH_OBJECTS	0

typedef enum 
{
    NO_COMMAND = 0u,
    COMM_MODE1 = 1u,
    COMM_MODE2 = 2u,
    COMM_MODE3 = 3u
}comm_cfg;



/* \brief Defines CHANGE line active mode. */
/* \brief Defines CHANGE line active mode. */
#define CHANGELINE_NEGATED          0u
#define CHANGELINE_ASSERTED         1u

/* This sets the I2C frequency to 400kHz (it's a feature in I2C driver that the
   actual value needs to be double that). */
#define I2C_SPEED                   800000u

#define CONNECT_OK                  1u
#define CONNECT_ERROR               2u

#define READ_MEM_OK                 1u
#define READ_MEM_FAILED             2u

#define MESSAGE_READ_OK             1u
#define MESSAGE_READ_FAILED         2u

#define WRITE_MEM_OK                1u
#define WRITE_MEM_FAILED            2u

#define CFG_WRITE_OK                1u
#define CFG_WRITE_FAILED            2u

#define I2C_INIT_OK                 1u
#define I2C_INIT_FAIL               2u

#define CRC_CALCULATION_OK          1u
#define CRC_CALCULATION_FAILED      2u

#define ID_MAPPING_OK               1u
#define ID_MAPPING_FAILED           2u

#define ID_DATA_OK                  1u
#define ID_DATA_NOT_AVAILABLE       2u


enum driver_setup_t {DRIVER_SETUP_OK, DRIVER_SETUP_INCOMPLETE};

/*! \brief Returned by get_object_address() if object is not found. */
#define OBJECT_NOT_FOUND   0u

/*! Address where object table starts at touch IC memory map. */
#define OBJECT_TABLE_START_ADDRESS      7U

/*! Size of one object table element in touch IC memory map. */
#define OBJECT_TABLE_ELEMENT_SIZE       6U

/*! Offset to RESET register from the beginning of command processor. */
#define RESET_OFFSET                    0u

/*! Offset to BACKUP register from the beginning of command processor. */
#define BACKUP_OFFSET       1u

/*! Offset to CALIBRATE register from the beginning of command processor. */
#define CALIBRATE_OFFSET    2u

/*! Offset to REPORTALL register from the beginning of command processor. */
#define REPORTATLL_OFFSET   3u

/*! Offset to DEBUG_CTRL register from the beginning of command processor. */
#define DEBUG_CTRL_OFFSET   4u

/*! Offset to DIAGNOSTIC_CTRL register from the beginning of command processor. */
#define DIAGNOSTIC_OFFSET   5u




//*****************************************************************************
//
//
//		std_objects_driver
//
//
//*****************************************************************************

/*! ===Header File Version Number=== */
#define OBJECT_LIST__VERSION_NUMBER     0x11


#define GEN_MESSAGEPROCESSOR_T5                   5u
#define GEN_COMMANDPROCESSOR_T6                   6u
#define GEN_POWERCONFIG_T7                        7u
#define GEN_ACQUISITIONCONFIG_T8                  8u
#define TOUCH_MULTITOUCHSCREEN_T9                 9u
#define TOUCH_KEYARRAY_T15                        15u
#define PROCI_LINEARIZATIONTABLE_T17              17u
#define SPT_COMCONFIG_T18                         18u
#define SPT_GPIOPWM_T19                           19u
#define PROCI_GRIPFACESUPPRESSION_T20             20u
#define PROCG_NOISESUPPRESSION_T22                22u
#define TOUCH_PROXIMITY_T23                       23u
#define PROCI_ONETOUCHGESTUREPROCESSOR_T24        24u
#define SPT_SELFTEST_T25                          25u
#define DEBUG_CTERANGE_T26                        26u
#define PROCI_TWOTOUCHGESTUREPROCESSOR_T27        27u
#define SPT_CTECONFIG_T28                         28u
#define DEBUG_DIAGNOSTIC_T37                      37u

/*----------------------------------------------------------------------------
  Messages definitions
----------------------------------------------------------------------------*/
#define TOUCH_DETECT		0x80
#define TOUCH_PRESS			0x40
#define TOUCH_RELEASE		0x20
#define TOUCH_MOVE			0x10
#define TOUCH_VECTOR		0x08
#define TOUCH_AMP			0x04
#define TOUCH_SUPPRESS		0x02
#define TOUCH_UNGRIP		0x01

/*
 * All entries spare up to 255
*/
#define RESERVED_T255                             255u

/*! @} */

/*----------------------------------------------------------------------------
  type definitions
----------------------------------------------------------------------------*/

typedef struct
{
   uint8_t reset;       /*!< Force chip reset             */
   uint8_t backupnv;    /*!< Force backup to eeprom/flash */
   uint8_t calibrate;   /*!< Force recalibration          */
   uint8_t reportall;   /*!< Force all objects to report  */
   uint8_t reserve;   /*!< Turn on output of debug data */
   uint8_t diagnostic;  /*!< Controls the diagnostic object */
} gen_commandprocessor_t6_config_t;



typedef struct
{
   uint8_t idleacqint;    /*!< Idle power mode sleep length in ms           */
   uint8_t actvacqint;    /*!< Active power mode sleep length in ms         */
   uint8_t actv2idleto;   /*!< Active to idle power mode delay length in units of 0.2s*/
   
} gen_powerconfig_t7_config_t;




typedef struct
{ 
   uint8_t chrgtime;          /*!< Charge-transfer dwell time             */  
   uint8_t reserved;          /*!< reserved                               */
   uint8_t tchdrift;          /*!< Touch drift compensation period        */
   uint8_t driftst;           /*!< Drift suspend time                     */
   uint8_t tchautocal;        /*!< Touch automatic calibration delay in units of 0.2s*/
   uint8_t sync;              /*!< Measurement synchronisation control    */
   uint8_t atchcalst;         /*!< recalibration suspend time after last detection */
   uint8_t atchcalsthr;       /*!< Anti-touch calibration suspend threshold */
#if defined(__VER_2_0__)
   uint8_t atchfrccalthr;     /*!< Anti-touch force calibration threshold */
   int8_t  atchfrccalration;  /*!< Anti-touch force calibration ratio */  
#endif
} gen_acquisitionconfig_t8_config_t;



typedef struct
{
   /* Screen Configuration */
   uint8_t ctrl;            /*!< ACENABLE LCENABLE Main configuration field  */

   /* Physical Configuration */
   uint8_t xorigin;         /*!< LCMASK ACMASK Object x start position on matrix  */
   uint8_t yorigin;         /*!< LCMASK ACMASK Object y start position on matrix  */
   uint8_t xsize;           /*!< LCMASK ACMASK Object x size (i.e. width)         */
   uint8_t ysize;           /*!< LCMASK ACMASK Object y size (i.e. height)        */

   /* Detection Configuration */
   uint8_t akscfg;          /*!< Adjacent key suppression config     */
   uint8_t blen;            /*!< Sets the gain of the analog circuits in front of the ADC. The gain should be set in
                            conjunction with the burst length to optimize the signal acquisition. Maximum gain values for
                            a given object/burst length can be obtained following a full calibration of the system. GAIN
                            has a maximum setting of 4; settings above 4 are capped at 4.*/
   uint8_t tchthr;          /*!< ACMASK Threshold for all object channels   */
   uint8_t tchdi;           /*!< Detect integration config           */

   uint8_t orient;  /*!< LCMASK Controls flipping and rotating of touchscreen
                        *   object */
   uint8_t mrgtimeout; /*!< Timeout on how long a touch might ever stay
                        *   merged - units of 0.2s, used to tradeoff power
                        *   consumption against being able to detect a touch
                        *   de-merging early */

   /* Position Filter Configuration */
   uint8_t movhysti;   /*!< Movement hysteresis setting used after touchdown */
   uint8_t movhystn;   /*!< Movement hysteresis setting used once dragging   */
   uint8_t movfilter;  /*!< Position filter setting controlling the rate of  */

   /* Multitouch Configuration */
   uint8_t numtouch;   /*!< The number of touches that the screen will attempt
                        *   to track */
   uint8_t mrghyst;    /*!< The hysteresis applied on top of the merge threshold
                        *   to stop oscillation */
   uint8_t mrgthr;     /*!< The threshold for the point when two peaks are
                        *   considered one touch */

   uint8_t amphyst;          /*!< TBD */

  /* Resolution Controls */
  uint16_t xrange;       /*!< LCMASK */
  uint16_t yrange;       /*!< LCMASK */
  uint8_t xloclip;       /*!< LCMASK */
  uint8_t xhiclip;       /*!< LCMASK */
  uint8_t yloclip;       /*!< LCMASK */
  uint8_t yhiclip;       /*!< LCMASK */
  /* edge correction controls */
  uint8_t xedgectrl;     /*!< LCMASK */
  uint8_t xedgedist;     /*!< LCMASK */
  uint8_t yedgectrl;     /*!< LCMASK */
  uint8_t yedgedist;     /*!< LCMASK */
  uint8_t jumplimit;
#if defined(__VER_2_0__)
  uint8_t tchhyst;       /* Touch threshold hysteresis */
#endif
} touch_multitouchscreen_t9_config_t;



typedef struct
{
   /* Key Array Configuration */
   uint8_t ctrl;           /*!< ACENABLE LCENABLE Main configuration field           */

   /* Physical Configuration */
   uint8_t xorigin;        /*!< ACMASK LCMASK Object x start position on matrix  */
   uint8_t yorigin;        /*!< ACMASK LCMASK Object y start position on matrix  */
   uint8_t size;           /*!< ACMASK LCMASK Object x size (i.e. width)         */

   /* Detection Configuration */
   uint8_t akscfg;         /*!< Adjacent key suppression config     */
   uint8_t blen;           /*!< ACMASK Burst length for all object channels*/
   uint8_t tchthr;         /*!< ACMASK Threshold for all object channels   */
   uint8_t tchdi;          /*!< Detect integration config           */
   uint8_t reserved[2];    /*!< Spare x2 */
   uint8_t movhysti;       /*!< Movement hysteresis setting used after touchdown */
   uint8_t movhystn;       /*!< Movement hysteresis setting used once dragging */
   uint8_t movfilter;      /*!< Position filter setting controlling the rate of  */

} touch_slider_wheel_t11_t12_t13_t14_config_t;



typedef struct
{
   /* Key Array Configuration */
   uint8_t ctrl;               /*!< ACENABLE LCENABLE Main configuration field           */

   /* Physical Configuration */
   uint8_t xorigin;           /*!< ACMASK LCMASK Object x start position on matrix  */
   uint8_t yorigin;           /*!< ACMASK LCMASK Object y start position on matrix  */
   uint8_t xsize;             /*!< ACMASK LCMASK Object x size (i.e. width)         */
   uint8_t ysize;             /*!< ACMASK LCMASK Object y size (i.e. height)        */

   /* Detection Configuration */
   uint8_t akscfg;             /*!< Adjacent key suppression config     */
   uint8_t blen;               /*!< ACMASK Burst length for all object channels*/
   uint8_t tchthr;             /*!< ACMASK LCMASK Threshold for all object channels   */
   uint8_t tchdi;              /*!< Detect integration config           */
   uint8_t reserved[2];        /*!< Spare x2 */

} touch_keyarray_t15_config_t;



typedef struct
{
 uint8_t ctrl;
 uint16_t xoffset;
 uint8_t  xsegment[16];
 uint16_t yoffset;
 uint8_t  ysegment[16];

} proci_linearizationtable_t17_config_t;



typedef struct
{
    uint8_t  ctrl;
    uint8_t  cmd;
    
} spt_comcconfig_t18_config_t;


typedef struct
{
   /* GPIOPWM Configuration */
   uint8_t ctrl;             /*!< Main configuration field           */
   uint8_t reportmask;       /*!< Event mask for generating messages to
                              *   the host */
   uint8_t dir;              /*!< Port DIR register   */
   uint8_t intpullup;        /*!< Port pull-up per pin enable register */
   uint8_t out;              /*!< Port OUT register*/
   uint8_t wake;             /*!< Port wake on change enable register  */
   uint8_t pwm;              /*!< Port pwm enable register    */
   uint8_t period;           /*!< PWM period (min-max) percentage*/
   uint8_t duty[4];          /*!< PWM duty cycles percentage */
   uint8_t trigger[4];   
} spt_gpiopwm_t19_config_t;



typedef struct
{
   uint8_t ctrl;
   uint8_t xlogrip;
   uint8_t xhigrip;
   uint8_t ylogrip;
   uint8_t yhigrip;
   uint8_t maxtchs;
   uint8_t reserved;
   uint8_t szthr1;
   uint8_t szthr2;
   uint8_t shpthr1;
   uint8_t shpthr2;
   uint8_t supextto;

} proci_gripfacesuppression_t20_config_t;


typedef struct
{
   uint8_t ctrl;
   uint8_t reserved;
   uint8_t reserved1;
   int16_t gcaful;
   int16_t gcafll;
   uint8_t actvgcafvalid;        /* LCMASK */
   uint8_t noisethr;
   uint8_t reserved2;
   uint8_t freqhopscale;
   uint8_t freq[5u];             /* LCMASK ACMASK */
   uint8_t idlegcafvalid;        /* LCMASK */

} procg_noisesuppression_t22_config_t;



typedef struct
{
   /* Prox Configuration */
   uint8_t ctrl;               /*!< ACENABLE LCENABLE Main configuration field           */

   /* Physical Configuration */
   uint8_t xorigin;           /*!< ACMASK LCMASK Object x start position on matrix  */
   uint8_t yorigin;           /*!< ACMASK LCMASK Object y start position on matrix  */
   uint8_t xsize;             /*!< ACMASK LCMASK Object x size (i.e. width)         */
   uint8_t ysize;             /*!< ACMASK LCMASK Object y size (i.e. height)        */
   uint8_t reserved_for_future_aks_usage;

   /* Detection Configuration */
   uint8_t blen;               /*!< ACMASK Burst length for all object channels*/
   uint16_t fxddthr;             /*!< Fixed detection threshold   */
   uint8_t fxddi;              /*!< Fixed detection integration  */
   uint8_t average;            /*!< Acquisition cycles to be averaged */
   uint16_t mvnullrate;               /*!< Movement nulling rate */
   uint16_t mvdthr;               /*!< Movement detection threshold */
   #if 0
   uint16_t tchthr;             /*!< LCMASK Threshold    */
   uint8_t tchdi;              /*!< Detect integration config           */
   uint8_t average;            /*!< LCMASK Sets the filter length on the prox signal */
   uint16_t rate;               /*!< Sets the rate that prox signal must exceed */
   #endif
} touch_proximity_t23_config_t;



typedef struct
{
   uint8_t  ctrl;
   uint8_t  numgest;
   uint16_t gesten;
   uint8_t  pressproc;
   uint8_t  tapto;
   uint8_t  flickto;
   uint8_t  dragto;
   uint8_t  spressto;
   uint8_t  lpressto;
   uint8_t  reppressto;
   uint16_t flickthr;
   uint16_t dragthr;
   uint16_t tapthr;
   uint16_t throwthr;

} proci_onetouchgestureprocessor_t24_config_t;



typedef struct
{
   uint16_t upsiglim;              /* LCMASK */
   uint16_t losiglim;              /* LCMASK */

} siglim_t;

/*! = Config Structure = */

typedef struct
{
  uint8_t  ctrl;                 /* LCENABLE */
  uint8_t  cmd;
#if(NUM_OF_TOUCH_OBJECTS)
  siglim_t siglim[NUM_OF_TOUCH_OBJECTS];   /* LCMASK */
#endif
} spt_selftest_t25_config_t;


typedef struct
{
    uint8_t  ctrl;          /*!< Bit 0 = object enable, bit 1 = report enable */
    uint8_t  numgest;       /*!< Runtime control for how many two touch gestures to process */
    uint8_t reserved2;
    uint8_t gesten;        /*!< Control for turning particular gestures on or off */
    uint8_t  rotatethr;
    uint16_t zoomthr;

} proci_twotouchgestureprocessor_t27_config_t;


typedef struct
{
   uint8_t ctrl;          /*!< Ctrl field reserved for future expansion */
   uint8_t cmd;           /*!< Cmd field for sending CTE commands */
   uint8_t mode;          /*!< LCMASK CTE mode configuration field */
   uint8_t idlegcafdepth; /*!< LCMASK The global gcaf number of averages when idle */
   uint8_t actvgcafdepth; /*!< LCMASK The global gcaf number of averages when active */
   uint8_t voltage;

} spt_cteconfig_t28_config_t;


typedef struct
{
    uint8_t mode;
    uint8_t page;
    uint8_t data[128];
    
} debug_diagnositc_t37_t;

typedef struct
{
    uint8_t mode;
    uint8_t page;
    int8_t data[128];
    
} debug_diagnositc_t37_delta_t;

typedef struct
{
    uint8_t mode;
    uint8_t page;
    uint16_t data[64];
    
} debug_diagnositc_t37_reference_t;

typedef struct
{
    uint8_t mode;
    uint8_t page;
    uint8_t data[128];
    
} debug_diagnositc_t37_cte_t;



//*****************************************************************************
//
//
//		info_block_driver
//
//
//*****************************************************************************

/*! \brief Object table element struct. */
typedef struct
{
	uint8_t object_type;     /*!< Object type ID. */
	uint16_t i2c_address;    /*!< Start address of the obj config structure. */
	uint8_t size;            /*!< Byte length of the obj config structure -1.*/
	uint8_t instances;       /*!< Number of objects of this obj. type -1. */
    uint8_t num_report_ids;  /*!< The max number of touches in a screen,
                              *   max number of sliders in a slider array, etc.*/
} object_t;


/*! \brief Info ID struct. */
typedef struct
{
	uint8_t family_id;            /* address 0 */
	uint8_t variant_id;           /* address 1 */
	
	uint8_t version;              /* address 2 */
	uint8_t build;                /* address 3 */
	
	uint8_t matrix_x_size;        /* address 4 */
	uint8_t matrix_y_size;        /* address 5 */
	
								  /*! Number of entries in the object table. The actual number of objects
    * can be different if any object has more than one instance. */
	uint8_t num_declared_objects; /* address 6 */
} info_id_t;

#endif //_QT602240_H_
