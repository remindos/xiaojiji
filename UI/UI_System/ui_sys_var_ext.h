/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_sys_var_ext.h
Creator: zj.zong			Date: 2009-06-03
Description: extern system variable for "*.c"
Others:
Version: V0.1
History:
	V0.1	2009-06-03		zj.zong
			1. build this module
********************************************************************************/
#ifndef SYS_VAR_EXT
#define SYS_VAR_EXT

extern ul32  data ul_disk_encrypt_capacity;
extern ul32  data ul_disk_nomal_capacity;
extern uc8  data uc_disk_wr_state;

extern uc8  xdata uc_menu_cur_idx_bk;
extern uc8  data uc_menu_cur_idx;
extern uc8  data uc_menu_top_idx;

extern uc8  xdata uc_menu_total_num;
extern uc8  xdata uc_menu_exit_cnt;
extern uc8  xdata uc_disk_number;
extern uc8  xdata uc_amt_onoff_flg;

extern uc8  xdata uc_mp3_replay_mode;
extern uc8  xdata uc_ebook_play_mode;
extern uc8  xdata uc_ebook_play_gap;
extern uc8  xdata uc_language_idx;
extern uc8  xdata uc_language_total_num;
extern uc8  xdata uc_mp3_eq_mode;
extern uc8  xdata uc_mp3_repeat_mode;
extern uc8  xdata uc_disk_state;
extern uc8  xdata uc_mp3_replay_time;
extern uc8  xdata uc_mp3_replay_gap;
extern uc8  xdata uc_mp3_volume_level;
extern uc8  xdata uc_rec_quality_level;
extern uc8  xdata uc_voice_repeat_mode;
extern uc8  xdata uc_voice_volume_level;
extern uc8  xdata uc_fm_band;
extern uc8  xdata uc_fm_volume_level;
extern uc8  xdata uc_sys_ver_buf[];
extern uc8  xdata uc_sys_date_buf[];
extern uc8  xdata uc_language_mapping_buf[];


extern uc8  xdata uc_fs_fat_sector_max_num1;
extern uc8  xdata uc_fs_fat_sector_max_num2;
extern uc8  xdata uc_fs_fat_sector_max_num3;
extern uc8  xdata uc_fs_fat_sector_max_num4;

extern ul32 xdata ul_fs_spare_space;

#endif