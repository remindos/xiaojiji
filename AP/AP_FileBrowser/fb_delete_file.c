/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: fb_delete_file.c
Creator: zj.zong					Date: 2009-05-07
Description: delete one file or all files
Others:
Version: V0.1
History:
	V0.1	2009-05-07		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef FB_DELETE_FILE_DEF
#include "sh86278_sfr_reg.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"

#ifdef AP_VOICE_DELEATE_FILE_DEF
	#include "..\..\UI\UI_Voice\ui_voice_header.h"
	#include "..\..\UI\UI_Voice\ui_voice_var_ext.h"
#endif
#ifdef AP_MUSIC_DELETE_FILE
	#include "..\..\UI\UI_Music\ui_music_header.h"
	#include "..\..\UI\UI_Music\ui_music_var_ext.h"
#endif
#ifdef AP_EBOOK_DEL_FILE
	#include "..\..\UI\UI_Ebook\ui_ebook_header.h"
	#include "..\..\UI\UI_Ebook\ui_ebook_var_ext.h"
#endif
extern void FS_Fdelete(void);
extern bit	FS_Fopen_Appointed(ui16 file_idx);


void Fb_Delete_OneFile(void);
void Fb_Delete_AllFile(void);

#ifdef AP_MUSIC_DELETE_FILE
void Ap_Check_File_Idx(void);
#endif
#ifdef AP_EBOOK_DEL_FILE
void Ap_Check_ebook_nofile_Idx(void);
#endif

void Fb_DeleteFile(void)
{
	switch(uc_dynamicload_idx)
	{
		case 20:
			Fb_Delete_OneFile();
			break;

		case 21:
			Fb_Delete_AllFile();
			break;
	}

	uc_dynamicload_idx = 0;
}


/*******************************************************************************
Function: Fb_Delete_File()
Description: delete one file
			 (file borwser--fb)
Calls:
Global:ui_fs_file_num_indir,ui_fs_special_file_num_indir
Input: Null
Output:Null
Others:
********************************************************************************/
void Fb_Delete_OneFile(void)
{
#ifndef AP_VOICE_DELEATE_FILE_DEF
	ui16 tmp_fs_item_num_indir;
#endif	
	FS_Fdelete();

	ui_fs_file_num_indir--;
	ui_fs_special_file_num_indir--;
	ui_fb_total_num--;

#ifndef AP_VOICE_DELEATE_FILE_DEF
	#ifdef AP_MUSIC_DELETE_FILE
		Ap_Check_File_Idx();
	 #endif
	#ifdef AP_EBOOK_DEL_FILE
		Ap_Check_ebook_nofile_Idx();
	#endif
		tmp_fs_item_num_indir = ui_fs_subdir_num + ui_fs_special_file_num_indir;
		if(uc_fs_subdir_stk_pt > 0)	
		{
			tmp_fs_item_num_indir++;			//has "dummy folder" in Sub Dir
		}
		
		if(ui_fb_sel_item_idx >= tmp_fs_item_num_indir)
		{
			ui_fb_sel_item_idx--;
			if(ui_fb_top_item_idx > 0)
			{
				ui_fb_top_item_idx--;
			}
		}
		
		if(tmp_fs_item_num_indir <= Dis_BrowserItem_MaxNum)
		{
			ui_fb_top_item_idx = 0;
		}
		else
		{
			if((tmp_fs_item_num_indir - ui_fb_top_item_idx) < Dis_BrowserItem_MaxNum)
			{
				if(ui_fb_top_item_idx > 0)
				{
					ui_fb_top_item_idx--;
				}
			}
		}
#else
		if(ui_fb_total_num == 0)
		{
			ui_voice_wav_idx = 0;
			b_fb_nofile_flg = 1;						//no wav file
			return;
		}
		if(ui_voice_wav_idx > ui_fb_total_num)	//if deleted file is the last wav
		{
			ui_voice_wav_idx = ui_fb_total_num;
		}
		FS_Fopen_Appointed(ui_voice_wav_idx - 1);	

#endif

}



/*******************************************************************************
Function: Fb_Delete_AllFile()
Description: delete all file
Calls:
Global:ui_fb_total_num, ui_fs_file_num_indir
Input: Null
Output:Null
Others:
*******************************************************************************/
void Fb_Delete_AllFile(void)
{
	while(ui_fs_special_file_num_indir != 0)
	{
		FS_Fopen_Appointed(0);
		FS_Fdelete();
		ui_fs_file_num_indir--;
		ui_fs_special_file_num_indir--;
		ui_fb_total_num--;
	}
	#ifdef AP_VOICE_DELEATE_FILE_DEF
		ui_voice_wav_idx = 0;
		ui_fb_total_num = 0;
		b_fb_nofile_flg = 1;
	#endif
	#ifdef AP_MUSIC_DELETE_FILE
		Ap_Check_File_Idx();
	#endif
	#ifdef AP_EBOOK_DEL_FILE
		Ap_Check_ebook_nofile_Idx();
	#endif

}

#ifdef AP_MUSIC_DELETE_FILE
void Ap_Check_File_Idx(void)
{
	if(ui_mp3_idx > ui_fb_total_num)
	{
		ui_mp3_idx = ui_fb_total_num;
	}
	
	if(ui_fb_total_num == 0)
	{
		b_fb_nofile_flg = 1;			//no file
	}
}
#endif

#ifdef AP_EBOOK_DEL_FILE
void Ap_Check_ebook_nofile_Idx(void)
{

	if(ui_fb_total_num == 0)
	{
		b_ebook_nofile_flg = 1;			//no file
	}
}
#endif
#endif