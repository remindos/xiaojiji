/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_poweron.c
Creator: zj.zong					Date: 2009-04-22
Description:
Others:
Version: V0.1
History:
	V0.1	2009-04-22		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_POWERON_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"

extern void Ui_PowerOn_Backlight(void);

extern void Ap_Init_Common_Variable(void);
extern void Ap_Get_Sys_Parameter(void);
extern void Ap_Detect_VoltageLevel(void);
extern void Ap_PowerOn_DynamicLoad_Music(void);
extern void Ap_PowerOn_DynamicLoad_Record(void);
extern void Ap_PowerOn_DynamicLoad_Voice(void);
extern void Ap_PowerOn_DynamicLoad_FM(void);
extern void Ap_PowerOn_DynamicLoad_System(void);
extern void Ap_PowerOn_DynamicLoad_Ebook(void);
extern void Ap_PowerOn_DynamicLoad_MainMenu(void);
extern void Ap_PowerOn_DynamicLoad_Telbook(void);

extern void Api_Delay_1ms(uc8 time_cut);
extern void Api_DynamicLoad_PowerOff(void);
extern void Api_Display_Picture(ui16 ui_pic_idx);
extern bit FS_Finit(void);
extern void Api_LcdDriver_Set_BklightLevel(uc8 bklight_grade);

void Ui_Check_PowerOff_LowVoltage(void);
void Ui_Check_PowerOff_DiskError(void);
void Ui_Display_WelcomePicture(void);
void Ui_Enter_SubMode(void);

/*******************************************************************************
Function: Ui_PowerOn_Main()
Description: power on mp3
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_PowerOn_Main(void)
{
	mReset_PC_Stack;						//initial PC Stack
	MR_IE0 = 0;
	MR_IE1 = 0;								//disable all IRQ
	MR_MSCON2 &= 0xFB;     				//de-assert MSI Plug flg 
	MR_MODEN2 |= 0x40;					//enable MSI Plug irq
	mReset_WatchDog;
   	mSet_MCUClock_12M;

	Ap_Init_Common_Variable();				//initial common variable used in all mode

	Ap_Get_Sys_Parameter();					//read sys parameter 512bytes
	
	Ui_Check_PowerOff_LowVoltage();
	Ui_Check_PowerOff_DiskError();
		
	Ui_PowerOn_Backlight();					//prepare display welcome picture

/*	if(b_usb_poweron == 0)					//pull out USB port, and USB not supply power, display welcome picture
	{
		Ui_Display_WelcomePicture();
	}*/

	Ui_Enter_SubMode();						//enter sub mode depend on variable uc_user_option2,bit0 	  
}


/*******************************************************************************
Function: Ui_Check_PowerOff_LowVoltage()
Description: detect if has USB connected, if no USB connected, power off mp3 when bat level is 0
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_PowerOff_LowVoltage(void)
{
	if((MR_USBCON & 0x1) == 0)
	{
		Ap_Detect_VoltageLevel();
	}
	else
	{
		uc_bat_level = 5;						//usb Mode
	}
	uc_bat_charge_idx = uc_bat_level;			//init battery charge index

	if(uc_bat_level == 0)
	{
		uc_poweroff_type = POWEROFF_LOW_VOL;	//low voltage power off
		Api_DynamicLoad_PowerOff();
	}
}


/*******************************************************************************
Function: Ui_Check_PowerOff_DiskError()
Description: detect if FS initial Flah error, if error and power off mp3
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_PowerOff_DiskError(void)
{
	if(FS_Finit() == 0)							//check disk
	{
		uc_poweroff_type = POWEROFF_FLASH_ERR;
		Api_DynamicLoad_PowerOff();
	}
}



/*******************************************************************************
Function: Ui_Display_WelcomePicture()
Description: diaplay welcome picture
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_WelcomePicture(void)
{
	uc8 i;

	b_play_poweron_flg = 1;					//set flag

	for(i = 0; i < uc_poweron_pic_num; i++)
	{
		mReset_WatchDog;					//clear watchdog
		uc_lcd_column_num = 0;
		uc_lcd_page_num = 0;
		Api_Display_Picture(PIC_POWERON_BASE_IDX + i);
		Api_Delay_1ms(TIME_POWERON_DIS);
	}	
} 


/*******************************************************************************
Function: Ui_Enter_SubMode()
Description: enter sub mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Enter_SubMode(void)
{
	Ap_PowerOn_DynamicLoad_Music();
	if((uc_user_option2 & 0x01) == 0)			//bit0: 0--enter the last power off mode; 1--enter main menu
	{
	  	switch(uc_save_mode_idx)
		{
			case MODE_0_MAIN_MENU:
				Ap_PowerOn_DynamicLoad_MainMenu();
				break;

			case MODE_1_MUSIC:
				Ap_PowerOn_DynamicLoad_Music();
				break;

			case MODE_2_RECORD:
				Ap_PowerOn_DynamicLoad_Record();
				break;

			case MODE_3_VOICE:
				Ap_PowerOn_DynamicLoad_Voice();
				break;

			case MODE_4_FM:
				Ap_PowerOn_DynamicLoad_FM();
				break;

			case MODE_5_SYSTEM:
				Ap_PowerOn_DynamicLoad_System();
				break;

			case MODE_6_EBOOK:
				Ap_PowerOn_DynamicLoad_Ebook();
				break;

			case MODE_7_TELBOOK:
				Ap_PowerOn_DynamicLoad_Telbook();
				break;

			default:
				Ap_PowerOn_DynamicLoad_MainMenu();
				break;
		}
	}
	else
	{
		Ap_PowerOn_DynamicLoad_MainMenu();
	}	
}

#endif