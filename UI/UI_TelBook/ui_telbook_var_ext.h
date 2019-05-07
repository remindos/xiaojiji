/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_telbook_var_ext.h
Creator: zj.zong			Date: 2009-06-04
Description: extern telbook variable for "*.c"
Others:
Version: V0.1
History:
	V0.1	2009-06-04		zj.zong
			1. build this module
********************************************************************************/
#ifndef TELBOOK_VAR_EXT
#define TELBOOK_VAR_EXT


extern bit b_telbook_file_exist;
extern bit b_export_file_ok;

extern uc8 data uc_menu_cur_idx;
extern uc8 data uc_menu_total_num;
extern uc8 data uc_menu_exit_cnt;
extern uc8 data uc_tel_namelist_top_idx;
extern uc8 data uc_tel_namelist_sel_idx;
extern uc8 data uc_tel_namelist_num;
extern uc8 data uc_tel_dis_info_byteoffet;
extern uc8 data uc_tel_info_length;
extern uc8 data uc_menu_top_idx;

extern ui16 data ui_tel_string_idx;

extern uc8 xdata uc_tel_dis_info_buf[16];


/*****************fs********************/
extern ul32  data ul_fs_byte_offset;

extern ul32 idata ul_fs_file_length;

extern uc8  xdata uc_fs_assign_buf_flg;
extern uc8  xdata uc_fs_item_83_name[];

extern ui16 xdata ui_fs_item_name[];		//64bytes for display telbook name and info
extern ui16 xdata ui_fs_file_num_indir;
extern ui16 xdata ui_fs_file_type;

extern ul32 xdata ul_fs_spare_space;

#endif
