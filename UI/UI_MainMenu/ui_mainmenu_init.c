/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_mainmenu_init.c
Creator: zj.zong					Date: 2009-04-22
Description: select mode(7'mode:music, .... telbook)
Others:
Version: V0.1
History:
	V0.1	2009-04-22		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_MAINMENU_INIT_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_MainMenu\ui_mainmenu_header.h"
#include ".\UI\UI_MainMenu\ui_mainmenu_var_ext.h"


extern void Ui_PowerOn_Backlight(void);
extern void Ap_Restore_MainMenu_Para(void);

void Ui_Init_MainMenu(void);
void Ui_Init_Menu_TopIdx(void);


/*******************************************************************************
Function: init_top_menu()
Description: initial top menu parameter
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Init_MainMenu(void)
{						
   	mSet_MCUClock_12M;
	Ap_Restore_MainMenu_Para();
	ui_sys_event &= EVT_USB;

	ui_idle_poweroff_cnt = 0;
  	if(uc_save_mode_idx == 0)
	{
		uc_menu_cur_idx = 1;
	}
	else
	{
		uc_menu_cur_idx = uc_save_mode_idx;		//init menu index and total item
	}

	uc_menu_total_num = MAINMENU_SUBMODE_NUM;	//total menu is 7

	Ui_PowerOn_Backlight();

	uc_save_mode_idx = MODE_0_MAIN_MENU;
	b_lcd_clrall_flg = 1;
	uc_lcd_dis_flg = 0xfd;						//set lcd display flag
	ui_sys_event |= EVT_DISPLAY;				//Set the display event	
}


#endif