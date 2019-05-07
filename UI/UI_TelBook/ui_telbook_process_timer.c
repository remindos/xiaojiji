/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_telbook_process_timer.c
Creator: zj.zong					Date: 2009-06-05
Description: process telbook timer event, as subroutine in telbook mode
Others:
Version: V0.1
History:
	V0.1	2009-06-05		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_TELBOOK_PROCESS_TIMER_DEF

#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_api_define.h"
#include "..\..\UI\UI_TelBook\ui_telbook_header.h"
#include "..\..\UI\UI_TelBook\ui_telbook_var_ext.h"
extern void Ui_Check_Refresh_MenuFlag(void);

extern void Api_Check_Battery(void);
extern void Api_Check_Idle_PowerOff(void);
extern void Api_Check_Sleep_PowerOff(void);
extern void Api_Check_BklightOff(void);

void Ui_Check_Exit_TelBookMenu(void);
void Ui_SetLcdFlag_0xfd(void);
void Ui_Return_To_TelBookMain(void);
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

		Api_Check_Battery();			//check battery
		
		Ui_Check_Exit_TelBookMenu();	//check exit telbook menu

		Api_Check_BklightOff();			//check back light
		
		Api_Check_Idle_PowerOff();		//idle mode
										
		Api_Check_Sleep_PowerOff();		//sleep mode
		
		if(uc_window_no == WINDOW_TELBOOK_MAIN)
		{
			Ui_Check_Refresh_MenuFlag();
		}
	}
}


/*******************************************************************************
Function: Ui_Check_Exit_TelBookMenu()
Description: exit telbook menu 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Check_Exit_TelBookMenu(void)				//exit menu window
{
	if((++uc_menu_exit_cnt) >= TIME_EXIT_MENU)
	{
		switch(uc_window_no)
		{
			case WINDOW_TELEBOOK_IN:
				uc_menu_cur_idx = 2;
				Ui_Return_To_TelBookMain();
				ui_sys_event |= EVT_DISPLAY;
				break;
				
			case WINDOW_TELEBOOK_OUT:
			   	uc_menu_cur_idx = 3;
				Ui_Return_To_TelBookMain();
				ui_sys_event |= EVT_DISPLAY;
				break;
		}
	}
}


/*******************************************************************************
Function: Ui_Return_To_TelBookMain()
Description: return to telbook main
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Return_To_TelBookMain(void)
{
	uc_window_no = WINDOW_TELBOOK_MAIN;
	uc_menu_top_idx = 0;
	Ui_Init_TopIdx();
	uc_menu_total_num = 4;
	uc_lcd_dis_flg= 0x58;					
	b_lcd_clrall_flg = 1;	   //clr screen
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