/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_bootblock_fm_var.asm
Creator: zj.zong					Date: 2009-08-23
Description:
Others:
Version: V0.1
History:
	V0.1	2009-08-23		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#include "ap_bootblock_fm.h"
#ifdef AP_BOOTBLOCK_FM_VAR_DEF

PUBLIC uc_chip_address
PUBLIC	uc_fm_write_bytes
PUBLIC	uc_fm_read_bytes

	DSEG	AT		12H
;

	XSEG	AT		7320H
uc_chip_address:			DS		1
uc_fm_write_bytes:			DS		68
uc_fm_read_bytes:			DS		68


#endif
	END