#ifndef __ARCH_ARM_MACH_MSM_SKY_SYS_RESET_H
#define __ARCH_ARM_MACH_MSM_SKY_SYS_RESET_H

#if defined(CONFIG_MACH_MSM8X55_EF30S)
#define SYS_RESET_REASON_NORMAL   0x776655DD
#elif defined(CONFIG_MACH_MSM8X55_EF18K)
#define SYS_RESET_REASON_NORMAL   0x77665500
#endif
#define SYS_RESET_REASON_EXCEPTION	0x776655E0
#define SYS_RESET_REASON_ASSERT		0x776655E1
#define SYS_RESET_REASON_LINUX    0x776655E2
#define SYS_RESET_REASON_ANDROID  0x776655E3
#define SYS_RESET_REASON_UNKNOWN  0x776655E4

#define SYS_RESET_PDL_DOWNLOAD_ENTRY		0xCC33CC33
#define SYS_RESET_PDL_IDLE_DOWNLOAD_ENTRY	0x33CC33CC

extern uint32_t sky_sys_rst_GetResetReason(void);
extern uint32_t sky_sys_rst_GetReset(void);
extern uint32_t sky_sys_rst_GetLcdBLStatus(void);
extern uint32_t sky_sys_rst_GetSilentBoot(void);
extern char *sky_sys_rst_GetLogBuffer(void);
extern int sky_sys_rst_SetSwReset(uint32_t *reason);
extern int sky_sys_rst_SetUserReset(uint32_t *reason);
extern int sky_sys_rst_SetLcdBLStatus(uint32_t eBrightness);
extern void sky_sys_rst_SetSilentBoot(uint32_t mode);
extern int sky_sys_rst_SwReset(uint32_t *reason);
extern int sky_sys_rst_UserReset(uint32_t *reset);
extern int sky_sys_rst_SwReset_imm(uint32_t *reason);
extern int sky_sys_rst_UserReset_imm(uint32_t *reason);

extern int sky_sys_rst_SetSwReset_exception(void);
extern int sky_sys_rst_SetSwReset_assert(void);

extern void sky_sys_rst_PowerDown(void);
extern void sky_sys_rst_NotiToMARM(uint32_t *reason);

extern void sky_sys_rst_set_prev_reset_info(void);
extern int sky_sys_rst_read_proc_reset_info(char *page, char **start, off_t off, int count, int *eof, void *data);
extern int sky_sys_rst_write_proc_reset_info(struct file *file, const char *buffer, unsigned long count, void *data);
#if defined(CONFIG_MACH_MSM8X55_EF30S)
#ifdef FEATURE_SKY_CHG_LOGO
extern uint32_t sky_sys_rst_Get_ChargingMode(void);
extern void sky_sys_rst_Set_ChargingMode(uint32_t mode);
#endif
#endif
#endif
