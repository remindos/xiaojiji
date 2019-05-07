/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_fm_init.c
Creator: zj.zong				Date: 2009-05-23
Description: init fm (init variable and get fm parameter)
Others:
Version: V0.1
History:
	V0.1	2009-05-23		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_FM_INIT_DEF

#include "sh86278_api_define.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_FM\ui_fm_header.h"
#include ".\UI\UI_FM\ui_fm_var_ext.h"
#include ".\DRV\DRV_FM\drv_fm.h"

extern void Ap_FM_Turner_Init(void);
extern void Api_Delay_1ms(uc8 timer_cnt);
extern void Api_RadioSetMute(uc8 mute_flg);
extern void Ap_Restore_FM_Para(void);
extern void Ap_FM_Enable_DAC(void);
extern void Api_DynamicLoad_FM_Target(uc8 load_code_idx);

void Ap_Init_FM_Var(void);

/*******************************************************************************
Function: Ap_FM_Init()
description: init fm
             1. init fm variable
             2. init fm module
             3. init DAC
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_Init(void)
{
	Ap_Init_FM_Var();					//Init user variable
	
	Api_DynamicLoad_FM_Target(CODE_FM_1_RESTORE_PARA_IDX);	//restore saved parameter from ef
		
	Ap_FM_Turner_Init();				//init fm module
	
	Ap_FM_Enable_DAC();						//init DAC
	
	mReset_WatchDog;
	uc_save_mode_idx = MODE_4_FM;
	uc_window_no = WINDOW_FM;			//set fm window
	uc_lcd_dis_flg = 0xfd;				//clr bit1 hold picture
	b_lcd_clrall_flg = 1;				//clr all page flag
	ui_sys_event |= EVT_DISPLAY;		//Set the display event
}


/*******************************************************************************
init FM variable
********************************************************************************/
void Ap_Init_FM_Var(void)
{
	b_fm_volume_up = 1;
	b_fm_search_flg = 0;
 	b_headphone_plug_flg = 0;
	
	uc_menu_exit_cnt = 0;
	uc_fm_volume_cnt = 0;
	uc_fm_menu_ch_idx = 0;
	uc_fm_cur_ch_idx_bk = 0;
	uc_menu_top_idx = 0;
	uc_refresh_select_flg = 0;
	uc_refresh_menu_num = 0;
	uc_headphone_plug_cnt = 0;
 	uc_headphone_unplug_cnt = 0;

	ui_sys_event &= EVT_USB;
}


#endif