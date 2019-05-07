/******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_bootblock_var.asm
Creator: zj.zong					Date: 2009-04-13
Description: 
Others:
Version: V0.1
History:
	V0.1	2009-04-13		zj.zong
		 1. build this module
*******************************************************************************/
#include "sh86278_compiler.h"
#ifdef	AP_BOOTBLOCK_VAR_DEF

PUBLIC	b_poweroff_flg

PUBLIC  uc_block_init_pageaddr
PUBLIC	uc_block_source_blkaddr
PUBLIC	uc_block_target_blkaddr
PUBLIC	uc_block_source_zoneaddr
PUBLIC	uc_block_target_zoneaddr
PUBLIC	uc_bootcode_cur_zone
PUBLIC	uc_bootcode_cur_block
PUBLIC	uc_bootcode_zone_bk1
PUBLIC	uc_bootcode_block_bk1
PUBLIC	uc_bootcode_zone_bk2
PUBLIC	uc_bootcode_block_bk2
PUBLIC	uc_bootcode_zone_bk3
PUBLIC	uc_bootcode_block_bk3
PUBLIC	uc_current_rait_address
PUBLIC	uc_others_rait_address
PUBLIC	uc_valid_page_num
PUBLIC	uc_block_copy_cnt
PUBLIC	uc_bootcode_cur_block_h
PUBLIC	uc_bootcode_block_bk1_h
PUBLIC	uc_bootcode_block_bk2_h
PUBLIC	uc_bootcode_block_bk3_h
PUBLIC	uc_block_source_blkaddr_h
PUBLIC	uc_block_target_blkaddr_h

PUBLIC uc_lcd_cmd_table
PUBLIC uc_pagemapping_table

	BSEG	AT	29H.0		;25H.7
b_poweroff_flg:				DBIT	1


	DSEG	AT		12H
uc_block_init_pageaddr:		DS	1
;uc_load_code_cnt:			DS	1
uc_bootcode_cur_zone:		DS	1
uc_bootcode_cur_block:		DS	1
uc_bootcode_zone_bk1:		DS	1
uc_bootcode_block_bk1:		DS	1
uc_bootcode_zone_bk2:		DS	1
uc_bootcode_block_bk2:		DS	1
uc_bootcode_zone_bk3:		DS	1
uc_bootcode_block_bk3:		DS	1
uc_current_rait_address:	DS	1
;uc_others_rait_address:		DS	1
uc_valid_page_num:			DS	1
uc_block_target_blkaddr:	DS	1
uc_block_source_blkaddr:	DS	1
uc_block_source_zoneaddr:	DS	1
	DSEG	AT		46H
uc_block_target_zoneaddr:	DS	1	
	DSEG	AT		7BH
uc_block_copy_cnt:			DS	1
uc_bootcode_cur_block_h:	DS	1
uc_bootcode_block_bk1_h:	DS	1
uc_bootcode_block_bk2_h:	DS	1
uc_bootcode_block_bk3_h:	DS	1


	XSEG	AT		7200H

uc_lcd_cmd_table:			DS	15
uc_pagemapping_table:		DS	256
uc_block_source_blkaddr_h:	DS	1
uc_block_target_blkaddr_h:	DS	1
uc_others_rait_address:		DS	1				
#endif
END