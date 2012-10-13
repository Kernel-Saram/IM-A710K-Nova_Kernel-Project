/*
 * Copyright (c) 2011 Yamaha Corporation
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "yas.h"

/* Default parameters */
#define YAS_L3G4200D_DEFAULT_DELAY                                          (100)
#ifdef PANTECH_FEATURE_SENSOR
#define YAS_L3G4200D_DEFAULT_POSITION CONFIG_INPUT_YAS_GYROSCOPE_POSITION
#else
#define YAS_L3G4200D_DEFAULT_POSITION                                         (0)
#endif

#define YAS_L3G4200D_MAX_DELAY                                              (200)
#define YAS_L3G4200D_MIN_DELAY_INT                                            (0)
#define YAS_L3G4200D_MIN_DELAY_POL                                           (10)
#define YAS_L3G4200D_MAX_POSITION                                             (7)
#define YAS_L3G4200D_MIN_POSITION                                             (0)
#define YAS_L3G4200D_MAX_DATA                                           (2000000)
#define YAS_L3G4200D_MIN_DATA                                          (-2000000)

#define YAS_L3G4200D_POWERUP_TIME                                           (350)

/* Register */
#define YAS_L3G4200D_WHOAMI_REG                                            (0x0F)
#define YAS_L3G4200D_WHOAMI_ID                                             (0xD3)

#define YAS_L3G4200D_FS_REG                                                (0x23)
#define YAS_L3G4200D_FS_MASK                                               (0x30)
#define YAS_L3G4200D_FS_SHIFT                                                 (4)
#define YAS_L3G4200D_FS_2000                                                  (2)
#define YAS_L3G4200D_FS_500                                                   (1)
#define YAS_L3G4200D_FS_250                                                   (0)

#define YAS_L3G4200D_BDU_REG                                               (0x23)
#define YAS_L3G4200D_BDU_MASK                                              (0x80)
#define YAS_L3G4200D_BDU_SHIFT                                                (7)

#define YAS_L3G4200D_ODR_REG                                               (0x20)
#define YAS_L3G4200D_ODR_MASK                                              (0xF0)
#define YAS_L3G4200D_ODR_SHIFT                                                (4)
#define YAS_L3G4200D_ODR_800HZ                                               (12)
#define YAS_L3G4200D_ODR_400HZ                                                (8)
#define YAS_L3G4200D_ODR_200HZ                                                (4)
#define YAS_L3G4200D_ODR_100HZ                                                (0)

#define YAS_L3G4200D_AXIS_REG                                              (0x20)
#define YAS_L3G4200D_AXIS_MASK                                             (0x07)
#define YAS_L3G4200D_AXIS_SHIFT                                               (0)
#define YAS_L3G4200D_AXIS_ENABLE                                           (0x07)
#define YAS_L3G4200D_AXIS_DISABLE                                          (0x00)

#define YAS_L3G4200D_INTERRUPT_REG                                         (0x22)
#define YAS_L3G4200D_INTERRUPT_MASK                                        (0x06)
#define YAS_L3G4200D_INTERRUPT_SHIFT                                          (1)
#define YAS_L3G4200D_INTERRUPT_ENABLE                                         (3)
#define YAS_L3G4200D_INTERRUPT_DISABLE                                        (0)

#define YAS_L3G4200D_POWER_REG                                             (0x20)
#define YAS_L3G4200D_POWER_MASK                                            (0x08)
#define YAS_L3G4200D_POWER_SHIFT                                              (3)

#define YAS_L3G4200D_FIFO_REG                                              (0x2E)
#define YAS_L3G4200D_FIFO_BYPASS                                           (0x00)
#define YAS_L3G4200D_FIFO_FIFO                                             (0x20)
#define YAS_L3G4200D_FIFO_STREAM                                           (0x40)
#define YAS_L3G4200D_FIFO_STREAM_FIFO                                      (0x60)
#define YAS_L3G4200D_FIFO_BYPASS_STREAM                                    (0x80)
#define YAS_L3G4200D_FIFO_MAX                                   YAS_GYRO_FIFO_MAX

#define YAS_L3G4200D_FIFO_ENABLE_REG                                       (0x24)
#define YAS_L3G4200D_FIFO_ENABLE_MASK                                      (0x40)
#define YAS_L3G4200D_FIFO_ENABLE_SHIFT                                        (6)
#define YAS_L3G4200D_FIFO_ENABLE_ENABLE                                    (0x5c)
#define YAS_L3G4200D_FIFO_ENABLE_DISABLE                                   (0x1c)

#define YAS_L3G4200D_FIFO_STATUS_REG                                       (0x2F)

#define YAS_L3G4200D_STATUS_REG                                            (0x27)
#define YAS_L3G4200D_DATA_REG                                              (0x28)

#define YAS_L3G4200D_RESOLUTION                                              (70)
#define YAS_L3G4200D_INITIAL_DATA_TIME                                      (320)

/* --------------------------------------------------------------------------- */
/*  Structure definition                                                       */
/* --------------------------------------------------------------------------- */
/* Output data rate */
struct yas_l3g4200d_odr {
    unsigned long delay;               /* min delay (msec) in the range of ODR */
    unsigned char odr;                 /* bandwidth register value             */
};

/* Driver private data */
struct yas_l3g4200d_data {
    int initialize;
    int device_open;
    int enable;
    int delay;
    int actual_delay;
    int position;
    int threshold;
    int filter_enable;
    int interrupt;
    int overrun;
    int initial_data;
    int wtm;
    struct yas_gyro_data last; /* mdps */
    struct yas_vector offset;
#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
	atomic_t selftest_result;
#endif
};

#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)

#define SELFTEST_2000_DPS_MIN_DATA 175
#define SELFTEST_2000_DPS_MAX_DATA 875

struct yas_l3g4200d_selftest_data {
	int result;
	struct yas_vector min;
	struct yas_vector max;
	struct yas_vector normal_avg;
	struct yas_vector selftest_avg;
	struct yas_vector abs_avg;
	uint8_t status;
    uint8_t reg[5];
};
#endif

/* --------------------------------------------------------------------------- */
/*  Data                                                                       */
/* --------------------------------------------------------------------------- */
/* Control block */
static struct yas_gyro_driver    cb;
static struct yas_gyro_driver   *pcb = NULL;
static struct yas_l3g4200d_data  l3g4200d_data;
static uint8_t                   l3g4200d_buf[YAS_L3G4200D_FIFO_MAX * 6];

/* Output data rate */
static const struct yas_l3g4200d_odr yas_l3g4200d_odr_tbl[] = {
    {2,   YAS_L3G4200D_ODR_800HZ},
    {3,   YAS_L3G4200D_ODR_400HZ},
    {5,   YAS_L3G4200D_ODR_200HZ},
    {10,  YAS_L3G4200D_ODR_100HZ},
};

/* Transformation matrix for chip mounting position */
static const int yas_l3g4200d_position_map[][3][3] = {
    {{ 1,  0,  0}, { 0,  1,  0}, { 0,  0,  1}}, /* top/upper-left */
    {{ 0,  1,  0}, {-1,  0,  0}, { 0,  0,  1}}, /* top/upper-right */
    {{-1,  0,  0}, { 0, -1,  0}, { 0,  0,  1}}, /* top/lower-right */
    {{ 0, -1,  0}, { 1,  0,  0}, { 0,  0,  1}}, /* top/lower-left */
    {{-1,  0,  0}, { 0,  1,  0}, { 0,  0, -1}}, /* bottom/upper-left */
    {{ 0, -1,  0}, {-1,  0,  0}, { 0,  0, -1}}, /* bottom/upper-right */
    {{ 1,  0,  0}, { 0, -1,  0}, { 0,  0, -1}}, /* bottom/lower-right */
    {{ 0,  1,  0}, { 1,  0,  0}, { 0,  0, -1}}, /* bottom/lower-left */
};

/* --------------------------------------------------------------------------- */
/*  Prototype declaration                                                      */
/* --------------------------------------------------------------------------- */
static int  yas_l3g4200d_read_reg(unsigned char, unsigned char *, unsigned char);
static int  yas_l3g4200d_write_reg(unsigned char, unsigned char *, unsigned char);
static int  yas_l3g4200d_read_reg_byte(unsigned char);
static void yas_l3g4200d_init_offset(void);
static void yas_l3g4200d_init_lastvalue(void);
static void yas_l3g4200d_init_gyro_data(struct yas_gyro_data *);
static void yas_l3g4200d_init_data(void);
static void yas_l3g4200d_reset_fifo(void);
static void yas_l3g4200d_configure(void);
static int  yas_l3g4200d_ischg_enable(int);
static int  yas_l3g4200d_set_wtm(void);

static int  yas_l3g4200d_lock(void);
static int  yas_l3g4200d_unlock(void);
static int  yas_l3g4200d_device_open(void);
static int  yas_l3g4200d_device_close(void);
static int  yas_l3g4200d_interrupt_enable(void);
static int  yas_l3g4200d_interrupt_disable(void);
static void yas_l3g4200d_interrupt_notify(int);
static int  yas_l3g4200d_msleep(int);

static int  yas_l3g4200d_init(void);
static int  yas_l3g4200d_term(void);
static int  yas_l3g4200d_get_delay(void);
static int  yas_l3g4200d_set_delay(int);
static int  yas_l3g4200d_get_enable(void);
static int  yas_l3g4200d_set_enable(int);
#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
static int  yas_l3g4200d_set_selftest_enable(int enable, int *result);
static int  yas_l3g4200d_get_selftest_result(void);
#endif
static int  yas_l3g4200d_get_position(void);
static int  yas_l3g4200d_set_position(int);
static int  yas_l3g4200d_get_offset(struct yas_vector *);
static int  yas_l3g4200d_set_offset(struct yas_vector *);
static int  yas_l3g4200d_get_filter(struct yas_gyro_filter *);
static int  yas_l3g4200d_set_filter(struct yas_gyro_filter *);
static int  yas_l3g4200d_get_filter_enable(void);
static int  yas_l3g4200d_set_filter_enable(int);

static int  yas_l3g4200d_measure_fifo(struct yas_gyro_data *, int);
static int  yas_l3g4200d_measure_direct(struct yas_gyro_data *);
static int  yas_l3g4200d_measure(struct yas_gyro_data *, int);

/* --------------------------------------------------------------------------- */
/*  Local functions                                                            */
/* --------------------------------------------------------------------------- */
/* register access functions */
static int yas_l3g4200d_read_reg(unsigned char adr, unsigned char *buf, unsigned char len)
{
    struct yas_gyro_driver_callback *cbk = &pcb->callback;
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    int err;
    
    if (p->device_open) {
        err = cbk->device_read(adr, buf, len);
        if (err != 0) {
            return err;
        }
        return err;
    }

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_write_reg(unsigned char adr, unsigned char *buf, unsigned char len)
{
    struct yas_gyro_driver_callback *cbk = &pcb->callback;
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    int err;

    if (p->device_open) {
        err = cbk->device_write(adr, buf, len);
        if (err != 0) {
            return err;
        }
        return err;
    }

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_read_reg_byte(unsigned char adr)
{
    unsigned char buf;
    int err;

    err = yas_l3g4200d_read_reg(adr, &buf, 1);
    if (err == 0) {
        return buf;
    }

    return 0;
}

static int yas_l3g4200d_write_reg_byte(unsigned char adr, unsigned char val)
{
    return yas_l3g4200d_write_reg(adr, &val, 1);
}

#define yas_l3g4200d_read_bits(r) \
    ((yas_l3g4200d_read_reg_byte(r##_REG) & r##_MASK) >> r##_SHIFT)

#define yas_l3g4200d_update_bits(r,v) \
    yas_l3g4200d_write_reg_byte(r##_REG, \
                                ((yas_l3g4200d_read_reg_byte(r##_REG) & ~r##_MASK) | ((v) << r##_SHIFT)))

static void yas_l3g4200d_init_offset(void)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    int i;

    for (i = 0; i < 3; ++i) {
        p->offset.v[i] = 0;
    }
}

static void yas_l3g4200d_init_lastvalue(void)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    int i;

    for (i = 0; i < 3; ++i) {
        p->last.xyz.v[i] = 0;
        p->last.raw.v[i] = 0;
    }
}

static void yas_l3g4200d_init_gyro_data(struct yas_gyro_data *data)
{
    int i;

    for (i = 0; i < 3; ++i) {
        data->xyz.v[i] = 0;
        data->raw.v[i] = 0;
    }

    data->num = 0;
    data->overrun = 0;
}

static void yas_l3g4200d_init_data(void)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    p->initialize = 0;
    p->device_open = 0;
    p->enable = 0;
    p->delay = YAS_L3G4200D_DEFAULT_DELAY;
    p->position = YAS_L3G4200D_DEFAULT_POSITION;
    p->threshold = YAS_GYRO_DEFAULT_FILTER_THRESH;
    p->filter_enable = 0;
    p->overrun = 0;
    p->wtm = 0;
    yas_l3g4200d_init_offset();
    yas_l3g4200d_init_lastvalue();
}

static void yas_l3g4200d_reset_fifo(void)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    yas_l3g4200d_write_reg_byte(YAS_L3G4200D_FIFO_REG, 0);
    yas_l3g4200d_write_reg_byte(YAS_L3G4200D_FIFO_REG, YAS_L3G4200D_FIFO_FIFO | p->wtm);
}

static void yas_l3g4200d_configure(void)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    /* Powerup */
    yas_l3g4200d_update_bits(YAS_L3G4200D_POWER, 1);

    /* Set full scale */
    yas_l3g4200d_update_bits(YAS_L3G4200D_FS, YAS_L3G4200D_FS_2000);
 
    if (p->interrupt) {
        /* Enable interrupt */
        yas_l3g4200d_update_bits(YAS_L3G4200D_INTERRUPT, YAS_L3G4200D_INTERRUPT_ENABLE);

        /* Set BDU  */
        yas_l3g4200d_update_bits(YAS_L3G4200D_BDU, 1);

       /* Setting Fifo (fifo mode, WTM) */
        yas_l3g4200d_update_bits(YAS_L3G4200D_FIFO_ENABLE, 1);

        /* Reset fifo */
        yas_l3g4200d_reset_fifo();
    } else {
        /* Disable interrupt */
        yas_l3g4200d_update_bits(YAS_L3G4200D_INTERRUPT, YAS_L3G4200D_INTERRUPT_DISABLE);

        /* Set BDU  */
        yas_l3g4200d_update_bits(YAS_L3G4200D_BDU, 1);

        /* Setting Fifo (fifo mode, WTM) */
        yas_l3g4200d_update_bits(YAS_L3G4200D_FIFO_ENABLE, 0);

        /* Clear Fifo register */
        yas_l3g4200d_write_reg_byte(YAS_L3G4200D_FIFO_REG, 0);
    }
}

static int yas_l3g4200d_ischg_enable(int enable)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    if (p->enable == enable) {
        return 0;
    }

    return 1;
}

static int  yas_l3g4200d_set_wtm(void)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    int wtm;

    wtm = p->delay / p->actual_delay;
    if (wtm > YAS_L3G4200D_FIFO_MAX) {
        wtm = YAS_L3G4200D_FIFO_MAX;
        return 0;
    }

    p->wtm = (wtm > 2) ? wtm : 2;

    return 0;
}

static int yas_l3g4200d_lock(void)
{
#ifdef PANTECH_AVOID_DEADLOCK
	return 0;
#else
	struct yas_gyro_driver_callback *cbk = &pcb->callback;
    int err;

    if (cbk->lock != NULL && cbk->unlock != NULL) {
        err = cbk->lock();
    } else {
        err = YAS_NO_ERROR;
    }

    return err;
#endif
}

static int yas_l3g4200d_unlock(void)
{
#ifdef PANTECH_AVOID_DEADLOCK
	return 0;
#else
    struct yas_gyro_driver_callback *cbk = &pcb->callback;
    int err;

    if (cbk->lock != NULL && cbk->unlock != NULL) {
        err = cbk->unlock();
    } else {
        err = YAS_NO_ERROR;
    }

    return err;
#endif
}

static int yas_l3g4200d_device_open(void)
{
    struct yas_gyro_driver_callback *cbk = &pcb->callback;
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    int err;

    if (p->device_open == 0) {
        err = cbk->device_open();
        if (err != YAS_NO_ERROR) {
            return YAS_ERROR_DEVICE_COMMUNICATION;
        }
        p->device_open = 1;
    }

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_device_close(void)
{
    struct yas_gyro_driver_callback *cbk = &pcb->callback;
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    int err;

    if (p->device_open != 0) {
        err = cbk->device_close();
        if (err != YAS_NO_ERROR) {
            return YAS_ERROR_DEVICE_COMMUNICATION;
        }
        p->device_open = 0;
    }

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_interrupt_enable(void)
{
    struct yas_gyro_driver_callback *cbk = &pcb->callback;
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    int err;

    if (p->interrupt == 0) {
        return YAS_NO_ERROR;
    }

    err = cbk->interrupt_enable();
    if (err != YAS_NO_ERROR) {
        return YAS_ERROR_INTERRUPT;
    }

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_interrupt_disable(void)
{
    struct yas_gyro_driver_callback *cbk = &pcb->callback;
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    int err;

    if (p->interrupt == 0) {
        return YAS_NO_ERROR;
    }

    err = cbk->interrupt_disable();
    if (err != YAS_NO_ERROR) {
        return YAS_ERROR_INTERRUPT;
    }

    return YAS_NO_ERROR;
}

static void yas_l3g4200d_interrupt_notify(int num)
{
    struct yas_gyro_driver_callback *cbk = &pcb->callback;
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    if (p->interrupt == 0) {
        return;
    }

    cbk->interrupt_notify(num);
}

static int yas_l3g4200d_msleep(int msec)
{
    struct yas_gyro_driver_callback *cbk = &pcb->callback;

    if (msec <= 0) {
        return YAS_ERROR_ARG;
    }

    cbk->msleep(msec);

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_init(void)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    int err;
    int id;

    /* Check intialize */
    if (p->initialize == 1) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    /* Open i2c/spi */
    err = yas_l3g4200d_device_open();
    if (err != YAS_NO_ERROR) {
        return err;
    }

    /* Check id */
    id = yas_l3g4200d_read_reg_byte(YAS_L3G4200D_WHOAMI_REG);
    if (id != YAS_L3G4200D_WHOAMI_ID) {
        yas_l3g4200d_device_close();
        return YAS_ERROR_CHIP_ID;
    }

    /* Init data */
    yas_l3g4200d_init_data();

    p->initialize = 1;

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_term(void)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    yas_l3g4200d_set_enable(0);

    /* Close i2c/spi */
    yas_l3g4200d_device_close();

    p->initialize = 0;

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_get_delay(void)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    return p->delay;
}

static int yas_l3g4200d_set_delay(int delay)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    unsigned char odr;
    int i;

    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    /* Determine optimum odr */
    for (i = 1; i < (int)(sizeof(yas_l3g4200d_odr_tbl) / sizeof(struct yas_l3g4200d_odr)) &&
             delay >= (int)yas_l3g4200d_odr_tbl[i].delay; i++)
        ;

    odr = yas_l3g4200d_odr_tbl[i-1].odr;
    p->delay = delay;
    p->actual_delay = yas_l3g4200d_odr_tbl[i-1].delay;

    if (p->enable) {
        if (p->interrupt) {
            yas_l3g4200d_set_wtm();
        }
        yas_l3g4200d_update_bits(YAS_L3G4200D_ODR, odr);
    }
 
    return YAS_NO_ERROR;
}

static int yas_l3g4200d_get_enable(void)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;
 
    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    return p->enable;
}

static int yas_l3g4200d_set_enable(int enable)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    int err;

    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    if (enable) {
        if (yas_l3g4200d_ischg_enable(enable)) {
            if (p->enable == 1) {
                return YAS_NO_ERROR;
            }
            p->overrun = 0;

            /* Open transfer i2c/spi */
            err = yas_l3g4200d_device_open();
            if (err != YAS_NO_ERROR) {
                return err;
            }

            /* init data */
            yas_l3g4200d_init_lastvalue();
            p->initial_data = 0;

            /* Set configuration */
            yas_l3g4200d_configure();

            p->enable = 1;

            /* Set DLPF */
            yas_l3g4200d_set_delay(p->delay);

            /* Interrupt enable */
            if (p->interrupt) {
                yas_l3g4200d_interrupt_enable();
            }

            /* Wait powerup */
            yas_l3g4200d_msleep(YAS_L3G4200D_POWERUP_TIME);
        }
    } else {
        if (yas_l3g4200d_ischg_enable(enable)) {
            if (p->enable == 0) {
                return YAS_NO_ERROR;
            }

            /* Interrupt disable */
            if (p->interrupt) {
                yas_l3g4200d_interrupt_disable();
            }

            /* Setting Fifo (fifo mode, WTM) */
            yas_l3g4200d_update_bits(YAS_L3G4200D_FIFO_ENABLE, 0);

            /* Powerdown */
            yas_l3g4200d_update_bits(YAS_L3G4200D_POWER, 0);

            /* Close transfer i2c/spi */
            err = yas_l3g4200d_device_close();
            if (err != YAS_NO_ERROR) {
                /* Set configuration */
                yas_l3g4200d_configure();
                return err;
            }

            p->enable = 0;
        }
    }

    return YAS_NO_ERROR;
}

#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
static int yas_l3g4200d_set_selftest_enable(int enable, int *result)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;
	struct yas_l3g4200d_selftest_data selftest_data = {
		.result = 0,
		.selftest_avg.v = {0,0,0},
		.abs_avg.v = {0,0,0},
		.status = 0,
	};
    int err;
	int i;
	uint8_t buf[6];
	uint16_t tmp_u16;
	int32_t tmp_32[3];

	atomic_set(&p->selftest_result, 0);

	/* Open transfer i2c/spi */
	err = yas_l3g4200d_device_open();
	if (err != YAS_NO_ERROR) {
		printk("yas_l3g4200d_device_open error.\n");
		return err;
	}

	// normal mode
	err = yas_l3g4200d_write_reg_byte(0x20, 0x6F);
	err = yas_l3g4200d_write_reg_byte(0x21, 0x00);
	err = yas_l3g4200d_write_reg_byte(0x22, 0x00);
	err = yas_l3g4200d_write_reg_byte(0x23, 0xA0);
	err = yas_l3g4200d_write_reg_byte(0x24, 0x02);
	
	if(err != YAS_NO_ERROR) {
		printk("(1) yas_l3g4200d_write_reg error.\n");
		return err;
	}
    yas_l3g4200d_msleep(800);

	selftest_data.status = yas_l3g4200d_read_reg_byte(YAS_L3G4200D_STATUS_REG);
	if( (selftest_data.status&0x08) != 0x08 ) {
		printk("(1) status check error.(%d)\n", selftest_data.status);
		return -1;
	}
	else {
		for(i=0; i<5; i++){
			yas_l3g4200d_read_reg(YAS_L3G4200D_DATA_REG, buf, 6);

			tmp_u16 = (buf[1]<<8) | (buf[0]&0xFF);
			//printk("tmp_u16 = 0x%04X\n", tmp_u16);
			tmp_32[0] = (tmp_u16>=0x8000) ? (-1)*(0x8000-(0x7FFF&tmp_u16)) : tmp_u16;
			selftest_data.normal_avg.v[0] += tmp_32[0];

			tmp_u16 = (buf[3]<<8) | (buf[2]&0xFF);
			//printk("tmp_u16 = 0x%04X\n", tmp_u16);
			tmp_32[1] = (tmp_u16>=0x8000) ? (-1)*(0x8000-(0x7FFF&tmp_u16)) : tmp_u16;
			selftest_data.normal_avg.v[1] += tmp_32[1];

			tmp_u16 = (buf[5]<<8) | (buf[4]&0xFF);
			//printk("tmp_u16 = 0x%04X\n", tmp_u16);
			tmp_32[2] = (tmp_u16>=0x8000) ? (-1)*(0x8000-(0x7FFF&tmp_u16)) : tmp_u16;
			selftest_data.normal_avg.v[2] += tmp_32[2];

			//printk("normal_avg[%d] (%d %d %d)\n", i, tmp_32[0], tmp_32[1], tmp_32[2]); 
		}
		selftest_data.normal_avg.v[0] = selftest_data.normal_avg.v[0]/5;
		selftest_data.normal_avg.v[1] = selftest_data.normal_avg.v[1]/5;
		selftest_data.normal_avg.v[2] = selftest_data.normal_avg.v[2]/5;
		printk("normal_avg[avg] (%d %d %d)\n", selftest_data.normal_avg.v[0], selftest_data.normal_avg.v[1], selftest_data.normal_avg.v[2]);
	}

	// selftest mode
	err = yas_l3g4200d_write_reg_byte(0x23, 0xA2);
	if(err != YAS_NO_ERROR) {
		printk("(2) yas_l3g4200d_write_reg error.\n");
		return err;
	}
    yas_l3g4200d_msleep(800);

	selftest_data.status = yas_l3g4200d_read_reg_byte(YAS_L3G4200D_STATUS_REG);
	if( (selftest_data.status&0x08) != 0x08 ) {
		printk("(2) status check error.(%d)\n", selftest_data.status);
		return -1;
	}
	else {
		for(i=0; i<5; i++){
			yas_l3g4200d_read_reg(YAS_L3G4200D_DATA_REG, buf, 6);

			tmp_u16 = (buf[1]<<8) | (buf[0]&0xFF);
			//printk("tmp_u16 = 0x%04X\n", tmp_u16);
			tmp_32[0] = (tmp_u16>=0x8000) ? (-1)*(0x8000-(0x7FFF&tmp_u16)) : tmp_u16;
			selftest_data.selftest_avg.v[0] += tmp_32[0];

			tmp_u16 = (buf[3]<<8) | (buf[2]&0xFF);
			//printk("tmp_u16 = 0x%04X\n", tmp_u16);
			tmp_32[1] = (tmp_u16>=0x8000) ? (-1)*(0x8000-(0x7FFF&tmp_u16)) : tmp_u16;
			selftest_data.selftest_avg.v[1] += tmp_32[1];

			tmp_u16 = (buf[5]<<8) | (buf[4]&0xFF);
			//printk("tmp_u16 = 0x%04X\n", tmp_u16);
			tmp_32[2] = (tmp_u16>=0x8000) ? (-1)*(0x8000-(0x7FFF&tmp_u16)) : tmp_u16;
			selftest_data.selftest_avg.v[2] += tmp_32[2];

			//printk("selftest_avg[%d] (%d %d %d)\n", i, tmp_32[0], tmp_32[1], tmp_32[2]); 
		}
		selftest_data.selftest_avg.v[0] = selftest_data.selftest_avg.v[0]/5;
		selftest_data.selftest_avg.v[1] = selftest_data.selftest_avg.v[1]/5;
		selftest_data.selftest_avg.v[2] = selftest_data.selftest_avg.v[2]/5;
		printk("selftest_avg[avg] (%d %d %d)\n\n", selftest_data.selftest_avg.v[0], selftest_data.selftest_avg.v[1], selftest_data.selftest_avg.v[2]);
	}

	selftest_data.abs_avg.v[0] = ABS(selftest_data.selftest_avg.v[0] - selftest_data.normal_avg.v[0])*70/1000;
	selftest_data.abs_avg.v[1] = ABS(selftest_data.selftest_avg.v[1] - selftest_data.normal_avg.v[1])*70/1000;
	selftest_data.abs_avg.v[2] = ABS(selftest_data.selftest_avg.v[2] - selftest_data.normal_avg.v[2])*70/1000;

	if( (selftest_data.abs_avg.v[0] >= SELFTEST_2000_DPS_MIN_DATA) && (selftest_data.abs_avg.v[0] <= SELFTEST_2000_DPS_MAX_DATA ) ) selftest_data.result+= 1;
	if( (selftest_data.abs_avg.v[1] >= SELFTEST_2000_DPS_MIN_DATA) && (selftest_data.abs_avg.v[1] <= SELFTEST_2000_DPS_MAX_DATA ) ) selftest_data.result+= 2;
	if( (selftest_data.abs_avg.v[2] >= SELFTEST_2000_DPS_MIN_DATA) && (selftest_data.abs_avg.v[2] <= SELFTEST_2000_DPS_MAX_DATA) ) selftest_data.result+= 4;

	printk("result = %d\n", selftest_data.result);
	*result = selftest_data.result;
	atomic_set(&p->selftest_result, selftest_data.result);

	err = yas_l3g4200d_write_reg_byte(0x23, 0x00);
	if(err != YAS_NO_ERROR) {
		printk("(3) yas_l3g4200d_write_reg error.\n");
		return err;
	}

	/* Close transfer i2c/spi */
	err = yas_l3g4200d_device_close();
	if (err != YAS_NO_ERROR) {
		printk("yas_l3g4200d_device_close error.\n");
		return err;
	}

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_get_selftest_result(void)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;
 
    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    return atomic_read(&p->selftest_result);
}


#endif

static int yas_l3g4200d_get_position(void)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    return p->position;
}

static int yas_l3g4200d_set_position(int position)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    p->position = position;

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_get_offset(struct yas_vector *offset)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    *offset = p->offset;

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_set_offset(struct yas_vector *offset)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    p->offset = *offset;

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_get_filter(struct yas_gyro_filter *filter)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    filter->threshold = p->threshold;

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_set_filter(struct yas_gyro_filter *filter)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    p->threshold = filter->threshold;

    return YAS_NO_ERROR;
}
static int yas_l3g4200d_get_filter_enable(void)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;
 
    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    return p->filter_enable;
}

static int yas_l3g4200d_set_filter_enable(int enable)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    p->filter_enable = enable;

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_data_filter(struct yas_gyro_data *out_data)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    if (p->filter_enable) {
        if ((ABS(p->last.xyz.v[0] - out_data->xyz.v[0]) > p->threshold) ||
            (ABS(p->last.xyz.v[1] - out_data->xyz.v[1]) > p->threshold) ||
            (ABS(p->last.xyz.v[2] - out_data->xyz.v[2]) > p->threshold)) {
        } else {
            *out_data = p->last;
        }
    }

    return YAS_NO_ERROR;
}

static int yas_l3g4200d_measure_fifo(struct yas_gyro_data *out_data, int num)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    uint8_t reg;
    int32_t raw[3];
    int data_num;
    int pos = p->position;
    int ret;
    int i,j,k;

    data_num = 0;

    /* Check devie */
    if (p->device_open == 0) {
        out_data[0] = p->last;
        out_data[0].num = 1;
        out_data[0].overrun = p->overrun;
        yas_l3g4200d_interrupt_enable();
        return 1;
    }

    /* Read FIFO status */
    reg = yas_l3g4200d_read_reg_byte(YAS_L3G4200D_FIFO_STATUS_REG); 

    /* FIFO empty */
    if (reg & 0x20) {
        out_data[0] = p->last;
        out_data[0].num = 1;
        out_data[0].overrun = p->overrun;
        yas_l3g4200d_interrupt_enable();
        return 1;
    }

    /* Check FIFO stored data level */
    data_num = reg & 0x1f;
    if (num < data_num) {
        data_num = num;
    }

    /* Disable interrupt */
    yas_l3g4200d_update_bits(YAS_L3G4200D_INTERRUPT, YAS_L3G4200D_INTERRUPT_DISABLE);

    /* Read angle rate data */
    for (i = 0; i < data_num; i++) {
        if ((ret= yas_l3g4200d_read_reg(YAS_L3G4200D_DATA_REG | 0x80, &l3g4200d_buf[i*6], 6)) != 0) {
            out_data[0] = p->last;
            out_data[0].num = 1;
            out_data[0].overrun = p->overrun;
            yas_l3g4200d_reset_fifo();
            yas_l3g4200d_update_bits(YAS_L3G4200D_INTERRUPT, YAS_L3G4200D_INTERRUPT_ENABLE);
            yas_l3g4200d_interrupt_enable();
            return 1;
        }
    }

    yas_l3g4200d_reset_fifo();

    /* Enable interrupt */
    yas_l3g4200d_update_bits(YAS_L3G4200D_INTERRUPT, YAS_L3G4200D_INTERRUPT_ENABLE);

    /* Check FIFO overrun */
    if (reg & 0x40) {
        p->overrun++;
        out_data[0] = p->last;
        out_data[0].num = 1;
        out_data[0].overrun = p->overrun;
        yas_l3g4200d_interrupt_enable();
        return 1;
    }

    for (i = 0; i < data_num; ++i) {
        for (j = 0; j < 3; ++j) {
            raw[j] = ((int16_t)((l3g4200d_buf[i*6+j*2+1] << 8)) | l3g4200d_buf[i*6+j*2]);
        }

        for (j = 0; j < 3; ++j) {
            /* coordinate transformation */
            out_data[i].raw.v[j] = 0;
            for (k = 0; k < 3; ++k) {
                out_data[i].raw.v[j] += raw[k] * yas_l3g4200d_position_map[pos][j][k];
            }
            out_data[i].xyz.v[j] = out_data[i].raw.v[j] * YAS_L3G4200D_RESOLUTION;
        }
        out_data[i].xyz.v[0] -= p->offset.v[0];
        out_data[i].xyz.v[1] -= p->offset.v[1];
        out_data[i].xyz.v[2] -= p->offset.v[2];
        out_data[i].overrun = p->overrun;
        out_data[i].num = data_num;
        yas_l3g4200d_data_filter(&out_data[i]);
        p->last = out_data[i];
    }

    /* Enable interrupt */
    yas_l3g4200d_interrupt_enable();

    return data_num;
}

static int yas_l3g4200d_measure_direct(struct yas_gyro_data *out_data)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    int32_t raw[3];
    int pos = p->position;
    int i,j;

    /* Check device */
    if (p->device_open == 0) {
        out_data[0] = p->last;
        out_data[0].num = 1;
        out_data[0].overrun = p->overrun;
        yas_l3g4200d_interrupt_enable();
        return 1;
    }

    /* Read angle rate data */
    if (yas_l3g4200d_read_reg(YAS_L3G4200D_DATA_REG | 0x80, &l3g4200d_buf[0], 6) != 0) {
        out_data[0] = p->last;
        return 1;
    }

    for (i = 0; i < 3; ++i) {
        raw[i] = ((int16_t)((l3g4200d_buf[i*2+1] << 8)) | l3g4200d_buf[i*2]);
    }

    for (i = 0; i < 3; ++i) {
        /* coordinate transformation */
        out_data->raw.v[i] = 0;
        for (j = 0; j < 3; ++j) {
            out_data->raw.v[i] += raw[j] * yas_l3g4200d_position_map[pos][i][j];
        }
        out_data->xyz.v[i] = out_data->raw.v[i] * YAS_L3G4200D_RESOLUTION;
    }

    out_data->xyz.v[0] -= p->offset.v[0];
    out_data->xyz.v[1] -= p->offset.v[1];
    out_data->xyz.v[2] -= p->offset.v[2];
    out_data->num = 1;
    out_data->overrun = p->overrun;
    yas_l3g4200d_data_filter(out_data);

    p->last = out_data[0];

    return 1;
}

static int yas_l3g4200d_measure(struct yas_gyro_data *out_data, int num)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    int actual_num;
    int i;

    /* Check initialize */
    if (p->initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    for (i = 0; i < num; i++) {
        yas_l3g4200d_init_gyro_data(&out_data[i]);
    }

    if (p->interrupt) {
        actual_num = yas_l3g4200d_measure_fifo(out_data, num);
    } else {
        actual_num = yas_l3g4200d_measure_direct(out_data);
    }

    return actual_num;
}

/* --------------------------------------------------------------------------- */
static int yas_init(void)
{
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_init();
    yas_l3g4200d_unlock();

    return err;
}

static int yas_term(void)
{
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_term();
    yas_l3g4200d_unlock();

    return err;
}

static int yas_get_delay(void)
{
    int ret;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    yas_l3g4200d_lock();
    ret = yas_l3g4200d_get_delay();
    yas_l3g4200d_unlock();

    return ret;
}

static int yas_set_delay(int delay)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    if (delay < YAS_L3G4200D_MIN_DELAY_INT || delay > YAS_L3G4200D_MAX_DELAY) {
        return YAS_ERROR_ARG;
    }

    if (delay == YAS_L3G4200D_MIN_DELAY_INT) {
        delay = 1;
    }

    if (p->interrupt == 0) {
        if (delay < YAS_L3G4200D_MIN_DELAY_POL) {
            delay = YAS_L3G4200D_MIN_DELAY_POL;
        }
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_set_delay(delay);
    yas_l3g4200d_unlock();

    return err;
}

static int yas_get_enable(void)
{
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_get_enable();
    yas_l3g4200d_unlock();

    return err;
}

static int yas_set_enable(int enable)
{
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    if (enable != 0) {
        enable = 1;
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_set_enable(enable);
    yas_l3g4200d_unlock();

    return err;
}

#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
static int yas_set_selftest_enable(int enable, int *result)
{
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    if (enable != 0) {
        enable = 1;
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_set_selftest_enable(enable, result);
    yas_l3g4200d_unlock();

    return err;
}

static int yas_get_selftest_result(void)
{
    int result;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    yas_l3g4200d_lock();
    result = yas_l3g4200d_get_selftest_result();
    yas_l3g4200d_unlock();

    return result;
}

#endif

static int yas_get_position(void)
{
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_get_position();
    yas_l3g4200d_unlock();

    return err;
}

static int yas_set_position(int position)
{
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    if (position < YAS_L3G4200D_MIN_POSITION ||
        position > YAS_L3G4200D_MAX_POSITION) {
        return YAS_ERROR_ARG;
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_set_position(position);
    yas_l3g4200d_unlock();

    return err;
}

static int yas_get_offset(struct yas_vector *offset)
{
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    if (offset == NULL) {
        return YAS_ERROR_ARG;
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_get_offset(offset);
    yas_l3g4200d_unlock();

    return err;
}

static int yas_set_offset(struct yas_vector *offset)
{
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    if (offset == NULL ||
        ABS(offset->v[0]) > YAS_L3G4200D_MAX_DATA ||
        ABS(offset->v[1]) > YAS_L3G4200D_MAX_DATA ||
        ABS(offset->v[2]) > YAS_L3G4200D_MAX_DATA) {
        return YAS_ERROR_ARG;
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_set_offset(offset);
    yas_l3g4200d_unlock();

    return err;
}

static int yas_get_filter(struct yas_gyro_filter *filter)
{
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    if (filter == NULL) {
        return YAS_ERROR_ARG;
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_get_filter(filter);
    yas_l3g4200d_unlock();

    return err;
}

static int yas_set_filter(struct yas_gyro_filter *filter)
{
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    if (filter == NULL || filter->threshold < 0 || filter->threshold > YAS_L3G4200D_MAX_DATA) {
        return YAS_ERROR_ARG;
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_set_filter(filter);
    yas_l3g4200d_unlock();

    return err;
}

static int yas_get_filter_enable(void)
{
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_get_filter_enable();
    yas_l3g4200d_unlock();

    return err;
}

static int yas_set_filter_enable(int enable)
{
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    if (enable != 0) {
        enable = 1;
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_set_filter_enable(enable);
    yas_l3g4200d_unlock();

    return err;
}

static void yas_interrupt_handler(void)
{
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    /* Check initialize */
    if (p->initialize == 0) {
        return;
    }

    yas_l3g4200d_interrupt_disable();
    yas_l3g4200d_interrupt_notify(p->wtm);
}

static int yas_measure(struct yas_gyro_data *data, int num)
{
    int err;

    /* Check intialize */
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    if (data == NULL || num <= 0) {
        return YAS_ERROR_ARG;
    }

    yas_l3g4200d_lock();
    err = yas_l3g4200d_measure(data, num);
    yas_l3g4200d_unlock();

    return err;
}

#if DEBUG
static int yas_get_register(uint8_t adr, uint8_t *val)
{
    if (pcb == NULL) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    /* Check initialize */
    if (l3g4200d_data.initialize == 0) {
        return YAS_ERROR_NOT_INITIALIZED;
    }

    *val = yas_l3g4200d_read_reg_byte(adr);

    return YAS_NO_ERROR;
}
#endif
/* --------------------------------------------------------------------------- */
/*  Global function                                                            */
/* --------------------------------------------------------------------------- */
int yas_gyro_driver_init(struct yas_gyro_driver *f, int interrupt)
{
    struct yas_gyro_driver_callback *cbk;
    struct yas_l3g4200d_data *p = &l3g4200d_data;

    /* Check parameter */
    if (f == NULL) {
        return YAS_ERROR_ARG;
    }

    /* Clear driver interface */
    f->init = NULL;
    f->term = NULL;
    f->get_delay = NULL;
    f->set_delay = NULL;
    f->get_enable = NULL;
    f->set_enable = NULL;
#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
    f->set_selftest_enable = NULL;
    f->get_selftest_result = NULL;
#endif
    f->get_position = NULL;
    f->set_position = NULL;
    f->get_offset = NULL;
    f->set_offset = NULL;
    f->get_filter = NULL;
    f->set_filter = NULL;
    f->get_filter_enable = NULL;
    f->set_filter_enable = NULL;
    f->measure = NULL;
#if DEBUG
    f->get_register = NULL;
#endif
    f->interrupt_handler = NULL;

    cbk = &f->callback;
    if (cbk->device_open == NULL ||
        cbk->device_close == NULL ||
        cbk->device_write == NULL ||
        cbk->device_read == NULL ||
        cbk->msleep == NULL) {
        return YAS_ERROR_ARG;
    }
    if (interrupt) {
        interrupt = 1;
        if (cbk->interrupt_enable == NULL ||
            cbk->interrupt_disable == NULL ||
            cbk->interrupt_notify == NULL) {
            return YAS_ERROR_ARG;
        }
    }

    /* Clear intialize */
    yas_l3g4200d_term();

    /* Set callback interface */
    cb.callback = *cbk;

    /* Set driver interface */
    f->init = yas_init;
    f->term = yas_term;
    f->get_delay = yas_get_delay;
    f->set_delay = yas_set_delay;
    f->get_enable = yas_get_enable;
    f->set_enable = yas_set_enable;
#if defined(PANTECH_FEATURE_SENSOR) && (YAS_GYRO_DRIVER == YAS_GYRO_DRIVER_L3G4200D)
    f->set_selftest_enable = yas_set_selftest_enable;
    f->get_selftest_result = yas_get_selftest_result;
#endif
    f->get_position = yas_get_position;
    f->set_position = yas_set_position;
    f->get_offset = yas_get_offset;
    f->set_offset = yas_set_offset;
    f->get_filter = yas_get_filter;
    f->set_filter = yas_set_filter;
    f->get_filter_enable = yas_get_filter_enable;
    f->set_filter_enable = yas_set_filter_enable;
    f->measure = yas_measure;
#if DEBUG
    f->get_register = yas_get_register;
#endif
    if (interrupt) {
        f->interrupt_handler = yas_interrupt_handler;
    } else {
        f->interrupt_handler = NULL;
    }

    p->interrupt = interrupt;
    pcb = &cb;

    return YAS_NO_ERROR;
}
