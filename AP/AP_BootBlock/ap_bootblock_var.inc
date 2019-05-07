/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_bootblock_var.inc
Creator: zj.zong					Date: 2009-04-13
Description: declare variable be used in bootblock
Others: 
Version: V0.1
History:
	V0.1	2009-04-13		zj.zong
			1. build this module
********************************************************************************/
#ifndef	AP_BOOTBLOCK_HEADER_DEF
#define AP_BOOTBLOCK_HEADER_DEF

EXTRN	BIT		(b_poweroff_flg)

EXTRN   DATA	(uc_block_init_pageaddr)
EXTRN	DATA	(uc_block_source_blkaddr)
EXTRN	DATA	(uc_block_target_blkaddr)
EXTRN	DATA	(uc_block_source_zoneaddr)
EXTRN	DATA	(uc_block_target_zoneaddr)
EXTRN	DATA	(uc_bootcode_cur_zone)
EXTRN	DATA	(uc_bootcode_cur_block)
EXTRN	DATA	(uc_bootcode_zone_bk1)
EXTRN	DATA	(uc_bootcode_block_bk1)
EXTRN	DATA	(uc_bootcode_zone_bk2)
EXTRN	DATA	(uc_bootcode_block_bk2)
EXTRN	DATA	(uc_bootcode_zone_bk3)
EXTRN	DATA	(uc_bootcode_block_bk3)
EXTRN	DATA	(uc_current_rait_address)
EXTRN	XDATA	(uc_others_rait_address)
EXTRN	DATA	(uc_valid_page_num)
EXTRN	DATA	(uc_block_copy_cnt)

EXTRN	DATA	(uc_bootcode_cur_block_h)
EXTRN	DATA	(uc_bootcode_block_bk1_h)
EXTRN	DATA	(uc_bootcode_block_bk2_h)
EXTRN	DATA	(uc_bootcode_block_bk3_h)
EXTRN	XDATA	(uc_block_source_blkaddr_h)
EXTRN	XDATA	(uc_block_target_blkaddr_h)

EXTRN	XDATA	(uc_lcd_cmd_table)
EXTRN	XDATA	(uc_pagemapping_table)
#endif