/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: top_menu_var.asm
Creator: zj.Zong					Date: 2007-12-07
Description: define variable be used in top menu mode
Others: 
Version: V0.1
History:
	V0.1	2007-12-07		zj.Zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_MAINMENU_VAR_DEF

/*****************************************/
#ifndef MAINMENU_VAR
#define MAINMENU_VAR


PUBLIC	uc_menu_cur_idx
PUBLIC	uc_menu_total_num


	XSEG	AT		7200H

uc_menu_cur_idx:		DS	1
uc_menu_total_num:		DS	1


#endif
#endif
	END
