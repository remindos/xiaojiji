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
#ifdef AP_VOICE_INIT2_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Voice\ui_voice_header.h"
#include "..\..\UI\UI_Voice\ui_voice_var_ext.h"

extern void FS_EnterRootDir(void);
extern bit  FS_EnterSubDir(ui16 dir_index);
extern void FS_GetItemNum_InDir(void);
extern bit FS_Fopen(ui16 file_idx);
extern bit FS_Fopen_Appointed(ui16 file_idx);
extern bit FS_Chk_FileType(void);  
extern void Ap_Enable_DAC(void);
extern void Api_Set_Volume(uc8 uc_voice_volume_level);
extern void Api_Delay_1ms(uc8 timer_cnt);
extern bit FS_Get_VoiceDir_Idx(void);

void Ap_Get_WavNumber(void);
/*******************************************************************************
Function: Ap_VoiceInit2()
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
void Ap_VoiceInit2(void)
{  

	mSet_MCUClock_33M;
	Ap_Get_WavNumber();
 	mSet_MCUClock_12M;

	if(ui_fb_total_num != 0)
	{
		//Ap_Enable_DAC();								 //init dac
		
		FS_Fopen_Appointed(ui_voice_wav_idx - 1);//open the current file
	}

	uc_save_mode_idx = MODE_3_VOICE;
	uc_window_no = WINDOW_VOICE;				//set voice window
	uc_lcd_dis_flg = 0xfd;						//display all icon	bit2 hold
	b_lcd_clrall_flg = 1;						//clr all page flag
	ui_sys_event |= EVT_DISPLAY;				//Set the display event
}


void Ap_Get_WavNumber(void)
{
	ui16 i, tmp_file_idx_indir = 0;
	
	if(b_voice_folder_exist == 1)				//search "VOICE" folder
	{													
		FS_EnterRootDir();						//enter "ROOT"
		FS_EnterSubDir(ui_fs_wav_dir_idx);		//enter "VOICE"
		FS_GetItemNum_InDir();					//Get wav file number

		ui_fb_total_num = ui_fs_special_file_num_indir;
		if(ui_fb_total_num > 0)
		{
			ui_voice_wav_idx = 1;
			b_fb_nofile_flg=0;				//first	file
		}
		else
		{
		b_fb_nofile_flg=1;
		}
		
		for(i = 0; i < ui_fs_file_num_indir; i++)
		{
			FS_Fopen(i);						//open file
			if(FS_Chk_FileType() == 1)			//check file type
			{
				tmp_file_idx_indir++;
				si_fs_file_idx = i;
				
				if((uc_fs_store_cluster1 == uc_fs_begin_cluster1) && (uc_fs_store_cluster2 == uc_fs_begin_cluster2))	//compare file para
				{
					if((uc_fs_store_length1 == (uc8)ul_fs_file_length) && (uc_fs_store_length2 == (uc8)(ul_fs_file_length>>8)))
					{
						ui_voice_wav_idx = tmp_file_idx_indir;	//saved file
					}
				}
			}
		}
	}
}


#endif