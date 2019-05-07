/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: fs_var.asm
Creator: andyliu					Date: 2008-03-19
Description: define variable be used in file system
Others: 
Version: V0.1
History:
	V0.1	2008-03-19		andyliu
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef FILE_SYSTEM_VAR_DEF
#include "sh86278_memory_assign.h"

#ifndef FS_VAR
#define FS_VAR


PUBLIC	b_fs_func_flg
PUBLIC	b_fs_dir_flg
PUBLIC	b_fs_file_end_flg
PUBLIC	b_fs_next_sector_flg
PUBLIC	b_fs_fat_type_flg
PUBLIC	b_fs_fat32_root_flg
PUBLIC	b_fs_del_entry_ext_flg
PUBLIC	b_fs_file_err_flg
PUBLIC	b_fs_first_open
PUBLIC	b_fs_first_wr_file_flg
PUBLIC	b_fs_pre_flg
PUBLIC	b_fs_temp_flg
PUBLIC	b_fs_got_fat2_addr_flg
PUBLIC	b_found_blank_cluster
PUBLIC	b_exit_program_flg
PUBLIC  b_get_free_capacity
PUBLIC	PartitionEntryNo
PUBLIC	fat_num

PUBLIC	uc_fs_assign_buf_flg
PUBLIC	ui_fs_file_type
PUBLIC	ui_fs_special_file_num_indir
PUBLIC	uc_fs_offset_in_sec1
PUBLIC	uc_fs_offset_in_sec2
PUBLIC	ui_fs_item_num
PUBLIC	ui_fs_subdir_num
PUBLIC	uc_fs_cur_cluster1
PUBLIC	uc_fs_cur_cluster2
PUBLIC	uc_fs_cur_cluster3
PUBLIC	uc_fs_begin_cluster1
PUBLIC	uc_fs_begin_cluster2
PUBLIC	uc_fs_begin_cluster3
PUBLIC	uc_fs_subdir_stk_pt
PUBLIC	uc_fs_tmp_var1
PUBLIC	uc_fs_tmp_var2
PUBLIC	uc_fs_tmp_var3
PUBLIC	uc_fs_tmp_var4
PUBLIC	uc_fs_tmp_var5
PUBLIC	uc_fs_tmp_var6
PUBLIC	uc_fs_tmp_var7
PUBLIC	uc_fs_tmp_var8
PUBLIC	ul_fs_byte_offset
PUBLIC	uc_fs_openfile_type

PUBLIC	uc_fs_cur_sec_in_cluster

PUBLIC	uc_fs_layer1_dir_bc1
PUBLIC	uc_fs_layer1_dir_bc2
PUBLIC	uc_fs_layer1_dir_bc3
PUBLIC	uc_fs_layer2_dir_bc1
PUBLIC	uc_fs_layer2_dir_bc2
PUBLIC	ul_fs_file_length

PUBLIC	uc_fs_file_cur_clst1
PUBLIC	uc_fs_file_cur_clst2
PUBLIC	uc_fs_file_cur_clst3
PUBLIC	uc_fs_file_cur_sec_in_clst
PUBLIC	uc_fs_file_cur_clst1_bk
PUBLIC	uc_fs_file_cur_clst2_bk
PUBLIC	uc_fs_file_cur_clst3_bk
PUBLIC	uc_fs_file_cur_sec_in_clst_bk
PUBLIC	uc_fs_cr_bgn_cluster1
PUBLIC	uc_fs_cr_bgn_cluster2
PUBLIC	uc_fs_cr_bgn_cluster3
PUBLIC	uc_fs_sav_entry_dpl
PUBLIC	uc_fs_sav_entry_dph
PUBLIC	uc_fs_dir_entry_dpl
PUBLIC	uc_fs_dir_entry_dph
PUBLIC	uc_fs_sav_entry_sect4
PUBLIC	uc_fs_sav_entry_sect3
PUBLIC	uc_fs_sav_entry_sect2
PUBLIC	uc_fs_sav_entry_sect1
PUBLIC	uc_fs_dir_entry_sect4
PUBLIC	uc_fs_dir_entry_sect3
PUBLIC	uc_fs_dir_entry_sect2
PUBLIC	uc_fs_dir_entry_sect1
PUBLIC	uc_fs_old_fat_addr1
PUBLIC	uc_fs_old_fat_addr2
PUBLIC	ui_fs_wav_dir_idx
PUBLIC	ul_fs_spare_space
PUBLIC	ul_fs_sd_spare_space
PUBLIC	uc_fs_wav_idx
PUBLIC	uc_fs_sec_per_cluster
PUBLIC	uc_fs_fdt_sec_num
PUBLIC	uc_fs_fat1_begin_1
PUBLIC	uc_fs_fat1_begin_2
PUBLIC	uc_fs_fat2_begin_1
PUBLIC	uc_fs_fat2_begin_2
PUBLIC	uc_fs_fdt_begin_1
PUBLIC	uc_fs_fdt_begin_2
PUBLIC	uc_fs_dat_begin_1
PUBLIC	uc_fs_dat_begin_2
PUBLIC	uc_fs_fat_srh_addr1
PUBLIC	uc_fs_fat_srh_addr2
PUBLIC	uc_fs_sd_fat_srh_addr1
PUBLIC	uc_fs_sd_fat_srh_addr2
PUBLIC	uc_fs_subdir_stk_buf
PUBLIC	uc_fs_entry_sector1
PUBLIC	uc_fs_entry_sector2
PUBLIC	uc_fs_entry_sector3
PUBLIC	uc_fs_entry_sector4
PUBLIC	uc_fs_entry_no
PUBLIC	uc_fs_item_name_length
PUBLIC	ui_fs_file_num_indir
PUBLIC	ui_fs_item_name
PUBLIC	uc_fs_item_83_name

PUBLIC	uc_fs_fat_sector_max_num1
PUBLIC	uc_fs_fat_sector_max_num2
PUBLIC	uc_fs_fat_sector_max_num3
PUBLIC	uc_fs_fat_sector_max_num4

PUBLIC	uc_fs_sd_fat_sector_max_num1
PUBLIC	uc_fs_sd_fat_sector_max_num2
PUBLIC	uc_fs_sd_fat_sector_max_num3
PUBLIC	uc_fs_sd_fat_sector_max_num4

PUBLIC	ui_fs_dir_path_buf
PUBLIC	si_fs_file_idx
PUBLIC	uc_fs_item_type
PUBLIC	uc_fs_idx_hi
PUBLIC	uc_fs_idx_lo
PUBLIC	uc_fs_dptr_hi
PUBLIC	uc_fs_dptr_lo
PUBLIC	uc_fs_entry_hi3
PUBLIC	uc_fs_entry_hi2
PUBLIC	uc_fs_entry_hi
PUBLIC	uc_fs_entry_lo
PUBLIC	uc_fs_entry_item_num
PUBLIC	uc_fs_cur_sec_in_cluster_bk
PUBLIC	uc_fs_sector1_bk
PUBLIC	uc_fs_sector0_bk

PUBLIC	uc_rec_wav_map_buf

PUBLIC	analytic_fat_length1
PUBLIC	analytic_fat_length2

PUBLIC  uc_hidden_sector1
PUBLIC  uc_hidden_sector2

	BSEG	AT		B_ADDR_FS
b_fs_func_flg:				DBIT	1
b_fs_dir_flg:				DBIT	1
b_fs_file_end_flg:			DBIT	1
b_fs_next_sector_flg:		DBIT	1
b_fs_fat_type_flg:			DBIT	1
b_fs_fat32_root_flg:		DBIT	1
b_fs_del_entry_ext_flg:		DBIT	1
b_fs_temp_flg:
b_fs_file_err_flg:
b_fs_got_fat2_addr_flg:
b_fs_first_wr_file_flg:		DBIT	1
b_fs_first_open:			DBIT	1
b_fs_pre_flg:				DBIT	1
b_found_blank_cluster:		DBIT	1
b_exit_program_flg:			DBIT	1
b_get_free_capacity:		DBIT	1



	DSEG	AT		D_ADDR_FS
uc_fs_offset_in_sec1:			DS	1
uc_fs_offset_in_sec2:			DS	1
ui_fs_subdir_num:				DS	2
uc_fs_cur_cluster1:				DS	1
uc_fs_cur_cluster2:				DS	1
uc_fs_cur_cluster3:				DS	1
uc_fs_begin_cluster1:			DS	1
uc_fs_begin_cluster2:			DS	1
uc_fs_begin_cluster3:			DS	1
uc_fs_subdir_stk_pt:			DS	1
uc_fs_tmp_var1:					DS	1
uc_fs_tmp_var2:					DS	1
uc_fs_tmp_var3:					DS	1
uc_fs_tmp_var4:					DS	1
uc_fs_tmp_var5:					DS	1
uc_fs_tmp_var6:					DS	1
uc_fs_tmp_var7:					DS	1
;uc_fs_tmp_var8:					DS	1
ul_fs_byte_offset:				DS	4



	XSEG	AT		X_ADDR_FS
uc_fs_tmp_var8:					DS	1
uc_fs_file_cur_clst1:			DS	1
uc_fs_file_cur_clst2:			DS	1
uc_fs_file_cur_clst3:			DS	1
uc_fs_file_cur_sec_in_clst:		DS	1
uc_fs_cr_bgn_cluster1:			DS	1
uc_fs_cr_bgn_cluster2:			DS	1
uc_fs_cr_bgn_cluster3:			DS	1
uc_fs_sav_entry_dpl:			DS	1
uc_fs_sav_entry_dph:			DS	1
uc_fs_dir_entry_dpl:			DS	1
uc_fs_dir_entry_dph:			DS	1
uc_fs_sav_entry_sect4:			DS	1
uc_fs_sav_entry_sect3:			DS	1
uc_fs_sav_entry_sect2:			DS	1
uc_fs_sav_entry_sect1:			DS	1
uc_fs_dir_entry_sect4:			DS	1
uc_fs_dir_entry_sect3:			DS	1
uc_fs_dir_entry_sect2:			DS	1
uc_fs_dir_entry_sect1:			DS	1
uc_fs_old_fat_addr1:			DS	1
uc_fs_old_fat_addr2:			DS	1
ul_fs_spare_space:				DS	4
ul_fs_sd_spare_space:			DS	4

uc_fs_sec_per_cluster:			DS	1
uc_fs_fdt_sec_num:				DS	1
uc_fs_fat1_begin_1:				DS	1
uc_fs_fat1_begin_2:				DS	1
uc_fs_fat2_begin_1:				DS	1
uc_fs_fat2_begin_2:				DS	1
uc_fs_fdt_begin_1:				DS	1
uc_fs_fdt_begin_2:				DS	1
uc_fs_dat_begin_1:				DS	1
uc_fs_dat_begin_2:				DS	1
uc_fs_fat_srh_addr1:			DS	1
uc_fs_fat_srh_addr2:			DS	1
uc_fs_sd_fat_srh_addr1:			DS	1
uc_fs_sd_fat_srh_addr2:			DS	1

uc_fs_entry_sector1:			DS	1
uc_fs_entry_sector2:			DS	1
uc_fs_entry_sector3:			DS	1
uc_fs_entry_sector4:			DS	1
uc_fs_entry_no:					DS	1

uc_fs_fat_sector_max_num1:		DS	1
uc_fs_fat_sector_max_num2:		DS	1
uc_fs_fat_sector_max_num3:		DS	1
uc_fs_fat_sector_max_num4:		DS	1
uc_fs_sd_fat_sector_max_num1:	DS	1
uc_fs_sd_fat_sector_max_num2:	DS	1
uc_fs_sd_fat_sector_max_num3:	DS	1
uc_fs_sd_fat_sector_max_num4:	DS	1

PartitionEntryNo:				DS	1
fat_num:						DS	1

uc_fs_subdir_stk_buf:			DS	40		;FS_MAX_SUBDIR_DEPTH*FS_BYTE_PER_LAYER
ui_fs_dir_path_buf:				DS	18		;(FS_MAX_SUBDIR_DEPTH+1)*2

uc_rec_wav_map_buf:				;DS	32	(for record mode)
ui_fs_item_name:				DS	64		;FS_NAME_LEN_CONST
uc_fs_item_83_name:				DS	12
uc_fs_item_name_length:			DS	1

uc_fs_item_type:				DS	1
ui_fs_file_type:				DS	2
uc_fs_assign_buf_flg:			DS	1

ui_fs_file_num_indir:			DS	2
ui_fs_wav_dir_idx:				DS	2
si_fs_file_idx:					DS	2
uc_fs_wav_idx:					DS	1
ui_fs_special_file_num_indir:	DS	2

uc_fs_idx_hi:					DS	1
uc_fs_idx_lo:					DS	1
uc_fs_dptr_hi:					DS	1
uc_fs_dptr_lo:					DS	1
uc_fs_entry_hi3:				DS	1
uc_fs_entry_hi2:				DS	1
uc_fs_entry_hi:					DS	1
uc_fs_entry_lo:					DS	1
uc_fs_entry_item_num:			DS	1
uc_fs_cur_sec_in_cluster_bk:	DS	1
uc_fs_sector1_bk:				DS	1
uc_fs_sector0_bk:				DS	1	
uc_fs_openfile_type:			DS	1
analytic_fat_length1:			DS	1
analytic_fat_length2:			DS	1
uc_hidden_sector1:				DS	1
uc_hidden_sector2:				DS	1

	ISEG	AT	I_ADDR_FS
uc_fs_cur_sec_in_cluster:		DS	1
uc_fs_layer1_dir_bc1:			DS	1
uc_fs_layer1_dir_bc2:			DS	1
uc_fs_layer1_dir_bc3:			DS	1
uc_fs_layer2_dir_bc1:			DS	1
uc_fs_layer2_dir_bc2:			DS	1
ul_fs_file_length:				DS	4		;ul_fs_file_length+3 lowest, ul_fs_file_length highest
uc_fs_file_cur_clst1_bk:		DS	1
uc_fs_file_cur_clst2_bk:		DS	1
uc_fs_file_cur_clst3_bk:		DS	1
uc_fs_file_cur_sec_in_clst_bk:	DS	1
ui_fs_item_num:					DS	2
#endif
#endif
END