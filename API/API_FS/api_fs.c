/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: fs_api.c
Creator: andyliu					Date: 2008-03-20
Description: file system api functions
Others:
Version: V0.6
History:
	V0.6	2010-06-12		andyliu
			1. delete function "_check_ef_addr_ok()"
	V0.5	2009-07-24		andyliu
			1. modify "expand capacity", add function _check_ef_addr_ok()
	V0.4	2009-02-09		andyliu
			1. modify "fs_fread()", for ef extended capacity
	V0.3	2008-05-22		zhanghuan
			1. add subroutine "FS_Init_OpenVar()" in part of functions
			2. add subroutine "FS_Init_OpenVar()"
	V0.2	2008-05-12		andyliu
			1. modify fs_fopen_appointed(), get "si_fs_file_idx"
	V0.1	2008-03-20		andyliu
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"

#ifdef FILE_SYSTEM_FUNC_DEF
#include "api_fs_var_ext.h"
#include "api_fs.h"

extern void dir_flag_addr(void);
extern bit	get_item_index(ui16 index, uc8 type);
extern void _seek_next_sector(void);
extern void _read_sect_asm(void);
extern void _get_item_number(void);
extern void _first_dec_2_fat(void);
extern void _fwrite_one_sector(void);
extern void _wr_fatbuf_2_card(void);
extern void _update_file_entry(void);
extern void	_get_83name(void);
extern void _get_item_par(void);
extern void _enter_subdir(ui16 dir_index);
extern void _wr_wav_rec_fat(void);
extern void _wr_wav_rec_entry(void);
extern void _wr_dir_dot(void);
extern void _delete_file(void);
extern void _write_file_header(void);
extern void _get_free_sector_space(void);
extern void _get_cluster_fir_sector(void);
extern void _cal_sector_address_1(void);
extern void _fs_read_sector_2_point(void);
extern bit _get_last_sec_in_cluster(void);
extern bit _fs_boot_sector_sub(void);
extern bit _fs_fseek_sub(ul32 offset);
extern bit _fetch_new_sector(void);
extern bit _get_long_file_name(void);
extern bit _seek_free_cluster(uc8 fat_addr_lo, uc8 fat_addr_hi);
extern bit _chk_file_type(void);
extern bit _write_file_entry(void);
extern bit 	b_sd_flash_flg; //allen
extern uc8 _srh_wavdir_in_root(void);
extern uc8 _sec_to_clu_exponent(void);
extern ui16 _merge_2_byte(uc8 lo,uc8 hi);
extern ui16 _FS_ExitSubdir(void);
void FS_Init_OpenVar(void);
void FS_Update_Srhfat_Sectoraddr(void);

#ifdef FS_POWER_ON_BOOT
/*******************************************************************************
Function: FS_Finit()
Description: boot sector, check flash
Calls:
Global:
Input: Null
Output:bit 0--flash error
           1--flash ok
Others:
********************************************************************************/
bit	FS_Finit(void)
{
	if(_fs_boot_sector_sub()==0)
	{
		return 0;
	}	
	
	return 1;
}
#endif

#ifdef UPDATE_NF_SRH_SECTOR
void FS_Update_NFSrhSector(void)
{		
	uc_fs_fat_srh_addr1 = uc_fs_fat1_begin_1;
	uc_fs_fat_srh_addr2 = uc_fs_fat1_begin_2;
}
#endif

#ifdef UPDATE_SD_SRH_SECTOR
void FS_Update_SDSrhSector(void)
{		
	uc_fs_sd_fat_srh_addr1 = uc_fs_fat1_begin_1;
	uc_fs_sd_fat_srh_addr2 = uc_fs_fat1_begin_2;
}
#endif



#ifdef FS_GET_ITEM_NUM_DEF
/*******************************************************************************
Function: FS_GetItemNum_InDir()
Description: calculate item number,subdir number in current directory
Calls:
Global:ITEM_NUM1,ITEM_NUM2,SUBDIR_NUM1,SUBDIR_NUM2
Input: Null
Output:Null
Others:
********************************************************************************/
void FS_GetItemNum_InDir()
{
	dir_flag_addr();
	_get_item_number();
	ui_fs_file_num_indir = ui_fs_item_num - ui_fs_subdir_num;
}
#endif



#ifdef FS_FSEEK_DEF
/*******************************************************************************
Function: FS_Fseek()
Description: 1. only support seek from the beginning of file
             2. offset must be the multiple of 512
Calls:
Global:ul_fs_byte_offset
Input: Null
Output:Null
Others:
********************************************************************************/
bit FS_Fseek(ul32 offset)
{
	if(_fs_fseek_sub(offset) == 0)
	{
		return 0;
	}

	ul_fs_byte_offset = offset;
	uc_fs_file_cur_clst2 = uc_fs_cur_cluster2;
	uc_fs_file_cur_clst2_bk = uc_fs_cur_cluster2;
	uc_fs_file_cur_clst1 = uc_fs_cur_cluster1;
	uc_fs_file_cur_clst1_bk = uc_fs_cur_cluster1;
	if(b_fs_fat_type_flg == 1)
	{
		uc_fs_file_cur_clst3 = uc_fs_cur_cluster3;
		uc_fs_file_cur_clst3_bk = uc_fs_cur_cluster3;
	}
	uc_fs_file_cur_sec_in_clst = uc_fs_cur_sec_in_cluster;
	uc_fs_file_cur_sec_in_clst_bk = uc_fs_cur_sec_in_cluster;

	return 1;
}
#endif



#ifdef FS_FREAD_DEF
/*******************************************************************************
Function: FS_Fread()
Description: now every time read file data, must be 512 byte in fixed memory area,
             so use file_cur_clst_hi, file_cur_clst_lo and uc_fs_file_cur_sec_in_clst record 
             where the file has been read 
Calls:
Global:uc_fs_file_cur_sec_in_clst,file_cur_clst_lo,file_cur_clst_hi
Input: Null
Output:bit 0--file end or read error
           1--read one sector ok
Others:
********************************************************************************/
bit FS_Fread(void)
{
	b_fs_file_err_flg = 0;
	
	if(ul_fs_byte_offset >= ul_fs_file_length)
	{
		return 0;
	}

	//read out the sector file_cur_clst_hi,file_cur_clst_lo 
	//input:uc_fs_file_cur_sec_in_clst,file_cur_clst_hi,file_cur_clst_lo
	_read_sect_asm();
	ul_fs_byte_offset += 512;

/*check logic addr is overflow for ef extended capacity*/
#ifdef SP_SD_DRIVER
	if(b_sd_flash_flg == 0)
	{
		if(sfc_errcode == 0xFF)			//=0xFF, Addr Overflow
		{
			return 0;
		}
	}
#else
	if(sfc_errcode == 0xFF)			//=0xFF, Addr Overflow
	{
		return 0;
	}
#endif

/*backup current cluster and sector offset in cluster, get next sector addr info*/
	uc_fs_cur_sec_in_cluster = uc_fs_file_cur_sec_in_clst;		//save in uc_fs_cur_sec_in_cluster,uc_fs_cur_cluster1,uc_fs_cur_cluster2
	uc_fs_cur_cluster1 = uc_fs_file_cur_clst1;
	uc_fs_cur_cluster2 = uc_fs_file_cur_clst2;
	
	uc_fs_file_cur_sec_in_clst_bk = uc_fs_file_cur_sec_in_clst;	//backup current cluster and sector offset in cluster
	uc_fs_file_cur_clst1_bk = uc_fs_file_cur_clst1;
	uc_fs_file_cur_clst2_bk = uc_fs_file_cur_clst2;
	
	if(b_fs_fat_type_flg == 1)
	{
		uc_fs_cur_cluster3 = uc_fs_file_cur_clst3;
		uc_fs_file_cur_clst3_bk = uc_fs_file_cur_clst3;
	}
	
	_seek_next_sector();				//only change par(uc_fs_cur_sec_in_cluster,uc_fs_cur_cluster1,2),no read data
	if(b_fs_file_err_flg == 1)
	{
		return 0;
	}
	
	//update uc_fs_file_cur_sec_in_clst,file_cur_clst_lo,file_cur_clst_lo to next xsector
	uc_fs_file_cur_sec_in_clst = uc_fs_cur_sec_in_cluster;
	if(b_fs_fat_type_flg == 1)
	{
		uc_fs_file_cur_clst3 = uc_fs_cur_cluster3;
	}
	uc_fs_file_cur_clst2 = uc_fs_cur_cluster2;
	uc_fs_file_cur_clst1 = uc_fs_cur_cluster1;
	
	return 1;
}
#endif



#ifdef FS_GET_PREVIOUS_SECTOR_DEF
/*******************************************************************************
Function: FS_Get_PreviousSector()
Description:
Calls:
Global:
Input: Null
Output:bit 0--file end or read error
           1--read one sector ok
Others:
********************************************************************************/
bit FS_Get_PreviousSector(void)
{
	if(ul_fs_byte_offset <= 512)
	{
		return 0;
	}
/*backup the next sector addr info*/
	uc_fs_file_cur_sec_in_clst = uc_fs_file_cur_sec_in_clst_bk;
	uc_fs_file_cur_clst1 = uc_fs_file_cur_clst1_bk;
	uc_fs_file_cur_clst2 = uc_fs_file_cur_clst2_bk;
	if(b_fs_fat_type_flg == 1)
	{
		uc_fs_file_cur_clst3 = uc_fs_file_cur_clst3_bk;
	}

/*get current logic block addr(sfc_logblkaddr2, sfc_logblkaddr1, sfc_logblkaddr0)*/
	uc_fs_cur_cluster1 = uc_fs_file_cur_clst1_bk;
	uc_fs_cur_cluster2 = uc_fs_file_cur_clst2_bk;
	uc_fs_cur_cluster3 = uc_fs_file_cur_clst3_bk;
	_get_cluster_fir_sector();
	uc_fs_cur_sec_in_cluster = uc_fs_file_cur_sec_in_clst_bk;
	_cal_sector_address_1();

/*get the previous sector addr and data*/
	if(_get_last_sec_in_cluster() == 1)
	{
		uc_fs_file_cur_sec_in_clst_bk = uc_fs_cur_sec_in_cluster;	//backup current sector and info
		uc_fs_file_cur_clst1_bk = uc_fs_cur_cluster1;
		uc_fs_file_cur_clst2_bk = uc_fs_cur_cluster2;
		if(b_fs_fat_type_flg == 1)
		{
			uc_fs_file_cur_clst3_bk = uc_fs_cur_cluster3;
		}
		_fs_read_sector_2_point();
		ul_fs_byte_offset -= 512;
			
		return 1;
	}
	
	return 0;
}
#endif



#ifdef FS_FWRITE_DEF
/*******************************************************************************
Function: FS_Fwrite()
Description: 
Calls:
Global:
Input: Null
Output:bit 0--fail
           1--write one sector ok
Others:
********************************************************************************/
bit FS_Fwrite(void)
{
	if(b_fs_first_wr_file_flg == 1)
	{
		b_fs_first_wr_file_flg = 0;
		_first_dec_2_fat();
	}
	else
	{
		if(_fetch_new_sector() == 0)
		{
			return 0;
		}
	}
	
	_fwrite_one_sector();
	ul_fs_byte_offset += 512;
	
	return 1;
}
#endif



#ifdef FS_FWRITE_OVER_DEF
/*******************************************************************************
Function: FS_Fwrite_Over()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void FS_Fwrite_Over(void)
{
	_wr_fatbuf_2_card();
	_update_file_entry();
}
#endif



#ifdef FS_FWRITE_HEADER_DEF
/*******************************************************************************
Function: FS_Fwrite_Header()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void FS_Fwrite_Header(void)
{
	_write_file_header();
}
#endif



#ifdef FS_SEC_TO_CLUSTER_DEF
/*******************************************************************************
Function: FS_Sector_To_Cluster()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
uc8 FS_Sector_To_Cluster(void)
{
	return(_sec_to_clu_exponent());
}
#endif



#ifdef FS_GET_LONG_NAME_DEF
/*******************************************************************************
Function: FS_Get_LongName()
Description: 
Calls:
Global:Null
Input: Null
Output:bit 0--have not long file name
           1--it's long file name
Others:
********************************************************************************/
bit FS_Get_LongName(void)
{
	if(_get_long_file_name() == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
#endif



#ifdef FS_GET_83_NAME_DEF
/*******************************************************************************
Function: fs_get_83name()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void FS_Get_83Name(void)
{
	_get_83name();
}
#endif



#ifdef FS_GET_FREE_SPACE_DEF
/*******************************************************************************
Function: FS_GetSpace()
Description:get the free capacity in flash disk(the unit is sector)
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void FS_GetSpace(void)
{
	_get_free_sector_space();
}
#endif




#ifdef FS_OPEN_DIR_DEF
/*******************************************************************************
Function: FS_OpenDir()
Description:
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
bit FS_OpenDir(ui16 dir_index)
{
	FS_Init_OpenVar();
	if(get_item_index(dir_index, 2) == 0)
	{
		return 0;
	}
	
	uc_fs_item_type = 2;		//current item type is folder
	_get_item_par();
	return (b_fs_func_flg);
}
#endif



#ifdef FS_ENTER_ROOT_DEF
/*******************************************************************************
Function: FS_EnterRootDir()
Description:enter root,the item in root uc_fs_subdir_stk_pt is 0
            clear uc_fs_subdir_stk_buf
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void FS_EnterRootDir(void)
{
	uc8 i;
	
	uc_fs_subdir_stk_pt = 0;
	FS_Init_OpenVar();
	if(b_fs_fat_type_flg == 1)
	{
		b_fs_fat32_root_flg = 1;
		b_fs_dir_flg = 1;
	}
	else
	{
		b_fs_fat32_root_flg = 0;
	}

    for(i = 0; i < FS_MAX_SUBDIR_DEPTH * FS_BYTE_PER_LAYER; i++)
    {
        uc_fs_subdir_stk_buf[i] = 0;
    }
}
#endif



#ifdef  FS_ENTER_DIR_DEF
/*******************************************************************************
Function: FS_EnterSubDir()
Description:enter assigned dir index directory,update subdir_stack_pt and subdir_stack
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
bit FS_EnterSubDir(ui16 dir_index)
{
	FS_Init_OpenVar();
	if(get_item_index(dir_index, 2) == 0)
	{
		return 0;
    }
	_enter_subdir(dir_index);
	FS_Init_OpenVar();
	
	return (b_fs_func_flg);
}
#endif



#ifdef  FS_EXIT_DIR_DEF
/*******************************************************************************
Function: FS_ExitSubDir()
Description:exit subdir
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
ui16 FS_ExitSubDir(void)
{
	FS_Init_OpenVar();
	return (_FS_ExitSubdir());
}
#endif



#ifdef FS_GET_FILEPATH_DEF
/*******************************************************************************
Function: FS_GetFilePath()
Description:
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void FS_GetFilePath(void)
{
	uc8 i;
	
	ui_fs_dir_path_buf[0] = uc_fs_subdir_stk_pt;
	
	for (i = 0; i < FS_MAX_SUBDIR_DEPTH; i++)
	{
		ui_fs_dir_path_buf[i+1] = _merge_2_byte(uc_fs_subdir_stk_buf[FS_BYTE_PER_LAYER*i+1], uc_fs_subdir_stk_buf[FS_BYTE_PER_LAYER*i]);
	}
}
#endif



#ifdef FS_ENTER_APPOINTED_DIR_DEF
/*******************************************************************************
Function: FS_EnterDir_Appointed()
Description:get into the dir pointed by ui_fs_dir_path_buf
            the structure of ui_fs_dir_path_buf:
	        ui_fs_dir_path_buf[0]:the current path depth , ui_fs_dir_path_buf[..]:the path info
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void FS_EnterDir_Appointed(void)
{
	ui16 i;
	
	FS_EnterRootDir();
	
	for (i = 0; i < ui_fs_dir_path_buf[0]; i++)
	{
		FS_EnterSubDir(ui_fs_dir_path_buf[i+1]);
	}
}
#endif



#ifdef FS_GET_VOICE_DIR_DEF
/*******************************************************************************
Function: 	FS_Get_VoiceDir_Idx()
Description:
Calls:
Global:Null
Input: Null
Output:
Others:
********************************************************************************/
bit FS_Get_VoiceDir_Idx(void)
{
	uc8 tmp_srh_rst;
	uc8 tmp_fat_srh_addr1, tmp_fat_srh_addr2;

	tmp_srh_rst = _srh_wavdir_in_root();


	if (tmp_srh_rst == 2)
	{
		tmp_fat_srh_addr1 = b_sd_flash_flg?uc_fs_sd_fat_srh_addr1:uc_fs_fat_srh_addr1;
		tmp_fat_srh_addr2 = b_sd_flash_flg?uc_fs_sd_fat_srh_addr2:uc_fs_fat_srh_addr2;
		
		if (_seek_free_cluster(tmp_fat_srh_addr1,tmp_fat_srh_addr2) == 0)
		{
			return 0;
		}	
		/*if (_seek_free_cluster(uc_fs_fat_srh_addr1,uc_fs_fat_srh_addr2) == 0)
		{
			return 0;
		}*/

		_wr_wav_rec_fat();
		_wr_wav_rec_entry();
		_wr_dir_dot();
		
		#ifdef TARGET_Record_1_Init
		FS_Update_Srhfat_Sectoraddr();
		#endif
			
		return 1;
	}
	else if(tmp_srh_rst == 1)
	{
		return 1;
	}
	
	return 0;
}
#endif



#ifdef FS_FOPEN_DEF
/*******************************************************************************
Function: FS_Fopen()
Description:after invoking this function:got file uc_fs_begin_cluster1,uc_fs_begin_cluster2
		  		  uc_fs_cur_cluster1,uc_fs_cur_cluster2
                  ul_fs_file_length1,2,3,4
            file in dir info  ENTRY_CLUSTER1,2,3
                  ENTRY_SECTOR_OFFSET
                  ENTRY_NUM
Calls:
Global:si_fs_file_idx(include all files)
Input: Null
Output:bit 0--fail
           1--ok
Others:
********************************************************************************/
bit FS_Fopen(ui16 tmp_si_fs_file_idx)
{
	if(get_item_index(tmp_si_fs_file_idx, 1) == 0)
	{
		return 0;
	}
	
	uc_fs_item_type = 1;		//current item type is file
	_get_item_par();

	uc_fs_file_cur_clst2 = uc_fs_begin_cluster2;
	uc_fs_file_cur_clst2_bk = uc_fs_begin_cluster2;
	uc_fs_file_cur_clst1 = uc_fs_begin_cluster1;
	uc_fs_file_cur_clst1_bk = uc_fs_begin_cluster1;
	if(b_fs_fat_type_flg == 1)
	{
		uc_fs_file_cur_clst3 = uc_fs_begin_cluster3;
		uc_fs_file_cur_clst3_bk = uc_fs_begin_cluster3;
	}
	uc_fs_file_cur_sec_in_clst = 1;
	uc_fs_file_cur_sec_in_clst_bk = 1;
	ul_fs_byte_offset = 0;

	return (b_fs_func_flg);
}
#endif



#ifdef FS_FOPEN_APPOINTED_DEF
/*******************************************************************************
Function: FS_Fopen_Appointed()
Description:after invoking this function:got file uc_fs_begin_cluster1,uc_fs_begin_cluster2
		  		  uc_fs_cur_cluster1,uc_fs_cur_cluster2
                  ul_fs_file_length1,2,3,4
            file in dir info  ENTRY_CLUSTER1,2,3
                  ENTRY_SECTOR_OFFSET
                  ENTRY_NUM
Calls:
Global:si_fs_file_idx, uc_fs_item_type
Input: Null
Output:bit 0--fail
           1--ok
Others:
********************************************************************************/
bit FS_Fopen_Appointed(ui16 file_index)
{
	si16 i=-1, tmp_si_fs_file_idx;
	tmp_si_fs_file_idx = -1;

	FS_Init_OpenVar();
	
get_appointed_file:
	tmp_si_fs_file_idx++;
	if(get_item_index(tmp_si_fs_file_idx, 1) == 0)
	{
		return 0;
	}
	
	if(_chk_file_type() == 1)
	{
		i++;
		if(i != file_index)
		{
			goto get_appointed_file;
		}
	}
	else
	{
		goto get_appointed_file;
	}
	
	si_fs_file_idx = tmp_si_fs_file_idx;
	uc_fs_item_type = 1;		//current item type is file
	_get_item_par();

	uc_fs_file_cur_clst2 = uc_fs_begin_cluster2;
	uc_fs_file_cur_clst2_bk = uc_fs_begin_cluster2;
	uc_fs_file_cur_clst1 = uc_fs_begin_cluster1;
	uc_fs_file_cur_clst1_bk = uc_fs_begin_cluster1;
	if(b_fs_fat_type_flg == 1)
	{
		uc_fs_file_cur_clst3 = uc_fs_begin_cluster3;
		uc_fs_file_cur_clst3_bk = uc_fs_begin_cluster3;
	}
	uc_fs_file_cur_sec_in_clst = 1;
	uc_fs_file_cur_sec_in_clst_bk = 1;
	ul_fs_byte_offset = 0;

	return (b_fs_func_flg);
}
#endif


#ifdef FS_GET_NEXT_FILE_INDIR_DEF
/*******************************************************************************
Function: FS_Get_NextFile_InDir()
Description:
Calls:
Global:si_fs_file_idx
Input: Null
Output:
Others:
********************************************************************************/
void FS_Get_NextFile_InDir(void)
{
loop_next_dir:
	while(ui_fs_file_num_indir > (si_fs_file_idx + 1))
	{
		si_fs_file_idx++;
		FS_Fopen(si_fs_file_idx);
		
		if(_chk_file_type() == 1)
		{
			return;
		}
	}

	si_fs_file_idx = -1;
	FS_Init_OpenVar();
	goto loop_next_dir;
}
#endif


#ifdef FS_GET_PREVIOUS_FILE_INDIR_DEF
/*******************************************************************************
Function: FS_GetPreviousFile_InDir()
Description:
Calls:
Global:si_fs_file_idx
Input: Null
Output:
Others:
********************************************************************************/
void FS_GetPreviousFile_InDir(void)
{
loop_last_dir:
	while(si_fs_file_idx > 0)
	{
		si_fs_file_idx--;
		FS_Fopen(si_fs_file_idx);
		
		if(_chk_file_type() == 1)
		{
			return;
		}
	}

	si_fs_file_idx = ui_fs_file_num_indir;
	goto loop_last_dir;
}
#endif



#ifdef FS_GET_NEXT_FILE_IN_DISK_DEF
/*******************************************************************************
Function: FS_GetNextFile_InDisk()
Description:
Calls:
Global:si_fs_file_idx
Input: Null
Output:
Others:
********************************************************************************/
void FS_GetNextFile_InDisk(void)
{
	ui16 tmp_dir_idx;
	bit tmp_file_dir;

	tmp_file_dir = 0;				//0:file  1:dir
	
search_nxt_file:
	if(tmp_file_dir == 0)
	{
		si_fs_file_idx++;
		if(si_fs_file_idx < ui_fs_file_num_indir)
		{
			FS_Fopen(si_fs_file_idx);
			if(_chk_file_type() == 1)
			{
				return;				//have got file and return
			}
			else
			{
				goto search_nxt_file;
			}
		}
		else						//searching file is over,begin to scan dir
		{
			tmp_file_dir = 1;		//search next dir
			tmp_dir_idx = -1;
			goto search_nxt_file;
		}
	}
	else
	{
		tmp_dir_idx++;
		if(tmp_dir_idx < ui_fs_subdir_num)
		{
			if(FS_EnterSubDir(tmp_dir_idx) == 1)
			{
				FS_GetItemNum_InDir();	//enter subdir and get item number

				tmp_file_dir = 0;
				si_fs_file_idx = -1;		//get into dir, and begin to scan file
			}
			else
			{
				goto search_nxt_dir;
			}
		}
		else					
		{
search_nxt_dir:								//searching file has been over,begin to scan dir
			tmp_dir_idx = FS_ExitSubDir();
			if(tmp_dir_idx == 0xffff)
			{								//now is in root,and cannot up,so from the first file in disk
				tmp_file_dir = 0;
				si_fs_file_idx = -1;
			}
			else
			{
				FS_GetItemNum_InDir();
				si_fs_file_idx = -1;		//get into dir, and begin to scan file
			}
		}
		
		goto search_nxt_file;
	}	   
}
#endif



#ifdef FS_GET_PREVIOUS_FILE_IN_DISK_DEF
/*******************************************************************************
Function: FS_GetPreviousFile_InDisk()
Description:
Calls:
Global:si_fs_file_idx
Input: Null
Output:
Others:
********************************************************************************/
void FS_GetPreviousFile_InDisk(void)
{
	ui16 tmp_dir_idx;
	bit tmp_file_dir;				//0:file  1:dir

	tmp_file_dir = 0;
	
search_previous_file:
	if(tmp_file_dir == 0)
	{
		if(si_fs_file_idx != 0)
		{
			si_fs_file_idx--;
			FS_Fopen(si_fs_file_idx);
			if(_chk_file_type() == 1)
			{
				return;
			}
			else
			{
				goto search_previous_file;
			}
		}
		else
		{
			tmp_dir_idx = FS_ExitSubDir();	
			if(tmp_dir_idx == 0xffff)
			{
				tmp_dir_idx = ui_fs_subdir_num;
			}
			else
			{
				FS_GetItemNum_InDir();
			}
			
			tmp_file_dir = 1;
			goto search_previous_file;
		}
	}
	else
	{
		if(tmp_dir_idx != 0)
		{
			tmp_dir_idx--;
			if(FS_EnterSubDir(tmp_dir_idx) == 1)
			{
				FS_GetItemNum_InDir();
				tmp_dir_idx = ui_fs_subdir_num;
			}
			else
			{
				goto search_lst_file;
			}
		}
		else
		{
search_lst_file:
			tmp_file_dir = 0;
			si_fs_file_idx = ui_fs_file_num_indir;
		}
		goto search_previous_file;	
	}
}
#endif



#ifdef FS_DELETE_DEF
/*******************************************************************************
Function: FS_Fdelete()
Description:
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void FS_Fdelete(void)
{
	_delete_file();
}
#endif



#ifdef FS_FCREATE_DEF
/*******************************************************************************
Function: FS_Fcreate()
Description:
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
bit FS_Fcreate(void)
{
	uc8 tmp_fat_srh_addr1, tmp_fat_srh_addr2;
		
	tmp_fat_srh_addr1 = b_sd_flash_flg?uc_fs_sd_fat_srh_addr1:uc_fs_fat_srh_addr1;
	tmp_fat_srh_addr2 = b_sd_flash_flg?uc_fs_sd_fat_srh_addr2:uc_fs_fat_srh_addr2;
	
	if (_seek_free_cluster(tmp_fat_srh_addr1,tmp_fat_srh_addr2) == 0)
	{
		return 0;
	}
	/*if (_seek_free_cluster(uc_fs_fat_srh_addr1,uc_fs_fat_srh_addr2) == 0)
	{
		return 0;
	}*/

	//save free cluster address,write_file_entry() may change them(extend fdt)
	uc_fs_cr_bgn_cluster3 = uc_fs_cur_cluster3;
	uc_fs_cr_bgn_cluster2 = uc_fs_cur_cluster2;
	uc_fs_cr_bgn_cluster1 = uc_fs_cur_cluster1;

	if((ui_fs_file_type & 0x0008) != 0)			//create wav file
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
	else										//create bin file
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
	
	if(write_file_entry() == 0)
	{
		return 0;
	}

	uc_fs_file_cur_clst2 = uc_fs_cr_bgn_cluster2;
	uc_fs_file_cur_clst2_bk = uc_fs_cr_bgn_cluster2;
	uc_fs_file_cur_clst1 = uc_fs_cr_bgn_cluster1;
	uc_fs_file_cur_clst1_bk = uc_fs_cr_bgn_cluster1;
	if(b_fs_fat_type_flg == 1)
	{
		uc_fs_file_cur_clst3 = uc_fs_cr_bgn_cluster3;
		uc_fs_file_cur_clst3_bk = uc_fs_cr_bgn_cluster3;
	}
	uc_fs_file_cur_sec_in_clst = 1;
	uc_fs_file_cur_sec_in_clst_bk = 1;
	ul_fs_byte_offset = 0;	

	b_fs_first_wr_file_flg = 1;

	return 1;
}
#endif


#ifdef FS_CHECK_FILE_TYPE
/*******************************************************************************
Function: FS_Chk_FileType()
Description:
Calls:
Global:Null
Input: Null
Output:bit 0--err
           1--file ok
Others:
********************************************************************************/
bit FS_Chk_FileType(void)
{
	return (_chk_file_type());
}
#endif


#ifdef FS_INIT_OPEN_VAR_DEF
/*******************************************************************************
Function: FS_Init_OpenVar()
Description:init variables when change directory
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void FS_Init_OpenVar(void)
{
	uc_fs_idx_hi = 0;				//modified by zhanghuan 2008-5-19
	uc_fs_idx_lo = 0;
	uc_fs_dptr_hi = (uc8)(FS_FCOMBUF >> 8);
	uc_fs_dptr_lo = (uc8)FS_FCOMBUF;
	uc_fs_entry_hi = uc_fs_fdt_begin_2;
	uc_fs_entry_lo = uc_fs_fdt_begin_1;
	uc_fs_entry_item_num = 0;
	uc_fs_cur_sec_in_cluster_bk = 1;
	b_fs_first_open = 1;
}
#endif

#ifdef FS_UPDATE_FAT_SRHADDR_DEF
extern void _seek_free_cluster_from_cbuf(uc8 fat_addr_lo, uc8 fat_addr_hi);

void FS_Update_Srhfat_Sectoraddr(void)
{
	uc8 tmp_fat_srh_addr1, tmp_fat_srh_addr2, tmp_exp;

	ul32 tmp_capacity, fs_spare_space;

	tmp_fat_srh_addr1 = b_sd_flash_flg?uc_fs_sd_fat_srh_addr1:uc_fs_fat_srh_addr1;
	tmp_fat_srh_addr2 = b_sd_flash_flg?uc_fs_sd_fat_srh_addr2:uc_fs_fat_srh_addr2;
	_seek_free_cluster_from_cbuf(tmp_fat_srh_addr1,tmp_fat_srh_addr2);
	
	if(b_sd_flash_flg)
	{
	 	fs_spare_space = ul_fs_sd_spare_space; 
	}
	else
	{
	 	fs_spare_space = ul_fs_spare_space; 
	}

	tmp_exp = FS_Sector_To_Cluster();

	tmp_capacity = (((ul_fs_byte_offset >> (9 + tmp_exp)) + 1) << tmp_exp);	 //20091124

	if(fs_spare_space <= tmp_capacity)
	{
		fs_spare_space = 0;
	}
	else
	{
		if(ul_fs_byte_offset!= 0)
		{
			fs_spare_space -= tmp_capacity;
		}
	}
	
	if(b_sd_flash_flg)
	{
	 	ul_fs_sd_spare_space = fs_spare_space; 
	}
	else
	{
	 	ul_fs_spare_space = fs_spare_space; 
	}
	 
}

#endif

#endif