/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_record_init1.c
Creator: zj.zong				Date: 2009-06-02
Description: init record (init variable and get record parameter)
Others:
Version: V0.1
History:
	V0.1	2009-06-02		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_RECORD_INIT1_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Record\ui_rec_header.h"
#include "..\..\UI\UI_Record\ui_rec_var_ext.h"

extern bit  FS_Get_VoiceDir_Idx(void);



void Ap_Init_RecVar(void);
//void Ap_Restore_RecPara(void);
bit  Ap_Get_NextWav_Idx(void);

/*******************************************************************************
Function: Ap_Rec_Init1()
description: init record
             1. init record variable
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Rec_Init1(void)
{
	Ap_Init_RecVar();					//init record variable

//	Ap_Restore_RecPara();				//restore parameter from ef

	if(FS_Get_VoiceDir_Idx() == 0)		//get "voice" floder index, if haven't "voice" folder and creat it
	{
		b_rec_card_full_flg = 1;		//card full
		uc_fs_wav_idx = 0;
		ui_rec_wav_total_num = 0;
	}
}



/*******************************************************************************
init record variable
********************************************************************************/
void Ap_Init_RecVar(void)
{
	b_rec_card_full_flg = 0;
	b_rec_wav_full_flg = 0;
	b_rec_decbuf_flg = 0;
	b_rec_mode_flg = 0;
	b_rec_adcbuf_full_flg = 1;
	b_rec_have_rec_flg  = 0;
	b_flicker_time_flg  = 0;
	b_sd_exist_flg = 0;

	uc_enc_step_idx = 0;
	ul_rec_cur_time = 0;
	ul_rec_remain_time = 0;
	ui_rec_wav_total_num = 0;
	uc_menu_exit_cnt = 0;
	uc_rec_time_cnt = 0;
	uc_rec_sav_map_idx = 0;
	uc_rec_sav_map_val = 0;
	
	uc_rec_playkey_cnt_h = 0;
	uc_rec_playkey_cnt_l = 0;
	uc_refresh_pic_num = 0;
	uc_refresh_rec_cnt = 0;
	uc_refresh_time_cnt = 0;
	uc_lcd_page_num_bk = 0;
	uc_refresh_menu_num = 0;
	uc_menu_top_idx = 0;
  
  	uc_rec_status = STATE_STOP;
	ui_sys_event &= EVT_USB;
	ui_fs_file_type = FS_FILE_TYPE_WAV;		//set bit3, file type is *.wav
}


#endif