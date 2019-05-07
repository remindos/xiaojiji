/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: fb_get_item_name.c
Creator: zj.zong					Date: 2009-06-11
Description: get item name in dir
Others:
Version: V0.1
History:
	V0.1	2009-06-11		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef FB_GET_ITEM_NAME_DEF

#include "sh86278_sfr_reg.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#ifdef AP_MUSIC_GET_ITEM_NAME
	#include "..\..\UI\UI_Music\ui_music_header.h"
	#include "..\..\UI\UI_Music\ui_music_var_ext.h"
#endif
#ifdef AP_EBOOK_GET_ITEM_NAME
	#include "..\..\UI\UI_Ebook\ui_ebook_header.h"
	#include "..\..\UI\UI_Ebook\ui_ebook_var_ext.h"
#endif
#ifdef AP_MUSIC_GET_ITEM_NAME
extern void	Ap_Check_LrcFile_Existed(void);
#endif	

extern bit	FS_EnterSubDir(ui16 dir_index);
extern bit	FS_OpenDir(ui16 dir_index);
extern bit	FS_Fopen_Appointed(ui16 file_idx);
extern bit	FS_Get_LongName(void);

extern void FS_Get_83Name(void);
extern void FS_Get_NextFile_InDir(void);

extern ui16 FS_ExitSubDir(void);
extern ui16 Api_Merge_2Byte_Sub(uc8 lo,uc8 hi);
extern ui16 Api_Isn_ConvertTo_Unicode(ui16 isn_index);

void Ap_Process_83Name(void);

/*******************************************************************************
Function: Fb_GetItemName_InDir()
Description: enter or exit dir, and get item number in current dir,
             include file number, folder number
			 (file browser---fb)
Calls:
Global:uc_dynamicload_idx(1--enter dir; 2--exit dir)
       ui_fb_sel_item_idx, ui_fb_top_item_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Fb_GetItemName_InDir(void)
{
	switch(uc_dynamicload_idx)
	{
		case 1:	 //1. get mp3 (ebook) file name in file browser mode;
			
			ui_fb_cur_item_idx -= ui_fs_subdir_num;
		
			if(b_browser_fir_getname_flg == 0)
			{
				b_browser_fir_getname_flg = 1;
				FS_Fopen_Appointed(ui_fb_cur_item_idx);
			}
			else
			{
				FS_Get_NextFile_InDir();
			}
		
			if(FS_Get_LongName() == 0)
			{
				FS_Get_83Name();					//rts is uc_fs_item_83_name
				Ap_Process_83Name();                                      
			}
			break;

		case 2:	 //2. get music (ebook) folder name in file browser mode;
			FS_OpenDir(ui_fb_cur_item_idx);
get_dir_name:		
			if(FS_Get_LongName() == 0)
			{
				FS_Get_83Name();					//rts is uc_fs_item_83_name
				uc_fs_item_83_name[11] = 0x20;		//add "space"
				Ap_Process_83Name();
			}
			break;

		case 3:	 //3.get mp3 file name in music play mode
			if(FS_Get_LongName() == 0)
			{
				FS_Get_83Name();					//rts is uc_fs_item_83_name
				Ap_Process_83Name();                                    
			}
			#ifdef AP_MUSIC_GET_ITEM_NAME 
				Ap_Check_LrcFile_Existed();
			#endif
			break;

		case 4:
			FS_EnterSubDir(FS_ExitSubDir());
			goto get_dir_name;
			break;
	}

	uc_dynamicload_idx = 0;
}


/*******************************************************************************
Function: Ap_Process_83Name()
Description: process 8.3 format item name, and convert isn into unicode
Calls:
Global:uc_fs_item_83_name[]
       ui_fs_item_name[]:  current item name(unicode)
       uc_fs_item_name_length: current item name length
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Process_83Name(void)
{
	uc8 i;
	ui16 tmp_unicode;

	uc_fs_item_name_length = 0;				//init the file length = 0

	if(uc_fs_item_83_name[8] != 0x20)		//check mp3 or wav file
	{
		uc_fs_item_83_name[11] = uc_fs_item_83_name[10];
		uc_fs_item_83_name[10] = uc_fs_item_83_name[9];
		uc_fs_item_83_name[9] = uc_fs_item_83_name[8];
		uc_fs_item_83_name[8] = '.';		//add "."
	}
	
	for(i = 0; i < 12; i++)
	{
		if(uc_fs_item_83_name[i] != 0x20)	//check is "space"
		{
			tmp_unicode = Api_Isn_ConvertTo_Unicode(Api_Merge_2Byte_Sub(uc_fs_item_83_name[i], 0x00));
			if(tmp_unicode == 0)
			{
				tmp_unicode = Api_Isn_ConvertTo_Unicode(Api_Merge_2Byte_Sub(uc_fs_item_83_name[i+1], uc_fs_item_83_name[i]));
				i++;
			}

			ui_fs_item_name[uc_fs_item_name_length] = tmp_unicode;
			uc_fs_item_name_length++;
		}
	}
}


#endif