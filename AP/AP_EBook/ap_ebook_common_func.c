 /*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_ebook_common_func.c
Creator: zj.zong			Date: 2009-08-19
Description: ebook common func
Others:
Version: V0.1
History:
	V0.1	2009-08-19		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_EBOOK_COMMON_FUNC_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_EBook\ui_ebook_header.h"
#include "..\..\UI\UI_EBook\ui_ebook_var_ext.h"

extern bit FS_Fread(void);
extern ui16 Api_Isn_ConvertTo_Unicode(ui16 isn_index);
extern ui16 Api_Merge_2Byte_Sub(uc8 lo,uc8 hi);


/*******************************************************************************
exchange high with low byte
********************************************************************************/
void Ap_Exchange_HighLow_Byte(void)
{
	ui16 i;
	uc8 tmp_var;
	for(i = 0; i < 256; i++)
	{
		tmp_var = uc_dbuf1[i * 2];
		uc_dbuf1[i * 2] = uc_dbuf1[i * 2 + 1];
		uc_dbuf1[i * 2 + 1] = tmp_var;
	}
}


/*******************************************************************************
read one byte "ISN", ui_ebook_byte_idx+1, appoint to next ISN
b_ebook_file_end: 1--file end
********************************************************************************/
void Ap_Get_OneByte(void)
{
	if(ui_ebook_byte_idx >= 512)
	{
		ui_ebook_byte_idx = 0;
		uc_fs_assign_buf_flg = SFC_APP_DBUF1;	//assign dbuf(0x5000) to fs
		if(FS_Fread() == 0)
		{
			b_ebook_file_end = 1;			//read error or file end
			return;
		}
		
		if(uc_ebook_unicode_flg != 1)
		{
			Ap_Exchange_HighLow_Byte();
		}
	}
	
	if(uc_ebook_unicode_flg == 0)	//ascii
	{
		ui_ebook_val = (ui16)uc_dbuf1[ui_ebook_byte_idx];
		ui_ebook_byte_idx++;		//read one byte ok
	}
	else							//unicode
	{
		ui_ebook_val = Api_Merge_2Byte_Sub(uc_dbuf1[ui_ebook_byte_idx+1],uc_dbuf1[ui_ebook_byte_idx]);
		ui_ebook_byte_idx += 2;		//read one byte ok(unicode is 2bytes)
	}
	
	if((ui_ebook_val == 0) && (uc_dbuf1[ui_ebook_byte_idx+1] == 0))
	{
		b_ebook_file_end = 1;		//file end
	}
	else
	{
		b_ebook_file_end = 0;
	}
}


#endif