/*
 * Copyright (c) 2010-2011 Yamaha Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */

#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/version.h>

#if defined(PANTECH_EARLY_SUSPEND_SENSOR_OFF) && defined(CONFIG_HAS_EARLYSUSPEND)
#include <linux/earlysuspend.h>
#endif // PANTECH_EARLY_SUSPEND_SENSOR_OFF && CONFIG_HAS_EARLYSUSPEND

#ifdef PANTECH_FEATURE_SENSOR

#if (SENSOR_TYPE == SENSOR_TYPE_PROXIMITY) || (SENSOR_TYPE == SENSOR_TYPE_LIGHT)
#if (YAS_PROX_DRIVER == YAS_PROX_DRIVER_APDS9900) || (YAS_LIG_DRIVER == YAS_LIG_DRIVER_APDS9900)
#include "apds9900.h"
#endif
#endif

#if (SENSOR_TYPE == SENSOR_TYPE_ORIENTATION)
#define SENSOR_NAME "orientation"
#elif (SENSOR_TYPE == SENSOR_TYPE_LIGHT)
#if (HAL_TYPE == HAL_TYPE_YAMAHA)
#define SENSOR_NAME "light"
#elif (HAL_TYPE == HAL_TYPE_INVENSENSE)
#define SENSOR_NAME "lightsensor-level"
#else
#define SENSOR_NAME "light"
#endif // HAL_TYPE
#elif (SENSOR_TYPE == SENSOR_TYPE_PRESSURE)
#define SENSOR_NAME "pressure"
#elif (SENSOR_TYPE == SENSOR_TYPE_TEMPERATURE)
#define SENSOR_NAME "temperature"
#elif (SENSOR_TYPE == SENSOR_TYPE_PROXIMITY)
#define SENSOR_NAME "proximity"
#endif // SENSOR_TYPE

#else

#if SENSOR_TYPE == 1
#define SENSOR_NAME "accelerometer"
#elif SENSOR_TYPE == 2
#define SENSOR_NAME "geomagnetic"
#elif SENSOR_TYPE == 3
#define SENSOR_NAME "orientation"
#elif SENSOR_TYPE == 4
#define SENSOR_NAME "gyroscope"
#elif SENSOR_TYPE == 5
#define SENSOR_NAME "light"
#elif SENSOR_TYPE == 6
#define SENSOR_NAME "pressure"
#elif SENSOR_TYPE == 7
#define SENSOR_NAME "temperature"
#elif SENSOR_TYPE == 8
#define SENSOR_NAME "proximity"
#endif

#endif // PANTECH_FEATURE_SENSOR

#define SENSOR_DEFAULT_DELAY            (200)   /* 200 ms */
#define SENSOR_MAX_DELAY                (2000)  /* 2000 ms */
#define ABS_STATUS                      (ABS_BRAKE)
#define ABS_WAKE                        (ABS_MISC)
#define ABS_CONTROL_REPORT              (ABS_THROTTLE)

/* -------------------------------------------------------------------- */
/* debug option */
/* -------------------------------------------------------------------- */
//#define SENSOR_ORIENTATION_DBG_ENABLE
#ifdef SENSOR_ORIENTATION_DBG_ENABLE
#define dbg(fmt, args...)   printk("_SHDBG_ [GNR_DRV] " fmt, ##args)
#else
#define dbg(fmt, args...)
#endif
#define dbg_func_in()       dbg("(%s)[+] %s\n", , SESNOR_NAME, __func__)
#define dbg_func_out()      dbg("(%s)[-] %s\n", SENSOR_NAME__func__)
#define dbg_line()          dbg("[LINE] %d(%s)\n", __LINE__, __func__)
/* -------------------------------------------------------------------- */
#define PANTECH_ATOMIC_USE // P12095. use atomic lock because of enable/disable's operation

static int suspend(void);
static int resume(void);

struct sensor_data {
#ifndef PANTECH_AVOID_DEADLOCK
    struct mutex mutex;
#endif
#ifdef PANTECH_ATOMIC_USE
	atomic_t enabled;
	atomic_t delay;
#else
    int enabled;
    int delay;
#endif
#if DEBUG
    int suspend;
#endif
#ifdef PANTECH_FEATURE_SENSOR
#if ( ((SENSOR_TYPE == SENSOR_TYPE_PROXIMITY) && (YAS_PROX_DRIVER == YAS_PROX_DRIVER_APDS9900)) || ((SENSOR_TYPE == SENSOR_TYPE_LIGHT) && (YAS_LIG_DRIVER == YAS_LIG_DRIVER_APDS9900)) ) 
	atomic_t debug;
#endif
#endif
};

static struct platform_device *sensor_pdev = NULL;
static struct input_dev *this_data = NULL;
#if defined(PANTECH_FEATURE_SENSOR) && (SENSOR_TYPE == SENSOR_TYPE_PROXIMITY) 
static int initialized = 0;
#endif

#if defined(PANTECH_EARLY_SUSPEND_SENSOR_OFF) && defined(CONFIG_HAS_EARLYSUSPEND)
#if (SENSOR_TYPE != SENSOR_TYPE_PROXIMITY)
static struct early_suspend es;
static atomic_t disable_forced = ATOMIC_INIT(0);
#endif
#endif // PANTECH_EARLY_SUSPEND_SENSOR_OFF && CONFIG_HAS_EARLYSUSPEND && (SENSOR_TYPE != SENSOR_TYPE_PROXIMITY)

static int
suspend(void)
{
    /* implement suspend of the sensor */
    YLOGD(("%s: suspend\n", SENSOR_NAME));

    if (strcmp(SENSOR_NAME, "gyroscope") == 0) {
        /* suspend gyroscope */
    }
    else if (strcmp(SENSOR_NAME, "light") == 0) {
        /* suspend light */
    }
    else if (strcmp(SENSOR_NAME, "pressure") == 0) {
        /* suspend pressure */
    }
    else if (strcmp(SENSOR_NAME, "temperature") == 0) {
        /* suspend temperature */
    }
    else if (strcmp(SENSOR_NAME, "proximity") == 0) {
        /* suspend proximity */
    }

    return 0;
}

static int
resume(void)
{
    /* implement resume of the sensor */
    YLOGD(("%s: resume\n", SENSOR_NAME));

    if (strcmp(SENSOR_NAME, "gyroscope") == 0) {
        /* resume gyroscope */
    }
    else if (strcmp(SENSOR_NAME, "light") == 0) {
        /* resume light */
    }
    else if (strcmp(SENSOR_NAME, "pressure") == 0) {
        /* resume pressure */
    }
    else if (strcmp(SENSOR_NAME, "temperature") == 0) {
        /* resume temperature */
    }
    else if (strcmp(SENSOR_NAME, "proximity") == 0) {
        /* resume proximity */
    }

#if DEBUG
    {
        struct sensor_data *data = input_get_drvdata(this_data);
        data->suspend = 0;
    }
#endif /* DEBUG */

    return 0;
}


/* Sysfs interface */
static ssize_t
sensor_delay_show(struct device *dev,
        struct device_attribute *attr,
        char *buf)
{
    struct input_dev *input_data = to_input_dev(dev);
    struct sensor_data *data = input_get_drvdata(input_data);
    int delay;

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_lock(&data->mutex);
#endif

#ifdef PANTECH_ATOMIC_USE
	delay = atomic_read(&data->delay);
#else
    delay = data->delay;
#endif

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_unlock(&data->mutex);
#endif

    return sprintf(buf, "%d\n", delay);
}

static ssize_t
sensor_delay_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count)
{
    struct input_dev *input_data = to_input_dev(dev);
    struct sensor_data *data = input_get_drvdata(input_data);
    int value = simple_strtol(buf, NULL, 10);
#ifdef PANTECH_ATOMIC_USE
	int en = atomic_read(&data->enabled);
#endif

#if (SENSOR_TYPE == SENSOR_TYPE_PROXIMITY) || (SENSOR_TYPE == SENSOR_TYPE_LIGHT)
#if (HAL_TYPE == HAL_TYPE_INVENSENSE)
	value = (int)(value/1000); // nano second to milli second
#endif
#endif

	dbg_func_in();

    if (value < 0) {
        return count;
    }

    if (SENSOR_MAX_DELAY < value) {
        value = SENSOR_MAX_DELAY;
    }

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_lock(&data->mutex);
#endif

#ifdef PANTECH_ATOMIC_USE
	atomic_set(&data->delay, value);
	dbg("%s : input_report_abs(%d,%d)\n", __func__, en, value);
    input_report_abs(input_data, ABS_CONTROL_REPORT, (en<<16) | value);
#else
    data->delay = value;

    input_report_abs(input_data, ABS_CONTROL_REPORT, (data->enabled<<16) | value);
#endif

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_unlock(&data->mutex);
#endif

	dbg_func_out();
    return count;
}

static ssize_t
sensor_enable_show(struct device *dev,
        struct device_attribute *attr,
        char *buf)
{
    struct input_dev *input_data = to_input_dev(dev);
    struct sensor_data *data = input_get_drvdata(input_data);
    int enabled;

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_lock(&data->mutex);
#endif

#ifdef PANTECH_ATOMIC_USE
	enabled = atomic_read(&data->enabled);
#else
    enabled = data->enabled;
#endif

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_unlock(&data->mutex);
#endif

    return sprintf(buf, "%d\n", enabled);
}

static ssize_t
sensor_enable_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count)
{
    struct input_dev *input_data = to_input_dev(dev);
    struct sensor_data *data = input_get_drvdata(input_data);
    int value = simple_strtol(buf, NULL, 10);
#ifdef PANTECH_ATOMIC_USE
	int en = atomic_read(&data->enabled);
	int dly = atomic_read(&data->delay);
#endif

	dbg_func_in();

    value = !!value;

#ifdef PANTECH_FEATURE_SENSOR
#if defined(PANTECH_FEATURE_SENSOR) && (SENSOR_TYPE == SENSOR_TYPE_PROXIMITY) 
	if(value) initialized = 0;
#endif
#if (SENSOR_TYPE == SENSOR_TYPE_PROXIMITY) && (YAS_PROX_DRIVER == YAS_PROX_DRIVER_APDS9900)
	apds9900_enable(APDS9900_TYPE_PROXIMITY, (bool)value);
#elif (SENSOR_TYPE == SENSOR_TYPE_LIGHT) && (YAS_LIG_DRIVER == YAS_LIG_DRIVER_APDS9900) 
	apds9900_enable(APDS9900_TYPE_LIGHT, (bool)value);
#endif
#endif // PANTECH_FEATURE_SENSOR

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_lock(&data->mutex);
#endif

#ifdef PANTECH_ATOMIC_USE
	dbg("%s : input_report_abs(%d,%d)\n", __func__, value, dly);
    input_report_abs(input_data, ABS_CONTROL_REPORT, (value<<16) | dly);

    if (en && !value) {
        suspend();
    }
    if (!en && value) {
        resume();
    }
	atomic_set(&data->enabled, value);
#else
	dbg("%s : input_report_abs(%d,%d)\n", __func__, value, data->delay);
    input_report_abs(input_data, ABS_CONTROL_REPORT, (value<<16) | data->delay);

    if (data->enabled && !value) {
        suspend();
    }
    if (!data->enabled && value) {
        resume();
    }
    data->enabled = value;
#endif

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_unlock(&data->mutex);
#endif

	dbg_func_out();

    return count;
}

static ssize_t
sensor_wake_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count)
{
    struct input_dev *input_data = to_input_dev(dev);
    static int cnt = 1;

	dbg_func_in();
	dbg("%s : input_report_abs(WAKE)\n", __func__);
    input_report_abs(input_data, ABS_WAKE, cnt++);
    input_sync(input_data);

    return count;
}

#ifdef PANTECH_FEATURE_SENSOR
#if ( ((SENSOR_TYPE == SENSOR_TYPE_PROXIMITY) && (YAS_PROX_DRIVER == YAS_PROX_DRIVER_APDS9900)) || ((SENSOR_TYPE == SENSOR_TYPE_LIGHT) && (YAS_LIG_DRIVER == YAS_LIG_DRIVER_APDS9900)) ) 
static ssize_t
sensor_measure_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count)
{
    struct input_dev *input_data = to_input_dev(dev);
    struct sensor_data *data = input_get_drvdata(input_data);
    int value = simple_strtol(buf, NULL, 10);

#if (SENSOR_TYPE == SENSOR_TYPE_PROXIMITY) && (YAS_PROX_DRIVER == YAS_PROX_DRIVER_APDS9900)
	axes_t val = {5,0,0};
	if(value) apds9900_measure(APDS9900_TYPE_PROXIMITY, &val);
#elif (SENSOR_TYPE == SENSOR_TYPE_LIGHT) && (YAS_LIG_DRIVER == YAS_LIG_DRIVER_APDS9900) 
	axes_t val = {0,0,0};
	if(value) apds9900_measure(APDS9900_TYPE_LIGHT, &val);
#else
	axes_t val;
#endif

	if(!atomic_read(&data->debug)) {
		val.y = 0;
		val.z = 0;
	}

	input_report_abs(input_data, ABS_X, val.x);
	input_report_abs(input_data, ABS_Y, val.y);
	input_report_abs(input_data, ABS_Z, val.z);
	input_sync(input_data);

	dbg_func_out();
    return count;
}

static ssize_t
sensor_debug_show(struct device *dev,
        struct device_attribute *attr,
        char *buf)
{
    struct input_dev *input_data = to_input_dev(dev);
    struct sensor_data *data = input_get_drvdata(input_data);

    return sprintf(buf, "%d\n", atomic_read(&data->debug));
}

static ssize_t
sensor_debug_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf,
        size_t count)
{
    struct input_dev *input_data = to_input_dev(dev);
    struct sensor_data *data = input_get_drvdata(input_data);
    int value = simple_strtol(buf, NULL, 10);

    atomic_set(&data->debug, value);

    return count;
}

#endif

#endif // PANTECH_FEATURE_SENSOR

#if DEBUG

static int sensor_suspend(struct platform_device *pdev, pm_message_t state);
static int sensor_resume(struct platform_device *pdev);

static ssize_t
sensor_debug_suspend_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct input_dev *input = to_input_dev(dev);
    struct sensor_data *data = input_get_drvdata(input);

    return sprintf(buf, "%d\n", data->suspend);
}

static ssize_t
sensor_debug_suspend_store(struct device *dev,
        struct device_attribute *attr,
        const char *buf, size_t count)
{
    unsigned long suspend = simple_strtol(buf, NULL, 10);

    if (suspend) {
        pm_message_t msg;
        memset(&msg, 0, sizeof(msg));
        sensor_suspend(sensor_pdev, msg);
    } else {
        sensor_resume(sensor_pdev);
    }

    return count;
}

#endif /* DEBUG */

static ssize_t
sensor_data_show(struct device *dev,
        struct device_attribute *attr,
        char *buf)
{
    struct input_dev *input_data = to_input_dev(dev);
#ifdef PANTECH_FEATURE_SENSOR
    int x, y, z;
#else
#if SENSOR_TYPE <= 4
    int x, y, z;
#else
    int x;
#endif
#endif // PANTECH_FEATURE_SENSOR

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
    unsigned long flags;

    spin_lock_irqsave(&input_data->event_lock, flags);


#ifdef PANTECH_FEATURE_SENSOR
	#if (SENSOR_TYPE == SENSOR_TYPE_PROXIMITY)
	if(initialized) {
		x = input_data->abs[ABS_X];
	}
	else {
		x = 5;
		initialized = 1;
	}
	#else
	x = input_data->abs[ABS_X];
	#endif
    y = input_data->abs[ABS_Y];
    z = input_data->abs[ABS_Z];
#else
	x = input_data->abs[ABS_X];
#if SENSOR_TYPE <= 4
    y = input_data->abs[ABS_Y];
    z = input_data->abs[ABS_Z];
#endif
#endif // PANTECH_FEATURE_SENSOR
    spin_unlock_irqrestore(&input_data->event_lock, flags);
#else
    x = input_abs_get_val(input_data, ABS_X);
#ifdef PANTECH_FEATURE_SENSOR
    y = input_abs_get_val(input_data, ABS_Y);
    z = input_abs_get_val(input_data, ABS_Z);
#else
#if SENSOR_TYPE <= 4
    y = input_abs_get_val(input_data, ABS_Y);
    z = input_abs_get_val(input_data, ABS_Z);
#endif
#endif // PANTECH_FEATURE_SENSOR
#endif

#ifdef PANTECH_FEATURE_SENSOR
    return sprintf(buf, "%d %d %d\n", x, y, z);
#else
#if SENSOR_TYPE <= 4
    return sprintf(buf, "%d %d %d\n", x, y, z);
#else
    return sprintf(buf, "%d\n", x);
#endif
#endif // PANTECH_FEATURE_SENSOR
}

static ssize_t
sensor_status_show(struct device *dev,
        struct device_attribute *attr,
        char *buf)
{
    struct input_dev *input_data = to_input_dev(dev);
    int status;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
    unsigned long flags;

    spin_lock_irqsave(&input_data->event_lock, flags);

    status = input_data->abs[ABS_STATUS];

    spin_unlock_irqrestore(&input_data->event_lock, flags);
#else
    status = input_abs_get_val(input_data, ABS_STATUS);
#endif

    return sprintf(buf, "%d\n", status);
}

static DEVICE_ATTR(delay, S_IRUGO|S_IWUSR|S_IWGRP,
        sensor_delay_show, sensor_delay_store);
static DEVICE_ATTR(enable, S_IRUGO|S_IWUSR|S_IWGRP,
        sensor_enable_show, sensor_enable_store);
static DEVICE_ATTR(wake, S_IWUSR|S_IWGRP,
        NULL, sensor_wake_store);
static DEVICE_ATTR(data, S_IRUGO, sensor_data_show, NULL);
static DEVICE_ATTR(status, S_IRUGO, sensor_status_show, NULL);
#ifdef PANTECH_FEATURE_SENSOR
#if ( ((SENSOR_TYPE == SENSOR_TYPE_PROXIMITY) && (YAS_PROX_DRIVER == YAS_PROX_DRIVER_APDS9900)) || ((SENSOR_TYPE == SENSOR_TYPE_LIGHT) && (YAS_LIG_DRIVER == YAS_LIG_DRIVER_APDS9900)) ) 
static DEVICE_ATTR(measure, S_IWUSR|S_IWGRP, NULL, sensor_measure_store);
static DEVICE_ATTR(debug, S_IWUSR|S_IWGRP, sensor_debug_show, sensor_debug_store);
#endif
#endif // PANTECH_FEATURE_SENSOR

#if DEBUG
static DEVICE_ATTR(debug_suspend, S_IRUGO|S_IWUSR,
                   sensor_debug_suspend_show, sensor_debug_suspend_store);
#endif /* DEBUG */

static struct attribute *sensor_attributes[] = {
    &dev_attr_delay.attr,
    &dev_attr_enable.attr,
    &dev_attr_wake.attr,
    &dev_attr_data.attr,
    &dev_attr_status.attr,
#ifdef PANTECH_FEATURE_SENSOR
#if ( ((SENSOR_TYPE == SENSOR_TYPE_PROXIMITY) && (YAS_PROX_DRIVER == YAS_PROX_DRIVER_APDS9900)) || ((SENSOR_TYPE == SENSOR_TYPE_LIGHT) && (YAS_LIG_DRIVER == YAS_LIG_DRIVER_APDS9900)) ) 
    &dev_attr_measure.attr,
    &dev_attr_debug.attr,
#endif
#endif // PANTECH_FEATURE_SENSOR
#if DEBUG
    &dev_attr_debug_suspend.attr,
#endif /* DEBUG */
    NULL
};

static struct attribute_group sensor_attribute_group = {
    .attrs = sensor_attributes
};

#if defined(PANTECH_EARLY_SUSPEND_SENSOR_OFF) && defined(CONFIG_HAS_EARLYSUSPEND) && (SENSOR_TYPE != SENSOR_TYPE_PROXIMITY)
static void sensor_early_suspend(struct early_suspend *h)
{
    struct sensor_data *data = input_get_drvdata(this_data);
    int rt = 0;
#ifdef PANTECH_ATOMIC_USE
	int en = atomic_read(&data->enabled);
	int dly = atomic_read(&data->delay);
#endif

	dbg_func_in();

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_lock(&data->mutex);
#endif


#ifdef PANTECH_ATOMIC_USE
	dbg("%s : data->enabled = %d\n", __func__, en);

    if (en) {
		dbg("%s : input_report_abs(0,%d)\n", __func__, dly);
        input_report_abs(this_data, ABS_CONTROL_REPORT, (0<<16) | dly);
        rt = suspend();
        atomic_set(&disable_forced, 1);
        atomic_set(&data->enabled, 0);
    }
#else
    if (data->enabled) {
        input_report_abs(this_data, ABS_CONTROL_REPORT, (0<<16) | data->delay);
        rt = suspend();
    }
#endif

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_unlock(&data->mutex);
#endif
}

static void sensor_late_resume(struct early_suspend *h)
{
    struct sensor_data *data = input_get_drvdata(this_data);
    int rt = 0;
#ifdef PANTECH_ATOMIC_USE
	int en = atomic_read(&data->enabled);
	int dly = atomic_read(&data->delay);
#endif

	dbg_func_in();

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_lock(&data->mutex);
#endif

#ifdef PANTECH_ATOMIC_USE
	dbg("%s : data->enabled = %d\n", __func__, en);

    if (atomic_read(&disable_forced)){
        atomic_set(&data->enabled, 1);
        atomic_set(&disable_forced, 0);
        en = atomic_read(&data->enabled);
    }

    if (en) {
        rt = resume();
		dbg("%s : input_report_abs(1,%d)\n", __func__, dly);
        input_report_abs(this_data, ABS_CONTROL_REPORT, (1<<16) | dly);
    }
#else
	dbg("%s : data->enabled = %d\n", __func__, data->enabled);

    if (data->enabled) {
        rt = resume();
        input_report_abs(this_data, ABS_CONTROL_REPORT, (1<<16) | data->delay);
    }
#endif

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_unlock(&data->mutex);
#endif

	dbg_func_out();

}
#endif // PANTECH_EARLY_SUSPEND_SENSOR_OFF && CONFIG_HAS_EARLYSUSPEND && (SENSOR_TYPE != SENSOR_TYPE_PROXIMITY)

static int
sensor_suspend(struct platform_device *pdev, pm_message_t state)
{
#if defined(PANTECH_EARLY_SUSPEND_SENSOR_OFF) && defined(CONFIG_HAS_EARLYSUSPEND) && (SENSOR_TYPE != SENSOR_TYPE_PROXIMITY)
	return 0;
#else

    struct sensor_data *data = input_get_drvdata(this_data);
    int rt = 0;
#ifdef PANTECH_ATOMIC_USE
	int en = atomic_read(&data->enabled);
	int dly = atomic_read(&data->delay);
#endif

	dbg_func_in();

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_lock(&data->mutex);
#endif


#ifdef PANTECH_ATOMIC_USE
	dbg("%s : data->enabled = %d\n", __func__, en);

    if (en) {
		dbg("%s : input_report_abs(0,%d)\n", __func__, dly);
        input_report_abs(this_data, ABS_CONTROL_REPORT, (0<<16) | dly);
        rt = suspend();
    }
#else
    if (data->enabled) {
        input_report_abs(this_data, ABS_CONTROL_REPORT, (0<<16) | data->delay);
        rt = suspend();
    }
#endif

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_unlock(&data->mutex);
#endif

	dbg_func_out();

    return rt;

#endif // PANTECH_EARLY_SUSPEND_SENSOR_OFF && CONFIG_HAS_EARLYSUSPEND && (SENSOR_TYPE != SENSOR_TYPE_PROXIMITY)
}

static int
sensor_resume(struct platform_device *pdev)
{
#if defined(PANTECH_EARLY_SUSPEND_SENSOR_OFF) && defined(CONFIG_HAS_EARLYSUSPEND) && (SENSOR_TYPE != SENSOR_TYPE_PROXIMITY)
	return 0;
#else

    struct sensor_data *data = input_get_drvdata(this_data);
    int rt = 0;
#ifdef PANTECH_ATOMIC_USE
	int en = atomic_read(&data->enabled);
	int dly = atomic_read(&data->delay);
#endif

	dbg_func_in();

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_lock(&data->mutex);
#endif

#ifdef PANTECH_ATOMIC_USE
	dbg("%s : data->enabled = %d\n", __func__, en);

    if (en) {
        rt = resume();
		dbg("%s : input_report_abs(1,%d)\n", __func__, dly);
        input_report_abs(this_data, ABS_CONTROL_REPORT, (1<<16) | dly);
    }
#else
	dbg("%s : data->enabled = %d\n", __func__, data->enabled);

    if (data->enabled) {
        rt = resume();
        input_report_abs(this_data, ABS_CONTROL_REPORT, (1<<16) | data->delay);
    }
#endif

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_unlock(&data->mutex);
#endif

	dbg_func_out();

    return rt;
#endif // PANTECH_EARLY_SUSPEND_SENSOR_OFF && CONFIG_HAS_EARLYSUSPEND && (SENSOR_TYPE == SENSOR_TYPE_PROXIMITY)
}

static int
sensor_probe(struct platform_device *pdev)
{
    struct sensor_data *data = NULL;
    struct input_dev *input_data = NULL;
    int input_registered = 0, sysfs_created = 0;
    int rt;

    data = kzalloc(sizeof(struct sensor_data), GFP_KERNEL);
    if (!data) {
        rt = -ENOMEM;
        goto err;
    }
#ifdef PANTECH_ATOMIC_USE
	atomic_set(&data->enabled, 0);
	atomic_set(&data->delay, SENSOR_DEFAULT_DELAY);
#else
    data->enabled = 0;
    data->delay = SENSOR_DEFAULT_DELAY;
#endif

#ifdef PANTECH_FEATURE_SENSOR
#if ( ((SENSOR_TYPE == SENSOR_TYPE_PROXIMITY) && (YAS_PROX_DRIVER == YAS_PROX_DRIVER_APDS9900)) || ((SENSOR_TYPE == SENSOR_TYPE_LIGHT) && (YAS_LIG_DRIVER == YAS_LIG_DRIVER_APDS9900)) ) 
	atomic_set(&data->debug, 0);
#endif
#endif

    input_data = input_allocate_device();
    if (!input_data) {
        rt = -ENOMEM;
        YLOGE(("sensor_probe: Failed to allocate input_data device\n"));
        goto err;
    }

    set_bit(EV_ABS, input_data->evbit);
    input_set_abs_params(input_data, ABS_X, 0x80000000, 0x7fffffff, 0, 0);
#ifdef PANTECH_FEATURE_SENSOR
    input_set_abs_params(input_data, ABS_Y, 0x80000000, 0x7fffffff, 0, 0);
    input_set_abs_params(input_data, ABS_Z, 0x80000000, 0x7fffffff, 0, 0);
#else
#if SENSOR_TYPE <= 4
    input_set_abs_params(input_data, ABS_Y, 0x80000000, 0x7fffffff, 0, 0);
    input_set_abs_params(input_data, ABS_Z, 0x80000000, 0x7fffffff, 0, 0);
#endif
#endif // PANTECH_FEATURE_SENSOR
    input_set_abs_params(input_data, ABS_RUDDER, 0x80000000, 0x7fffffff, 0, 0);
    input_set_abs_params(input_data, ABS_STATUS, 0, 3, 0, 0);
    input_set_abs_params(input_data, ABS_WAKE, 0x80000000, 0x7fffffff, 0, 0);
    input_set_abs_params(input_data, ABS_CONTROL_REPORT, 0x80000000, 0x7fffffff, 0, 0);
    input_data->name = SENSOR_NAME;

    rt = input_register_device(input_data);
    if (rt) {
        YLOGE(("sensor_probe: Unable to register input_data device: %s\n",
               input_data->name));
        goto err;
    }
    input_set_drvdata(input_data, data);
    input_registered = 1;

    rt = sysfs_create_group(&input_data->dev.kobj,
            &sensor_attribute_group);
    if (rt) {
        YLOGE(("sensor_probe: sysfs_create_group failed[%s]\n",
               input_data->name));
        goto err;
    }
    sysfs_created = 1;
#ifndef PANTECH_AVOID_DEADLOCK
    mutex_init(&data->mutex);
#endif
    this_data = input_data;

#if defined(PANTECH_EARLY_SUSPEND_SENSOR_OFF) && defined(CONFIG_HAS_EARLYSUSPEND) && (SENSOR_TYPE != SENSOR_TYPE_PROXIMITY)
	es.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
	es.suspend = sensor_early_suspend;
	es.resume = sensor_late_resume;
	register_early_suspend(&es);
#endif
    return 0;

err:
    if (data != NULL) {
        if (input_data != NULL) {
            if (sysfs_created) {
                sysfs_remove_group(&input_data->dev.kobj,
                        &sensor_attribute_group);
            }
            if (input_registered) {
                input_unregister_device(input_data);
            }
            else {
                input_free_device(input_data);
            }
            input_data = NULL;
        }
        kfree(data);
    }

    return rt;
}

static int
sensor_remove(struct platform_device *pdev)
{
    struct sensor_data *data;

    if (this_data != NULL) {
        data = input_get_drvdata(this_data);
        sysfs_remove_group(&this_data->dev.kobj,
                &sensor_attribute_group);
        input_unregister_device(this_data);
        if (data != NULL) {
            kfree(data);
        }
    }

#if defined(PANTECH_EARLY_SUSPEND_SENSOR_OFF) && defined(CONFIG_HAS_EARLYSUSPEND) && (SENSOR_TYPE != SENSOR_TYPE_PROXIMITY)
	if(es.suspend == sensor_early_suspend)
		unregister_early_suspend(&es);
#endif

    return 0;
}

/*
 * Module init and exit
 */
static struct platform_driver sensor_driver = {
    .probe      = sensor_probe,
    .remove     = sensor_remove,
    .suspend    = sensor_suspend,
    .resume     = sensor_resume,
    .driver = {
        .name   = SENSOR_NAME,
        .owner  = THIS_MODULE,
    },
};

static int __init sensor_init(void)
{
    sensor_pdev = platform_device_register_simple(SENSOR_NAME, 0, NULL, 0);
    if (IS_ERR(sensor_pdev)) {
        return -1;
    }
    return platform_driver_register(&sensor_driver);
}
module_init(sensor_init);

static void __exit sensor_exit(void)
{
    platform_driver_unregister(&sensor_driver);
    platform_device_unregister(sensor_pdev);
}
module_exit(sensor_exit);

MODULE_AUTHOR("Yamaha Corporation");
MODULE_LICENSE( "GPL" );
MODULE_VERSION("4.0.500");
