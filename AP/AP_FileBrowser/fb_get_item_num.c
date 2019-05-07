/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: fb_get_item_num.c
Creator: zj.zong			Date: 2009-05-07
Description: get item number in dir(include file number, folder number)
Others:
Version: V0.1
History:
	V0.1	2009-05-07		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef FB_GET_ITEM_NUM_DEF

#include "sh86278_sfr_reg.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"

#ifdef AP_MUSIC_GET_ITEM_NUM
	#include "..\..\UI\UI_Music\ui_music_header.h"
	#include "..\..\UI\UI_Music\ui_music_var_ext.h"
#endif
#ifdef AP_EBOOK_GET_ITEM_NUM
	#include "..\..\UI\UI_Ebook\ui_ebook_header.h"
	#include "..\..\UI\UI_Ebook\ui_ebook_var_ext.h"
#endif
extern bit FS_EnterSubDir(ui16 dir_index);
extern ui16 FS_ExitSubDir(void);
extern void FS_GetItemNum_InDir(void);


/*******************************************************************************
Function: Fb_GetItemNumber_InDir()
Description: enter or exit dir, and get item number in current dir,
             include file number, folder number
			 (file browser---fb)
Calls:
Global:uc_dynamicload_idx(10--enter dir; 11--exit dir)
       ui_fb_sel_item_idx, ui_fb_top_item_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Fb_GetItemNumber_InDir(void)					//input is uc_dynamicload_idx
{
	ui16 tmp_sel_item_idx;

	switch(uc_dynamicload_idx)
	{
		case 10:
			tmp_sel_item_idx = ui_fb_sel_item_idx;

			if(uc_fs_subdir_stk_pt > 0)					//uc_fs_subdir_stk_pt: 0--ROOT, Others--Sub dir
			{
				tmp_sel_item_idx--;						//have "dummy folder" in SUB DIR
			}

			if(FS_EnterSubDir(tmp_sel_item_idx) == 0)
			{
				goto EnterSubdir_Err;
			}

			FS_GetItemNum_InDir();						//get item num in current dir
EnterSubdir_Err:
			ui_fb_top_item_idx = 0;						//init the ui_browser_top_item_idx
			ui_fb_sel_item_idx = 0;
			break;

		case 11:
			ui_fb_sel_item_idx = FS_ExitSubDir();		//get the current folder index
			if(uc_fs_subdir_stk_pt > 0)
			{
				ui_fb_sel_item_idx++;					//have one "dummy folder" except ROOT
			}
		
			FS_GetItemNum_InDir();						//get the total item number in current 

			if(ui_fb_sel_item_idx < Dis_BrowserItem_MaxNum)
			{
				ui_fb_top_item_idx = 0;
			}
			else
			{
				ui_fb_top_item_idx = ui_fb_sel_item_idx - Dis_BrowserItem_MaxNum + 1;
			}
			break;

	}
	
	uc_dynamicload_idx = 0;
}


#endif