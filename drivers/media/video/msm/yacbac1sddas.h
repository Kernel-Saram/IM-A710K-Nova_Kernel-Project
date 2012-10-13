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

#ifndef YACBAC1SDDAS_H
#define YACBAC1SDDAS_H

#include<linux/types.h>
#include<mach/camera.h>

#define BRIGHTNESS_STEP_8

extern struct yacbac1sddas_reg yacbac1sddas_regs;

enum yacbac1sddas_width
{
	WORD_LEN,
	TRIPLE_LEN,
	BYTE_LEN,
	ZERO_LEN
};

struct yacbac1sddas_i2c_reg_conf
{
	unsigned short waddr;
	unsigned short wdata;
	enum yacbac1sddas_width width;
	unsigned short mdelay_time;
};

#define YACBAC1SDDAS_CFG_BRIGHT_MAX_PARAMS 		2
#define YACBAC1SDDAS_CFG_WB_MAX_PARAMS 			10
#define YACBAC1SDDAS_CFG_EFFECT_MAX_PARAMS 		11
#define YACBAC1SDDAS_CFG_EXPOSURE_MAX_PARAMS 		3
#define YACBAC1SDDAS_CFG_PREVIEW_FPS_MAX_PARAMS 	14
#define YACBAC1SDDAS_CFG_FLICKER_MAX_PARAMS 		5
#define YACBAC1SDDAS_CFG_REFLECT_MAX_PARAMS 		2

struct yacbac1sddas_reg
{
	/* Init Paramter */
	const struct yacbac1sddas_i2c_reg_conf *init_parm;
	uint16_t init_parm_size;

//preview frame rate
	const struct yacbac1sddas_i2c_reg_conf (*preview_fps_cfg_settings)[YACBAC1SDDAS_CFG_PREVIEW_FPS_MAX_PARAMS];
	uint16_t preview_fps_cfg_settings_size;
	
//effect
	const struct yacbac1sddas_i2c_reg_conf (*effect_cfg_settings)[YACBAC1SDDAS_CFG_EFFECT_MAX_PARAMS];
	uint16_t effect_cfg_settings_size;

//wb	
	const struct yacbac1sddas_i2c_reg_conf (*wb_cfg_settings)[YACBAC1SDDAS_CFG_WB_MAX_PARAMS];
	uint16_t wb_cfg_settings_size;

//brightness	
	const struct yacbac1sddas_i2c_reg_conf (*bright_cfg_settings)[YACBAC1SDDAS_CFG_BRIGHT_MAX_PARAMS];
	uint16_t bright_cfg_settings_size;
	
//exposure	
	const struct yacbac1sddas_i2c_reg_conf (*exposure_cfg_settings)[YACBAC1SDDAS_CFG_EXPOSURE_MAX_PARAMS];
	uint16_t exposure_cfg_settings_size;

//flicker
	const struct yacbac1sddas_i2c_reg_conf (*flicker_cfg_settings)[YACBAC1SDDAS_CFG_FLICKER_MAX_PARAMS];
	uint16_t flicker_cfg_settings_size;	

	//flicker
	const struct yacbac1sddas_i2c_reg_conf (*reflect_cfg_settings)[YACBAC1SDDAS_CFG_REFLECT_MAX_PARAMS];
	uint16_t reflect_cfg_settings_size;
};


#endif /* YACBAC1SDDAS_H */
 
