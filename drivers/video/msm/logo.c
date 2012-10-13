/* drivers/video/msm/logo.c
 *
 * Show Logo in RLE 565 format
 *
 * Copyright (C) 2008 Google Incorporated
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fb.h>
#include <linux/vt_kern.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>

#include <linux/irq.h>
#include <asm/system.h>

#define fb_width(fb)	((fb)->var.xres)
#define fb_height(fb)	((fb)->var.yres)
#define fb_size(fb)	((fb)->var.xres * (fb)->var.yres * 2)

#ifdef CONFIG_SKY_FB_24BPP_RGB888
static void memset24(void *_ptr, unsigned short val, unsigned count)
{
    unsigned char *ptr = _ptr;
    unsigned char r, g, b;

    r = (unsigned char)((val & 0xf800) >> 8);
    g = (unsigned char)((val & 0x07e0) >> 3);
    b = (unsigned char)((val & 0x001f) << 3);

    count >>= 1;
    while (count--)
    {
#if (0)    
        *ptr++ = 0;
        *ptr++ = b;
        *ptr++ = g;
        *ptr++ = r; // 32bpp
#else
        *ptr++ = b;
        *ptr++ = g;
        *ptr++ = r;
        *ptr++ = 0; // 32bpp
#endif
    }
}
#else
static void memset16(void *_ptr, unsigned short val, unsigned count)
{
	unsigned short *ptr = _ptr;
	count >>= 1;
	while (count--)
		*ptr++ = val;
}
#endif


/* 565RLE image format: [count(2 bytes), rle(2 bytes)] */
int load_565rle_image(char *filename)
{
	struct fb_info *info;
	int fd, err = 0;
	unsigned count, max;
#ifdef CONFIG_SKY_FB_24BPP_RGB888
	unsigned char *data, *bits;
   unsigned short *ptr;
#else
	unsigned short *data, *bits, *ptr;
#endif

	info = registered_fb[0];
	if (!info) {
		printk(KERN_WARNING "%s: Can not access framebuffer\n",
			__func__);
		return -ENODEV;
	}

	fd = sys_open(filename, O_RDONLY, 0);
	if (fd < 0) {
		printk(KERN_WARNING "%s: Can not open %s\n",
			__func__, filename);
		return -ENOENT;
	}
	count = (unsigned)sys_lseek(fd, (off_t)0, 2);
	if (count == 0) {
		sys_close(fd);
		err = -EIO;
		goto err_logo_close_file;
	}
	sys_lseek(fd, (off_t)0, 0);
	data = kmalloc(count, GFP_KERNEL);
	if (!data) {
		printk(KERN_WARNING "%s: Can not alloc data\n", __func__);
		err = -ENOMEM;
		goto err_logo_close_file;
	}
	if ((unsigned)sys_read(fd, (char *)data, count) != count) {
		err = -EIO;
		goto err_logo_free_data;
	}

	max = fb_width(info) * fb_height(info);
	ptr = (unsigned short *)data;
#ifdef CONFIG_SKY_FB_24BPP_RGB888
	bits = (unsigned char *)(info->screen_base);
#else
	bits = (unsigned short *)(info->screen_base);
#endif
	while (count > 3) {
		unsigned n = ptr[0];
		if (n > max)
			break;
#ifdef CONFIG_SKY_FB_24BPP_RGB888
		memset24(bits, ptr[1], n << 1);
		//bits += (n*3);
		bits += (n*4);
#else
		memset16(bits, ptr[1], n << 1);
		bits += n;
#endif
		max -= n;
		ptr += 2;
		count -= 4;
	}

err_logo_free_data:
	kfree(data);
err_logo_close_file:
	sys_close(fd);
	return err;
}

#ifdef SKY_LCD_LOADBAR_ANDROID
#if defined(FEATURE_SW_RESET_RELEASE_MODE)
extern bool sky_sys_rst_is_silent_boot_mode(void);
#endif

static unsigned short loading_progress_t[8] = 
{
//    0x9c13, 0xd61a, 0xc5b8, 0xa474, 0x8330, 0x9c33, 0xc5b8, 0x9c33
//    0x000c,0x0000,0x01a0,0x0212,0x0009,0x0000,0x06c0,0x0000
    0x9c13,0xd61a,0xc598,0xa454,0x8310,0x9c13,0xc598,0x9c13
};

int sky_lcdc_display_loadingbar(int ratio)
{

   struct fb_info *info;
   int err = 0;
#ifdef CONFIG_SKY_FB_24BPP_RGB888
   unsigned char *load_bar;
#else
   unsigned short *load_bar;
#endif
   unsigned short i;
   unsigned short BAR_COLOR = 0xFFFF;

   const unsigned short ST_X = 19, ST_Y = 743;
   const unsigned short SCREEN_WIDTH = 480;
   const unsigned short BAR_WIDTH = 441;
   const unsigned short BAR_HEIGHT = 8;

   int cr=0;
   
#if defined(FEATURE_SW_RESET_RELEASE_MODE)
   if(sky_sys_rst_is_silent_boot_mode())
   	 return 0;
#endif

   if(ratio>100) ratio=100;
   cr=(int)(ratio*BAR_WIDTH/100);


   info = registered_fb[0];
   if (!info) {
      printk(KERN_WARNING "%s: Can not access framebuffer\n",
      __func__);
      return -ENODEV;
   }

#ifdef CONFIG_SKY_FB_24BPP_RGB888
   //load_bar = (unsigned char *)(info->screen_base) + ((SCREEN_WIDTH*ST_Y+ST_X)*3);
   load_bar = (unsigned char *)(info->screen_base) + ((SCREEN_WIDTH*ST_Y+ST_X)*4); // 32bpp
#else
   load_bar = (unsigned short *)(info->screen_base) + SCREEN_WIDTH*ST_Y+ST_X;
#endif
   for(i=0;i<BAR_HEIGHT;i++)
   {
      BAR_COLOR = loading_progress_t[i];
#ifdef CONFIG_SKY_FB_24BPP_RGB888
      memset24(load_bar, BAR_COLOR, cr<<1);
      //load_bar = load_bar + (SCREEN_WIDTH*3);
      load_bar = load_bar + (SCREEN_WIDTH*4); // 32bpp
#else
      memset16(load_bar, BAR_COLOR, cr<<1);
      load_bar = load_bar + SCREEN_WIDTH;
#endif
   }

   return err;
}

EXPORT_SYMBOL(sky_lcdc_display_loadingbar);
#endif //#ifdef SKY_LCD_LOADBAR_ANDROID

EXPORT_SYMBOL(load_565rle_image);
