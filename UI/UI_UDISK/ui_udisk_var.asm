/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_usb_var.asm
Creator: zj.zong				Date: 2009-04-29
Description: define variable be used in usb mode
Others: 
Version: V0.1
History:
	V0.1	2009-04-29		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_UDISK_VAR_DEF

/*****************************************/
#ifndef UI_DISK_VAR
#define UI_DISK_VAR


PUBLIC	USBStatus_bk		
PUBLIC	USBStatus	

PUBLIC	USBConnect
PUBLIC	USBCharge_cnt	
PUBLIC	USBCharge_idx

PUBLIC	usb_startflag	

PUBLIC	uc_pwmr_width
PUBLIC	uc_pwmg_width
PUBLIC	uc_pwmb_width
PUBLIC	uc_pwmw_width
PUBLIC	uc_pwmc_width
PUBLIC	uc_pwm_step
PUBLIC	uc_pwm_cnt
PUBLIC	uc_pwm_mode


	XSEG	AT		7200H

USBStatus_bk:				DS	1
USBStatus:					DS	1

USBConnect:					DS	1
USBCharge_cnt:				DS	1
USBCharge_idx:				DS	1

usb_startflag:				DS	1

uc_pwmr_width:				DS	1
uc_pwmg_width:				DS	1
uc_pwmb_width:				DS	1
uc_pwmw_width:				DS	1
uc_pwmc_width:				DS	1
uc_pwm_step:				DS	1
uc_pwm_cnt:					DS	1
uc_pwm_mode:				DS	1

#endif
#endif
	END
