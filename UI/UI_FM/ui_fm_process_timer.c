/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_fm_process_timer.c
Creator: zj.zong					Date: 2009-05-25
Description: process fm timer event, as subroutine in fm mode
Others:
Version: V0.1
History:
	V0.1	2009-05-25		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_FM_PROCESS_TIMER_DEF
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include ".\UI\UI_FM\ui_fm_header.h"
#include ".\UI\UI_FM\ui_fm_var_ext.h"
#include ".\DRV\DRV_FM\drv_fm.h"

extern void Ui_Check_Refresh_MenuFlag(void);

extern void Api_Check_Battery(void);
extern void Api_Check_Sleep_PowerOff(void);
extern void Api_Check_BklightOff(void);
extern void Api_Detect_Headphone_Plug(void);
extern void Api_Set_FM_Volume(uc8 volume_level);
extern void Ap_FM_CancelMute(void);
extern void Ap_FM_SetMute(void);
extern void Ap_FM_Enable_DAC(void);
extern void Ap_FM_Disable_DAC(void);

extern void Api_Check_Battery(void);
extern void Api_Check_Sleep_PowerOff(void);
extern void Api_Check_BklightOff(void);
extern void Api_Detect_Headphone_Plug(void);
extern void Api_Set_FM_Volume(uc8 volume_level);

void Ui_Check_Exit_Volume_Menu(void);
void Ui_Check_Headphone_ShortProtect(void);
void Ui_ProcessEvt_Timer(void);
void Ui_Check_FM_AutoSearch(void);
void Ui_Check_FM_Volume_Up(void);
void Ui_ReturnTo_FM(void);
void Ui_SetLcdFlag_0xfd(void);
void Ui_Init_TopIdx(void);
/*******************************************************************************
Function: Ui_ProcessEvt_Timer()
Description: process timer 100ms/once
Calls:
Global:uc_timer_cnt
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ProcessEvt_Timer(void)
{
   	if(uc_timer_cnt >= 5)				//5*20ms = 100ms
	{
		uc_timer_cnt = 0;

		Api_Check_Battery();				//detect battery

		Api_Check_Sleep_PowerOff();			//sleep mode

		if(b_fm_search_flg == 0)
		{
			Api_Check_BklightOff();		//power off backlight, if autosearch or halfsearch and have not power off bkligh
		}

		Ui_Check_Exit_Volume_Menu();		//5s exit setting volume and menu window

		Ui_Check_FM_AutoSearch();			//if it's autosearch or halfautosearch, and mute dac
		
		Ui_Check_FM_Volume_Up();			//volume up

		Ui_Check_Headphone_ShortProtect();

		#ifdef PCB_WITH_SPEAKER
		Api_Detect_Headphone_Plug();
		#endif
	}
}


/*******************************************************************************
Function: Ui_Check_Exit_Volume_Menu()
Description: exit delete menu or volume setting mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_Exit_Volume_Menu(void)
{
  	if(uc_window_no != WINDOW_FM)
	{
		if(uc_window_no == WINDOW_FM_MENU)
		{
			Ui_Check_Refresh_MenuFlag();
		}
		if((++uc_menu_exit_cnt) >= TIME_EXIT_MENU)
		{
			Ui_ReturnTo_FM();
		}
	}
}


/*******************************************************************************
Function: Ui_Check_FM_AutoSearch()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_FM_AutoSearch(void)
{
	if((uc_fm_mode != FM_MODE_PRESET) && (uc_fm_mode != FM_MODE_TUNING))	//auto search
	{
		if((MR_DACCLK & 0x04) == 0)
		{
			Ap_FM_SetMute();
			if(uc_fm_volume_level != 0)
			{
				Api_Set_FM_Volume(1);
			}
			MR_DACCLK |= 0x04;			//dac Reference voltage is ground
		}
	}
	else
	{
		if((MR_DACCLK & 0x04) != 0)
		{
			b_fm_volume_up = 1;
			uc_fm_volume_cnt = 0;
		}
	}
}


/*******************************************************************************
Function: Ui_Check_FM_Volume_Up()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_FM_Volume_Up(void)
{
	if(b_fm_volume_up == 1)
	{
		b_fm_volume_up = 0;
		Api_Set_FM_Volume(uc_fm_volume_level);
		Ap_FM_CancelMute();
		MR_DACCLK &= 0xfb;				//clr bit2; set Vrefdac is normal
	}
}


/*******************************************************************************
Function: Ui_Check_Headphone_ShortProtect()
Description: check headphone short protect
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_Headphone_ShortProtect(void)
{
	if((MR_DACSTA & 0x07) != 0)
	{
		Ap_FM_Disable_DAC();
		Ap_FM_Enable_DAC();
	}
	else
	{
		if((MR_DACPCON & 0x80) == 0)
		{
			MR_DACPCON |= 0x80;					//enable headphone short detection
		}
	}
}


/*******************************************************************************
Function: Ui_ReturnTo_FM_Menu()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ReturnTo_FM_Menu(void)
{
	uc_menu_cur_idx = uc_menu_cur_idx_bk;
	uc_menu_total_num = FM_MENU_NUM_MAX;
	
	uc_window_no = WINDOW_FM_MENU;
	uc_menu_top_idx = 0;
	Ui_Init_TopIdx();	

	Ui_SetLcdFlag_0xfd();
}



/*******************************************************************************
Function: Ui_ReturnTo_FM()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ReturnTo_FM(void)
{
	uc_window_no = WINDOW_FM;
	Ui_SetLcdFlag_0xfd();
	ui_sys_event |= EVT_DISPLAY;
}

/*******************************************************************************
Function: Ui_SetLcdFlag_0xfd()
Description: set clr lcd flag, and dis flag 0xff
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_SetLcdFlag_0xfd(void)	
{
	b_lcd_clrall_flg = 1;
	uc_lcd_dis_flg = 0xfd;		 //clr bit1 hold flag
}	

/*******************************************************************************
Function: Ui_Init_TopIdx()
Description: set clr lcd flag, and dis flag 0xff
*******************************************************************************/
void Ui_Init_TopIdx(void)
{
	if(uc_menu_cur_idx > (DisMenu_MaxNum + uc_menu_top_idx))	//"1 <= uc_menu_cur_idx <= 10"
	{
		uc_menu_top_idx = uc_menu_cur_idx - DisMenu_MaxNum;
	}
	else if(uc_menu_cur_idx <= uc_menu_top_idx)
	{
		uc_menu_top_idx = uc_menu_cur_idx - 1;
	}		
}


#endif