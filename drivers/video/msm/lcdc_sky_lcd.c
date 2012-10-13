/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora Forum nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * Alternatively, provided that this notice is retained in full, this software
 * may be relicensed by the recipient under the terms of the GNU General Public
 * License version 2 ("GPL") and only version 2, in which case the provisions of
 * the GPL apply INSTEAD OF those given above.  If the recipient relicenses the
 * software under the GPL, then the identification text in the MODULE_LICENSE
 * macro must be changed to reflect "GPLv2" instead of "Dual BSD/GPL".  Once a
 * recipient changes the license terms to the GPL, subsequent recipients shall
 * not relicense under alternate licensing terms, including the BSD or dual
 * BSD/GPL terms.  In addition, the following license statement immediately
 * below and between the words START and END shall also then apply when this
 * software is relicensed under the GPL:
 *
 * START
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 and only version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * END
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

//#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <mach/gpio.h>
#include <mach/pmic.h>
#include "msm_fb.h"

#ifndef __LCD_DEBUG__
#define __LCD_DEBUG__
#define ENTER_FUNC2() printk(KERN_ERR "[SKY_LCD] +%s\n", __FUNCTION__);
#define EXIT_FUNC2() printk(KERN_ERR "[SKY_LCD] -%s\n", __FUNCTION__);
#define PRINT(X) printk(KERN_ERR "[SKY_LCD] -%s:%s\n", __FUNCTION__,X);
#define PRINT2(X) printk(KERN_ERR "[SKY_LCD] -%s:%d\n", __FUNCTION__,X);
#endif /* __LCD_DEBUG__ */

// 20100702, kkcho, 사용 모델별 정의
//=============================
//== EF18K WVGA(480x800) Casio & Ortustech ==
#define LCDC_SKY_LCD_COM32T3M621LX  // WS부터 적용 예정(PT는 MDDI사용)

//== 모델 추가.... ==

// Backlight
#define FEATURE_SKY_BACLIGHT_SC620 //EF18K & EF14L
//#define I2C_SW_CONTROL  // kkcho, HW_i2c Error시 SW_GPIO_CTRL로 확인필요할때 사용.
//=============================

/* define spi function */
static void send_spi_command(u8 reg, u8 count, u8 *param);

#ifdef SKY_MDP_VG_CLEAR_HOLD_CHANGE  
extern void force_mdp4_overlay_unset(void);
extern void force_mdp4_overlay_control(bool onoff);    
#endif

#define SPI_SCLK		   45
#define SPI_CS		   46
#define SPI_SDI		   47
#define SPI_SDO		   48
#define LCD_RESET	   122
#define LCD_BL_EN        130

#ifdef I2C_SW_CONTROL
#define LCD_BL_SDA      173  // board_msm7230.c에서 이루어짐.. 
#define LCD_BL_SCL      174

const uint8 SC620_I2C_ID = 0xE0;
const uint8 SC620_I2C_DELAY = 5;
#endif

#define GPIO_HIGH_VALUE	1
#define GPIO_LOW_VALUE	       0

#define DEBUG_EN 0

#ifdef FEATURE_SKY_BACLIGHT_SC620
#define SC620_DEVICE_ADDRESS       0xE0  // ID 등록은 board-msm7x30.c에서 이루어짐....
#define SC620_LED_ONOFF_CTL_REG 0x00
#define SC620_LED1_DIMMING_CTL   0x01
#define SC620_LED2_DIMMING_CTL   0x02
#define SC620_LED3_DIMMING_CTL   0x03
#define SC620_LED4_DIMMING_CTL   0x04
#define SC620_LED5_DIMMING_CTL   0x05
#define SC620_LED6_DIMMING_CTL   0x06
#define SC620_LED7_DIMMING_CTL   0x07
#define SC620_LED8_DIMMING_CTL   0x08
#define SC620_GAIN_REGISTER          0x09
#define SC620_DIMMING_DEFAULT     0x01
#define SC620_DIMMING_MAX             0x32
#define SC620_GAIN_DEFAULT           0x08
#define SC620_GAIN_1                        0x03
#define SC620_GAIN_2                        0x05
#define SC620_GAIN_3                        0x07
#define SC620_GAIN_4                        0x09
#define SC620_GAIN_5                        0x0B
#define SC620_GAIN_6                        0x0D
#define SC620_GAIN_MAX                   0x0F

static boolean flag_lcd_bl_off = FALSE;
#if defined(CONFIG_MACH_MSM8X55_EF30S) || defined(CONFIG_MACH_MSM8X55_EF18K) 
#if (BOARD_VER == WS20)
static boolean flag_lcd_bl_ctrl = TRUE;
#endif
#endif

#endif

//#define NOP()   do {asm volatile ("NOP");} while (0)
#define NOP()       udelay(1);

u8 parameter_list[30];

static unsigned char bit_shift[8] = { (1 << 7),	/* MSB */
	(1 << 6),
	(1 << 5),
	(1 << 4),
	(1 << 3),
	(1 << 2),
	(1 << 1),
	(1 << 0)		               /* LSB */
};

static struct msm_panel_common_pdata *lcdc_sky_pdata;

static uint32_t lcd_gpio_init_table[] = {
	//GPIO_CFG(LCD_RESET, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(LCD_BL_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	//GPIO_CFG(SPI_SCLK, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	//GPIO_CFG(SPI_CS, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	//GPIO_CFG(SPI_SDI, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	//GPIO_CFG(SPI_SDO, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), 
};

static void lcd_gpio_init(uint32_t *table, int len, unsigned enable)
{
	int n, rc;
	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n],
			enable ? GPIO_CFG_ENABLE : GPIO_CFG_DISABLE);
		if (rc) {
			printk(KERN_ERR "%s: gpio_tlmm_config(%#x)=%d\n",
				__func__, table[n], rc);
			break;
		}
	}
}

#ifdef FEATURE_SKY_BACLIGHT_SC620
#ifndef I2C_SW_CONTROL
static struct i2c_client *led_i2c_client = NULL;

static int led_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int rc = 0;

	ENTER_FUNC2(); 
#if 0
	led_i2c_client = client;
#else
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
	{
		led_i2c_client = NULL;
		rc = -1;
		if(DEBUG_EN) 
		printk(KERN_ERR "[led_i2c_probe] failed!!!\n");
	}
	else
	{
		led_i2c_client = client;
	}
#endif
	EXIT_FUNC2();

	if(DEBUG_EN) printk(KERN_ERR "[led_i2c_probe] successed!!!\n");

	return rc;
}


static int led_i2c_remove(struct i2c_client *client)
{
#if 0
	int rc;

	rc = i2c_detach_client(client);

	return rc;
#endif

	led_i2c_client = NULL;

	if(DEBUG_EN) printk(KERN_ERR "[led_i2c_probe] removed!!!\n");
	return 0;
}

static const struct i2c_device_id led_id[] = {
	{ "SC620", 0 },  { }
};

static struct i2c_driver led_i2c_driver = {
	.driver = {
		.name = "SC620",
		.owner = THIS_MODULE,
	},
	.probe = led_i2c_probe,
	.remove = __devexit_p(led_i2c_remove),
	.id_table = led_id,
};
#endif

static void led_i2c_api_Init(void)
{
#ifdef I2C_SW_CONTROL
	lcd_gpio_init(backlight_gpio_init_table, ARRAY_SIZE(backlight_gpio_init_table), 1);
	gpio_set_value(LCD_BL_SDA, GPIO_LOW_VALUE);
	gpio_set_value(LCD_BL_SDA, GPIO_LOW_VALUE);   
#else
	int result = 0;

	ENTER_FUNC2();
	result = i2c_add_driver(&led_i2c_driver);
	if(result){
		if(DEBUG_EN) printk(KERN_ERR "[led_i2c_api_Init] error!!!\n");
	}
	EXIT_FUNC2();
#endif  
}

#if (0)
static void led_i2c_api_DeInit(void)
{
	i2c_del_driver(&led_i2c_driver);
}
#endif

#ifdef FEATURE_I2C_WRITE_CHECK
static uint8 led_i2c_master_recv(uint8 reg)
{
	static int ret = 0;
	unsigned char wbuf[2];
	unsigned char rbuf[2] = {0,0};
	struct i2c_msg msgs[2];
	uint8 uiData;

	if(!led_i2c_client)
	{
		if(DEBUG_EN) printk(KERN_ERR "[led_i2c_write_word] led_i2c_client is null!!!\n");
		return -1;
	}

	wbuf[0] = reg;

	msgs[0].addr = led_i2c_client->addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = wbuf;

	msgs[1].addr = led_i2c_client->addr;
	msgs[1].flags = 1; //I2C_M_RD;
	msgs[1].len = 1;
	msgs[1].buf = rbuf;

#ifdef FEATURE_I2C_DBG_MSG
	if(DEBUG_EN) printk(KERN_ERR "[led_i2c_read_word] ID[0x%02x] reg[0x%02x]\n", led_i2c_client->addr, reg);
#endif

#if 1 // android 3145 버전에서 msgs 2개 한번에 보내면 에러리턴됨
	ret = i2c_transfer(led_i2c_client->adapter, msgs, 1);
	if (ret < 0)
	{
		if(DEBUG_EN) printk(KERN_ERR "[led_i2c_read_word] write error!!!\n");
		return FALSE;
	}
	ret = i2c_transfer(led_i2c_client->adapter, &msgs[1], 1);
#else
	ret = i2c_transfer(led_i2c_client->adapter, msgs, 2);
#endif
	if (ret < 0)
	{
		if(DEBUG_EN) printk(KERN_ERR "[led_i2c_read_word] read error!!!\n");
			return FALSE;
	}
	else
	{
		uiData = rbuf[0];
#ifdef FEATURE_I2C_DBG_MSG
		if(DEBUG_EN) printk(KERN_ERR "[led_i2c_read_word] read OK!!!\n");
		if(DEBUG_EN) printk(KERN_ERR "[led_i2c_read_word] reg[0x%02x], data[0x%02x]\n", reg, uiData);
#endif
		return uiData;
	}
}
#endif

#ifdef I2C_SW_CONTROL
static void sda_write (unsigned char onoff)
{
	if(onoff)
	{
		gpio_set_value(LCD_BL_SDA, GPIO_HIGH_VALUE);
	}
	else
	{
		gpio_set_value(LCD_BL_SDA, GPIO_LOW_VALUE);
	}
} 

static void scl_write (unsigned char onoff)
{
	if(onoff)
	{
		gpio_set_value(LCD_BL_SCL, GPIO_HIGH_VALUE);
	}
	else
	{
		gpio_set_value(LCD_BL_SCL, GPIO_LOW_VALUE);
	}
}

static void SCL_DELAY (void)
{
	clk_busy_wait(SC620_I2C_DELAY);
}

static void i2c_wait (void)
{
	clk_busy_wait(SC620_I2C_DELAY);
}


static void i2c_start_condition (void)
{
	sda_write (1);
	scl_write (1);  
	sda_write(0);
	SCL_DELAY();
	scl_write (0);
	i2c_wait();
}

static void i2c_stop_condition (void)
{
	sda_write (0);
	scl_write(1);
	SCL_DELAY();
	sda_write (1);
	i2c_wait();  
}

static void i2c_ack (void)
{
	sda_write (0);
	scl_write (1);
	SCL_DELAY();
	scl_write(0);
	i2c_wait();
}

static void i2c_write_byte(unsigned char data)
{
	signed char  bit_idx;

	for (bit_idx = 7; bit_idx >= 0; bit_idx--)
	{
		sda_write (data>>bit_idx & 0x01);
		SCL_DELAY();
		scl_write (1);
		SCL_DELAY();
		scl_write (0);
		//BOOT_SCL_DELAY();
	}
	//boot_i2c_ack();
}
#endif

#if defined(CONFIG_MACH_MSM8X55_EF30S) || defined(CONFIG_MACH_MSM8X55_EF18K) 
#if (BOARD_VER == WS20)
static uint8 led_i2c_master_send(uint8 reg, uint8 data)
{
#ifdef I2C_SW_CONTROL
	i2c_start_condition();
	i2c_write_byte(SC620_I2C_ID);
	i2c_ack();
	i2c_write_byte(reg);
	i2c_ack();
	i2c_write_byte(data);
	i2c_ack();
	i2c_stop_condition();
	return TRUE;
#else
	static int ret = 0;
	unsigned char buf[4];
	struct i2c_msg msg[2];
#ifdef FEATURE_I2C_WRITE_CHECK
	uint16 rData;
#endif /* FEATURE_I2C_WRITE_CHECK */

	if(!led_i2c_client)
	{
		if(DEBUG_EN) printk(KERN_ERR "[led_i2c_master_send] led_i2c_client is null!!!\n");
			return -1;
	}

	buf[0] = reg;
	buf[1] = data;

	msg[0].addr = led_i2c_client->addr;  
	msg[0].flags = 0;
	msg[0].len = 2;
	msg[0].buf = buf;

#ifdef FEATURE_I2C_DBG_MSG
	if(DEBUG_EN) printk(KERN_ERR "[led_i2c_master_send] ID[0x%02x] reg[0x%02x] data[0x%02x]\n", led_i2c_client->addr, reg, data);
#endif /* FEATURE_I2C_DBG_MSG */

	ret = i2c_transfer(led_i2c_client->adapter, msg, 1);
	if (ret < 0)
	{
		if(DEBUG_EN) printk(KERN_ERR "[led_i2c_master_send] write error!!!\n");
			return FALSE;
	}
	else
	{
#ifdef FEATURE_I2C_DBG_MSG
		if(DEBUG_EN) printk(KERN_ERR "[led_i2c_master_send] write OK!!!\n");
#endif /* FEATURE_I2C_DBG_MSG */

#ifdef FEATURE_I2C_WRITE_CHECK
		rData = led_i2c_master_recv(reg);
		if (rData != data)
		{
			if(DEBUG_EN) printk(KERN_ERR "[led_i2c_master_send:error] reg[0x%02x], data[0x%02x]\n", reg, rData);
		}
#endif /* FEATURE_I2C_WRITE_CHECK */
		return TRUE;
	}
#endif
}
#endif
#endif


#if (defined(CONFIG_MACH_MSM8X55_EF30S) || defined(CONFIG_MACH_MSM8X55_EF18K))  && defined(CONFIG_PANTECH_ERROR_LOG) 
static void backlight_on(void)
{
        ENTER_FUNC2();  
        gpio_set_value(LCD_BL_EN ,GPIO_LOW_VALUE);
        mdelay(5);
        gpio_set_value(LCD_BL_EN ,GPIO_HIGH_VALUE);  
        mdelay(5);           
        led_i2c_master_send(SC620_LED_ONOFF_CTL_REG, 0xFF); 

        led_i2c_master_send(SC620_LED1_DIMMING_CTL, SC620_DIMMING_MAX);
        led_i2c_master_send(SC620_LED2_DIMMING_CTL, SC620_DIMMING_MAX);
        led_i2c_master_send(SC620_LED3_DIMMING_CTL, SC620_DIMMING_MAX);
        led_i2c_master_send(SC620_LED4_DIMMING_CTL, SC620_DIMMING_MAX);            
        led_i2c_master_send(SC620_LED5_DIMMING_CTL, SC620_DIMMING_MAX);       
        led_i2c_master_send(SC620_LED6_DIMMING_CTL, SC620_DIMMING_MAX);   
        led_i2c_master_send(SC620_LED7_DIMMING_CTL, SC620_DIMMING_MAX);
        led_i2c_master_send(SC620_LED8_DIMMING_CTL, SC620_DIMMING_MAX);        
        led_i2c_master_send(SC620_GAIN_REGISTER, SC620_GAIN_MAX);    
        EXIT_FUNC2();  
}
#endif
static void backlight_off(void)
{
#ifdef SKY_MDP_VG_CLEAR_HOLD_CHANGE  
      signed char  mdp_reg_fush_counter;
#endif
	ENTER_FUNC2();   
	led_i2c_master_send(SC620_GAIN_REGISTER, 0x00);
	led_i2c_master_send(SC620_LED1_DIMMING_CTL, 0x00);
	led_i2c_master_send(SC620_LED2_DIMMING_CTL, 0x00);
	led_i2c_master_send(SC620_LED3_DIMMING_CTL, 0x00);
	led_i2c_master_send(SC620_LED4_DIMMING_CTL, 0x00);            
	led_i2c_master_send(SC620_LED5_DIMMING_CTL, 0x00);       
	led_i2c_master_send(SC620_LED6_DIMMING_CTL, 0x00);   
	led_i2c_master_send(SC620_LED7_DIMMING_CTL, 0x00);
	led_i2c_master_send(SC620_LED8_DIMMING_CTL, 0x00);    

	led_i2c_master_send(SC620_LED_ONOFF_CTL_REG, 0x00);     
	mdelay(5);
	gpio_set_value(LCD_BL_EN ,GPIO_LOW_VALUE);
	flag_lcd_bl_off = TRUE;
#ifdef SKY_MDP_VG_CLEAR_HOLD_CHANGE    
       force_mdp4_overlay_control(1);  // screen_hold
       for (mdp_reg_fush_counter = 100; mdp_reg_fush_counter >= 0; mdp_reg_fush_counter--)   
       {
            force_mdp4_overlay_unset();  // force_overlay_view_clear 
        }
        force_mdp4_overlay_control(1); // screen_hold
#endif        
	EXIT_FUNC2();  
}
#endif /*FEATURE_SKY_BACLIGHT_SC620*/

#if defined(CONFIG_MACH_MSM8X55_EF30S) || defined(CONFIG_MACH_MSM8X55_EF18K) 
#if (BOARD_VER == WS20)
static void send_spi_command(u8 command, u8 count, u8 *parameter_list)
{
	u8 i,j = 0;

	/* Enable the Chip Select */
	gpio_set_value(SPI_CS, GPIO_LOW_VALUE);
	NOP();
	NOP();

	gpio_set_value(SPI_SCLK, GPIO_LOW_VALUE);
	NOP();
	//CMD
	/* DNC : COMMAND = 0, PARAMETER = 1  */
	gpio_set_value(SPI_SDI, GPIO_LOW_VALUE);

	/* #2: Drive the Clk High*/
	NOP(); 
	gpio_set_value(SPI_SCLK, GPIO_HIGH_VALUE);   
	NOP();
	NOP();
   
	/* Before we enter here the Clock should be Low ! */
	for(i=0;i<8;i++)
	{
		gpio_set_value(SPI_SCLK, GPIO_LOW_VALUE);
		NOP();
		/* #1: Drive the Data (High or Low) */
		if(command & bit_shift[i])
		gpio_set_value(SPI_SDI, GPIO_HIGH_VALUE);        
		else
		gpio_set_value(SPI_SDI, GPIO_LOW_VALUE);        
		/* #2: Drive the Clk High*/
		NOP(); 
		gpio_set_value(SPI_SCLK, GPIO_HIGH_VALUE); 
		NOP();
		NOP();
	}

	//PARAMETER
	for(j=0; j<count; j++)
	{
		gpio_set_value(SPI_SCLK, GPIO_LOW_VALUE);
		NOP();
		/* DNC : COMMAND = 0, PARAMETER = 1  */
		gpio_set_value(SPI_SDI, GPIO_HIGH_VALUE); 
		/* #2: Drive the Clk High*/
		NOP(); 
		gpio_set_value(SPI_SCLK, GPIO_HIGH_VALUE);    
		NOP();
		NOP();

		for(i=0;i<8;i++)
		{
			gpio_set_value(SPI_SCLK, GPIO_LOW_VALUE);
			NOP();
			/* #1: Drive the Data (High or Low) */
			if(parameter_list[j] & bit_shift[i])
			    gpio_set_value(SPI_SDI, GPIO_HIGH_VALUE); 
			else
			    gpio_set_value(SPI_SDI, GPIO_LOW_VALUE); 

			/* #2: Drive the Clk High*/
			NOP(); 
			gpio_set_value(SPI_SCLK, GPIO_HIGH_VALUE);    
			NOP(); 
			NOP();     
		}  
	}
	/* Idle state of SDO (MOSI) is Low */
	gpio_set_value(SPI_SDI, GPIO_LOW_VALUE); 
	gpio_set_value(SPI_SCLK, GPIO_LOW_VALUE);

	/* Now Disable the Chip Select */
	NOP();
	NOP();
	gpio_set_value(SPI_CS, GPIO_HIGH_VALUE);

	NOP();
}
#endif
#endif


#if defined(CONFIG_MACH_MSM8X55_EF30S) || defined(CONFIG_MACH_MSM8X55_EF18K) 
#if (BOARD_VER == WS20)
static void lcd_setup(void)
{
	ENTER_FUNC2();

	gpio_set_value(LCD_RESET, GPIO_LOW_VALUE);
	mdelay(6);
	gpio_set_value(LCD_RESET, GPIO_HIGH_VALUE);
	mdelay(12);   

	parameter_list[0] = 0xFF;
	parameter_list[1] = 0x83;
	parameter_list[2] = 0x63;
	send_spi_command(0xB9, 3, parameter_list);

	parameter_list[0] = 0x81;
	parameter_list[1] = 0x24;
	parameter_list[2] = 0x04;
	parameter_list[3] = 0x02;
	parameter_list[4] = 0x02;
	parameter_list[5] = 0x03;
	parameter_list[6] = 0x10;
	parameter_list[7] = 0x10;
	parameter_list[8] = 0x34;
	parameter_list[9] = 0x3C;
	parameter_list[10] = 0x3F;
	parameter_list[11] = 0x3F;
	send_spi_command(0xB1, 12, parameter_list);

	send_spi_command(0x11, 0, parameter_list);
	mdelay(5);

	send_spi_command(0x20, 0, parameter_list);

	parameter_list[0] = 0x00;
	send_spi_command(0x36, 1, parameter_list);

	parameter_list[0] = 0x70;
	send_spi_command(0x3A, 1, parameter_list);
	mdelay(120);

	parameter_list[0] = 0x78;
	parameter_list[1] = 0x24;
	parameter_list[2] = 0x04;
	parameter_list[3] = 0x02;
	parameter_list[4] = 0x02;
	parameter_list[5] = 0x03;
	parameter_list[6] = 0x10;
	parameter_list[7] = 0x10;
	parameter_list[8] = 0x34;
	parameter_list[9] = 0x3C;
	parameter_list[10] = 0x3F;
	parameter_list[11] = 0x3F;
	send_spi_command(0xB1, 12, parameter_list);

	parameter_list[0] = 0x09;
	send_spi_command(0xB3, 1, parameter_list);

       parameter_list[0] = 0x00;
       parameter_list[1] = 0x08;
       parameter_list[2] = 0x6E;
       parameter_list[3] = 0x07;
       parameter_list[4] = 0x01;
       parameter_list[5] = 0x01;
       parameter_list[6] = 0x62;
       parameter_list[7] = 0x01;
       parameter_list[8] = 0x57;    
	send_spi_command(0xB4, 9, parameter_list);

	parameter_list[0] = 0x0B;
	send_spi_command(0xCC, 1, parameter_list);

       parameter_list[0] = 0x03;
       parameter_list[1] = 0x49;
       parameter_list[2] = 0x4E;
       parameter_list[3] = 0x4C;
       parameter_list[4] = 0x57;
       parameter_list[5] = 0xF4;
       parameter_list[6] = 0x0B;
       parameter_list[7] = 0x4E;
       parameter_list[8] = 0x92;
       parameter_list[9] = 0x57;
       parameter_list[10] = 0x1A;
       parameter_list[11] = 0x99;
       parameter_list[12] = 0x96;
       parameter_list[13] = 0x0C;
       parameter_list[14] = 0x10;
       parameter_list[15] = 0x01;
       parameter_list[16] = 0x47;
       parameter_list[17] = 0x4D;
       parameter_list[18] = 0x57;
       parameter_list[19] = 0x62;
       parameter_list[20] = 0xFF;
       parameter_list[21] = 0x0A;
       parameter_list[22] = 0x4E;
       parameter_list[23] = 0xD1;
       parameter_list[24] = 0x16;
       parameter_list[25] = 0x19;
       parameter_list[26] = 0x98;
       parameter_list[27] = 0xD6;
       parameter_list[28] = 0x0E;
       parameter_list[29] = 0x11; 
	send_spi_command(0xE0, 30, parameter_list);
	mdelay(5);

	send_spi_command(0x29, 0, parameter_list);
	//mdelay(10);
}
#endif
#endif // 이부분을 #elif...EF30S feature 처리 할 것... lcdc_sky_panel_on 참고..


static void spi_init(void)
{
	ENTER_FUNC2();
	/* Set the output so that we dont disturb the slave device */    
	gpio_set_value(SPI_SCLK, GPIO_LOW_VALUE);
       gpio_set_value(SPI_SDI, GPIO_LOW_VALUE);
        
	/* Set the Chip Select De-asserted */
	gpio_set_value(SPI_CS, GPIO_HIGH_VALUE);
	EXIT_FUNC2();
}

#if defined(CONFIG_MACH_MSM8X55_EF30S) || defined(CONFIG_MACH_MSM8X55_EF18K) 
#if (BOARD_VER == WS20)
bool flag_sky_panel_off = FALSE;
static int lcdc_sky_panel_on(struct platform_device *pdev)
{
	ENTER_FUNC2();
#ifdef SKY_MDP_VG_CLEAR_HOLD_CHANGE    
       force_mdp4_overlay_unset(); // force_overlay_view_clear 
#endif

#ifdef SKY_FAST_LCD_ON // 20101112, kkcho, for using the LCD_SLEEP_MODE            
       send_spi_command(0x11, 0, parameter_list); // sleep-out
       mdelay(5); 
       //send_spi_command(0x29, 0, parameter_list); // lcd_on
#else       
	if (flag_sky_panel_off)
	  lcd_setup();
#endif

#ifdef SKY_MDP_VG_CLEAR_HOLD_CHANGE    
	force_mdp4_overlay_control(0); //release screen_hold
#endif
       flag_sky_panel_off = FALSE;
	EXIT_FUNC2();
	return 0;
}
#endif
#endif

#if defined(CONFIG_MACH_MSM8X55_EF30S) || defined(CONFIG_MACH_MSM8X55_EF18K) 
#if (BOARD_VER == WS20)
static int lcdc_sky_panel_off(struct platform_device *pdev)
{
        ENTER_FUNC2();  
#ifdef SKY_MDP_VG_CLEAR_HOLD_CHANGE    
        force_mdp4_overlay_control(1); // screen_hold
        force_mdp4_overlay_unset(); // force_overlay_view_clear 
#endif        

#ifdef SKY_FAST_LCD_ON // 20101112, kkcho, for using the LCD_SLEEP_MODE      
        //send_spi_command(0x28, 0, parameter_list); 
        //mdelay(5);
        send_spi_command(0x10, 0, parameter_list); // sleep-in
        mdelay(20); //wait for several frame :: guide
#else
        gpio_set_value(LCD_RESET, GPIO_LOW_VALUE);
#endif

        flag_sky_panel_off = TRUE;  
        EXIT_FUNC2();
        return 0;
}
#endif
#endif // 이부분을 #elif...EF30S feature 처리 할 것... lcdc_sky_panel_on 참고..



#ifdef SKY_REMOVE_NOISE_MDP_INIT
int sky_lcd_screen_off(void)
{
    ENTER_FUNC2();
    send_spi_command(0x10, 0, parameter_list);
    mdelay(20); //wait for several frame :: guide
    flag_sky_panel_off = TRUE; 
    EXIT_FUNC2();
    return 0;
}
#endif

#if defined(CONFIG_MACH_MSM8X55_EF30S) || defined(CONFIG_MACH_MSM8X55_EF18K) 
#if (BOARD_VER == WS20)
static void lcdc_sky_set_backlight(struct msm_fb_data_type *mfd)
{
#ifdef FEATURE_SKY_BACLIGHT_SC620
	boolean ret = TRUE;
       int dimming_enable_counter;
       int dimming_control_bit;

ENTER_FUNC2();   

	if (mfd->bl_level ==0) 
	{
		backlight_off();
		flag_lcd_bl_ctrl = TRUE;
	}
       else
       {
		if (flag_lcd_bl_off)
		{
			gpio_set_value(LCD_BL_EN ,GPIO_LOW_VALUE);
			mdelay(5);
			gpio_set_value(LCD_BL_EN ,GPIO_HIGH_VALUE);  
			mdelay(5);           
			flag_lcd_bl_off = FALSE;
		}       

		if (flag_lcd_bl_ctrl)
		{
			ret = led_i2c_master_send(SC620_LED_ONOFF_CTL_REG, 0xFF); 
			if (ret == 0)   printk(KERN_ERR "[Error : led_i2c_master_send]l!!!\n");
			flag_lcd_bl_ctrl = FALSE;
		}

              if (mfd->bl_level <=2) 
                     dimming_control_bit = 0x06; //6 decimal
              else if (mfd->bl_level >=14)
                     dimming_control_bit = 0x2A;  // 42 decimal              
              else
                     dimming_control_bit = 2+3*mfd->bl_level;  // low more mfd->bl_level+34
              
              /*--20101211, kkcho,  back-light level set!-----------------------------------
              step    dimming_level   gain_level(uA)      current(dimming_level*gain_level) 
                1                 6                  31.25                  0.1875 mA
                2                 6                  62.5                    0.375 mA
                3                 11                93.75                   1.03 mA
                4                 14               125                       1.75 mA
                5                 17               156.25                  2.65 mA
                6                 20               187.5                    3.75 mA
                7                 23               218.75                  5 mA
                8                 26               250                       6.5 mA
                9                 29               281.25                   8.15 mA
                10               32               312.5                    10 mA       =========> default!
                11               35               343.75                  12 mA
                12               38               375                       14.2 mA
                13               41               406.25                   16.7 mA
                14               42               437.5                     18.3 mA
                15               42               468.75                   19.6 mA
                16               42               500                       21mA       ==========> max!
              --------------------------------------------------------------------*/        
              //printk(KERN_ERR "[SKY_LCD] -%s:%d\n", __FUNCTION__, (mfd->bl_level-1));
              //printk(KERN_ERR "[SKY_LCD] -%s:dimming_levle : %d\n", __FUNCTION__, dimming_control_bit);
              for (dimming_enable_counter = 1; dimming_enable_counter < 8; dimming_enable_counter++)
              {
                     ret = led_i2c_master_send(dimming_enable_counter, dimming_control_bit);  
                     if (ret == 0)   printk(KERN_ERR "[Error : led_i2c_master_send]l!!!\n");                             
              }       

              ret = led_i2c_master_send(SC620_GAIN_REGISTER, (mfd->bl_level-1));
              if (ret == 0)   printk(KERN_ERR "[Error : led_i2c_master_send]l!!!\n");
		//ret = led_i2c_master_send(SC620_GAIN_REGISTER, (((mfd->bl_level)<<1)+1));    
       }
	   
	EXIT_FUNC2();
#endif  
}
#endif
#endif

//static int __init lcdc_sky_probe(struct platform_device *pdev)
static int  lcdc_sky_probe(struct platform_device *pdev)
{
	ENTER_FUNC2();
	if (pdev->id == 0) {
		lcdc_sky_pdata = pdev->dev.platform_data;
		return 0;
	}
	msm_fb_add_device(pdev);
	EXIT_FUNC2();
	return 0;
}

static struct platform_driver this_driver = {
	.probe  = lcdc_sky_probe,
	.driver = {
		.name   = "lcdc_sky_lcd",
	},
};

static struct msm_fb_panel_data lcdc_sky_panel_data = {
	.on = lcdc_sky_panel_on,
	.off = lcdc_sky_panel_off,
	.set_backlight = lcdc_sky_set_backlight,
};

static struct platform_device this_device = {
	.name   = "lcdc_sky_lcd",
	.id	= 1,
	.dev	= {
		.platform_data = &lcdc_sky_panel_data,
	}
};

static int __init lcdc_sky_init(void)
{
	int ret;
	struct msm_panel_info *pinfo;

	ENTER_FUNC2();

       lcd_gpio_init(lcd_gpio_init_table, ARRAY_SIZE(lcd_gpio_init_table), 1);

       // SPI Output Init.
       spi_init();
       
       gpio_set_value(LCD_RESET, GPIO_HIGH_VALUE);
       //gpio_set_value(LCD_BL_EN, GPIO_LOW_VALUE);
#ifdef FEATURE_SKY_BACLIGHT_SC620    
       led_i2c_api_Init();
#endif

	ret = platform_driver_register(&this_driver);
	if (ret)
		return ret;

	pinfo = &lcdc_sky_panel_data.panel_info;
	pinfo->xres = 480;
	pinfo->yres = 800;
	pinfo->type = LCDC_PANEL;
	pinfo->pdest = DISPLAY_1;
	pinfo->bl_max = 16;
	pinfo->bl_min = 1;
	pinfo->wait_cycle = 0;
	pinfo->bpp = 24;
	pinfo->fb_num = 2;
	pinfo->clk_rate = 24500000;
	pinfo->lcdc.h_back_porch =10;
	pinfo->lcdc.h_front_porch = 8;
	pinfo->lcdc.h_pulse_width = 10;
	pinfo->lcdc.v_back_porch = 2;
	pinfo->lcdc.v_front_porch = 2;
	pinfo->lcdc.v_pulse_width = 2;
	pinfo->lcdc.border_clr = 0;	/* blk */
	pinfo->lcdc.underflow_clr = 0x00; /* black */
	pinfo->lcdc.hsync_skew = 0;

	ret = platform_device_register(&this_device);
	if (ret)
		platform_driver_unregister(&this_driver);

	// Initial Seq.
	//lcd_setup();

	EXIT_FUNC2();
	return ret;
}

#if (defined(CONFIG_MACH_MSM8X55_EF30S) || defined(CONFIG_MACH_MSM8X55_EF18K))  && defined(CONFIG_PANTECH_ERROR_LOG) 
void force_lcd_screen_on(void)
{
    ENTER_FUNC2(); 
    //lcd_gpio_init(lcd_gpio_init_table, ARRAY_SIZE(lcd_gpio_init_table), 1);
    //spi_init();
    gpio_set_value(LCD_RESET, GPIO_HIGH_VALUE);
    //led_i2c_api_Init();
    lcd_setup();
    backlight_on();
    EXIT_FUNC2();
}
EXPORT_SYMBOL(force_lcd_screen_on);
#endif
module_init(lcdc_sky_init);
