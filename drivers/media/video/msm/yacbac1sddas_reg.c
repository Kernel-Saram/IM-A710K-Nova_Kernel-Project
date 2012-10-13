/* Copyright (c) 2009, PANTECH. All rights reserved.
 */

#include "yacbac1sddas.h"
//#include <linux/kernel.h>

/* register configration */
/* VGA tunning values 20110330 */
static const struct yacbac1sddas_i2c_reg_conf yacbac1sddas_init_parm[] =
{
	{0x03, 0x00, BYTE_LEN, 0},
#if 0		
	{0x01, 0x81, BYTE_LEN, 0},
	{0x01, 0x83, BYTE_LEN, 0},
	{0x01, 0x81, BYTE_LEN, 0},
#elif 1
	{0x01, 0xF1, BYTE_LEN, 0},
	{0x01, 0xF3, BYTE_LEN, 0},
	{0x01, 0xF1, BYTE_LEN, 0},	
#else
	{0x01, 0xE1, BYTE_LEN, 0},
	{0x01, 0xE3, BYTE_LEN, 0},
	{0x01, 0xE1, BYTE_LEN, 0},	
#endif	
	{0x03, 0x20, BYTE_LEN, 0},
	{0x10, 0x0C, BYTE_LEN, 0},
	{0x03, 0x22, BYTE_LEN, 0},
	{0x10, 0x6B, BYTE_LEN, 0},
	
	{0x03, 0x00, BYTE_LEN, 0}, //page 00
	{0x10, 0x08, BYTE_LEN, 0},
	{0x11, 0x90, BYTE_LEN, 0},
	{0x12, 0x04, BYTE_LEN, 0},
	{0x20, 0x00, BYTE_LEN, 0},
	{0x21, 0x06, BYTE_LEN, 0},
	{0x22, 0x00, BYTE_LEN, 0},
	{0x23, 0x06, BYTE_LEN, 0},
	{0x24, 0x01, BYTE_LEN, 0},
	{0x25, 0xE0, BYTE_LEN, 0},
	{0x26, 0x02, BYTE_LEN, 0},
	{0x27, 0x80, BYTE_LEN, 0},
	
	{0x40, 0x01, BYTE_LEN, 0},
	{0x41, 0x50, BYTE_LEN, 0},
	{0x42, 0x00, BYTE_LEN, 0},
	{0x43, 0x14, BYTE_LEN, 0},
	
	{0x46, 0x0F, BYTE_LEN, 0},
	{0x47, 0xFA, BYTE_LEN, 0},
	
	{0x80, 0x3E, BYTE_LEN, 0},
	{0x81, 0x96, BYTE_LEN, 0},
	{0x82, 0x90, BYTE_LEN, 0},
	{0x83, 0x00, BYTE_LEN, 0},
	{0x84, 0x2C, BYTE_LEN, 0},
	               
	{0x90, 0x14, BYTE_LEN, 0}, //07 //blc
	{0x91, 0x14, BYTE_LEN, 0}, //08 
	{0x92, 0x58, BYTE_LEN, 0}, //60 
	{0x93, 0x50, BYTE_LEN, 0}, //10 
	{0x94, 0x9F, BYTE_LEN, 0},
	{0x95, 0x80, BYTE_LEN, 0},
	
	{0x98, 0x20, BYTE_LEN, 0},
	{0xA0, 0x01, BYTE_LEN, 0},
	{0xA2, 0x00, BYTE_LEN, 0},
	{0xA4, 0x00, BYTE_LEN, 0},
	{0xA6, 0x00, BYTE_LEN, 0},
	{0xA8, 0x00, BYTE_LEN, 0},
	{0xAA, 0x00, BYTE_LEN, 0},
	{0xAC, 0x00, BYTE_LEN, 0},
	{0xAE, 0x00, BYTE_LEN, 0},
	
	{0x03, 0x02, BYTE_LEN, 0},
	{0x10, 0x00, BYTE_LEN, 0},
	{0x13, 0x00, BYTE_LEN, 0},
	{0x18, 0x1C, BYTE_LEN, 0},
	{0x19, 0x00, BYTE_LEN, 0},
	{0x1A, 0x00, BYTE_LEN, 0},
	{0x1B, 0x08, BYTE_LEN, 0},
	{0x1C, 0x00, BYTE_LEN, 0},
	{0x1D, 0x00, BYTE_LEN, 0},
	{0x20, 0x33, BYTE_LEN, 0},
	{0x21, 0xAA, BYTE_LEN, 0},
	{0x22, 0xA6, BYTE_LEN, 0},
	{0x23, 0xB0, BYTE_LEN, 0},
	{0x31, 0x99, BYTE_LEN, 0},
	{0x32, 0x00, BYTE_LEN, 0},
	{0x33, 0x00, BYTE_LEN, 0},
	{0x34, 0x3C, BYTE_LEN, 0},
	{0x50, 0x21, BYTE_LEN, 0},
	{0x54, 0x30, BYTE_LEN, 0},
	{0x56, 0xFE, BYTE_LEN, 0},
	{0x62, 0x78, BYTE_LEN, 0},
	{0x63, 0x9E, BYTE_LEN, 0},
	{0x64, 0x78, BYTE_LEN, 0},
	{0x65, 0x9E, BYTE_LEN, 0},
	{0x72, 0x7A, BYTE_LEN, 0},
	{0x73, 0x9A, BYTE_LEN, 0},
	{0x74, 0x7A, BYTE_LEN, 0},
	{0x75, 0x9A, BYTE_LEN, 0},
	{0x82, 0x09, BYTE_LEN, 0},
	{0x84, 0x09, BYTE_LEN, 0},
	{0x86, 0x09, BYTE_LEN, 0},
	{0xA0, 0x03, BYTE_LEN, 0},
	{0xA8, 0x1D, BYTE_LEN, 0},
	{0xAA, 0x49, BYTE_LEN, 0},
	{0xB9, 0x8A, BYTE_LEN, 0},
	{0xBB, 0x8A, BYTE_LEN, 0},
	{0xBC, 0x04, BYTE_LEN, 0},
	{0xBD, 0x10, BYTE_LEN, 0},
	{0xBE, 0x04, BYTE_LEN, 0},
	{0xBF, 0x10, BYTE_LEN, 0},
	
	{0x03, 0x10, BYTE_LEN, 0}, //page 10
	{0x10, 0x01, BYTE_LEN, 0},
	{0x12, 0x30, BYTE_LEN, 0},
	{0x40, 0x00, BYTE_LEN, 0},
	{0x41, 0x80, BYTE_LEN, 0},
	{0x50, 0xA0, BYTE_LEN, 0},
	
	{0x60, 0x1F, BYTE_LEN, 0},
	{0x61, 0x7A, BYTE_LEN, 0},
	{0x62, 0x72, BYTE_LEN, 0},
	{0x63, 0xE0, BYTE_LEN, 0},
	{0x64, 0xa8, BYTE_LEN, 0},
	               
	{0x03, 0x11, BYTE_LEN, 0},
	{0x10, 0x19, BYTE_LEN, 0},
	{0x11, 0x0E, BYTE_LEN, 0},
	{0x21, 0x18, BYTE_LEN, 0},
	{0x50, 0x03, BYTE_LEN, 0},
	{0x60, 0x06, BYTE_LEN, 0},
	{0x62, 0x43, BYTE_LEN, 0},
	{0x63, 0x63, BYTE_LEN, 0},
	{0x74, 0x0D, BYTE_LEN, 0},
	{0x75, 0x0D, BYTE_LEN, 0},
	               
	{0x03, 0x12, BYTE_LEN, 0},
	{0x40, 0x23, BYTE_LEN, 0},
	{0x41, 0x37, BYTE_LEN, 0},
	{0x50, 0x01, BYTE_LEN, 0},
	{0x70, 0x1d, BYTE_LEN, 0},
	{0x74, 0x05, BYTE_LEN, 0},
	{0x75, 0x04, BYTE_LEN, 0},
	{0x91, 0x34, BYTE_LEN, 0},
	{0xB0, 0xC9, BYTE_LEN, 0},
	{0xD0, 0xB1, BYTE_LEN, 0},
	               
	{0x03, 0x13, BYTE_LEN, 0}, //page 13
	{0x10, 0x33, BYTE_LEN, 0},
	{0x11, 0x01, BYTE_LEN, 0},
	{0x12, 0x00, BYTE_LEN, 0},
	{0x13, 0x02, BYTE_LEN, 0},
	{0x14, 0x10, BYTE_LEN, 0},
	{0x20, 0x02, BYTE_LEN, 0},
	{0x21, 0x02, BYTE_LEN, 0},
	{0x23, 0x2F, BYTE_LEN, 0},
	{0x24, 0x0F, BYTE_LEN, 0},
	{0x25, 0x40, BYTE_LEN, 0},
	{0x28, 0x00, BYTE_LEN, 0},
	{0x29, 0x78, BYTE_LEN, 0},
	{0x30, 0xFF, BYTE_LEN, 0},
	{0x80, 0x0B, BYTE_LEN, 0},
	{0x81, 0x11, BYTE_LEN, 0},
	{0x83, 0x5D, BYTE_LEN, 0},
	{0x90, 0x03, BYTE_LEN, 0},
	{0x91, 0x03, BYTE_LEN, 0},
	{0x93, 0x2F, BYTE_LEN, 0},
	{0x94, 0x0F, BYTE_LEN, 0},
	{0x95, 0x80, BYTE_LEN, 0},
	               
	{0x03, 0x14, BYTE_LEN, 0},
	{0x10, 0x01, BYTE_LEN, 0},
	{0x20, 0x80, BYTE_LEN, 0},
	{0x21, 0x80, BYTE_LEN, 0},
	
	{0x22, 0x60, BYTE_LEN, 0},
	{0x23, 0x55, BYTE_LEN, 0},
	{0x24, 0x53, BYTE_LEN, 0},
	
	{0x03, 0x15, BYTE_LEN, 0},
	{0x10, 0x03, BYTE_LEN, 0},
	
	{0x14, 0x50, BYTE_LEN, 0},
	{0x16, 0x40, BYTE_LEN, 0},
	{0x17, 0x2f, BYTE_LEN, 0},
	               
	{0x30, 0xcc, BYTE_LEN, 0},
	{0x31, 0x61, BYTE_LEN, 0},
	{0x32, 0x15, BYTE_LEN, 0},
	{0x33, 0x22, BYTE_LEN, 0},
	{0x34, 0xcd, BYTE_LEN, 0},
	{0x35, 0x2b, BYTE_LEN, 0},
	{0x36, 0x01, BYTE_LEN, 0},
	{0x37, 0x33, BYTE_LEN, 0},
	{0x38, 0x74, BYTE_LEN, 0},
	
	{0x40, 0x00, BYTE_LEN, 0},
	{0x41, 0x00, BYTE_LEN, 0},
	{0x42, 0x00, BYTE_LEN, 0},
	{0x43, 0x96, BYTE_LEN, 0},
	{0x44, 0x00, BYTE_LEN, 0},
	{0x45, 0x16, BYTE_LEN, 0},
	{0x46, 0x86, BYTE_LEN, 0},
	{0x47, 0x06, BYTE_LEN, 0},
	{0x48, 0x00, BYTE_LEN, 0},
	
	{0x03, 0x16, BYTE_LEN, 0},
	{0x10, 0x01, BYTE_LEN, 0},
	{0x30, 0x00, BYTE_LEN, 0},
	{0x31, 0x0D, BYTE_LEN, 0},
	{0x32, 0x18, BYTE_LEN, 0},
	{0x33, 0x2C, BYTE_LEN, 0},
	{0x34, 0x57, BYTE_LEN, 0},
	{0x35, 0x7A, BYTE_LEN, 0},
	{0x36, 0x93, BYTE_LEN, 0},
	{0x37, 0xA7, BYTE_LEN, 0},
	{0x38, 0xB8, BYTE_LEN, 0},
	{0x39, 0xC6, BYTE_LEN, 0},
	{0x3A, 0xD2, BYTE_LEN, 0},
	{0x3B, 0xE4, BYTE_LEN, 0},
	{0x3C, 0xF1, BYTE_LEN, 0},
	{0x3D, 0xF9, BYTE_LEN, 0},
	{0x3E, 0xFF, BYTE_LEN, 0},
	
	{0x03, 0x17, BYTE_LEN, 0},
	{0xC4, 0x3C, BYTE_LEN, 0},
	{0xC5, 0x32, BYTE_LEN, 0},
	
	{0x03, 0x20, BYTE_LEN, 0}, //page 20
	{0x10, 0x0C, BYTE_LEN, 0},
	{0x11, 0x00, BYTE_LEN, 0},
	               
	{0x20, 0x01, BYTE_LEN, 0},
	{0x28, 0x3F, BYTE_LEN, 0},
	{0x29, 0xA3, BYTE_LEN, 0},
	{0x2A, 0xF0, BYTE_LEN, 0}, //variable fixed 03
	{0x2B, 0x34, BYTE_LEN, 0}, //variable fixed 35
	               
	{0x30, 0x78, BYTE_LEN, 0},
	{0x60, 0x95, BYTE_LEN, 0},
	{0x70, 0x40, BYTE_LEN, 0},
	               
	{0x78, 0x23, BYTE_LEN, 0},
	{0x79, 0x20, BYTE_LEN, 0},
	{0x7A, 0x24, BYTE_LEN, 0},

	{0x83, 0x00, BYTE_LEN, 0},
	{0x84, 0xC3, BYTE_LEN, 0},
	{0x85, 0x50, BYTE_LEN, 0},

	{0x86, 0x00, BYTE_LEN, 0},
	{0x87, 0xFA, BYTE_LEN, 0},
	               
	{0x88, 0x04, BYTE_LEN, 0},
	{0x89, 0x93, BYTE_LEN, 0},
	{0x8A, 0xE0, BYTE_LEN, 0},
	
	{0x8B, 0x3A, BYTE_LEN, 0},
	{0x8C, 0x98, BYTE_LEN, 0},
	
	{0x8D, 0x30, BYTE_LEN, 0},
	{0x8E, 0xD4, BYTE_LEN, 0},
	
	{0x8F, 0xC4, BYTE_LEN, 0},
	{0x90, 0x68, BYTE_LEN, 0},
	
	{0x91, 0x02, BYTE_LEN, 0},
	{0x92, 0xD2, BYTE_LEN, 0},
	{0x93, 0xA8, BYTE_LEN, 0},
	
	{0x98, 0x8C, BYTE_LEN, 0},
	{0x99, 0x23, BYTE_LEN, 0},
	
	{0x9C, 0x06, BYTE_LEN, 0},
	{0x9D, 0xD6, BYTE_LEN, 0},
	{0x9E, 0x00, BYTE_LEN, 0},
	{0x9F, 0xFA, BYTE_LEN, 0},
	
	{0xB0, 0x18, BYTE_LEN, 0},
	{0xB1, 0x14, BYTE_LEN, 0},
	{0xB2, 0xE8, BYTE_LEN, 0},
	{0xB3, 0x18, BYTE_LEN, 0},
	
	{0xB4, 0x18, BYTE_LEN, 0},
	{0xB5, 0x40, BYTE_LEN, 0},
	{0xB6, 0x2C, BYTE_LEN, 0},
	{0xB7, 0x25, BYTE_LEN, 0},
	{0xB8, 0x22, BYTE_LEN, 0},
	{0xB9, 0x20, BYTE_LEN, 0},
	{0xBA, 0x1F, BYTE_LEN, 0},
	{0xBB, 0x1E, BYTE_LEN, 0},
	{0xBC, 0x1D, BYTE_LEN, 0},
	{0xBD, 0x1C, BYTE_LEN, 0},
	
	{0xC0, 0x0C, BYTE_LEN, 0},
	
	{0xc3, 0xA0, BYTE_LEN, 0},
	{0xc4, 0x98, BYTE_LEN, 0},
	
	{0xc8, 0xa0, BYTE_LEN, 0},
	{0xc9, 0x98, BYTE_LEN, 0},
	               
	{0x03, 0x22, BYTE_LEN, 0}, //AWB   by Leemaster 2011.03.30//page 22
	{0x10, 0xE2, BYTE_LEN, 0}, //FA->F2->E2
	{0x11, 0x26, BYTE_LEN, 0}, //2E->26  Adaptive CMC OFF
	{0x21, 0x44, BYTE_LEN, 0},
	
	{0x30, 0x80, BYTE_LEN, 0},
	{0x31, 0x80, BYTE_LEN, 0},
	{0x38, 0x11, BYTE_LEN, 0}, 
	{0x39, 0x66, BYTE_LEN, 0}, 
	{0x40, 0xF3, BYTE_LEN, 0}, 
	{0x41, 0x44, BYTE_LEN, 0}, 
	{0x42, 0x33, BYTE_LEN, 0}, 
	{0x43, 0xF0, BYTE_LEN, 0},
	{0x44, 0xAA, BYTE_LEN, 0},
	{0x45, 0x66, BYTE_LEN, 0},
	{0x46, 0x00, BYTE_LEN, 0},
	           
	{0x80, 0x41, BYTE_LEN, 0},
	{0x81, 0x23, BYTE_LEN, 0},
	{0x82, 0x3C, BYTE_LEN, 0},
	           
	{0x83, 0x5C, BYTE_LEN, 0},
	{0x84, 0x22, BYTE_LEN, 0},
	{0x85, 0x50, BYTE_LEN, 0},
	{0x86, 0x22, BYTE_LEN, 0},
	           
	{0x87, 0x56, BYTE_LEN, 0},
	{0x88, 0x45, BYTE_LEN, 0},
	{0x89, 0x30, BYTE_LEN, 0},
	{0x8A, 0x28, BYTE_LEN, 0},
	           
	{0x8B, 0x05, BYTE_LEN, 0},
	{0x8D, 0x25, BYTE_LEN, 0},
	{0x8E, 0x31, BYTE_LEN, 0},
	           
	{0x8f, 0x4f, BYTE_LEN, 0},
	{0x90, 0x4f, BYTE_LEN, 0},
	{0x91, 0x4d, BYTE_LEN, 0},
	{0x92, 0x4a, BYTE_LEN, 0},
	{0x93, 0x45, BYTE_LEN, 0},
	{0x94, 0x40, BYTE_LEN, 0},
	{0x95, 0x3a, BYTE_LEN, 0},
	{0x96, 0x36, BYTE_LEN, 0},
	{0x97, 0x32, BYTE_LEN, 0},
	{0x98, 0x2d, BYTE_LEN, 0},
	{0x99, 0x29, BYTE_LEN, 0},
	{0x9a, 0x28, BYTE_LEN, 0},
	{0x9b, 0x08, BYTE_LEN, 0},
	           
	{0x03, 0x22, BYTE_LEN, 0},
	{0x10, 0xFA, BYTE_LEN, 0}, //e2
	           
	{0x03, 0x20, BYTE_LEN, 0},
	{0x10, 0x8c, BYTE_LEN, 0}, 
#if 0	
	{0x01, 0x80, BYTE_LEN, 0}
#elif 1
	{0x01, 0xF0, BYTE_LEN, 0}
#else
	{0x01, 0xE0, BYTE_LEN, 0}
#endif

	
};


static const struct yacbac1sddas_i2c_reg_conf yacbac1sddas_preview_fps_settings_cfg[][YACBAC1SDDAS_CFG_PREVIEW_FPS_MAX_PARAMS]={
       {
		//5fps(dummy)
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//6fps(dummy)
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//7fps
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x0C, BYTE_LEN, 0},
		{0x2a, 0x03, BYTE_LEN, 0},
		{0x2b, 0x35, BYTE_LEN, 0},
		{0x88, 0x03, BYTE_LEN, 0},
		{0x89, 0x0D, BYTE_LEN, 0},
		{0x8A, 0x40, BYTE_LEN, 0},
		{0x91, 0x03, BYTE_LEN, 0},
		{0x92, 0x3B, BYTE_LEN, 0},
		{0x93, 0x26, BYTE_LEN, 0},
		{0x03, 0x00, BYTE_LEN, 0},
		{0x11, 0x94, BYTE_LEN, 0},
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x8C, BYTE_LEN, 0}
	},
	{
		//8fps
		{0x03, 0x20, BYTE_LEN, 0},                
	  {0x10, 0x0C, BYTE_LEN, 0},
	  {0x2a, 0x03, BYTE_LEN, 0},
	  {0x2b, 0x35, BYTE_LEN, 0},
	  {0x88, 0x02, BYTE_LEN, 0},
	  {0x89, 0xAB, BYTE_LEN, 0},
	  {0x8A, 0x98, BYTE_LEN, 0},
	  {0x91, 0x02, BYTE_LEN, 0},
	  {0x92, 0xD2, BYTE_LEN, 0},
	  {0x93, 0xA8, BYTE_LEN, 0},
	  {0x03, 0x00, BYTE_LEN, 0},
	  {0x11, 0x94, BYTE_LEN, 0},
	  {0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x8C, BYTE_LEN, 0}
	},
	{
		//9fps(dummy)
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//10fps
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x0C, BYTE_LEN, 0},
		{0x2a, 0x03, BYTE_LEN, 0},
		{0x2b, 0x35, BYTE_LEN, 0},
		{0x88, 0x02, BYTE_LEN, 0},
		{0x89, 0x19, BYTE_LEN, 0},
		{0x8A, 0x1C, BYTE_LEN, 0},
		{0x91, 0x02, BYTE_LEN, 0},
		{0x92, 0x40, BYTE_LEN, 0},
		{0x93, 0x2C, BYTE_LEN, 0},
		{0x03, 0x00, BYTE_LEN, 0},
		{0x11, 0x94, BYTE_LEN, 0},
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x8C, BYTE_LEN, 0}
	},
	{
		//11fps
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x0C, BYTE_LEN, 0},
		{0x2a, 0x03, BYTE_LEN, 0},
		{0x2b, 0x35, BYTE_LEN, 0},
		{0x88, 0x01, BYTE_LEN, 0},
		{0x89, 0xe8, BYTE_LEN, 0},
		{0x8A, 0x48, BYTE_LEN, 0},
		{0x91, 0x02, BYTE_LEN, 0},
		{0x92, 0x0a, BYTE_LEN, 0},
		{0x93, 0x76, BYTE_LEN, 0},
		{0x03, 0x00, BYTE_LEN, 0},
		{0x11, 0x94, BYTE_LEN, 0},
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x8C, BYTE_LEN, 0}
	},
	{
		//12fps
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x0C, BYTE_LEN, 0},
		{0x2a, 0x03, BYTE_LEN, 0},
		{0x2b, 0x35, BYTE_LEN, 0},
		{0x88, 0x01, BYTE_LEN, 0},
		{0x89, 0xb7, BYTE_LEN, 0},
		{0x8A, 0x74, BYTE_LEN, 0},
		{0x91, 0x01, BYTE_LEN, 0},
		{0x92, 0xde, BYTE_LEN, 0},
		{0x93, 0x84, BYTE_LEN, 0},
		{0x03, 0x00, BYTE_LEN, 0},
		{0x11, 0x94, BYTE_LEN, 0},
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x8C, BYTE_LEN, 0}

	},
	{
		//13fps
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x0C, BYTE_LEN, 0},
		{0x2a, 0x03, BYTE_LEN, 0},
		{0x2b, 0x35, BYTE_LEN, 0},
		{0x88, 0x01, BYTE_LEN, 0},
		{0x89, 0x86, BYTE_LEN, 0},
		{0x8A, 0xa0, BYTE_LEN, 0},
		{0x91, 0x01, BYTE_LEN, 0},
		{0x92, 0xb8, BYTE_LEN, 0},
		{0x93, 0xeb, BYTE_LEN, 0},
		{0x03, 0x00, BYTE_LEN, 0},
		{0x11, 0x94, BYTE_LEN, 0},
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x8C, BYTE_LEN, 0}
	},
	{
		//14fps
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x0C, BYTE_LEN, 0},
		{0x2a, 0x03, BYTE_LEN, 0},
		{0x2b, 0x35, BYTE_LEN, 0},
		{0x88, 0x01, BYTE_LEN, 0},
		{0x89, 0x86, BYTE_LEN, 0},
		{0x8A, 0xa0, BYTE_LEN, 0},
		{0x91, 0x01, BYTE_LEN, 0},
		{0x92, 0x98, BYTE_LEN, 0},
		{0x93, 0xb1, BYTE_LEN, 0},
		{0x03, 0x00, BYTE_LEN, 0},
		{0x11, 0x94, BYTE_LEN, 0},
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x8C, BYTE_LEN, 0}

	},
{
		//15fps
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x0C, BYTE_LEN, 0},
		{0x2a, 0x03, BYTE_LEN, 0},
		{0x2b, 0x35, BYTE_LEN, 0},
		{0x88, 0x01, BYTE_LEN, 0},
		{0x89, 0x55, BYTE_LEN, 0},
		{0x8A, 0xcc, BYTE_LEN, 0},
		{0x91, 0x01, BYTE_LEN, 0},
		{0x92, 0x7c, BYTE_LEN, 0},
		{0x93, 0xdc, BYTE_LEN, 0},
		{0x03, 0x00, BYTE_LEN, 0},
		{0x11, 0x94, BYTE_LEN, 0},
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x8C, BYTE_LEN, 0}

	},
	{
		//16fps
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x0C, BYTE_LEN, 0},
		{0x2a, 0x03, BYTE_LEN, 0},
		{0x2b, 0x35, BYTE_LEN, 0},
		{0x88, 0x01, BYTE_LEN, 0},
		{0x89, 0x55, BYTE_LEN, 0},
		{0x8A, 0xcc, BYTE_LEN, 0},
		{0x91, 0x01, BYTE_LEN, 0},
		{0x92, 0x64, BYTE_LEN, 0},
		{0x93, 0x72, BYTE_LEN, 0},
		{0x03, 0x00, BYTE_LEN, 0},
		{0x11, 0x94, BYTE_LEN, 0},
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x8C, BYTE_LEN, 0}

	},
{
		//17fps
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x0C, BYTE_LEN, 0},
		{0x2a, 0x03, BYTE_LEN, 0},
		{0x2b, 0x35, BYTE_LEN, 0},
		{0x88, 0x01, BYTE_LEN, 0},
		{0x89, 0x24, BYTE_LEN, 0},
		{0x8A, 0xf8, BYTE_LEN, 0},
		{0x91, 0x01, BYTE_LEN, 0},
		{0x92, 0x4e, BYTE_LEN, 0},
		{0x93, 0x79, BYTE_LEN, 0},
		{0x03, 0x00, BYTE_LEN, 0},
		{0x11, 0x94, BYTE_LEN, 0},
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x8C, BYTE_LEN, 0}

	},
{
		//18fps
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x0C, BYTE_LEN, 0},
		{0x2a, 0x03, BYTE_LEN, 0},
		{0x2b, 0x35, BYTE_LEN, 0},
		{0x88, 0x01, BYTE_LEN, 0},
		{0x89, 0x24, BYTE_LEN, 0},
		{0x8A, 0xf8, BYTE_LEN, 0},
		{0x91, 0x01, BYTE_LEN, 0},
		{0x92, 0x3b, BYTE_LEN, 0},
		{0x93, 0x6e, BYTE_LEN, 0},
		{0x03, 0x00, BYTE_LEN, 0},
		{0x11, 0x94, BYTE_LEN, 0},
		{0x03, 0x20, BYTE_LEN, 0},
		{0x10, 0x8C, BYTE_LEN, 0}

	},
	{
		//19fps
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
{
		//20fps
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//21fps
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//22fps
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//23fps
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//24fps
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//25fps
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//26fps
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//27fps
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//28fps
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//29fps
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//30fps
		{0x03, 0x00, BYTE_LEN, 0},
		{0x11, 0x91, BYTE_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	}
};

//YACBAC1SDDAS EFFECT
//================================================================
static const struct yacbac1sddas_i2c_reg_conf const yacbac1sddas_effect_settings_cfg[][YACBAC1SDDAS_CFG_EFFECT_MAX_PARAMS] = {
	{
		//none
		{ 0x03, 0x10, BYTE_LEN, 0},
	       { 0x11, 0x03, BYTE_LEN, 0},
	       { 0x12, 0x30, BYTE_LEN, 0},
	       { 0x13, 0x00, BYTE_LEN, 0},
	       { 0x44, 0x80, BYTE_LEN, 0},
	       { 0x45, 0x80, BYTE_LEN, 0},
	       { 0x47, 0x7F, BYTE_LEN, 0},
	       { 0x03, 0x13, BYTE_LEN, 0},
	    { 0x20, 0x02, BYTE_LEN, 0},
	    { 0x21, 0x02, BYTE_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
{
		//mono
		{ 0x03, 0x10, BYTE_LEN, 0},
	       { 0x11, 0x03, BYTE_LEN, 0},
	       { 0x12, 0x33, BYTE_LEN, 0},
	       { 0x13, 0x00, BYTE_LEN, 0},
	       { 0x44, 0x80, BYTE_LEN, 0},
	       { 0x45, 0x80, BYTE_LEN, 0},
	       { 0x47, 0x7F, BYTE_LEN, 0},
	       { 0x03, 0x13, BYTE_LEN, 0},
	       { 0x20, 0x07, BYTE_LEN, 0},
	       { 0x21, 0x03, BYTE_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//negative
		{ 0x03, 0x10, BYTE_LEN, 0},
		{ 0x11, 0x03, BYTE_LEN, 0},
		{ 0x12, 0x38, BYTE_LEN, 0},
		{ 0x13, 0x00, BYTE_LEN, 0},
		{ 0x44, 0x80, BYTE_LEN, 0},
		{ 0x45, 0x80, BYTE_LEN, 0},
		{ 0x47, 0x7F, BYTE_LEN, 0},
		{ 0x03, 0x13, BYTE_LEN, 0},
		{ 0x20, 0x07, BYTE_LEN, 0},
		{ 0x21, 0x03, BYTE_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//sola
		{ 0x03, 0x10, BYTE_LEN, 0},
		{ 0x11, 0x03, BYTE_LEN, 0},
		{ 0x12, 0x38, BYTE_LEN, 0},
		{ 0x13, 0x00, BYTE_LEN, 0},
		{ 0x44, 0x80, BYTE_LEN, 0},
		{ 0x45, 0x80, BYTE_LEN, 0},
		{ 0x47, 0x7F, BYTE_LEN, 0},
		{ 0x03, 0x13, BYTE_LEN, 0},
		{ 0x20, 0x07, BYTE_LEN, 0},
		{ 0x21, 0x03, BYTE_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//sepia
		{ 0x03, 0x10, BYTE_LEN, 0},
	       { 0x11, 0x03, BYTE_LEN, 0},
	       { 0x12, 0x33, BYTE_LEN, 0},
	       { 0x13, 0x00, BYTE_LEN, 0},
	       { 0x40, 0x00, BYTE_LEN, 0},
	       { 0x44, 0x70, BYTE_LEN, 0},
	       { 0x45, 0x98, BYTE_LEN, 0},
	       { 0x47, 0x7F, BYTE_LEN, 0},
	       { 0x03, 0x13, BYTE_LEN, 0},
	       { 0x20, 0x07, BYTE_LEN, 0},
	       { 0x21, 0x03, BYTE_LEN, 0}
	}
};

//YACBAC1SDDAS WHITE BALANCE
//================================================================
static const struct yacbac1sddas_i2c_reg_conf const yacbac1sddas_wb_settings_cfg[][YACBAC1SDDAS_CFG_WB_MAX_PARAMS] = {
{
		//auto
		{0x03, 0x22, BYTE_LEN, 0},
		{0x10, 0x62, BYTE_LEN, 0},//B[7] Disable
		{0x80, 0x38, BYTE_LEN, 0},//Rgain                                      
		{0x81, 0x23, BYTE_LEN, 0},//Ggain                                      
		{0x82, 0x28, BYTE_LEN, 0},//Bgain                                      
		{0x83, 0x5C, BYTE_LEN, 0},//RMax                                       
		{0x84, 0x25, BYTE_LEN, 0},//RMin                                       
		{0x85, 0x52, BYTE_LEN, 0},//BMax                                       
		{0x86, 0x24, BYTE_LEN, 0},//BMin                                       
		{0x10, 0xFA, BYTE_LEN, 0}//B[7] Enable  
	},
	{
		//custom(auto)
		{ 0x03, 0x22, BYTE_LEN, 0},
		{ 0x10, 0x62, BYTE_LEN, 0},//B[7] Disable
		{ 0x80, 0x45, BYTE_LEN, 0},//Rgain
		{ 0x81, 0x20, BYTE_LEN, 0},//Ggain
		{ 0x82, 0x48, BYTE_LEN, 0},//Bgain
		{ 0x83, 0x66, BYTE_LEN, 0},//RMax
		{ 0x84, 0x22, BYTE_LEN, 0},//RMin
		{ 0x85, 0x58, BYTE_LEN, 0},//BMax
		{ 0x86, 0x22, BYTE_LEN, 0},//BMin
		{0x10, 0xFA, BYTE_LEN, 0}//B[7] Enable  
	},
	{
		//incandescent
		{ 0x03, 0x22, BYTE_LEN, 0},
		{0x10, 0x62, BYTE_LEN, 0},
		{0x80, 0x26, BYTE_LEN, 0},
		{0x81, 0x20, BYTE_LEN, 0},
		{0x82, 0x5A, BYTE_LEN, 0},
		{0x83, 0x28, BYTE_LEN, 0},
		{0x84, 0x24, BYTE_LEN, 0},
		{0x85, 0x5C, BYTE_LEN, 0},
		{0x86, 0x58, BYTE_LEN, 0},
		{0x10, 0xFA, BYTE_LEN, 0}
	},
	{
		//fluorescent
		{ 0x03, 0x22, BYTE_LEN, 0},
		{0x10, 0x62, BYTE_LEN, 0},
		{0x80, 0x40, BYTE_LEN, 0},
		{0x81, 0x20, BYTE_LEN, 0},
		{0x82, 0x49, BYTE_LEN, 0},
		{0x83, 0x42, BYTE_LEN, 0},
		{0x84, 0x3E, BYTE_LEN, 0},
		{0x85, 0x4B, BYTE_LEN, 0},
		{0x86, 0x47, BYTE_LEN, 0},
		{0x10, 0xFA, BYTE_LEN, 0}
	},
	{
		// daylight
		{ 0x03, 0x22, BYTE_LEN, 0},
		{0x10, 0x62, BYTE_LEN, 0},
		{0x80, 0x56, BYTE_LEN, 0},
		{0x81, 0x20, BYTE_LEN, 0},
		{0x82, 0x2B, BYTE_LEN, 0},
		{0x83, 0x58, BYTE_LEN, 0},
		{0x84, 0x54, BYTE_LEN, 0},
		{0x85, 0x2D, BYTE_LEN, 0},
		{0x86, 0x29, BYTE_LEN, 0},
		{0x10, 0xFA, BYTE_LEN, 0}
	},
	{
		//cloudy
		{ 0x03, 0x22, BYTE_LEN, 0},
		{0x10, 0x62, BYTE_LEN, 0},
		{0x80, 0x65, BYTE_LEN, 0},
		{0x81, 0x20, BYTE_LEN, 0},
		{0x82, 0x2B, BYTE_LEN, 0},
		{0x83, 0x68, BYTE_LEN, 0},
		{0x84, 0x62, BYTE_LEN, 0},
		{0x85, 0x2D, BYTE_LEN, 0},
		{0x86, 0x29, BYTE_LEN, 0},
		{0x10, 0xFA, BYTE_LEN, 0}
	}
};


//YACBAC1SDDAS BRIGHTNESS
//================================================================
static const struct yacbac1sddas_i2c_reg_conf const yacbac1sddas_bright_settings_cfg[][YACBAC1SDDAS_CFG_BRIGHT_MAX_PARAMS] = {
#ifndef BRIGHTNESS_STEP_8
	{
		//-5
		{0x03, 0x10, BYTE_LEN, 0},
		{0x40, 0xD5, BYTE_LEN, 0}, // AE_RULE_BASE_TARGET
	},
#endif
{
		//-4
		{0x03, 0x10, BYTE_LEN, 0},
		{0x40, 0xC5, BYTE_LEN, 0}, // AE_RULE_BASE_TARGET
	},
	{
		//-3
		{0x03, 0x10, BYTE_LEN, 0},
		{0x40, 0xB5, BYTE_LEN, 0}, // AE_RULE_BASE_TARGET
	},
	{
		//-2
		{0x03, 0x10, BYTE_LEN, 0},
		{0x40, 0xA5, BYTE_LEN, 0}, // AE_RULE_BASE_TARGET
	},
	{
		//-1
		{0x03, 0x10, BYTE_LEN, 0},
		{0x40, 0x95, BYTE_LEN, 0}, // AE_RULE_BASE_TARGET
	},

{
		//0
		{0x03, 0x10, BYTE_LEN, 0},
		{0x40, 0x00, BYTE_LEN, 0}, // AE_RULE_BASE_TARGET
	},

{
		// 1
		{0x03, 0x10, BYTE_LEN, 0},
		{0x40, 0x15, BYTE_LEN, 0}, // AE_RULE_BASE_TARGET
	},

{
		// 2
		{0x03, 0x10, BYTE_LEN, 0},
		{0x40, 0x25, BYTE_LEN, 0}, // AE_RULE_BASE_TARGET
	},

{
		// 3
		{0x03, 0x10, BYTE_LEN, 0},
		{0x40, 0x35, BYTE_LEN, 0}, // AE_RULE_BASE_TARGET
	},

{
		// 4
		{0x03, 0x10, BYTE_LEN, 0},
		{0x40, 0x45, BYTE_LEN, 0}, // AE_RULE_BASE_TARGET
	},
#ifndef BRIGHTNESS_STEP_8
	{
		// 5
		{0x03, 0x10, BYTE_LEN, 0},
		{0x40, 0x55, BYTE_LEN, 0}, // AE_RULE_BASE_TARGET
	}
#endif
};

//YACBAC1SDDAS EXPOSURE
//================================================================
static const struct yacbac1sddas_i2c_reg_conf const yacbac1sddas_exposure_settings_cfg[][YACBAC1SDDAS_CFG_EXPOSURE_MAX_PARAMS] = {

{
		//normal
		{ 0x03,0x20, BYTE_LEN, 0},
		{ 0x20,0x01, BYTE_LEN, 0},  //AE Weight ON
		{ 0x60,0x95, BYTE_LEN, 0}
	},
{
		//spot
		{ 0x03,0x20, BYTE_LEN, 0},
		{ 0x20,0x01, BYTE_LEN, 0},  //AE Weight ON
		{ 0x60,0xC0, BYTE_LEN, 0}
	},
{
		//average
		{ 0x03,0x20, BYTE_LEN, 0},
		{ 0x20,0x01, BYTE_LEN, 0},  //AE Weight ON
		{ 0x60,0x55, BYTE_LEN, 0}
	},
{
		//spot
		{ 0x03,0x20, BYTE_LEN, 0},
		{ 0x20,0x01, BYTE_LEN, 0},  //AE Weight ON
		{ 0x60,0xC0, BYTE_LEN, 0}
	}
};

static const struct yacbac1sddas_i2c_reg_conf const yacbac1sddas_flicker_settings_cfg[][YACBAC1SDDAS_CFG_FLICKER_MAX_PARAMS] = {
	{
		/* YACBAC1SDDAS_Auto_FD */
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		/* YACBAC1SDDAS_Manual_FD_60Hz */
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		/* YACBAC1SDDAS_Manual_FD_50Hz */
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		/* YACBAC1SDDAS_Auto_FD */
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	}
};

#ifdef F_SKYCAM_FIX_CFG_REFLECT
//YACBAC1SDDAS REFLECT
//================================================================
static const struct yacbac1sddas_i2c_reg_conf const yacbac1sddas_reflect_settings_cfg[][YACBAC1SDDAS_CFG_REFLECT_MAX_PARAMS] = {

	{
		//NONE
		{ 0x03,0x00, BYTE_LEN, 0},
		//{ 0x11, 0x91, BYTE_LEN, 0}
		{0x11, 0x90, BYTE_LEN, 100} // 0x93
	},
	{
		//MIRROR
		{ 0x03,0x00, BYTE_LEN, 0},
		//{ 0x11, 0x90, BYTE_LEN, 0}
		{ 0x11, 0x91, BYTE_LEN, 0}  // 0x92
	},
	{
		//WATER
		{ 0x03,0x00, BYTE_LEN, 0},
		{ 0x11, 0x92, BYTE_LEN, 0}  // 0x96
	},
	{
		//MIRROR + WATER
		{ 0x03,0x00, BYTE_LEN, 0},
		{ 0x11, 0x93, BYTE_LEN, 0}  //97
	}
};
#endif

struct yacbac1sddas_reg yacbac1sddas_regs = {

	.init_parm = &yacbac1sddas_init_parm[0],
	.init_parm_size = ARRAY_SIZE(yacbac1sddas_init_parm),

//preview frame rate
	.preview_fps_cfg_settings = yacbac1sddas_preview_fps_settings_cfg,
	.preview_fps_cfg_settings_size = YACBAC1SDDAS_CFG_PREVIEW_FPS_MAX_PARAMS,

//effect
	.effect_cfg_settings = yacbac1sddas_effect_settings_cfg,
	.effect_cfg_settings_size = YACBAC1SDDAS_CFG_EFFECT_MAX_PARAMS,

//wb
	.wb_cfg_settings = yacbac1sddas_wb_settings_cfg,
	.wb_cfg_settings_size = YACBAC1SDDAS_CFG_WB_MAX_PARAMS,

//brightness
	.bright_cfg_settings = yacbac1sddas_bright_settings_cfg,
	.bright_cfg_settings_size = YACBAC1SDDAS_CFG_BRIGHT_MAX_PARAMS,

//exposure
	.exposure_cfg_settings = yacbac1sddas_exposure_settings_cfg,
	.exposure_cfg_settings_size = YACBAC1SDDAS_CFG_EXPOSURE_MAX_PARAMS,

//flicker
	.flicker_cfg_settings = yacbac1sddas_flicker_settings_cfg,
	.flicker_cfg_settings_size = YACBAC1SDDAS_CFG_FLICKER_MAX_PARAMS,
#ifdef F_SKYCAM_FIX_CFG_REFLECT
//reflect
       .reflect_cfg_settings = yacbac1sddas_reflect_settings_cfg,
	.reflect_cfg_settings_size = YACBAC1SDDAS_CFG_REFLECT_MAX_PARAMS,
#endif
};

