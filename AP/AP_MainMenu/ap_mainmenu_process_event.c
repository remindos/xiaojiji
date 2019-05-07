/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_mainmenu_process_event.c
Creator: zj.zong					Date: 2009-04-22
Description: process mainmenu events
Others:
Version: V0.1
History:
	V0.1	2009-04-22		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_MAINMENU_PROCESS_DEF

#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "..\..\UI\UI_MainMenu\ui_mainmenu_header.h"
#include "..\..\UI\UI_MainMenu\ui_mainmenu_var_ext.h"

extern void Ui_ProcessEvt_BatSub(void);
extern void Ui_PowerOn_Backlight(void);
extern void Api_Read_SysPara_ToDbuf0(void);
extern void Api_DynamicLoad_CodeMpm1_8kExecute(void);
extern void Api_DynamicLoad_CodeCpm(void);
extern void FS_WrFlash_Interface(void);

void Ap_ProcessEvt_Usb(void);
void Ap_ProcessEvt_Bat(void);
void Ap_PowerOff_MP3(void);
void Ap_Enable_IrqBit(void);
void Ap_Save_MainMenu_Para(void);


/*******************************************************************************
Function: Ap_Enable_IrqBit()
Description: enable irq
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Enable_IrqBit(void)
{
 	MR_IE0 = 0x01;						//enable USB Detect irq
	MR_IE1 = 0x3f;						//enable mcu interrupt(timer irq, ADC irq)
	MR_USBCON |= 0x04;					//enable USB Detect irq
	MR_IE0 |= 0x80;						//enable irq
}


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
	Ui_PowerOn_Backlight();	
   	uc_load_code_idx = CODE_USB_MODE_CPM_IDX;
	Api_DynamicLoad_CodeCpm();
	uc_load_code_idx = CODE_USB_MODE_IDX;
	Api_DynamicLoad_CodeMpm1_8kExecute();
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
	Ui_ProcessEvt_BatSub();				//pm1_common.c

	uc_lcd_dis_flg |= 0x01;				//display battery
	ui_sys_event |= EVT_DISPLAY;	

	if((MR_USBCON & 0x1) == 0)			//if USB connected, and not power off
	{
		if(uc_bat_low_cnt >= BATTERY_LOW_CNT)
		{
			uc_poweroff_type = POWEROFF_LOW_VOL;
			Ap_PowerOff_MP3();
		}
	}	
}


/*******************************************************************************
Function: power_off_mp3()
Description: enter power off mode
Calls:
Global:uc_poweroff_type
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_PowerOff_MP3(void)
{
	Ap_Save_MainMenu_Para();
	
	uc_load_code_idx = CODE_POWEROFF_MODE_IDX;		//enter power off mp3 mode
	Api_DynamicLoad_CodeMpm1_8kExecute();
}


/*******************************************************************************
Function: Ap_Restore_MainMenu_Para()
Description: restore mainmenu parameter from flash for one sector
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Restore_MainMenu_Para(void)
{
	Api_Read_SysPara_ToDbuf0();
	sfc_reserved_area_flag = 0;
	
	if((uc_dbuf0[0] == 'S') && (uc_dbuf0[1] == 'I') && (uc_dbuf0[2] == 'N') && (uc_dbuf0[3] == 'O'))
	{
		uc_save_mode_idx = uc_dbuf0[339];
	}
	else
	{
		uc_save_mode_idx = 1;
	}
}


/*******************************************************************************
Function: Ap_Save_MainMenu_Para()
Description: save mainmenu parameter to flash for one sector
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Save_MainMenu_Para(void)
{
	Api_Read_SysPara_ToDbuf0();

	uc_dbuf0[339] = uc_save_mode_idx;				//save mode 

	b_sfc_immediate_wr_flg = 1;
	FS_WrFlash_Interface();
	sfc_reserved_area_flag = 0;					//clr reserved area flag

}

#endif