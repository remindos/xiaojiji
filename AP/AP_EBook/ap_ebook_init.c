/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_ebook_init.c
Creator: zj.zong			Date: 2009-05-05
Description: init ebook parameter
Others:
Version: V0.1
History:
	V0.1	2009-05-05		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_EBOOK_INIT_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_EBook\ui_ebook_header.h"
#include "..\..\UI\UI_EBook\ui_ebook_var_ext.h"

extern bit FS_EnterSubDir(ui16 dir_index);
extern void FS_GetItemNum_InDir(void);
extern void FS_EnterRootDir(void);
extern void FS_EnterDir_Appointed(void);
extern void FS_GetFilePath(void);
extern bit FS_Fopen(ui16 file_idx);
extern bit FS_Chk_FileType(void);
extern ui16 FS_ExitSubDir(void);

extern void _media_readsector();

void Ap_Init_EbookVar(void);
void Ap_RestoreEbook_Para(void);
void Ap_Scan_AllDisk(void);
void Ap_Get_SelFile(void);
void Ap_Init_Ebook_Display(void);
/*******************************************************************************
Function: Ap_EbookInit()
Description: init ebook parameter
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_EbookInit(void)
{
	switch(uc_dynamicload_idx)
	{
		case 40:
			Ap_Init_EbookVar();
			Ap_RestoreEbook_Para();
			break;
		case 41:
	        mSet_MCUClock_33M;
        	Ap_Scan_AllDisk();
        	mSet_MCUClock_12M;

	        if(ui_fb_total_num != 0)
        	{
	     	FS_EnterDir_Appointed();
	     	FS_GetItemNum_InDir();							//get the ebook number in cur dir
        	}
			Ap_Init_Ebook_Display();

			break;
     }
	 uc_dynamicload_idx=0;
 }
 /*******************************************************************************
Ap_Init_Ebook_Display
********************************************************************************/
 void Ap_Init_Ebook_Display(void)
         {  
		    uc_save_mode_idx = MODE_6_EBOOK;
			uc_menu_top_idx = 0;
		   if(b_sd_exist_flg == 0)
               {
			   uc_window_no =WINDOW_EBOOK_MENU_FLASH ;//set ebook browser window
			   uc_menu_cur_idx=1;
			   uc_menu_total_num=3;
	           }
		   else
		       {
		       uc_window_no = WINDOW_EBOOK_MENU_SD;	 //set ebook browser window
			   uc_menu_total_num=4;
			   if(b_sd_flash_flg==1)
			   {uc_menu_cur_idx=1;
			   }
			   else
			   {uc_menu_cur_idx=2;
			   }
		     }
		    uc_lcd_dis_flg = 0xfd;				//display all icon
          	b_lcd_clrall_flg = 1;				//clr all page flag
            ui_sys_event |= EVT_DISPLAY;		//Set the display event
			
		 }
/*******************************************************************************
init variable
********************************************************************************/
void Ap_Init_EbookVar(void)
{
	b_ebook_file_end = 0;
	b_ebook_nofile_flg = 0;
	b_browser_fir_getname_flg = 0;
	b_sd_exist_flg = 0;
	
	uc_fb_sel_item_type = 0;
	uc_ebook_play_gap_cnt = 0;
	ui_fb_cur_item_idx = 0;
	uc_menu_exit_cnt = 0;
	uc_dynamicload_idx = 0;
	ui_fb_total_num = 0;
	uc_ebook_play_mode = 1;
	ui_sys_event &= EVT_USB;
	uc_refresh_menu_num = 0;

	ui_fb_sel_item_idx = 0;
	ui_fb_top_item_idx = 0;
	ui_fs_file_type = FS_FILE_TYPE_TXT;		//set bit6, file type is *.txt
}



/*******************************************************************************
restore ebook parameter
********************************************************************************/
void Ap_RestoreEbook_Para(void)
{
	sfc_logblkaddr0 = 0x02;							//0x0000 0202
	sfc_logblkaddr1 = 0x02;
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
	sfc_occ_buf_flag = SFC_APP_DBUF0;				//occupy dbuf(0x4000)
	sfc_reserved_area_flag = 1;						//read flash reserved area
	buf_offset0 = 0x00;								//buffer offset :0x0000
	buf_offset1 = 0x00;
	_media_readsector();							//read one sector
	sfc_reserved_area_flag = 0;					//clr reserved area flag

	if((uc_dbuf0[0] == 'S') && (uc_dbuf0[1] == 'I') && (uc_dbuf0[2] == 'N') && (uc_dbuf0[3] == 'O'))
	{

		uc_ebook_mark1_cluster1 = uc_dbuf0[250];			//bookmark 1
		uc_ebook_mark1_cluster2 = uc_dbuf0[251];
		uc_ebook_mark1_length1 = uc_dbuf0[252];
		uc_ebook_mark1_length2 = uc_dbuf0[253];				//bookmark1 cluster and length
		ui_ebook_mark1 = 0;
		ui_ebook_mark1 = ((ui16)uc_dbuf0[254]) << 8;
		ui_ebook_mark1 |= (ui16)uc_dbuf0[255];				//bookmark1 page index
			
		uc_ebook_mark2_cluster1 = uc_dbuf0[256];			//bookmark 2
		uc_ebook_mark2_cluster2 = uc_dbuf0[257];
		uc_ebook_mark2_length1 = uc_dbuf0[258];
		uc_ebook_mark2_length2 = uc_dbuf0[259];				//bookmark2 cluster and length	
		ui_ebook_mark2 = 0;
		ui_ebook_mark2 = ((ui16)uc_dbuf0[260]) << 8;
		ui_ebook_mark2 |= (ui16)uc_dbuf0[261];				//bookmark2 page index

		uc_ebook_mark3_cluster1 = uc_dbuf0[262];			//bookmark 3
		uc_ebook_mark3_cluster2 = uc_dbuf0[263];
		uc_ebook_mark3_length1 = uc_dbuf0[264];
		uc_ebook_mark3_length2 = uc_dbuf0[265];				//bookmark3 cluster and length
		ui_ebook_mark3 = 0;
		ui_ebook_mark3 = ((ui16)uc_dbuf0[266]) << 8;
		ui_ebook_mark3 |= (ui16)uc_dbuf0[267];				//bookmark3 page index

		b_sd_flash_flg = uc_dbuf0[268];				 //save flash-sd
		uc_ebook_play_mode = uc_dbuf0[269];			 // 1:auto play, 0:manua
		uc_ebook_play_gap = uc_dbuf0[270];					// play gap 2s			    l
		uc_ebookmark_flag = uc_dbuf0[271];
	}
	else                									// read parameter error
	{
	    b_sd_flash_flg = 0;
	 	uc_ebook_play_mode = 1;
		uc_ebook_play_gap = 2;

		uc_ebook_mark1_cluster1 = 0;							//bookmark1 is 0 			
		uc_ebook_mark1_cluster2 = 0;
		uc_ebook_mark1_length1 = 0;
		uc_ebook_mark1_length2 = 0;
		ui_ebook_mark1 = 0;
		uc_ebook_mark2_cluster1 = 0;						   	//bookmark2 is 0 	
		uc_ebook_mark2_cluster2 = 0;
		uc_ebook_mark2_length1 = 0;
		uc_ebook_mark2_length2 = 0;
		ui_ebook_mark2 = 0;
		uc_ebook_mark3_cluster1 = 0;						   	//bookmark3 is 0 	
		uc_ebook_mark3_cluster2 = 0;
		uc_ebook_mark3_length1 = 0;
		uc_ebook_mark3_length2 = 0;
		ui_ebook_mark3 = 0;
	}

}

 /*******************************************************************************
scan_AllDisk
********************************************************************************/
void Ap_Scan_AllDisk(void)
{
	ui16 i, tmp_dir_idx;

	FS_EnterRootDir();					//enter root dir
	tmp_dir_idx = 0;
	ui_fb_total_num=0;

	while(1)
	{
		FS_GetItemNum_InDir();			//get mp3 file number and folder number in dir
		tmp_file_idx_indir = 0;
		
		for(i = 0; i < ui_fs_file_num_indir; i++)
		{
Label_Check_Next:
			FS_Fopen(i);
			if(FS_Chk_FileType() == 1)
			{
				tmp_file_idx_indir++;
				ui_fb_total_num++;

				if (tmp_file_idx_indir == 1)
				{
				 	Ap_Get_SelFile();				//first file
				}
				if((uc_ebook_mark1_cluster1 == uc_fs_begin_cluster1) && (uc_ebook_mark1_cluster2 == uc_fs_begin_cluster2))
				{
					if((uc_ebook_mark1_length1 == (uc8)ul_fs_file_length) && (uc_ebook_mark1_length2 == (uc8)(ul_fs_file_length>>8)))
					{
						Ap_Get_SelFile();		  			//get bookmark1 saved file
						if((uc_ebookmark_flag & 0x08)!= 0)	//judge last saved bookmark is bookmark1
						{
						 	return;
						} 
						goto Label_Check_Next;		   //file claster address and length equal but file not last save
					}
				}
				if((uc_ebook_mark2_cluster1 == uc_fs_begin_cluster1) && (uc_ebook_mark2_cluster2 == uc_fs_begin_cluster2))
				{
					if((uc_ebook_mark2_length1 == (uc8)ul_fs_file_length) && (uc_ebook_mark2_length2 == (uc8)(ul_fs_file_length>>8)))
					{
						Ap_Get_SelFile();					//get bookmark2 saved file
						if((uc_ebookmark_flag & 0x10)!= 0)	//judge last saved bookmark is bookmark2
						{
						 	return;
						}
						goto Label_Check_Next;		   //file claster address and length equal but file not last save
					}
				}
				if((uc_ebook_mark3_cluster1 == uc_fs_begin_cluster1) && (uc_ebook_mark3_cluster2 == uc_fs_begin_cluster2))
				{
					if((uc_ebook_mark3_length1 == (uc8)ul_fs_file_length) && (uc_ebook_mark3_length2 == (uc8)(ul_fs_file_length>>8)))
					{
						Ap_Get_SelFile();					//get bookmark3 saved file
						if((uc_ebookmark_flag & 0x20)!= 0)  //judge last saved bookmark is bookmark3
						{
						 	return;
						}
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
			if(tmp_dir_idx == 0xffff)			//already it's ROOT
			{
				break;
			}
			
			tmp_dir_idx++;
			goto Label_IntoDir_Again;
		}
	} 
	if(ui_fb_total_num == 0)
	{
		b_ebook_nofile_flg = 1;
	}
	else
	{
		b_ebook_nofile_flg = 0;
	}		

}

void Ap_Get_SelFile(void)				//get last saved file
{
	FS_GetFilePath();					//get current file path
	
	ui_fb_sel_item_idx = tmp_file_idx_indir + ui_fs_subdir_num;
	if(uc_fs_subdir_stk_pt == 0)
	{
		ui_fb_sel_item_idx--;		//have not "dummy folder" in ROOT
	}
	if(ui_fb_sel_item_idx < Dis_BrowserItem_MaxNum)
	{
		ui_fb_top_item_idx = 0;
	}
	else
	{
		ui_fb_top_item_idx = ui_fb_sel_item_idx - Dis_BrowserItem_MaxNum + 1;
	}
}

#endif