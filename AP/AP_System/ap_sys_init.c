/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_system_init.c
Creator: zj.zong					Date: 2009-05-31
Description: init system(init variable and get voice parameter)
Others:
Version: V0.1
History:
	V0.1	2009-05-31		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_SYSTEM_INIT_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_System\ui_sys_header.h"
#include "..\..\UI\UI_System\ui_sys_var_ext.h"


extern void Api_Read_SysPara_ToDbuf0(void);
extern void _media_readsector(void);

void Ap_SysInit(void);
void Ap_Restore_FactoryPara(void);
void Ap_Restore_SysPara(void);
void Ap_Init_SysVar(void);
void Ap_Init_SysPara(void);
/*******************************************************************************
Function: Ap_SysInit()
description: init system parameter
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SysInit(void)
{
   	mSet_MCUClock_12M;
	Ap_Init_SysVar();						

	Ap_Restore_SysPara();				//read sysparameter from ef reserved area
	
	uc_save_mode_idx = MODE_5_SYSTEM;
	
	uc_window_no = WINDOW_SYS_MENU;
	uc_menu_cur_idx = 1;
	uc_menu_cur_idx_bk = 1;
	uc_menu_total_num = SYS_MENU_NUM;
	
	b_lcd_clrall_flg = 1;
	uc_lcd_dis_flg = 0xfd;				//set lcd display flag  clr bit 1 hold picture
	ui_sys_event |= EVT_DISPLAY;		//Set the display event
}



/*******************************************************************************
init system variable
********************************************************************************/
void Ap_Init_SysVar(void)
{
	uc_menu_exit_cnt = 0;
	ui_sys_event &= EVT_USB;
	uc_refresh_menu_num = 0;
	uc_refresh_select_flg = 0;
	uc_lcd_page_num_bk = 0;
	uc_menu_top_idx = 0;
	
}
 
/*******************************************************************************
get parameter from flash
********************************************************************************/
void Ap_Restore_SysPara(void)
{
	Api_Read_SysPara_ToDbuf0();

	Ap_Init_SysPara();
}

/*******************************************************************************
resume setting of factory
********************************************************************************/
void Ap_Restore_FactoryPara(void)
{
 	sfc_logblkaddr0 = 0x02;						//0x0000 0402
	sfc_logblkaddr1 = 0x04;
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
	sfc_reserved_area_flag = 1;					//read flash reserved area
	sfc_occ_buf_flag = SFC_APP_DBUF0;			//occupy dbuf 0x4000
	buf_offset0 = 0x00;							//buffer offset :0x0000
	buf_offset1 = 0x00;
	_media_readsector();						//read one sector	
 	Ap_Init_SysPara();
}


void Ap_Init_SysPara(void)
{
	if((uc_dbuf0[0] == 'S') && (uc_dbuf0[1] == 'I') && (uc_dbuf0[2] == 'N') && (uc_dbuf0[3] == 'O'))  
	{

			uc_mp3_eq_mode = uc_dbuf0[25];
			uc_mp3_repeat_mode = uc_dbuf0[26];
			uc_mp3_replay_time = uc_dbuf0[28];
			uc_mp3_replay_gap = uc_dbuf0[29];
			uc_mp3_volume_level = uc_dbuf0[30];
		   	uc_mp3_replay_mode = uc_dbuf0[31]; 
			uc_voice_repeat_mode = uc_dbuf0[79];			
			uc_rec_quality_level = uc_dbuf0[81];
				
			uc_voice_volume_level = uc_dbuf0[80];
			uc_fm_band = uc_dbuf0[210];
			uc_fm_volume_level = uc_dbuf0[221];
		
			uc_user_option1 = uc_dbuf0[335];
			uc_user_option2 = uc_dbuf0[336];
			uc_user_option3 = uc_dbuf0[337];
			uc_save_mode_idx = uc_dbuf0[339];
			uc_bklight_time_level = uc_dbuf0[341];
			uc_bklight_grade = uc_dbuf0[343];
	
			uc_idle_poweroff_level = uc_dbuf0[345];
			uc_sleep_poweroff_level = uc_dbuf0[346];

			uc_language_idx = uc_dbuf0[347];

			uc_language_total_num = uc_dbuf0[348];
			uc_disk_state = uc_dbuf0[349];
			uc_amt_onoff_flg = uc_dbuf0[352];

			uc_lcd_contrast_base = uc_dbuf0[362];
			uc_lcd_contrast_data = uc_dbuf0[363];
			uc_ebook_play_mode = uc_dbuf0[269];
			uc_ebook_play_gap = uc_dbuf0[270];
	
			uc_sys_date_buf[0] = uc_dbuf0[440];		//20090801
			uc_sys_date_buf[1] = uc_dbuf0[441];
			uc_sys_date_buf[2] = uc_dbuf0[442];
			uc_sys_date_buf[3] = uc_dbuf0[443];
			uc_sys_date_buf[4] = uc_dbuf0[444];
			uc_sys_date_buf[5] = uc_dbuf0[445];
			uc_sys_date_buf[6] = uc_dbuf0[446];
			uc_sys_date_buf[7] = uc_dbuf0[447];
			uc_sys_date_buf[8] = uc_dbuf0[448];
			uc_sys_date_buf[9] = uc_dbuf0[449];

			uc_sys_ver_buf[0] = uc_dbuf0[450];		//V1.00
			uc_sys_ver_buf[1] = uc_dbuf0[451];
			uc_sys_ver_buf[2] = uc_dbuf0[452];
			uc_sys_ver_buf[3] = uc_dbuf0[453];
			uc_sys_ver_buf[4] = uc_dbuf0[454];
			uc_sys_ver_buf[5] = uc_dbuf0[455];
			uc_sys_ver_buf[6] = uc_dbuf0[456];
			uc_sys_ver_buf[7] = uc_dbuf0[457];
			uc_sys_ver_buf[8] = uc_dbuf0[458];
			uc_sys_ver_buf[9] = uc_dbuf0[459];
			
			uc_disk_number = uc_dbuf0[507];

	}	
	sfc_reserved_area_flag = 0;					//clr reserved area flag
}





#endif