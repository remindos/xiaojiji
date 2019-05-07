/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: fs_var_ext.h
Creator: andyliu					Date: 2008-03-19
Description: extern fs variable for "*.c"
Others:
Version: V0.1
History:
	V0.1	2008-03-19		andyliu
			1. build this module
********************************************************************************/
#ifndef FS_VAR_EXT
#define FS_VAR_EXT

extern bit b_fs_func_flg;
extern bit b_fs_dir_flg;
extern bit b_fs_file_end_flg;
extern bit b_fs_next_sector_flg;
extern bit b_fs_fat_type_flg;
extern bit b_fs_fat32_root_flg;
extern bit b_fs_del_entry_ext_flg;
extern bit b_fs_file_err_flg;
extern bit b_fs_first_open;
extern bit b_fs_first_wr_file_flg;
extern bit b_fs_pre_flg;
extern bit b_fs_temp_flg;
extern bit b_fs_got_fat2_addr_flg;
extern bit b_sd_flash_flg;

extern uc8 data sfc_logblkaddr0;
extern uc8 data sfc_logblkaddr1;
extern uc8 data sfc_logblkaddr2;
extern uc8 data sfc_logblkaddr3;
extern uc8 data EF_MAX_NUM_OF_SECTORLO1;
extern uc8 data EF_MAX_NUM_OF_SECTORLO2;
extern uc8 data EF_MAX_NUM_OF_SECTORHI;

extern ui16 data ui_fs_subdir_num;
extern uc8  data uc_fs_cur_cluster1;
extern uc8  data uc_fs_cur_cluster2;
extern uc8  data uc_fs_cur_cluster3;
extern uc8  data uc_fs_begin_cluster1;
extern uc8  data uc_fs_begin_cluster2;
extern uc8  data uc_fs_begin_cluster3;
extern uc8  data uc_fs_subdir_stk_pt;
extern ul32 data ul_fs_byte_offset;


extern uc8  idata uc_fs_cur_sec_in_cluster;
extern ul32 idata ul_fs_file_length;
extern ui16 idata ui_fs_item_num;

extern uc8  idata uc_fs_file_cur_clst1_bk;
extern uc8  idata uc_fs_file_cur_clst2_bk;
extern uc8  idata uc_fs_file_cur_clst3_bk;
extern uc8  idata uc_fs_file_cur_sec_in_clst_bk;

extern uc8 xdata uc_fs_openfile_type;
extern uc8 xdata uc_fs_item_type;
extern uc8 xdata uc_fs_cr_bgn_cluster1;
extern uc8 xdata uc_fs_cr_bgn_cluster2;
extern uc8 xdata uc_fs_cr_bgn_cluster3;
extern uc8 xdata uc_fs_file_cur_clst1;
extern uc8 xdata uc_fs_file_cur_clst2;
extern uc8 xdata uc_fs_file_cur_clst3;
extern uc8 xdata uc_fs_file_cur_sec_in_clst;
extern uc8 xdata uc_fs_fat1_begin_1;
extern uc8 xdata uc_fs_fat1_begin_2;
extern uc8 xdata uc_fs_subdir_stk_buf[];
extern uc8 xdata uc_fs_item_83_name[];
extern uc8 xdata uc_fs_item_name_length;
extern uc8 xdata uc_fs_wav_idx;
extern ui16 xdata ui_fs_dir_path_buf[];
extern ui16 xdata ui_fs_wav_dir_idx;
extern ui16 xdata ui_fs_item_name[];
extern ui16 xdata ui_fs_file_num_indir;
extern ul32 xdata ul_fs_spare_space;
extern ul32 xdata ul_fs_sd_spare_space;
extern si16 xdata si_fs_file_idx;

extern uc8 xdata uc_fs_fat_srh_addr1;
extern uc8 xdata uc_fs_fat_srh_addr2;
extern uc8 xdata uc_fs_sd_fat_srh_addr1;
extern uc8 xdata uc_fs_sd_fat_srh_addr2;

extern uc8 xdata uc_fs_idx_hi;
extern uc8 xdata uc_fs_idx_lo;
extern uc8 xdata uc_fs_dptr_hi;
extern uc8 xdata uc_fs_dptr_lo;
extern uc8 xdata uc_fs_entry_hi3;
extern uc8 xdata uc_fs_entry_hi2;
extern uc8 xdata uc_fs_entry_hi;
extern uc8 xdata uc_fs_entry_lo;
extern uc8 xdata uc_fs_entry_item_num;
extern uc8 xdata uc_fs_cur_sec_in_cluster_bk;
extern uc8 xdata uc_fs_sector1_bk;
extern uc8 xdata uc_fs_sector0_bk;

extern uc8 xdata uc_fs_fdt_begin_1;
extern uc8 xdata uc_fs_fdt_begin_2;

extern ui16 xdata ui_fs_file_type;
extern uc8 data sfc_errcode;

#endif