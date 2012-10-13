#include <stdio.h>
#include <linux/gpio.h>
#include <mach/vreg.h>
#include <linux/delay.h>

#ifdef CONFIG_MACH_MSM8X55_EF30S
#define YAS529_GPIO_RESET	177
#define YAS529_VREG_STR		"rf" // L14
#define YAS529_VREG_VOLT	2800
#else
#error "[yas_control.c] BOARD_VER is not defined."
#endif

static bool yas_power_status = 0; // 0:off / 1:on

void yas529_set_power(bool on)
{
	struct vreg *vr;
	int rc;

	if(on != yas_power_status) {
	
		vr = vreg_get(NULL, YAS529_VREG_STR);
		if(on) {
			rc = vreg_set_level(vr, YAS529_VREG_VOLT);
			if(rc){
				printk(KERN_ERR "%s: vreg %s set level failed (%d)\n", __func__, YAS529_VREG_STR, rc);
			}
			else {
				rc = vreg_enable(vr);
				if (rc) {
					printk(KERN_ERR "%s: vreg %s enable failed (%d)\n", __func__, YAS529_VREG_STR, rc);
				}
			}
		}
		else {
			rc = vreg_disable(vr);
			if(rc){
				printk(KERN_ERR "%s: vreg %s set level failed (%d)\n", __func__, YAS529_VREG_STR, rc);
			}
		}

		yas_power_status = on;
	}
}

void yas529_set_reset(bool enable)
{
	unsigned cfg = GPIO_CFG(YAS529_GPIO_RESET, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA);
	gpio_tlmm_config(cfg, GPIO_CFG_ENABLE);

	if(enable) {
		gpio_set_value(YAS529_GPIO_RESET, 1);
		mdelay(5);
		gpio_set_value(YAS529_GPIO_RESET, 0);
		mdelay(5);
		gpio_set_value(YAS529_GPIO_RESET, 1);
		mdelay(5);
	}
	else {
		gpio_set_value(YAS529_GPIO_RESET, 0);
		mdelay(5);
	}
}
