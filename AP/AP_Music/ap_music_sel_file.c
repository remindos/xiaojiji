/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_music_sel_file.c
Creator: zj.zong					Date: 2009-04-29
Description: select next file or previous file
Others:
Version: V0.1
History:
	V0.1	2009-04-27		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_MUSIC_SELECT_FILE_DEF

#include "sh86278_sfr_reg.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "..\..\UI\UI_Music\ui_music_header.h"
#include "..\..\UI\UI_Music\ui_music_var_ext.h"
#include <stdlib.h>

extern ui16 FS_ExitSubDir(void);
extern bit FS_Fread(void);
extern bit FS_Chk_FileType(void);
extern bit FS_Fopen(ui16 file_idx);
extern bit FS_Fopen_Appointed(ui16 file_idx);
extern bit FS_EnterSubDir(ui16 dir_index);
extern void FS_EnterRootDir(void);
extern void FS_GetItemNum_InDir(void);
extern void FS_Get_NextFile_InDir(void);

ui16 Ap_GetRandom(void);
void Ap_Sel_NextFile(void);
void Ap_Sel_PreviousFile(void);
void Ap_Get_Next_PlayFile(void);
void Ap_Get_FileIdx_InDisk(void);
void Ap_Get_File_From_MP3Idx(void);
void Ap_Open_OppointedFile(void);
void Ap_Get_Valid_Idx(void);

ui16 tmp_i, tmp_dir_idx, tmp_mp3_idx;

/*******************************************************************************
Function: Ap_Sel_File()
Description: select next file or previous file
Calls:
Global:uc_dynamicload_idx:41--sel next file;
						  42--sel previous file; 
						  43--file end; 
						  44--get file index in disk
						  45--get the saved mp3 idx, and for continue playing
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Sel_File(void)
{
	switch(uc_dynamicload_idx)
	{
		case 41:	  //sel next music file
			b_mp3_get_next_file = 1;

			Ap_Sel_NextFile();
			goto clr_fs_store_para;
			break;

	 	case 42:	  //sel previous
			b_mp3_get_next_file = 1;

			Ap_Sel_PreviousFile();
			goto clr_fs_store_para;
			break;

		case 43:	  //file end
			b_mp3_get_next_file = 1;

			Ap_Get_Next_PlayFile();
			goto clr_fs_store_para;
			break;

		case 44:	  //get current file index in disk
			b_mp3_get_next_file = 1;
		
			Ap_Get_FileIdx_InDisk();			
		clr_fs_store_para:
				uc_fs_store_cluster1 = 0;		
				uc_fs_store_cluster2 = 0;
				uc_fs_store_length1 = 0;
				uc_fs_store_length2	= 0;
			break;

	 	case 45:	  //global is ui_mp3_idx
			b_mp3_get_next_file = 1;
		
			Ap_Get_File_From_MP3Idx();		
			goto not_clr_para;
			break;
		
		case 46:
			Ap_Open_OppointedFile();
			goto not_clr_para;
			break;

		case 47:
			Ap_Get_Valid_Idx();
			Ap_Open_OppointedFile();
			goto not_clr_para;
			break;
	}
	ul_mp3_byte_offset_bk = 0;
	ui_mp3_played_time = 0;
not_clr_para:
	uc_dynamicload_idx = 0;
}



void Ap_Open_OppointedFile(void)
{
	FS_Fopen_Appointed(ui_mp3_idx_indir - 1);
}



void Ap_Sel_NextFile(void)
{
//	if(uc_mp3_repeat_mode != REPEAT_RANDOM)
//	{
		if((uc_mp3_repeat_mode == REPEAT_FOLDER_NOR) || (uc_mp3_repeat_mode == REPEAT_FOLDER))
		{
			if(ui_mp3_idx_indir < ui_fs_special_file_num_indir)	//calculate the file index in cur dir
			{
				ui_mp3_idx_indir++;
				ui_mp3_idx++;
			}
			else
			{
				ui_mp3_idx_indir = 1;
				ui_mp3_idx = ui_mp3_idx - ui_fs_special_file_num_indir + 1;
			}
			
			FS_Get_NextFile_InDir();
		}
		else									//repeat in disk
		{			
			if(ui_mp3_idx < ui_fb_total_num)	//calculate the file index in disk
			{
				ui_mp3_idx++;
			}
			else
			{
				ui_mp3_idx = 1;
			}
			Ap_Get_File_From_MP3Idx();
		}
//	}
//	else
//	{
//		ui_mp3_idx = Ap_GetRandom();
//		Ap_Get_File_From_MP3Idx();
//	}
}



void Ap_Sel_PreviousFile(void)
{
//	if(uc_mp3_repeat_mode != REPEAT_RANDOM)
//	{
		if((uc_mp3_repeat_mode == REPEAT_FOLDER_NOR) || (uc_mp3_repeat_mode == REPEAT_FOLDER))
		{
			if(ui_mp3_idx_indir == 1)	//calculate the file index in cur dir
			{
				ui_mp3_idx_indir = ui_fs_special_file_num_indir;
				ui_mp3_idx = ui_mp3_idx + ui_fs_special_file_num_indir - 1;
			}
			else
			{
				ui_mp3_idx_indir--;
				ui_mp3_idx--;
			}
			
			FS_Fopen_Appointed(ui_mp3_idx_indir - 1);
		}
		else							//repeat in disk
		{
			if(ui_mp3_idx == 1)			//calculate the file index in disk
			{
				ui_mp3_idx = ui_fb_total_num;
			}
			else
			{
				ui_mp3_idx--;
			}
			Ap_Get_File_From_MP3Idx();
		}
//	}
//	else
//	{
//		ui_mp3_idx = Ap_GetRandom();
//		Ap_Get_File_From_MP3Idx();
//	}
}



void Ap_Get_FileIdx_InDisk(void)
{
	uc_fs_store_cluster1 = uc_fs_begin_cluster1;	//save current file parameter
	uc_fs_store_cluster2 = uc_fs_begin_cluster2;
	uc_fs_store_length1 = (uc8)ul_fs_file_length;
	uc_fs_store_length2 = (uc8)(ul_fs_file_length >> 8);
	
	FS_EnterRootDir();
	tmp_mp3_idx = 0;
	tmp_dir_idx = 0;
	
	while(1)
	{
		FS_GetItemNum_InDir();		//get mp3 file number and folder number in dir
		
		for(tmp_i = 0; tmp_i < ui_fs_file_num_indir; tmp_i++)
		{
			FS_Fopen(tmp_i);
			if(FS_Chk_FileType() == 1)
			{
				tmp_mp3_idx++;
				
				if((uc_fs_store_cluster1 == uc_fs_begin_cluster1) && (uc_fs_store_cluster2 == uc_fs_begin_cluster2))
				{
					if((uc_fs_store_length1 == (uc8)ul_fs_file_length) && (uc_fs_store_length2 == (uc8)(ul_fs_file_length>>8)))
					{
						si_fs_file_idx = tmp_i;
						ui_mp3_idx = tmp_mp3_idx;
						return;
					}
				}
			}
		}

into_dir_again:	
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
			goto into_dir_again;
		}
	}
}




/*******************************************************************************
Function: Ap_Get_Next_PlayFile()
Description: if file end, and get next file
Calls:
Global:b_mp3_get_next_file: 1--ok; 0--error
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Get_Next_PlayFile(void)
{
	if(uc_mp3_repeat_mode == REPEAT_ONE)	//repeat one
	{
		if(b_mp3_file_err == 0)
		{
			FS_Fopen_Appointed(ui_mp3_idx_indir - 1);//open current file
		}
		else								//current mp3 file is error
		{
			goto get_next_file;
		}
	}
 /*
	else if((uc_mp3_repeat_mode == REPEAT_NORMAL) || (uc_mp3_repeat_mode == REPEAT_PREVIEW))
	{
		if(ui_mp3_idx_indir < ui_fs_special_file_num_indir)
		{
			ui_mp3_idx_indir++;
		}
		else
		{
			ui_mp3_idx_indir = 1;
		}
		
		if(ui_mp3_idx < ui_fb_total_num)	//calculate the file index in disk
		{
			ui_mp3_idx++;
		}
		else
		{
			ui_mp3_idx = 1;
			b_mp3_get_next_file = 0;
		}
		
		Ap_Get_File_From_MP3Idx();
	}
	
	else if(uc_mp3_repeat_mode == REPEAT_FOLDER_NOR)
	{
		if(ui_mp3_idx_indir < ui_fs_special_file_num_indir)
		{
			ui_mp3_idx_indir++;
			ui_mp3_idx++;					//index + 1
		}
		else
		{
			ui_mp3_idx_indir = 1;
			ui_mp3_idx = ui_mp3_idx - ui_fs_special_file_num_indir + 1;
			b_mp3_get_next_file = 0;
		}
		
		FS_Get_NextFile_InDir();
	}
	


	else if(uc_mp3_repeat_mode == REPEAT_RANDOM)
	{
		ui_mp3_idx = Ap_GetRandom();
		Ap_Get_File_From_MP3Idx();
	}   */
	else //if(uc_mp3_repeat_mode == REPEAT_ALL)
	{
get_next_file:
		if(ui_mp3_idx_indir < ui_fs_special_file_num_indir)			//index in cur dir
		{
			ui_mp3_idx_indir++;
		}
		else
		{
			ui_mp3_idx_indir = 1;
		}
		
		if(ui_mp3_idx < ui_fb_total_num)	//index all file
		{
			ui_mp3_idx++;
		}
		else
		{
			ui_mp3_idx = 1;
		}
		
		Ap_Get_File_From_MP3Idx();
	}
/*	else if(uc_mp3_repeat_mode == REPEAT_FOLDER)	//if(uc_mp3_repeat_mode == REPEAT_FOLDER)
	{
		if(ui_mp3_idx_indir < ui_fs_special_file_num_indir)	//index in cur dir
		{
			ui_mp3_idx_indir++;
			ui_mp3_idx++;
		}
		else
		{
			ui_mp3_idx_indir = 1;
			ui_mp3_idx = ui_mp3_idx - ui_fs_special_file_num_indir + 1;
		}

		FS_Get_NextFile_InDir();
	} */
}



/*******************************************************************************
Function: Ap_Get_File_From_MP3Idx()
Description: 1. get current mp3 file according to the ui_mp3_idx,
             2. calculate the mp3 index in dir
Calls:
Global:ui_mp3_idx, si_fs_file_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Get_File_From_MP3Idx(void)
{	
	tmp_mp3_idx = 0;
	tmp_dir_idx = 0;
	FS_EnterRootDir();

	while(1)
	{
		ui_mp3_idx_indir = 0;
		FS_GetItemNum_InDir();		//get mp3 file number and folder number in dir
		
		for(tmp_i = 0; tmp_i < ui_fs_file_num_indir; tmp_i++)
		{
			FS_Fopen(tmp_i);
			if(FS_Chk_FileType() == 1)
			{
				tmp_mp3_idx++;
				ui_mp3_idx_indir++;
				if(tmp_mp3_idx == ui_mp3_idx)
				{
					si_fs_file_idx = tmp_i;
					return;
				}
			}
		}
		
into_dir_again:	
		if(FS_EnterSubDir(tmp_dir_idx) == 1)
		{
			tmp_dir_idx = 0;
		}
		else
		{
			tmp_dir_idx = FS_ExitSubDir();
			if(tmp_dir_idx == 0xffff)	//already it's ROOT
			{
				break;
			}
			
			tmp_dir_idx++;
			goto into_dir_again;
		}
	}
}




void Ap_Get_Valid_Idx(void)
{
	b_mp3_save_flg = 0;				//clr save flag
	b_mp3_restore_file_flg = 0;

	ui_mp3_tmp_var = ui_fb_sel_item_idx - ui_fs_subdir_num;
	if(uc_fs_subdir_stk_pt > 0)		//subdir have "dummy folder"
	{
		ui_mp3_tmp_var--;
	}
	ui_mp3_idx_indir = ui_mp3_tmp_var + 1;
}


#endif

