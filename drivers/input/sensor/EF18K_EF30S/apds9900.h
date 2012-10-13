#ifndef _APDS9900_H_
#define _APDS9900_H_

#define HAL_TYPE_YAMAHA (0)
#define HAL_TYPE_INVENSENSE (1)

#if defined(CONFIG_EF18K_SENSORS_APDS9900) || defined(CONFIG_EF30S_SENSORS_APDS9900)
#define HAL_TYPE HAL_TYPE_YAMAHA
#else
#error "Unknown HAL TYPE for APDS9900"
#endif

#define APDS9900_TYPE_PROXIMITY		1
#define APDS9900_TYPE_LIGHT			2
#define APDS9900_TYPE_ALL           (APDS9900_TYPE_PROXIMITY | APDS9900_TYPE_LIGHT)

typedef struct {
        int x;
        int y;
        int z;
} axes_t;

int apds9900_enable(int type, bool enable);
int apds9900_measure(int type, axes_t* val);
//void apds9900_suspend(void);
//void apds9900_resume(void);

#endif // _APDS9900_H_
