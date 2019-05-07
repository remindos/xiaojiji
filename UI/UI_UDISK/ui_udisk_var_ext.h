/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_usb_var_ext.h
Creator:zj.zong					Date: 2009-04-29
Description: extern usb variable for "*.c"
Others:
Version: V0.1
History:
	V0.1	2009-04-29		zj.zong
			1. build this module
********************************************************************************/
#ifndef UI_UDISK_VAR_EXT
#define UI_UDISK_VAR_EXT

extern uc8  xdata USBStatus;
extern uc8  xdata USBStatus_bk;

extern uc8 xdata USBConnect;
extern uc8 xdata USBCharge_cnt;
extern uc8 xdata USBCharge_idx;
extern uc8 xdata usb_startflag;

extern uc8 idata uc_200us_timer_cnt;
extern uc8 xdata uc_pwmr_width;
extern uc8 xdata uc_pwmg_width;
extern uc8 xdata uc_pwmb_width;
extern uc8 xdata uc_pwmw_width;
extern uc8 xdata uc_pwmc_width;
extern uc8 xdata uc_pwm_step;
extern uc8 xdata uc_pwm_cnt;
extern uc8 xdata uc_pwm_mode;

extern bit b_led_flg;
extern bit b_led_pa3;
extern bit b_led_pa4;
extern bit b_led_pa5;
extern bit b_led_pa6;
extern bit b_bat_low_flag;

#endif