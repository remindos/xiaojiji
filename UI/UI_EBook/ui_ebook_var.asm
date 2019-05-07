/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_ebook_var.asm
Creator: zj.zong					Date: 2009-05-04
Description: define variable be used in ebook mode
Others: 
Version: V0.1
History:
	V0.1	2009-05-04		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_EBOOK_VAR_DEF

/*****************************************/
#ifndef EBOOK_VAR
#define EBOOK_VAR


PUBLIC	b_ebook_file_end
PUBLIC	b_ebook_nofile_flg
PUBLIC	b_ebook_first_unicode_flg
PUBLIC	b_ebbok_unicode_8_16_flg
PUBLIC	b_browser_fir_getname_flg


PUBLIC	ui_ebook_mark1
PUBLIC	ui_ebook_mark2
PUBLIC	ui_ebook_mark3
PUBLIC	ui_fb_top_item_idx
PUBLIC	ui_fb_sel_item_idx

PUBLIC	uc_ebook_play_mode
PUBLIC	uc_ebook_play_gap
PUBLIC	uc_ebook_play_gap_cnt
PUBLIC  uc_ebookmark_flag
PUBLIC	uc_fb_sel_item_type
PUBLIC	uc_menu_top_idx			//allen

PUBLIC	uc_menu_cur_idx_bk
PUBLIC	uc_menu_cur_idx	 
PUBLIC	uc_menu_total_num
PUBLIC	uc_menu_exit_cnt
PUBLIC	uc_dynamicload_idx
PUBLIC	uc_fb_tmp_dir_max_item
PUBLIC	uc_fb_dis_cnt
PUBLIC	uc_ebook_mark1_cluster1
PUBLIC	uc_ebook_mark1_cluster2
PUBLIC	uc_ebook_mark1_length1
PUBLIC	uc_ebook_mark1_length2
PUBLIC	uc_ebook_mark2_cluster1
PUBLIC	uc_ebook_mark2_cluster2
PUBLIC	uc_ebook_mark2_length1
PUBLIC	uc_ebook_mark2_length2
PUBLIC	uc_ebook_mark3_cluster1
PUBLIC	uc_ebook_mark3_cluster2
PUBLIC	uc_ebook_mark3_length1
PUBLIC	uc_ebook_mark3_length2
PUBLIC	uc_ebook_unicode_flg

PUBLIC	ui_fb_total_num
PUBLIC	ui_fb_cur_item_idx
PUBLIC	ui_ebook_byte_idx
PUBLIC	ui_ebook_val
PUBLIC	ui_ebook_page_idx
PUBLIC	ui_ebook_page_num
PUBLIC	ui_ebook_sfc_addr
PUBLIC	ui_ebook_tmp_var
PUBLIC  tmp_file_idx_indir
PUBLIC  b_fs_subdir_stk_pt

PUBLIC	uc_fs_store_cluster1
PUBLIC	uc_fs_store_cluster2
PUBLIC	uc_fs_store_length1
PUBLIC	uc_fs_store_length2

	BSEG	AT		28H.5
b_ebook_file_end:			DBIT	1
b_ebook_nofile_flg:			DBIT	1
b_ebook_first_unicode_flg:	DBIT	1
b_ebbok_unicode_8_16_flg:	DBIT	1
b_browser_fir_getname_flg:	DBIT	1
b_fs_subdir_stk_pt:	     	DBIT	1

DSEG			AT		13H
ui_ebook_page_idx:			DS		2
ui_ebook_page_num:			DS		2
ui_ebook_mark1:				DS		2
ui_ebook_mark2:				DS		2
ui_ebook_mark3:				DS		2
ui_fb_cur_item_idx:			DS		2
uc_ebookmark_flag:			DS 		1

DSEG			AT		2AH
uc_dynamicload_idx:			DS		1
uc_fb_sel_item_type:		DS		1

ui_fb_top_item_idx:			DS		2
ui_fb_sel_item_idx:			DS		2

XSEG			AT		7200H

uc_ebook_play_mode:			DS		1
uc_ebook_play_gap:			DS		1
uc_ebook_play_gap_cnt:		DS		1
uc_menu_cur_idx_bk:			DS		1
uc_menu_cur_idx:			DS		1
uc_menu_total_num:			DS		1
uc_menu_exit_cnt:			DS		1
uc_fb_tmp_dir_max_item:		DS		1
uc_fb_dis_cnt:				DS		1
uc_ebook_unicode_flg:		DS		1
uc_menu_top_idx:			DS      1       //allen

uc_ebook_mark1_cluster1:	DS		1
uc_ebook_mark1_cluster2:	DS		1
uc_ebook_mark1_length1:		DS		1
uc_ebook_mark1_length2:		DS		1

uc_ebook_mark2_cluster1:	DS		1
uc_ebook_mark2_cluster2:	DS		1
uc_ebook_mark2_length1:		DS		1
uc_ebook_mark2_length2:		DS		1

uc_ebook_mark3_cluster1:	DS		1
uc_ebook_mark3_cluster2:	DS		1
uc_ebook_mark3_length1:		DS		1
uc_ebook_mark3_length2:		DS		1

tmp_file_idx_indir:			DS		2
ui_ebook_tmp_var:			DS		2
ui_ebook_val:				DS		2
ui_ebook_byte_idx:			DS		2
ui_ebook_sfc_addr:			DS		2
ui_fb_total_num:			DS		2

uc_fs_store_length1:		DS		1
uc_fs_store_length2:		DS		1
uc_fs_store_cluster1:		DS		1
uc_fs_store_cluster2:		DS		1
#endif
#endif
	END
