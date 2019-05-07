/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_telbook_var.asm
Creator: zj.zong				Date: 2009-06-04
Description: define variable be used in telbook mode
Others: 
Version: V0.1
History:
	V0.1	2009-06-04		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#include "sh86278_memory_assign.h"
#ifdef UI_TELBOOK_VAR_DEF

/*****************************************/
#ifndef TELBOOK_VAR
#define TELBOOK_VAR

PUBLIC b_telbook_file_exist
PUBLIC b_export_file_ok
;PUBLIC b_sd_flash_flg
PUBLIC b_has_real_data_flg

PUBLIC	uc_menu_cur_idx
PUBLIC	uc_menu_total_num
PUBLIC	uc_menu_exit_cnt
PUBLIC	uc_tel_namelist_top_idx
PUBLIC	uc_tel_namelist_sel_idx
PUBLIC	uc_tel_namelist_num
PUBLIC	uc_tel_dis_info_byteoffet
PUBLIC	uc_tel_info_length
PUBLIC	uc_tel_dis_info_buf
PUBLIC	uc_menu_top_idx


PUBLIC	ui_tel_string_idx


	BSEG	AT		28H.5

b_telbook_file_exist:		DBIT	1
b_export_file_ok:			DBIT	1
;b_sd_flash_flg:				DBIT    1
b_has_real_data_flg:		DBIT	1


DSEG			AT		14H
uc_menu_cur_idx:			DS		1
uc_menu_total_num:			DS		1
uc_tel_namelist_top_idx:	DS		1
uc_tel_namelist_sel_idx:	DS		1
uc_tel_namelist_num:		DS		1
uc_menu_exit_cnt:			DS		1
uc_tel_dis_info_byteoffet:	DS		1
ui_tel_string_idx:			DS		2
uc_tel_info_length:			DS		1
uc_menu_top_idx:			DS		1

XSEG		AT		6000H

uc_tel_dis_info_buf:		DS		16


XSEG		AT		X_ADDR_FS_FATBUF
uc_fs_fatbuf:	DS		512

#endif
#endif


END