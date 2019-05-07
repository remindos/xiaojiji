/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_system_enhance.c
Creator: zj.zong					Date: 2009-06-01
Description: subroutine with no decode for mcu
Others:
Version: V0.1
History:
	V0.1	2009-06-01		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_SYSTEM_ENHANCE_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_System\ui_sys_header.h"
#include "..\..\UI\UI_System\ui_sys_var_ext.h"

extern void Ui_ProcessEvt_BatSub(void);
extern void Ui_PowerOn_Backlight(void);
extern void Ui_ReturnTo_SysMenu(void);

extern void Api_Delay_1ms(uc8 time_cnt);
extern void Api_DynamicLoad_CodeMpm1_8kExecute(void);
extern void Api_Read_SysPara_ToDbuf0(void);
extern void Api_DynamicLoad_PowerOff(void);
extern void Api_DynamicLoad_MainMenu(void);
extern void Api_DynamicLoad_USB(void);
extern void _media_readsector(void);
extern void FS_WrFlash_Interface(void);
extern bit FS_Finit(void);
extern void FS_GetSpace(void);

void Ap_Exit_SysMode(void);
void Ap_PowerOff_MP3(void);
void Ap_ProcessEvt_Usb(void);
void Ap_ProcessEvt_Bat(void);
void Ap_Save_SysPara(void);
void Ap_Enable_IrqBit(void);
void Ap_ProcessEvt_Msi(void);
void Ap_Save_Sys_USB_Para(void);



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
	MR_IE1 = 0x07;						//enable mcu interrupt(timer irq, ADC irq MSI)
	MR_USBCON |= 0x04;					//enable USB Detect irq
	MR_IE0 |= 0x80;						//enable irq
}


/*******************************************************************************
Function: Ap_SwitchTo_MainMenu()
Description: return to main	menu
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SwitchTo_MainMenu(void)
{
	Ap_Exit_SysMode();	
	Api_DynamicLoad_MainMenu();	   //enter main menu
}
/*******************************************************************************
Function: Ap_Exit_SysMode()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Exit_SysMode(void)
{
	Ap_Save_SysPara();
}


/*******************************************************************************
Function: Ap_Save_SysPara()
Description: save system parameter
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Save_SysPara(void)
{
	Api_Read_SysPara_ToDbuf0();

	uc_dbuf0[25] = uc_mp3_eq_mode;
	uc_dbuf0[26] = uc_mp3_repeat_mode;
	uc_dbuf0[28] = uc_mp3_replay_time;
	uc_dbuf0[29] = uc_mp3_replay_gap;
	uc_dbuf0[30] = uc_mp3_volume_level;
	uc_dbuf0[31] = uc_mp3_replay_mode; 
	uc_dbuf0[79] = uc_voice_repeat_mode;			
	uc_dbuf0[81] = uc_rec_quality_level;
	uc_dbuf0[80] = uc_voice_volume_level;
	uc_dbuf0[210] = uc_fm_band;
	uc_dbuf0[221] = uc_fm_volume_level;
		
	uc_dbuf0[335] = uc_user_option1;
	uc_dbuf0[336] = uc_user_option2;
	uc_dbuf0[337] = uc_user_option3;
	uc_dbuf0[339] =	uc_save_mode_idx;
	uc_dbuf0[341] = uc_bklight_time_level;
	uc_dbuf0[343] = uc_bklight_grade;
	
	uc_dbuf0[345] = uc_idle_poweroff_level;
	uc_dbuf0[346] = uc_sleep_poweroff_level;
	uc_dbuf0[347] = uc_language_idx;

	uc_dbuf0[348] = uc_language_total_num;
	uc_dbuf0[349] = uc_disk_state;
	uc_dbuf0[362] = uc_lcd_contrast_base;
	uc_dbuf0[363] = uc_lcd_contrast_data;
	uc_dbuf0[269] = uc_ebook_play_mode;
	uc_dbuf0[270] = uc_ebook_play_gap;
	uc_dbuf0[352] =	uc_amt_onoff_flg;

	
	b_sfc_immediate_wr_flg = 1;
	FS_WrFlash_Interface();
	Ap_Save_Sys_USB_Para();


}


 
void Ap_Save_Sys_USB_Para(void)
{

	sfc_logblkaddr0 = 0x04;						//0x0000 0204
	sfc_logblkaddr1 = 0x02;
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
	sfc_occ_buf_flag = SFC_APP_DBUF0;			//occupy dbuf0
	buf_offset0 = 0x00;							//buffer offset :0x0000
	buf_offset1 = 0x00;
	_media_readsector();						//read one sector

	if(uc_disk_number == 2)
	{
		uc_dbuf0[4] = (uc_dbuf0[4]&0xfc) | uc_disk_state;//only can modify bit0 and bit1 for disk1 and disk2
	}

	if((uc_user_option3&0x04) != 0)
	{
		if(uc_amt_onoff_flg == 1)
		{
			uc_dbuf0[4] = uc_dbuf0[4]|0x04;//only can modify bit2 for amt 1 have 0 no 
		}
		else
		{
			uc_dbuf0[4] = uc_dbuf0[4]&0xfb;//only can modify bit2 for amt 1 have 0 no 
		}
	}
	b_sfc_immediate_wr_flg = 1;
	FS_WrFlash_Interface();
	sfc_reserved_area_flag = 0;						//clr reserved area	flg
}

/*******************************************************************************
Function: Ap_PowerOff_MP3()
Description: enter power off mode
Calls:
Global:uc_poweroff_type
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_PowerOff_MP3(void)
{
	Ap_Exit_SysMode();
	
	Api_DynamicLoad_PowerOff();
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
	Ui_ProcessEvt_BatSub();				

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
	Ap_Exit_SysMode();
	Ui_PowerOn_Backlight();						
	Api_DynamicLoad_USB();
}

/*******************************************************************************
Function: Ap_ProcessEvt_Msi()
Description: check SD/MMC card plug in or out, and process event corresponding  
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_ProcessEvt_Msi(void)
{
	if((ui_bklight_time == 0) && (uc_bklight_time_level != 0))
	{
		Ui_PowerOn_Backlight();
	}

	if((b_msi_plug_flg == 0) && (b_sd_flash_flg == 1))
	{
		b_sd_flash_flg = 0;

		if(FS_Finit() == 0)
		{
			uc_poweroff_type = POWEROFF_FLASH_ERR;
			Api_DynamicLoad_PowerOff();
		}

		mSet_MCUClock_33M;
		FS_GetSpace();				//get the flash spare capacity
		mSet_MCUClock_12M;

		Ui_ReturnTo_SysMenu();
	}
}


#endif