/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_telbook_process_event.c
Creator: zj.zong					Date: 2009-06-05
Description: process telbook events (process USB, bat, ...)
			 as subroutine in telbook mode
Others:
Version: V0.1
History:
	V0.1	2009-06-02		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_TELBOOK_PROCESS_DEF

#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_api_define.h"
#include "..\..\UI\UI_TelBook\ui_telbook_header.h"
#include "..\..\UI\UI_TelBook\ui_telbook_var_ext.h"

extern void Ui_ProcessEvt_BatSub(void);
extern void Ui_PowerOn_Backlight(void);

extern void Ap_PowerOff_MP3(void);
extern void Ap_Exit_TelBookMode(void);
extern void Api_DynamicLoad_USB(void);
//extern void Ui_Display_InsertCard(void);
extern void Ap_SwitchTo_MainMenu(void);
extern bit FS_Finit(void);
extern bit b_sd_exist_flg;
extern bit b_has_real_data_flg;
extern void Api_Delay_1ms(uc8 timer_cnt);

void Ap_ProcessEvt_Usb(void);
void Ap_ProcessEvt_Bat(void);
//void Ap_Init_Msi_Sub(void);
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
	Ap_Exit_TelBookMode();		//exit telbook mode		
	Ui_PowerOn_Backlight();						

	Api_DynamicLoad_USB();	   //danamic load usb code
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
	Ui_ProcessEvt_BatSub();			//get battery level

	uc_lcd_dis_flg |= 0x01;			//display battery
	ui_sys_event |= EVT_DISPLAY;	

	if((MR_USBCON & 0x1) == 0)		//if USB connected, and not power off
	{
		if(uc_bat_low_cnt >= BATTERY_LOW_CNT)
		{
			uc_poweroff_type = POWEROFF_LOW_VOL;
			Ap_PowerOff_MP3();	    // power off mp3
		}
	}
}


/*void Ap_ProcessEvt_Msi(void)
{
	if((ui_bklight_time == 0) && (uc_bklight_time_level != 0))
	{
		Ui_PowerOn_Backlight();	
	}
	
	if(b_msi_plug_flg == 0)
	{
		if(!b_has_real_data_flg)
		{
			b_sd_exist_flg = 0;				//have not sd card
			uc_msi_status = 0xff;

			if(b_sd_flash_flg == 1)
			{
				b_sd_flash_flg=0;		
			}
//			Ui_Display_InsertCard();
			Ap_SwitchTo_MainMenu();
		}		
	}
	else										//refresh menu interface
	{
		Api_Delay_1ms(250);
		Ap_Init_Msi_Sub();					
	}	
}*/

/*void Ap_Init_Msi_Sub(void)
{
	uc_msi_status = 0xff;
	b_sd_flash_flg = 1;	
		
	if((MR_MSCON4 & 0x80) != 0)				//if have sd
	{
		if(uc_msi_status != 0)
		{
			MSI_Init();
			if(uc_msi_status == 0xff)
			{
				b_sd_exist_flg = 0;
			}
			else
			{
				b_sd_flash_flg=1;
				b_sd_exist_flg=1;
				if(FS_Finit() == 0)
				{
					b_sd_exist_flg = 0;
					uc_msi_status = 0xff; 						
				}	
	 		}
		}
	}
	else
	{
		b_sd_flash_flg = 0;	
		b_sd_exist_flg = 0;	
	}
}*/

#endif