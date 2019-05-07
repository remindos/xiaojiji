/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_ebook_var_ext.h
Creator: zj.zong			Date: 2009-05-04
Description: extern ebook variable for "*.c"
Others:
Version: V0.1
History:
	V0.1	2009-05-04		zj.zong
			1. build this module
********************************************************************************/
#ifndef EBOOK_VAR_EXT
#define EBOOK_VAR_EXT

extern bit b_ebook_file_end;
extern bit b_ebook_nofile_flg;
extern bit b_ebook_first_unicode_flg;
extern bit b_ebbok_unicode_8_16_flg;
extern bit b_browser_fir_getname_flg;
extern bit b_sd_exist_flg;
extern bit b_fs_subdir_stk_pt;


extern uc8 data uc_ebookmark_flag;
extern uc8 data uc_fb_sel_item_type;
extern uc8 data uc_dynamicload_idx;

extern ui16 data ui_ebook_page_idx;
extern ui16 data ui_ebook_page_num;
extern ui16 data ui_ebook_mark1;
extern ui16 data ui_ebook_mark2;
extern ui16 data ui_ebook_mark3;
extern ui16 data ui_fb_top_item_idx;
extern ui16 data ui_fb_sel_item_idx;
extern ui16 data ui_fb_cur_item_idx;

extern uc8  xdata uc_menu_top_idx;        //allen
extern uc8  xdata uc_ebook_play_mode;
extern uc8  xdata uc_ebook_play_gap;
extern uc8  xdata uc_ebook_play_gap_cnt;
extern uc8  xdata uc_menu_cur_idx_bk;
extern uc8  xdata uc_menu_cur_idx;
extern uc8  xdata uc_menu_total_num;
extern uc8  xdata uc_menu_exit_cnt;
extern uc8  xdata uc_fb_tmp_dir_max_item;
extern uc8  xdata uc_fb_dis_cnt;
extern uc8  xdata uc_ebook_unicode_flg;
extern uc8  xdata uc_ebook_mark1_cluster1;
extern uc8  xdata uc_ebook_mark1_cluster2;
extern uc8  xdata uc_ebook_mark1_length1;
extern uc8  xdata uc_ebook_mark1_length2;
extern uc8  xdata uc_ebook_mark2_cluster1;
extern uc8  xdata uc_ebook_mark2_cluster2;
extern uc8  xdata uc_ebook_mark2_length1;
extern uc8  xdata uc_ebook_mark2_length2;
extern uc8  xdata uc_ebook_mark3_cluster1;
extern uc8  xdata uc_ebook_mark3_cluster2;
extern uc8  xdata uc_ebook_mark3_length1;
extern uc8  xdata uc_ebook_mark3_length2;

extern ui16 xdata tmp_file_idx_indir;
extern ui16 xdata ui_ebook_byte_idx;
extern ui16 xdata ui_ebook_val;
extern ui16 xdata ui_ebook_sfc_addr;
extern ui16 xdata ui_ebook_tmp_var;
extern ui16 xdata ui_fb_total_num;


/******************fs*******************/
extern uc8   data uc_fs_begin_cluster1;
extern uc8   data uc_fs_begin_cluster2;
extern uc8   data uc_fs_begin_cluster3;
extern uc8   data uc_fs_subdir_stk_pt;

extern ui16  data ui_fs_subdir_num;
extern ui16  data ui_fs_item_num;
extern ul32  data ul_fs_byte_offset;

extern ul32 idata ul_fs_file_length;

extern uc8  xdata uc_fs_item_83_name[];
extern uc8  xdata uc_fs_assign_buf_flg;
extern uc8  xdata uc_fs_item_type;
extern uc8  xdata uc_fs_store_subdir_stk_pt;
extern uc8  xdata uc_fs_store_cluster3;
extern uc8  xdata uc_fs_item_name_length;

extern ui16 xdata ui_fs_file_type;
extern ui16 xdata ui_fs_item_name[];
extern ui16 xdata ui_fs_file_num_indir;
extern ui16 xdata ui_fs_special_file_num_indir;
extern si16 xdata si_fs_file_idx;

#endif