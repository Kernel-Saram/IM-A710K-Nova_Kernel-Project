/* -------------------------------------------------------------------- */
/* EF18S gpio, vreg & resolution */
/* -------------------------------------------------------------------- */
#ifdef QT_MULTITOUCH_ENABLE
#define MAX_NUM_FINGER  5
#else
#define MAX_NUM_FINGER  1
#endif

#define SCREEN_RESOLUTION_X     480
#define SCREEN_RESOLUTION_Y     800

/* -------------------------------------------------------------------- */
/* DEVICE   : mxT224 CONFIGURATION */
/* -------------------------------------------------------------------- */
#define GPIO_TOUCH_CHG			112
#define GPIO_TOUCH_RST			166
#define VDD_TOUCH_ID			"gp9"
#define VDD_TOUCH_MV			2700

/* _SPT_USERDATA_T38 INSTANCE 0 */
#define T7_IDLEACQINT                       32
#define T7_ACTVACQINT                      16
#define T7_ACTV2IDLETO                     20

/* _GEN_ACQUISITIONCONFIG_T8 INSTANCE 0 */
#define T8_CHRGTIME                           10
#define T8_ATCHDRIFT                          0	//5
#define T8_TCHDRIFT                            5	//20
#define T8_DRIFTST                              5	//20
#define T8_TCHAUTOCAL                      0
#define T8_SYNC                                   0
#define T8_ATCHCALST                         0
#define T8_ATCHCALSTHR                    15	//0
#if defined(__VER_2_0__)
#define T8_ATCHFRCCALTHR                100	//50         /* V2.0 추가 */
#define T8_ATCHFRCCALRATIO            50	//25         /* V2.0 추가 */
#endif

/* _TOUCH_MULTITOUCHSCREEN_T9 INSTANCE 0 */
#define T9_CTRL                                     0x8f
#define T9_XORIGIN                              0
#define T9_YORIGIN                              0
#define T9_XSIZE                                    19
#define T9_YSIZE                                    11
#define T9_AKSCFG                                 0
#define T9_BLEN                                     0x11
#define T9_TCHTHR                                45
#define T9_TCHDI                                   2
#define T9_ORIENT                                  1
#define T9_MRGTIMEOUT                        0
#define T9_MOVHYSTI                             10
#define T9_MOVHYSTN                            2
#define T9_MOVFILTER                            30
#define T9_NUMTOUCH                            MAX_NUM_FINGER
#define T9_MRGHYST                               10
#define T9_MRGTHR                                 10
#define T9_AMPHYST                               10
#define T9_XRANGE                                  (874-1)    // 898 = (97.65/87.1) * 800    
#define T9_YRANGE                                  (480-1)    
#define T9_XLOCLIP                                 0
#define T9_XHICLIP                                  0
#define T9_YLOCLIP                                 3
#define T9_YHICLIP                                  3
#define T9_XEDGECTRL                            0
#define T9_XEDGEDIST                             0
#define T9_YEDGECTRL                            0
#define T9_YEDGEDIST                            0
#define T9_JUMPLIMIT                            20
#if defined(__VER_2_0__)
#define T9_TCHHYST                               (T9_TCHTHR/4)       /* V2.0 추가 */
#endif

/* [TOUCH_KEYARRAY_T15 INSTANCE 0]    */
#define T15_CTRL                                    0
#define T15_XORIGIN                             0
#define T15_YORIGIN                             0
#define T15_XSIZE                                  0
#define T15_YSIZE                                  0
#define T15_AKSCFG                              0
#define T15_BLEN                                   0
#define T15_TCHTHR                              0
#define T15_TCHDI                                 0
#define T15_RESERVED_0                      0
#define T15_RESERVED_1                      0

/*  [SPT_COMMSCONFIG_T18 INSTANCE 0]        */
#define T18_CTRL                                    0
#define T18_COMMAND                           1

/* _SPT_GPIOPWM_T19 INSTANCE 0 */
#define T19_CTRL                                    0
#define T19_REPORTMASK                      0
#define T19_DIR                                      0
#define T19_INTPULLUP                         0
#define T19_OUT                                     0
#define T19_WAKE                                  0
#define T19_PWM                                    0
#define T19_PERIOD                              0
#define T19_DUTY_0                              0
#define T19_DUTY_1                              0
#define T19_DUTY_2                              0
#define T19_DUTY_3                              0
#define T19_TRIGGER_0                         0
#define T19_TRIGGER_1                         0
#define T19_TRIGGER_2                         0
#define T19_TRIGGER_3                         0

/* _PROCI_GRIPFACESUPPRESSION_T20 INSTANCE 0 */
#define T20_CTRL                                    0
#define T20_XLOGRIP                             0
#define T20_XHIGRIP                              0
#define T20_YLOGRIP                             0
#define T20_YHIGRIP                              0
#define T20_MAXTCHS                             0
#define T20_RESERVED_0                       0
#define T20_SZTHR1                                0
#define T20_SZTHR2                                0
#define T20_SHPTHR1                             0
#define T20_SHPTHR2                             0
#define T20_SUPEXTTO                           0

/* _PROCG_NOISESUPPRESSION_T22 INSTANCE 0 */
#define T22_CTRL                                    13
#define T22_RESERVED_0                      0
#define T22_RESERVED_1                      0
#define T22_GCAFUL                               25
#define T22_GCAFLL                               -25
#define T22_ACTVGCAFVALID                4
#define T22_NOISETHR                          25
#define T22_RESERVED_2                      0
#define T22_FREQHOPSCALE                 0
#define T22_FREQ_0                              15
#define T22_FREQ_1                              24
#define T22_FREQ_2                              27
#define T22_FREQ_3                              42
#define T22_FREQ_4                              45
#define T22_IDLEGCAFVALID                4

/* [TOUCH_PROXIMITY_T23 INSTANCE 0] */
#define T23_CTRL                                   0
#define T23_XORIGIN                            0
#define T23_YORIGIN                            0
#define T23_XSIZE                                  0
#define T23_YSIZE                                  0
#define T23_RESERVED                          0
#define T23_BLEN                                   0
#define T23_FXDDTHR                            0
#define T23_FXDDI                                 0
#define T23_AVERAGE                             0
#define T23_MVNULLRATE                      0
#define T23_MVDTHR                              0

/* T24_[PROCI_ONETOUCHGESTUREPROCESSOR_T24 INSTANCE 0] */
#define T24_CTRL                                   0
#define T24_NUMGEST                           0
#define T24_GESTEN                              0
#define T24_PROCESS                            0
#define T24_TAPTO                                 0
#define T24_FLICKTO                             0
#define T24_DRAGTO                              0
#define T24_SPRESSTO                          0
#define T24_LPRESSTO                          0
#define T24_REPPRESSTO                      0
#define T24_FLICKTHR                            0
#define T24_DRAGTHR                            0
#define T24_TAPTHR                               0
#define T24_THROWTHR                         0

/* [SPT_SELFTEST_T25 INSTANCE 0] */
#define T25_CTRL                                    0
#define T25_CMD                                     0
#define T25_SIGLIM_0_UPSIGLIM        13500
#define T25_SIGLIM_0_LOSIGLIM        5500
#define T25_SIGLIM_1_UPSIGLIM        13500
#define T25_SIGLIM_1_LOSIGLIM        5500
#define T25_SIGLIM_2_UPSIGLIM        0
#define T25_SIGLIM_2_LOSIGLIM        0

/* [PROCI_TWOTOUCHGESTUREPROCESSOR_T27 INSTANCE 0] */
#define T27_CTRL                                    0
#define T27_NUMGEST                            0
#define T27_RESERVED_0                      0
#define T27_GESTEN                               0
#define T27_ROTATETHR                        0
#define T27_ZOOMTHR                           0

/* _SPT_CTECONFIG_T28 INSTANCE 0 */
#define T28_CTRL                                    1
#define T28_CMD                                     0
#define T28_MODE                                   3
#define T28_IDLEGCAFDEPTH                8
#define T28_ACTVGCAFDEPTH               12
#define T28_VOLTAGE                     60 

#ifdef CHARGER_MODE
typedef enum
{  
    CHARGER_UNPLUGGED_TCHTHR = T9_TCHTHR,
    CHARGER_UNPLUGGED_TCHDI = T9_TCHDI,
    CHARGER_UNPLUGGED_IDLEGCAFDEPTH = T28_IDLEGCAFDEPTH,
    CHARGER_UNPLUGGED_ACTVGCAFDEPTH = T28_ACTVGCAFDEPTH,
    CHARGER_UNPLUGGED_NOISETHR = T22_NOISETHR,
    CHARGER_UNPLUGGED_IDLEACQINT = T7_IDLEACQINT,  
    CHARGER_UNPLUGGED_ACTVACQINT = T7_ACTVACQINT,

    CHARGER_PLUGGED_TCHTHR = 50,
    CHARGER_PLUGGED_TCHDI = 3,
    CHARGER_PLUGGED_IDLEGCAFDEPTH = 16,
    CHARGER_PLUGGED_ACTVGCAFDEPTH = 32,
    CHARGER_PLUGGED_NOISETHR = 30,
    CHARGER_PLUGGED_IDLEACQINT = 255,
    CHARGER_PLUGGED_ACTVACQINT = 255,
} CHARGER_THR;
#endif //CHARGER_MODE

#ifdef CHECK_FHE
typedef enum
{
    FHE_CLEAR_TCHTHR = CHARGER_UNPLUGGED_TCHTHR,
    FHE_CLEAR_TCHDI = CHARGER_UNPLUGGED_TCHDI,
    FHE_CLEAR_IDLEGCAFDEPTH = CHARGER_UNPLUGGED_IDLEGCAFDEPTH,
    FHE_CLEAR_ACTVGCAFDEPTH = CHARGER_UNPLUGGED_ACTVGCAFDEPTH,
    FHE_CLEAR_NOISETHR = CHARGER_UNPLUGGED_NOISETHR,
    FHE_CLEAR_IDLEACQINT = CHARGER_UNPLUGGED_IDLEACQINT, 
    FHE_CLEAR_ACTVACQINT = CHARGER_UNPLUGGED_ACTVACQINT, 

    FHE_SET_TCHTHR = CHARGER_PLUGGED_TCHTHR,
    FHE_SET_TCHDI = 3,
    FHE_SET_IDLEGCAFDEPTH = 16,
    FHE_SET_ACTVGCAFDEPTH = 32,
    FHE_SET_NOISETHR = CHARGER_PLUGGED_NOISETHR,
    FHE_SET_IDLEACQINT = 255, 
    FHE_SET_ACTVACQINT = 255,
} FHE_ARG;
#endif

