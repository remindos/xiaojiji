/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: msi_var.asm
Creator: andyliu					Date: 2008-07-25
Description: define variable that is msi driver
Others: 
Version: V0.1
History:
	V0.1	2008-07-25		andyliu
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
;#include "sh86278_func_asm.inc"
#include "sh86278_memory_assign.h"

PUBLIC	b_msi_sd1_mmc0_flg
PUBLIC	b_msi_sdhc_flg
PUBLIC	b_read_block_flg

PUBLIC	uc_msi_sec_num_max_3
PUBLIC	uc_msi_sec_num_max_2
PUBLIC	uc_msi_sec_num_max_1
PUBLIC	uc_msi_sec_num_max_0
//PUBLIC	uc_msi_clk_sel_cnt
//PUBLIC	uc_msi_clk_351K_cnt

#ifdef	MSI_API_MULTI_SEC_DEF
PUBLIC	uc_msi_block_num
PUBLIC	b_msi_test_ready_ok
PUBLIC	bak_ef_4kpage_flg
PUBLIC	b_msi_rd_status_flg
PUBLIC	b_msi_wr_status_flg
#endif

	BSEG	AT	B_ADDR_MSI
b_msi_sd1_mmc0_flg:		DBIT	1
b_msi_sdhc_flg:			DBIT	1
b_read_block_flg:		DBIT	1

	XSEG	AT		X_ADDR_MSI
uc_msi_sec_num_max_3:	DS		1
uc_msi_sec_num_max_2:	DS		1
uc_msi_sec_num_max_1:	DS		1
uc_msi_sec_num_max_0:	DS		1
//uc_msi_clk_sel_cnt:		DS		1
//uc_msi_clk_351K_cnt:	DS		1


#ifdef	MSI_API_MULTI_SEC_DEF
	BSEG	AT		B_ADDR_MSI_MULTI
b_msi_test_ready_ok:		DBIT	1
bak_ef_4kpage_flg:			DBIT	1
b_msi_wr_status_flg:		DBIT	1
b_msi_rd_status_flg:		DBIT	1

MSI_XDATA1	SEGMENT		XDATA	 
			RSEG		MSI_XDATA1
uc_msi_block_num:		DS		1
#endif


	END