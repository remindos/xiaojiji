/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_telbook_enhance.c
Creator: zj.zong					Date: 2009-06-05
Description: 
Others:
Version: V0.1
History:
	V0.1	2009-06-05		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_CHECK_TEL_FILE_DEF

#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_api_define.h"
#include "..\..\UI\UI_TelBook\ui_telbook_header.h"
#include "..\..\UI\UI_TelBook\ui_telbook_var_ext.h"

extern void FS_EnterRootDir(void);
extern void FS_GetItemNum_InDir(void);
extern bit FS_Chk_FileType(void);
extern bit FS_Fseek(ul32 offset);
extern bit FS_Fopen(ui16 file_idx);
extern void FS_Get_83Name(void);
extern void FS_Fread(void);

void Ap_Check_TelFile_Exist(void);
bit Ap_Check_TelbookSaveBin(void);
void Ap_Exchange_HighLowByte(void);

/*******************************************************************************
Function: Ap_Check_TelFile_Exist()
Description: : 1 exist , 0 no exist
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Check_TelFile_Exist(void)
{
	ui16 i ;
	
	FS_EnterRootDir();					//enter root
	FS_GetItemNum_InDir();
	
	for(i = 0; i < ui_fs_file_num_indir; i++)
	{
		FS_Fopen(i);
		if(FS_Chk_FileType() == 1)
		{
			FS_Get_83Name();
			if(Ap_Check_TelbookSaveBin() == 1)
			{
				b_telbook_file_exist = 1;  	//TEL_SAVE.BIN file is existed
			   	return ;	
			} 
		}
	}
	b_telbook_file_exist = 0;	//have not find bin file
}

/*******************************************************************************
check save telbook bin
********************************************************************************/
bit Ap_Check_TelbookSaveBin(void)
{
	if((uc_fs_item_83_name[0]=='T') && (uc_fs_item_83_name[1]=='E') && (uc_fs_item_83_name[2]=='L') &&
	   (uc_fs_item_83_name[3]=='_') && (uc_fs_item_83_name[4]=='S') && (uc_fs_item_83_name[5]=='A') &&
	   (uc_fs_item_83_name[6]=='V') && (uc_fs_item_83_name[7]=='E'))
	{
		if(ul_fs_file_length == 0x6400)		//tel_save.bin file length is 25k bytes
		{
			FS_Fseek(0x6200);
			uc_fs_assign_buf_flg = SFC_APP_DBUF1;	//assign dbuf1(0x5000) to fs
			FS_Fread();
			Ap_Exchange_HighLowByte();
			if((uc_dbuf1[0x1e0]==0x55) && (uc_dbuf1[0x1e1]==0xAA) &&
			   (uc_dbuf1[0x1ee]==0x55) && (uc_dbuf1[0x1ef]==0xAA))
			{
				FS_Fseek(0);
				return 1;		//file ok
			}
		}
		else
		{
			return 0;			//file error
		}
	}
	else
	{
		return 0;				//file error
	}
}

/*******************************************************************************
exchange high with low byte
********************************************************************************/
void Ap_Exchange_HighLowByte(void)
{
	uc8 tmp_var;
	ui16 i;

	for(i = 0; i < 256; i++)
	{
		tmp_var = uc_dbuf1[i * 2];
		uc_dbuf1[i * 2] = uc_dbuf1[i * 2 + 1];
		uc_dbuf1[i * 2 + 1] = tmp_var;
	}
}

#endif
