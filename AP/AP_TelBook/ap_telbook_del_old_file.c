/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_telbook_del_old_file.c
Creator: zj.zong					Date: 2009-06-05
Description: 
Others:
Version: V0.1
History:
	V0.1	2009-06-05		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_TELBOOK_DEL_OLD_FILE_DEF

#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_api_define.h"
#include "..\..\UI\UI_TelBook\ui_telbook_header.h"
#include "..\..\UI\UI_TelBook\ui_telbook_var_ext.h"

extern bit  FS_Fopen(ui16 file_idx);
extern bit  FS_Chk_FileType(void);
extern bit  FS_Fcreate(void);
extern void FS_Get_83Name(void);
extern void FS_EnterRootDir(void);
extern void FS_GetItemNum_InDir(void);
extern void FS_Fdelete(void);

void Ap_Delete_Old_BK_File(void);

/*******************************************************************************
delete old telbook file
*******************************************************************************/
void Ap_Delete_Old_BK_File(void)
{
	ui16 i ;
	
//	FS_EnterRootDir();					//enter root
//	FS_GetItemNum_InDir();
	
	for(i = 0; i < ui_fs_file_num_indir; i++)
	{
		FS_Fopen(i);
		if(FS_Chk_FileType() == 1)
		{
			FS_Get_83Name();
			
			if((uc_fs_item_83_name[0]=='T') && (uc_fs_item_83_name[1]=='E') && (uc_fs_item_83_name[2]=='L') &&
			   (uc_fs_item_83_name[3]=='_') && (uc_fs_item_83_name[4]=='B') && (uc_fs_item_83_name[5]=='A') &&
			   (uc_fs_item_83_name[6]=='K'))
			{
				FS_Fdelete();
				break;
			}
		}
	}
}

#endif