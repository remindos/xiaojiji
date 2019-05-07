/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: drv_msi_var_ext.inc
Creator: aj.zhao					Date: 2009-06-11
Description: extern msi resident variable for "*.asm"
Others:
Version: V0.1
History:
	V0.1	2009-06-11		aj.zhao
			1. build this module
********************************************************************************/
#ifndef MSI_RESIDENT_VAR_EXT_ASM
#define MSI_RESIDENT_VAR_EXT_ASM

EXTRN	BIT		(b_msi_sd1_mmc0_flg)
EXTRN	BIT		(b_msi_sdhc_flg)
EXTRN	BIT		(b_read_block_flg)

EXTRN	DATA	(uc_msi_status)
EXTRN	DATA	(uc_msi_err_flg)
EXTRN	DATA	(uc_msi_occ_buf_flg)
EXTRN	DATA	(uc_msi_response_buf)
EXTRN	XDATA	(uc_msi_sec_num_max_3)
EXTRN	XDATA	(uc_msi_sec_num_max_2)
EXTRN	XDATA	(uc_msi_sec_num_max_1)
EXTRN	XDATA	(uc_msi_sec_num_max_0)
EXTRN	XDATA	(uc_user_option1)
EXTRN	XDATA	(uc_user_option2)
//EXTRN	XDATA	(uc_msi_clk_sel_cnt)
//EXTRN	XDATA	(uc_msi_clk_351K_cnt)
EXTRN	DATA	(uc_msi_sec_addr_3)
EXTRN	DATA	(uc_msi_sec_addr_2)
EXTRN	DATA	(uc_msi_sec_addr_1)
EXTRN	DATA	(uc_msi_sec_addr_0)
EXTRN	DATA	(uc_msi_byte_offset_1)
EXTRN	DATA	(uc_msi_byte_offset_0)
EXTRN	DATA	(uc_msi_count0)
EXTRN	DATA	(uc_msi_count1)
EXTRN	DATA	(uc_msi_count2)
EXTRN	DATA	(sfc_errcode)


#ifdef MSI_API_MULTI_SEC_DEF
EXTRN	BIT		(b_msi_rd_status_flg)
EXTRN	BIT		(b_msi_wr_status_flg)
EXTRN	BIT		(b_msi_test_ready_ok)
EXTRN	BIT		(bak_ef_4kpage_flg)
EXTRN	XDATA	(uc_msi_block_num)
#endif

EXTRN	IDATA	(uc_msi_save_moden2)


#endif