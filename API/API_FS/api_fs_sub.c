/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: fs_api_sub.c
Creator: andyliu					Date: 2008-03-20
Description: file system api subfunction
Others:
Version: V0.1
History:
	V0.1	2008-03-20		andyliu
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"

#ifdef FILE_SYSTEM_FUNC_DEF
#include "api_fs_var_ext.h"
#include "api_fs.h"


extern void _get_item_idx_from_type_idx(ui16 index, uc8 type);

#ifdef BEG_CLST_ADDR_DEF
/*******************************************************************************
Function: dir_flag_addr()
Description:
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void dir_flag_addr(void)
{
	if(uc_fs_subdir_stk_pt == 0)
	{
		if(b_fs_fat_type_flg == 0)
		{
			b_fs_dir_flg = 0;
			b_fs_fat32_root_flg = 0;
		}
		else
		{
			b_fs_dir_flg = 1;
			b_fs_fat32_root_flg = 1;
			uc_fs_begin_cluster1 = 2;
			uc_fs_begin_cluster2 = 0;
			uc_fs_begin_cluster3 = 0;
		}
	}
	else //if ((uc_fs_subdir_stk_pt>0)&&(uc_fs_subdir_stk_pt< = FS_MAX_SUBDIR_DEPTH))
	{
		b_fs_dir_flg = 1;
		b_fs_fat32_root_flg = 0;
		uc_fs_begin_cluster1 = uc_fs_subdir_stk_buf[(uc_fs_subdir_stk_pt - 1) * FS_BYTE_PER_LAYER + 2];
		uc_fs_begin_cluster2 = uc_fs_subdir_stk_buf[(uc_fs_subdir_stk_pt - 1) * FS_BYTE_PER_LAYER + 3];
		if(b_fs_fat_type_flg == 1)
		{
			uc_fs_begin_cluster3 = uc_fs_subdir_stk_buf[(uc_fs_subdir_stk_pt - 1) * FS_BYTE_PER_LAYER + 4];
		}
	}
}
#endif



#ifdef SRH_FILE
/*******************************************************************************
Function: get_item_index()
Description:according to dir indix or file index,calculate item index in current directory
Calls:
Global:type: 1--file
             2--directory
Input: Null
Output:bit 0--fail
           1--ok
Others:
********************************************************************************/
bit get_item_index(ui16 index, uc8 type)
{ 
	dir_flag_addr();
	
	_get_item_idx_from_type_idx(index, type);

	return (b_fs_func_flg);
}
#endif

#endif