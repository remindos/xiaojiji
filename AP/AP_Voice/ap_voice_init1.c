/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_voice_init.c
Creator: zj.zong					Date: 2009-05-21
Description: init voice(init variable and get voice parameter)
Others:
Version: V0.1
History:
	V0.1	2009-05-21		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_VOICE_INIT1_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Voice\ui_voice_header.h"
#include "..\..\UI\UI_Voice\ui_voice_var_ext.h"


extern bit FS_Get_VoiceDir_Idx(void);
//extern void Api_Set_Volume(uc8 uc_voice_volume_level);
extern void Api_Delay_1ms(uc8 timer_cnt);
extern void Api_Read_SysPara_ToDbuf0(void);
extern void Ap_Enable_DAC(void);

void Ap_Init_VoiceVar(void);
void Ap_Restore_VoicePara(void); 
void Ap_VoiceInit1(void);

/*******************************************************************************
Function: Ap_VoiceInit1()
description: init voice
             1. init voice variable
             2. get total wav file number in "VOICE" dir
             3. init DAC
             4. Open current played file
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_VoiceInit1(void)
{  switch(uc_dynamicload_idx)
  {
    case 40:
  	  Ap_Init_VoiceVar();				  			 //init voice variable

      Ap_Restore_VoicePara();					 //restore	voice parameter
      Ap_Enable_DAC();
	  break;

   case 41:
	  if(FS_Get_VoiceDir_Idx() == 1)			   //get wav total number in "VOICE" dir
	  {
		b_voice_folder_exist = 1;
	  }
	  else
	  {
		b_voice_folder_exist = 0;
 	  }					   
      break;
  }
   uc_dynamicload_idx = 0;
 }

/*******************************************************************************
init voice variable
********************************************************************************/
void Ap_Init_VoiceVar(void)
{
#ifdef AVOID_POP_NOISE
	b_voice_startplay_flg = 0;
	uc_voice_startplay_cnt = 0;
#endif
	b_voice_get_next_file = 1;
	b_voice_file_err = 0;
	b_voice_file_end = 0;
	b_voice_dec_reset_flg = 0;
 	b_voice_folder_exist = 0;
 	b_headphone_plug_flg = 0;
	uc_filldata_cnt = 0;
	uc_mpegstate_cnt = 0;
	b_sd_exist_flg = 0;

	ui_voice_wav_idx = 0;
	ui_voice_random_idx_bk = 0;
	ui_fb_total_num = 0;
	uc_voice_time_cnt = 0;
	ui_voice_cur_time = 0;
	ui_voice_total_time = 0;
	ui_voice_replay_a_time = 0;																																																	  
	ui_voice_replay_b_time = 0;
	ul_voice_replay_a_byte = 0;
	ul_voice_byterate = 0;
	ul_voice_wavdata_len = 0;
	ul_voice_samplerate = 0;
	ui_voice_audioformat = 0;
	uc_daci_sr = 0;
	
	uc_voice_repeat_mode = 0;
	uc_voice_replay_ab_flg = 0;
	uc_voice_ab_fliker_cnt = 0;
	uc_menu_exit_cnt = 0;
	uc_refresh_voice_cnt = 0;
	uc_refresh_pic_num = 0;
	uc_menu_top_idx = 0;
	uc_refresh_menu_num = 0;
  	uc_refresh_select_flg = 0;
  	uc_headphone_plug_cnt = 0;
  	uc_headphone_unplug_cnt = 0;

	uc_headphone_short_time = 0;

	uc_voice_status = STATE_STOP;
	uc_voice_status_bk = STATE_STOP;
	uc_mpeg_state = STATE_STOP;
	ui_sys_event &= EVT_USB;
	ui_fs_file_type = FS_FILE_TYPE_WAV;		//set bit3, file type is *.wav
}



/*******************************************************************************
get parameter from flash
********************************************************************************/
void Ap_Restore_VoicePara(void)
{
	Api_Read_SysPara_ToDbuf0();

	if((uc_dbuf0[0] == 'S') && (uc_dbuf0[1] == 'I') && (uc_dbuf0[2] == 'N') && (uc_dbuf0[3] == 'O'))  
	{
		b_sd_flash_flg = uc_dbuf0[78];				//select flash or SD/MMC card	

		if(b_sd_flash_flg == 0)
		{
			uc_fs_store_cluster1 = uc_dbuf0[70];		//the saved file addr in flash
			uc_fs_store_cluster2 = uc_dbuf0[71];
			uc_fs_store_length1 = uc_dbuf0[72];			//the saved file length in flash
			uc_fs_store_length2 = uc_dbuf0[73];
		}
		else
		{
			uc_fs_store_cluster1 = uc_dbuf0[74];		//the saved file addr in SD/MMC card
			uc_fs_store_cluster2 = uc_dbuf0[75];
			uc_fs_store_length1 = uc_dbuf0[76];			//the saved file length in SD/MMC card
			uc_fs_store_length2 = uc_dbuf0[77];	
		}

		uc_voice_repeat_mode = uc_dbuf0[79];			//voice repeat
		uc_voice_volume_level = uc_dbuf0[80];			//voice volume leve
	}
	else
	{
	uc_fs_store_cluster1 = 0;					//the saved file addr
	uc_fs_store_cluster2 = 0;
	uc_fs_store_length1 = 0;					//the saved file length
	uc_fs_store_length2 = 0;

	uc_voice_repeat_mode = REPEAT_FOLDER;		//repeat folder
	uc_voice_volume_level = 16;					//voice volume level
	}
	sfc_reserved_area_flag = 0;					//clr reserved area flag
}




#endif