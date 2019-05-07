/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_music_subfunc_dload.c
Creator: zj.zong					Date: 2009-04-29
Description: dynamic loading subfunc in music
Others:
Version: V0.1
History:
	V0.1	2009-04-27		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_MUSIC_SUBFUNC_DLOAD_DEF

#include "sh86278_sfr_reg.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "..\..\UI\UI_Music\ui_music_header.h"
#include "..\..\UI\UI_Music\ui_music_var_ext.h"


void Ap_Cal_TotalTime(void);
void Ap_SelNext_InBrowser(void);
void Ap_SelPrevious_InBrowser(void);
void Ap_Init_PlayVar(void);
void Ap_init_StopMenu_Sub(void);
void Ap_Display_BrowserItem_Sub(void);
void Ap_Save_Mp3_Parameter(void);

ui16 Ap_Get_SampleRate(void);
ui16 Ap_Get_BitRate(void);

/*******************************************************************************
Function: Ap_Music_SubFunc_DLoad()
Description: select next file or previous file
Calls:
Global:uc_dynamicload_idx:50--Ap_Cal_TotalTime(), Calculate Total Time
                          51--Ap_SelNext_InBrowser()
                          52--Ap_SelPrevious_InBrowser()
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Music_SubFunc_DLoad(void)
{
	switch(uc_dynamicload_idx)
	{
		case 50:
			Ap_Cal_TotalTime();
			break;
			
		case 51:
			Ap_SelNext_InBrowser();
			break;
			
		case 52:
			Ap_SelPrevious_InBrowser();
			break;

		case 53:
			Ap_Init_PlayVar();
			break;

		case 54:
			Ap_init_StopMenu_Sub();
			break;	

		case 55:
			Ap_Display_BrowserItem_Sub();
			break;	

		case 56:
			Ap_Save_Mp3_Parameter();
			break;				
		}
   uc_dynamicload_idx = 0;
}



void Ap_Cal_TotalTime(void)
{
	ui_mp3_bitrate = Ap_Get_BitRate();
	
	if(ul_mp3_vbr_total_frame != 0)				//VBR Song
	{
		ui_mp3_total_time = (int)(ul_mp3_vbr_total_frame * 1152 / Ap_Get_SampleRate());
	}
	else										//CBR Song
	{
		ui_mp3_total_time = (int)((ul_fs_file_length-ul_id3_size) / (ui_mp3_bitrate * 125));
	}
}



void Ap_SelNext_InBrowser(void)
{
	ui16 tmp_fs_item_num_indir;

	tmp_fs_item_num_indir = ui_fs_subdir_num + ui_fs_special_file_num_indir;
	if(uc_fs_subdir_stk_pt > 0)	
	{
		tmp_fs_item_num_indir++;		//has "dummy folder" in SubDir
	}
	
	if(tmp_fs_item_num_indir > 0)
	{
		ui_fb_sel_item_idx++;
		if(ui_fb_sel_item_idx < tmp_fs_item_num_indir)
		{
			if(ui_fb_sel_item_idx >= (ui_fb_top_item_idx + Dis_BrowserItem_MaxNum))
			{
				ui_fb_top_item_idx++;
			}
		}
		else
		{
			ui_fb_sel_item_idx = 0;
			ui_fb_top_item_idx = 0;
		}

		b_lcd_clrall_flg = 1;
		uc_lcd_dis_flg = 0xfd;
	}
}



void Ap_SelPrevious_InBrowser(void)
{
	ui16 tmp_fs_item_num_indir;

	tmp_fs_item_num_indir = ui_fs_subdir_num + ui_fs_special_file_num_indir;
	if(uc_fs_subdir_stk_pt > 0)	
	{
		tmp_fs_item_num_indir++;		//has "dummy folder" in SubDir
	}
	
	if(tmp_fs_item_num_indir > 0)
	{
		if(ui_fb_sel_item_idx == 0)
		{
			ui_fb_sel_item_idx = tmp_fs_item_num_indir - 1;
			if(tmp_fs_item_num_indir >= Dis_BrowserItem_MaxNum)
			{
				ui_fb_top_item_idx = ui_fb_sel_item_idx - Dis_BrowserItem_MaxNum + 1;
			}
			else
			{
				ui_fb_top_item_idx = 0;
			}
		}
		else
		{
			ui_fb_sel_item_idx--;
		}
	
		if(ui_fb_top_item_idx > ui_fb_sel_item_idx)
		{
			ui_fb_top_item_idx = ui_fb_sel_item_idx;
		}

		b_lcd_clrall_flg = 1;
		uc_lcd_dis_flg = 0xfd;
	}
}

void Ap_Init_PlayVar(void)
{
	uc_loop_cnt = 7;
	b_mpeg_dec5s_ok = 0;						//clr decode mp3 header flag		
	b_mp3_dis_freq_flg = 1;
	b_mp3_file_err = 0;
	b_mp3_file_end = 0;
	b_mp3_save_flg = 0;
	b_mp3_lrc_flg = 0;
	b_lrc_file_exist = 0;
	b_mp3_dec_reset_flg = 0;

	uc_mp3_refresh_id3_cnt = 0;
	uc_mp3_refresh_lrc_cnt = 0;
	uc_mp3_time_cnt = 0;
	ui_mp3_cur_time = 0;
	ui_mp3_total_time = 0;
	uc_mp3_replay_ab_flg = 0;					//0-normal, 1:set A, 2:set B, 3:A-B
	uc_mp3_dis_offset = 0;
	uc_mp3_lrc_offset = 0;
	uc_mp3_fill_dbuf_flg = 0;
	uc_mp3_bitrate_idx = 0;
	uc_daci_sr = 0;
	uc_filldata_cnt = 0;

	ui_mp3_random_idx_bk = ui_mp3_idx;
	ul_mp3_vbr_total_frame = 0;
	
	ui_sys_event &= ~(EVT_MUSIC_FILE_ERR|EVT_MUSIC_FILE_END|EVT_MUSIC_FILL_DECBUF);
} 


void Ap_init_StopMenu_Sub(void)
{

	uc_window_no = WINDOW_MUSIC_MENU_BROWSER;
		
	ui_fb_sel_item_idx = ui_mp3_idx_indir + ui_fs_subdir_num;
	if(uc_fs_subdir_stk_pt == 0)
	{
		ui_fb_sel_item_idx--;				//have not "dummy folder" in ROOT
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

void Ap_Display_BrowserItem_Sub(void)
{

	b_browser_fir_getname_flg = 0;
	uc_fb_sel_item_type = 0;
	
	ui_mp3_tmp_var = ui_fs_subdir_num + ui_fs_special_file_num_indir;	//folder + mp3 file 
	if(uc_fs_subdir_stk_pt > 0)
	{
		ui_mp3_tmp_var++;		//file + folder + dummy folder
	}

	if(ui_mp3_tmp_var >= Dis_BrowserItem_MaxNum)	//init item number that is displayed on page
	{
		uc_fb_tmp_dir_max_item = Dis_BrowserItem_MaxNum;
	}
	else
	{
		uc_fb_tmp_dir_max_item = (uc8)ui_mp3_tmp_var;
	}

	uc_lcd_page_num = 2;  
}

void Ap_Save_Mp3_Parameter()
{
	uc_fs_store_cluster1 = uc_fs_begin_cluster1;//backup current mp3 file parameter for continue playing
	uc_fs_store_cluster2 = uc_fs_begin_cluster2;
	uc_fs_store_length1 = (uc8)ul_fs_file_length;
	uc_fs_store_length2 = (uc8)(ul_fs_file_length >> 8);   	  
}
#endif