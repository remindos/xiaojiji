/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_music_init.c
Creator: zj.zong					Date: 2009-04-27
Description: initial music (include variable and get music parameter)
Others:
Version: V0.1
History:
	V0.1	2009-04-27		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_MUSIC_INIT_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Music\ui_music_header.h"
#include "..\..\UI\UI_Music\ui_music_var_ext.h"

extern void Ap_Enable_DAC(void);
extern void Api_Read_SysPara_ToDbuf0(void);
extern void Api_Set_Volume(uc8 vol_level);

extern void _media_readsector(void);
extern ui16 FS_ExitSubDir(void);
extern bit FS_Fopen(ui16 file_idx);
extern bit FS_Fopen_Appointed(ui16 file_idx);
extern bit FS_EnterSubDir(ui16 dir_index);
extern bit FS_Chk_FileType(void);
extern void FS_EnterRootDir(void);
extern void FS_EnterDir_Appointed(void);
extern void FS_GetItemNum_InDir(void);
extern void FS_GetFilePath(void);

void Ap_Init_MusicVar(void);
void Ap_Restore_MusicPara(void);
void Ap_Scan_AllDisk(void);
void Ap_Init_Music_Display(void);
#ifdef SP_PLAY_RESERVEDMP3_DEF
void Ap_Rsv_Get_MP3Number(void);
#endif
/*******************************************************************************
Function: Ap_MusicInit()
Description: init music
             1. init music variable
             2. get total mp3 file number in disk
             3. init DAC
             4. Open current played file
*******************************************************************************/
void Ap_MusicInit(void)
{
	switch(uc_dynamicload_idx)
	{
		case 40:
			Ap_Init_MusicVar();
			Ap_Restore_MusicPara();
			Ap_Enable_DAC();
			Ap_Init_Music_Display();
#ifdef SP_PLAY_RESERVEDMP3_DEF
			Ap_Rsv_Get_MP3Number();	//ef reserved
#endif
			break;

		case 41:
			mSet_MCUClock_33M;

			Ap_Scan_AllDisk();
			if(b_fb_nofile_flg == 0)
			{	
				FS_EnterDir_Appointed();
				FS_GetItemNum_InDir();							//get the mp3 number in cur dir
				FS_Fopen_Appointed(ui_mp3_idx_indir - 1);		//open the current file
			}

			mSet_MCUClock_12M;
			break;
		case 42:
			mSet_MCUClock_33M;

			Ap_Scan_AllDisk();
			FS_EnterRootDir();
			mSet_MCUClock_12M;
			break;
	}

	uc_dynamicload_idx = 0;	
}


void Ap_Init_Music_Display(void)
{
	uc_save_mode_idx = MODE_1_MUSIC;
	//uc_window_no = WINDOW_MUSIC;						//set music window
	//uc_window_no = WINDOW_MUSIC_TOP_MENU;
	uc_window_no = WINDOW_MUSIC_LOGO;
	uc_menu_cur_idx = uc_save_mode_idx;//1;
	uc_menu_total_num = 5;
	b_lcd_clrall_flg = 1;								//clr all page flag
	uc_lcd_dis_flg = 0xff;								//display all icon
	ui_sys_event |= EVT_DISPLAY;						//Set the display event	
}


/*******************************************************************************
Function: Ap_Init_MusicVar()
Description: init music variable
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_Init_MusicVar(void)
{
#ifdef AVOID_POP_NOISE
	b_mp3_startplay_flg = 0;
	uc_mp3_startplay_cnt = 0;
#endif
	b_lock_flg = 0;
//	uc_lock_level = 0;
//	ui_lock_time = 0;
	b_status_flg = 1;

	b_mute_flg = 0;

	uc_logo_idx = 0;
	uc_logo_cnt = 0;

	b_flicker_flg = 0;
	uc_flicker_cnt = 0;

	b_lrc_file_exist = 0;
	b_mp3_lrc_flg = 0;
	b_mp3_save_flg = 0;
	b_mp3_get_next_file = 0;
	b_mp3_replay_start = 0;
	b_fb_nofile_flg = 1;
//	b_mp3_lrc_freq_flg = 1;
	b_browser_fir_getname_flg = 0;
	b_headphone_plug_flg = 0;
	uc_refresh_menu_num = 0;

	uc_filldata_cnt = 0;
	uc_mpegstate_cnt = 0;

	ul_mp3_replay_a_byte = 0;
	ui_mp3_replay_a_time = 0;
	ui_mp3_replay_b_time = 0;
	uc_mp3_replay_ab_flg = 0;							//0-normal, 1:set A, 2:set B, 3:A-B
	
	ui_mp3_cur_time = 0;
	ui_mp3_played_time = 0;
	ui_mp3_total_time = 0;
	
	uc_mp3_replay_time_cnt = 0;
	uc_mp3_replay_gap_cnt = 0;
	ui_mp3_bitrate = 0;
	
	uc_menu_exit_cnt = 0;
//	uc_mp3_nofile_cnt = 0;
	uc_mp3_time_cnt = 0;
	uc_mp3_ab_fliker_cnt = 0;
//	uc_mp3_volume_fliker_cnt = 0;
	uc_mp3_refresh_id3_cnt = 0;
	uc_mp3_refresh_freq_cnt = 0;
//	uc_mp3_volume_cnt = 0;
	
	uc_dynamicload_idx = 0;
	uc_mp3_dis_offset = 0;
	ui_mp3_tmp_var = 0;
	
	ui_mp3_idx = 0;
	ui_mp3_idx_indir = 0;
	ui_fb_total_num = 0;

	uc_headphone_plug_cnt = 0;
	uc_headphone_unplug_cnt = 0;
//	uc_headphone_short_time = 0;

	uc_mp3_status = STATE_STOP;
	uc_mp3_status_bk = STATE_STOP;
	uc_mpeg_state = STATE_STOP;
	
//	uc_lcd_y_saddr_bk = Menu_FirstStr_Y;			//for display menu firstly
	ui_sys_event &= EVT_USB;
	ui_fs_file_type = FS_FILE_TYPE_MP3;					//set bit0, file type is *.mp3	

//	uc_dir_idx = 0;
	uc_next_action = ACTION_STOP;	
//	ui_play_time_bak = 0;
}


/*******************************************************************************
Function: Ap_Restore_MusicPara()
Description: restore music parameter
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Restore_MusicPara(void)
{
	Api_Read_SysPara_ToDbuf0();

	if((uc_dbuf0[0] == 'S') && (uc_dbuf0[1] == 'I') && (uc_dbuf0[2] == 'N') && (uc_dbuf0[3] == 'O'))
	{
		b_sd_flash_flg = uc_dbuf0[24];

		if(b_sd_flash_flg == 0)
		{
			uc_fs_store_cluster1 = uc_dbuf0[4];					//the saved file addr
			uc_fs_store_cluster2 = uc_dbuf0[5];
			uc_fs_store_length1 = uc_dbuf0[6];					//the saved file length
			uc_fs_store_length2 = uc_dbuf0[7];

			ul_mp3_byte_offset_bk = 0;
			ul_mp3_byte_offset_bk = ((ul32)uc_dbuf0[8]) << 24;	//high
			ul_mp3_byte_offset_bk |= ((ul32)uc_dbuf0[9]) << 16;
			ul_mp3_byte_offset_bk |= ((ul32)uc_dbuf0[10]) << 8;
			ul_mp3_byte_offset_bk |= (ul32)uc_dbuf0[11];		//low

			ui_mp3_played_time = ((ui16)uc_dbuf0[12]) << 8;		//the last saved time
			ui_mp3_played_time |= (ui16)uc_dbuf0[13];
		}
		else
		{
			uc_fs_store_cluster1 = uc_dbuf0[14];				//the saved file addr
			uc_fs_store_cluster2 = uc_dbuf0[15];
			uc_fs_store_length1 = uc_dbuf0[16];					//the saved file length
			uc_fs_store_length2 = uc_dbuf0[17];

			ul_mp3_byte_offset_bk = 0;
			ul_mp3_byte_offset_bk = ((ul32)uc_dbuf0[18]) << 24;	//high
			ul_mp3_byte_offset_bk |= ((ul32)uc_dbuf0[19]) << 16;
			ul_mp3_byte_offset_bk |= ((ul32)uc_dbuf0[20]) << 8;
			ul_mp3_byte_offset_bk |= (ul32)uc_dbuf0[21];		//low

			ui_mp3_played_time = ((ui16)uc_dbuf0[22]) << 8;		//the last saved time
			ui_mp3_played_time |= (ui16)uc_dbuf0[23];	
		}

		uc_mp3_eq_mode = uc_dbuf0[25];						//eq
		uc_mp3_repeat_mode = uc_dbuf0[26];					//repeat

		uc_mp3_replay_time = uc_dbuf0[28];					//A-B replay times
		uc_mp3_replay_gap = uc_dbuf0[29];					//A-B replay gap
		uc_mp3_volume_level = uc_dbuf0[30];					//music volume level

		b_mp3_replay_mode = uc_dbuf0[31] & 0x01;			//replay mode: auto or manual 

	   	uc_disktype_flg = uc_dbuf0[250];
		uc_disktype_flg = 0x00;
		uc_lock_level = uc_dbuf0[261];

		uc_mp3_repeat_mode = REPEAT_ALL;			//repeat
	}
	else
	{
		b_mp3_replay_mode = 0;						//auto replay
		
		uc_fs_store_cluster1 = 0;					//the saved file addr
		uc_fs_store_cluster2 = 0;
		uc_fs_store_length1 = 0;					//the saved file length
		uc_fs_store_length2 = 0;

		ul_mp3_byte_offset_bk = 0;
		ui_mp3_played_time = 0;

		uc_mp3_eq_mode = 0;							//normal eq
		uc_mp3_repeat_mode = REPEAT_ALL;			//repeat

		uc_mp3_replay_time = 10;					//A-B replay times
		uc_mp3_replay_gap = 3;						//A-B replay gap
		uc_mp3_volume_level = 16;					//music volume level
		uc_disktype_flg = 0x00;

		
		b_lock_flg = 0;
	}
}


/*******************************************************************************
Function: Ap_Scan_AllDisk()
Description: 1. get the total mp3 file number
			 2. check the saved mp3 file
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Scan_AllDisk(void)
{
	ui16 i, tmp_dir_idx, tmp_file_idx_indir;

	FS_EnterRootDir();
	tmp_dir_idx = 0;
	ui_fb_total_num = 0;
	ui_mp3_idx_indir = 0;

	while(1)
	{
		FS_GetItemNum_InDir();				//get mp3 file number and folder number in dir
		tmp_file_idx_indir = 0;
		
		for(i = 0; i < ui_fs_file_num_indir; i++)
		{
			FS_Fopen(i);
			if(FS_Chk_FileType() == 1)
			{
				tmp_file_idx_indir++;
				ui_fb_total_num++;
				if(ui_fb_total_num == 1)
				{
					FS_GetFilePath();		//get current file path
					
					si_fs_file_idx = i;
					ui_mp3_idx_indir = 1;
					ui_mp3_idx = 1;
				}

				if((uc_fs_store_cluster1 == uc_fs_begin_cluster1) && (uc_fs_store_cluster2 == uc_fs_begin_cluster2))
				{
					if((uc_fs_store_length1 == (uc8)ul_fs_file_length) && (uc_fs_store_length2 == (uc8)(ul_fs_file_length>>8)))
					{
						FS_GetFilePath();	//get current file path
						
						si_fs_file_idx = i;
						ui_mp3_idx_indir = tmp_file_idx_indir;
						ui_mp3_idx = ui_fb_total_num;
					}
				}
			}
		}

Label_IntoDir_Again:	
		if(FS_EnterSubDir(tmp_dir_idx) == 1)
		{
			tmp_dir_idx = 0;
		}
		else
		{
			tmp_dir_idx = FS_ExitSubDir();
			if(tmp_dir_idx == 0xffff)		//already it's ROOT
			{
				break;
			}
			
			tmp_dir_idx++;
			goto Label_IntoDir_Again;
		}
	}
	
	if(ui_fb_total_num == 0)
	{
		b_fb_nofile_flg = 1;
	}
	else
	{
		b_fb_nofile_flg = 0;
	}	
}

#endif