/*
 * Copyright (c) 2011 Yamaha Corporation
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
#include <linux/workqueue.h>
#include <linux/slab.h>
#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/time.h>
#endif

#define __LINUX_KERNEL_DRIVER__
#include "yas.h"
#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0   /* i2c */
#include <linux/i2c.h>
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1 /* spi */
#include <linux/device.h>
#include <linux/spi/spi.h>
#endif
#ifdef PANTECH_FEATURE_SENSOR
#if (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
#include "yas_gyro_driver-l3g4200d.c"
#endif
#else
#include "yas_gyro_driver.c"
#endif

#define YAS_GYRO_KERNEL_VERSION                                                     "4.0.500"
#define YAS_GYRO_KERNEL_NAME                                                      "gyroscope"

/* ABS axes parameter range [um/s^2] (for input event) */
#define ABSMAX_DPS                                                                  (2000000)
#define ABSMIN_DPS                                                                 (-2000000)

#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
#else
#define delay_to_jiffies(d)                                       ((d)?msecs_to_jiffies(d):1)
#endif

/* ---------------------------------------------------------------------------------------- *
   Function prototype declaration
 * ---------------------------------------------------------------------------------------- */
static struct yas_gyro_private_data *yas_gyro_get_data(void);
static void yas_gyro_set_data(struct yas_gyro_private_data *);
#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
static void yas_gyro_current_time(int32_t *sec, int32_t *msec);
#endif
static int yas_gyro_lock(void);
static int yas_gyro_unlock(void);
static int yas_gyro_device_open(void);
static int yas_gyro_device_close(void);
static int yas_gyro_device_write(uint8_t, const uint8_t *, int);
static int yas_gyro_device_read(uint8_t, uint8_t *, int);
#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
static int yas_gyro_interrupt_enable(void);
static int yas_gyro_interrupt_disable(void);
static void yas_gyro_interrupt_notify(int);
static irqreturn_t yas_gyro_interrupt_handler(int, void *);
#endif /* defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT) */
static void yas_gyro_msleep(int);

static int yas_gyro_core_driver_init(struct yas_gyro_private_data *);
static void yas_gyro_core_driver_fini(struct yas_gyro_private_data *);
static int yas_gyro_get_enable(struct yas_gyro_driver *);
static int yas_gyro_set_enable(struct yas_gyro_driver *, int);
static int yas_gyro_get_delay(struct yas_gyro_driver *);
static int yas_gyro_set_delay(struct yas_gyro_driver *, int);
static int yas_gyro_get_position(struct yas_gyro_driver *);
static int yas_gyro_set_position(struct yas_gyro_driver *, int);
static int yas_gyro_get_offset(struct yas_gyro_driver *, struct yas_vector *);
static int yas_gyro_set_offset(struct yas_gyro_driver *, struct yas_vector *);
static int yas_gyro_get_threshold(struct yas_gyro_driver *);
static int yas_gyro_set_threshold(struct yas_gyro_driver *, int);
static int yas_gyro_get_filter_enable(struct yas_gyro_driver *);
static int yas_gyro_set_filter_enable(struct yas_gyro_driver *, int);
static int yas_gyro_measure(struct yas_gyro_driver *, struct yas_gyro_data *, int);
static int yas_gyro_input_init(struct yas_gyro_private_data *);
static void yas_gyro_input_fini(struct yas_gyro_private_data *);
#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
static int yas_gyro_set_selftest_enable(struct yas_gyro_driver *driver, int enable);
static int yas_gyro_get_selftest_result(struct yas_gyro_driver *driver);
#endif

static ssize_t yas_gyro_enable_show(struct device *, struct device_attribute *, char *);
static ssize_t yas_gyro_enable_store(struct device *,struct device_attribute *, const char *, size_t);
static ssize_t yas_gyro_delay_show(struct device *, struct device_attribute *, char *);
static ssize_t yas_gyro_delay_store(struct device *, struct device_attribute *, const char *, size_t);
static ssize_t yas_gyro_position_show(struct device *, struct device_attribute *, char *);
static ssize_t yas_gyro_position_store(struct device *, struct device_attribute *, const char *, size_t);
static ssize_t yas_gyro_offset_show(struct device *, struct device_attribute *, char *);
static ssize_t yas_gyro_offset_store(struct device *, struct device_attribute *, const char *, size_t);
static ssize_t yas_gyro_threshold_show(struct device *, struct device_attribute *, char *);
static ssize_t yas_gyro_threshold_store(struct device *, struct device_attribute *, const char *, size_t);
static ssize_t yas_gyro_filter_enable_show(struct device *, struct device_attribute *, char *);
static ssize_t yas_gyro_filter_enable_store(struct device *, struct device_attribute *, const char *, size_t);
static ssize_t yas_gyro_wake_store(struct device *, struct device_attribute *, const char *, size_t);
static ssize_t yas_gyro_private_data_show(struct device *, struct device_attribute *, char *);
#if DEBUG
static ssize_t yas_gyro_debug_reg_show(struct device *, struct device_attribute *, char *);
#endif
#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
static ssize_t yas_gyro_selftest_enable_store(struct device *,struct device_attribute *, const char *, size_t);
static ssize_t yas_gyro_selftest_result_show(struct device *dev, struct device_attribute *attr, char *buf);
#endif
#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0   /*i2c */
static int yas_gyro_suspend(struct i2c_client *, pm_message_t);
static int yas_gyro_resume(struct i2c_client *);
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1 /* spi */
static int yas_gyro_suspend(struct spi_device *, pm_message_t);
static int yas_gyro_resume(struct spi_device *);
#endif

static void yas_gyro_work_func(struct work_struct *);
#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0   /* i2c */
static int yas_gyro_probe(struct i2c_client *, const struct i2c_device_id *);
static int yas_gyro_remove(struct i2c_client *);
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1 /* spi */
static int yas_gyro_probe(struct spi_device *);
static int yas_gyro_remove(struct spi_device *);
#endif

/* ---------------------------------------------------------------------------------------- *
   Driver private data
 * ---------------------------------------------------------------------------------------- */
struct yas_gyro_private_data {
#ifndef PANTECH_AVOID_DEADLOCK
    struct mutex driver_mutex;
    struct mutex data_mutex;
#endif
#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0   /* i2c */
    struct i2c_client *client;
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1 /* spi */
    struct spi_device *client;
#endif
    struct input_dev *input;
    struct yas_gyro_driver *driver;
#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
    struct work_struct work;
#else
    struct delayed_work work;
#endif /* defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT) */
    struct yas_gyro_data last;
    int suspend;
    int suspend_enable;
#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
    int interrupt_enable;
#endif /* defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT) */
    int p_time;
};

static struct yas_gyro_private_data *yas_gyro_private_data = NULL;
static struct yas_gyro_private_data *yas_gyro_get_data(void) {return yas_gyro_private_data;}
static void yas_gyro_set_data(struct yas_gyro_private_data *data) {yas_gyro_private_data = data;}
static struct yas_gyro_data gyro_data[YAS_GYRO_FIFO_MAX];


/* ---------------------------------------------------------------------------------------- *
   Local function
 * ---------------------------------------------------------------------------------------- */
#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
static void yas_gyro_current_time(int32_t *sec, int32_t *msec)
{
    struct timeval tv;
    do_gettimeofday(&tv);
    *sec = tv.tv_sec;
    *msec = (tv.tv_usec % 1000 == 0) ? tv.tv_usec / 1000 : tv.tv_usec / 1000 + 1;
}
#endif
/* ---------------------------------------------------------------------------------------- *
   Gyroelerlomete core driver callback function
 * ---------------------------------------------------------------------------------------- */
static int yas_gyro_lock(void)
{
#ifndef PANTECH_AVOID_DEADLOCK
    struct yas_gyro_private_data *data = yas_gyro_get_data();

    mutex_lock(&data->driver_mutex);
#endif

    return 0;
}

static int yas_gyro_unlock(void)
{
#ifndef PANTECH_AVOID_DEADLOCK
    struct yas_gyro_private_data *data = yas_gyro_get_data();

    mutex_unlock(&data->driver_mutex);
#endif

    return 0;
}

static int yas_gyro_device_open(void)
{
    return 0;
}

static int yas_gyro_device_close(void)
{
    return 0;
}

#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0    /* i2c */
static int yas_gyro_device_write(uint8_t adr, const uint8_t *buf, int len)
{
    struct yas_gyro_private_data *data = yas_gyro_get_data();
    uint8_t buffer[16];

    if (len <= 0 || len > sizeof(buffer)-1 || buf == NULL) {
        return -1;
    }

    buffer[0] = adr;
    memcpy(&buffer[1], buf, len);

    if (i2c_master_send(data->client, buffer, len+1) < 0) {
        return -1;
    }

    return 0;
}
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1  /* spi */
static int yas_gyro_device_write(uint8_t adr, const uint8_t *buf, int len)
{
    struct yas_gyro_private_data *data = yas_gyro_get_data();
    uint8_t buffer[16];
    int ret;

    if (len <= 0 || len > sizeof(buffer)-1 ||  buf == NULL) {
        return -1;
    }

#if YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D
    adr |= (len > 1 ? 0x40 : 0x00);
#endif
    buffer[0] = adr;
    memcpy(&buffer[1], buf, len);

    ret = spi_write(data->client, buffer, len+1);
    if (ret != 0) {
        return -1;
    }

    return 0;
}
#endif

#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0    /* i2c */
static int yas_gyro_device_read(uint8_t adr, uint8_t *buf, int len)
{
    struct yas_gyro_private_data *data = yas_gyro_get_data();
    uint8_t reg;
    int err;

#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
	//printk("%s : adr=0x%02X slaveaddr=0x%02X\n", __func__, adr, data->client->addr);
	reg = (len>1) ? (adr|0x80) : adr;
	err = i2c_master_send(data->client,  &reg, 1);
	if(err != 1) {
		printk("i2c_master_send error(%d).\n", err);
		return err;
	}

	err = i2c_master_recv(data->client, buf, len);
	if(err != len) {
		printk("i2c_master_recv error(%d).\n", err);
		return err;
	}
#else
    struct i2c_msg msg[2];
    reg = adr;
    msg[0].addr = data->client->addr;
    msg[0].flags = 0;
    msg[0].len = 1;
    msg[0].buf = &reg;
    msg[1].addr = data->client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].len = len;
    msg[1].buf = buf;

    err = i2c_transfer(data->client->adapter, msg, 2);
    if (err != 2) {
        dev_err(&data->client->dev,
                "i2c_transfer() read error: slave_addr=%02x, reg_addr=%02x, err=%d\n", data->client->addr, adr, err);
        return err;
    }
#endif // defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)

    return 0;

}
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1
static int yas_gyro_device_read(uint8_t adr, uint8_t *buf, int len)
{
    struct yas_gyro_private_data *data = yas_gyro_get_data();
    uint8_t rx_buf[16];
    uint8_t tx_buf[sizeof(rx_buf)];
    struct spi_transfer xfer = {
        .tx_buf = tx_buf,
        .rx_buf = rx_buf,
        .len = len + 1
    };
    struct spi_message msg;

    if (len <= 0 || len > sizeof(rx_buf)-1 || buf == NULL) {
        return -1;
    }

#if YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D
    adr = (adr | 0x80) | (len > 1 ? 0x40 : 0x00);
#endif
    tx_buf[0] = adr;
    memset(&tx_buf[1], 0xff, len);

    spi_message_init(&msg);
    spi_message_add_tail(&xfer, &msg);
    spi_sync(data->client, &msg);

    memcpy(buf, &rx_buf[1], len);

    return 0;
}
#endif

#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
static int yas_gyro_interrupt_enable(void)
{
    struct yas_gyro_private_data *data = yas_gyro_get_data();

    if (data->interrupt_enable == 0) {
        enable_irq(data->client->irq);
        data->interrupt_enable = 1;
    }

    return 0;
}

static int yas_gyro_interrupt_disable(void)
{
    struct yas_gyro_private_data *data = yas_gyro_get_data();

    if (data->interrupt_enable == 1) {
        disable_irq(data->client->irq);
        data->interrupt_enable = 0;
    }

    return 0;
}

static void yas_gyro_interrupt_notify(int num)
{
    struct yas_gyro_private_data *data = yas_gyro_get_data();

    schedule_work(&data->work);
}

static irqreturn_t yas_gyro_interrupt_handler(int irq, void *dev)
{
    struct yas_gyro_private_data *data = yas_gyro_get_data();
    struct yas_gyro_driver *driver = data->driver;

    if (driver->interrupt_handler) {
        driver->interrupt_handler();
    }

    return IRQ_HANDLED;
}

#endif /* defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT) */

static void yas_gyro_msleep(int msec)
{
    msleep(msec);
}

/* ---------------------------------------------------------------------------------------- *
   Gyroscope core driver access function
 * ---------------------------------------------------------------------------------------- */
static int yas_gyro_core_driver_init(struct yas_gyro_private_data *data)
{
    struct yas_gyro_driver_callback *cbk;
    struct yas_gyro_driver *driver;
    int interrupt;
    int err;

    data->driver = driver = kzalloc(sizeof(struct yas_gyro_driver), GFP_KERNEL);
    if (!driver) {
        err = -ENOMEM;
        return err;
    }

#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
    data->interrupt_enable = 1;
#endif
    cbk = &driver->callback;
    cbk->lock = yas_gyro_lock;
    cbk->unlock = yas_gyro_unlock;
    cbk->device_open = yas_gyro_device_open;
    cbk->device_close = yas_gyro_device_close;
    cbk->device_write = yas_gyro_device_write;
    cbk->device_read = yas_gyro_device_read;
#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
    cbk->interrupt_enable = yas_gyro_interrupt_enable;
    cbk->interrupt_disable = yas_gyro_interrupt_disable;
    cbk->interrupt_notify = yas_gyro_interrupt_notify;
    interrupt = 1;
#else
    cbk->interrupt_enable = NULL;
    cbk->interrupt_disable = NULL;
    cbk->interrupt_notify = NULL;
    interrupt = 0;
#endif
    cbk->msleep = yas_gyro_msleep;

    err = yas_gyro_driver_init(driver, interrupt);
    if (err != YAS_NO_ERROR) {
        kfree(driver);
        return err;
    }

    err = driver->init();
    if (err != YAS_NO_ERROR) {
        kfree(driver);
        return err;
    }

    err = driver->set_position(CONFIG_INPUT_YAS_GYROSCOPE_POSITION);
    if (err != YAS_NO_ERROR) {
        kfree(driver);
        return err;
    }

    return 0;
}

static void yas_gyro_core_driver_fini(struct yas_gyro_private_data *data)
{
    struct yas_gyro_driver *driver = data->driver;

    driver->term();
    kfree(driver);
}

static int yas_gyro_get_enable(struct yas_gyro_driver *driver)
{
    return driver->get_enable();
}

static int yas_gyro_set_enable(struct yas_gyro_driver *driver, int enable)
{
#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
#else
    struct yas_gyro_private_data *data = yas_gyro_get_data();
    int delay = driver->get_delay();
#endif

#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
    driver->set_enable(enable);
#else
    if (enable) {
        driver->set_enable(enable);
        schedule_delayed_work(&data->work, delay_to_jiffies(delay) + 1);
    } else {
        cancel_delayed_work_sync(&data->work);
        driver->set_enable(enable);
    }
#endif /* #if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT) */
    return 0;
}

static int yas_gyro_get_delay(struct yas_gyro_driver *driver)
{
    return driver->get_delay();
}

static int yas_gyro_set_delay(struct yas_gyro_driver *driver, int delay)
{
#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
    driver->set_delay(delay);
#else
    struct yas_gyro_private_data *data = yas_gyro_get_data();

    if (driver->get_enable()) {
        cancel_delayed_work_sync(&data->work);
        driver->set_delay(delay);
        schedule_delayed_work(&data->work, delay_to_jiffies(delay) + 1);

    } else {
        driver->set_delay(delay);
    }
#endif /* defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT) */

    return 0;
}

static int yas_gyro_get_position(struct yas_gyro_driver *driver)
{
    return driver->get_position();
}

static int yas_gyro_set_position(struct yas_gyro_driver *driver, int position)
{
    return driver->set_position(position);
}

static int yas_gyro_get_offset(struct yas_gyro_driver *driver, struct yas_vector *offset)
{
    return driver->get_offset(offset);
}

static int yas_gyro_set_offset(struct yas_gyro_driver *driver, struct yas_vector *offset)
{
    return driver->set_offset(offset);
}

static int yas_gyro_get_threshold(struct yas_gyro_driver *driver)
{
    struct yas_gyro_filter filter;

    driver->get_filter(&filter);

    return filter.threshold;
}

static int yas_gyro_set_threshold(struct yas_gyro_driver *driver, int threshold)
{
    struct yas_gyro_filter filter;

    filter.threshold = threshold;

    return driver->set_filter(&filter);
}

static int yas_gyro_get_filter_enable(struct yas_gyro_driver *driver)
{
    return driver->get_filter_enable();
}

static int yas_gyro_set_filter_enable(struct yas_gyro_driver *driver, int enable)
{
    return driver->set_filter_enable(enable);
}

static int yas_gyro_measure(struct yas_gyro_driver *driver, struct yas_gyro_data *gyro, int num)
{
    int actual_num;

    actual_num = driver->measure(gyro, num);

    return actual_num;
}

#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
static int yas_gyro_set_selftest_enable(struct yas_gyro_driver *driver, int enable)
{
    struct yas_gyro_private_data *data = yas_gyro_get_data();
	int running = driver->get_enable();
	int result = 0;
	int err = 0;

	if(!enable) return enable;

	if(running) {
		yas_gyro_set_enable(data->driver, !running);
	}

	err = driver->set_selftest_enable(enable, &result);
//	if(err == YAS_NO_ERROR) input_report_abs(data->input, ABS_GAS, result);

	if(running) yas_gyro_set_enable(data->driver, running);

	return err;
}

static int yas_gyro_get_selftest_result(struct yas_gyro_driver *driver)
{
	int result = driver->get_selftest_result();
	return result;
}
#endif

/* ---------------------------------------------------------------------------------------- *
   Input device interface
 * ---------------------------------------------------------------------------------------- */
static int yas_gyro_input_init(struct yas_gyro_private_data *data)
{
    struct input_dev *dev;
    int err;

    dev = input_allocate_device();
    if (!dev) {
        return -ENOMEM;
    }
    dev->name = "gyroscope";
#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0
    dev->id.bustype = BUS_I2C;
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1
    dev->id.bustype = BUS_HOST;
#endif
    input_set_capability(dev, EV_ABS, ABS_MISC);
    input_set_abs_params(dev, ABS_X, ABSMIN_DPS, ABSMAX_DPS, 0, 0);
    input_set_abs_params(dev, ABS_Y, ABSMIN_DPS, ABSMAX_DPS, 0, 0);
    input_set_abs_params(dev, ABS_Z, ABSMIN_DPS, ABSMAX_DPS, 0, 0);
#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
    input_set_abs_params(dev, ABS_GAS, ABSMIN_DPS, ABSMAX_DPS, 0, 0);
#endif
    input_set_drvdata(dev, data);

    err = input_register_device(dev);
    if (err < 0) {
        input_free_device(dev);
        return err;
    }
    data->input = dev;

    return 0;
}

static void yas_gyro_input_fini(struct yas_gyro_private_data *data)
{
    struct input_dev *dev = data->input;

    input_unregister_device(dev);
    input_free_device(dev);
}

static ssize_t yas_gyro_enable_show(struct device *dev,
                                    struct device_attribute *attr,
                                    char *buf)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);

    return sprintf(buf, "%d\n", yas_gyro_get_enable(data->driver));
}

static ssize_t yas_gyro_enable_store(struct device *dev,
                                     struct device_attribute *attr,
                                     const char *buf,
                                     size_t count)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);
    unsigned long enable = simple_strtoul(buf, NULL, 10);

    yas_gyro_set_enable(data->driver, enable);

    return count;
}

static ssize_t yas_gyro_delay_show(struct device *dev,
                                   struct device_attribute *attr,
                                   char *buf)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);

    return sprintf(buf, "%d\n", yas_gyro_get_delay(data->driver));
}

static ssize_t yas_gyro_delay_store(struct device *dev,
                                    struct device_attribute *attr,
                                    const char *buf,
                                    size_t count)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);
    unsigned long delay = simple_strtoul(buf, NULL, 10);

    yas_gyro_set_delay(data->driver, delay);

    return count;
}

static ssize_t yas_gyro_position_show(struct device *dev,
                                     struct device_attribute *attr,
                                     char *buf)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);

    return sprintf(buf, "%d\n", yas_gyro_get_position(data->driver));
}

static ssize_t yas_gyro_position_store(struct device *dev,
                                       struct device_attribute *attr,
                                       const char *buf,
                                       size_t count)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);
    unsigned long position = simple_strtoul(buf, NULL,10);

    yas_gyro_set_position(data->driver, position);

    return count;
}

static ssize_t yas_gyro_offset_show(struct device *dev,
                                    struct device_attribute *attr,
                                    char *buf)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);
    struct yas_vector offset;

    yas_gyro_get_offset(data->driver, &offset);

    return sprintf(buf, "%d %d %d\n", offset.v[0], offset.v[1], offset.v[2]);
}

static ssize_t yas_gyro_offset_store(struct device *dev,
                                     struct device_attribute *attr,
                                     const char *buf,
                                     size_t count)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);
    struct yas_vector offset;

    sscanf(buf, "%d %d %d", &offset.v[0], &offset.v[1], &offset.v[2]);
    yas_gyro_set_offset(data->driver, &offset);

    return count;
}

static ssize_t yas_gyro_threshold_show(struct device *dev,
                                       struct device_attribute *attr,
                                       char *buf)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);

    return sprintf(buf, "%d\n", yas_gyro_get_threshold(data->driver));
}

static ssize_t yas_gyro_threshold_store(struct device *dev,
                                        struct device_attribute *attr,
                                        const char *buf,
                                        size_t count)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);
    unsigned long threshold = simple_strtoul(buf, NULL,10);

    yas_gyro_set_threshold(data->driver, threshold);

    return count;
}

static ssize_t yas_gyro_filter_enable_show(struct device *dev,
                                           struct device_attribute *attr,
                                           char *buf)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);

    return sprintf(buf, "%d\n", yas_gyro_get_filter_enable(data->driver));
}

static ssize_t yas_gyro_filter_enable_store(struct device *dev,
                                            struct device_attribute *attr,
                                            const char *buf,
                                            size_t count)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);
    unsigned long enable = simple_strtoul(buf, NULL,10);;

    yas_gyro_set_filter_enable(data->driver, enable);

    return count;
}

static ssize_t yas_gyro_wake_store(struct device *dev,
                                   struct device_attribute *attr,
                                   const char *buf,
                                   size_t count)
{
    struct input_dev *input = to_input_dev(dev);
    static atomic_t serial = ATOMIC_INIT(0);

    input_report_abs(input, ABS_MISC, atomic_inc_return(&serial));

    return count;
}

static ssize_t yas_gyro_private_data_show(struct device *dev,
                                          struct device_attribute *attr,
                                          char *buf)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);
    struct yas_gyro_data gyro;

#ifdef PANTECH_AVOID_DEADLOCK
    gyro = data->last;
#else
	mutex_lock(&data->data_mutex);
    gyro = data->last;
    mutex_unlock(&data->data_mutex);
#endif

    return sprintf(buf, "%d %d %d\n", gyro.xyz.v[0], gyro.xyz.v[1], gyro.xyz.v[2]);
}

#if DEBUG
#if YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D
#define ADR_MAX (0x39)
#else
#define ADR_MAX (0x16)
#endif
static uint8_t reg[ADR_MAX];

static ssize_t yas_gyro_debug_reg_show(struct device *dev,
                                       struct device_attribute *attr,
                                       char *buf)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);
    ssize_t count = 0;
    int ret;
    int i;

    memset(reg, -1, ADR_MAX);
    for (i = 0; i < ADR_MAX; i++) {
        ret = data->driver->get_register(i, &reg[i]);
        if(ret != 0) {
            dev_err(dev, "get_register() erorr %d (%d)\n", ret, i);
        } else {
            count += sprintf(&buf[count], "%02x: %d\n", i, reg[i]);
        }
    }

    return count;
}

static ssize_t yas_gyro_debug_suspend_show(struct device *dev,
                                           struct device_attribute *attr,
                                           char *buf)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);

    return sprintf(buf, "%d\n", data->suspend);
}

static ssize_t yas_gyro_debug_suspend_store(struct device *dev,
                                            struct device_attribute *attr,
                                            const char *buf, size_t count)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);
#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0   /* i2c */
    struct i2c_client *client = data->client;
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1
    struct spi_device *client = data->client;
#endif
    unsigned long suspend = simple_strtoul(buf, NULL, 10);
    pm_message_t msg;

    if (suspend) {
        yas_gyro_suspend(client, msg);
    } else {
        yas_gyro_resume(client);
    }

    return count;
}
#endif /* DEBUG */

#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
static ssize_t yas_gyro_selftest_enable_store(struct device *dev,
                                     struct device_attribute *attr,
                                     const char *buf,
                                     size_t count)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);
    unsigned long selftest_enable = simple_strtoul(buf, NULL, 10);

    yas_gyro_set_selftest_enable(data->driver, selftest_enable);

    return count;
}

static ssize_t yas_gyro_selftest_result_show(struct device *dev,
                                    struct device_attribute *attr,
                                    char *buf)
{
    struct input_dev *input = to_input_dev(dev);
    struct yas_gyro_private_data *data = input_get_drvdata(input);

    return sprintf(buf, "%d\n", yas_gyro_get_selftest_result(data->driver));
}
#endif

static DEVICE_ATTR(enable,
                   S_IRUGO|S_IWUSR|S_IWGRP,
                   yas_gyro_enable_show,
                   yas_gyro_enable_store
                   );
static DEVICE_ATTR(delay,
                   S_IRUGO|S_IWUSR|S_IWGRP,
                   yas_gyro_delay_show,
                   yas_gyro_delay_store
                   );
static DEVICE_ATTR(threshold,
                   S_IRUGO|S_IWUSR,
                   yas_gyro_threshold_show,
                   yas_gyro_threshold_store
                   );
static DEVICE_ATTR(filter_enable,
                   S_IRUGO|S_IWUSR,
                   yas_gyro_filter_enable_show,
                   yas_gyro_filter_enable_store
                   );
static DEVICE_ATTR(position,
                   S_IRUGO|S_IWUSR,
                   yas_gyro_position_show,
                   yas_gyro_position_store
                   );
static DEVICE_ATTR(offset,
                   S_IRUGO|S_IWUSR,
                   yas_gyro_offset_show,
                   yas_gyro_offset_store
                   );
static DEVICE_ATTR(wake,
                   S_IWUSR|S_IWGRP,
                   NULL,
                   yas_gyro_wake_store);
static DEVICE_ATTR(data,
                   S_IRUGO,
                   yas_gyro_private_data_show,
                   NULL);
#if DEBUG
static DEVICE_ATTR(debug_reg,
                   S_IRUGO,
                   yas_gyro_debug_reg_show,
                   NULL
                   );
static DEVICE_ATTR(debug_suspend,
                   S_IRUGO|S_IWUSR,
                   yas_gyro_debug_suspend_show,
                   yas_gyro_debug_suspend_store
                   );
#endif /* DEBUG */
#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
static DEVICE_ATTR(selftest_enable,
                   S_IRUGO|S_IWUSR,
                   NULL,
                   yas_gyro_selftest_enable_store
                   );
static DEVICE_ATTR(selftest_result,
                   S_IRUGO|S_IWUSR,
                   yas_gyro_selftest_result_show,
                   NULL
                   );
#endif

static struct attribute *yas_gyro_attributes[] = {
    &dev_attr_enable.attr,
    &dev_attr_delay.attr,
    &dev_attr_position.attr,
    &dev_attr_wake.attr,
    &dev_attr_data.attr,
    &dev_attr_offset.attr,
    &dev_attr_threshold.attr,
    &dev_attr_filter_enable.attr,
#if DEBUG
    &dev_attr_debug_reg.attr,
    &dev_attr_debug_suspend.attr,
#endif /* DEBUG */
#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
    &dev_attr_selftest_enable.attr,
    &dev_attr_selftest_result.attr,
#endif
    NULL
};

static struct attribute_group yas_gyro_attribute_group = {
    .attrs = yas_gyro_attributes
};

static void yas_gyro_work_func(struct work_struct *work)
{
#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
    struct yas_gyro_private_data *data = container_of((struct work_struct *)work, struct yas_gyro_private_data, work);
    int32_t sec, msec, c_time, x_time;
    int num;
#else
    struct yas_gyro_private_data *data = container_of((struct delayed_work *)work, struct yas_gyro_private_data, work);
#endif /* defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT) */
    unsigned long delay = yas_gyro_get_delay(data->driver);

#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
    x_time = 0;
    yas_gyro_current_time(&sec, &msec);
    if (msec == 1000) {
        sec++;
        msec = 0;
    }
    c_time = sec * 1000 + msec;
    x_time = c_time - data->p_time;
    data->p_time = c_time;

    memset(gyro_data, 0, NELEMS(gyro_data));
    num = yas_gyro_measure(data->driver, gyro_data, YAS_GYRO_FIFO_MAX);
    if (num < 1 || x_time < delay) {
        return;
    }

    input_report_abs(data->input, ABS_X, gyro_data[num-1].xyz.v[0]);
    input_report_abs(data->input, ABS_Y, gyro_data[num-1].xyz.v[1]);
    input_report_abs(data->input, ABS_Z, gyro_data[num-1].xyz.v[2]);
    input_sync(data->input);

#ifdef PANTECH_AVOID_DEADLOCK
    data->last = gyro_data[num -1];
#else
	mutex_lock(&data->data_mutex);
    data->last = gyro_data[num -1];
    mutex_unlock(&data->data_mutex);
#endif // PANTECH_AVOID_DEADLOCK
#else /* defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT) */
    memset(gyro_data, 0, NELEMS(gyro_data));
    yas_gyro_measure(data->driver, gyro_data, 1);
    input_report_abs(data->input, ABS_X, gyro_data[0].xyz.v[0]);
    input_report_abs(data->input, ABS_Y, gyro_data[0].xyz.v[1]);
    input_report_abs(data->input, ABS_Z, gyro_data[0].xyz.v[2]);
    input_sync(data->input);
#ifdef PANTECH_AVOID_DEADLOCK
    data->last = gyro_data[0];
#else
	mutex_lock(&data->data_mutex);
    data->last = gyro_data[0];
    mutex_unlock(&data->data_mutex);
#endif
	schedule_delayed_work(&data->work, delay_to_jiffies(delay) + 1);
#endif
}

#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0   /* i2c */
static int yas_gyro_probe(struct i2c_client *client, const struct i2c_device_id *id)
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1 /* spi */
static int yas_gyro_probe(struct spi_device *client)
#endif
{
    struct yas_gyro_private_data *data;
    int err;

    /* Setup private data */
    data = kzalloc(sizeof(struct yas_gyro_private_data), GFP_KERNEL);
    if (!data) {
        err = -ENOMEM;
        goto ERR1;
    }
    yas_gyro_set_data(data);

#ifndef PANTECH_AVOID_DEADLOCK
    mutex_init(&data->driver_mutex);
    mutex_init(&data->data_mutex);
#endif

#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0   /*i2c */
    /* Setup i2c client */
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
        err = -ENODEV;
        goto ERR2;
    }
    i2c_set_clientdata(client, data);
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1 /* spi */
    /* Setup spi device */
    spi_set_drvdata(client, data);
#endif
    data->client = client;

    /* Setup gyroscope core driver */
    err = yas_gyro_core_driver_init(data);
    if (err < 0) {
        goto ERR2;
    }

#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
    /* Setup driver interface */
    INIT_WORK(&data->work, yas_gyro_work_func);
#else
    /* Setup driver interface */
    INIT_DELAYED_WORK(&data->work, yas_gyro_work_func);
#endif /* defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT) */

    /* Setup input device interface */
    err = yas_gyro_input_init(data);
    if (err < 0) {
        goto ERR3;
    }

    /* Setup sysfs */
    err = sysfs_create_group(&data->input->dev.kobj, &yas_gyro_attribute_group);
    if (err < 0) {
        goto ERR4;
    }

#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
    /* Setup interrupt */
    if (request_irq(data->client->irq, yas_gyro_interrupt_handler, 0, "Ext-Bus", NULL)) {
        printk(KERN_INFO "request_irq error\n");
        return -ENODEV;
    }
#endif /* defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT) */

    return 0;

 ERR4:
    yas_gyro_input_fini(data);
 ERR3:
    yas_gyro_core_driver_fini(data);
 ERR2:
    kfree(data);
 ERR1:
    return err;
}

#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0   /*i2c */
static int yas_gyro_remove(struct i2c_client *client)
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1 /* spi */
static int yas_gyro_remove(struct spi_device *client)
#endif
{
#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0   /*i2c */
    struct yas_gyro_private_data *data = i2c_get_clientdata(client);
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1 /* spi */
    struct yas_gyro_private_data *data = dev_get_drvdata((struct device *)client);
#endif
    struct yas_gyro_driver *driver = data->driver;

#if defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
    free_irq(data->client->irq, NULL);
#endif /* defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT) */
    yas_gyro_set_enable(driver, 0);
    sysfs_remove_group(&data->input->dev.kobj, &yas_gyro_attribute_group);
    yas_gyro_input_fini(data);
    yas_gyro_core_driver_fini(data);
    kfree(data);

    return 0;
}

#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0   /*i2c */
static int yas_gyro_suspend(struct i2c_client *client, pm_message_t mesg)
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1 /* spi */
static int yas_gyro_suspend(struct spi_device *client, pm_message_t mesg)
#endif
{
#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0   /*i2c */
    struct yas_gyro_private_data *data = i2c_get_clientdata((struct i2c_client *)client);
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1 /* spi */
    struct yas_gyro_private_data *data = dev_get_drvdata((struct device *)client);
#endif
    struct yas_gyro_driver *driver = data->driver;

    (void)mesg;

    data->suspend_enable = yas_gyro_get_enable(driver);
    if (data->suspend_enable) {
#if !defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
        cancel_delayed_work_sync(&data->work);
#endif /* defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT) */

        yas_gyro_set_enable(driver, 0);
    }

    return 0;
}

#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0   /*i2c */
static int yas_gyro_resume(struct i2c_client *client)
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1 /* spi */
static int yas_gyro_resume(struct spi_device *client)
#endif
{
#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0   /*i2c */
    struct yas_gyro_private_data *data = i2c_get_clientdata(client);
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1 /* spi */
    struct yas_gyro_private_data *data = dev_get_drvdata((struct device *)client);
#endif
    struct yas_gyro_driver *driver = data->driver;
    int delay;

    if (data->suspend_enable) {
        delay = yas_gyro_get_delay(driver);
#if !defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT)
        schedule_delayed_work(&data->work, delay_to_jiffies(delay) + 1);
#endif /* defined(CONFIG_INPUT_YAS_GYROSCOPE_INTERRUPT) */
        yas_gyro_set_enable(driver, 1);
    }

    return 0;
}

#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0     /* i2c */
static const struct i2c_device_id yas_gyro_id[] = {
    {YAS_GYRO_KERNEL_NAME, 0},
    {},
};

MODULE_DEVICE_TABLE(i2c, yas_gyro_id);
struct i2c_driver yas_gyro_driver = {
    .driver = {
        .name = "gyroscope",
        .owner = THIS_MODULE,
    },
    .probe = yas_gyro_probe,
    .remove = yas_gyro_remove,
    .suspend = yas_gyro_suspend,
    .resume = yas_gyro_resume,
    .id_table = yas_gyro_id,
};
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1 /* spi */
struct spi_driver yas_gyro_driver = {
    .driver = {
        .name = "gyroscope",
        .owner = THIS_MODULE,
    },
    .probe = yas_gyro_probe,
    .remove = yas_gyro_remove,
    .suspend = yas_gyro_suspend,
    .resume = yas_gyro_resume,
};
#endif

/* ---------------------------------------------------------------------------------------- *
   Module init and exit
 * ---------------------------------------------------------------------------------------- */
static int __init yas_gyro_init(void)
{
#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0
    return i2c_add_driver(&yas_gyro_driver);
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1
    return spi_register_driver(&yas_gyro_driver);
#endif
}
module_init(yas_gyro_init);

static void __exit yas_gyro_exit(void)
{
#if CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 0
    i2c_del_driver(&yas_gyro_driver);
#elif CONFIG_INPUT_YAS_GYROSCOPE_INTERFACE == 1
    spi_unregister_driver(&yas_gyro_driver);
#endif
}
module_exit(yas_gyro_exit);

MODULE_DESCRIPTION("gyro kernel driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(YAS_GYRO_KERNEL_VERSION);
