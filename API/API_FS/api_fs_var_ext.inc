/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: fs_var_ext.inc
Creator: andyliu					Date: 2008-03-19
Description: extern fs variable for "*.asm"
Others:
Version: V0.1
History:
	V0.1	2008-03-19		andyliu
			1. build this module
********************************************************************************/
#ifndef FS_VAR_EXT_ASM
#define FS_VAR_EXT_ASM

EXTRN	BIT		(b_fs_func_flg)
EXTRN	BIT		(b_fs_dir_flg)
EXTRN	BIT		(b_fs_file_end_flg)
EXTRN	BIT		(b_fs_next_sector_flg)
EXTRN	BIT		(b_fs_fat_type_flg)
EXTRN	BIT		(b_fs_fat32_root_flg)
EXTRN	BIT		(b_fs_del_entry_ext_flg)
EXTRN	BIT		(b_fs_file_err_flg)
EXTRN	BIT		(b_fs_first_open)
EXTRN	BIT		(b_fs_first_wr_file_flg)
EXTRN	BIT		(b_fs_pre_flg)
EXTRN	BIT		(b_fs_temp_flg)
EXTRN	BIT		(b_fs_got_fat2_addr_flg)
EXTRN	BIT 	(sfc_reserved_area_flag)
EXTRN	BIT		(b_found_blank_cluster)
EXTRN	BIT		(b_get_free_capacity)
EXTRN	BIT		(b_exit_program_flg)

EXTRN	DATA	(uc_fs_offset_in_sec1)
EXTRN	DATA	(uc_fs_offset_in_sec2)
EXTRN	DATA	(ui_fs_subdir_num)
EXTRN	DATA	(uc_fs_cur_cluster1)
EXTRN	DATA	(uc_fs_cur_cluster2)
EXTRN	DATA	(uc_fs_cur_cluster3)
EXTRN	DATA	(uc_fs_begin_cluster1)
EXTRN	DATA	(uc_fs_begin_cluster2)
EXTRN	DATA	(uc_fs_begin_cluster3)
EXTRN	DATA	(uc_fs_subdir_stk_pt)
EXTRN	DATA	(uc_fs_tmp_var1)
EXTRN	DATA	(uc_fs_tmp_var2)
EXTRN	DATA	(uc_fs_tmp_var3)
EXTRN	DATA	(uc_fs_tmp_var4)
EXTRN	DATA	(uc_fs_tmp_var5)
EXTRN	DATA	(uc_fs_tmp_var6)
EXTRN	DATA	(uc_fs_tmp_var7)
EXTRN	XDATA	(uc_fs_tmp_var8)
EXTRN	DATA	(ul_fs_byte_offset)

EXTRN	XDATA	(uc_fs_openfile_type)
EXTRN	XDATA	(PartitionEntryNo)
EXTRN	XDATA	(fat_num)

EXTRN	IDATA	(uc_fs_cur_sec_in_cluster)
EXTRN	IDATA	(uc_fs_layer1_dir_bc1)
EXTRN	IDATA	(uc_fs_layer1_dir_bc2)
EXTRN	IDATA	(uc_fs_layer1_dir_bc3)
EXTRN	IDATA	(uc_fs_layer2_dir_bc1)
EXTRN	IDATA	(uc_fs_layer2_dir_bc2)
EXTRN	IDATA	(ul_fs_file_length)
EXTRN	IDATA	(ui_fs_item_num)

EXTRN	IDATA	(uc_fs_file_cur_clst1_bk)
EXTRN	IDATA	(uc_fs_file_cur_clst2_bk)
EXTRN	IDATA	(uc_fs_file_cur_clst3_bk)
EXTRN	IDATA	(uc_fs_file_cur_sec_in_clst_bk)

EXTRN	XDATA	(uc_fs_file_cur_clst1)
EXTRN	XDATA	(uc_fs_file_cur_clst2)
EXTRN	XDATA	(uc_fs_file_cur_clst3)
EXTRN	XDATA	(uc_fs_file_cur_sec_in_clst)
EXTRN	XDATA	(uc_fs_cr_bgn_cluster1)
EXTRN	XDATA	(uc_fs_cr_bgn_cluster2)
EXTRN	XDATA	(uc_fs_cr_bgn_cluster3)
EXTRN	XDATA	(uc_fs_sav_entry_dpl)
EXTRN	XDATA	(uc_fs_sav_entry_dph)
EXTRN	XDATA	(uc_fs_dir_entry_dpl)
EXTRN	XDATA	(uc_fs_dir_entry_dph)
EXTRN	XDATA	(uc_fs_sav_entry_sect4)
EXTRN	XDATA	(uc_fs_sav_entry_sect3)
EXTRN	XDATA	(uc_fs_sav_entry_sect2)
EXTRN	XDATA	(uc_fs_sav_entry_sect1)
EXTRN	XDATA	(uc_fs_dir_entry_sect4)
EXTRN	XDATA	(uc_fs_dir_entry_sect3)
EXTRN	XDATA	(uc_fs_dir_entry_sect2)
EXTRN	XDATA	(uc_fs_dir_entry_sect1)
EXTRN	XDATA	(uc_fs_old_fat_addr1)
EXTRN	XDATA	(uc_fs_old_fat_addr2)
EXTRN	XDATA	(ui_fs_wav_dir_idx)
EXTRN	XDATA	(ul_fs_spare_space)
EXTRN	XDATA	(ul_fs_sd_spare_space)
EXTRN	XDATA	(uc_fs_wav_idx)
EXTRN	XDATA	(uc_fs_sec_per_cluster)
EXTRN	XDATA	(uc_fs_fdt_sec_num)
EXTRN	XDATA	(uc_fs_fat1_begin_1)
EXTRN	XDATA	(uc_fs_fat1_begin_2)
EXTRN	XDATA	(uc_fs_fat2_begin_1)
EXTRN	XDATA	(uc_fs_fat2_begin_2)
EXTRN	XDATA	(uc_fs_fdt_begin_1)
EXTRN	XDATA	(uc_fs_fdt_begin_2)
EXTRN	XDATA	(uc_fs_dat_begin_1)
EXTRN	XDATA	(uc_fs_dat_begin_2)
EXTRN	XDATA	(uc_fs_fat_srh_addr1)
EXTRN	XDATA	(uc_fs_fat_srh_addr2)
EXTRN	XDATA	(uc_fs_sd_fat_srh_addr1)
EXTRN	XDATA	(uc_fs_sd_fat_srh_addr2)
EXTRN	XDATA	(uc_fs_subdir_stk_buf)
EXTRN	XDATA	(uc_fs_entry_sector1)
EXTRN	XDATA	(uc_fs_entry_sector2)
EXTRN	XDATA	(uc_fs_entry_sector3)
EXTRN	XDATA	(uc_fs_entry_sector4)
EXTRN	XDATA	(uc_fs_entry_no)
EXTRN	XDATA	(uc_fs_item_name_length)
EXTRN	XDATA	(ui_fs_file_num_indir)
EXTRN	XDATA	(ui_fs_item_name)
EXTRN	XDATA	(uc_fs_item_83_name)
EXTRN	XDATA	(uc_fs_fat_sector_max_num1)
EXTRN	XDATA	(uc_fs_fat_sector_max_num2)
EXTRN	XDATA	(uc_fs_fat_sector_max_num3)
EXTRN	XDATA	(uc_fs_fat_sector_max_num4)

EXTRN	XDATA	(uc_fs_sd_fat_sector_max_num1)
EXTRN	XDATA	(uc_fs_sd_fat_sector_max_num2)
EXTRN	XDATA	(uc_fs_sd_fat_sector_max_num3)
EXTRN	XDATA	(uc_fs_sd_fat_sector_max_num4)

EXTRN	XDATA	(ui_fs_dir_path_buf)
EXTRN	XDATA	(si_fs_file_idx)
EXTRN	XDATA	(uc_fs_item_type)
EXTRN	XDATA	(ui_fs_file_type)
EXTRN	XDATA	(uc_fs_assign_buf_flg)
EXTRN	XDATA	(ui_fs_special_file_num_indir)
EXTRN	XDATA	(uc_fs_idx_hi)
EXTRN	XDATA	(uc_fs_idx_lo)
EXTRN	XDATA	(uc_fs_dptr_hi)
EXTRN	XDATA	(uc_fs_dptr_lo)
EXTRN	XDATA	(uc_fs_entry_hi3)
EXTRN	XDATA	(uc_fs_entry_hi2)
EXTRN	XDATA	(uc_fs_entry_hi)
EXTRN	XDATA	(uc_fs_entry_lo)
EXTRN	XDATA	(uc_fs_entry_item_num)
EXTRN	XDATA	(uc_fs_cur_sec_in_cluster_bk)
EXTRN	XDATA	(uc_fs_sector1_bk)
EXTRN	XDATA	(uc_fs_sector0_bk)

EXTRN   DATA 	(sfc_logblkaddr0)
EXTRN   DATA 	(sfc_logblkaddr1)
EXTRN   DATA	(sfc_logblkaddr2)
EXTRN   DATA 	(sfc_logblkaddr3)
EXTRN	DATA	(sfc_occ_buf_flag)
EXTRN	DATA	(buf_offset0)
EXTRN	DATA	(buf_offset1)
EXTRN   DATA	(page_flag0)
EXTRN   DATA	(page_flag1)
EXTRN	BIT   	(b_sfc_immediate_wr_flg)
;EXTRN	BIT	(last_write_flag)

EXTRN	XDATA	(ef_disk0_capacity0)
EXTRN	XDATA	(ef_disk0_capacity1)
EXTRN	XDATA	(ef_disk0_capacity2)
EXTRN	XDATA	(ef_disk0_capacity3)
EXTRN	DATA	(sfc_errcode)
EXTRN	XDATA	(analytic_fat_length1)
EXTRN	XDATA	(analytic_fat_length2)
EXTRN	XDATA	(uc_hidden_sector1)
EXTRN	XDATA	(uc_hidden_sector2)
#endif