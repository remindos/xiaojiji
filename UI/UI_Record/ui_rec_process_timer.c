/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_record_process_timer.c
Creator: zj.zong					Date: 2009-06-02
Description: process record timer event, as subroutine in record mode
Others:
Version: V0.1
History:
	V0.1	2009-06-02		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_RECORD_PROCESS_TIMER_DEF

#include "sh86278_api_define.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_Record\ui_rec_header.h"
#include ".\UI\UI_Record\ui_rec_var_ext.h"

extern void Api_Check_Battery(void);
extern void Api_Check_Idle_PowerOff(void);
extern void Api_Check_Sleep_PowerOff(void);
extern void Api_Check_BklightOff(void);
extern void Ui_Check_Refresh_MenuFlag(void);

void Ui_ProcessEvt_Timer(void);
void Ui_Get_Cur_RecordingTime(void);
void Ui_Check_Refresh_RecLogo(void);
void Ui_Check_Refresh_CurrentTime(void);
void Ui_SetLcdFlag_0xfd(void);
void Ui_ReturnTo_Rec(void);
void Ui_ReturnTo_MenuSD(void);
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
	if(uc_timer_cnt >= 5)						//5*20ms = 100ms
	{
		uc_timer_cnt = 0;

		Api_Check_Battery();

		Api_Check_Sleep_PowerOff();				//sleep mode
		Api_Check_BklightOff();					//power off backlight

	/*	if(uc_msi_status != 0)					//Timeout Error
		{
			b_msi_plug_flg = 0;
			ui_sys_event |= EVT_MSI;					//unplug SD Card/MMC
			return;
		}	*/

		if(uc_rec_status == STATE_STOP)
		{
			Api_Check_Idle_PowerOff();			//if record stop state and power off
		}
		else if(uc_rec_status == STATE_RECORD)
		{
			ui_idle_poweroff_cnt = 0;

			Ui_Get_Cur_RecordingTime();			//get the current recording time
			Ui_Check_Refresh_RecLogo();
		}
		if(uc_window_no != WINDOW_REC)
		{
			Ui_Check_Refresh_MenuFlag();
		}
 		else if(uc_rec_status == STATE_PAUSE)
		{
			Ui_Check_Refresh_CurrentTime();
		}
	}
}


/*******************************************************************************
Function: Ui_Check_Refresh_CurrentTime()
Description:
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_Refresh_CurrentTime(void)
{
   	if(uc_refresh_time_cnt++ > TIME_REFRESH_CURRENT_TIME)					
	{
		uc_refresh_time_cnt = 0;								
		b_flicker_time_flg = ~b_flicker_time_flg;
		uc_lcd_dis_flg |= 0x04;
		ui_sys_event |= EVT_DISPLAY;
	 }
}

/*******************************************************************************
Function: Ui_Get_Cur_RecordingTime()
Description: check current recording time(+1s)
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_Refresh_RecLogo(void)
{
	if(uc_refresh_rec_cnt++ > TIME_REFRESH_REC)				
	{
		uc_refresh_rec_cnt = 0;							
		uc_refresh_pic_num++;

		if(uc_refresh_pic_num >= 8)													
		{
			uc_refresh_pic_num = 0;								
		}
		uc_lcd_dis_flg |= 0x14;
		ui_sys_event |= EVT_DISPLAY;
	}
}


/*******************************************************************************
Function: Ui_Get_Cur_RecordingTime()
Description: check current recording time(+1s)
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Get_Cur_RecordingTime(void)
{
	ul_rec_temp_var = ((ul_fs_byte_offset - 512) >> 9) * 101 / 800;	 //calculate the current time
	if(b_rec_samplerate_flg == 1)				//16KHz sample rate(collect 16*1024 byte per second)
	{
		ul_rec_cur_time = ul_rec_temp_var >> 1;
	}
	else										//8KHz sample rate(collect 8*1024 byte per second)
	{
		ul_rec_cur_time = ul_rec_temp_var;
	}
	uc_rec_time_cnt++;							//refresh time

	if(uc_rec_time_cnt >= REC_1S_TIME)
	{
		uc_rec_time_cnt = 0;	
		uc_lcd_dis_flg |= 0x14;					//set bit2,bit4 refresh time
		ui_sys_event |= EVT_DISPLAY;
	}
}

/*******************************************************************************
Function: Ui_SetLcdFlag_0xfd()
Description:  set display flag
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_SetLcdFlag_0xfd(void)
{
	b_lcd_clrall_flg = 1;
	uc_lcd_dis_flg= 0xfd;					//bit 1 hold picture
	ui_sys_event |= EVT_DISPLAY;			//set the display event
}

 /*******************************************************************************
Function: Ui_ReturnTo_MenuSD()
Description: return to rec SD menu
*******************************************************************************/
void Ui_ReturnTo_MenuSD(void)
{
	uc_window_no = WINDOW_REC_MENU_SD;
	uc_menu_total_num =4;// Rec_MenuSD_Num;
	Ui_SetLcdFlag_0xfd();
	ui_sys_event |= EVT_DISPLAY;
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

/*******************************************************************************
Function: Ui_ReturnTo_Rec()
Description:  return to record window
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ReturnTo_Rec(void)
{
	uc_window_no = WINDOW_REC;
	Ui_SetLcdFlag_0xfd();
}

#endif