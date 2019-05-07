/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_telbook_init.c
Creator: zj.zong				Date: 2009-06-05
Description: init telbook (init variable)
Others:
Version: V0.1
History:
	V0.1	2009-06-05		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_TELBOOK_INIT_DEF

#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_api_define.h"
#include "..\..\UI\UI_TelBook\ui_telbook_header.h"
#include "..\..\UI\UI_TelBook\ui_telbook_var_ext.h"


extern void _media_readsector(void);
void Ap_Init_TelBookVar(void);

extern void _media_readsector(void);

extern void Ap_Init_EF_LogicAddr(void);
extern void Ap_Exchange_HighLowByte(void);
void Ap_Init_TelFile(void);

/*******************************************************************************
Function: Ap_Init_TelFile()
description: init telbook
             1. init telbook variable
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
ui16 code tmp_name_offset[4] = {0, 0x80, 0x100, 0x180};
void Ap_Init_TelFile(void)
{
	uc8 i, j, k;
	ui16 tmp_name_idx = 0;
	
	uc_tel_namelist_num = 0;
	Ap_Init_EF_LogicAddr();
	for(i = 0; i < 50; i++)
	{
		sfc_logblkaddr0 = TELBOOK_EF_START_ADDR + i;
		sfc_reserved_area_flag = 1;					//read flash reserved area
		buf_offset0 = 0x00;							//buffer offset: 0x0800
		buf_offset1 = 0x00;
		sfc_occ_buf_flag = SFC_APP_DBUF1;			//accupy dbuf(0x5000)	
		_media_readsector();
		sfc_reserved_area_flag = 0;					//clr reserved area flag

		Ap_Exchange_HighLowByte();
		for(j = 0; j < 4; j++)
		{
			if(uc_dbuf1[tmp_name_offset[j]] == 0)
			{
				break;
			}
			if((uc_dbuf1[tmp_name_offset[j]]==0xff) && (uc_dbuf1[tmp_name_offset[j]+1]==0xff))
			{
				break;
			}
			
			for(k = 0; k < 16; k++)
			{
				uc_dbuf0[tmp_name_idx] = uc_dbuf1[tmp_name_offset[j] + k];
				tmp_name_idx++;
			}
			uc_tel_namelist_num++;
		}
	}
}

#endif