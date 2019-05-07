/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_import_telbook.c
Creator: zj.zong					Date: 2009-06-05
Description: 
Others:
Version: V0.1
History:
	V0.1	2009-06-05		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_TELBOOK_IMPORTE_DEF

#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_api_define.h"
#include "..\..\UI\UI_TelBook\ui_telbook_header.h"
#include "..\..\UI\UI_TelBook\ui_telbook_var_ext.h"

extern void FS_WrFlash_Interface(void);
extern void FS_Fread(void);
extern void FS_Fdelete(void);

void Ap_Import_TelFile(void);
void Ap_Init_EF_LogicAddr(void);
/*******************************************************************************
Function: Ap_Import_TelFile()
Description: import telbook file
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Import_TelFile(void)
{
	uc8 i;

/*write bin file into the flash reserved area*/
	for(i = 0; i < 50; i++)			//bin file length is 25k = 50sector
	{
		uc_fs_assign_buf_flg = SFC_APP_DBUF1;	//assign dbuf1(0x5000) to fs
		FS_Fread();
		
		Ap_Init_EF_LogicAddr();
		sfc_logblkaddr0 = TELBOOK_EF_START_ADDR + i;
		sfc_reserved_area_flag = 1;					//read flash reserved area
		buf_offset0 = 0x00;							//offset is 1k byte
		buf_offset1 = 0x00 ;
		sfc_occ_buf_flag = SFC_APP_DBUF1;			//occupy dbuf 0x5000
		if(i == 49)
		{
			b_sfc_immediate_wr_flg = 1;
		}
		else
		{
			b_sfc_immediate_wr_flg = 0;
		}
		FS_WrFlash_Interface();
		sfc_reserved_area_flag = 0;					//clr reserved area flag
	}
	
/*delete tel_save.bin file*/
	FS_Fdelete();
}

/*******************************************************************************
init ef logic address
********************************************************************************/
void Ap_Init_EF_LogicAddr(void)
{
	sfc_logblkaddr1 = 0x00;
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
}

#endif