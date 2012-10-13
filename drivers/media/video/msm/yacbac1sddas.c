/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <linux/delay.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <media/msm_camera.h>
#include <mach/gpio.h>
#include "yacbac1sddas.h"

#define MT9P111_MODULE_ABNORMAL_OPERATION_DEFENCE


#ifdef CONFIG_PANTECH_CAMERA_TUNER
#define YACBAC1SDDAS_REG_MCU_ADDR      0x098E
#define YACBAC1SDDAS_REG_MCU_DATA      0x0990
#endif

#include <mach/vreg.h>

#include "camsensor_yacbac1sddas_tuneup.h"


/* Hynix yacbac1sddas Registers and their values */
/* Sensor Core Registers */
#define  REG_YACBAC1SDDAS_MODEL_ID 0x3000
#define  YACBAC1SDDAS_MODEL_ID     0x1580

#define REG_YACBAC1SDDAS_REG_MCU_VAR_ADDR      0x098C
#define REG_YACBAC1SDDAS_REG_MCU_VAR_DATA      0x0990
#define REG_YACBAC1SDDAS_REG_FRAME_CNT         0x303a
#define REG_YACBAC1SDDAS_REG_MCU_DATA_SEQ_CMD  0xa103

#define YACBAC1SDDAS_I2C_RETRY_CNT   3
#define YACBAC1SDDAS_MAX_WAIT_CNT    20
#define YACBAC1SDDAS_POLL_PERIOD_MS  10

/* Time in milisecs for waiting for the sensor to reset.*/
#define YACBAC1SDDAS_RESET_DELAY_MSECS   66

#define YACBAC1SDDAS_DEFAULT_CLOCK_RATE 24000000

/*******************************************************/

//[[ PSJ_0100412_BEGIN -- 서브카메라 Standby Pin
#define CAM_VGA_STANDBY	133

// 2010-05-11 Main Camera Standby pin
#define CAM_5M_STANDBY	123

#define ON 1
#define OFF 0
/*******************************************************/





struct yacbac1sddas_work
{
	struct work_struct work;
};

static struct yacbac1sddas_work *yacbac1sddas_sensorw;
static struct i2c_client *yacbac1sddas_client;


struct yacbac1sddas_ctrl_t
{
	const struct msm_camera_sensor_info *sensordata;
};

static struct yacbac1sddas_ctrl_t  * yacbac1sddas_ctrl = NULL;

struct yacbac1sddas_vreg_t
{
	const char *name;
	unsigned short mvolt;
};



static DECLARE_WAIT_QUEUE_HEAD(yacbac1sddas_wait_queue);
DECLARE_MUTEX(yacbac1sddas_sem);
static int16_t yacbac1sddas_effect = CAMERA_EFFECT_OFF;


#ifdef CONFIG_PANTECH_CAMERA_TUNER
static yacbac1sddas_tune_state_type yacbac1sddas_tup_state = YACBAC1SDDAS_TUNE_STATE_NONE;
static yacbac1sddas_tune_mode_type yacbac1sddas_tup_mode = YACBAC1SDDAS_TUNE_STATE_TUNNING_MODE_ON;
//static uint8_t yacbac1sddas_done_set_tune_load = 0;
static yacbac1sddas_params_tbl_t yacbac1sddas_params_tbl;
#endif


/*=============================================================
	EXTERNAL DECLARATIONS
==============================================================*/
extern struct yacbac1sddas_reg yacbac1sddas_regs;

/*=============================================================*/

/*
================================================================================
LOCAL API DECLARATIONS
================================================================================
*/
#ifdef CONFIG_PANTECH_CAMERA_TUNER
static u_int32_t yacbac1sddas_i2c_write_params
(
    yacbac1sddas_cfg_param_t *params,
    u_int32_t num_params
);
#endif

static int32_t yacbac1sddas_i2c_write (uint16_t saddr, uint16_t waddr,
	uint16_t wdata, enum yacbac1sddas_width width);

/*
================================================================================
FUNCTION
================================================================================
*/

typedef struct {
	int32_t id;
	const char *label;
	uint32_t num;
} gpio_ctrl_t;

#if 0
typedef struct {
	int32_t id;
	const char *label;
	const char *vname;
	uint32_t mvolt;
} vreg_ctrl_t;
#endif
typedef struct {
	int32_t id;
	const char *label;
	const char *vname;
	uint32_t value;
} vreg_ctrl_t;


#if defined(CONFIG_MACH_MSM8X55_EF30S)

#define CAM_5MP_RST_N	0
#define CAM_5MP_STB	1
#define CAM_VGA_RST_N	2
#define CAM_VGA_STB_N	3
#define CAM_LDO_EN	4 /* control external LDO (VDD_CAMA_2800) */
#define CAM_FLASH_EN	5 /* switch, flash driver IC */
#define CAM_GPIO_MAX	6

static gpio_ctrl_t g_gpios[] = {
	{CAM_5MP_RST_N, "CAM_5MP_RST_N", 121},
	{CAM_5MP_STB,   "CAM_5MP_STB",   123},
	{CAM_VGA_RST_N, "CAM_VGA_RST_N",  31},
	{CAM_VGA_STB_N, "CAM_VGA_STB_N", 133},
	{CAM_LDO_EN,    "CAM_LDO_EN",     43}
};

#define VDD_CAM_1800	0   /* gp7 */
#define VDD_CAM_1800_IO	1   /* gp10 */
#define VDD_CAMA_2800	2   /* external LDO (controlled by CAM_LDO_EN) */


static vreg_ctrl_t g_vregs[] = {
	{VDD_CAM_1800,    "VDD_CAM_1800",    "gp7",  1900},
	{VDD_CAM_1800_IO, "VDD_CAM_1800_IO", "gp10", 1900},
	{VDD_CAMA_2800,   "VDD_CAMA_2800",   NULL,   CAM_LDO_EN }
};

#elif defined(CONFIG_MACH_MSM8X55_EF18K)

#define CAM_5MP_RST_N	0
#define CAM_5MP_STB	1
#define CAM_VGA_RST_N	2
#define CAM_VGA_STB_N	3
#define CAM_GPIO_MAX	4
#define CAM_FLASH_EN    5 /* switch, flash driver IC */


#define VDD_CAM_1800	0
#define VDD_CAMA_2800	1
#define VDD_CAMAF_2800	2
#define VDD_CAM_MAX	3

static gpio_ctrl_t g_gpios[] = {
	{CAM_5MP_RST_N, "CAM_5MP_RST_N", 121},
	{CAM_5MP_STB,   "CAM_5MP_STB",   123},
	{CAM_VGA_RST_N, "CAM_VGA_RST_N",  31},
	{CAM_VGA_STB_N, "CAM_VGA_STB_N", 133},
};

static vreg_ctrl_t g_vregs[] = {
	{VDD_CAM_1800,    "VDD_CAM_1800",    "gp7",  1900},
	{VDD_CAMA_2800,   "VDD_CAMA_2800",   "gp10", 2800},
	{VDD_CAMAF_2800,  "VDD_CAMAF_2800",  "gp4",  2800},
};
#else
#error "unknown model"
#endif


static int32_t yacbac1s_gpio_out(int32_t id, int32_t val)
{
	int32_t rc = 0;

	if ((id < CAM_5MP_RST_N) || (id > CAM_GPIO_MAX)) {
		SKYCERR("%s#%d: -EINVAL\n", __func__, __LINE__);
		return -EINVAL;
	}

	rc = gpio_request(g_gpios[id].num, g_gpios[id].label);
	if (rc) {
		SKYCERR("%s#%d: error. rc=%d\n", __func__, __LINE__, rc);
		goto gpio_out_fail;
	}

	rc = gpio_direction_output(g_gpios[id].num, val);
	if (rc) {
		SKYCERR("%s#%d: error. rc=%d\n", __func__, __LINE__, rc);
		goto gpio_out_fail;
	}

	gpio_free(g_gpios[id].num);

	SKYCDBG("%s: set %s to %d\n", __func__, g_gpios[id].label, val);
	return 0;

gpio_out_fail:
	return rc;
}

static int32_t yacbac1s_vreg_out(int32_t id, int32_t on)
{
	struct vreg *vreg = NULL;
	int32_t rc = 0;
#if defined(CONFIG_MACH_MSM8X55_EF30S)
	if ((id != VDD_CAM_1800) && (id != VDD_CAM_1800_IO)) {
		SKYCERR("%s#%d: -EINVAL\n", __func__, __LINE__);
		return -EINVAL;
	}
#elif defined(CONFIG_MACH_MSM8X55_EF18K)
	if ((id < VDD_CAM_1800) || (id >= VDD_CAM_MAX)){
		SKYCERR("%s#%d: -EINVAL\n", __func__, __LINE__);
		return -EINVAL;
	}
#endif

	vreg = vreg_get(NULL, g_vregs[id].vname);
	if (IS_ERR(vreg)) {
		rc = -ENOENT;
		SKYCERR("%s#%d: -ENOENT\n", __func__, __LINE__);
		goto vreg_out_fail;
	}

	if (on) {
		rc = vreg_set_level(vreg, g_vregs[id].value);
		if (rc) {
			SKYCERR("%s#%d: error. rc=%d\n", __func__, __LINE__, rc);
			goto vreg_out_fail;
		}
		rc = vreg_enable(vreg);
		if (rc) {
			SKYCERR("%s#%d: error. rc=%d\n", __func__, __LINE__, rc);
			goto vreg_out_fail;
		}
		SKYCDBG("%s: turn ON %s\n", __func__, g_vregs[id].label);
	} else {
		rc = vreg_disable(vreg);
		if (rc) {
			SKYCERR("%s#%d: error. rc=%d\n", __func__, __LINE__, rc);
			goto vreg_out_fail;
		}
		SKYCDBG("%s: turn OFF %s\n", __func__, g_vregs[id].label);
	}

	return 0;

vreg_out_fail:
	return rc;
}

static int32_t yacbac1s_power_on(void)
{
	int32_t rc = 0;

	/* VDD_CAM_1800_IO is opened and merged with VDD_CAM_1800 in ET15
	 * to correct 'no image' and 'burnt image' issues.
	 * We should not remove VDD_CAM_1800_IO for backward compatibility. */
#if defined(CONFIG_MACH_MSM8X55_EF30S)
	if (yacbac1s_vreg_out(VDD_CAM_1800_IO, 1))	rc = -EIO;
#endif
	/* YACBAC1S doesn't use VDD_CAM_1800, but you should turn on this to
	 * make MT9P111 go to proper STANDBY state. */
	if (yacbac1s_vreg_out(VDD_CAM_1800, 1))		rc = -EIO;
	/* prevent 5MP/VGA RESET signals from becoming HIGH state just after
	 * supplying CORE/IO voltages. (sensor HW issue) */
	if (yacbac1s_gpio_out(CAM_5MP_RST_N, 0))	rc = -EIO;
	if (yacbac1s_gpio_out(CAM_VGA_RST_N, 0))	rc = -EIO;
	msleep(5);
	/* turn on VDD_CAMA_2800 */
#if defined(CONFIG_MACH_MSM8X55_EF30S)
	if (yacbac1s_gpio_out(CAM_LDO_EN, 1))		rc = -EIO;
#elif defined(CONFIG_MACH_MSM8X55_EF18K)
	if (yacbac1s_vreg_out(VDD_CAMA_2800, 1))		rc = -EIO;
#endif
	msleep(5);
	if (yacbac1s_gpio_out(CAM_VGA_STB_N, 1))	rc = -EIO;
	msleep(15);
	if (yacbac1s_gpio_out(CAM_VGA_RST_N, 1))	rc = -EIO;
	msleep(2);
	if (yacbac1s_gpio_out(CAM_VGA_STB_N, 0))	rc = -EIO;
	msleep(15);
	if (yacbac1s_gpio_out(CAM_5MP_RST_N, 1))	rc = -EIO;
	msleep(10);
	/* force reset MT9P111 */
	if (yacbac1sddas_i2c_write((0x7a >> 1), 0x001c, 0x0001, WORD_LEN) < 0) rc = -EIO;
	if (yacbac1sddas_i2c_write((0x7a >> 1), 0x0018, 0x6008, WORD_LEN) < 0) rc = -EIO;
	if (yacbac1sddas_i2c_write((0x7a >> 1), 0x0018, 0x6009, WORD_LEN) < 0) rc = -EIO;
	if (yacbac1sddas_i2c_write((0x7a >> 1), 0x001c, 0x0000, WORD_LEN) < 0) rc = -EIO;
	msleep(10);
	if (yacbac1s_gpio_out(CAM_5MP_STB, 1))		rc = -EIO;
	msleep(15);
	if (yacbac1s_gpio_out(CAM_VGA_STB_N, 1))	rc = -EIO;
	msleep(15);
	if (yacbac1s_gpio_out(CAM_VGA_RST_N, 0))	rc = -EIO;
	msleep(3);
	if (yacbac1s_gpio_out(CAM_VGA_RST_N, 1))	rc = -EIO;
	msleep(10);

	return rc;
}

static int32_t yacbac1s_power_off(void)
{
	int32_t rc = 0;

	/* VDD_CAM_1800_IO is opened and merged with VDD_CAM_1800 in ET15
	 * to correct 'no image' and 'burnt image' issues.
	 * We should not remove VDD_CAM_1800_IO for backward compatibility. */

	if (yacbac1s_gpio_out(CAM_5MP_RST_N, 0))	rc = -EIO;
	msleep(3);
	if (yacbac1s_gpio_out(CAM_5MP_STB, 0))		rc = -EIO;
	msleep(3);
	if (yacbac1s_gpio_out(CAM_VGA_RST_N, 0))	rc = -EIO;
	msleep(2);
	if (yacbac1s_gpio_out(CAM_VGA_STB_N, 0))	rc = -EIO;
	/* turn off VDD_CAMA_2800 */
#if defined(CONFIG_MACH_MSM8X55_EF30S)	
	if (yacbac1s_gpio_out(CAM_LDO_EN, 0))		rc = -EIO;
#elif defined(CONFIG_MACH_MSM8X55_EF18K)
	if (yacbac1s_vreg_out(VDD_CAMA_2800, 0))		rc = -EIO;
#endif	
	msleep(4);
	if (yacbac1s_vreg_out(VDD_CAM_1800, 0))		rc = -EIO;
	msleep(4);
#if defined(CONFIG_MACH_MSM8X55_EF30S)
	if (yacbac1s_vreg_out(VDD_CAM_1800_IO, 0))	rc = -EIO;
#endif
	return rc;
}

static int32_t yacbac1sddas_i2c_txdata(unsigned short saddr, unsigned char *txdata, int length)
{
	struct i2c_msg msg[] =
	{
		{
			.addr = saddr,
			.flags = 0,
			.len = length,
			.buf = txdata,
		},
	};

	if( i2c_transfer(yacbac1sddas_client->adapter, msg ,1 ) < 0)
	{
		SKYCERR("yacbac1sddas_i2c_txdata failed\n");
		return -EIO;
	}

	return 0;

}

static int32_t yacbac1sddas_i2c_write(unsigned short saddr, unsigned short waddr, unsigned short wdata, enum yacbac1sddas_width width)
{
	int32_t rc = -EFAULT;
	unsigned char buf[4];

	memset(buf, 0, sizeof(buf));

	switch(width)
	{
		case WORD_LEN:  {
			buf[0] = (waddr & 0xFF00)>>8;
			buf[1] = (waddr & 0x00FF);
			buf[2] = (wdata & 0xFF00)>>8;
			buf[3] = (wdata & 0x00FF);

			rc = yacbac1sddas_i2c_txdata(saddr, buf, 4);
		}
			break;

		case TRIPLE_LEN: {
		buf[0] = (waddr & 0xFF00)>>8;
		buf[1] = (waddr & 0x00FF);
		buf[2] = wdata;
			rc = yacbac1sddas_i2c_txdata(saddr, buf, 3);
		}
			break;

		case BYTE_LEN:
		{
			buf[0] = waddr;
			buf[1] = wdata;
			rc = yacbac1sddas_i2c_txdata(saddr, buf, 2);
		}
		break;

		default:
			break;
	}

	if( rc < 0 )
		SKYCERR("i2c write failed, addr = 0x%x, val = 0x%x\n", waddr, wdata);

	return rc;

}


static int32_t yacbac1sddas_i2c_write_table(struct yacbac1sddas_i2c_reg_conf const *item , int num_items)
{
	int i;
	int32_t rc = 0;

	for(i = 0; i < num_items; i++)
	{
		if(item->width == ZERO_LEN)
		{
			SKYCDBG("ZERO_LEN continue ADDR = 0x%x, VALUE = 0x%x\n",item->waddr, item->wdata);
			continue;
		}

		rc = yacbac1sddas_i2c_write(yacbac1sddas_client->addr,
			item->waddr, item->wdata,
			item->width);

		if ( rc < 0)
			return rc;

		if ( item->mdelay_time != 0)
			msleep(item->mdelay_time);

		item++;
	}

	return rc;

}


#ifdef CONFIG_PANTECH_CAMERA_TUNER
static int32_t yacbac1sddas_i2c_rxdata(unsigned short saddr, int slength , unsigned char *rxdata, int rxlength)
{


	 struct i2c_msg msgs[] =
	 {
	 		{
				.addr = saddr,
				.flags = 0,
				.len = slength,
				.buf = rxdata,
	 		},
			{
				.addr = saddr,
				.flags = I2C_M_RD,
				.len = rxlength,
				.buf = rxdata,
			},
	 };
	SKYCDBG("i2c_transfer start!\n");
	if(i2c_transfer(yacbac1sddas_client->adapter, msgs, 2) < 0)
	{
		SKYCERR("yacbac1sddas_i2c_rxdata failed!\n");
		return -EIO;
	}
	SKYCDBG("i2c_transfer END!\n");
	return 0;

}

#if 0
static int32_t yacbac1sddas_i2c_read(unsigned short saddr, unsigned short raddr, unsigned short *rdata)
{
	int32_t rc =0;
	unsigned char buf[4];
//	unsigned char tmp_raddr;
	if(!rdata)
		return -EIO;

	memset(buf, 0, sizeof(buf));

	buf[0] = (raddr & 0xFF00) >> 8;
	buf[1] = (raddr & 0x00FF);

//	tmp_raddr = raddr;
	SKYCDBG("yacbac1sddas_i2c_read start\n");
	rc = yacbac1sddas_i2c_rxdata(saddr, 2 , buf, 2);
	SKYCDBG("rc = %d\n", rc);

	if(rc < 0)
		return rc;

	*rdata = buf[0] << 8 | buf[1];

	if( rc < 0 )
		SKYCDBG("yacbac1sddas_i2c_read failed!\n");

	return rc;
}
#endif
#endif

#ifdef CONFIG_PANTECH_CAMERA_TUNER
static int32_t yacbac1sddas_i2c_write_byte(unsigned char waddr, unsigned char wdata)
{
	int32_t rc = -EIO;
	unsigned char buf[2];

	memset(buf, 0, sizeof(buf));

	buf[0] = waddr;
	buf[1] = wdata;

	rc = yacbac1sddas_i2c_txdata(yacbac1sddas_client->addr, buf, 2);
	SKYCDBG("TUNE_W: waddr=0x%02x, wdata=0x%02x\n", waddr, wdata);
	if (rc < 0)
		SKYCERR("TUNE_W: waddr = 0x%02x, wdata = 0x%02x!\n", waddr, wdata);

	return rc;
}

static int32_t yacbac1sddas_i2c_read_byte(unsigned char raddr, unsigned char *rdata)
{
	int32_t rc = 0;
	unsigned char buf[1];

	if (!rdata)
		return -EIO;

	memset(buf, 0, sizeof(buf));

	buf[0] = raddr;

	rc = yacbac1sddas_i2c_rxdata(yacbac1sddas_client->addr, 1, buf, 1);
	if (rc < 0)
		return rc;

	*rdata = buf[0];

	SKYCDBG("TUNE_R: raddr=0x%02x, rdata=0x%02x\n", raddr, *rdata);
	if (rc < 0)
		SKYCERR("TUNE_R: raddr=0x%02x\n", raddr);

	return rc;
}
#endif

/* Need to Check initializ register */
static int32_t yacbac1sddas_reg_init(void)
{

	int32_t rc=0;
	SKYCDBG("Yacbac1sddas_reg_init Start !!! \n");

#ifdef CONFIG_PANTECH_CAMERA_TUNER
	// 튜닝 파일값
	if((yacbac1sddas_tup_state == YACBAC1SDDAS_TUNE_STATE_LOAD_VALUE) && (yacbac1sddas_params_tbl.init.num_params > 0))
	{
		yacbac1sddas_i2c_write_params(yacbac1sddas_params_tbl.init.params,
								yacbac1sddas_params_tbl.init.num_params);
	}
	// static 값
	else
#endif
/* Need to i2c_write_table initialize register  Jitae Lee 2010-03-24 */
	rc = yacbac1sddas_i2c_write_table(&yacbac1sddas_regs.init_parm[0], yacbac1sddas_regs.init_parm_size);
	if(rc < 0)
	{
		SKYCERR("yacbac1sddas_reg_init i2c write failed!\n");
		return rc;
	}
	SKYCDBG("yacbac1sddas_reg_init Success!\n");
	return rc;

}

/* Add by Lee Jitae 2010-05-13 , Not user EF18K-1200 */

static int32_t yacbac1sddas_set_effect(int32_t effect)
{
	int rc = 0;

	if(effect < YACBAC1SDDAS_CFG_EFFECT_NONE || effect >= YACBAC1SDDAS_CFG_EFFECT_MAX){
		SKYCDBG("%s error. effect=%d\n", __func__, effect);
		return -EINVAL;
	}

#ifdef CONFIG_PANTECH_CAMERA_TUNER
	// 튜닝 파일값
	if((yacbac1sddas_tup_state == YACBAC1SDDAS_TUNE_STATE_LOAD_VALUE) && (yacbac1sddas_params_tbl.effect[effect].num_params > 0))
	{
		yacbac1sddas_i2c_write_params(yacbac1sddas_params_tbl.effect[effect].params,
								yacbac1sddas_params_tbl.effect[effect].num_params);
	}
	// static 값
	else
#endif
	{
		rc = yacbac1sddas_i2c_write_table(yacbac1sddas_regs.effect_cfg_settings[effect],
						yacbac1sddas_regs.effect_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("CAMERA_WB I2C FAIL!!! return~~\n");
		return rc;
	}

	yacbac1sddas_effect = effect;
	/* Refresh Sequencer */
	SKYCDBG("%s end\n",__func__);

	return rc;

}


static int32_t yacbac1sddas_start_preview(void)
{
	int32_t rc = 0;
#ifdef CONFIG_PANTECH_CAMERA_TUNER
	if(yacbac1sddas_tup_state == YACBAC1SDDAS_TUNE_STATE_LOAD_VALUE)
	{
		rc = yacbac1sddas_reg_init();
		if (rc < 0)
		{
			SKYCERR("yacbac1sddas_i2c_write_table FAIL!!! return~~\n");
			return rc;
		}
	}
#endif
	SKYCDBG("%s done.\n", __func__);

	return rc;
}

static int32_t yacbac1sddas_start_snapshot(void)
{
	int32_t rc = 0;

       SKYCDBG("%s done.\n", __func__);

	return rc;
}

static int32_t yacbac1sddas_set_sensor_mode(int mode)
{
	int32_t rc = 0;

	SKYCDBG("%s start , mode = %d\n" , __func__, mode);

	switch(mode)  // check data sheet
	{
		case SENSOR_PREVIEW_MODE: /* Don't Need Set Sensor Driver to use i2c device */
			SKYCDBG("yacbac1sddas SENSOR_PREVIEW_MODE\n");
			yacbac1sddas_start_preview();
			break;

		case SENSOR_SNAPSHOT_MODE: /* Don't Need Set Sensor Driver to use i2c device */
			SKYCDBG("yacbac1sddas SENSOR_SNAPSHOT_MODE\n");
			yacbac1sddas_start_snapshot();
			break;
#if 0
		case SENSOR_RAW_SNAPSHOT_MODE:  /* Yacbac1sddas is not support raw_snapshot_mode */
			yacbac1sddas_start_snapshot();
			break;
#endif

		default:
			rc = -EINVAL;
			break;
	}

	if (rc < 0)
		return rc;

	SKYCDBG("%s end\n",__func__);
	return rc;
}


static int32_t yacbac1sddas_set_reflect(int32_t reflect)
{

	int32_t rc = 0;

	SKYCDBG("%s: reflect=%d\n", __func__, reflect);

	if ((reflect < YACBAC1SDDAS_CFG_REFLECT_NONE) || (reflect >= YACBAC1SDDAS_CFG_REFLECT_MAX))
		return -EINVAL;

#ifdef CONFIG_PANTECH_CAMERA_TUNER
	if((yacbac1sddas_tup_state == YACBAC1SDDAS_TUNE_STATE_LOAD_VALUE) && (yacbac1sddas_params_tbl.reflect[reflect].num_params > 0))
	{
		yacbac1sddas_i2c_write_params(yacbac1sddas_params_tbl.reflect[reflect].params,
								yacbac1sddas_params_tbl.reflect[reflect].num_params);
	}
	// static 값
	else
#endif
	{
		rc = yacbac1sddas_i2c_write_table(yacbac1sddas_regs.reflect_cfg_settings[reflect],
						yacbac1sddas_regs.reflect_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("CAMERA_REFLECT I2C FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}

/* Add by Lee Jitae 2010-05-13 , Not user EF18K-1200 */

//static int32_t yacbac1sddas_set_frame_rate(int32_t fps)
//static int32_t yacbac1sddas_set_preview_fps(int32_t fps)
static int32_t yacbac1sddas_set_frame_rate(int16_t preview_fps)
{
	/* 0 : variable 30fps, 1 ~ 30 : fixed fps */
	/* default: variable 8 ~ 30fps */
	int32_t rc = 0;

#if 0 // need modification, PANTECH_CAMERA_TODO
	if ((preview_fps < C_SKYCAM_MIN_PREVIEW_FPS) ||
		(preview_fps > C_SKYCAM_MAX_PREVIEW_FPS)) {
		SKYCDBG("%s: -EINVAL, preview_fps=%d\n",
			__func__, preview_fps);
		return -EINVAL;
	}
#endif

	SKYCDBG("%s: preview_fps=%d\n", __func__, preview_fps);

#ifdef CONFIG_PANTECH_CAMERA_TUNER
	if((yacbac1sddas_tup_state == YACBAC1SDDAS_TUNE_STATE_LOAD_VALUE) && (yacbac1sddas_params_tbl.ffps[preview_fps-5].num_params > 0))
	{
		yacbac1sddas_i2c_write_params(yacbac1sddas_params_tbl.ffps[preview_fps-5].params,
								yacbac1sddas_params_tbl.ffps[preview_fps-5].num_params);
	}
	// static 값
	else
#endif
	{
		rc = yacbac1sddas_i2c_write_table(yacbac1sddas_regs.preview_fps_cfg_settings[preview_fps-5],
						yacbac1sddas_regs.preview_fps_cfg_settings_size);
	}

	if(rc < 0)
	{
		SKYCERR("Video fps setting i2c_write_talbe fail\n");
		return rc;
	}

	SKYCDBG("%s end rc = %d\n",__func__, rc);

	return rc;
}


/* Add by Lee Jitae 2010-05-13 , Not user EF18K-1200 */
static int32_t yacbac1sddas_set_brightness(int32_t brightness)
{
	int32_t rc = 0;
	SKYCDBG("%s start~ receive brightness = %d\n",__func__, brightness);

	if ((brightness < YACBAC1SDDAS_CFG_BRIGHT_M4) || (brightness >= YACBAC1SDDAS_CFG_BRIGHT_MAX)) {
		SKYCERR("%s error. brightness=%d\n", __func__, brightness);
		return -EINVAL;
	}
#ifdef CONFIG_PANTECH_CAMERA_TUNER
	// 튜닝 파일값
	if((yacbac1sddas_tup_state == YACBAC1SDDAS_TUNE_STATE_LOAD_VALUE) && (yacbac1sddas_params_tbl.bright[brightness].num_params > 0))
	{
		yacbac1sddas_i2c_write_params(yacbac1sddas_params_tbl.bright[brightness].params,
								yacbac1sddas_params_tbl.bright[brightness].num_params);
	}
	// static 값
	else
#endif
	{
		rc = yacbac1sddas_i2c_write_table(yacbac1sddas_regs.bright_cfg_settings[brightness],
						yacbac1sddas_regs.bright_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("CAMERA_BRIGHTNESS I2C FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}

/* Add by Lee Jitae 2010-05-13 , Not user EF18K-1200 */
static int32_t yacbac1sddas_set_wb(int32_t wb)
{
	int32_t rc=0;

	SKYCDBG("%s: wb=%d\n", __func__, wb);

#ifdef CONFIG_PANTECH_CAMERA_TUNER
	// 튜닝 파일값
	if((yacbac1sddas_tup_state == YACBAC1SDDAS_TUNE_STATE_LOAD_VALUE) && (yacbac1sddas_params_tbl.wb[wb-1].num_params > 0))
	{
		yacbac1sddas_i2c_write_params(yacbac1sddas_params_tbl.wb[wb-1].params,
								yacbac1sddas_params_tbl.wb[wb-1].num_params);
	}
	// static 값
	else
#endif
	{
		rc = yacbac1sddas_i2c_write_table(yacbac1sddas_regs.wb_cfg_settings[wb -1],
						yacbac1sddas_regs.wb_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("CAMERA_WB I2C FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}

/* Add by Lee Jitae 2010-05-13 , Not user EF18K-1200 */
static int32_t yacbac1sddas_set_exposure_mode(int32_t exposure)
{

	int32_t rc = 0;

	SKYCDBG("%s: exposure=%d\n", __func__, exposure);

	if ((exposure < YACBAC1SDDAS_CFG_EXPOSURE_NORMAL) || (exposure >= YACBAC1SDDAS_CFG_EXPOSURE_MAX))
		return -EINVAL;

#ifdef CONFIG_PANTECH_CAMERA_TUNER
	if((yacbac1sddas_tup_state == YACBAC1SDDAS_TUNE_STATE_LOAD_VALUE) && (yacbac1sddas_params_tbl.exposure[exposure].num_params > 0))
	{
		yacbac1sddas_i2c_write_params(yacbac1sddas_params_tbl.exposure[exposure].params,
								yacbac1sddas_params_tbl.exposure[exposure].num_params);
	}
	// static 값
	else
#endif
	{
		rc = yacbac1sddas_i2c_write_table(yacbac1sddas_regs.exposure_cfg_settings[exposure],
						yacbac1sddas_regs.exposure_cfg_settings_size);
	}

	if (rc < 0)
	{
		SKYCERR("CAMERA_EXPOSURE I2C FAIL!!! return~~\n");
		return rc;
	}

	SKYCDBG("%s end\n",__func__);

	return rc;
}


#ifdef CONFIG_PANTECH_CAMERA_TUNER
static u_int32_t yacbac1sddas_i2c_write_params
(
    yacbac1sddas_cfg_param_t *params,
    u_int32_t num_params
)
{
    u_int8_t read_data = 0;
    u_int8_t write_data = 0;
//    u_int8_t write_data_byte = 0;
    u_int32_t i = 0;

    SKYCDBG(">>%s ", __func__);
    SKYCDBG("- params = 0x%p, num_params = %d", params, num_params);

    if ((params == NULL) || (num_params == 0))
    {
        SKYCERR("Invalid param! [params = 0x%08x][num_params = %d]", (u_int32_t)params, num_params);
        return -EIO;
    }

    for (i = 0; i < num_params; i++)
    {
        switch(params[i].op)
        {
            case CAMOP_NOP:
                SKYCDBG("WRITE: NOP");
                break;

            case CAMOP_DELAY:
                SKYCDBG("WRITE: DELAY (%dms)", params[i].data);
                msleep(params[i].data);
                break;

            case CAMOP_WRITE:
                if (yacbac1sddas_i2c_write_byte((unsigned char)params[i].addr, (unsigned char)params[i].data) < 0)
                {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
                }
                break;
            case CAMOP_WRITE_BIT:
            {
                u_int8_t bit_pos = 0;
                u_int8_t bit_val = 0;

                if (yacbac1sddas_i2c_read_byte((unsigned char)params[i].addr, &read_data) < 0)
                {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
                }

                bit_pos = (params[i].data & 0xff00) >> 8;
                bit_val = (params[i].data & 0x00ff);

                if (bit_val != 0) // set to 1
                {
                    write_data = read_data | (0x01 << bit_pos);
                    SKYCDBG("set 0x%04x[%d] to 1", params[i].addr, bit_pos);
                }
                else // set to 0
                {
                    write_data = read_data & (~(0x01 << bit_pos));
                    SKYCDBG("set 0x%04x[%d] to 0", params[i].addr, bit_pos);
                }

                if (yacbac1sddas_i2c_write_byte((unsigned char)params[i].addr, write_data) < 0)
                {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
                }
                break;
            }

            case CAMOP_POLL_REG:
            {
                u_int8_t poll_delay = 0;
                u_int8_t poll_retry = 0;
                u_int16_t poll_reg = 0;
                u_int8_t poll_data = 0;
                u_int8_t poll_mask = 0;
                u_int16_t retry_cnt = 0;

                if (params[i+1].op != CAMOP_POLL_REG)
                {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
                }

                poll_delay = (params[i+1].data & 0xff00) >> 8;
                poll_retry = (params[i+1].data & 0x00ff);
                poll_reg = params[i].addr;
                poll_data = params[i+1].addr;
                poll_mask = params[i].data;;

                SKYCDBG("start polling register... if [0x%04x] AND 0x%04x equals 0x%04x, then stop polling", poll_reg, poll_mask, poll_data);


                for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
                {
                    if (yacbac1sddas_i2c_read_byte((unsigned char)poll_reg, &read_data) < 0)
                    {
                        SKYCERR("<<%s (-EIO)", __func__);
                        return -EIO;
                    }

                    if ((read_data & poll_mask) != poll_data)
                    {
                        SKYCERR("retry polling register... after sleeping %d ms", poll_delay);
                        msleep(poll_delay);
                    }
                    else
                    {
                        SKYCERR("stop polling register... retried %d/%d time(s) (delay = %d ms)", retry_cnt, poll_retry, poll_delay);
                        break;
                    }
                }

                if (retry_cnt == poll_retry)
                {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
                }

                ++i;
                break;
            }

            case CAMOP_POLL_MCU_VAR:
            {
#if 0
                u_int16_t poll_delay = 0;
                u_int16_t poll_retry = 0;
                u_int16_t poll_mcu_var = 0;
                u_int16_t poll_data = 0;
                u_int16_t poll_mask = 0;
                u_int16_t retry_cnt = 0;

                if (params[i+1].op != CAMOP_POLL_MCU_VAR)
                {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
                }

                poll_delay = (params[i+1].data & 0xff00) >> 8;
                poll_retry = (params[i+1].data & 0x00ff);
                poll_mcu_var = params[i].addr;
                poll_data = (uint8_t)params[i+1].addr;
                poll_mask = (uint8_t)params[i].data;;

                SKYCDBG("start polling MCU variable... if [0x%04x] AND 0x%04x equals 0x%04x, then stop polling", poll_mcu_var, poll_mask, poll_data);


                for (retry_cnt = 0; retry_cnt < poll_retry; retry_cnt++)
                {
                    /* PANTECH_CAMERA_TODO, vga has MCU addr ?, happens truncation */
                    if (yacbac1sddas_i2c_write_byte(YACBAC1SDDAS_REG_MCU_ADDR, (uint8_t)poll_mcu_var) < 0)
                    {
                        SKYCERR("<<%s (-EIO)", __func__);
                        return -EIO;
                    }

                    /* PANTECH_CAMERA_TODO, vga has MCU addr ?, happens truncation */
                    if (yacbac1sddas_i2c_read_byte(YACBAC1SDDAS_REG_MCU_DATA, &read_data) < 0)
                    {
                        SKYCERR("<<%s (-EIO)", __func__);
                        return -EIO;
                    }

                    if ((read_data & poll_mask) != poll_data)
                    {
                        SKYCERR("retry polling MCU variable... after sleeping %d ms", poll_delay);
                        msleep(poll_delay);
                    }
                    else
                    {
                        SKYCERR("stop polling MCU variable... retried %d/%d time(s) (delay = %d ms)", retry_cnt, poll_retry, poll_delay);
                        break;
                    }
                }

                if (retry_cnt == poll_retry)
                {
                    SKYCERR("<<%s (-EIO)", __func__);
                    return -EIO;
                }

                ++i;
#else
		return -EIO;
#endif
                break;
            }

            default:
                SKYCERR("Invalid op code! [op = %d]", params[i].op);
                return -EIO;
                break;
        }

    }

    SKYCDBG("<<%s (TRUE)", __func__);
    return TRUE;
}

static void yacbac1sddas_init_params_tbl (int8_t *stream)
{
    yacbac1sddas_cfg_param_t *params = NULL;
    u_int32_t num_params = 0;
    u_int32_t i = 0;

    SKYCDBG(">>%s ()", __func__);

    camsensor_yacbac1sddas_tup_init(stream);
    num_params = camsensor_yacbac1sddas_tup_get_init_params(&params);
    if ((num_params > 0) && (params != NULL))
    {
        yacbac1sddas_params_tbl.init.num_params = num_params;
        yacbac1sddas_params_tbl.init.params = params;
        SKYCERR("YACBAC1SDDAS INIT params are loaded from TUNEUP file!");
    }
    else
    {
        yacbac1sddas_params_tbl.init.num_params = num_params;
        yacbac1sddas_params_tbl.init.params = params;
        SKYCERR("YACBAC1SDDAS INIT params are loaded from TUNEUP file!");
#if 0
        yacbac1sddas_params_tbl.init.num_params = sizeof(yacbac1sddas_cfg_init_params) / sizeof(yacbac1sddas_cfg_param_t);
        yacbac1sddas_params_tbl.init.params = &yacbac1sddas_cfg_init_params[0];
        SKYCDBG("YACBAC1SDDAS INIT params are loaded from RO data!", 0, 0, 0);
#endif
    }

    for (i = 0; i < YACBAC1SDDAS_CFG_WB_MAX; i++)
    {
        num_params = camsensor_yacbac1sddas_tup_get_wb_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            yacbac1sddas_params_tbl.wb[i].num_params = num_params;
            yacbac1sddas_params_tbl.wb[i].params = params;
            SKYCERR("YACBAC1SDDAS WB params [%d] are loaded from TUNEUP file!", i);
        }
        else
        {
            yacbac1sddas_params_tbl.wb[i].num_params = num_params;
            yacbac1sddas_params_tbl.wb[i].params = params;
            SKYCERR("YACBAC1SDDAS WB params [%d] are loaded from TUNEUP file!", i);
#if 0
            yacbac1sddas_params_tbl.wb[i].num_params = YACBAC1SDDAS_CFG_WB_MAX_PARAMS;
            yacbac1sddas_params_tbl.wb[i].params = &yacbac1sddas_cfg_wb_params[i][0];
            SKYCDBG("YACBAC1SDDAS WB params [%d] are loaded from RO data!", i, 0, 0);
#endif
        }
    }

    for (i = 0; i < YACBAC1SDDAS_CFG_BRIGHT_MAX; i++)
    {
        num_params = camsensor_yacbac1sddas_tup_get_bright_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            yacbac1sddas_params_tbl.bright[i].num_params = num_params;
            yacbac1sddas_params_tbl.bright[i].params = params;
            SKYCERR("YACBAC1SDDAS BRIGHT params [%d] are loaded from TUNEUP file!", i);
        }
        else
        {
      	     yacbac1sddas_params_tbl.bright[i].num_params = num_params;
            yacbac1sddas_params_tbl.bright[i].params = params;
            SKYCERR("YACBAC1SDDAS BRIGHT params [%d] are loaded from TUNEUP file!", i);
#if 0
            yacbac1sddas_params_tbl.bright[i].num_params = YACBAC1SDDAS_CFG_BRIGHT_MAX_PARAMS;
            yacbac1sddas_params_tbl.bright[i].params = &yacbac1sddas_cfg_bright_params[i][0];
            SKYCDBG("YACBAC1SDDAS BRIGHT params [%d] are loaded from RO data!", i, 0, 0);
#endif
        }

    }

    for (i = 0; i < YACBAC1SDDAS_CFG_EXPOSURE_MAX; i++)
    {
        num_params = camsensor_yacbac1sddas_tup_get_exposure_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            yacbac1sddas_params_tbl.exposure[i].num_params = num_params;
            yacbac1sddas_params_tbl.exposure[i].params = params;
            SKYCERR("YACBAC1SDDAS EXPOSURE params [%d] are loaded from TUNEUP file!", i);
        }
        else
        {
            yacbac1sddas_params_tbl.exposure[i].num_params = num_params;
            yacbac1sddas_params_tbl.exposure[i].params = params;
            SKYCERR("YACBAC1SDDAS EXPOSURE params [%d] are loaded from TUNEUP file!", i);
#if 0
            yacbac1sddas_params_tbl.exposure[i].num_params = YACBAC1SDDAS_CFG_EXPOSURE_MAX_PARAMS;
            yacbac1sddas_params_tbl.exposure[i].params = &yacbac1sddas_cfg_exposure_params[i][0];
            SKYCDBG("YACBAC1SDDAS EXPOSURE params [%d] are loaded from RO data!", i, 0, 0);
#endif
        }

    }

    for (i = YACBAC1SDDAS_CFG_FFPS_MIN; i <= YACBAC1SDDAS_CFG_FFPS_MAX; i++)
    {
        num_params = camsensor_yacbac1sddas_tup_get_ffps_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            yacbac1sddas_params_tbl.ffps[i - YACBAC1SDDAS_CFG_FFPS_MIN].num_params = num_params;
            yacbac1sddas_params_tbl.ffps[i - YACBAC1SDDAS_CFG_FFPS_MIN].params = params;
            SKYCERR("YACBAC1SDDAS fixed FPS params [%d] are loaded from TUNEUP file!", i - YACBAC1SDDAS_CFG_FFPS_MIN);
        }
        else
        {
            yacbac1sddas_params_tbl.ffps[i - YACBAC1SDDAS_CFG_FFPS_MIN].num_params = num_params;
            yacbac1sddas_params_tbl.ffps[i - YACBAC1SDDAS_CFG_FFPS_MIN].params = params;
            SKYCERR("YACBAC1SDDAS fixed FPS params [%d] are loaded from TUNEUP file!", i - YACBAC1SDDAS_CFG_FFPS_MIN);
#if 0
            yacbac1sddas_params_tbl.ffps[i - YACBAC1SDDAS_CFG_FFPS_MIN].num_params = YACBAC1SDDAS_CFG_FFPS_MAX_PARAMS;
            yacbac1sddas_params_tbl.ffps[i - YACBAC1SDDAS_CFG_FFPS_MIN].params = &yacbac1sddas_cfg_ffps_params[i - YACBAC1SDDAS_CFG_FFPS_MIN][0];
            SKYCDBG("YACBAC1SDDAS fixed FPS params [%d] are loaded from RO data!", i - YACBAC1SDDAS_CFG_FFPS_MIN, 0, 0);
#endif
        }
    }
#if 0
    for (i = 0; i < YACBAC1SDDAS_CFG_REFLECT_MAX; i++)
    {
        num_params = camsensor_yacbac1sddas_tup_get_reflect_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            yacbac1sddas_params_tbl.reflect[i].num_params = num_params;
            yacbac1sddas_params_tbl.reflect[i].params = params;
            SKYCDBG("YACBAC1SDDAS REFLECT params [%d] are loaded from TUNEUP file!", i, 0, 0);
        }
        else
        {
            yacbac1sddas_params_tbl.reflect[i].num_params = num_params;
            yacbac1sddas_params_tbl.reflect[i].params = params;
            SKYCDBG("YACBAC1SDDAS REFLECT params [%d] are loaded from TUNEUP file!", i, 0, 0);
#if 0
            yacbac1sddas_params_tbl.reflect[i].num_params = YACBAC1SDDAS_CFG_FFPS_MAX_PARAMS;
            yacbac1sddas_params_tbl.reflect[i].params = &yacbac1sddas_cfg_reflect_params[i][0];
            SKYCDBG("YACBAC1SDDAS REFLECT params [%d] are loaded from RO data!", i, 0, 0);
#endif
        }
    }

    for (i = 0; i < YACBAC1SDDAS_CFG_EFFECT_MAX; i++)
    {
        num_params = camsensor_yacbac1sddas_tup_get_effect_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            yacbac1sddas_params_tbl.effect[i].num_params = num_params;
            yacbac1sddas_params_tbl.effect[i].params = params;
            SKYCDBG("YACBAC1SDDAS EFFECT params [%d] are loaded from TUNEUP file!", i, 0, 0);
        }
        else
        {
            yacbac1sddas_params_tbl.effect[i].num_params = num_params;
            yacbac1sddas_params_tbl.effect[i].params = params;
            SKYCDBG("YACBAC1SDDAS EFFECT params [%d] are loaded from TUNEUP file!", i, 0, 0);
#if 0
            yacbac1sddas_params_tbl.effect[i].num_params = YACBAC1SDDAS_CFG_EFFECT_MAX_PARAMS;
            yacbac1sddas_params_tbl.effect[i].params = &yacbac1sddas_cfg_effect_params[i][0];
            SKYCDBG("YACBAC1SDDAS EFFECT params [%d] are loaded from RO data!", i, 0, 0);
#endif
	}
    }

for (i = 0; i < YACBAC1SDDAS_CFG_FLICKER_MAX; i++)
    {
        num_params = camsensor_yacbac1sddas_tup_get_flicker_params(i, &params);
        if ((num_params > 0) && (params != NULL))
        {
            yacbac1sddas_params_tbl.flicker[i].num_params = num_params;
            yacbac1sddas_params_tbl.flicker[i].params = params;
            SKYCDBG("YACBAC1SDDAS FLICKER params [%d] are loaded from TUNEUP file!", i, 0, 0);
        }
        else
        {
            yacbac1sddas_params_tbl.flicker[i].num_params = num_params;
            yacbac1sddas_params_tbl.flicker[i].params = params;
	     SKYCDBG("YACBAC1SDDAS FLICKER params [%d] are loaded from TUNEUP file!", i, 0, 0);
#if 0
            yacbac1sddas_params_tbl.flicker[i].num_params = YACBAC1SDDAS_CFG_FLICKER_MAX_PARAMS;
            yacbac1sddas_params_tbl.flicker[i].params = &yacbac1sddas_cfg_flicker_params[i][0];
            SKYCDBG("YACBAC1SDDAS FLICKER params [%d] are loaded from RO data!", i, 0, 0);
#endif
	}
    }
#endif
    SKYCDBG("<<%s ()", __func__);
    return;
}

static int yacbac1sddas_set_tune_value(const struct sensor_cfg_data *scfg)
{
	int32_t rc = 0;
	int32_t loop_count = 0;
	int32_t i = 0;
	int32_t last_size = 0;
	int32_t copy_size = 0;
	uint8_t * pKTune_stream = NULL;
	uint8_t * pUTune_stream = NULL;

if(yacbac1sddas_tup_state != YACBAC1SDDAS_TUNE_STATE_LOAD_VALUE)
{
	SKYCDBG("%s start\n",__func__);

	pKTune_stream = kmalloc(scfg->cfg.tune_value.file_size, GFP_KERNEL);
	SKYCDBG("pKTune_stream =%p\n", pKTune_stream);
	if (!pKTune_stream) {
		SKYCERR("failed to malloc.\n");
		rc = -ENOMEM;
		goto update_fw_user_fail;
	}

	//유저인터페이스에서 튜닝 파일 스트림을 얻어옴
	pUTune_stream = scfg->cfg.tune_value.pfile_stream;
	//페이지 단위 사이즈로 나누어 loop_count 저장
	loop_count = (scfg->cfg.tune_value.file_size / 4096) + 1;
	last_size = scfg->cfg.tune_value.file_size % 4096;
	copy_size = 4096;

	SKYCDBG("scfg->cfg.tune_value.pfile_stream=%p, scfg->cfg.tune_value.file_size=%d\n", scfg->cfg.tune_value.pfile_stream, scfg->cfg.tune_value.file_size);
	for(i = 0; i < loop_count; i++)
	{
		//마지막 루프일 경우 남은 사이즈로 변경
		if(i == (loop_count-1))
			copy_size = last_size;
		else
			copy_size = 4096;

		if (copy_from_user(pKTune_stream + i*copy_size, (void *)pUTune_stream +  i*copy_size, copy_size))
		{
			rc = -EFAULT;
			goto update_fw_user_fail;
        	}
		//SKYCDBG(" i =%d, loop_count=%d, copy_size=%d, pKTune_stream=%x\n", i, loop_count, copy_size, pKTune_stream);
	}

	//파일 스트림에서 개별 튜닝값을 배열에 로딩
	yacbac1sddas_init_params_tbl(pKTune_stream);

	yacbac1sddas_tup_state = YACBAC1SDDAS_TUNE_STATE_LOAD_VALUE;
}

#if 0
	if (pKTune_stream)
		kfree(pKTune_stream);
#endif
	SKYCDBG("%s done.\n", __func__);
	return 0;

update_fw_user_fail:
	if (pKTune_stream)
	{
		kfree(pKTune_stream);
		pKTune_stream = NULL;
	}
	SKYCDBG("%s error. rc=%d\n", __func__, rc);
	return rc;
}
#endif

static int yacbac1sddas_sensor_init_probe(const struct msm_camera_sensor_info *data)
{
	int32_t rc;
	SKYCDBG("%s start\n", __func__);


	/* Need to fps regisetre setting */
#ifdef CONFIG_PANTECH_CAMERA_TUNER
	if(yacbac1sddas_tup_mode == YACBAC1SDDAS_TUNE_STATE_TUNNING_MODE_ON)
		return rc;
#endif

	rc = yacbac1sddas_reg_init();
	if(rc < 0)
		goto init_probe_fail;
	return rc;

init_probe_fail:
	SKYCDBG("%s init_probe_fail\n",__func__);
	return rc;
}


int32_t yacbac1sddas_sensor_init(const struct msm_camera_sensor_info *data)
{


	int32_t rc = 0;
//	uint16_t fps = 0;

	SKYCDBG("%s:	Line:%d \n", __func__,__LINE__);


	SKYCDBG("%s start\n", __func__);

	rc = yacbac1s_power_on();
	if(rc)
	{
		rc = -ENOENT;
		SKYCERR(" yacbac1sddas_power failed rc = %d\n", rc);
		goto init_fail;
	}

	yacbac1sddas_ctrl = kzalloc(sizeof(struct yacbac1sddas_ctrl_t), GFP_KERNEL);
	if( !yacbac1sddas_ctrl)
	{
		SKYCERR("yacbac1sddas_init failed!\n");
		rc = -ENOMEM;
		goto init_fail;
	}


	if(data)
		yacbac1sddas_ctrl->sensordata = data;

	/* enable mclk first */
	msm_camio_clk_rate_set(YACBAC1SDDAS_DEFAULT_CLOCK_RATE);
	msleep(20);

	msm_camio_camif_pad_reg_reset();
	msleep(20);

	SKYCDBG("%s:	Line:%d \n", __func__, __LINE__);
	rc = yacbac1sddas_sensor_init_probe(data);
	if( rc < 0)
		goto init_fail;
	/* if need other initialize, I should add the initialize */

	goto init_done;
	/******************************/
	/*********************************************************/

init_done:
	SKYCDBG("%s end\n", __func__);
	return rc;

init_fail:
	SKYCDBG("%s init_fail\n", __func__);
	(void)yacbac1s_power_off();
	kfree(yacbac1sddas_ctrl);
	return rc;
}

static int32_t yacbac1sddas_init_client(struct i2c_client *client)
{
	/* Initialize the MSM_CAMI2C Chip */
	init_waitqueue_head(&yacbac1sddas_wait_queue);
	return 0;
}



int32_t yacbac1sddas_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cfg_data;
    int32_t rc = 0;

	if( copy_from_user(&cfg_data, (void *)argp, sizeof(struct sensor_cfg_data)))
	{
		SKYCERR("%s copy_from_user error\n" , __func__);
		return -EFAULT;
	}

	SKYCDBG("yacbac1sddas_ioctl, cfgtype = %d, mode %d\n", cfg_data.cfgtype, cfg_data.mode);
	//mutex_lock(&yacbac1sddas_mut);

	switch(cfg_data.cfgtype) // If need more function. Add the case
	{
		case CFG_SET_MODE:
			rc = yacbac1sddas_set_sensor_mode(cfg_data.mode);
			break;

		case CFG_SET_EFFECT:
			rc = yacbac1sddas_set_effect(cfg_data.cfg.effect);
			break;

		case CFG_SET_BRIGHTNESS:
			rc = yacbac1sddas_set_brightness(cfg_data.cfg.brightness);
			SKYCDBG("%s: CFG_SET_BRIGHTNESS, rc = %d\n", __func__, rc);
			break;

		case CFG_SET_FPS:
			rc = yacbac1sddas_set_frame_rate(cfg_data.cfg.p_fps);
//			rc = yacbac1sddas_set_preview_fps(cfg_data.cfg.p_fps);
			SKYCDBG("%s: CFG_SET_FPS, rc = %x\n", __func__, rc);
			break;

		case CFG_SET_WB:
			rc = yacbac1sddas_set_wb(cfg_data.cfg.wb);
			break;

#ifdef CONFIG_PANTECH_CAMERA_TUNER
		case CFG_SET_TUNE_VALUE:
			SKYCDBG("%s: CFG_SET_TUNE_VALUE, rc=%d\n", __func__, rc);
			rc = yacbac1sddas_set_tune_value(&cfg_data);
			break;
#endif

		case CFG_SET_EXPOSURE_MODE:
			rc = yacbac1sddas_set_exposure_mode( cfg_data.cfg.exposure);
			break;

		case CFG_SET_REFLECT:
			SKYCDBG("%s: CFG_SET_REFLECT, rc=%d\n", __func__, rc);
			rc = yacbac1sddas_set_reflect(cfg_data.cfg.reflect);
			break;

		case CFG_GET_AF_MAX_STEPS:
		default:
			rc = -EINVAL;
			break;

	}
	//mutex_unlock(&yacbac1sddas_mut);
	return rc;
}




int32_t yacbac1sddas_sensor_release(void)
{
//	int rc = -EBADF;
	int rc = 0;
	SKYCDBG("%s is called\n", __func__);

	rc = yacbac1s_power_off();

	if(rc < 0)
		return rc;

	kfree(yacbac1sddas_ctrl);
	yacbac1sddas_ctrl = NULL;

#ifdef CONFIG_PANTECH_CAMERA_TUNER
	//튜닝값 로딩 체크 flag
	yacbac1sddas_tup_state = YACBAC1SDDAS_TUNE_STATE_NONE;
	//yacbac1sddas_done_set_tune_load = FALSE;
#endif

	SKYCDBG("%s done.\n", __func__);
	return 0;
}



static int yacbac1sddas_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{

	int rc = 0;
	SKYCDBG("%s called\n", __func__);

	if ( !i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
	{
		SKYCERR("i2c_check_faunctionality failed\n");
		rc = -ENOTSUPP;
		goto probe_failure;
	}

	yacbac1sddas_sensorw = kzalloc(sizeof(struct yacbac1sddas_work), GFP_KERNEL);
	if( !yacbac1sddas_sensorw)
	{
		SKYCERR("kzalloc failed.\n");
		rc = -ENOMEM;
		goto probe_failure;
	}

	i2c_set_clientdata(client, yacbac1sddas_sensorw);
	yacbac1sddas_init_client(client);
	yacbac1sddas_client = client;
	msleep(50);

	SKYCDBG("%s successed! rc = %d\n",__func__, rc);
	return 0;

probe_failure:
	SKYCDBG("%s failed! rc = %d\n", __func__ ,rc);
	kfree(yacbac1sddas_sensorw);

	return rc;

}

static const struct i2c_device_id yacbac1sddas_i2c_id[] =
{
		{"yacbac1sddas", 0},
		{},
};


static struct i2c_driver yacbac1sddas_i2c_driver = {
	.id_table = yacbac1sddas_i2c_id,
	.probe = yacbac1sddas_i2c_probe,
	.remove = __exit_p(yacbac1sddas_i2c_remove),
	.driver = {
		.name = "yacbac1sddas",
			},
};

/* Need to check this module  */

static int32_t yacbac1sddas_init_i2c(void)
{
	int32_t rc = 0;
	SKYCDBG("%s start\n", __func__);

	rc = i2c_add_driver(&yacbac1sddas_i2c_driver);
	SKYCDBG("%s ,rc = %d\n",__func__, rc);
	if( IS_ERR_VALUE(rc))
		goto init_i2c_fail;

	SKYCDBG("%s end\n", __func__);
	return 0;

init_i2c_fail:
	SKYCERR("%s failed! (%d)\n", __func__, rc);
	return rc;
}






static int yacbac1sddas_sensor_probe(const struct msm_camera_sensor_info *info, struct msm_sensor_ctrl *s)
{
	int rc = 0;
	SKYCDBG("%s start",__func__);

#if 1
	rc =yacbac1sddas_init_i2c(); // need to i2c initialize rotuine
	SKYCDBG("%s rc = %d \n", __func__, rc);
	if( rc < 0 || yacbac1sddas_client == NULL )
	{
		SKYCERR("%s rc = %d \n", __func__, rc);
		rc = -ENOTSUPP;
		goto probe_fail;
	}
#else
	rc =  i2c_add_driver(&yacbac1sddas_i2c_driver);
	SKYCDBG("%s rc = %d, yacbac1sddas_client = %x \n", __func__, rc, yacbac1sddas_client);
	if ( rc < 0 /* || yacbac1sddas_client == NULL */)
	{
		rc = -ENOTSUPP;
		goto probe_fail;
	}
#endif

#if 0
	rc = yacbac1sddas_power(ON); // Make VGA camera power module
	if (rc < 0)
	{
		SKYCDBG("%s rc = %d, \n", __func__, rc);
		goto probe_fail;
	}
	msm_camio_clk_rate_set(YACBAC1SDDAS_DEFAULT_CLOCK_RATE);
	msleep(20);

	rc = yacbac1sddas_sensor_init_probe(info);
	if( rc < 0)
	{
		SKYCDBG("%s rc = %d, \n", __func__, rc);
		goto probe_fail;
	}
#endif

	s->s_init = yacbac1sddas_sensor_init;
	s->s_release = yacbac1sddas_sensor_release;
	s->s_config = yacbac1sddas_sensor_config;
#if 0
	rc = yacbac1sddas_power(OFF);
	if( rc <0 )
	{
		SKYCDBG("%s rc = %d, \n", __func__, rc);
		goto probe_fail;
	}
#endif

	s->s_camera_type = FRONT_CAMERA_2D;
#ifdef F_SKYCAM_FAKE_FRONT_CAMERA
	s->s_mount_angle = 270;
#else
	s->s_mount_angle = 0;
#endif
//PANTECH_CAMERA_TODO, what's this?	gpio_set_value_cansleep(info->sensor_reset, 0);
//	msleep(20);
//	gpio_free(info->sensor_reset);


	SKYCDBG("%s successed!\n", __func__);
	return 0;

probe_fail:
	SKYCERR("%s failed!  rc = %d\n", __func__, rc);
	return rc;
}



static int __yacbac1sddas_probe(struct platform_device *pdev)
{
	SKYCDBG("%s start\n", __func__);
	return msm_camera_drv_start(pdev,yacbac1sddas_sensor_probe);
}

static struct platform_driver msm_camera_driver = {
	.probe = __yacbac1sddas_probe,
	.driver = {
	    .name ="msm_camera_yacbac1sddas",
		.owner = THIS_MODULE,
		},
};

static int __init yacbac1sddas_init(void)
{
	SKYCDBG("%s start\n", __func__);
	return platform_driver_register(&msm_camera_driver);
}
module_init(yacbac1sddas_init);

