/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_sys_process_timer.c
Creator: zj.zong					Date: 2009-06-02
Description: process sys timer event, as subroutine in sys mode
Others:
Version: V0.1
History:
	V0.1	2009-06-02s		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_SYS_TIMER_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_System\ui_sys_header.h"
#include ".\UI\UI_System\ui_sys_var_ext.h"


extern void Api_Check_Battery(void);
extern void Api_Check_Idle_PowerOff(void);
extern void Api_Check_Sleep_PowerOff(void);
extern void Api_Check_BklightOff(void);
extern void Ui_Get_BkLight_Color(void);
extern void Ui_PowerOn_Backlight(void);
extern void Ui_Check_Refresh_MenuFlag(void);

extern void Api_LcdDriver_Set_BklightLevel(uc8 bklight_grade);

void Ui_Check_Exit_SysMenu(void); 
void Ui_ReturnTo_SysMenu(void);
void Ui_Init_TopIdx(void);
void Ui_ReturnTo_SysMenu_PowerOff(void);
void Ui_SetLcdFlag_0xfd(void);

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
	if(uc_timer_cnt >= 5)					//5*20ms = 100ms
	{
		uc_timer_cnt = 0;

		Api_Check_Battery();				//detect battery

		Ui_Check_Refresh_MenuFlag();

		Ui_Check_Exit_SysMenu();

		Api_Check_Idle_PowerOff();			//idle mode

		Api_Check_Sleep_PowerOff();			//sleep mode

		Api_Check_BklightOff();				//power off backlight
	}

}


/*******************************************************************************
Function: Ui_Check_Exit_SysMenu()
Description: exit second menu more than 5s
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/

void Ui_Check_Exit_SysMenu(void)
{
	if(uc_window_no != WINDOW_SYS_MENU)				//system menu
	{
		if((++uc_menu_exit_cnt) >= TIME_EXIT_MENU)	//5s
		{
			uc_menu_exit_cnt = 0;
			Ui_ReturnTo_SysMenu();
		}
	}
}


/*******************************************************************************
Function: Ui_ReturnTo_SysMenu()
Description:  exit second mnu
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ReturnTo_SysMenu(void)
{
	if(uc_window_no == WINDOW_SYS_MENU_CONTRAST)
	{
		Api_LcdDriver_Set_BklightLevel(uc_bklight_grade);
	}
 	
	uc_window_no = WINDOW_SYS_MENU;

	uc_menu_cur_idx = uc_menu_cur_idx_bk;
	uc_menu_total_num = SYS_MENU_NUM;

	uc_menu_top_idx = 0;
	Ui_Init_TopIdx();	

	Ui_SetLcdFlag_0xfd();
}

/*******************************************************************************
Function: Ui_ReturnTo_SysMenu_PowerOff()
Description:  exit second mnu
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ReturnTo_SysMenu_PowerOff(void)	 
{
	uc_window_no = WINDOW_SYS_MENU_POWEROFF;
	uc_menu_total_num = 3;
	uc_menu_top_idx = 0;
	Ui_Init_TopIdx();	

	Ui_SetLcdFlag_0xfd();
}

/*******************************************************************************
Function: Ui_Init_TopIdx()
Description: set clr lcd flag, and dis flag 0xff
*******************************************************************************/
void Ui_Init_TopIdx(void)
{
	if(uc_menu_cur_idx > (DisMenu_MaxNum + uc_menu_top_idx))		//"1 <= uc_menu_cur_idx <= 10"
	{
		uc_menu_top_idx = uc_menu_cur_idx - DisMenu_MaxNum;
	}
	else if(uc_menu_cur_idx <= uc_menu_top_idx)
	{
		uc_menu_top_idx = uc_menu_cur_idx - 1;
	}
	
	if((uc_window_no == WINDOW_SYS_MENU) && ((uc_user_option3&0x04)== 0))	//skip AMT menu
	{
		if(uc_menu_cur_idx > 6)
		{
			uc_menu_top_idx--;
		}
		else if(uc_menu_cur_idx >= DisMenu_MaxNum)					
		{
			uc_menu_top_idx = uc_menu_cur_idx - DisMenu_MaxNum;							
		}
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

#endif