/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_fm_process_event.c
Creator: zj.zong					Date: 2009-05-25
Description: process fm events (process USB, bat, ...)
			 as subroutine in fm mode
Others:
Version: V0.1
History:
	V0.1	2009-05-25		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_FM_PROCESS_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_FM\ui_fm_header.h"
#include ".\UI\UI_FM\ui_fm_var_ext.h"
#include ".\DRV\DRV_FM\drv_fm.h"


extern void Ui_ProcessEvt_BatSub(void);
extern void Ui_PowerOn_Backlight(void); 
extern void Ap_PowerOff_MP3(void);
extern void Api_DynamicLoad_USB(void);
extern void Ap_Exit_FM_Mode(void);
extern void Ap_FM_AutoSearch(void);

void Ap_ProcessEvt_Usb(void);
void Ap_ProcessEvt_Bat(void);
void Ap_processEvt_FM_Search(void);


/*******************************************************************************
Function: Ap_ProcessEvt_Usb()
Description: process usb event, enter usb mode
Calls:
Global:uc_load_code_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_ProcessEvt_Usb(void)
{
	Ap_Exit_FM_Mode();				//save fm parameter				
	Ui_PowerOn_Backlight();						
	Api_DynamicLoad_USB();
}


/*******************************************************************************
Function: Ap_processEvt_FM_Search()
Description: process fm autosearch or halfsearch
Calls:
Global:uc_fm_mode
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_processEvt_FM_Search(void)
{
	Ap_FM_AutoSearch();

	uc_lcd_dis_flg |= 0x40;			//display freq, freq bar, ch index
	ui_sys_event |= EVT_DISPLAY;	//set the display event
}


/*******************************************************************************
Function: Ap_ProcessEvt_Bat()
Description: process battery
Calls:
Global:uc_bat_low_cnt, uc_poweroff_type
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_ProcessEvt_Bat(void)
{
 	Ui_ProcessEvt_BatSub();							//get battery level
	if(b_bat_changed_flg == 1)
	{
		b_bat_changed_flg = 0;
	 	uc_lcd_dis_flg |= 0x01;						//display battery
		ui_sys_event |= EVT_DISPLAY;	
	}
	
	if((MR_USBCON & 0x1) == 0)						//if USB connected, and not power off
	{
		if(uc_bat_low_cnt >= BATTERY_LOW_CNT)
		{
			uc_poweroff_type = POWEROFF_LOW_VOL;
			Ap_PowerOff_MP3();
		}
	}
}


#endif