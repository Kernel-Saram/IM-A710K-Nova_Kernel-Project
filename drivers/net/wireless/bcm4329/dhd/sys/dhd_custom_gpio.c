/*
* Customer code to add GPIO control during WLAN start/stop
* Copyright (C) 1999-2010, Broadcom Corporation
* 
*      Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2 (the "GPL"),
* available at http://www.broadcom.com/licenses/GPLv2.php, with the
* following added to such license:
* 
*      As a special exception, the copyright holders of this software give you
* permission to link this software with independent modules, and to copy and
* distribute the resulting executable under terms of your choice, provided that
* you also meet, for each linked independent module, the terms and conditions of
* the license of that module.  An independent module is a module which is not
* derived from this software.  The special exception does not apply to any
* modifications of the software.
* 
*      Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a license
* other than the GPL, without Broadcom's express prior written consent.
*
* $Id: dhd_custom_gpio.c,v 1.1.4.8.4.1 2010/09/02 23:13:16 Exp $
*/


#include <typedefs.h>
#include <linuxver.h>
#include <osl.h>
#include <bcmutils.h>

#include <dngl_stats.h>
#include <dhd.h>

#include <wlioctl.h>
#include <wl_iw.h>

#ifdef CONFIG_BCM4329_PT_PATCH
#include <asm/gpio.h>
#endif

#define FEATURE_SKY_WIFI_ADDR_BACKUP	//20110625 hskim_wifi for wifi mac setting

#ifdef FEATURE_SKY_WIFI_ADDR_BACKUP		//20110625 hskim_wifi for wifi mac setting
#include <mach/rpc_nv.h>
#define PANMAC_MAC_MAX_LEN  17
#define NV_WLAN_MAC_ADDRESS_I 4678
#endif     //FEATURE_SKY_WIFI_ADDR_BACKUP

#define WL_ERROR(x) printf x
#define WL_TRACE(x)

#ifdef CUSTOMER_HW
extern  void bcm_wlan_power_off(int);
extern  void bcm_wlan_power_on(int);
#endif /* CUSTOMER_HW */
#ifdef CUSTOMER_HW2
int wifi_set_carddetect(int on);
int wifi_set_power(int on, unsigned long msec);
int wifi_get_irq_number(unsigned long *irq_flags_ptr);
#endif

#ifdef CONFIG_BCM4329_PT_PATCH
static int dhd_wl_reset_gpio = CONFIG_BCM4329_WL_RESET_GPIO;
#endif

#if defined(OOB_INTR_ONLY)

#if defined(BCMLXSDMMC)
extern int sdioh_mmc_irq(int irq);
#endif /* (BCMLXSDMMC)  */

#ifdef CUSTOMER_HW3
#include <mach/gpio.h>
#endif

#if defined(CONFIG_BCM4329_PT_PATCH) && defined(CONFIG_BCM4329_RAND_MAC)
#include <linux/random.h>
#endif

/* Customer specific Host GPIO defintion  */
static int dhd_oob_gpio_num = -1; /* GG 19 */

module_param(dhd_oob_gpio_num, int, 0644);
MODULE_PARM_DESC(dhd_oob_gpio_num, "DHD oob gpio number");

int dhd_customer_oob_irq_map(unsigned long *irq_flags_ptr)
{
	int  host_oob_irq = 0;

#ifdef CUSTOMER_HW2
	host_oob_irq = wifi_get_irq_number(irq_flags_ptr);

#else /* for NOT  CUSTOMER_HW2 */
#if defined(CUSTOM_OOB_GPIO_NUM)
	if (dhd_oob_gpio_num < 0) {
		dhd_oob_gpio_num = CUSTOM_OOB_GPIO_NUM;
	}
#endif

	if (dhd_oob_gpio_num < 0) {
		WL_ERROR(("%s: ERROR customer specific Host GPIO is NOT defined \n",
			__FUNCTION__));
		return (dhd_oob_gpio_num);
	}

	WL_ERROR(("%s: customer specific Host GPIO number is (%d)\n",
	         __FUNCTION__, dhd_oob_gpio_num));

#if defined(CUSTOMER_HW) || defined(CONFIG_BCM4329_PT_PATCH)
	host_oob_irq = MSM_GPIO_TO_INT(dhd_oob_gpio_num);
#elif defined CUSTOMER_HW3
	gpio_request(dhd_oob_gpio_num, "oob irq");
	host_oob_irq = gpio_to_irq(dhd_oob_gpio_num);
	gpio_direction_input(dhd_oob_gpio_num);
#endif /* CUSTOMER_HW */
#endif /* CUSTOMER_HW2 */

	return (host_oob_irq);
}
#endif /* defined(OOB_INTR_ONLY) */

/* Customer function to control hw specific wlan gpios */
void
dhd_customer_gpio_wlan_ctrl(int onoff)
{
	switch (onoff) {
		case WLAN_RESET_OFF:
			WL_TRACE(("%s: call customer specific GPIO to insert WLAN RESET\n",
				__FUNCTION__));
#ifdef CUSTOMER_HW
			bcm_wlan_power_off(2);
#endif /* CUSTOMER_HW */
#ifdef CONFIG_BCM4329_PT_PATCH
			gpio_set_value(dhd_wl_reset_gpio, 0);
			printk("WLAN_RESET_OFF[%d]\n",
				gpio_get_value(dhd_wl_reset_gpio));
#endif
#ifdef CUSTOMER_HW2
			wifi_set_power(0, 0);
#endif
			WL_ERROR(("=========== WLAN placed in RESET ========\n"));
		break;

		case WLAN_RESET_ON:
			WL_TRACE(("%s: callc customer specific GPIO to remove WLAN RESET\n",
				__FUNCTION__));
#ifdef CUSTOMER_HW
			bcm_wlan_power_on(2);
#endif /* CUSTOMER_HW */
#ifdef CONFIG_BCM4329_PT_PATCH
			gpio_set_value(dhd_wl_reset_gpio, 1);
			printk("WLAN_RESET_ON[%d]\n",
				gpio_get_value(dhd_wl_reset_gpio));
#endif
#ifdef CUSTOMER_HW2
			wifi_set_power(1, 0);
#endif
			WL_ERROR(("=========== WLAN going back to live  ========\n"));
		break;

		case WLAN_POWER_OFF:
			WL_TRACE(("%s: call customer specific GPIO to turn off WL_REG_ON\n",
				__FUNCTION__));
#ifdef CUSTOMER_HW
			bcm_wlan_power_off(1);
#endif /* CUSTOMER_HW */
#ifdef CONFIG_BCM4329_PT_PATCH
			gpio_set_value(dhd_wl_reset_gpio, 0);
			printk("WLAN_POWER_OFF[%d]\n",
				gpio_get_value(dhd_wl_reset_gpio));
#endif
		break;

		case WLAN_POWER_ON:
			WL_TRACE(("%s: call customer specific GPIO to turn on WL_REG_ON\n",
				__FUNCTION__));
#ifdef CUSTOMER_HW
			bcm_wlan_power_on(1);
#endif /* CUSTOMER_HW */
#ifdef CONFIG_BCM4329_PT_PATCH
			gpio_set_value(dhd_wl_reset_gpio, 1);
			printk("WLAN_POWER_ON[%d]\n",
				gpio_get_value(dhd_wl_reset_gpio));
#endif
			/* Lets customer power to get stable */
			OSL_DELAY(200);
		break;
	}
}

#ifdef GET_CUSTOM_MAC_ENABLE
/* Function to get custom MAC address */
int
dhd_custom_get_mac_address(unsigned char *buf)
{
	WL_TRACE(("%s Enter\n", __FUNCTION__));
	if (!buf)
		return -EINVAL;

	/* Customer access to MAC address stored outside of DHD driver */

#ifdef EXAMPLE_GET_MAC
	/* EXAMPLE code */
	{
		struct ether_addr ea_example = {{0x00, 0x11, 0x22, 0x33, 0x44, 0xFF}};
		bcopy((char *)&ea_example, buf, sizeof(struct ether_addr));
	}
#endif /* EXAMPLE_GET_MAC */

#ifndef FEATURE_SKY_WIFI_ADDR_BACKUP //20110625 hskim_wifi for wifi mac setting
#ifdef CONFIG_BCM4329_PT_PATCH
	{
		void *fp = NULL;
		int len;
		int ret = -1;
		char ethbuf[32] = {0, };
		static struct ether_addr tmp;
		static int custom_mac_applied = 0;


		if ( custom_mac_applied )
		{
			memcpy(buf, &tmp, sizeof(struct ether_addr));
			return 0;
		}


		if ((fp = dhd_os_open_image(CONFIG_BCM4329_WLANMAC_PATH)) == NULL)
			goto err2;

		len = dhd_os_get_image_block(ethbuf, 32, fp);
		if ( len < 17 ) /* mac address format xx:xx:xx:xx:xx:xx */
			goto err1;

		ethbuf[17] = '\0';
		if (sscanf(ethbuf, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
			&tmp.octet[0], &tmp.octet[1], &tmp.octet[2],
			&tmp.octet[3], &tmp.octet[4], &tmp.octet[5]) != 6)
			goto err1;

		memcpy(buf, &tmp, sizeof(struct ether_addr));
		custom_mac_applied = 1;

		ret = 0;
err1:
		dhd_os_close_image(fp);
err2:
		if ( ret )
		{
#ifdef CONFIG_BCM4329_RAND_MAC
			int i;

			tmp.octet[0] = 0x00;
			tmp.octet[1] = 0x0f;
			tmp.octet[2] = 0xe4;
			for ( i = 3; i < ETHER_ADDR_LEN; i++ )
				get_random_bytes(&tmp.octet[i], 1);

			memcpy(buf, &tmp, sizeof(struct ether_addr));
			custom_mac_applied = 1;
			ret = 0;
#endif
			return ret;
		}
	}
#endif /* CONFIG_BCM4329_PT_PATCH */

	return 0;
#endif   //20100827 thkim_wifi block for wifi mac setting

#ifdef FEATURE_SKY_WIFI_ADDR_BACKUP	
#ifdef CONFIG_BCM4329_PT_PATCH
	{
		//void *fp = NULL;	//hskim_wifi block for compile error
		//int len;	//hskim_wifi block for compile error
		int ret = -1;
		//char ethbuf[32] = {0, };	//hskim_wifi block for compile error
		//static struct ether_addr tmp;   //20101022 thkim_wifi for soft ap wifi mac setting
		static int custom_mac_applied = 0;

		//20100827 thkim_wifi for wifi mac setting
		//#ifdef CONFIG_PANTECH_WIFI_MAC_ADDR
		  static struct ether_addr nv_mac;
		  nv_cmd_item_type item;
		  int result;
		  //char temp[PANMAC_MAC_MAX_LEN + 1];	//hskim_wifi block for compile error
		  int i;  
		//#endif     //CONFIG_PANTECH_WIFI_MAC_ADDR		

		if ( custom_mac_applied )
		{
			memcpy(buf, &nv_mac, sizeof(struct ether_addr));   //20101022 thkim_wifi for soft ap wifi mac setting
			return 0;
		}
		
		  result = msm_nv_read(NV_WLAN_MAC_ADDRESS_I, &item);
		  if (0 != result)
		  {
		    printk(KERN_INFO "msm_nv_read failed (%d)\n", result);
		    // return 0;
		  }	
		       nv_mac.octet[0] = (item.wlan_mac_address[0] >> 8)  & 0xFF;
		       nv_mac.octet[1] = (item.wlan_mac_address[0] >> 16) & 0xFF;
		       nv_mac.octet[2] = (item.wlan_mac_address[0] >> 24) & 0xFF;			 
		       nv_mac.octet[3] = (item.wlan_mac_address[1] >> 16) & 0xFF;            	
		       nv_mac.octet[4] = (item.wlan_mac_address[1] >> 24) & 0xFF;	
		       nv_mac.octet[5] = (item.wlan_mac_address[0])       & 0xFF;			 
		   		 
		#if 0   //20100702 thkim_wifi test code	 
		     printk(KERN_INFO "nv_mac.octet[0] (%d)\n", nv_mac.octet[0]);
			 printk(KERN_INFO "nv_mac.octet[1] (%d)\n", nv_mac.octet[1]);  
			 printk(KERN_INFO "nv_mac.octet[2] (%d)\n", nv_mac.octet[2]);
			 printk(KERN_INFO "nv_mac.octet[3] (%d)\n", nv_mac.octet[3]);  
			 printk(KERN_INFO "nv_mac.octet[4] (%d)\n", nv_mac.octet[4]);
			 printk(KERN_INFO "nv_mac.octet[5] (%d)\n", nv_mac.octet[5]);  
		#endif
		
		if ((result !=0)||((nv_mac.octet[0]==0x00)&&(nv_mac.octet[1]==0x12)&&(nv_mac.octet[2]==0x34)
			&&(nv_mac.octet[3]==0x56)&&(nv_mac.octet[4]==0x78)&&(nv_mac.octet[5]==0x90)))
			goto err1;		

		memcpy(buf, &nv_mac, sizeof(struct ether_addr));
		custom_mac_applied = 1;

		ret = 0;

err1:
		if ( ret )
		{
#ifdef CONFIG_BCM4329_RAND_MAC

			//20100827 thkim_wifi for oversea group wifi mac address oui   //20101022 thkim_wifi for soft ap wifi mac setting
			nv_mac.octet[0] = 0x2e;
			nv_mac.octet[1] = 0x30;
			nv_mac.octet[2] = 0x68;
			
			for ( i = 3; i < ETHER_ADDR_LEN; i++ )
				get_random_bytes(&nv_mac.octet[i], 1);    //20101022 thkim_wifi for soft ap wifi mac setting

			memcpy(buf, &nv_mac, sizeof(struct ether_addr));    //20101022 thkim_wifi for soft ap wifi mac setting
			custom_mac_applied = 1;
			ret = 0;
#endif
			return ret;
		}
	}
#endif /* CONFIG_BCM4329_PT_PATCH */

	return 0;
#endif    //FEATURE_SKY_WIFI_ADDR_BACKUP	
}
#endif /* GET_CUSTOM_MAC_ENABLE */
