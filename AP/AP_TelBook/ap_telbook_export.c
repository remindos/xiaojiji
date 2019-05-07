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
#ifdef AP_TELBOOK_EXPORT_DEF

#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_api_define.h"
#include "..\..\UI\UI_TelBook\ui_telbook_header.h"
#include "..\..\UI\UI_TelBook\ui_telbook_var_ext.h"

extern bit FS_Fcreate(void);
extern void _media_readsector();
extern void FS_Fwrite(void);
extern void FS_Fwrite_Header(void);
extern void FS_Fwrite_Over(void);
extern void FS_Fdelete(void);
extern void FS_GetSpace(void);

void Ap_Export_TelFile(void);
void Ap_Export_TelFileSub(void);
void Ap_Init_EF_LogicAddr(void);

/*******************************************************************************
export telbook
********************************************************************************/
void Ap_Export_TelFile(void)
{ 	
	//FS_GetSpace();
	if(ul_fs_spare_space >= 50)
	{
		if(FS_Fcreate() == 1)
		{
			Ap_Export_TelFileSub();
			b_export_file_ok = 1;  //export file is ok
		//	return ;	
		}
		else
		{
		   	b_export_file_ok = 0; //create file is failed
		//	return ;	
		}
	}
	else
	{
 		b_export_file_ok = 0;	//no space
	//	return ;		
	}
}


void Ap_Export_TelFileSub(void)
{
	uc8 i;
	for(i = 0; i < 50; i++)
	{
		Ap_Init_EF_LogicAddr();
		sfc_logblkaddr0 = TELBOOK_EF_START_ADDR + i;
		sfc_reserved_area_flag = 1;					//read flash reserved area
		sfc_occ_buf_flag = SFC_APP_CBUF;			//accupy cbuf(0x3000~0x31FF)	
		buf_offset0 = (uc8)REC_ENC_BUF_OFFSET_W;
		buf_offset1 = (uc8)(REC_ENC_BUF_OFFSET_W >> 8);
		_media_readsector();
		sfc_reserved_area_flag = 0;					//clr reserved area flag

		uc_fs_assign_buf_flg = SFC_APP_CBUF;		//write cbuf
		FS_Fwrite();
	}
	
	Ap_Init_EF_LogicAddr();
	sfc_logblkaddr0 = TELBOOK_EF_START_ADDR;
	sfc_reserved_area_flag = 1;					//read flash reserved area
	sfc_occ_buf_flag = SFC_APP_FBUF;			//accupy Fbuf
	buf_offset0 = (uc8)FCOMBUF_OFFSET_W;		//buffer offset :0x0000
	buf_offset1 = (uc8)(FCOMBUF_OFFSET_W >> 8);
	_media_readsector();
	sfc_reserved_area_flag = 0;					//clr reserved area flag

	FS_Fwrite_Header();				//write header (ef common buffer)
	FS_Fwrite_Over();				//write fat bufer
}

/*******************************************************************************
init ef logic address
*******************************************************************************/
void Ap_Init_EF_LogicAddr(void)
{
	sfc_logblkaddr1 = 0x00;
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
}

#endif