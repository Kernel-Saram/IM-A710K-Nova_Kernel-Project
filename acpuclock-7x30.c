/*
*
* Copyright (C) 2007 Google, Inc.
* Copyright (c) 2007-2010, Code Aurora Forum. All rights reserved.
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
* =========================================================================================
*
* DEVIL SANCTUARY - Low Voltage OverClock 
* 
* Update : 2012-10-15
*
* 극저전압이 아닌 극저전압 + 저전압 최종 안정 버전이다.
*
* =========================================================================================
*
*/

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <linux/mutex.h>
#include <linux/io.h>
#include <linux/sort.h>
#include <mach/board.h>
#include <mach/msm_iomap.h>
#include <asm/mach-types.h>

#include "smd_private.h"
#include "clock.h"
#include "clock-local.h"
#include "clock-7x30.h"
#include "acpuclock.h"
#include "spm.h"

#define SCSS_CLK_CTL_ADDR (MSM_ACC_BASE + 0x04)
#define SCSS_CLK_SEL_ADDR (MSM_ACC_BASE + 0x08)

#define PLL2_L_VAL_ADDR 		(MSM_CLK_CTL_BASE + 0x33C)
#define dprintk(msg...) 		cpufreq_debug_printk(CPUFREQ_DEBUG_DRIVER, "cpufreq-msm", msg)

#define VREF_SEL     1							/* 0: 0.625V (50mV step), 1: 0.3125V (25mV step). */
#define V_STEP       (25 * (2 - VREF_SEL)) 			/* Minimum voltage step size. */
#define VREG_DATA    (VREG_CONFIG | (VREF_SEL << 5))
#define VREG_CONFIG  (BIT(7) | BIT(6)) 				/* Enable VREG, pull-down if disabled. */

#define MV(mv)       ((mv) / (!((mv) % V_STEP))) 			/* Cause a compile error if the voltage is not a multiple of the step size. */
#define VDD_RAW(mv)  (((MV(mv) / V_STEP) - 30) | VREG_DATA) 	/* mv = (750mV + (raw * 25mV)) * (2 - VREF_SEL) */

#define MAX_AXI_KHZ  192000

struct clock_state {
	struct clkctl_acpu_speed	*current_speed;
	struct mutex			lock;
	uint32_t				acpu_switch_time_us;
	uint32_t				vdd_switch_time_us;
	struct clk				*ebi1_clk;
};

struct clkctl_acpu_speed {
	unsigned int	acpu_clk_khz;
	int			src;
	unsigned int	acpu_src_sel;
	unsigned int	acpu_src_div;
	unsigned int	axi_clk_hz;
	unsigned int	vdd_mv;
	unsigned int	vdd_raw;
	unsigned long	lpj;
};

static struct clock_state drv_state = { 0 };

/* DEVIL new generation PLL FREQ TABLE : MAX 1.7 */
static struct cpufreq_frequency_table freq_table[] = {
	{ 0, 122880 },
	{ 1, 245760 },
	{ 2, 368640 },
	{ 3, 460800 },
	{ 4, 576000 },
	{ 5, 652800 },
	{ 6, 768000 },
	{ 7, 806400 },
	{ 8, 921600 },
	{ 9, 1024000 },
	{ 10, 1113000 },
	{ 11, 1209600 },
	{ 12, 1305600 },
	{ 13, 1401600 },
	{ 14, 1516800 },
	{ 15, 1612800 },
	{ 16, 1708800 },
	{ 17, CPUFREQ_TABLE_END },
};

#define SRC_LPXO (-2)
#define SRC_AXI  (-1)

/*
*
* 전압 설정은 극저전압 설정으로 사용에는 프리징이 없으나 고사양게임에서 프리징 발생을 확인하여, 기존에 저전압설정을 기본으로 수정되었다.
*
* 테스트 해보니 전 클럭에서 고사양게임시 발열은 높아졌으나 프리징은 발생하지 않았다.
*
* 전압 최종 테스트일 : 2012-10-15 밤
*
*/

static struct clkctl_acpu_speed acpu_freq_tbl[] = {
	{ 24576,  SRC_LPXO, 0, 0,  30720000,  900, VDD_RAW(900) },
	{ 61440,  PLL_3,    5, 11, 61440000,  900, VDD_RAW(900) },
	{ 122880, PLL_3,    5, 5,  61440000,  900, VDD_RAW(900) },
	{ 184320, PLL_3,    5, 4,  61440000,  900, VDD_RAW(900) },
	{ MAX_AXI_KHZ, SRC_AXI, 1, 0, 61440000, 900, VDD_RAW(900) },
	{ 245760, PLL_3,    5, 2,  61440000,  900, VDD_RAW(900) },	
	{ 368640, PLL_3,    5, 1,  122800000, 900, VDD_RAW(900) },
	{ 460800, PLL_1,    2, 0,  153600000, 900, VDD_RAW(900) },
	{ 576000, PLL_1,    2, 0,  153600000, 900, VDD_RAW(900) },
	{ 652800, PLL_1,    2, 0,  153600000, 900, VDD_RAW(900) },
	{ 768000, PLL_1,    2, 0,  153600000, 1050, VDD_RAW(1050) },
	{ 806400, PLL_2,    3, 0,  UINT_MAX, 1100, VDD_RAW(1100) },
	{ 921600, PLL_2,    3, 0,  UINT_MAX, 1100, VDD_RAW(1100) },
	{ 1024000, PLL_2,   3, 0,  UINT_MAX, 1150, VDD_RAW(1150) },
	{ 1113000, PLL_2,   3, 0,  UINT_MAX, 1150, VDD_RAW(1150) },
	{ 1209600, PLL_2,   3, 0,  UINT_MAX, 1150, VDD_RAW(1150) },
	{ 1305600, PLL_2,   3, 0,  UINT_MAX, 1200, VDD_RAW(1200) },	
	{ 1401600, PLL_2,   3, 0,  UINT_MAX, 1200, VDD_RAW(1200) }, /* 1.4Ghz Modify 1250->1200 			(Stable : Down Voltage) */	
	{ 1516800, PLL_2,   3, 0,  UINT_MAX, 1250, VDD_RAW(1250) }, /* 1.5Ghz Modify 1300->1250->1225->1250	(Stable : Down Voltage) */
	{ 1612800, PLL_2,   3, 0,  UINT_MAX, 1300, VDD_RAW(1300) }, /* 1.6Ghz Modify 1350->1300->1275->1300	(Stable : Down Voltage) */	
	{ 1708800, PLL_2,   3, 0,  UINT_MAX, 1400, VDD_RAW(1400) }, /* 1.7Ghz Modify 1400->1350->1400 		(Stable : Normal Voltage) */
	{ 0 }
};


#define POWER_COLLAPSE_KHZ MAX_AXI_KHZ
unsigned long acpuclk_power_collapse(void)
{
	int ret = acpuclk_get_rate(smp_processor_id());
	acpuclk_set_rate(smp_processor_id(), POWER_COLLAPSE_KHZ, SETRATE_PC);
	return ret;
}

#define WAIT_FOR_IRQ_KHZ MAX_AXI_KHZ
unsigned long acpuclk_wait_for_irq(void)
{
	int ret = acpuclk_get_rate(smp_processor_id());
	acpuclk_set_rate(smp_processor_id(), WAIT_FOR_IRQ_KHZ, SETRATE_SWFI);
	return ret;
}

static int acpuclk_set_acpu_vdd(struct clkctl_acpu_speed *s)
{
	int ret = msm_spm_set_vdd(0, s->vdd_raw);
	if (ret)
		return ret;

	udelay(drv_state.vdd_switch_time_us);
	return 0;
}

static void acpuclk_set_src(const struct clkctl_acpu_speed *s)
{
	uint32_t reg_clksel, reg_clkctl, src_sel;

	reg_clksel = readl(SCSS_CLK_SEL_ADDR);

	src_sel = reg_clksel & 1;

	reg_clkctl = readl(SCSS_CLK_CTL_ADDR);
	reg_clkctl &= ~(0xFF << (8 * src_sel));
	reg_clkctl |= s->acpu_src_sel << (4 + 8 * src_sel);
	reg_clkctl |= s->acpu_src_div << (0 + 8 * src_sel);
	writel(reg_clkctl, SCSS_CLK_CTL_ADDR);

	/* Program PLL2 L val for overclocked speeds. */	
	if(s->src == PLL_2) {
		writel(s->acpu_clk_khz/19200, PLL2_L_VAL_ADDR);
	}
	
	reg_clksel ^= 1;

	writel(reg_clksel, SCSS_CLK_SEL_ADDR);
}

int acpuclk_set_rate(int cpu, unsigned long rate, enum setrate_reason reason)
{
	struct clkctl_acpu_speed *tgt_s, *strt_s;
	int res, rc = 0;

	if (reason == SETRATE_CPUFREQ)
		mutex_lock(&drv_state.lock);

	strt_s = drv_state.current_speed;

	if (rate == strt_s->acpu_clk_khz)
		goto out;

	for (tgt_s = acpu_freq_tbl; tgt_s->acpu_clk_khz != 0; tgt_s++) {
		if (tgt_s->acpu_clk_khz == rate)
			break;
	}
	if (tgt_s->acpu_clk_khz == 0) {
		rc = -EINVAL;
		goto out;
	}

	if (reason == SETRATE_CPUFREQ) {
		if (tgt_s->vdd_mv > strt_s->vdd_mv) {
			rc = acpuclk_set_acpu_vdd(tgt_s);
			if (rc < 0) {
				pr_err("ACPU VDD increase to %d mV failed "
					"(%d)\n", tgt_s->vdd_mv, rc);
				goto out;
			}
		}
	}

	dprintk("Switching from ACPU rate %u KHz -> %u KHz\n",
	       strt_s->acpu_clk_khz, tgt_s->acpu_clk_khz);

	if (tgt_s->axi_clk_hz > strt_s->axi_clk_hz) {
		rc = clk_set_min_rate(drv_state.ebi1_clk,
					tgt_s->axi_clk_hz);
		if (rc < 0) {
			pr_err("Setting AXI min rate failed (%d)\n", rc);
			goto out;
		}
	}

	if (strt_s->src != tgt_s->src && tgt_s->src >= 0) {
		dprintk("Enabling PLL %d\n", tgt_s->src);
		local_src_enable(tgt_s->src);
	}

	acpuclk_set_src(tgt_s);
	drv_state.current_speed = tgt_s;
	loops_per_jiffy = tgt_s->lpj;

	if (reason == SETRATE_SWFI)
		goto out;

	if (strt_s->src != tgt_s->src && strt_s->src >= 0) {
		dprintk("Disabling PLL %d\n", strt_s->src);
		local_src_disable(strt_s->src);
	}

	if (tgt_s->axi_clk_hz < strt_s->axi_clk_hz) {
		res = clk_set_min_rate(drv_state.ebi1_clk,
					tgt_s->axi_clk_hz);
		if (res < 0)
			pr_warning("Setting AXI min rate failed (%d)\n", res);
	}

	if (reason == SETRATE_PC)
		goto out;

	if (tgt_s->vdd_mv < strt_s->vdd_mv) {
		res = acpuclk_set_acpu_vdd(tgt_s);
		if (res)
			pr_warning("ACPU VDD decrease to %d mV failed (%d)\n",
					tgt_s->vdd_mv, res);
	}

	dprintk("ACPU speed change complete\n");
out:
	if (reason == SETRATE_CPUFREQ)
		mutex_unlock(&drv_state.lock);

	return rc;
}

unsigned long acpuclk_get_rate(int cpu)
{
	WARN_ONCE(drv_state.current_speed == NULL,
		  "acpuclk_get_rate: not initialized\n");
	if (drv_state.current_speed)
		return drv_state.current_speed->acpu_clk_khz;
	else
		return 0;
}

uint32_t acpuclk_get_switch_time(void)
{
	return drv_state.acpu_switch_time_us;
}

unsigned long clk_get_max_axi_khz(void)
{
	return MAX_AXI_KHZ;
}
EXPORT_SYMBOL(clk_get_max_axi_khz);


/*
----------------------------------------------------------------------------
		Clock driver initialization
---------------------------------------------------------------------------
*/

static void __init acpuclk_init(void)
{
	struct clkctl_acpu_speed *s;
	uint32_t div, sel, src_num;
	uint32_t reg_clksel, reg_clkctl;
	int res;

	drv_state.ebi1_clk = clk_get(NULL, "ebi1_clk");
	BUG_ON(IS_ERR(drv_state.ebi1_clk));

	reg_clksel = readl(SCSS_CLK_SEL_ADDR);

	switch ((reg_clksel >> 1) & 0x3) {

	case 0:
		reg_clkctl = readl(SCSS_CLK_CTL_ADDR);
		src_num = reg_clksel & 0x1;
		sel = (reg_clkctl >> (12 - (8 * src_num))) & 0x7;
		div = (reg_clkctl >> (8 -  (8 * src_num))) & 0xF;

		for (s = acpu_freq_tbl; s->acpu_clk_khz != 0; s++) {
			if (s->acpu_src_sel == sel && s->acpu_src_div == div)
				break;
		}
		if (s->acpu_clk_khz == 0) {
			pr_err("Error - ACPU clock reports invalid speed\n");
			return;
		}
		break;

	case 2:
		for (s = acpu_freq_tbl; s->acpu_clk_khz != 0
			&& s->src != PLL_2 && s->acpu_src_div == 0; s++)
			;
		if (s->acpu_clk_khz != 0) {
			acpuclk_set_src(s);

			reg_clksel = readl(SCSS_CLK_SEL_ADDR);
			reg_clksel &= ~(0x3 << 1);
			writel(reg_clksel, SCSS_CLK_SEL_ADDR);
			break;
		}
		
	default:
		pr_err("Error - ACPU clock reports invalid source\n");
		return;
	}

	acpuclk_set_acpu_vdd(s);

	drv_state.current_speed = s;

	if (s->src >= 0)
		local_src_enable(s->src);

	res = clk_set_min_rate(drv_state.ebi1_clk, s->axi_clk_hz);
	if (res < 0)
		pr_warning("Setting AXI min rate failed!\n");

	pr_info("ACPU running at %d KHz\n", s->acpu_clk_khz);

	return;
}

static void __init lpj_init(void)
{
	int i;
	const struct clkctl_acpu_speed *base_clk = drv_state.current_speed;

	for (i = 0; acpu_freq_tbl[i].acpu_clk_khz; i++) {
		acpu_freq_tbl[i].lpj = cpufreq_scale(loops_per_jiffy,
						base_clk->acpu_clk_khz,
						acpu_freq_tbl[i].acpu_clk_khz);
	}
}

#define RPM_BYPASS_MASK	(1 << 3)
#define PMIC_MODE_MASK	(1 << 4)

void __init msm_acpu_clock_init(struct msm_acpu_clock_platform_data *clkdata)
{
	pr_info("acpu_clock_init()\n");

	mutex_init(&drv_state.lock);
	drv_state.acpu_switch_time_us = clkdata->acpu_switch_time_us;
	drv_state.vdd_switch_time_us = clkdata->vdd_switch_time_us;
	acpuclk_init();
	lpj_init();

#ifdef CONFIG_CPU_FREQ_MSM
	cpufreq_frequency_table_get_attr(freq_table, smp_processor_id());
#endif
}

