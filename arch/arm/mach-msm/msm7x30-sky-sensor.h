#ifndef _MSM7X30_SKY_SENSOR_H_
#define _MSM7X30_SKY_SENSOR_H_

#include <linux/gpio.h>
#include <linux/i2c-gpio.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <mach/vreg.h>

#define I2C_DEV_INDEX_GEOMAGNETIC		11 //  /dev/i2c-11
#define I2C_DEV_INDEX_ACCELEROMETER		12 //  /dev/i2c-12
#define I2C_DEV_INDEX_PROXIMITY			13 //  /dev/i2c-13
#define I2C_DEV_INDEX_GYROSCOPE			14 //  /dev/i2c-14

// GEOMAGNETIC
#if defined(CONFIG_EF38L_SENSORS_YAS529)
#define GEOMAGNETIC_SCL_PIN 178
#define GEOMAGNETIC_SDA_PIN 126
#define GEOMAGNETIC_UDELAY 2 // 250 kHz
#define GEOMAGNETIC_RST_PIN     177
#elif defined(CONFIG_EF36S_SENSORS_YAS529)
#define GEOMAGNETIC_SCL_PIN 178
#define GEOMAGNETIC_SDA_PIN 126
#define GEOMAGNETIC_UDELAY 2 // 250 kHz
#define GEOMAGNETIC_RST_PIN	177
#elif defined(CONFIG_EF18K_SENSORS_YAS529)
#define GEOMAGNETIC_SCL_PIN 178
#define GEOMAGNETIC_SDA_PIN 126
#define GEOMAGNETIC_UDELAY 2 // 250 kHz
#elif defined(CONFIG_EF30S_SENSORS_YAS529)
#define GEOMAGNETIC_SCL_PIN 178
#define GEOMAGNETIC_SDA_PIN 126
#define GEOMAGNETIC_UDELAY 2 // 250 kHz
#endif

// ACCELEROMETER
#if defined(CONFIG_EF38L_SENSORS_BMA150)
#define ACCELEROMETER_INT_PIN 180
#elif defined(CONFIG_EF36S_SENSORS_BMA150)
#define ACCELEROMETER_INT_PIN 180
#elif defined(CONFIG_EF18K_SENSORS_BMA150)
#define ACCELEROMETER_SCL_PIN 128
#define ACCELEROMETER_SDA_PIN 129
#define ACCELEROMETER_UDELAY 2 // 250 kHz
#elif defined(CONFIG_EF30S_SENSORS_BMA150)
#define ACCELEROMETER_SCL_PIN 128
#define ACCELEROMETER_SDA_PIN 129
#define ACCELEROMETER_UDELAY 2 // 250 kHz
#endif

// GYROSCOPE
#if defined(CONFIG_EF38L_SENSORS_MPU3050)
#define GYROSCOPE_SCL_PIN 2
#define GYROSCOPE_SDA_PIN 3
#define GYROSCOPE_UDELAY 2 // 250 kHz
#define GYROSCOPE_INT_PIN       44
#elif defined(CONFIG_EF36S_SENSORS_MPU3050)
#define GYROSCOPE_SCL_PIN 2
#define GYROSCOPE_SDA_PIN 3
#define GYROSCOPE_UDELAY 2 // 250 kHz
#define GYROSCOPE_INT_PIN	44
#elif defined(CONFIG_EF30S_SENSORS_L3G4200D)
#define GYROSCOPE_SCL_PIN 2
#define GYROSCOPE_SDA_PIN 3
#define GYROSCOPE_UDELAY 2 // 250 kHz
#define GYROSCOPE_INT_PIN	44
#endif

// PROXIMITY
#if defined(CONFIG_EF38L_SENSORS_APDS9900)
#define PROXIMITY_SCL_PIN 38
#define PROXIMITY_SDA_PIN 39
#define PROXIMITY_UDELAY 2 // 250 kHz
#define PROXIMITY_INT_PIN       42
#elif defined(CONFIG_EF36S_SENSORS_APDS9900)
#define PROXIMITY_SCL_PIN 38
#define PROXIMITY_SDA_PIN 39
#define PROXIMITY_UDELAY 2 // 250 kHz
#define PROXIMITY_INT_PIN	42
#elif defined(CONFIG_EF18K_SENSORS_APDS9900)
#define PROXIMITY_SCL_PIN 38
#define PROXIMITY_SDA_PIN 39
#define PROXIMITY_UDELAY 2 // 250 kHz
#elif defined(CONFIG_EF30S_SENSORS_APDS9900)
#define PROXIMITY_SCL_PIN 38
#define PROXIMITY_SDA_PIN 39
#define PROXIMITY_UDELAY 2 // 250 kHz
#endif

// POWER
#if defined(CONFIG_EF38L_SENSORS_APDS9900)
#define POWER_FIRST_STRING "rf"
#define POWER_FIRST_VOLTAGE 2800
#define POWER_SECOND_STRING "wlan"
#define POWER_SECOND_VOLTAGE 2850
#elif defined(CONFIG_EF36S_SENSORS_APDS9900)
#define POWER_FIRST_STRING "rf"
#define POWER_FIRST_VOLTAGE 2800
#define POWER_SECOND_STRING "wlan"
#define POWER_SECOND_VOLTAGE 2850
#elif defined(CONFIG_EF18K_SENSORS_APDS9900)
#define POWER_FIRST_STRING "rf"
#define POWER_FIRST_VOLTAGE 2800
#define POWER_SECOND_STRING "wlan"
#define POWER_SECOND_VOLTAGE 2850
#elif defined(CONFIG_EF30S_SENSORS_APDS9900)
#define POWER_FIRST_STRING "rf"
#define POWER_FIRST_VOLTAGE 2800
#define POWER_SECOND_STRING "wlan"
#define POWER_SECOND_VOLTAGE 2850
#define POWER_THIRD_STRING "gp2"
#define POWER_THIRD_VOLTAGE 2600
#else
#define POWER_FIRST_STRING "rf"
#define POWER_FIRST_VOLTAGE 2800
#define POWER_SECOND_STRING "wlan"
#define POWER_SECOND_VOLTAGE 2850
#endif


static struct i2c_gpio_platform_data gpio_i2c_geomagnetic_data = {
#if defined(GEOMAGNETIC_SCL_PIN) && defined(GEOMAGNETIC_SDA_PIN) && defined(GEOMAGNETIC_UDELAY)
	.scl_pin = GEOMAGNETIC_SCL_PIN,
	.sda_pin = GEOMAGNETIC_SDA_PIN,
	.udelay = GEOMAGNETIC_UDELAY
#endif
};

static struct i2c_gpio_platform_data gpio_i2c_accelerometer_data = {
#if defined(ACCELEROMETER_SCL_PIN) && defined(ACCELEROMETER_SDA_PIN) && defined(ACCELEROMETER_UDELAY)
	.scl_pin = ACCELEROMETER_SCL_PIN,
	.sda_pin = ACCELEROMETER_SDA_PIN,
	.udelay = ACCELEROMETER_UDELAY
#endif
};

static struct i2c_gpio_platform_data gpio_i2c_gyroscope_data = {
#if defined(GYROSCOPE_SCL_PIN) && defined(GYROSCOPE_SDA_PIN) && defined(GYROSCOPE_UDELAY)
	.scl_pin = GYROSCOPE_SCL_PIN,
	.sda_pin = GYROSCOPE_SDA_PIN,
	.udelay = GYROSCOPE_UDELAY
#endif
};

static struct i2c_gpio_platform_data gpio_i2c_proximity_data = {
#if defined(PROXIMITY_SCL_PIN) && defined(PROXIMITY_SDA_PIN) && defined(PROXIMITY_UDELAY)
	.scl_pin = PROXIMITY_SCL_PIN,
	.sda_pin = PROXIMITY_SDA_PIN,
	.udelay = PROXIMITY_UDELAY
#endif
};

struct platform_device msm_device_sensor_geomagnetic = {
	.name = "i2c-gpio",
	.id = I2C_DEV_INDEX_GEOMAGNETIC,
	.dev = {
		.platform_data = &gpio_i2c_geomagnetic_data,
	}
};

struct platform_device msm_device_sensor_accelerometer = {
	.name = "i2c-gpio",
	.id = I2C_DEV_INDEX_ACCELEROMETER,
	.dev = {
		.platform_data = &gpio_i2c_accelerometer_data,
	}
};

struct platform_device msm_device_sensor_gyroscope = {
	.name = "i2c-gpio",
	.id = I2C_DEV_INDEX_GYROSCOPE,
	.dev = {
		.platform_data = &gpio_i2c_gyroscope_data,
	}
};

struct platform_device msm_device_sensor_proximity = {
	.name = "i2c-gpio",
	.id = I2C_DEV_INDEX_PROXIMITY,
	.dev = {
		.platform_data = &gpio_i2c_proximity_data,
	}
};

static struct i2c_board_info geomagnetic_i2c_info[] __initdata = {
	#if defined(CONFIG_EF38L_SENSORS_YAS529) || defined(CONFIG_EF36S_SENSORS_YAS529) || defined(CONFIG_EF18K_SENSORS_YAS529) || defined(CONFIG_EF30S_SENSORS_YAS529)
	{
		I2C_BOARD_INFO("geomagnetic", 0x2E),
	},
	#endif
};

static struct i2c_board_info accelerometer_i2c_info[] __initdata = {
	#if defined(CONFIG_EF18K_SENSORS_BMA150) || defined(CONFIG_EF30S_SENSORS_BMA150) 
	{
		I2C_BOARD_INFO("accelerometer", 0x38),
	},
	#endif
};

static struct i2c_board_info proximity_i2c_info[] __initdata = {
	#if defined(CONFIG_EF38L_SENSORS_APDS9900) || defined(CONFIG_EF36S_SENSORS_APDS9900) || defined(CONFIG_EF18K_SENSORS_APDS9900) || defined(CONFIG_EF30S_SENSORS_APDS9900)
	{
		I2C_BOARD_INFO("apds9900", 0x39),
	},
	#endif
};

static struct i2c_board_info gyroscope_i2c_info[] __initdata = {
	#if defined(CONFIG_EF38L_SENSORS_MPU3050) || defined(CONFIG_EF36S_SENSORS_MPU3050)
	{
		I2C_BOARD_INFO("mpu3050", 0x68),
		.irq = MSM_GPIO_TO_INT(GYROSCOPE_INT_PIN), //gpio_to_irq
	},
	#elif defined(CONFIG_EF30S_SENSORS_L3G4200D)
	{
		I2C_BOARD_INFO("gyroscope", 0x68),
		.irq = MSM_GPIO_TO_INT(GYROSCOPE_INT_PIN), //gpio_to_irq
	},
	#endif
};

static int sensors_power_on(void)
{
	int rc = 0;
#ifdef POWER_FIRST_VOLTAGE
	struct vreg *vreg_first;
#endif
#ifdef POWER_SECOND_VOLTAGE
	struct vreg *vreg_second;
#endif
#ifdef POWER_THIRD_VOLTAGE
	struct vreg *vreg_third;
#endif

#ifdef POWER_FIRST_VOLTAGE
	vreg_first = vreg_get(NULL, POWER_FIRST_STRING);
	if(!vreg_first) {
		pr_err("%s: VREG (%s) get failed\n", __func__, POWER_FIRST_STRING);
		return rc;
	}

	rc = vreg_set_level(vreg_first, POWER_FIRST_VOLTAGE);
	if (rc) {
		pr_err("%s: VREG (%s) set voltage to %dmv failed\n", __func__, POWER_FIRST_STRING, POWER_FIRST_VOLTAGE);
		vreg_disable(vreg_first);
		return rc;
	}

	rc = vreg_enable(vreg_first);
	if (rc) {
		pr_err("%s: VREG (%s) enable failed\n", __func__, POWER_FIRST_STRING);
		vreg_disable(vreg_first);
		return rc;
	}
#endif

#ifdef POWER_SECOND_VOLTAGE
	vreg_second = vreg_get(NULL, POWER_SECOND_STRING);
	if(!vreg_second) {
		pr_err("%s: VREG (%s) get failed\n", __func__, POWER_SECOND_STRING);
		return rc;
	}

	rc = vreg_set_level(vreg_second, POWER_SECOND_VOLTAGE);
	if (rc) {
		pr_err("%s: VREG (%s) set voltage to %dmv failed\n", __func__, POWER_SECOND_STRING, POWER_SECOND_VOLTAGE);
		vreg_disable(vreg_second);
		return rc;
	}

	rc = vreg_enable(vreg_second);
	if (rc) {
		pr_err("%s: VREG (%s) enable failed\n", __func__, POWER_SECOND_STRING);
		vreg_disable(vreg_second);
		return rc;
	}
#endif


#ifdef POWER_THIRD_VOLTAGE
	vreg_third = vreg_get(NULL, POWER_THIRD_STRING);
	if(!vreg_third) {
		pr_err("%s: VREG (%s) get failed\n", __func__, POWER_THIRD_STRING);
		return rc;
	}

	rc = vreg_set_level(vreg_third, POWER_THIRD_VOLTAGE);
	if (rc) {
		pr_err("%s: VREG (%s) set voltage to %dmv failed\n", __func__, POWER_THIRD_STRING, POWER_THIRD_VOLTAGE);
		vreg_disable(vreg_third);
		return rc;
	}

	rc = vreg_enable(vreg_third);
	if (rc) {
		pr_err("%s: VREG (%s) enable failed\n", __func__, POWER_THIRD_STRING);
		vreg_disable(vreg_third);
		return rc;
	}
#endif

	return rc;
}

static int sensors_hw_init(void)
{
	int rc = 0;
	
	sensors_power_on();
	
#ifdef GYROSCOPE_INT_PIN
	rc = gpio_tlmm_config(GPIO_CFG(GYROSCOPE_INT_PIN, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, GYROSCOPE_INT_PIN);
		return rc;
	}
#endif

#ifdef ACCELEROMETER_INT_PIN
	rc = gpio_tlmm_config(GPIO_CFG(ACCELEROMETER_INT_PIN, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, ACCELEROMETER_INT_PIN);
		return rc;
	}
#endif

#ifdef GEOMAGNETIC_RST_PIN
	rc = gpio_tlmm_config(GPIO_CFG(GEOMAGNETIC_RST_PIN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, GEOMAGNETIC_RST_PIN);
		return rc;
	}

	rc = gpio_request(GEOMAGNETIC_RST_PIN, "gms_reset");
	if (rc) {
		pr_err("%s: unable to request gpio %d (%d)\n", __func__, GEOMAGNETIC_RST_PIN, rc);
		return rc;
	}

	gpio_set_value(GEOMAGNETIC_RST_PIN, 1);
#endif

#ifdef PROXIMITY_INT_PIN
	rc = gpio_tlmm_config(GPIO_CFG(PROXIMITY_INT_PIN, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, PROXIMITY_INT_PIN);
		return rc;
	}
#endif

	return rc;
}


#endif // _MSM7X30_SKY_SENSOR_H_
