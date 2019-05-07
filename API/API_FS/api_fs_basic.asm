/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: fs_basic_func.asm
Creator: andyliu					Date: 2008-03-20
Description: file system basic functions
Others:
Version: V2.04
History:
	V2.04	2010-06-12		AndyLiu
			1. delete function "_check_ef_addr_ok"
	V2.03	2010-03-05		aj.zhao
			1. modify "_write_sector_fatbuf", modify Fatbuf from Ibuf to Fbuf
	V2.02	2010-02-03		zong
			1. modify "_write_sector_fatbuf", modify Fatbuf from Fbuf to Ibuf
	V2.01	2009-12-02		andyliu
			1. modify "_fs_boot_sector_sub", flash max capacity = fs capacity - fat*2 - boot area
	V2.00	2009-11-26		andyliu
			1. modify "_write_file_entry": init the begin cluster is 0
			2. modify "_update_file_entry": update the begin cluster
			3. modify "_delete_file": if file length is 0, and not update fat table
	V1.9	2009-11-24		andyliu
			1. modify "_check_capacity", (cluster-2)*sectornum_per_cluster - max sector num
	V1.8	2009-11-15		andyliu
			1. modify "_fs_boot_sector_sub", flash max capacity = fs capacity - fat*2 - boot area
	V1.7	2009-10-30		zong
			1. modify "_fs_boot_sector_sub", flash max capacity = fs capacity - fat*2
			2. modify "_get_free_sector_space"
	V1.6	2009-07-24		andyliu
			1. support >8G flash
	V1.5	2009-05-13		andyliu
			1. add variable "uc_fs_fat_sector_max_num4"
	V1.4	2009-04-30		andyliu
			1. modify "_fs_read_sector()", add function _check_ef_addr_ok()
	V1.3	2008-11-17		andyliu
			1. modify "_get_free_sector_space()", if spare capacity is lt 0, and set spare capacity=0
	V1.2	2008-10-28		andyliu
			1. modify "_write_file_entry()", create wav file in fat16
	V1.1	2008-10-27		andyliu
			1. modify "_fs_boot_sector_sub()", modify to get capacity info
	V1.0	2008-10-23		andyliu
			1. support "*.avi" in subroutine _chk_file_type()
	V0.9	2008-10-08		andyliu
			1. support SD/MMC
	V0.8	2008-08-28		andyliu
			1. support create "*.bin" file
	V0.7	2008-07-21		andyliu
			1. add "_clr_512byte" after "_write_sector"
	V0.6	2008-07-04		zhanghuan
			1. modify "_get_last_sec_in_cluster",search pre cluster in currect
			2. modify "_get_next_cluster"
	V0.5	2008-06-11
			1. add b_sfc_immediate_wr_flg before "_write_sector"
			2. 
	V0.4	2008-06-05
			1. modify "_get_item_par", not restore uc_fs_entry_no, uc_fs_entry_sector3, uc_fs_entry_sector2, uc_fs_entry_sector1
	V0.3	2008-06-04
			1. modify "_wr_dir_dot", clr ebuf common buf before write flash
	V0.2	2008-05-22		zhanghuan
			1. modify "__get_item_idx_from_type_idx"
	V0.1	2008-03-20		andyliu
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"

#ifdef FILE_SYSTEM_FUNC_DEF
#include "sh86278_sfr_reg.inc"
#include ".\API\API_FS\api_fs_var_ext.inc"
#include ".\API\API_FS\api_fs.h"
#include "hwdriver_resident.h"


EXTRN  	CODE  	(_media_readsector)

#ifdef SP_SD_DRIVER
	#ifndef	TARGET_DebugGame
	EXTRN	XDATA	(uc_msi_sec_num_max_3)
	EXTRN	XDATA	(uc_msi_sec_num_max_2)
	EXTRN	XDATA	(uc_msi_sec_num_max_1)
	EXTRN	XDATA	(uc_msi_sec_num_max_0)
	EXTRN	BIT		(b_sd_flash_flg)
	#endif
	
	#ifndef FS_NOT_SD_DEF
		#ifdef FS_READ_SECTOR
			EXTRN  	CODE  	(_read_msi_sector)
		#else
			#ifdef FS_READ_SECTOR_TO_POINT
				EXTRN  	CODE  	(_read_msi_sector)
			#endif
		#endif
	#endif
	
	#ifdef FS_WRITE_INTERFACE
		EXTRN	code	(_write_flash)
		#ifndef FS_NOT_SD_DEF
			EXTRN	code	(_write_msi_sector)
		#endif
	#else
		#ifdef FS_WRITE_EF_INTERFACE
			EXTRN	code	(_write_flash)
		#endif
	#endif
#else
	#ifdef FS_WRITE_INTERFACE
		EXTRN	code	(_write_flash)
	#else
		#ifdef FS_WRITE_EF_INTERFACE 
			EXTRN	code	(_write_flash)
#endif
	#endif
#endif


FS_BASIC_FUNC	SEGMENT		CODE
				RSEG		FS_BASIC_FUNC



#ifdef FS_FREAD_PREVIOUS_SECTOR_DEF
PUBLIC	_get_dir_pre_sector_addr
PUBLIC	_fs_read_sector_2_point
PUBLIC	_get_cluster_fir_sector
PUBLIC	_cal_sector_address_1
PUBLIC	_get_last_sec_in_cluster
#endif


#ifdef	FS_POWER_ON_BOOT
PUBLIC  _fs_boot_sector_sub
/*******************************************************************************
********************************************************************************/
_fs_boot_sector_sub:
	MOV		sfc_logblkaddr0, #00H
	MOV		sfc_logblkaddr1, #00H
	MOV		sfc_logblkaddr2, #00H
	MOV  	sfc_logblkaddr3, #00H
	CALL	_fs_read_sector

	MOV		DPTR, #FS_FCOMBUF + 1FEH
	MOVX	A, @DPTR					;check 55aa

	CJNE	A, #0AAH, boot_err_ret
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #055H, boot_err_ret

	MOV		DPTR, #FS_FCOMBUF + 1BFH
	MOVX	A, @DPTR			;check active partition, 80:active, 00:not active, other:illegal
	CJNE	A, #80H, not_active_partition
	JMP		search_real_boot_sector

not_active_partition:
	CJNE	A, #00H, analyze_BPB_info		

search_real_boot_sector:
	MOV		R7, #4
/*** judge partition no #1 ***/	
	MOV		DPTR, #PartitionEntryNo
	MOVX	A, @DPTR
	CJNE	A, #01H, search_partition_2
	MOV		A, #2
	MOVX	@DPTR, A
	MOV		DPTR, #FS_FCOMBUF + 1C6H	//partition entry #1
	JMP		judge_partition_legal
/*** judge partition no #2 ***/	
search_partition_2:	
	CJNE	A, #02H, search_partition_3
	MOV		A, #3
	MOVX	@DPTR, A
	MOV		DPTR, #FS_FCOMBUF + 1D6H	//partition entry #2
	JMP		judge_partition_legal
/*** judge partition no #3 ***/	
search_partition_3:	
	CJNE	A, #03H, search_partition_4
	MOV		A, #4
	MOVX	@DPTR, A
	MOV		DPTR, #FS_FCOMBUF + 1E6H	//partition entry #3
	JMP		judge_partition_legal
/*** judge partition no #4 ***/	
search_partition_4:	
	MOV		A, #0
	MOVX	@DPTR, A
	MOV		DPTR, #FS_FCOMBUF + 1F6H	//partition entry #4		
judge_partition_legal:		
	MOVX  	A, @DPTR
	MOV		R1, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV		R0, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV   	R3, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV   	R2, A
	CJNE	R0, #00H, get_partition_addr
	CJNE	R1, #00H, get_partition_addr	
	CJNE	R2, #00H, get_partition_addr
	CJNE	R3, #00H, get_partition_addr
	MOV		A, DPL
	CJNE	A, #0F9H, search_next_partition_table
	MOV		DPTR, #FS_FCOMBUF + 1C6H
	JMP		loop_search_partition	
search_next_partition_table:	
	MOV		A, DPL
	ADD		A, #13
	MOV		DPL, A
loop_search_partition:	
	DJNZ	R7, judge_partition_legal
	JMP		get_partition_addr
	
boot_err_ret: 
	CLR		C
	RET	
	
get_partition_addr:	
	MOV   	sfc_logblkaddr3, R3
	MOV   	sfc_logblkaddr2, R2
	MOV   	sfc_logblkaddr1, R1
	MOV   	sfc_logblkaddr0, R0	
	CALL	_fs_read_sector

/*** now is partition DBR ***/
	MOV		DPTR, #FS_FCOMBUF + 1FEH
	MOVX	A, @DPTR					;check 55aa
	CJNE	A, #0AAH, boot_err_ret
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #55H, boot_err_ret

analyze_BPB_info:
;check "FAT16"
	MOV		DPTR, #FS_FCOMBUF + 36H
	MOVX	A, @DPTR
	CJNE	A, #41H, boot_check_FAT32	;"A"
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #46H, boot_err_ret		;"F"
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #31H, boot_err_ret		;"1"			
	MOV		DPTR, #FS_FCOMBUF + 3BH
	MOVX	A, @DPTR
	CJNE	A, #36H, boot_err_ret		;"6"		
	CLR		b_fs_fat_type_flg
	JMP		get_BPB_info

boot_check_FAT32:
;check "FAT32"
	MOV		DPTR, #FS_FCOMBUF + 52H
	MOVX	A, @DPTR
	CJNE	A, #41H, boot_err_ret		;"A"
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #46H, boot_err_ret		;"F"
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #33H, boot_err_ret		;"3"			
	MOV		DPTR, #FS_FCOMBUF + 57H
	MOVX	A, @DPTR
	CJNE	A, #32H, boot_err_ret		;"2"
	SETB	b_fs_fat_type_flg

get_BPB_info:
	;sector number per cluster
	MOV		DPTR, #FS_FCOMBUF + 0CH		
	MOVX	A, @DPTR
	MOV		DPTR, #uc_fs_sec_per_cluster
	MOVX	@DPTR, A

	;reserved sector, use R2(LO), R3(HI)
	MOV		DPTR, #FS_FCOMBUF + 0EH
	MOVX	A, @DPTR
	MOV		R3, A

	INC		DPTR
	MOVX	A, @DPTR
	MOV		R2, A

	MOV		A, sfc_logblkaddr0		;get fat begin address
	MOV		DPTR, #uc_hidden_sector1
	MOVX	@DPTR, A
	ADD		A, R2
	MOV		DPTR, #uc_fs_fat1_begin_1
	MOVX	@DPTR, A
	MOV		A, sfc_logblkaddr1
	MOV		DPTR, #uc_hidden_sector2
	MOVX	@DPTR, A
	ADDC	A, R3
	MOV		DPTR, #uc_fs_fat1_begin_2
	MOVX	@DPTR, A
	MOV		DPTR, #FS_FCOMBUF + 11H
	MOVX	A, @DPTR
	MOV		DPTR, #fat_num
	MOVX	@DPTR, A

	//JB		b_fs_fat_type_flg, boot_fat32_proc
	JNB		b_fs_fat_type_flg, boot_fat16_proc
	JMP		boot_fat32_proc

boot_fat16_proc:
;sector number per fat, use R5, R4
	MOV		DPTR, #FS_FCOMBUF + 16H
	MOVX	A, @DPTR
	MOV		R5, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV		R4, A
	MOV		R6, #00H
	MOV		R7, #00H

	MOV		DPTR, #uc_fs_fat1_begin_1
	MOVX	A, @DPTR
	MOV		R0, A
	MOV		DPTR, #uc_fs_fat1_begin_2
	MOVX	A, @DPTR
	MOV		R1, A
	MOV		R2, #000H	
	MOV		R3, #000H	
	CLR		b_fs_got_fat2_addr_flg
		
loop_search_fdt16:
	CALL	_fs_add_32bit
	JB		b_fs_got_fat2_addr_flg, goon_srh_fdt16
	CALL	get_fat2_addr
goon_srh_fdt16:	
	MOV		DPTR, #fat_num
	MOVX	A, @DPTR
	DEC		A
	MOVX	@DPTR, A
	JNZ		loop_search_fdt16

	MOV		DPTR, #uc_fs_fdt_begin_1
	MOV		A, R0
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_fdt_begin_2
	MOV		A, R1
	MOVX	@DPTR, A

;root entry, use R4, R5
	MOV		DPTR, #FS_FCOMBUF + 10H
	MOVX	A, @DPTR
	MOV		R4, A
	MOV		DPTR, #FS_FCOMBUF + 13H
	MOVX	A, @DPTR
	MOV		R5, A	

	MOV		A, R4
	MOV		R0, A	
	MOV		A, R5
	MOV		R1, A
	MOV		R2, #4
	CALL	_r_shift
	MOV		DPTR, #uc_fs_fdt_sec_num
	MOV		A, R0
	MOVX	@DPTR, A

	MOV		DPTR, #uc_fs_fdt_begin_1
	MOVX	A, @DPTR
	ADD		A, R0
	MOV		DPTR, #uc_fs_dat_begin_1
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_fdt_begin_2
	MOVX	A, @DPTR
	ADDC	A, R1
	MOV		DPTR, #uc_fs_dat_begin_2
	MOVX	@DPTR, A
			
	JMP		boot_get_flash_capacity	

boot_fat32_proc:
	MOV		DPTR, #FS_FCOMBUF + 24H
	MOVX	A, @DPTR
	MOV		R5, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV		R4, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV		R7, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV		R6, A

	MOV		DPTR, #uc_fs_fat1_begin_1
	MOVX	A, @DPTR
	MOV		R0, A
	MOV		DPTR, #uc_fs_fat1_begin_2
	MOVX	A, @DPTR
	MOV		R1, A
	MOV		R2, #0
	MOV		R3, #0
	CLR		b_fs_got_fat2_addr_flg

loop_search_fdt32:
	CALL	_fs_add_32bit
	JB		b_fs_got_fat2_addr_flg, goon_srh_fdt32
	CALL	get_fat2_addr
goon_srh_fdt32:	
	MOV		DPTR, #fat_num
	MOVX	A, @DPTR
	DEC		A
	MOVX	@DPTR, A
	JNZ		loop_search_fdt32

	MOV		DPTR, #uc_fs_dat_begin_1
	MOV		A, R0
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_dat_begin_2
	MOV		A, R1
	MOVX	@DPTR, A

boot_get_flash_capacity:
	CALL	fs_get_flash_capacity		;andyliu 20091115
	SETB	C		;set success flag
	RET

get_fat2_addr:
	MOV		DPTR, #uc_fs_fat2_begin_1
	MOV		A, R0
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_fat2_begin_2
	MOV		A, R1
	MOVX	@DPTR, A
	SETB	b_fs_got_fat2_addr_flg
	RET			

fs_get_flash_capacity:
	JB		b_fs_fat_type_flg, _get_Total32_Large16_Sector		//aj 20091130
/*** READ SMALL SECTOR NUM ***/	 
	MOV		DPTR, #FS_FCOMBUF + 15H
	MOVX	A, @DPTR
	MOV		R1, A					//aj 20091130

	MOV		DPTR, #FS_FCOMBUF + 12H
	MOVX	A, @DPTR
	MOV		R0, A					//aj 20091130

	MOV		R2, #00H
	MOV		R3, #00H
		
/*** FAT16 CHECK LESS THAN 32M ***/
	JNZ		_sub_FAT_table	//aj 20091130
	MOV		A, R1
	JNZ		_sub_FAT_table

/*** FAT32 TOTAL SECTOR AND FAT16 LARGE SECTOR ***/		
_get_Total32_Large16_Sector:		
	MOV		DPTR, #FS_FCOMBUF + 20H
	MOVX	A, @DPTR
	MOV		R1,	A			//sector low word high bit (FBUF)

	INC		DPTR
	MOVX	A, @DPTR
	MOV		R0,	A			//sector low word low bit

	INC		DPTR
	MOVX	A, @DPTR
	MOV		R3,	A			//sector high word high bit

	INC		DPTR
	MOVX	A, @DPTR
	MOV		R2,	A			//sector high word low bit

/*** include FAT1,FAT2,boot sector,32sector(only FAT16) ***/
_sub_FAT_table:				//aj 20091130
	MOV		DPTR, #uc_fs_dat_begin_1 	//aj 20100720
	MOVX	A, @DPTR
	MOV		R4, A
	MOV		DPTR, #uc_fs_dat_begin_2	//aj 20100720
	MOVX	A, @DPTR
	MOV		R5, A
	MOV		R6, #00H
	MOV		R7, #00H		  
	CALL	_sub_32bit
	
	MOV		DPTR, #uc_hidden_sector1 	
	MOVX	A, @DPTR
	MOV		R4, A
	MOV		DPTR, #uc_hidden_sector2	
	MOVX	A, @DPTR
	MOV		R5, A
	MOV		R6, #00H
	MOV		R7, #00H

	CALL	_fs_add_32bit

	CALL	_sec_to_clu_exponent
	MOV		A, R7
	MOV		R4, A
	CALL	_r_shift_32bit
	MOV		A, R7
	MOV		R4, A
	CALL	_l_shift_32bit
		
	JB		b_sd_flash_flg, obtain_sd_total_capacity
	MOV		A, R3
	MOV		DPTR, #uc_fs_fat_sector_max_num4
	MOVX	@DPTR, A

	MOV		A, R2
	MOV		DPTR, #uc_fs_fat_sector_max_num3
	MOVX	@DPTR, A
 
	MOV		A, R1
	MOV		DPTR, #uc_fs_fat_sector_max_num2
	MOVX	@DPTR, A

 	MOV		A, R0
	MOV		DPTR, #uc_fs_fat_sector_max_num1
	MOVX	@DPTR, A
	RET

obtain_sd_total_capacity:	
	MOV		A, R3
	MOV		DPTR, #uc_fs_sd_fat_sector_max_num4
	MOVX	@DPTR, A

	MOV		A, R2
	MOV		DPTR, #uc_fs_sd_fat_sector_max_num3
	MOVX	@DPTR, A
 
	MOV		A, R1
	MOV		DPTR, #uc_fs_sd_fat_sector_max_num2
	MOVX	@DPTR, A

 	MOV		A, R0
	MOV		DPTR, #uc_fs_sd_fat_sector_max_num1
	MOVX	@DPTR, A
	RET

#endif


#ifdef GET_ITEM_NUM
PUBLIC  _get_item_number
/*******************************************************************************
;fuction1.2
;fat16, fat32
uc_fs_tmp_var2: record item number in one sector(0-15)
uc_fs_tmp_var1: FDT sector number counter(0-31)
uc_fs_tmp_var6, uc_fs_tmp_var7:MP3 total number counter
INPUT:uc_fs_begin_cluster1, uc_fs_begin_cluster2, (uc_fs_begin_cluster3)
No return;
Output global var:ui_fs_item_num, ui_fs_item_num+1, ui_fs_subdir_num, ui_fs_subdir_num+1
********************************************************************************/
_get_item_number:
	CLR		A
	MOV		R0, #ui_fs_item_num
	MOV		@R0, A
	INC		R0
	MOV		@R0, A
	MOV		DPTR, #ui_fs_special_file_num_indir
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	MOV		ui_fs_subdir_num, #0
	MOV		ui_fs_subdir_num + 1, #0

get_item_num_beg:
	JB		b_fs_dir_flg, in_subdir
	
	;now in ROOT
	MOV		uc_fs_tmp_var2, #0
	MOV		DPTR, #uc_fs_fdt_sec_num
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var1, A
 
	MOV		DPTR, #uc_fs_fdt_begin_1
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A
	MOV		DPTR, #uc_fs_fdt_begin_2
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	MOV		sfc_logblkaddr2, #00H
	MOV  	sfc_logblkaddr3, #00H

read:
	CALL	_fs_read_sector

ste_pointer:
	MOV		DPTR, #FS_FCOMBUF
	JMP		check_item

in_subdir:
	CALL	_get_first_sector				;default input par:uc_fs_begin_cluster1, uc_fs_begin_cluster2

in_subdir32:
	JB		b_fs_fat32_root_flg, get_item_num_fat32
	MOV		uc_fs_tmp_var2, #2
	MOV		DPTR, #(FS_FCOMBUF + 40H)
	JMP		check_item

get_item_num_fat32:
	MOV		uc_fs_tmp_var2, #0				;in fat32 root, no dot and dot dot entry
	MOV		DPTR, #FS_FCOMBUF
	
check_item:
	CALL	_check_item_type
	MOV		A, R7
	CJNE	A, #0FFH, check1
	JMP		check_over
check1:
	CJNE	A, #00H, check2
	JMP		go_on
check2:
	CJNE	A, #02H, special_file_chk
	MOV		A, ui_fs_subdir_num + 1
	ADD		A, #1
	MOV		ui_fs_subdir_num + 1, A
	MOV		A, ui_fs_subdir_num
	ADDC	A, #0
	MOV		ui_fs_subdir_num, A
	JMP		add_item
	
special_file_chk:
	MOV		R2, DPL
	MOV		R3, DPH
	CALL	_get_file_num_chk
	JNC		restor_dptr_for_goon

	MOV		DPTR, #ui_fs_special_file_num_indir + 1
	MOVX	A, @DPTR
	ADD		A, #1
	MOVX	@DPTR, A
	MOV		DPTR, #ui_fs_special_file_num_indir
	MOVX	A, @DPTR
	ADDC	A, #0
	MOVX	@DPTR, A
	
restor_dptr_for_goon:
	MOV		DPL, R2
	MOV		DPH, R3

add_item:
	MOV		R0, #ui_fs_item_num + 1
	MOV		A, @R0
	ADD		A, #1
	MOV		@R0, A
	DEC		R0
	MOV		A, @R0
	ADDC	A, #0
	MOV		@R0, A

go_on:
	INC		uc_fs_tmp_var2
	MOV		A, uc_fs_tmp_var2
	CJNE	A, #16, check_item
	MOV		uc_fs_tmp_var2, #0

	JNB		b_fs_dir_flg, root_inc

	CLR		b_fs_next_sector_flg
	CALL	_get_next_sector

go_on22:
	JB		b_fs_file_end_flg, check_over
	JMP		ste_pointer

jmp_read:
	JMP		read

root_inc:
	CALL	_inc_sector_addr
	DEC		uc_fs_tmp_var1
	MOV		A, uc_fs_tmp_var1
	JNZ		jmp_read

check_over:
	RET
#endif



#ifdef	FS_FSEEK_DEF
PUBLIC  _convert_byte_info
PUBLIC  __fs_fseek_sub
/*******************************************************************************
bit fseek_sub(unsigned long offset)
	MOV		R6, uc_fs_tmp_var3
	MOV		R7, uc_fs_tmp_var2
********************************************************************************/
__fs_fseek_sub:
	CALL	_convert_byte_info	;OUTPUT uc_fs_tmp_var2, uc_fs_tmp_var3

	MOV		uc_fs_tmp_var7, #00H		;HIGH
	MOV		uc_fs_tmp_var6, #00H		;LOW
	MOV		uc_fs_tmp_var5, #00H		;used for save the FAT sector addr where cluste located
	MOV		uc_fs_tmp_var4, #00H		;

#ifdef UI_MOVIE_MAIN_DEF
	CLR		A							;zhanghuan modify 2008-07-03
	MOV		DPTR, #uc_fs_sector1_bk
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_sector0_bk
	MOVX	@DPTR, A
#endif

	MOV		uc_fs_cur_cluster1, uc_fs_begin_cluster1
	MOV		uc_fs_cur_cluster2, uc_fs_begin_cluster2

	JNB		b_fs_fat_type_flg, cluster_seek_beg
	MOV		uc_fs_cur_cluster3, uc_fs_begin_cluster3

cluster_seek_beg:
	MOV		A, uc_fs_tmp_var2
	JNZ		cluster_seek_beg_2
	MOV		A, uc_fs_tmp_var3
	JNZ		cluster_seek_beg_2
	JMP		fseek_sub_ret
cluster_seek_beg_2:
#ifdef UI_MOVIE_MAIN_DEF
	CLR		b_fs_pre_flg
#endif
	CALL	_get_next_cluster
	JB		b_fs_file_end_flg, fseek_sub_err_end
	INC		uc_fs_tmp_var6
	MOV		A, uc_fs_tmp_var6
	JNZ		compare_with_tmp_var23	
	INC		uc_fs_tmp_var7

compare_with_tmp_var23:
	MOV		A, uc_fs_tmp_var6
	CJNE	A, uc_fs_tmp_var2, cluster_seek_beg_2
	MOV		A, uc_fs_tmp_var7
	CJNE	A, uc_fs_tmp_var3, cluster_seek_beg_2

fseek_sub_ret:
	SETB	C
	RET

fseek_sub_err_end:
	CLR		C
	RET


/**********************************************************************
unsigneed int convert_byte_info(unsigneed long byte)
						 R4, R5, R6, R7
Return:according to inputed bytes information , get it occupy how many cluster
	uc_fs_cur_sec_in_cluster(sector offset in the final cluster)
**********************************************************************/
_convert_byte_info:
	MOV		uc_fs_tmp_var7, R7
	MOV		uc_fs_tmp_var6, R6
	MOV		uc_fs_tmp_var5, R5
	MOV		uc_fs_tmp_var4, R4
	CALL	_sec_to_clu_exponent	;only use R7

	MOV		A, R7
	JZ		convert_not_rlc
	MOV		R0, A
	CLR		A
info1:
	SETB	C
	RLC		A
	DJNZ	R0, info1

convert_not_rlc:
	MOV		uc_fs_tmp_var1, A

	MOV		R3, uc_fs_tmp_var4
	MOV		R2, uc_fs_tmp_var5
	MOV		R1, uc_fs_tmp_var6
	MOV		R0, uc_fs_tmp_var7
	;shifter bunmber
	MOV		A, R7
	ADD		A, #9
	MOV		R4, A
	CALL	_r_shift_32bit
	
	MOV		uc_fs_tmp_var3, R1		;
	MOV		uc_fs_tmp_var2, R0		;uc_fs_tmp_var2, uc_fs_tmp_var3 CLUSTER NUMBER

	MOV		R3, uc_fs_tmp_var4
	MOV		R2, uc_fs_tmp_var5
	MOV		R1, uc_fs_tmp_var6
	MOV		R0, uc_fs_tmp_var7
	;shifter bunmber
	MOV		R4, #9
	CALL	_r_shift_32bit

	MOV		A, R0
	ANL		A, uc_fs_tmp_var1
	INC		A
	MOV		R0, #uc_fs_cur_sec_in_cluster
	MOV		@R0, A

	MOV		R6, uc_fs_tmp_var3
	MOV		R7, uc_fs_tmp_var2
	RET
#endif



#ifdef	FS_FREAD_DEF
PUBLIC	_read_sect_asm
/*******************************************************************************
input:uc_fs_file_cur_sec_in_clst, file_cur_clst_hi, file_cur_clst_lo
********************************************************************************/
_read_sect_asm:
	MOV		DPTR, #uc_fs_file_cur_sec_in_clst
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var1, A
	MOV		DPTR, #uc_fs_file_cur_clst2
	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster2, A
	MOV		DPTR, #uc_fs_file_cur_clst1
	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster1, A

	JNB		b_fs_fat_type_flg, cal_sector_actual_addr
	MOV		DPTR, #uc_fs_file_cur_clst3
	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster3, A

cal_sector_actual_addr:
	CALL	_get_cluster_fir_sector
	MOV		A, uc_fs_tmp_var1

	CALL	_cal_sector_address

	CALL	_fs_read_sector_2_point
	RET
#endif





#ifdef FS_FREAD_PREVIOUS_SECTOR_DEF
/*******************************************************************************
get dir previous sector, maybe cross cluster
input :b_fs_dir_flg, sfc_logblkaddr0, sfc_logblkaddr1, sfc_logblkaddr2, sfc_logblkaddr3
output:sfc_logblkaddr0, sfc_logblkaddr1, sfc_logblkaddr2, sfc_logblkaddr3
change globalvar:uc_fs_tmp_var1, uc_fs_tmp_var2, uc_fs_tmp_var3, uc_fs_tmp_var4, uc_fs_tmp_var5
		 uc_fs_cur_cluster1, uc_fs_cur_cluster2, uc_fs_cur_cluster3
********************************************************************************/
_get_dir_pre_sector_addr:
	JB		 b_fs_dir_flg, norm_proc_pre_sector
	;in FAT16 root
	MOV		A, sfc_logblkaddr3
	JNZ		not_reach_fdt_begin
	MOV		A, sfc_logblkaddr2
	JNZ		not_reach_fdt_begin 
	
	MOV		DPTR, #uc_fs_fdt_begin_1
	MOVX	A, @DPTR
	CJNE	A, sfc_logblkaddr0, not_reach_fdt_begin

	MOV		DPTR, #uc_fs_fdt_begin_2
	MOVX	A, @DPTR
	CJNE	A, sfc_logblkaddr1, not_reach_fdt_begin

	;now in fat16 root, has been arrive at the beginning of FDT
	;cannot get previous sector
	CLR		C
	RET

not_reach_fdt_begin:
get_pre_sector_ret1:
	CALL	_dec_sector_addr
	SETB	C
	RET


_get_last_sec_in_cluster:
#ifdef UI_MOVIE_MAIN_DEF
	CLR		b_fs_temp_flg
#endif
	CALL	parse_sector_info		;output is R0~R4
	MOV		A, R4
	JZ		get_last_cluster
	MOV		R0, #uc_fs_cur_sec_in_cluster
	MOV		@R0, A
	JMP		get_pre_sector_ret1
	
get_last_cluster:
	MOV		uc_fs_tmp_var1, R0		;get last cluster
	MOV		uc_fs_tmp_var2, R1
	MOV		uc_fs_tmp_var3, R2

#ifdef UI_MOVIE_MAIN_DEF
	MOV		uc_fs_cur_cluster1, R0
	MOV		uc_fs_cur_cluster2, R1
	MOV		uc_fs_cur_cluster3, R2
	
	JB		b_fs_pre_flg, goon_seek_match_cluster

	CLR		A							;zhanghuan modify 2008-07-03
	MOV		DPTR, #uc_fs_sector1_bk
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_sector0_bk
	MOVX	@DPTR, A
	SETB	b_fs_pre_flg

	JMP		goon_seek_match_cluster
#endif
init_uc_fs_cur_cluster:
	MOV		uc_fs_cur_cluster1, uc_fs_begin_cluster1
	MOV		uc_fs_cur_cluster2, uc_fs_begin_cluster2
	MOV		uc_fs_cur_cluster3, uc_fs_begin_cluster3
#ifdef UI_MOVIE_MAIN_DEF
	SETB	b_fs_temp_flg
#endif
	JMP		goon_seek_match_cluster_beg
	
norm_proc_pre_sector:
	CALL	parse_sector_info
	MOV		A, R4
	JZ		span_cluster_process
	JMP		get_pre_sector_ret1

span_cluster_process:
	;now R0, R1, R3 current cluster
	MOV		uc_fs_tmp_var1, R0
	MOV		uc_fs_tmp_var2, R1
	MOV		uc_fs_tmp_var3, R2

	MOV		A, uc_fs_subdir_stk_pt
	JNZ		span_pro_in_dir 
	MOV		uc_fs_cur_cluster1, #02H
	MOV		uc_fs_cur_cluster2, #00H
	MOV		uc_fs_cur_cluster3, #00H
	JMP		goon_seek_match_cluster_beg

span_pro_in_dir:
	DEC		A
	MOV		DPTR, #uc_fs_subdir_stk_buf		;get uc_fs_subdir_stk_buf begin address
	MOV		B, #FS_BYTE_PER_LAYER
	MUL		AB
	ADD		A, DPL
	MOV		DPL, A
	MOV		A, DPH
	ADDC	A, #0
	MOV		DPH, A

	INC		DPTR
	INC		DPTR

	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster1, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster2, A	
	INC		DPTR
	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster3, A		;get dir begin cluster

goon_seek_match_cluster_beg:
	MOV		uc_fs_tmp_var5, #00H		;used for save the FAT sector addr where cluste located
	MOV		uc_fs_tmp_var4, #00H		;
#ifdef UI_MOVIE_MAIN_DEF
	CLR		A							;zhanghuan modify 2008-07-03
	MOV		DPTR, #uc_fs_sector1_bk
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_sector0_bk
	MOVX	@DPTR, A
#endif	
goon_seek_match_cluster:
	MOV		R0, #uc_fs_layer1_dir_bc1	;save previous cluster, for find link
	MOV		A, uc_fs_cur_cluster1
	MOV		@R0, A
	MOV		R0, #uc_fs_layer1_dir_bc2
	MOV		A, uc_fs_cur_cluster2
	MOV		@R0, A
	MOV		R0, #uc_fs_layer1_dir_bc3
	MOV		A, uc_fs_cur_cluster3
	MOV		@R0, A

	CALL	_get_next_cluster

	JNB		b_fs_file_end_flg, chk_link_if_eq
	CLR		C
	RET

chk_link_if_eq:
#ifdef UI_MOVIE_MAIN_DEF
	JB		b_fs_pre_flg, got_prev_cluster_11
	JB		b_fs_temp_flg, cmp_cluster
	JMP		init_uc_fs_cur_cluster
cmp_cluster:
#endif
	MOV		A, uc_fs_tmp_var1
	CJNE	A, uc_fs_cur_cluster1, goon_seek_match_cluster
	MOV		A, uc_fs_tmp_var2
	CJNE	A, uc_fs_cur_cluster2, goon_seek_match_cluster

	JB		b_fs_fat_type_flg, fat32_chk_link_if_eq
	JMP		got_prev_cluster

fat32_chk_link_if_eq:
	MOV		A, uc_fs_tmp_var3
	CJNE	A, uc_fs_cur_cluster3, goon_seek_match_cluster	

got_prev_cluster:
	MOV		R0, #uc_fs_layer1_dir_bc1	;	GET previous cluster, find link
	MOV		A, @R0
	MOV		uc_fs_cur_cluster1, A
	MOV		R0, #uc_fs_layer1_dir_bc2
	MOV		A, @R0
	MOV		uc_fs_cur_cluster2, A
	MOV		R0, #uc_fs_layer1_dir_bc3
	MOV		A, @R0
	MOV		uc_fs_cur_cluster3, A
got_prev_cluster_11:
	CALL	_get_cluster_fir_sector
	
	MOV		DPTR, #uc_fs_sec_per_cluster
	MOVX	A, @DPTR
	MOV		R0, #uc_fs_cur_sec_in_cluster
	MOV		@R0, A
	CALL	_cal_sector_address

	SETB	C
	RET
#endif
	
#ifdef SECTRO_TO_CLUSTER	
/*************************************************************************
get information from sector address, including cluster, sector offset
INPUT: sfc_logblkaddr0, sfc_logblkaddr1, sfc_logblkaddr2, sfc_logblkaddr3
OUTPUT:R0-R3(cluster No), R4(sector offset, begin from 0)
*************************************************************************/
parse_sector_info:
	MOV		R0, sfc_logblkaddr0
	MOV		R1, sfc_logblkaddr1
	MOV		R2, sfc_logblkaddr2

sector_addr_cal_clst:
	MOV		R3, sfc_logblkaddr3

	MOV		DPTR, #uc_fs_dat_begin_1
	MOVX	A, @DPTR
	MOV		R4, A
	MOV		DPTR, #uc_fs_dat_begin_2
	MOVX	A, @DPTR
	MOV		R5, A
	MOV		R6, #00H
	MOV		R7, #00H

	CALL	_sub_32bit 
	MOV		uc_fs_tmp_var1, R0
	
	CALL	_sec_to_clu_exponent	;only use R7
	MOV		A, R7
	MOV		R4, A
	CALL	_r_shift_32bit

	MOV		A, R0
	ADD		A, #2
	MOV		R0, A	;;;;?
	MOV		A, R1
	ADDC	A, #0
	MOV		R1, A	;;;;?
	MOV		A, R2
	ADDC	A, #0
	MOV		R2, A	;have got cluster No.

	MOV		A, R7
	JZ		parse_info_1
	MOV		R6, A
	CLR		A
parse_setb1:
	SETB	C
	RLC		A
	DJNZ	R6, parse_setb1

parse_info_1:
	ANL		A, uc_fs_tmp_var1
	MOV		R4, A
	RET
#endif	


#ifdef	FS_FWRITE_DEF
PUBLIC  _first_dec_2_fat
PUBLIC	_fetch_new_sector
PUBLIC	_fwrite_one_sector
/*******************************************************************************
output: 0--fail
        1--success
********************************************************************************/
_fetch_new_sector:
	MOV		DPTR, #uc_fs_sec_per_cluster
	MOVX	A, @DPTR
	MOV		B, A
	MOV		DPTR, #uc_fs_file_cur_sec_in_clst
	MOVX	A, @DPTR
	CJNE	A, B, not_span_cluster	
	JMP		fetch_new_cluster

not_span_cluster:
	MOV		DPTR, #uc_fs_file_cur_sec_in_clst
	MOVX	A, @DPTR
	INC		A
	MOVX	@DPTR, A
	SETB	C
	RET

fetch_new_cluster:
	MOV		DPTR, #uc_fs_file_cur_clst1
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var2, A
	MOV		DPTR, #uc_fs_file_cur_clst2
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var3, A
	MOV		DPTR, #uc_fs_file_cur_clst3
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var7, A

	CALL	_fetch_new_cluster_fw
	
	JC		fetch_new_cluster_ok
	;card full, return 0
	RET

fetch_new_cluster_ok:
	MOV		DPTR, #uc_fs_file_cur_sec_in_clst
	MOV		A, #01H
	MOVX	@DPTR, A

	MOV		DPTR, #uc_fs_file_cur_clst1
	MOV		A, uc_fs_cur_cluster1
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_file_cur_clst2
	MOV		A, uc_fs_cur_cluster2
	MOVX	@DPTR, A
	JNB		b_fs_fat_type_flg, fetch_new_cluster_ret
	MOV		DPTR, #uc_fs_file_cur_clst3
	MOV		A, uc_fs_cur_cluster3
	MOVX	@DPTR, A
	
fetch_new_cluster_ret:
	SETB	C
	RET


/************************************************************************
;get new blank cluster 1.from fatbuf 2.fatbuf is full, get from card, then
;update fatbuf
;INPUT:uc_fs_tmp_var2, uc_fs_tmp_var3, uc_fs_tmp_var7(old cluster)
;OUTPUT:uc_fs_cur_cluster1, uc_fs_cur_cluster2, uc_fs_cur_cluster3
************************************************************************/
_fetch_new_cluster_fw:
	MOV		DPTR, #FS_FATBUF

	CALL	_srh_blank_clu_one_sector

	JC		get_blank_in_fatbuf
	;need use decbuf, copy data to dbuf1
	MOV		R3, #HIGH FS_BATBUF_BK2
	MOV		R1, #HIGH FS_BATBUF_BK1
	CALL	_src_r1_to_des_r3

	JB		b_sd_flash_flg, get_sd_old1
	MOV		DPTR, #uc_fs_fat_srh_addr1
	JMP		save_sd_old1
get_sd_old1:
	MOV		DPTR, #uc_fs_sd_fat_srh_addr1
save_sd_old1:	
	MOVX	A, @DPTR
	MOV		DPTR, #uc_fs_old_fat_addr1
	MOVX	@DPTR, A
	ADD		A, #01H
	MOV		R7, A
	
	JB		b_sd_flash_flg, get_sd_old2
	MOV		DPTR, #uc_fs_fat_srh_addr2
	JMP		save_sd_old2
get_sd_old2:
	MOV		DPTR, #uc_fs_sd_fat_srh_addr2
save_sd_old2:	
	MOVX	A, @DPTR
	MOV		DPTR, #uc_fs_old_fat_addr2
	MOVX	@DPTR, A
	ADDC	A, #00H
	MOV		R5, A

	CALL	__seek_free_cluster			;uc_fs_tmp_var2, uc_fs_tmp_var3, uc_fs_tmp_var7 cannot change
	JC		srf_incard_new_clst_ok
	;card full, return 0
	RET

srf_incard_new_clst_ok:
	JB		b_fs_fat_type_flg, proc_fat_decbuf_32
	MOV		R0, #HIGH FS_FATBUF
	CALL	_fat16_fat_wr_link
	CALL	_write_sector_from_fatbuf
	MOV		R0, #HIGH FS_BATBUF_BK1
	CALL	_fat16_fat_wr_ffff
	JMP		fatbuf_copy_from_dec1

proc_fat_decbuf_32:
	MOV		R0, #HIGH FS_FATBUF
	CALL	fat32_fat_wr_link
	CALL	_write_sector_from_fatbuf
	MOV		R0, #HIGH FS_BATBUF_BK1
	CALL	fat32_fat_wr_0fffffff

fatbuf_copy_from_dec1:
	CALL	copy_decbuf_2_fatbuf

	MOV		R1, #HIGH FS_BATBUF_BK2
	MOV		R3, #HIGH FS_BATBUF_BK1
	CALL	_src_r1_to_des_r3
	SETB	C
	RET

get_blank_in_fatbuf:
	MOV		R4, DPL
	MOV		A, DPH
	CLR		C
	SUBB	A, #HIGH (FS_FATBUF - FS_BATBUF_BK1)
	MOV		R5, A		;record fat offset in one sector, for fat_pos_2_cluster_32/16 invole

	JB		b_sd_flash_flg, use_sd_srh_addr1
	MOV		DPTR, #uc_fs_fat_srh_addr1
	JMP		save_srh_param_r0
use_sd_srh_addr1:
	MOV		DPTR, #uc_fs_sd_fat_srh_addr1
save_srh_param_r0:		
	MOVX	A, @DPTR
	MOV		R0, A
	
	JB		b_sd_flash_flg, use_sd_srh_addr2
	MOV		DPTR, #uc_fs_fat_srh_addr2
	JMP		save_srh_param_r1
use_sd_srh_addr2:
	MOV		DPTR, #uc_fs_sd_fat_srh_addr2
save_srh_param_r1:		
	MOVX	A, @DPTR
	MOV		R1, A

	MOV		DPL, R4
	MOV		DPH, R5

	JB		b_fs_fat_type_flg, get_blank_in_fatbuf_32
	CALL	_fat_pos_2_cluster_16
	JMP		fatbuf_chk_cap

get_blank_in_fatbuf_32:
	CALL	_fat_pos_2_cluster_32

fatbuf_chk_cap:
	CALL	_check_capacity
	JNC		fatbuf_get_free_clst_ok
	JMP		card_is_full

fatbuf_get_free_clst_ok:
	MOV		uc_fs_cur_cluster1, uc_fs_tmp_var4
	MOV		uc_fs_cur_cluster2, uc_fs_tmp_var5
	MOV		uc_fs_cur_cluster3, uc_fs_tmp_var6
	JB		b_fs_fat_type_flg, fatbuf_link_update_32
	MOV		R0, #HIGH FS_FATBUF
	CALL	_fat16_fat_wr_link
	CALL	_fat16_fat_wr_ffff
	JMP		fatbuf_update_ret

fatbuf_link_update_32:
	MOV		R0, #HIGH FS_FATBUF
	CALL	fat32_fat_wr_link
	CALL	fat32_fat_wr_0fffffff	
	
fatbuf_update_ret:	
	SETB	C
	RET

/************************************************************************
************************************************************************/
_fwrite_one_sector:
	MOV		DPTR, #uc_fs_file_cur_sec_in_clst
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var1, A
	MOV		DPTR, #uc_fs_file_cur_clst2
	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster2, A
	MOV		DPTR, #uc_fs_file_cur_clst1
	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster1, A

	JB		b_fs_fat_type_flg, fwrite_sector_fat32
	;FAT16
	JMP		fw_cal_sector_actual_addr

fwrite_sector_fat32:
	MOV		DPTR, #uc_fs_file_cur_clst3
	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster3, A

fw_cal_sector_actual_addr:
	CALL	_get_cluster_fir_sector

	DEC		uc_fs_tmp_var1	
	MOV		A, uc_fs_tmp_var1

	ADD		A, sfc_logblkaddr0
	MOV		sfc_logblkaddr0, A
	CLR		A
	ADDC	A, sfc_logblkaddr1
	MOV		sfc_logblkaddr1, A
	CLR		A
	ADDC	A, sfc_logblkaddr2
	MOV		sfc_logblkaddr2, A
	CLR		A
	ADDC	A, sfc_logblkaddr3
	MOV		sfc_logblkaddr3, A

	JMP		_write_sector_2_point



/************************************************************************
;input:R0(update link, DPH address, 21 or FS_FATBUF)
************************************************************************/
_fat16_fat_wr_link:
	MOV		A, uc_fs_tmp_var2
	MOV		B, #2
	MUL		AB
	MOV		DPL, A
	MOV		A, R0			
	ADD		A, B
	MOV		DPH, A			;goto fat old cluster position, wr link

	MOV		A, uc_fs_cur_cluster2
	MOVX	@DPTR, A
	MOV		A, uc_fs_cur_cluster1
	INC		DPTR
	MOVX	@DPTR, A
	;above complete write link
	RET



/************************************************************************
;input:R0(update link, DPH address, 21 or FS_FATBUF)
************************************************************************/
_fat16_fat_wr_ffff:
	MOV		A, uc_fs_cur_cluster1
	MOV		B, #2
	MUL		AB
	MOV		DPL, A
	MOV		A, R0			
	ADD		A, B
	MOV		DPH, A			;goto fat new cluster position, wr 0xffff

	MOV		A, #0FFH
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	;above complete write 0xffff
	RET



/************************************************************************
;input:R0(update link, DPH address, 21 or FS_FATBUF)
************************************************************************/
fat32_fat_wr_link:
	MOV		A, uc_fs_tmp_var2
	ANL		A, #01111111B
	MOV		B, #4
	MUL		AB
	MOV		DPL, A
	MOV		A, R0			
	ADD		A, B
	MOV		DPH, A			;goto fat old cluster position, wr link

	MOV		A, uc_fs_cur_cluster2
	MOVX	@DPTR, A
	MOV		A, uc_fs_cur_cluster1
	INC		DPTR
	MOVX	@DPTR, A
	CLR		A
	INC		DPTR
	MOVX	@DPTR, A
	MOV		A, uc_fs_cur_cluster3
	INC		DPTR
	MOVX	@DPTR, A
	;above complete write link
	RET



/************************************************************************
;input:R0(update link, DPH address, 21 or FS_FATBUF)
************************************************************************/
fat32_fat_wr_0fffffff:
	MOV		A, uc_fs_cur_cluster1
	ANL		A, #01111111B
	MOV		B, #4
	MUL		AB
	MOV		DPL, A
	MOV		A, R0			
	ADD		A, B
	MOV		DPH, A			;goto fat new cluster position, wr 0x0fffffff

	MOV		A, #0FFH
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	MOV		A, #0FH
	INC		DPTR
	MOVX	@DPTR, A
	MOV		A, #0FFH
	INC		DPTR
	MOVX	@DPTR, A
	RET						;above complete write 0x0fffffff


/************************************************************************
************************************************************************/
copy_decbuf_2_fatbuf:
	MOV		R3, #HIGH FS_FATBUF
	MOV		R1, #HIGH FS_BATBUF_BK1
	JMP		_src_r1_to_des_r3


/************************************************************************
************************************************************************/
_first_dec_2_fat:
	JB		b_sd_flash_flg, get_from_sd1_1
	MOV		DPTR, #uc_fs_fat_srh_addr1
	JMP		get_log_addr0_1
get_from_sd1_1:
	MOV		DPTR, #uc_fs_sd_fat_srh_addr1	
get_log_addr0_1:	
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A
 
	JB		b_sd_flash_flg, get_from_sd2_1
	MOV		DPTR, #uc_fs_fat_srh_addr2
	JMP		get_log_addr1_1
get_from_sd2_1:
	MOV		DPTR, #uc_fs_sd_fat_srh_addr2
get_log_addr1_1:	
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	
	MOV		sfc_logblkaddr2, #00H
	MOV		sfc_logblkaddr3, #00H
	CALL	_fs_read_sector

	MOV		R3, #HIGH FS_FATBUF
	MOV		R1, #HIGH FS_BATBUF_BK1
	CALL	_src_r1_to_des_r3		;R0, R1->R2, R3
	RET
#endif


#ifdef	FS_FWRITE_OVER_DEF
PUBLIC	_wr_fatbuf_2_card
PUBLIC	_update_file_entry
PUBLIC  _write_file_header
/*******************************************************************************
********************************************************************************/
_wr_fatbuf_2_card:
	JB		b_sd_flash_flg, get_from_sd1_2
	MOV		DPTR, #uc_fs_fat_srh_addr1
	JMP		get_log_addr0_2
get_from_sd1_2:
	MOV		DPTR, #uc_fs_sd_fat_srh_addr1	
get_log_addr0_2:	
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A
	
	JB		b_sd_flash_flg, get_from_sd2_2
	MOV		DPTR, #uc_fs_fat_srh_addr2
	JMP		get_log_addr1_2
get_from_sd2_2:
	MOV		DPTR, #uc_fs_sd_fat_srh_addr2
get_log_addr1_2:	
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	
	MOV		sfc_logblkaddr2, #00H
	MOV		sfc_logblkaddr3, #00H

	CALL	_write_sector_fatbuf
	RET
	


_update_file_entry:
	MOV		DPTR, #uc_fs_sav_entry_sect4
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr3, A
	MOV		DPTR, #uc_fs_sav_entry_sect3
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr2, A
	MOV		DPTR, #uc_fs_sav_entry_sect2
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	MOV		DPTR, #uc_fs_sav_entry_sect1
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A
	CALL	_fs_read_sector

file_entry_sector_ok:
	MOV		DPTR, #uc_fs_cr_bgn_cluster1
	MOVX	A, @DPTR		
	MOV		R3, A	;B1
	MOV		DPTR, #uc_fs_cr_bgn_cluster2
	MOVX	A, @DPTR		
	MOV		R6, A	;B2
	MOV		DPTR, #uc_fs_cr_bgn_cluster3
	MOVX	A, @DPTR		
	MOV		R7, A	;B3

	MOV		DPTR, #uc_fs_sav_entry_dpl
	MOVX	A, @DPTR
	MOV		R0, A
	MOV		DPTR, #uc_fs_sav_entry_dph
	MOVX	A, @DPTR
	MOV		R1, A
	MOV		A, R0
	ADD		A, #14H
	MOV		DPL, A
	MOV		A, R1
	ADDC	A, #00H
	MOV		DPH, A						;reach file entry length
	
	JB		b_fs_fat_type_flg, wr_fat32_cluster
	CLR		A
	MOVX	@DPTR, A					;14H
	INC		DPTR
	MOVX	@DPTR, A					;15H
	JMP		wr_file_length

wr_fat32_cluster:
	CLR		A
	MOVX	@DPTR, A					;14H	
	INC		DPTR
	MOV		A, R7
	MOVX	@DPTR, A					;15H

wr_file_length:
	INC		DPTR						;16H
	INC		DPTR						;17H
	INC		DPTR						;18H
	INC		DPTR						;19H

	INC		DPTR						;2AH
	MOV		A, R6
	MOVX	@DPTR, A
	INC		DPTR						;2BH
	MOV		A, R3
	MOVX	@DPTR, A
	
	INC		DPTR
	MOV		A, ul_fs_byte_offset + 2
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, ul_fs_byte_offset + 3
	MOVX	@DPTR, A
	INC		DPTR 
	MOV		A, ul_fs_byte_offset
	MOVX	@DPTR, A
	INC		DPTR 
	MOV		A, ul_fs_byte_offset + 1
	MOVX	@DPTR, A

	SETB	b_sfc_immediate_wr_flg
	JMP		_write_sector



_write_file_header:
	MOV		DPTR, #uc_fs_cr_bgn_cluster1
	MOVX	A, @DPTR		
	MOV		uc_fs_cur_cluster1, A	;B1
	MOV		DPTR, #uc_fs_cr_bgn_cluster2
	MOVX	A, @DPTR		
	MOV		uc_fs_cur_cluster2, A	;B2
	MOV		DPTR, #uc_fs_cr_bgn_cluster3
	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster3, A

	CALL	_get_cluster_fir_sector

	SETB	b_sfc_immediate_wr_flg
	JMP		_write_sector
#endif



#ifdef	FS_GET_LONG_NAME_DEF
PUBLIC  _get_long_file_name
PUBLIC  _get_dir_name
/*******************************************************************************
bit FS_Get_LongName
change global var:uc_fs_tmp_var1, uc_fs_tmp_var2, uc_fs_tmp_var3, uc_fs_tmp_var4, uc_fs_tmp_var5, uc_fs_tmp_var6, uc_fs_tmp_var7
			uc_fs_cur_cluster1, uc_fs_cur_cluster2, uc_fs_cur_cluster3
output:
********************************************************************************/
_get_dir_name:
_get_long_file_name:
	MOV		DPTR, #ui_fs_item_name
	MOV		A, DPL	
	MOV		R0, #uc_fs_layer2_dir_bc1
	MOV		@R0, A
	MOV		A, DPH
	MOV		R0, #uc_fs_layer2_dir_bc2
	MOV		@R0, A

	MOV		R0, #FS_NAME_LEN_CONST
	CLR		A
clr_name_var_loop:
	MOVX	@DPTR, A
	INC		DPTR
	DJNZ	R0, clr_name_var_loop

	MOV		uc_fs_tmp_var7, #00H			;uc_fs_tmp_var7:count long file name length

	MOV		DPTR, #uc_fs_entry_sector1
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A

	MOV		DPTR, #uc_fs_entry_sector2
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	
	MOV		DPTR, #uc_fs_entry_sector3
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr2, A

	MOV		DPTR, #uc_fs_entry_sector4
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr3, A

	CALL	_fs_read_sector
	CALL	_sector_word_reverse

	MOV		DPTR, #uc_fs_entry_no
	MOVX	A, @DPTR
	MOV		R7, A					;R7 save uc_fs_entry_no
	MOV		R0, A
	MOV		R1, #00H
	MOV		R2, #5
	CALL	_l_shift
	MOV		A, R1
	ADD		A, #HIGH FS_FCOMBUF
	MOV		DPH, A
	MOV		DPL, R0

	MOV		uc_fs_tmp_var6, R7			;initial entry No., save in uc_fs_tmp_var6

get_lfn_loop:
	MOV		A, uc_fs_tmp_var6
	JNZ		upward_get_lname 
	JMP		span_sector_chk_lfn

upward_get_lname:
	DEC		uc_fs_tmp_var6				;uc_fs_tmp_var6:current entry No.
		
	MOV		A, DPL
	CLR		C
	SUBB	A, #32
	MOV		DPL, A
	MOV		A, DPH
	SUBB	A, #00H
	MOV		DPH, A

get_lfn_loop_1:
	MOVX	A, @DPTR
	CJNE	A, #0E5H, get_lfn_chk_lfn

lfn_over_ready_ret:
	MOV		A, uc_fs_tmp_var7
	JZ		 not_get_lfn_ret
	JMP		get_lfn_ok_ret

not_get_lfn_ret:
	CLR		C			;no lfn entry
	RET

get_lfn_chk_lfn:
	MOV		A, DPL
	ADD		A, #11
	MOV		DPL, A

	MOVX	A, @DPTR
	CJNE	A, #0FH, lfn_over_ready_ret
	CLR		C
	MOV		A, DPL
	SUBB	A, #10
	MOV		DPL, A

	MOV		R6, DPL		;entry index
	MOV		R7, DPH

	;now begin to get file long name
	MOV		R0, #uc_fs_layer2_dir_bc1
	MOV		A, @R0
	MOV		R2, A
	MOV		R0, #uc_fs_layer2_dir_bc2
	MOV		A, @R0
	MOV		R3, A			;R2, R3:ui_fs_item_name index

	MOV		R4, #5			;1st part in entry, 5 word
	MOV		R5, #00H		;3 part counter
copy_lfn_part:				;
	MOV		DPL, R6
	MOV		DPH, R7
	MOVX	A, @DPTR
	MOV		R0, A			;tmp save
	INC		DPTR			;
	MOVX	A, @DPTR
	MOV		R1, A			;tmp save
	JNZ		lfn_not_end
	CJNE	R0, #00H, lfn_not_end
	JMP		lfn_is_over

lfn_not_end:
	INC		DPTR
	MOV		R6, DPL
	MOV		R7, DPH
	MOV		DPL, R2
	MOV		DPH, R3
	MOV		A, R0
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, R1
	MOVX	@DPTR, A
	INC		DPTR
	MOV		R2, DPL
	MOV		R3, DPH
	INC		uc_fs_tmp_var7
	INC		uc_fs_tmp_var7
	MOV		A, uc_fs_tmp_var7
	CJNE	A, #FS_NAME_LEN_CONST, lfn_not_end_goon
	JMP		get_lfn_ok_ret

lfn_not_end_goon:
	DJNZ	R4, copy_lfn_part

	INC		R5
	CJNE	R5, #01H, lfn_jmp_3rd_part
	MOV		R4, #6			;2nd part in entry, 6 word
	MOV		A, R6
	ADD		A, #3
	MOV		R6, A			;update DPTR
	JMP		copy_lfn_part

lfn_jmp_3rd_part:
	CJNE	R5, #02H, lfn_need_srh_lst_entry
	MOV		R4, #2			;3rd part in entry, 2 word
	MOV		A, R6
	ADD		A, #2
	MOV		R6, A			;update DPTR
	JMP		copy_lfn_part
	
lfn_need_srh_lst_entry:
	MOV		A, R6
	CLR		C
	SUBB	A, #32
	MOV		DPL, A
	MOV		A, R7
	SUBB	A, #00H
	MOV		DPH, A

	MOV		A, R2
	MOV		R0, #uc_fs_layer2_dir_bc1
	MOV		@R0, A
	MOV		A, R3
	MOV		R0, #uc_fs_layer2_dir_bc2
	MOV		@R0, A

	JMP		get_lfn_loop

lfn_is_over:
	JMP		lfn_over_ready_ret
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
get_lfn_ok_ret:
	MOV		A, uc_fs_tmp_var7
	CLR		C
	RRC		A
	MOV		DPTR, #uc_fs_item_name_length
	MOVX	@DPTR, A

	MOV		DPTR, #ui_fs_item_name	;process ui_fs_item_name
	MOV		R7, #32					;ui_fs_item_name is 32word
loop_odd_even:
	MOVX	A, @DPTR
	MOV		R6, A					;save odd number
	MOV		R4, DPH					;save DPTR
	MOV		R3, DPL
	INC		DPTR
	MOVX	A, @DPTR
	MOV		DPH, R4
	MOV		DPL, R3
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, R6				;restore odd number
	MOVX	@DPTR, A
	INC		DPTR
	DJNZ	R7, loop_odd_even
	
	SETB	C
	RET


span_sector_chk_lfn:
	CALL	_get_dir_pre_sector_addr
	JC		lnf_read_new_sector
	;not got previous sector
	JMP		lfn_over_ready_ret

lnf_read_new_sector:
	CALL	_fs_read_sector
	CALL	_sector_word_reverse
	
	MOV		DPTR, #FS_FCOMBUF + 1E0H
	MOV		uc_fs_tmp_var6, #15
	CALL	get_lfn_loop_1
	RET
#endif



#ifdef	FS_GET_83_NAME_DEF
PUBLIC  _get_83name
/*******************************************************************************
********************************************************************************/
_get_83name:
	MOV		DPTR, #uc_fs_entry_sector1
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A

	MOV		DPTR, #uc_fs_entry_sector2
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	
	MOV		DPTR, #uc_fs_entry_sector3
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr2, A

	MOV		DPTR, #uc_fs_entry_sector4
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr3, A

	CALL	_fs_read_sector
	CALL	_sector_word_reverse

	MOV		DPTR, #uc_fs_entry_no
	MOVX	A, @DPTR
	MOV		R0, A
	MOV		R1, #00H
	MOV		R2, #5
	CALL	_l_shift
	MOV		A, R1
	ADD		A, #HIGH FS_FCOMBUF
	MOV		R7, A		;save DPTR
	MOV		A, R0
	MOV		R6, A

	MOV		DPTR, #uc_fs_item_83_name
	MOV		R0, DPL
	MOV		R1, DPH

	MOV		R4, #11
loop_copy_name:
	MOV		DPL, R6
	MOV		DPH, R7
	MOVX	A, @DPTR
	INC		DPTR
	MOV		R6, DPL
	MOV		R7, DPH
	MOV		DPL, R0
	MOV		DPH, R1
	MOVX	@DPTR, A
	INC		DPTR
	MOV		R0, DPL
	MOV		R1, DPH
	DJNZ	R4, loop_copy_name
	RET
#endif



#ifdef GET_ITEM_PAR_DEF
PUBLIC  _get_item_par
/*******************************************************************************
GET_ITEM_PAR
Input:	ui_fs_item_num, ui_fs_item_num+1;	CUR_ITEM_IDX2, CUR_ITEM_IDX1
Condition: invoke GET_ITEM_NUMBER firstly

Return:	0 or 0xffh
Output global var:ENTRY_CLUSTER1, 2, 3
			ENTRY_SECTOR_OFFSET, 
			uc_fs_entry_no
			uc_fs_begin_cluster1, uc_fs_begin_cluster2
			uc_fs_cur_cluster1, uc_fs_cur_cluster2
			ul_fs_file_length1, 2, 3, 4

used:uc_fs_tmp_var1, uc_fs_tmp_var2, uc_fs_tmp_var3, uc_fs_tmp_var4
char get_item_par();
********************************************************************************/
_get_item_par:
	MOV		DPL, uc_fs_tmp_var1
	MOV		DPH, uc_fs_tmp_var3

	JNB		b_fs_fat_type_flg, get_par_fat16
	MOV		A, DPL
	CLR		C
	SUBB	A, #11
	MOV		DPL, A
	MOV		A, DPH
	SUBB	A, #00H
	MOV		DPH, A
	
	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster3, A
	MOV		uc_fs_begin_cluster3, A
	MOV		A, DPL
	ADD		A, #5
	MOV		DPL, A
	JMP		get_other_par

get_par_fat16:
	MOV		A, DPL
	CLR		C
	SUBB	A, #6
	MOV		DPL, A
	MOV		A, DPH
	SUBB	A, #00H
	MOV		DPH, A

get_other_par:
	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster2, A
	MOV		uc_fs_begin_cluster2, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV		uc_fs_begin_cluster1, A
	MOV		uc_fs_cur_cluster1, A

	INC		DPTR
	MOVX	A, @DPTR
	MOV		R0, #ul_fs_file_length+2
	MOV		@R0, A

	INC		DPTR
	INC		R0
	MOVX	A, @DPTR
	MOV		@R0, A

	INC		DPTR
	MOV		R0, #ul_fs_file_length
	MOVX	A, @DPTR
	MOV		@R0, A

	INC		DPTR
	INC		R0
	MOVX	A, @DPTR
	MOV		@R0, A
	RET
#endif


#ifdef	SRH_FILE
PUBLIC  __get_item_idx_from_type_idx
/*******************************************************************************
Input:R7(First Par):type index				(uc_fs_tmp_var3(hi), uc_fs_tmp_var6(lo))
				R5(Second Par):type	1:file, 2:directory		(uc_fs_tmp_var5)

uc_fs_tmp_var4: count assigned type item lo	
uc_fs_tmp_var7: count assigned type item HI 
uc_fs_tmp_var2: item number in one sector(0-15)
uc_fs_tmp_var1: root fdt sector number (32)

DEFAULT INPUT: uc_fs_begin_cluster1, uc_fs_begin_cluster2

OUTPUT: b_fs_func_flg 0:fail 1:success
	CUR_ITEM_IDX1, CUR_ITEM_IDX2

void get_item_idx_from_type_idx(ui16 data index,  uc8 type)
********************************************************************************/
__get_item_idx_from_type_idx:
	CLR		b_fs_func_flg			;FAIL FLAG

	MOV		uc_fs_tmp_var5, R5 
	MOV		A, R7
	ADD		A, #1
	MOV		uc_fs_tmp_var6, A
	MOV		A, R6
	ADDC	A, #0
	MOV		uc_fs_tmp_var3, A

	MOV		DPTR, #uc_fs_idx_lo
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var4, A
	MOV		DPTR, #uc_fs_idx_hi
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var7, A
	MOV		DPTR, #uc_fs_cur_sec_in_cluster_bk
	MOVX	A, @DPTR
	MOV		R0, #uc_fs_cur_sec_in_cluster
	MOV		@R0, A

	JB		b_fs_dir_flg, idx_in_subdir
	
	;now in ROOT
	MOV		DPTR, #uc_fs_entry_item_num
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var2, A
	MOV		DPTR, #uc_fs_entry_lo		;zhanghuan 2008-5-19
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A
	MOV		DPTR, #uc_fs_entry_hi
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	MOV		sfc_logblkaddr2, #0
	MOV		sfc_logblkaddr3, #0

	MOV		DPTR, #uc_fs_fdt_sec_num
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var1, A

idx_read:
	CALL	_fs_read_sector

idx_set_pointer:
	MOV		DPTR, #uc_fs_dptr_hi		;zhanghuan 2008-5-19
	MOVX	A, @DPTR
	MOV		B, A
	MOV		DPTR, #uc_fs_dptr_lo
	MOVX	A, @DPTR
	MOV		DPH, B
	MOV		DPL, A
	JMP		idx_check_item

idx_in_subdir:
	;default input par:uc_fs_begin_cluster1, uc_fs_begin_cluster2
	JNB		b_fs_first_open, not_first_open
	CALL	_get_first_sector
	MOV		DPTR, #uc_fs_entry_lo		;zhanghuan 2008-5-19
	MOV		A, sfc_logblkaddr0
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_entry_hi
	MOV		A, sfc_logblkaddr1
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_entry_hi2
	MOV		A, sfc_logblkaddr2
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_entry_hi3
	MOV		A, sfc_logblkaddr3
	MOVX	@DPTR, A
not_first_open:
	MOV		DPTR, #uc_fs_entry_lo
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A
	MOV		DPTR, #uc_fs_entry_hi
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	MOV		DPTR, #uc_fs_entry_hi2
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr2, A
	MOV		DPTR, #uc_fs_entry_hi3
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr3, A
	CALL	_fs_read_sector

idx_in_subdir22:
	JB		b_fs_fat32_root_flg, idx_check_fat32
	JNB		b_fs_first_open, not_first_open11
	CLR		b_fs_first_open
	MOV		uc_fs_tmp_var2, #2
	MOV		DPTR, #(FS_FCOMBUF + 40H)
	JMP		idx_check_item
not_first_open11:
idx_check_fat32:
;	in fat32 root, no dot and dot dot entry
	CLR		b_fs_first_open
	MOV		DPTR, #uc_fs_entry_item_num
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var2, A
	MOV		DPTR, #uc_fs_dptr_hi		;zhanghuan 2008-5-19
	MOVX	A, @DPTR
	MOV		B, A
	MOV		DPTR, #uc_fs_dptr_lo
	MOVX	A, @DPTR
	MOV		DPH, B
	MOV		DPL, A

idx_check_item:
	CALL	_check_item_type
	MOV		A, R7
	CJNE	A, #0FFH, idx_check1
	JMP		idx_seek_fail
idx_check1:
	CJNE	A, #00H, idx_check2
	JMP		idx_go_on
idx_check2:
	CJNE	A, #02H, idx_file_proc
	;BE DIR
	MOV		A, uc_fs_tmp_var5
	CJNE	A, #02H, skip_and_add_idx
	;need count dir
	JMP		idx_add_comp_with_par1

idx_file_proc:
	MOV		A, uc_fs_tmp_var5
	CJNE	A, #01H, skip_and_add_idx
	;need count file

idx_add_comp_with_par1:
	MOV		A, uc_fs_tmp_var4
	ADD		A, #1
	MOV		uc_fs_tmp_var4, A
	MOV		A, uc_fs_tmp_var7
	ADDC	A, #0
	MOV		uc_fs_tmp_var7, A
	MOV		A, uc_fs_tmp_var6
	CJNE	A, uc_fs_tmp_var4, skip_and_add_idx
	MOV		A, uc_fs_tmp_var3
	CJNE	A, uc_fs_tmp_var7, skip_and_add_idx


	PUSH	DPH
	PUSH	DPL
	MOV		DPTR, #uc_fs_entry_sector1
	MOV		A, sfc_logblkaddr0
	MOVX	@DPTR, A

	MOV		DPTR, #uc_fs_entry_sector2
	MOV		A, sfc_logblkaddr1
	MOVX	@DPTR, A
	
	MOV		DPTR, #uc_fs_entry_sector3
	MOV		A, sfc_logblkaddr2
	MOVX	@DPTR, A

	MOV		DPTR, #uc_fs_entry_sector4
	MOV		A, sfc_logblkaddr3
	MOVX	@DPTR, A

	MOV		DPTR, #uc_fs_entry_no
	MOV		A, uc_fs_tmp_var2
	MOVX	@DPTR, A
	POP		DPL
	POP		DPH


	INC		uc_fs_tmp_var2
	MOV		A, uc_fs_tmp_var2
	CJNE	A, #16, idx_seek_ok_11
	MOV		uc_fs_tmp_var2, #0
	CLR		b_fs_next_sector_flg

	JNB		b_fs_dir_flg, fat16_directory

	CALL	parse_sector_info
	MOV		uc_fs_cur_cluster1, R0
	MOV		uc_fs_cur_cluster2, R1
	MOV		uc_fs_cur_cluster3, R2
	CALL	_get_next_sector
	MOV		DPTR, #FS_FCOMBUF
	SETB	b_fs_func_flg
idx_seek_ok_11:
	JMP		idx_seek_ok

skip_and_add_idx:
idx_go_on:
	INC		uc_fs_tmp_var2
	MOV		A, uc_fs_tmp_var2
	CJNE	A, #16, rdy_check_item
	MOV		uc_fs_tmp_var2, #0

	JNB		b_fs_dir_flg, idx_root_inc

	CLR		b_fs_next_sector_flg
	CALL	parse_sector_info				;zhanghuan 2008-5-19
	MOV		uc_fs_cur_cluster1, R0
	MOV		uc_fs_cur_cluster2, R1
	MOV		uc_fs_cur_cluster3, R2
	CALL	_get_next_sector

	JB		b_fs_file_end_flg, idx_seek_fail
	MOV		DPTR, #FS_FCOMBUF
	JMP		idx_check_item

rdy_check_item:
	JMP		idx_check_item

rdy_idx_read:
	JMP		idx_read

idx_root_inc:
	CALL	_inc_sector_addr
	MOV		DPTR, #FS_FCOMBUF
	MOV		A, DPH
	MOV		B, DPL
	MOV		DPTR, #uc_fs_dptr_hi		;zhanghuan 2008-5-19
	MOVX	@DPTR, A
	MOV		A, B
	MOV		DPTR, #uc_fs_dptr_lo
	MOVX	@DPTR, A
	DEC		uc_fs_tmp_var1
	MOV		A, uc_fs_tmp_var1
	JNZ		rdy_idx_read	

idx_seek_fail:
	CLR		b_fs_func_flg			;FAIL FLAG
	RET




fat16_directory:
	CALL	_inc_sector_addr
	MOV		DPTR, #FS_FCOMBUF

idx_seek_ok:
	MOV		uc_fs_tmp_var1, DPL
	MOV		uc_fs_tmp_var3, DPH
	MOV		A, DPH							;zhanghuan 2008-5-19
	MOV		B, DPL
	MOV		DPTR, #uc_fs_dptr_hi
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_dptr_lo
	MOV		A, B
	MOVX	@DPTR, A

	MOV		DPTR, #uc_fs_entry_lo
	MOV		A, sfc_logblkaddr0
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_entry_hi
	MOV		A, sfc_logblkaddr1
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_entry_hi2
	MOV		A, sfc_logblkaddr2
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_entry_hi3
	MOV		A, sfc_logblkaddr3
	MOVX	@DPTR, A

	MOV		DPTR, #uc_fs_entry_item_num
	MOV		A, uc_fs_tmp_var2
	MOVX	@DPTR, A

	MOV		A ,uc_fs_tmp_var4
	MOV		DPTR, #uc_fs_idx_lo
	MOVX	@DPTR, A
	MOV		A, uc_fs_tmp_var7
	MOV		DPTR, #uc_fs_idx_hi
	MOVX	@DPTR, A

/*DPTR - 0x20H, uc_fs_entry_no - 1*/
	MOV		A, uc_fs_tmp_var3	;high addr
	CJNE	A, #HIGH FS_FCOMBUF, not_6600h
	MOV		A, uc_fs_tmp_var1	;low addr
	CJNE	A, #00H, not_6600h
	MOV		uc_fs_tmp_var3, #(HIGH FS_FCOMBUF) + 2	;6800H
	MOV		uc_fs_tmp_var1, #00H

not_6600h:
	MOV		A, uc_fs_tmp_var2
	JZ		first_item
	DEC		uc_fs_tmp_var2
	JMP		label_ok
first_item:
	MOV		uc_fs_tmp_var2, #15
label_ok:
/*read current sector*/
	JNB		b_fs_func_flg, not_read_sector
	MOV		DPTR, #uc_fs_entry_sector1
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A 

	MOV		DPTR, #uc_fs_entry_sector2
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	
	MOV		DPTR, #uc_fs_entry_sector3
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr2, A

	MOV		DPTR, #uc_fs_entry_sector4
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr3, A

	CALL	_fs_read_sector

not_read_sector:
	MOV		R0, #uc_fs_cur_sec_in_cluster			;Save the current sector in cluster
	MOV		A, @R0
	MOV		DPTR, #uc_fs_cur_sec_in_cluster_bk
	MOVX	@DPTR, A

	SETB	b_fs_func_flg 	
	RET
#endif



#ifdef	ENTER_DIR_DEF
PUBLIC  __enter_subdir
/*******************************************************************************
uc_fs_tmp_var3 store dir index1(lo);		uc_fs_tmp_var4 store dir index2(hi)
uc_fs_subdir_stk_pt = 0, means in root
uc_fs_subdir_stk_buf structure:dir index2,dir index1,dir(being entered) begin cluster1,dir begin cluster2,dir begin cluster3			 
ATTENTION: uc_fs_subdir_stk_buf defined in 256byte(DATA), addressed by R0, only 8 bit
uc8 enter_subdir(ui16 dir_index);
return:0 or 0xffh
********************************************************************************/
__enter_subdir:
	MOV		uc_fs_tmp_var5, R7		;R7:input par
	MOV		uc_fs_tmp_var6, R6

	MOV		A, uc_fs_subdir_stk_pt
	MOV		B, #FS_MAX_SUBDIR_DEPTH
	CLR		C
	SUBB	A, B
	JNC		exceed_depth

	INC		uc_fs_subdir_stk_pt

	CALL	_get_item_par		;used:uc_fs_tmp_var1, uc_fs_tmp_var2, uc_fs_tmp_var3, uc_fs_tmp_var4

	MOV		DPTR, #uc_fs_subdir_stk_buf		;get uc_fs_subdir_stk_buf begin address
	MOV		A, uc_fs_subdir_stk_pt
	DEC		A
	MOV		B, #FS_BYTE_PER_LAYER
	MUL		AB
	ADD		A, DPL
	MOV		DPL, A
	MOV		A, DPH
	ADDC	A, #0
	MOV		DPH, A

	MOV		A, uc_fs_tmp_var6
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, uc_fs_tmp_var5
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, uc_fs_begin_cluster1
	MOVX	@DPTR, A 
	INC		DPTR
	MOV		A, uc_fs_begin_cluster2
	MOVX	@DPTR, A
	JNB		b_fs_fat_type_flg, enter_dir_ret
 	INC		DPTR
	MOV		A, uc_fs_begin_cluster3
	MOVX	@DPTR, A	

enter_dir_ret:
	SETB	b_fs_func_flg
	RET

exceed_depth:
	CLR		b_fs_func_flg
	RET
#endif



#ifdef	FS_EXIT_DIR_DEF
PUBLIC  _FS_ExitSubdir
/*******************************************************************************
OUT_FROM_DIR
ui16 out_from_dir(void);
return:0xffffh or the dir index having been out
********************************************************************************/
_FS_ExitSubdir:
	MOV		A, uc_fs_subdir_stk_pt
	JZ		pt_err		;now is root, cannot up

	MOV		DPTR, #uc_fs_subdir_stk_buf	;get uc_fs_subdir_stk_buf begin address
	MOV		A, uc_fs_subdir_stk_pt
	DEC		A
	MOV		B, #FS_BYTE_PER_LAYER
	MUL		AB
	ADD		A, DPL
	MOV		DPL, A
	MOV		A, DPH
	ADDC	A, #0
	MOV		DPH, A

	MOVX	A, @DPTR		;get the index of the dir being out 
	MOV		R6, A
	CLR		A
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV		R7, A
	CLR		A
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A		
	INC		DPTR
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
		
	DEC		uc_fs_subdir_stk_pt
	RET

pt_err:
	MOV		R7, #0FFH
	MOV		R6, #0FFH	
	RET
#endif



#ifdef	RECORD_MODULE_DIR
PUBLIC  _srh_wavdir_in_root
PUBLIC  _wr_wav_rec_entry
PUBLIC  _wr_dir_dot
/*******************************************************************************
search wav_rec directory in root, if exiset, get dir index, return 0x01;
if not exist, get new-created dir position which record first del entry to create wav_rec
					RECORD entry position in uc_fs_sav_entry_dpl, uc_fs_sav_entry_dph.sav_entry_sect123
					return 0x02
if fail, return 00;
output global var:wav_dir_idx 1, 2(wav_rec directory index)
********************************************************************************/
_srh_wavdir_in_root:
	MOV		ui_fs_subdir_num, #0
	MOV		ui_fs_subdir_num+1, #0
	CLR		b_fs_del_entry_ext_flg
	JB		b_fs_fat_type_flg, fat32_srh_root
	JMP		fat16_srh_root
fat32_srh_root:
 	MOV		uc_fs_begin_cluster1, #02H
	MOV		uc_fs_begin_cluster2, #00H
	MOV		uc_fs_begin_cluster3, #00H	
	CALL	_get_first_sector	
	MOV		uc_fs_tmp_var2, #0
	JMP		root_scan_set_dptr

fat16_srh_root:
	MOV		DPTR, #uc_fs_fdt_sec_num
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var1, A

	MOV		DPTR, #uc_fs_fdt_begin_1
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A
	MOV		DPTR, #uc_fs_fdt_begin_2
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	MOV		sfc_logblkaddr2, #0
	MOV		sfc_logblkaddr3, #0
	MOV		uc_fs_tmp_var2, #0

root_sector_read2dec:
	CALL	_fs_read_sector

root_scan_set_dptr:
	MOV		DPTR, #FS_FCOMBUF

root_entry_scan:
	CALL	_check_item_type
	CJNE	R7, #02H, root_classify_1
	CALL	_chk_dir_name
	JC		get_wav_rec_dir	

	MOV		A, ui_fs_subdir_num + 1
	ADD		A, #1
	MOV		ui_fs_subdir_num + 1, A
	MOV		A, ui_fs_subdir_num
	ADDC	A, #0
	MOV		ui_fs_subdir_num, A
 
	JMP		root_goto_next_entry

root_classify_1:
	CJNE	R7, #00H, root_classify_2
	CALL	_chk_delete_item
	JC		root_goto_next_entry
	;be del entry
	JB		b_fs_del_entry_ext_flg, skip_sav_del_entry
	DEC		DPL
	CALL	save_wavrec_entry_2_par	
	
	MOV		DPTR, #ui_fs_wav_dir_idx
	MOV		A, ui_fs_subdir_num
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, ui_fs_subdir_num + 1
	MOVX	@DPTR, A
	SETB	b_fs_del_entry_ext_flg

skip_sav_del_entry:
	;in chk_delete_item, R0, R1 save next entry DPTR, here is special
	MOV		DPL, R0
	MOV		DPH, R1
	JMP		root_goto_next_entry

root_classify_2:
	CJNE	R7, #0FFH, root_goto_next_entry
	JMP		not_find_wav_rec_proc

get_wav_rec_dir:
	MOV		DPTR, #ui_fs_wav_dir_idx
	MOV		A, ui_fs_subdir_num
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, ui_fs_subdir_num + 1
	MOVX	@DPTR, A
	MOV		R7, #01H
	RET

root_goto_next_entry:
	INC		uc_fs_tmp_var2
	MOV		A, uc_fs_tmp_var2
	CJNE	A, #16, root_entry_scan
	MOV		uc_fs_tmp_var2, #0

	JNB		b_fs_fat_type_flg, root_fat16_self_inc

	CLR		b_fs_next_sector_flg
	CALL	_get_next_sector
	JB		b_fs_file_end_flg, root_fat32_over
	JMP		root_scan_set_dptr

jmp_got_wavrec_pos_ret:
	JMP		got_wavrec_pos_ret

root_fat16_self_inc:
	CALL	_inc_sector_addr
	DEC		uc_fs_tmp_var1
	MOV		A, uc_fs_tmp_var1
	JNZ		root_fat16_not_over
	;now fat16 root fdt is over, need check if exist del entry
	;if not exist, error exist
	JB		b_fs_del_entry_ext_flg, jmp_got_wavrec_pos_ret
	MOV		R7, #00H
	RET


root_fat32_over:
	JNB		b_fs_del_entry_ext_flg, got_wavrec_pos_ret_fat32
	JMP		got_wavrec_pos_ret
got_wavrec_pos_ret_fat32:
	;FAT32 root is over, and not exist del entry, so extend root
	MOV		uc_fs_offset_in_sec1, DPL
	MOV		uc_fs_offset_in_sec2, DPH
	MOV		uc_fs_tmp_var2, sfc_logblkaddr0
	MOV		uc_fs_tmp_var3, sfc_logblkaddr1
	MOV		uc_fs_tmp_var7, sfc_logblkaddr2
;	MOV		uc_fs_tmp_var8, sfc_logblkaddr3
	MOV		DPTR, #uc_fs_tmp_var8
	MOV		A,sfc_logblkaddr3
	MOVX	@DPTR,A	

	JB		b_sd_flash_flg, from_sd_get_blank_addr1_1	//AJ Modify 2011-8-17
	MOV		DPTR, #uc_fs_fat_srh_addr1	
	JMP		get_blank_para_r7_1	
from_sd_get_blank_addr1_1:
	MOV		DPTR, #uc_fs_sd_fat_srh_addr1
get_blank_para_r7_1:	
	MOVX	A, @DPTR
	MOV		R7, A
	
	JB		b_sd_flash_flg, from_sd_get_blank_addr2_1	//AJ Modify 2011-8-17
	MOV		DPTR, #uc_fs_fat_srh_addr2
	JMP		get_blank_para_r5_1
from_sd_get_blank_addr2_1:
	MOV		DPTR, #uc_fs_sd_fat_srh_addr2
get_blank_para_r5_1:	
	MOVX	A, @DPTR
	MOV		R5, A	
	CALL	__seek_free_cluster
	JC		root_extend_for_wavrec
	MOV		R7, #00H
	RET

root_extend_for_wavrec:
	CALL	_extend_entry_update_fat_1	;write ffff
	
	CALL	_extend_entry_update_fat_2	;write link

	;clear dir all entry
	CALL	_get_cluster_fir_sector	;get new-got cluster first sector address
	MOV		DPTR, #uc_fs_sec_per_cluster
	MOVX	A, @DPTR
	DEC		A
	MOV		uc_fs_tmp_var7, A
clr_dir_entry_root:	
	CALL	_clr_512byte
	CALL	_write_sector
	CALL	_inc_sector_addr
	DJNZ	uc_fs_tmp_var7, clr_dir_entry_root
	SETB	b_sfc_immediate_wr_flg
	CALL	_write_sector
	CALL	_clr_512byte
	CALL	_get_cluster_fir_sector	;get new-got cluster first sector address

	MOV		DPTR, #ui_fs_wav_dir_idx
	MOV		A, ui_fs_subdir_num
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, ui_fs_subdir_num + 1
	MOVX	@DPTR, A

	MOV		DPTR, #FS_FCOMBUF
	CALL	save_wavrec_entry_2_par
	JMP		got_wavrec_pos_ret

root_fat16_not_over:
	JMP		root_sector_read2dec

not_find_wav_rec_proc:
														;search root is over, not find WAV_REC directory, so need create it
	JB		b_fs_del_entry_ext_flg, got_wavrec_pos_ret
	MOV		R6, DPL										;not get del entry, so create WAV_REC here
	MOV		R7, DPH

	MOV		DPTR, #ui_fs_wav_dir_idx
	MOV		A, ui_fs_subdir_num
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, ui_fs_subdir_num + 1
	MOVX	@DPTR, A

	MOV		DPL, R6
	MOV		DPH, R7

	MOV		A, DPL
	CLR		C
	SUBB	A, #32
	MOV		DPL, A
	MOV		A, DPH
	SUBB	A, #00H
	MOV		DPH, A
	CALL	save_wavrec_entry_2_par

got_wavrec_pos_ret:
	MOV		R7, #02H		;will overwrite del entry befor
	RET

save_wavrec_entry_2_par:
	MOV		R6, DPL
	MOV		R7, DPH

	MOV		DPTR, #uc_fs_dir_entry_dpl
	MOV		A, R6
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_dir_entry_dph
	MOV		A, R7
	MOVX	@DPTR, A

	MOV		DPTR, #uc_fs_dir_entry_sect4
	MOV		A, sfc_logblkaddr3
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_dir_entry_sect3
	MOV		A, sfc_logblkaddr2
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_dir_entry_sect2
	MOV		A, sfc_logblkaddr1
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_dir_entry_sect1
	MOV		A, sfc_logblkaddr0
	MOVX	@DPTR, A
	RET


/*************************************************************************
*************************************************************************/
_wr_wav_rec_entry:
	MOV		DPTR, #uc_fs_dir_entry_sect4
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr3, A
	MOV		DPTR, #uc_fs_dir_entry_sect3
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr2, A
	MOV		DPTR, #uc_fs_dir_entry_sect2
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	MOV		DPTR, #uc_fs_dir_entry_sect1
	MOVX	A, @DPTR	
	MOV		sfc_logblkaddr0, A

	CALL	_fs_read_sector

	MOV		DPTR, #uc_fs_dir_entry_dpl
	MOVX	A, @DPTR
	MOV		R6, A
	MOV		DPTR, #uc_fs_dir_entry_dph
	MOVX	A, @DPTR
	MOV		DPH, A
	MOV		DPL, R6

	MOV		A, #4FH
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #56H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #43H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #49H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #20H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #45H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #20H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #20H
	MOVX	@DPTR, A
	INC		DPTR
	
	CALL	_wr_entry_sub1
	SETB	b_sfc_immediate_wr_flg
	JMP		_write_sector


/*************************************************************************
*************************************************************************/
_wr_dir_dot:
	CALL	_get_cluster_fir_sector
	MOV		DPTR, #uc_fs_sec_per_cluster
	MOVX	A, @DPTR
	DEC		A
	MOV		uc_fs_tmp_var7, A
clr_dir_content:	
	CALL	_clr_512byte
	CALL	_write_sector
	CALL	_inc_sector_addr
	DJNZ	uc_fs_tmp_var7, clr_dir_content
	SETB	b_sfc_immediate_wr_flg
	CALL	_write_sector
	CALL	_clr_512byte
	CALL	_get_cluster_fir_sector	;get new-got cluster first sector address
	
 	MOV		DPTR, #FS_FCOMBUF
	MOV		A, #20H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #2EH
	MOVX	@DPTR, A
	INC		DPTR

	MOV		R1, #6
	MOV		A, #20H
entry_loop_wr_20h:
	MOVX	@DPTR, A
	INC		DPTR
	DJNZ	R1, entry_loop_wr_20h

	CALL	wr_entry_sub1_1

	INC		DPTR
	CLR		A
	MOVX	@DPTR, A
	INC		DPTR

	JB		b_fs_fat_type_flg, wr_wavrec_dir_clusthi
	MOVX	@DPTR, A
	JMP		wav_dir_clusthi_over

wr_wavrec_dir_clusthi:
	MOV		A, uc_fs_cur_cluster3
	MOVX	@DPTR, A

wav_dir_clusthi_over:
	INC		DPTR
	CALL	wr_entry_sub1_2
	INC		DPTR
	MOV		A, uc_fs_cur_cluster2
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, uc_fs_cur_cluster1
	MOVX	@DPTR, A
	INC		DPTR
	CLR		A
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A

	INC		DPTR
	MOV		A, #2EH
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	INC		DPTR

	MOV		R1, #6
	MOV		A, #20H
entry_loop_wr_20h_1:
	MOVX	@DPTR, A
	INC		DPTR
	DJNZ	R1, entry_loop_wr_20h_1

	CALL	wr_entry_sub1_1
	INC		DPTR
	CLR		A
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	INC		DPTR

	CALL	wr_entry_sub1_2
	INC		DPTR
	MOV		R1, #6
	CLR		A
entry_loop_wr_00h_1:
	MOVX	@DPTR, A
	INC		DPTR
	DJNZ	R1, entry_loop_wr_00h_1

	SETB	b_sfc_immediate_wr_flg
	JMP		_write_sector
#endif



#ifdef	FS_DELETE_DEF
PUBLIC  _delete_file
/*******************************************************************************
delete file, the file must be opened(open_file), then delete
********************************************************************************/
_delete_file:
	CALL	del_file_entry

	MOV		R0, #ul_fs_file_length
	MOV		A, @R0
	JNZ		del_fat
	INC		R0
	MOV		A, @R0
	JNZ		del_fat
	INC		R0
	MOV		A, @R0
	JNZ		del_fat
	INC		R0
	MOV		A, @R0
	JNZ		del_fat
	RET
del_fat:
	CALL	del_file_fat
	RET


del_file_entry:
	MOV		DPTR, #uc_fs_entry_sector1
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A

	MOV		DPTR, #uc_fs_entry_sector2
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	
	MOV		DPTR, #uc_fs_entry_sector3
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr2, A

	MOV		DPTR, #uc_fs_entry_sector4
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr3, A

	CALL	_fs_read_sector

	MOV		DPTR, #uc_fs_entry_no
	MOVX	A, @DPTR
	MOV		R7, A
	MOV		R0, A
	MOV		R1, #00H
	MOV		R2, #5
	CALL	_l_shift
	MOV		A, R1
	ADD		A, #HIGH FS_FCOMBUF
	MOV		DPH, A
	MOV		DPL, R0

	INC		DPTR
	MOV		A, #0E5H
	MOVX	@DPTR, A		;write 0xe5
		
	MOV		uc_fs_tmp_var6, R7	;initial entry No., save in uc_fs_tmp_var6

erase_entry_loop:
	MOV		A, uc_fs_tmp_var6
	JNZ		upward_del_long_name 
	JMP		span_sector_get_lfn_entry

upward_del_long_name:
	DEC		uc_fs_tmp_var6		;uc_fs_tmp_var6:current entry No.
		
	MOV		A, DPL
	CLR		C
	SUBB	A, #32
	MOV		DPL, A
	MOV		A, DPH
	SUBB	A, #00H
	MOV		DPH, A

upward_del_long_name_2:
	MOVX	A, @DPTR
	CJNE	A, #0E5H, chk_if_lfn_entry
	JMP		erase_lfn_entry_end

chk_if_lfn_entry:
	MOV		R0, A		;save entry first byte
	MOV		R1, DPL		;DPH not change

	MOV		A, DPL
	ADD		A, #9
	MOV		DPL, A

	MOVX	A, @DPTR
	CJNE	A, #0FH, erase_lfn_entry_end
	MOV		DPL, R1
	MOV		A, #0E5H
	MOVX	@DPTR, A

	MOV		A, R0
	JB		ACC.6, erase_lfn_entry_end
	JMP		erase_entry_loop

erase_lfn_entry_end:
	SETB	b_sfc_immediate_wr_flg
	JMP		_write_sector


span_sector_get_lfn_entry:
	SETB	b_sfc_immediate_wr_flg
	CALL	_write_sector

	CALL	_get_dir_pre_sector_addr
	JC		new_sector_del_lfn_entry
	RET					;not got previous sector, return

new_sector_del_lfn_entry:
	CALL	_fs_read_sector
	
	MOV		DPTR, #FS_FCOMBUF + 1E1H
	MOV		uc_fs_tmp_var6, #15
	JMP		upward_del_long_name_2


del_file_fat:
	MOV		R0, uc_fs_begin_cluster1
	MOV		R1, uc_fs_begin_cluster2
	MOV		R2, uc_fs_begin_cluster3
	MOV		uc_fs_tmp_var1, #00H
	MOV		uc_fs_tmp_var2, #00H

cluster_del_loop:
	CALL	_cluster_2_fat_addr
	MOV		uc_fs_tmp_var6, DPL
	MOV		uc_fs_tmp_var7, DPH
	MOV		A, R3
	CJNE	A, uc_fs_tmp_var1, del_need_reread
	MOV		A, R4
	CJNE	A, uc_fs_tmp_var2, del_need_reread
	JMP		del_cluster_chk_end			;in the same sector

del_need_reread:
	MOV		A, uc_fs_tmp_var1
	JNZ		del_write_back
	MOV		A, uc_fs_tmp_var2
	JNZ		del_write_back

	;for the first time, need not write back to card
	MOV		uc_fs_tmp_var1, R3
	MOV		uc_fs_tmp_var2, R4
	JMP		del_need_reread_2		

del_write_back:
	MOV		uc_fs_tmp_var1, R3
	MOV		uc_fs_tmp_var2, R4

	CALL	_chk_save_srh_blank_sector
	SETB	b_sfc_immediate_wr_flg
	CALL	_write_sector

del_need_reread_2:
	MOV		sfc_logblkaddr0, uc_fs_tmp_var1
	MOV		sfc_logblkaddr1, uc_fs_tmp_var2
	MOV		sfc_logblkaddr2, #00H
	MOV		sfc_logblkaddr3, #00H
	CALL	_fs_read_sector

del_cluster_chk_end:
	MOV		DPL, uc_fs_tmp_var6
	MOV		DPH, uc_fs_tmp_var7

	JB		b_fs_fat_type_flg, lcall_fat32_del_fat
	MOVX	A, @DPTR
	MOV		R1, A			;save the number which point to next cluster high
	CLR		A
	MOVX	@DPTR, A		;clear cluster
	INC		DPTR
	MOVX	A, @DPTR		;save the number which point to next cluster low
	MOV		R0, A
	CLR		A
	MOVX	@DPTR, A		;clear cluster
	
//	CALL	_set_free_space	;aj modify 2011-9-14
	
	MOV		A, R0
	JNZ		del_chk_cluster_end
	MOV		A, R1
	JNZ		ljmp_cluster_del_loop
	JMP		del_fat_over

lcall_fat32_del_fat:
	JMP		fat32_del_fat	

ljmp_cluster_del_loop:
	JMP		jmp_cluster_del_loop

del_chk_cluster_end:
	ANL		A, #0F8H
	CJNE	A, #0F8H, ljmp_cluster_del_loop
	MOV		A, R1
	CJNE	A, #0FFH, ljmp_cluster_del_loop
	JMP		del_fat_over


/*** aj modify 2011-9-14 ***/
_set_free_space:
	PUSH	DPL
	PUSH	DPH
	PUSH	00H
	PUSH	01H
	PUSH	02H
	PUSH	03H
	PUSH	04H
	
/* release file length space, BYTE-->SECTOR */	
	MOV		R0, #ul_fs_file_length
	MOV		A, @R0
	MOV		R3, A
	
	INC		R0
	MOV		A, @R0
	MOV		R2, A
	
	INC		R0
	MOV		A, @R0
	MOV		R1, A
	
	INC		R0
	MOV		A, @R0
	MOV		R0, A
	
	//MOV		A, R4
	//MOV		R0, A
	
	MOV		R4, #9
	CALL	_r_shift_32bit
	
/* SECTOR+1, means include the sector witch not enough with 1 sector */	
	CLR		C
	
	MOV		A, R0
	ADD		A, #1
	MOV		R0, A
	
	MOV		A, R1
	ADDC	A, #0
	MOV		R1, A
	
	MOV		A, R2
	ADDC	A, #0
	MOV		R2, A
	
	MOV		A, R3
	ADDC	A, #0
	MOV		R3, A	
	
	CLR		C
	
	JB		b_sd_flash_flg, get_sd_spare_sapce
	MOV		DPTR, #ul_fs_spare_space+3
	MOVX	A, @DPTR
	ADD		A, R0
	MOVX	@DPTR, A
	
	MOV		DPTR, #ul_fs_spare_space+2
	MOVX	A, @DPTR
	ADDC	A, R1
	MOVX	@DPTR, A
	
	MOV		DPTR, #ul_fs_spare_space+1
	MOVX	A, @DPTR
	ADDC	A, R2
	MOVX	@DPTR, A
	
	MOV		DPTR, #ul_fs_spare_space
	MOVX	A, @DPTR
	ADDC	A, R3
	MOVX	@DPTR, A
	JMP		get_spare_space_accomplish
	
get_sd_spare_sapce:	
	MOV		DPTR, #ul_fs_sd_spare_space+3
	MOVX	A, @DPTR
	ADD		A, R0
	MOVX	@DPTR, A
	
	MOV		DPTR, #ul_fs_sd_spare_space+2
	MOVX	A, @DPTR
	ADDC	A, R1
	MOVX	@DPTR, A
	
	MOV		DPTR, #ul_fs_sd_spare_space+1
	MOVX	A, @DPTR
	ADDC	A, R2
	MOVX	@DPTR, A
	
	MOV		DPTR, #ul_fs_sd_spare_space
	MOVX	A, @DPTR
	ADDC	A, R3
	MOVX	@DPTR, A
	
get_spare_space_accomplish:	
	POP		04H
	POP		03H
	POP		02H
	POP		01H
	POP		00H
	POP		DPH
	POP		DPL
	RET	
/*** aj modify 2011-9-14 ***/
	

fat32_del_fat:
	MOVX	A, @DPTR			;get next cluster, store in R0, R1, R2, R3
	MOV		R1, A			;r3 highest, r0, lowest
	CLR		A
	MOVX	@DPTR, A		;clear cluster
	INC		DPTR
	MOVX	A, @DPTR
	MOV		R0, A
	CLR		A
	MOVX	@DPTR, A		;clear cluster
	INC		DPTR
	MOVX	A, @DPTR	
	MOV		R3, A
	CLR		A
	MOVX	@DPTR, A		;clear cluster			
	INC		DPTR
	MOVX	A, @DPTR
	MOV		R2, A
	CLR		A
	MOVX	@DPTR, A		;clear cluster

	//CALL	_set_free_space	;aj modify 2011-9-14

	MOV		A, R0
	JNZ		del_chk_cluster_end_32
	MOV		A, R1
	JNZ		jmp_cluster_del_loop
	MOV		A, R2
	JNZ		jmp_cluster_del_loop
	MOV		A, R3
	JNZ		jmp_cluster_del_loop
	JMP		del_fat_over

jmp_cluster_del_loop:
	JMP		cluster_del_loop

del_chk_cluster_end_32:
	MOV		A, R0
	ANL		A, #0F8H
	CJNE	A, #0F8H, jmp_cluster_del_loop
	MOV		A, R1
	CJNE	A, #0FFH, jmp_cluster_del_loop
	MOV		A, R2
	CJNE	A, #0FFH, jmp_cluster_del_loop
	MOV		A, R3
	ANL		A, #0FH				;mask high 4bit
	CJNE	A, #0FH, jmp_cluster_del_loop

del_fat_over:
	CALL	_set_free_space
	CALL	_chk_save_srh_blank_sector
   	SETB	b_sfc_immediate_wr_flg
	JMP		_write_sector



PUBLIC  _chk_save_srh_blank_sector

_chk_save_srh_blank_sector:
	CLR		C
	JB		b_sd_flash_flg, chk_sd_srh2_2
	MOV		DPTR, #uc_fs_fat_srh_addr2
	JMP		check_log_addr1_2
chk_sd_srh2_2:
	MOV		DPTR, #uc_fs_sd_fat_srh_addr2	
check_log_addr1_2:	
	MOVX	A, @DPTR
	SUBB	A, sfc_logblkaddr1
	
	JC		update_over
	JNZ		update_srh_blank_addr2
	
	CLR		C
	JB		b_sd_flash_flg, chk_sd_srh1_2
	MOV		DPTR, #uc_fs_fat_srh_addr1
	JMP		check_log_addr0_2
chk_sd_srh1_2:
	MOV		DPTR, #uc_fs_sd_fat_srh_addr1
check_log_addr0_2:	
	MOVX	A, @DPTR
	SUBB	A, sfc_logblkaddr0
	
	JC		update_over
	JZ		update_over
	JMP		update_srh_blank_addr1
	
update_srh_blank_addr2:	
	JB		b_sd_flash_flg, save_log_addr1_1
	MOV		DPTR, #uc_fs_fat_srh_addr2
	JMP		save_addr1_1
save_log_addr1_1:	
	MOV		DPTR, #uc_fs_sd_fat_srh_addr2
save_addr1_1:	
	MOV		A, sfc_logblkaddr1
	MOVX	@DPTR, A
	
update_srh_blank_addr1:	
	JB		b_sd_flash_flg, save_log_addr0_1
	MOV		DPTR, #uc_fs_fat_srh_addr1
	JMP		save_addr0_1
save_log_addr0_1:	
	MOV		DPTR, #uc_fs_sd_fat_srh_addr1
save_addr0_1:	
	MOV		A, sfc_logblkaddr0
	MOVX	@DPTR, A
	
update_over:
	RET


#endif



#ifdef	FS_FCREATE_DEF
PUBLIC  _write_file_entry
/*******************************************************************************
bit _write_file_entry(uc8 begin_cluster1, uc8 begin_cluster2, uc8 begin_cluster3);
Description:search blank or del entry, write entry content, if need extend dir fdt, update fat

return:0, extend dir fdt fail;1, ok
input global var:uc_fs_begin_cluster1, uc_fs_begin_cluster2, uc_fs_begin_cluster3
uc_fs_tmp_var2, uc_fs_tmp_var4, uc_fs_tmp_var5, uc_fs_tmp_var6
uc_fs_tmp_var1, uc_fs_tmp_var3, uc_fs_tmp_var7 save par
********************************************************************************/
_write_file_entry:
	MOV		DPTR, #uc_fs_cr_bgn_cluster1
	MOVX	A, @DPTR		
	MOV		R0, A	;B1
	MOV		DPTR, #uc_fs_cr_bgn_cluster2
	MOVX	A, @DPTR		
	MOV		R1, A	;B2
	MOV		DPTR, #uc_fs_cr_bgn_cluster3
	MOVX	A, @DPTR		
	MOV		R2, A	;B3

	;read free fat to decbuf
	CALL	_fat_clst_wr_end_sub

	MOV		DPTR, #ui_fs_file_type + 1
	MOVX	A, @DPTR
	JNB		ACC.3, not_crt_wav

/*cread wav file*/
	CALL	_get_first_sector		;fat16
	
	MOV		uc_fs_tmp_var2, #2		;fat16
	MOV		DPTR, #(FS_FCOMBUF + 40H)
	JMP		goon_chk_dir_entry
	
/*cread bin file*/
not_crt_wav:
	JNB		b_fs_fat_type_flg, fat16_root_fir_sec
fat32_root:
	CALL	_get_first_sector
	JMP		bin_label_1
	
fat16_root_fir_sec:
	MOV		DPTR, #uc_fs_fdt_begin_1
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A
	MOV		DPTR, #uc_fs_fdt_begin_2
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	MOV		sfc_logblkaddr2, #00H
	MOV		sfc_logblkaddr3, #00H
	
	MOV		DPTR, #uc_fs_fdt_sec_num
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var1, A
read_fat16_root:
	CALL	_fs_read_sector
	
bin_label_1:
	MOV		uc_fs_tmp_var2, #0		;fat16
	MOV		DPTR, #FS_FCOMBUF
	JMP		goon_chk_dir_entry

goon_chk_dir_entry:
	CALL	_check_item_type
	MOV		A, R7
	CJNE	A, #0FFH, chk_if_be_del_item
	JMP		get_blank_entry_in_dir

chk_if_be_del_item:
	CJNE	A, #00H, goon_chk_blank_or_del
	CALL	_chk_delete_item
	JC		goon_chk_blank_or_del
	DEC		DPL
	JMP		complete_dptr_location

goon_chk_blank_or_del:
	INC		uc_fs_tmp_var2
	MOV		A, uc_fs_tmp_var2
	CJNE	A, #16, goon_chk_dir_entry
	MOV		uc_fs_tmp_var2, #0

	MOV		DPTR, #ui_fs_file_type + 1
	MOVX	A, @DPTR
	JB		ACC.3, srh_voice_dir
srh_root_dir:
	JNB		b_fs_fat_type_flg, fat16_root_next_sec
srh_voice_dir:
	CLR		b_fs_next_sector_flg		;fat16, inc, judge exceed b
	CALL	_get_next_sector
	JMP		wr_file_entry_002
	
fat16_root_next_sec:
	CALL	_inc_sector_addr
	DEC		uc_fs_tmp_var1
	MOV		A, uc_fs_tmp_var1
	JNZ		read_fat16_root
	CLR  	 C
	RET

wr_file_entry_002:
	JB		b_fs_file_end_flg, extend_dir_fdt
	MOV		DPTR, #FS_FCOMBUF
	JMP		goon_chk_dir_entry

extend_dir_fdt:
	;save current dptr, sector address, for update fat(extend_entry_update_fat_2)
	MOV		uc_fs_offset_in_sec1, DPL
	MOV		uc_fs_offset_in_sec2, DPH
	MOV		uc_fs_tmp_var2, sfc_logblkaddr0
	MOV		uc_fs_tmp_var3, sfc_logblkaddr1
	MOV		uc_fs_tmp_var7, sfc_logblkaddr2
;	MOV		uc_fs_tmp_var8, sfc_logblkaddr3
	MOV		DPTR,#uc_fs_tmp_var8
	MOV		A,sfc_logblkaddr3
	MOVX	@DPTR,A	
	JB		b_sd_flash_flg, from_sd_get_blank_addr1_2	//AJ Modify 2011-8-17
	MOV		DPTR, #uc_fs_fat_srh_addr1
	JMP		get_blank_para_r7_2	
from_sd_get_blank_addr1_2:
	MOV		DPTR, #uc_fs_sd_fat_srh_addr1
get_blank_para_r7_2:	
	MOVX	A, @DPTR
	MOV		R7, A
	
	JB		b_sd_flash_flg, from_sd_get_blank_addr2_2	//AJ Modify 2011-8-17
	MOV		DPTR, #uc_fs_fat_srh_addr2
	JMP		get_blank_para_r5_2
from_sd_get_blank_addr2_2:
	MOV		DPTR, #uc_fs_sd_fat_srh_addr2
get_blank_para_r5_2:	
	MOVX	A, @DPTR
	MOV		R5, A	
	
	CALL	__seek_free_cluster
	JC		 get_pos_extend_dir_fdt
	RET		;card full, return 0
	
get_pos_extend_dir_fdt:			;extend dir fdt update fat
	CALL	_extend_entry_update_fat_1	;write ffff
	
	CALL	_extend_entry_update_fat_2;write link

	;clear dir all entry
	CALL	_get_cluster_fir_sector		;get new-got cluster first sector address
	MOV		DPTR, #uc_fs_sec_per_cluster
	MOVX	A, @DPTR
	DEC		A
	MOV		uc_fs_tmp_var7, A
clr_dir_entry_t:	
	CALL	_clr_512byte
	CALL	_write_sector
	CALL	_inc_sector_addr
	DJNZ	uc_fs_tmp_var7, clr_dir_entry_t
	SETB	b_sfc_immediate_wr_flg
	CALL	_write_sector
	CALL	_clr_512byte
	CALL	_get_cluster_fir_sector	;get new-got cluster first sector address
	MOV		DPTR, #FS_FCOMBUF + 20H


get_blank_entry_in_dir:			;write entry
	;1.adjust dptr, dptr-0x20
	MOV		A, DPL
	CLR		C
	SUBB	A, #20H
	MOV		DPL, A
	MOV		A, DPH
	SUBB	A, #0
	MOV		DPH, A

complete_dptr_location:
	;2.write file name
	;R0-R2 save index, R4, R5 save dptr, R3, R6, R7, save begin cluster
	MOV		R4, DPL
	MOV		R5, DPH	;save dptr

	MOV		DPTR, #uc_fs_sav_entry_dpl
	MOV		A, R4
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_sav_entry_dph
	MOV		A, R5
	MOVX	@DPTR, A

	MOV		DPTR, #uc_fs_wav_idx
	MOVX	A, @DPTR
	MOV		B, #100
	DIV		AB
	ADD		A, #30H
	MOV		R2, A	;3RD

	MOV		A, B
	MOV		B, #10
	DIV		AB
	ADD		A, #30H
	MOV		R1, A	;2ND
	MOV		A, B
	ADD		A, #30H
	MOV		R0, A	;1ST

/*	MOV		DPTR, #uc_fs_cr_bgn_cluster1
	MOVX	A, @DPTR		
	MOV		R3, A	;B1
	MOV		DPTR, #uc_fs_cr_bgn_cluster2
	MOVX	A, @DPTR		
	MOV		R6, A	;B2
	MOV		DPTR, #uc_fs_cr_bgn_cluster3
	MOVX	A, @DPTR		
	MOV		R7, A	;B3*/
	
	MOV		DPTR, #ui_fs_file_type + 1
	MOVX	A, @DPTR
	JNB		ACC.3, not_wav_name
	
	MOV		DPL, R4
	MOV		DPH, R5
	
	MOV		A, #49H	;"I"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #4DH	;"M"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #52H	;"R"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #43H	;"C"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, R2														
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #30H	;"0"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, R0
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, R1
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #41H	;"A"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #57H	;"W"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #20H	;
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #56H	;"V"
	MOVX	@DPTR, A
	JMP		write_file_date

not_wav_name:
	MOV		DPL, R4
	MOV		DPH, R5
	
	MOV		A, #45H	;"E"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #54H	;"T"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #5FH	;"_"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #4CH	;"L"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #41H	;"A"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #42H	;"B"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #20H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #4BH	;"K"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #49H	;"I"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #42H	;"B"
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #20H	;
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #4EH	;"N"
	MOVX	@DPTR, A
	
write_file_date:
	;3.write date time;4.write begin cluster(save_c3, save_c2, save_c1)
	INC		DPTR
	MOV		A, #49H	;
	MOVX	@DPTR, A
	INC		DPTR
	CLR		A
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #5DH
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #0E7H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #35H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #3BH
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #35H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #3BH
	MOVX	@DPTR, A

	JB		b_fs_fat_type_flg, wr_entry_dt_fat32
	INC		DPTR
	CLR		A
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	JMP		wr_entry_dt_2

wr_entry_dt_fat32:				;begin cluster is 0
	INC		DPTR
	CLR		A
	MOVX	@DPTR, A	
	INC		DPTR
	;MOV		A, R7
	MOVX	@DPTR, A

wr_entry_dt_2:
	INC		DPTR
	MOV		A, #60H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #3AH
	MOVX	@DPTR, A		
	INC		DPTR
	MOV		A, #35H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #3BH
	MOVX	@DPTR, A

	INC		DPTR				;begin cluster is 0
	;MOV		A, R6
	CLR		A
	MOVX	@DPTR, A
	INC		DPTR
	;MOV		A, R3
	MOVX	@DPTR, A
	
	;5.write file length
	INC		DPTR
	MOV		A, #00H
	MOVX	@DPTR, A
	INC		DPTR
	CLR		A
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A

	MOV		DPTR, #uc_fs_sav_entry_sect1
	MOV		A, sfc_logblkaddr0
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_sav_entry_sect2
	MOV		A, sfc_logblkaddr1
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_sav_entry_sect3
	MOV		A, sfc_logblkaddr2
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_sav_entry_sect4
	MOV		A, sfc_logblkaddr3
	MOVX	@DPTR, A

	SETB	b_sfc_immediate_wr_flg
	CALL	_write_sector
	SETB	C
	RET
#endif



#ifdef FS_READ_SECTOR_TO_POINT
/*******************************************************************************
********************************************************************************/
_fs_read_sector_2_point:
;;	MOV		sfc_logblkaddr3, #00H
	MOV		MR_WDT, #0FFH

	MOV		DPTR, #uc_fs_assign_buf_flg
	MOVX	A, @DPTR
	MOV		sfc_occ_buf_flag, A
	MOV		buf_offset0, #00H
	MOV		buf_offset1, #00H
#ifdef SP_SD_DRIVER
	#ifndef FS_NOT_SD_DEF
	JB		b_sd_flash_flg, sd_read_1		;0: read ef one sector; 1: read sd/mmc one sector
	JMP		_media_readsector
sd_read_1:
	CALL	_read_msi_sector
	JC		sd_read_ok
	SETB	b_fs_file_err_flg
sd_read_ok:
	RET
	#else
	JMP		_media_readsector
	#endif
#else
	JMP		_media_readsector
#endif
#endif


#ifdef CAL_ADDRESS
/*******************************************************************************
********************************************************************************/
_cal_sector_address_1:
	MOV		R0, #uc_fs_cur_sec_in_cluster
	MOV		A, @R0

_cal_sector_address:
	DEC		A
	ADD		A, sfc_logblkaddr0
	MOV		sfc_logblkaddr0, A
	
	CLR		A
	ADDC	A, sfc_logblkaddr1
	MOV		sfc_logblkaddr1, A

	CLR		A
	ADDC	A, sfc_logblkaddr2
	MOV		sfc_logblkaddr2, A
	
	CLR		A
	ADDC	A, sfc_logblkaddr3
	MOV		sfc_logblkaddr3, A
	RET
#endif									  


#ifdef FS_SD_MULTI_READ
EXTRN CODE (_read_msi_multisector_b)
EXTRN CODE (_read_msi_multisector_e)

PUBLIC	_fs_sd_multi_read

_fs_sd_multi_read:
	MOV		DPTR, #uc_msi_block_num
	MOV		A, #8
	MOVX	@DPTR, A

	CALL	_read_msi_multisector_b 	
	MOV		sfc_errcode, #01H
	JMP		_read_msi_multisector_e
#endif



#ifdef	FS_READ_SECTOR
PUBLIC	_fs_read_sector

/*******************************************************************************
********************************************************************************/
_fs_read_sector:
;;	MOV  	sfc_logblkaddr3, #00H
	MOV   	MR_WDT, #0FFH
	MOV   	sfc_occ_buf_flag, #SFC_APP_FBUF			;ef occupy EBUF
	MOV   	buf_offset0, #LOW FS_FCOMBUF_OFFSET
	MOV   	buf_offset1, #HIGH FS_FCOMBUF_OFFSET
#ifdef SP_SD_DRIVER
	#ifndef FS_NOT_SD_DEF
	JB		b_sd_flash_flg, read_sd_2
	CALL  	_media_readsector_tmp
	JMP		read_ok
read_sd_2:
	CALL	_read_msi_sector
	#else
	CALL  	_media_readsector_tmp
	#endif
#else
	CALL  	_media_readsector_tmp
#endif
read_ok:
#ifdef FS_FWRITE_DEF
	MOV		R1, #HIGH FS_FCOMBUF
	MOV		R3, #HIGH FS_BATBUF_BK1
	CALL	_src_r1_to_des_r3
#endif
	RET


_media_readsector_tmp:
	CALL	_media_readsector
	MOV		A, sfc_errcode
	CJNE	A, #0FFH, addr_not_over				;=0xFF, Addr Overflow
	CALL	_clr_fcombuf
addr_not_over:
	RET

_clr_fcombuf:
	MOV		R7, #00H
	MOV		DPTR, #X_ADDR_FCOMBUF
	CLR		A
loop_clr:
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	INC		DPTR
	DJNZ	R7, loop_clr
	RET
#endif



#ifdef FS_WRITE_SECTOR
/******************************************************************************
input global:sfc_logblkaddr0, sfc_logblkaddr1, sfc_logblkaddr2
*******************************************************************************/
_write_sector:
;;	MOV		sfc_logblkaddr3, #00H
	MOV		MR_WDT, #0FFH
	MOV		buf_offset0, #LOW FS_FCOMBUF_OFFSET
	MOV		buf_offset1, #HIGH FS_FCOMBUF_OFFSET
	MOV		sfc_occ_buf_flag, #SFC_APP_FBUF			;ef occupy EBUF
	JMP		FS_Wr_Interface
#endif



#ifdef FS_WRITE_SECTOR_TO_POINT
/******************************************************************************
input global:sfc_logblkaddr0, sfc_logblkaddr1, sfc_logblkaddr2
*******************************************************************************/
_write_sector_2_point:
;;	MOV		sfc_logblkaddr3, #00H
	MOV		MR_WDT, #0FFH
	MOV		buf_offset0, #LOW REC_ENC_BUF_OFFSET_W
	MOV		buf_offset1, #HIGH REC_ENC_BUF_OFFSET_W
	MOV		DPTR, #uc_fs_assign_buf_flg
	MOVX	A, @DPTR
	MOV		sfc_occ_buf_flag, A
	JMP		FS_Wr_Interface
#endif



#ifdef FS_WRITE_INTERFACE
PUBLIC	FS_Wr_Interface
/*******************************************************************************
********************************************************************************/
FS_Wr_Interface:
	MOV		MR_WDT, #0FFH
#ifdef SP_SD_DRIVER
	#ifndef FS_NOT_SD_DEF
	JB		b_sd_flash_flg, write_sd_1
	JMP		_write_flash
write_sd_1:
	JMP		_write_msi_sector
	#else
	JMP		_write_flash
	#endif
#else
	JMP		_write_flash
#endif
#endif


#ifdef FS_WRITE_EF_INTERFACE
PUBLIC	FS_WrFlash_Interface
FS_WrFlash_Interface:
	MOV		MR_WDT, #0FFH
	JMP		_write_flash
#endif



#ifdef R_SH16_DEF
PUBLIC _r_shift
/*******************************************************************************
********************************************************************************/
_r_shift:
	MOV    A, R2
	JZ     r_shift_out

r_shift1:
	MOV    A, R1
	CLR    C
	RRC    A
	MOV    R1, A

	MOV    A, R0
	RRC    A
	MOV    R0, A
	DJNZ   R2, r_shift1
r_shift_out:
	RET
#endif



#ifdef	ADD_32_DEF
PUBLIC  _fs_add_32bit
/*******************************************************************************
********************************************************************************/
_fs_add_32bit:
	MOV   A, R0
	ADD   A, R4
	MOV   R0, A

	MOV   A, R1
	ADDC  A, R5
	MOV   R1, A
	
	MOV   A, R2
	ADDC  A, R6
	MOV   R2, A

	MOV   A, R3
	ADDC  A, R7
	MOV   R3, A
	RET
#endif



#ifdef	CLST_1_SECT_RD_DEF
/*******************************************************************************
fat16, fat32
after call this, DECBUF_ADDR_HI, DECBUF_ADDR_LO are assigned
Input: uc_fs_begin_cluster1, uc_fs_begin_cluster2, (uc_fs_begin_cluster3)
********************************************************************************/
_get_first_sector:
	MOV		uc_fs_cur_cluster1, uc_fs_begin_cluster1
	MOV		uc_fs_cur_cluster2, uc_fs_begin_cluster2

	JNB		b_fs_fat_type_flg, get_first_sector_goon
	MOV		uc_fs_cur_cluster3, uc_fs_begin_cluster3

get_first_sector_goon:
	MOV		R0, #uc_fs_cur_sec_in_cluster
	MOV		A, #01H
	MOV		@R0, A

	CALL	_get_cluster_fir_sector
	CALL	_fs_read_sector
	RET
#endif



#ifdef	PARSE_ITEM_DEF
/*******************************************************************************
Input:DPH, DPL(point to the entry begin address)
Output:DPH, DPL(point to the next entry begin address)
				 R7:FF,  blank item entry
							0,  deleted item or long file name
							2,  directory
							3,  file	
********************************************************************************/
_check_item_type:
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #0E5H, not_del_item
	;to be a delete item
	MOV		R7, #0
	JMP		adjust_pt1

not_del_item:
	CJNE	A, #00H, not_blank_entry
	;to be a blank item entry
	MOV		R7, #0FFH
adjust_pt1:
	MOV		A, DPL
	ADD		A, #31
	MOV		DPL, A
	MOV		A, DPH
	ADDC	A, #00H
	MOV		DPH, A
	JMP		check_ret

not_blank_entry:
	MOV		A, DPL
	ADD		A, #09H
	MOV		DPL, A
	MOVX	A, @DPTR
	CJNE	A, #0FH, not_long_fn
	;to be long file name
	MOV		R7, #0
	JMP		adjust_pt2

not_long_fn:
	JB		ACC.4, be_dir
	;to be file
 	MOV		R7, #3
	JMP		adjust_pt2

be_dir:
	MOV		R7, #2

adjust_pt2:
	MOV		A, DPL
	ADD		A, #22
	MOV		DPL, A
	MOV		A, DPH
	ADDC	A, #00H
	MOV		DPH, A	
check_ret:
	RET
#endif



#ifdef FILE_CHK_MODULE
PUBLIC  _chk_file_type
/*******************************************************************************
bit chk_if_be_mp3()
firstly, check extend name, mp3 or c9d(cdd only be checked in root)
mp3 file, return 1
otherwise return 0
such fuction must be invoked after invoking open_file, now dptr point to final byte of one 
file entry
********************************************************************************/
_chk_file_type:
	MOV		DPL, uc_fs_tmp_var1
	MOV		DPH, uc_fs_tmp_var3

_get_file_num_chk:
	CLR		C
	MOV		A, DPL
	SUBB	A, #23
	MOV		DPL, A
	MOV		A, DPH
	SUBB	A, #0
	MOV		DPH, A		;now dptr point to file extended name

	MOV		R6, DPH
	MOV		R7, DPL
	MOV		DPTR, #ui_fs_file_type + 1
	MOVX	A, @DPTR
	MOV		B, A
	JNB		B.0, type_mmv
type_mp3:
	MOV		DPH, R6
	MOV		DPL, R7
	MOVX	A, @DPTR
	CJNE	A, #4DH, type_mmv	;'M'
	DEC		DPL
	MOVX	A, @DPTR
	CJNE	A, #50H, type_mmv	;'P'
	INC		DPTR
	INC		DPTR
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #33H, type_mmv	;'3'
	MOV		A, #FS_OPENFILE_TYPE_MP3
	JMP		set_file_type
	/*MOV		DPTR, #uc_fs_openfile_type
	MOVX	@DPTR, A
	SETB	C
	RET*/
type_mmv:
	JNB		B.1, type_amv
	MOV		DPH, R6
	MOV		DPL, R7
	MOVX	A, @DPTR
	CJNE	A, #4DH, type_amv	;'M'
	DEC		DPL
	MOVX	A, @DPTR
	CJNE	A, #4DH, type_amv	;'M'
	INC		DPTR
	INC		DPTR
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #56H, type_amv	;'V'
	MOV		A, #FS_OPENFILE_TYPE_MMV
	JMP		set_file_type
	/*MOV		DPTR, #uc_fs_openfile_type
	MOVX	@DPTR, A
	SETB	C
	RET*/
type_amv:
	JNB		B.2, type_wav
	MOV		DPH, R6
	MOV		DPL, R7
	MOVX	A, @DPTR
	CJNE	A, #41H, type_wav	;'A'
	DEC		DPL
	MOVX	A, @DPTR
	CJNE	A, #4DH, type_wav	;'M'
	INC		DPTR
	INC		DPTR
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #56H, type_wav	;'V'
	MOV		A, #FS_OPENFILE_TYPE_AMV
	JMP		set_file_type
	/*MOV		DPTR, #uc_fs_openfile_type
	MOVX	@DPTR, A
	SETB	C
	RET*/
type_wav:
	JNB		B.3, type_bmp
	MOV		DPH, R6
	MOV		DPL, R7
	MOVX	A, @DPTR
	CJNE	A, #57H, type_bmp	;'W'
	DEC		DPL
	MOVX	A, @DPTR
	CJNE	A, #41H, type_bmp	;'A'
	INC		DPTR
	INC		DPTR
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #56H, type_bmp	;'V'
	MOV		A, #FS_OPENFILE_TYPE_WAV
	JMP		set_file_type
	/*MOV		DPTR, #uc_fs_openfile_type
	MOVX	@DPTR, A
	SETB	C
	RET*/
type_bmp:
	JNB		B.4, type_jpg
	MOV		DPH, R6
	MOV		DPL, R7
	MOVX	A, @DPTR
	CJNE	A, #42H, type_jpg	;'B'
	DEC		DPL
	MOVX	A, @DPTR
	CJNE	A, #4DH, type_jpg	;'M'
	INC		DPTR
	INC		DPTR
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #50H, type_jpg	;'P'
	MOV		A, #FS_OPENFILE_TYPE_BMP
	JMP		set_file_type
	/*MOV		DPTR, #uc_fs_openfile_type
	MOVX	@DPTR, A
	SETB	C
	RET*/
type_jpg:
	JNB		B.5, type_txt
	MOV		DPH, R6
	MOV		DPL, R7
	MOVX	A, @DPTR
	CJNE	A, #4AH, type_txt	;'J'
	DEC		DPL
	MOVX	A, @DPTR
	CJNE	A, #50H, type_txt	;'P'
	INC		DPTR
	INC		DPTR
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #47H, type_txt	;'G'
	MOV		A, #FS_OPENFILE_TYPE_JPG
	JMP		set_file_type
	/*MOV		DPTR, #uc_fs_openfile_type
	MOVX	@DPTR, A
	SETB	C
	RET*/
type_txt:
	JNB		B.6, type_lrc
	MOV		DPH, R6
	MOV		DPL, R7
	MOVX	A, @DPTR
	CJNE	A, #54H, type_lrc	;'T'
	DEC		DPL
	MOVX	A, @DPTR
	CJNE	A, #58H, type_lrc	;'X'
	INC		DPTR
	INC		DPTR
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #54H, type_lrc	;'T'
	MOV		A, #FS_OPENFILE_TYPE_TXT
	JMP		set_file_type
	/*MOV		DPTR, #uc_fs_openfile_type
	MOVX	@DPTR, A
	SETB	C
	RET*/
type_lrc:
	JNB		B.7, type_bin
	MOV		DPH, R6
	MOV		DPL, R7
	MOVX	A, @DPTR
	CJNE	A, #4cH, type_bin	;'L'
	DEC		DPL
	MOVX	A, @DPTR
	CJNE	A, #52H, type_bin	;'R'
	INC		DPTR
	INC		DPTR
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #43H, type_bin	;'C'
	MOV		A, #FS_OPENFILE_TYPE_LRC
	JMP		set_file_type
	/*MOV		DPTR, #uc_fs_openfile_type
	MOVX	@DPTR, A
	SETB	C
	RET*/
type_bin:
	MOV		DPTR, #ui_fs_file_type
	MOVX	A, @DPTR
	MOV		B, A
	JNB		B.0, type_avi
	MOV		DPH, R6
	MOV		DPL, R7
	MOVX	A, @DPTR
	CJNE	A, #42H, type_avi	;'B'
	DEC		DPL
	MOVX	A, @DPTR
	CJNE	A, #49H, type_avi	;'I'
	INC		DPTR
	INC		DPTR
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #4eH, type_avi	;'N'
	MOV		A, #FS_OPENFILE_TYPE_BIN
	JMP		set_file_type
	/*MOV		DPTR, #uc_fs_openfile_type
	MOVX	@DPTR, A
	SETB	C
	RET*/
type_avi:
	JNB		B.1, type_null
	MOV		DPH, R6
	MOV		DPL, R7
	MOVX	A, @DPTR
	CJNE	A, #41H, type_null	;'A'
	DEC		DPL
	MOVX	A, @DPTR
	CJNE	A, #56H, type_null	;'V'
	INC		DPTR
	INC		DPTR
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #49H, type_null	;'I'
	MOV		A, #FS_OPENFILE_TYPE_AVI
set_file_type:
	MOV		DPTR, #uc_fs_openfile_type
	MOVX	@DPTR, A
	SETB	C
	RET
type_null:
	CLR		C
	RET
#endif



#ifdef NXT_SECTOR_DEF
PUBLIC  _seek_next_sector
/*******************************************************************************
_seek_next_sector for c invoke
Input global var: uc_fs_cur_sec_in_cluster, uc_fs_cur_cluster1, uc_fs_cur_cluster2
Output global var:uc_fs_cur_sec_in_cluster, uc_fs_cur_cluster1, uc_fs_cur_cluster2

b_fs_next_sector_flg: used to identify	seek_next_sector and get_next_sector
		get_next_sector:read sector
		seek_next_sector:only change parametar, no read actual sector
********************************************************************************/
_seek_next_sector:
	SETB	b_fs_next_sector_flg

_get_next_sector:
	CLR		b_fs_file_end_flg

	MOV		DPTR, #uc_fs_sec_per_cluster
	MOVX	A, @DPTR
	MOV		B, A
	MOV		R0, #uc_fs_cur_sec_in_cluster
	MOV		A, @R0
	CJNE	A, B, not_seek
	JMP		seek_nxt_cluster

not_seek:
	JB		b_fs_next_sector_flg, skip_read1
	CALL	_inc_sector_addr
	CALL	_fs_read_sector
skip_read1:
	MOV		R0, #uc_fs_cur_sec_in_cluster
	MOV		A, @R0
	INC		A
	MOV		@R0, A
	RET

seek_nxt_cluster:
	JNB		b_fs_fat_type_flg, seek_nxt_cluster_fat16
	JMP		seek_nxt_cluster_fat32
	
seek_nxt_cluster_fat16:
	;uc_fs_cur_cluster1 is the offset in one sector
	;uc_fs_cur_cluster2 is the sector number in fat
	CALL	_cluster_cal_fat_addr_fat16

	JB		b_fs_next_sector_flg, seek_nxt_sector_dptr_fat16
	CALL	_fs_read_sector		

	CALL	_adjust_fat_dptr_addr_fat16
	JMP		goon_process_fat16

seek_nxt_sector_dptr_fat16:
	CALL	_fs_read_sector
	MOV		A, uc_fs_cur_cluster1
	MOV		B, #2
	MUL		AB
	MOV		DPL, A
	MOV		A, #HIGH FS_FCOMBUF			;DECBUF_ADDR_HI	
	ADD		A, B
	MOV		DPH, A

goon_process_fat16:
	CALL	_get_nxt_cluster_sub_fat16
	JNB		b_fs_file_end_flg, seek_nxt_cluster_end
	RET

seek_nxt_cluster_fat32:
	CALL	_cluster_cal_fat_addr_fat32

	JB		b_fs_next_sector_flg, seek_nxt_sector_dptr_fat32
	CALL	_fs_read_sector		

	CALL	_adjust_fat_dptr_addr_fat32
	JMP		goon_process_fat32

seek_nxt_sector_dptr_fat32:
	CALL	_fs_read_sector

	MOV		DPL, uc_fs_offset_in_sec1
	MOV		A, #HIGH FS_FCOMBUF			;DECBUF_ADDR_HI	
	ADD		A, uc_fs_offset_in_sec2
	MOV		DPH, A


goon_process_fat32:
	CALL	_get_nxt_cluster_sub_fat32
	JNB		b_fs_file_end_flg, seek_nxt_cluster_end
	RET

seek_nxt_cluster_end:
	MOV		R0, #uc_fs_cur_sec_in_cluster
	MOV		A, #01H
	MOV		@R0, A

	JB		b_fs_next_sector_flg, skip_read2
	CALL	_get_cluster_fir_sector
	CALL	_fs_read_sector
skip_read2:
	RET
#endif



#ifdef INC_ADDR_DEF
/*******************************************************************************
********************************************************************************/
_inc_sector_addr:
	MOV		A, sfc_logblkaddr0
	ADD		A, #1
	MOV		sfc_logblkaddr0, A

	MOV		A, sfc_logblkaddr1
	ADDC	A, #0
	MOV		sfc_logblkaddr1, A

	MOV		A, sfc_logblkaddr2
	ADDC	A, #0
	MOV		sfc_logblkaddr2, A
	
	MOV		A, sfc_logblkaddr3
	ADDC	A, #0
	MOV		sfc_logblkaddr3, A
	RET
#endif



#ifdef CLST_1_SECT_CAL_DEF
/*******************************************************************************
fat16, fat32
Input:	uc_fs_cur_cluster1, uc_fs_cur_cluster2, (uc_fs_cur_cluster3)
Output:	sfc_logblkaddr0, sfc_logblkaddr1
	sfc_logblkaddr2, SECTOR_ADDR_HI2
********************************************************************************/
_get_cluster_fir_sector:
	MOV		A, uc_fs_cur_cluster1
	CLR		C
	SUBB	A, #2
	MOV		R0, A		;R0, low
	MOV		A, uc_fs_cur_cluster2
	SUBB	A, #0
	MOV		R1, A		;R1, high

	JNB		b_fs_fat_type_flg, fat16_get_fir_sector1
	MOV		A, uc_fs_cur_cluster3
	SUBB	A, #0
	MOV		R2, A		;R2, highest

fat16_get_fir_sector1:
	MOV		DPTR, #uc_fs_sec_per_cluster
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, R0
	MUL		AB
	MOV		R4, A		;sfc_logblkaddr0
	MOV		R0, B		;R0 save B
	MOV		DPTR, #uc_fs_sec_per_cluster
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, R1
	MUL		AB
	ADD		A, R0
	MOV		R5, A		;sfc_logblkaddr1

	JNB		b_fs_fat_type_flg, fat16_get_fir_sector2

	;FAT32
	MOV		R0, B
	MOV		DPTR, #uc_fs_sec_per_cluster
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, R2
	MUL		AB
	ADD		A, R0
	MOV		R6, A		;sfc_logblkaddr2

	MOV		R7, B		;sfc_logblkaddr3
	JMP		cal_sector_addr_rid_bug
	
fat16_get_fir_sector2:
 	MOV		A, B
	ADDC	A, #0
	MOV		R6, A		;sfc_logblkaddr2
	CLR		A
	ADDC	A, #0
	MOV		R7, A		;sfc_logblkaddr3

cal_sector_addr_rid_bug:
	MOV		DPTR, #uc_fs_dat_begin_1
	MOVX	A, @DPTR
	MOV		R0, A
	MOV		DPTR, #uc_fs_dat_begin_2
	MOVX	A, @DPTR
	MOV		R1, A	
	MOV		R2, #0
	MOV		R3, #0
	CALL	_fs_add_32bit

	MOV		sfc_logblkaddr0, R0
	MOV		sfc_logblkaddr1, R1
	MOV		sfc_logblkaddr2, R2
	MOV		sfc_logblkaddr3, R3
	RET
#endif



#ifdef PARSE_CLST_2FAT_DEF
/*******************************************************************************
********************************************************************************/
_cluster_cal_fat_addr_fat16:
	;uc_fs_cur_cluster1 is the offset in one sector
	;uc_fs_cur_cluster2 is the sector number in fat
	MOV		DPTR, #uc_fs_fat1_begin_1
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, uc_fs_cur_cluster2
	ADD		A, B
	MOV		sfc_logblkaddr0, A
	MOV		DPTR, #uc_fs_fat1_begin_2
	MOVX	A, @DPTR
	ADDC	A, #0
	MOV		sfc_logblkaddr1, A
	MOV		sfc_logblkaddr2, #0
	MOV		sfc_logblkaddr3, #0
	RET	


_adjust_fat_dptr_addr_fat16:
	MOV		A, uc_fs_cur_cluster1
	MOV		B, #2
	MUL		AB

	MOV		DPL, A
#ifdef UI_MOVIE_MAIN_DEF
	MOV		A, #HIGH FS_FATBUF			;DECBUF_ADDR_HI	
#else
	MOV		A, #HIGH FS_FCOMBUF			;DECBUF_ADDR_HI	
#endif
	ADD		A, B
	MOV		DPH, A
	RET


_get_nxt_cluster_sub_fat16:
	MOVX	A, @DPTR			;get next cluster, store in R0, R1
	MOV		R1, A			;R1 hi
	INC		DPTR
	MOVX	A, @DPTR
	MOV		R0, A			;R0 lo
	
	MOV		A, R0
	CJNE	A, #00H, get_nxt_clu1
	MOV		A, R1
	CJNE	A, #00H, get_cluster_ok
	SETB	b_fs_file_err_flg
	JMP		get_no_nxt_cluster

get_nxt_clu1:
	ANL		A, #0F8H
	CJNE	A, #0F8H, get_cluster_ok
	MOV		A, R1
	CJNE	A, #0FFH, get_cluster_ok
	JMP		get_no_nxt_cluster

get_cluster_ok:
 	MOV		uc_fs_cur_cluster1, R0
	MOV		uc_fs_cur_cluster2, R1

	CLR		b_fs_file_end_flg
	RET
	
get_no_nxt_cluster:
	SETB	b_fs_file_end_flg	
	RET


;get fat position(fat addr, and sector offset)according current cluster
_cluster_cal_fat_addr_fat32:
	MOV		R0, uc_fs_cur_cluster1
	MOV		R1, uc_fs_cur_cluster2
	MOV		R2, uc_fs_cur_cluster3
	MOV		R3, #0
	MOV		R4, #7
	CALL	_r_shift_32bit

	MOV		DPTR, #uc_fs_fat1_begin_1
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, R0
	ADD		A, B
	MOV		sfc_logblkaddr0, A
	MOV		DPTR, #uc_fs_fat1_begin_2
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, R1
	ADDC	A, B
	MOV		sfc_logblkaddr1, A
	MOV		sfc_logblkaddr2, #0
	MOV		sfc_logblkaddr3, #0

	MOV		A, uc_fs_cur_cluster1
	ANL		A, #01111111B
	MOV		B, #4
	MUL		AB
	MOV		uc_fs_offset_in_sec1, A
	MOV		uc_fs_offset_in_sec2, B
	RET



_adjust_fat_dptr_addr_fat32:
	MOV		DPL, uc_fs_offset_in_sec1
#ifdef UI_MOVIE_MAIN_DEF
	MOV		A, #HIGH FS_FATBUF			;DECBUF_ADDR_HI	
#else
	MOV		A, #HIGH FS_FCOMBUF			;DECBUF_ADDR_HI	
#endif
	ADD		A, uc_fs_offset_in_sec2
	MOV		DPH, A
	RET


_get_nxt_cluster_sub_fat32:
	MOVX	A, @DPTR		;get next cluster, store in R0, R1, R2, R3
	MOV		R1, A			;r3 highest, r0, lowest
	INC		DPTR
	MOVX	A, @DPTR
	MOV		R0, A
	INC		DPTR
	MOVX	A, @DPTR	
	MOV		R3, A			
	INC		DPTR
	MOVX	A, @DPTR
	MOV		R2, A

	MOV		A, R0
	CJNE	A, #00H, fat32_lo1
	MOV		A, R1
	CJNE	A, #00H, fat32_lo1
	MOV		A, R2
	CJNE	A, #00H, fat32_lo1
	MOV		A, R3
	CJNE	A, #00H, fat32_lo1
	JMP		no_nxt_cluster

fat32_lo1:
	MOV		A, R0
	ANL		A, #0F8H
	CJNE	A, #0F8H, fat32_norm_cluster
	MOV		A, R1
	CJNE	A, #0FFH, fat32_norm_cluster
	MOV		A, R2
	CJNE	A, #0FFH, fat32_norm_cluster
	MOV		A, R3
	ANL		A, #0FH			;mask high 4bit
	CJNE	A, #0FH, fat32_norm_cluster
	JMP		no_nxt_cluster

fat32_norm_cluster:
 	MOV		uc_fs_cur_cluster1, R0
	MOV		uc_fs_cur_cluster2, R1
	MOV		uc_fs_cur_cluster3, R2
	CLR		b_fs_file_end_flg
	RET

no_nxt_cluster:
	SETB	b_fs_file_end_flg	
	RET
#endif



#ifdef R_SH32_DEF
PUBLIC _r_shift_32bit
/*******************************************************************************
Function:logic 32 bit right shift n bit
Input:	 R0, R1, R2, R3, R4
	 R0:low;	R3:high;	R4:shift bit number	
Output:	 R0, R1, R2, R3	
********************************************************************************/
_r_shift_32bit:
	MOV		A, R4
	JZ		r_shift_32bit_out

r_shift_32bit_sub:
	MOV		A, R3
	CLR		C
	RRC		A
	MOV		R3, A

	MOV		A, R2
	RRC		A
	MOV		R2, A

	MOV		A, R1
	RRC		A
	MOV		R1, A

	MOV		A, R0
	RRC		A
	MOV		R0, A
	DJNZ	R4, r_shift_32bit_sub
r_shift_32bit_out:
	RET

#endif



#ifdef NXT_CLST_DEF
/*******************************************************************************
void get_next_cluster(void)
Input global var: uc_fs_cur_cluster1, uc_fs_cur_cluster2
			uc_fs_tmp_var4, uc_fs_tmp_var5 save where cluster number of last time located in FAT(sector address)
Output global var:uc_fs_cur_cluster1, uc_fs_cur_cluster2, b_fs_file_end_flg
********************************************************************************/
_get_next_cluster:
#ifdef UI_MOVIE_MAIN_DEF
	MOV		DPTR, #uc_fs_sector1_bk		;zhanghuan modify 2008-07-03
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var5, A
	MOV		DPTR, #uc_fs_sector0_bk
	MOVX	A, @DPTR
	MOV		uc_fs_tmp_var4, A
#endif

	JNB		b_fs_fat_type_flg, fat16_get_nxt
	JMP		fat32_get_nxt
fat16_get_nxt:
	;uc_fs_cur_cluster1 is the offset in one sector
	;uc_fs_cur_cluster2 is the sector number in fat
	MOV		DPTR, #uc_fs_fat1_begin_1
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, uc_fs_cur_cluster2
	ADD		A, B
	MOV		sfc_logblkaddr0, A
	MOV		R6, A			;R6 save
	MOV		DPTR, #uc_fs_fat1_begin_2
	MOVX	A, @DPTR
	ADDC	A, #0

cmp_read:
	MOV		sfc_logblkaddr1, A
	MOV		R7, A
	MOV		A, R6
	CJNE	A, uc_fs_tmp_var4, fat_update_tmp_var4
	MOV		A, R7
	CJNE	A, uc_fs_tmp_var5, fat_update_tmp_var5
	JNB		b_fs_fat_type_flg, fat16_get_cluster_no_read
	JMP		fat32_get_cluster_no_read

fat_update_tmp_var4:
	MOV		uc_fs_tmp_var4, R6

fat_update_tmp_var5:
	MOV		uc_fs_tmp_var5, R7
	MOV		sfc_logblkaddr2, #0
	MOV		sfc_logblkaddr3, #0

#ifdef UI_MOVIE_MAIN_DEF
	MOV		DPTR, #uc_fs_sector1_bk		;zhanghuan modify 2008-07-03
	MOV		A, uc_fs_tmp_var5
	MOVX	@DPTR, A
	MOV		DPTR, #uc_fs_sector0_bk
	MOV		A, uc_fs_tmp_var4
	MOVX	@DPTR, A
	CALL	_fs_read_sector
	MOV		R1, #HIGH FS_FCOMBUF
	MOV		R3, #HIGH FS_FATBUF
	CALL	_src_r1_to_des_r3
#else
	CALL	_fs_read_sector
#endif
	JNB		b_fs_fat_type_flg, fat16_get_cluster_no_read
	JMP		fat32_get_cluster_no_read
fat16_get_cluster_no_read:

#ifdef UI_MOVIE_MAIN_DEF
	JB		b_fs_pre_flg, srh_pre_cluster_fat16
#endif

	CALL	_adjust_fat_dptr_addr_fat16
	CALL	_get_nxt_cluster_sub_fat16
	RET

#ifdef UI_MOVIE_MAIN_DEF
srh_pre_cluster_fat16:				;zhanghuan modify 2008-07-03
	MOV		DPTR, #FS_FATBUF
	MOV		A, uc_fs_cur_cluster2
	MOV		R1, #0	
	MOV		R0, A
	MOV		R2, #8
	CALL	_l_shift
	MOV		uc_fs_tmp_var7, R1
	MOV		uc_fs_tmp_var6, R0
goon_seek_match_cluster_fat16:
	MOV		A, DPH
	CLR		C
	SUBB	A, #HIGH FS_FATBUF
	MOV		R1, A
	MOV		R0, DPL
	MOV		R2, #1
	CALL	_r_shift

	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster2, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster1, A
	INC		DPTR

	MOV		A, DPH
	CJNE	A, #(HIGH FS_FATBUF)+2H, goon_cmp_match_cluster_fat16
	MOV		A, DPL
	CJNE	A, #0, goon_cmp_match_cluster_fat16
	CLR		b_fs_pre_flg
	RET

goon_cmp_match_cluster_fat16:
	MOV		A, uc_fs_tmp_var1
	CJNE	A, uc_fs_cur_cluster1, goon_seek_match_cluster_fat16
	MOV		A, uc_fs_tmp_var2
	CJNE	A, uc_fs_cur_cluster2, goon_seek_match_cluster_fat16

	MOV		A, R0
	CLR		C
	ADD		A, uc_fs_tmp_var6
	MOV		uc_fs_cur_cluster1, A	
	MOV		A, R1
	ADDC	A, uc_fs_tmp_var7	
	MOV		uc_fs_cur_cluster2, A
	RET
#endif

fat32_get_nxt:
	MOV		R0, uc_fs_cur_cluster1
	MOV		R1, uc_fs_cur_cluster2
	MOV		R2, uc_fs_cur_cluster3
	MOV		R3, #0
	MOV		R4, #7
	CALL	_r_shift_32bit

	MOV		DPTR, #uc_fs_fat1_begin_1
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, R0
	ADD		A, B
	MOV		sfc_logblkaddr0, A
	MOV		R6, A

	MOV		DPTR, #uc_fs_fat1_begin_2
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, R1
	ADDC	A, B
	JMP		cmp_read
fat32_get_cluster_no_read:

#ifdef UI_MOVIE_MAIN_DEF
	JB		b_fs_pre_flg, srh_pre_cluster_fat32
#endif

	MOV		A, uc_fs_cur_cluster1
	ANL		A, #01111111B
	MOV		B, #4
	MUL		AB
	MOV		uc_fs_offset_in_sec1, A
	MOV		uc_fs_offset_in_sec2, B

	CALL	_adjust_fat_dptr_addr_fat32
	CALL	_get_nxt_cluster_sub_fat32
	RET

#ifdef UI_MOVIE_MAIN_DEF
srh_pre_cluster_fat32:				;zhanghuan modify 2008-07-03
	MOV		R0, uc_fs_cur_cluster1
	MOV		R1, uc_fs_cur_cluster2
	MOV		R2, uc_fs_cur_cluster3
	MOV		R3, #0
	MOV		R4, #7
	CALL	_r_shift_32bit
	MOV		R4, #7
	CALL	_l_shift_32bit
	MOV		uc_fs_tmp_var7, R1
	MOV		uc_fs_tmp_var6, R0
	MOV		uc_fs_tmp_var5, R2
	MOV		DPTR, #FS_FATBUF
goon_seek_match_cluster_fat32:
	MOV		A, DPH
	CLR		C
	SUBB	A, #HIGH FS_FATBUF
	MOV		R1, A
	MOV		R0, DPL
	MOV		R2, #2
	CALL	_r_shift

	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster2, A
	INC		DPTR
	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster1, A
	INC		DPTR
	INC		DPTR
	MOVX	A, @DPTR
	MOV		uc_fs_cur_cluster3, A
	INC		DPTR

	MOV		A, DPH
	CJNE	A, #(HIGH FS_FATBUF)+2H, goon_cmp_match_cluster_fat32
	MOV		A, DPL
	CJNE	A, #0, goon_cmp_match_cluster_fat32
	CLR		b_fs_pre_flg
	RET

goon_cmp_match_cluster_fat32:
	MOV		A, uc_fs_tmp_var1
	CJNE	A, uc_fs_cur_cluster1, goon_seek_match_cluster_fat32
	MOV		A, uc_fs_tmp_var2
	CJNE	A, uc_fs_cur_cluster2, goon_seek_match_cluster_fat32
	MOV		A, uc_fs_tmp_var3
	CJNE	A, uc_fs_cur_cluster3, goon_seek_match_cluster_fat32

	MOV		A, R0
	CLR		C
	ADD		A, uc_fs_tmp_var6
	MOV		uc_fs_cur_cluster1, A	
	MOV		A, R1
	ADDC	A, uc_fs_tmp_var7	
	MOV		uc_fs_cur_cluster2, A
	MOV		uc_fs_cur_cluster3, uc_fs_tmp_var5
	RET
#endif

#endif



#ifdef CLST_EXP_DEF
PUBLIC	_sec_to_clu_exponent
/*******************************************************************************
Function:if uc_fs_sec_per_cluster = 8,  becasuse 8 = 2^3,  so R7 = 3 
Output:	R7
********************************************************************************/
_sec_to_clu_exponent:
	MOV		R7, #0
	MOV		DPTR, #uc_fs_sec_per_cluster
	MOVX	A, @DPTR
goon_rs:
	CLR		C
	RRC		A
	JZ		out_rs
	INC		R7
	JMP		goon_rs
out_rs:
	RET

#endif



#ifdef DEC_ADDR_DEF
/*******************************************************************************
fat16, fat32
********************************************************************************/
_dec_sector_addr:
	CLR		C
	MOV		A, sfc_logblkaddr0
	SUBB	A, #1
	MOV		sfc_logblkaddr0, A

	MOV		A, sfc_logblkaddr1
	SUBB	A, #0
	MOV		sfc_logblkaddr1, A

	MOV		A, sfc_logblkaddr2
	SUBB	A, #0
	MOV		sfc_logblkaddr2, A
	
	MOV		A, sfc_logblkaddr3
	SUBB	A, #0
	MOV		sfc_logblkaddr3, A
	RET
#endif



#ifdef	SUB_32_DEF
PUBLIC _sub_32bit
/*******************************************************************************
Function:32bit subtract subroutine
Input:	 R0, R1, R2, R3, R4, R5, R6, R7
	 From low to high:r0->r3, r4->r7	
	 R3R2R1R0-R7R6R5R4
Output:	 R0, R1, R2, R3	
********************************************************************************/
_sub_32bit:
	CLR		C
	MOV		A, R0	
	SUBB	A, R4
	MOV		R0, A

	MOV		A, R1	
	SUBB	A, R5
	MOV		R1, A	
	
	MOV		A, R2	
	SUBB	A, R6
	MOV		R2, A

	MOV		A, R3	
	SUBB	A, R7
	MOV		R3, A
	RET
#endif



#ifdef SRH_FREE_IN_FAT_DEF
/*******************************************************************************
input:DPTR(decbuf or fatbuf begin addr)
output:
	C:1, get blank, DPTR(got blank cluster address in fat)
	C:0, not get in one sector
********************************************************************************/
_srh_blank_clu_one_sector:
	JB		b_fs_fat_type_flg, fat32_seek_free_pos
	;fat16
	MOV		uc_fs_tmp_var1, #0FFH
fat16_seek_free:	
	MOVX	A, @DPTR
	JZ		fat16_first_00
	INC		DPTR
	JMP		fat16_fat_count_256

fat16_first_00:
	INC		DPTR
	MOVX	A, @DPTR
	JZ		get_blank_cluster

fat16_fat_count_256:
	INC		DPTR
	INC		uc_fs_tmp_var1
	MOV		A, uc_fs_tmp_var1
	CJNE	A, #0FFH, fat16_seek_free		;count 256 times	CLR		C
	CLR		C
	RET

fat32_seek_free_pos:
	MOV		uc_fs_tmp_var1, #00H
fat32_seek_free_2:
	MOVX	A, @DPTR
	JZ		fat32_first_00		;4
	INC		DPTR
	INC		DPTR
	INC		DPTR
	JMP		fat32_fat_count_128

fat32_first_00:
	INC		DPTR
	MOVX	A, @DPTR
	JZ		fat32_second_00		;3
	INC		DPTR
	INC		DPTR
	JMP		fat32_fat_count_128
	
fat32_second_00:
	INC		DPTR
	MOVX	A, @DPTR
	JZ		fat32_third_00		;2
	INC		DPTR
	JMP		fat32_fat_count_128

fat32_third_00:
	INC		DPTR
	MOVX	A, @DPTR			;1
	JZ		get_blank_cluster	
		
fat32_fat_count_128:
	INC		DPTR
	INC		uc_fs_tmp_var1
	MOV		A, uc_fs_tmp_var1
	CJNE	A, #80H, fat32_seek_free_2		;count 128 times
	CLR		C
	RET

get_blank_cluster:
	SETB	C
	RET
#endif



#ifdef COPY_512B_DEF
/*******************************************************************************
src_r1_to_des_r3:512 bytes copy
input:R1(source), R3(destination)

src_r1_to_des_r3_size_r6:
input:R1(source), R3(destination), R6(size, size = R6*16)
********************************************************************************/
_src_r1_to_des_r3:	
	;R0, R1->R2, R3
	MOV		R6, #32 

src_r1_to_des_r3_size_r6:
	MOV		R0, #00H
	MOV		R2, #00H
	MOV		R5, #16

copy_512b_loop_1:	
	MOV		DPH, R1
	MOV		DPL, R0

	MOVX	A, @DPTR 
	INC		DPTR
	MOV		R1, DPH
	MOV		R0, DPL

	MOV		DPH, R3
	MOV		DPL, R2
	MOVX	@DPTR, A
	INC		DPTR
	MOV		R3, DPH
	MOV		R2, DPL

	DJNZ	R5 , copy_512b_loop_1
	MOV		R5, #16
	DJNZ	R6 , copy_512b_loop_1
	RET
#endif



#ifdef SRH_BLANK_DEF
PUBLIC	__seek_free_cluster
/*******************************************************************************
bit seek_free_cluster(uc8 fat_addr_lo, uc8 fat_addr_hi);
Description:seek blank cluster from fat address [fat_addr_hi, fat_addr_lo]
return:1, ok;0, fail
output:uc_fs_cur_cluster1, uc_fs_cur_cluster2, uc_fs_cur_cluster3

uc_fs_tmp_var1, uc_fs_tmp_var4, uc_fs_tmp_var5, uc_fs_tmp_var6
********************************************************************************/
__seek_free_cluster:
	MOV		sfc_logblkaddr0, R7
	MOV		sfc_logblkaddr1, R5
	MOV		sfc_logblkaddr2, #0
	MOV		sfc_logblkaddr3, #0

seek_in_fat_sector:	
	CALL	_fs_read_sector
#ifdef FS_FWRITE_DEF
	MOV		DPTR, #FS_BATBUF_BK1
#else
	MOV		DPTR, #FS_FCOMBUF
#endif

	CALL	_srh_blank_clu_one_sector
	JNC		goon_srh_in_fat
	;have got blank cluser
	JNB		b_fs_fat_type_flg, fat16_get_free_cluster
	JMP		fat32_get_free_cluster

goon_srh_in_fat:
	CALL	_inc_sector_addr
	CALL	_chk_if_exceed_fat1
	JC		seek_in_fat_sector
	JMP		card_is_full


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;get blank cluser
fat16_get_free_cluster:
	MOV		R0, sfc_logblkaddr0
	MOV		R1, sfc_logblkaddr1
	CALL	_fat_pos_2_cluster_16

seek_blank_clust_ret:
	CALL	_check_capacity
	JNC		get_free_cluster_ok

card_is_full:
	CLR		C
	RET

get_free_cluster_ok:
	MOV		uc_fs_cur_cluster1, uc_fs_tmp_var4
	MOV		uc_fs_cur_cluster2, uc_fs_tmp_var5
	MOV		uc_fs_cur_cluster3, uc_fs_tmp_var6

	JB		b_sd_flash_flg, get_sd_free_fat_sector0
	MOV		DPTR, #uc_fs_fat_srh_addr1
	JMP		get_fat_sector0
get_sd_free_fat_sector0:	
	MOV		DPTR, #uc_fs_sd_fat_srh_addr1
get_fat_sector0:	
	MOV		A, sfc_logblkaddr0
	MOVX	@DPTR, A
	
	JB		b_sd_flash_flg, get_sd_free_fat_sector1
	MOV		DPTR, #uc_fs_fat_srh_addr2
	JMP		get_fat_sector1
get_sd_free_fat_sector1:	
	MOV		DPTR, #uc_fs_sd_fat_srh_addr2
get_fat_sector1:	
	MOV		A, sfc_logblkaddr1
	MOVX	@DPTR, A

	SETB	C
	RET

fat32_get_free_cluster:
	MOV		R0, sfc_logblkaddr0
	MOV		R1, sfc_logblkaddr1
	CALL	_fat_pos_2_cluster_32
	JMP		seek_blank_clust_ret
#endif



#ifdef WR_SECTOR_SRC_FATBUF
/*******************************************************************************
********************************************************************************/
_write_sector_fatbuf:
	MOV		MR_WDT, #0FFH
	MOV		R6, #HIGH X_ADDR_FS_FATBUF			;7200H
	MOV		R7, #LOW X_ADDR_FS_FATBUF
	MOV		R4, #HIGH FS_FCOMBUF				;6600H
	MOV		R5, #LOW FS_FCOMBUF
	MOV		R0, #4								;512Bytes
convey_fatbuf:	
	MOV		R1, #128
continue_convey_fatbuf:	
	MOV		DPH, R6
	MOV		DPL, R7
	MOVX	A, @DPTR
	INC		DPTR
	MOV		R6, DPH
	MOV		R7, DPL

	MOV		DPH, R4
	MOV		DPL, R5
	MOVX	@DPTR, A
	INC		DPTR
	MOV		R4, DPH
	MOV		R5, DPL
	DJNZ	R1, continue_convey_fatbuf
	DJNZ	R0, convey_fatbuf
					
	MOV		sfc_occ_buf_flag, #SFC_APP_FBUF		;Fbuf
	MOV		buf_offset0, #LOW FS_FCOMBUF_OFFSET
	MOV		buf_offset1, #HIGH FS_FCOMBUF_OFFSET
	SETB	b_sfc_immediate_wr_flg
	JMP		fs_wr_interface

_write_sector_from_fatbuf:
	;first copy to decbuf1, tmp
	MOV		DPTR, #uc_fs_old_fat_addr1
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A
	MOV		DPTR, #uc_fs_old_fat_addr2
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	MOV		sfc_logblkaddr2, #00H
	MOV		sfc_logblkaddr3, #00H

	JMP		_write_sector_fatbuf
#endif



#ifdef FAT_CAL_CLST_DEF
/*******************************************************************************
according to fat sector address, offset in one fat sector, calculate actual cluster
input:	R0(fat sector lo), R1(fat sector hi)
	DPTR(offset in one fat sector, point to cluster beg +1), 0x21xx
output:uc_fs_tmp_var4(cluster1), uc_fs_tmp_var5(cluster2)
********************************************************************************/
_fat_pos_2_cluster_16:
	MOV		R4, DPL
	MOV		R5, DPH

	CLR		C
	MOV		DPTR, #uc_fs_fat1_begin_1
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, R0
	SUBB	A, B
	MOV		R0, A
	MOV		DPTR, #uc_fs_fat1_begin_2
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, R1
	SUBB	A, B
	MOV		R1, A

	MOV		DPL, R4
	MOV		DPH, R5

	MOV		R2, #8
	CALL	_l_shift
	MOV		uc_fs_tmp_var4, R0
	MOV		uc_fs_tmp_var5, R1			;
	
	CLR		C
	MOV		A, DPL
	SUBB	A, #1
	MOV		DPL, A
	MOV		A, DPH
	SUBB	A, #0
	MOV		DPH, A

	CLR		C
	MOV		R0, DPL
	MOV		A, DPH
#ifdef FS_FWRITE_DEF
	SUBB	A, #HIGH FS_BATBUF_BK1
#else
	SUBB	A, #HIGH FS_FCOMBUF
#endif
	MOV		R1, A
	MOV		R2, #1
	CALL	_r_shift
	
	MOV		A, R0
	ADD		A, uc_fs_tmp_var4
	MOV		R0, A
	MOV		uc_fs_tmp_var4, A
	MOV		A, R1
	ADDC	A, uc_fs_tmp_var5
	MOV		R1, A			;R0, R1, GET the free cluster
	MOV		uc_fs_tmp_var5, A		;uc_fs_tmp_var4, uc_fs_tmp_var5 save	
	RET



/*******************************************************************************
according to fat sector address, offset in one fat sector, calculate actual cluster
input:	R0(fat sector lo), R1(fat sector hi)
	DPTR(offset in one fat sector, point to cluster beg +3), 0x21xx
output:uc_fs_tmp_var4(cluster1), uc_fs_tmp_var5(cluster2), uc_fs_tmp_var6(cluster3)
********************************************************************************/
_fat_pos_2_cluster_32:
	MOV		R4, DPL
	MOV		R5, DPH

	CLR		C
	MOV		DPTR, #uc_fs_fat1_begin_1
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, R0
	SUBB	A, B
	MOV		R0, A
	MOV		DPTR, #uc_fs_fat1_begin_2
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, R1
	SUBB	A, B
	MOV		R1, A

	MOV		DPL, R4
	MOV		DPH, R5

	MOV		R2, #00H
	MOV		R3, #00H
	MOV		R4, #7
	CALL	_l_shift_32bit
	MOV		uc_fs_tmp_var4, R0
	MOV		uc_fs_tmp_var5, R1
	MOV		uc_fs_tmp_var6, R2
	
	CLR		C
	MOV		A, DPL
	SUBB	A, #3
	MOV		DPL, A
	MOV		A, DPH
	SUBB	A, #0
	MOV		DPH, A

	CLR		C
	MOV		R0, DPL
	MOV		A, DPH
#ifdef FS_FWRITE_DEF
	SUBB	A, #HIGH FS_BATBUF_BK1
#else
	SUBB	A, #HIGH FS_FCOMBUF
#endif
	MOV		R1, A
	MOV		R2, #2
	CALL	_r_shift

	MOV		A, R0
	ADD		A, uc_fs_tmp_var4
	MOV		R0, A
	MOV		uc_fs_tmp_var4, A
	MOV		A, R1
	ADDC	A, uc_fs_tmp_var5
	MOV		R1, A			
	MOV		uc_fs_tmp_var5, A		
	MOV		A, uc_fs_tmp_var6
	ADDC	A, #00H
	MOV		R2, A			;R0, R1, GET the free cluster, uc_fs_tmp_var4, uc_fs_tmp_var5, uc_fs_tmp_var6 save
	MOV		uc_fs_tmp_var6, A
	RET
#endif



#ifdef CHK_CAPACITY_DEF
/*******************************************************************************
input :R0, R1, R2(FAT32), (CLUSTER)
(cluster-2)*sectornum_per_cluster+dat_begin
compare with:
MAX_NUM_OF_SECTOR-sectornum_per_cluster
output:C:1 NOT exceed		0:exceed
********************************************************************************/
_check_capacity:
	MOV		A, R0
	CLR		C
	SUBB	A, #2
	MOV		R0, A

	MOV		A, R1
	SUBB	A, #0
	MOV		R1, A

	JB		b_fs_fat_type_flg, check_capacity_1
	MOV		R2, #0		;(cluster-2)
	JMP		check_capacity_2

check_capacity_1:
	MOV		A, R2
	SUBB	A, #0
	MOV		R2, A

check_capacity_2:
	CALL	_sec_to_clu_exponent
	MOV		A, R7
	MOV		R4, A

	MOV		R3, #00H
	CALL	_l_shift_32bit	;(cluster-2)*sectornum_per_cluster

/*	MOV		DPTR, #uc_fs_dat_begin_1			 	//20091124
	MOVX	A, @DPTR
	MOV		R4, A
	MOV		DPTR, #uc_fs_dat_begin_2
	MOVX	A, @DPTR
	MOV		R5, A
	MOV		R6, #00H
	MOV		R7, #00H
	CALL	_add_32bit		;get sector address, save in R0-R3	   */

	;MAX_SECTOR - sectornum_per_cluster
	MOV		DPTR, #uc_fs_sec_per_cluster
	MOVX	A, @DPTR
	MOV		B, A
	
	JB		b_sd_flash_flg, chk_sd_exceed
	MOV		DPTR, #uc_fs_fat_sector_max_num1
	MOVX	A, @DPTR
	CLR		C
	SUBB	A, B
	MOV		R4, A
	
	MOV		DPTR, #uc_fs_fat_sector_max_num2
	MOVX	A, @DPTR
	SUBB	A, #00H
	MOV		R5, A
	
	MOV		DPTR, #uc_fs_fat_sector_max_num3
	MOVX	A, @DPTR
	SUBB	A, #00H
	MOV		R6, A

	MOV		DPTR, #uc_fs_fat_sector_max_num4
	MOVX	A, @DPTR
	SUBB	A, #00H
	MOV		R7, A
	JMP		check_nf_exceed
	
chk_sd_exceed:	
	MOV		DPTR, #uc_fs_sd_fat_sector_max_num1
	MOVX	A, @DPTR
	CLR		C
	SUBB	A, B
	MOV		R4, A

	MOV		DPTR, #uc_fs_sd_fat_sector_max_num2
	MOVX	A, @DPTR
	SUBB	A, #00H
	MOV		R5, A

	MOV		DPTR, #uc_fs_sd_fat_sector_max_num3
	MOVX	A, @DPTR
	SUBB	A, #00H
	MOV		R6, A

	MOV		DPTR, #uc_fs_sd_fat_sector_max_num4
	MOVX	A, @DPTR
	SUBB	A, #00H
	MOV		R7, A
	
check_nf_exceed:
	CALL	_sub_32bit		;R0~R3 - R4~R7 = ?
	JC		no_exceed_capacity
	
	MOV		A, R3
	JNZ		exceed_card_capacity
	MOV		A, R2
	JNZ		exceed_card_capacity
	MOV		A, R1
	JNZ		exceed_card_capacity
	MOV		A, R0
	JZ		no_exceed_capacity

exceed_card_capacity:
	SETB	C
	RET

no_exceed_capacity:
	CLR		C
	RET
#endif



#ifdef FAT1_BOUNDARY_CHK_DEF
/*******************************************************************************
compare with fat2 begin sector addr
output:Carry
not exceed: 1
exceed		 : 0
********************************************************************************/
_chk_if_exceed_fat1:
	MOV		DPTR, #uc_fs_fat2_begin_2
	MOVX	A, @DPTR
	CLR		C
	SUBB	A, sfc_logblkaddr1
	JC		sector_addr_exceed_fat2
	;uc_fs_fat2_begin_2	> = 	sfc_logblkaddr1
	JZ		chk_lo_sector_addr
chk_fat_addr_ok:
	SETB	C
	RET

chk_lo_sector_addr:
	MOV		DPTR, #uc_fs_fat2_begin_1
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, sfc_logblkaddr0
	CLR		C
	SUBB	A, B
	JC		chk_fat_addr_ok

sector_addr_exceed_fat2:
	CLR		C
	RET
#endif



#ifdef L_SH16_DEF
/*******************************************************************************
Function:logic 16 bit left shift n bit
Input:	 R0, R1, R2
	 R0:low;	R1:high;	R2:shift bit number	
Output:	 R0, R1	
********************************************************************************/
_l_shift:
	MOV		A, R2
	JZ		l_shift_out

l_shift1:
	MOV		A, R0
	CLR		C
	RLC		A
	MOV		R0, A

	MOV		A, R1
	RLC		A
	MOV		R1, A
	DJNZ	R2, l_shift1
l_shift_out:
	RET
#endif



#ifdef L_SH32_DEF
/*******************************************************************************
Function:logic 32 bit left shift n bit
Input:	 R0, R1, R2, R3, R4
	 R0:low;	R3:high;	R4:shift bit number	
Output:	 R0, R1, R2, R3	
********************************************************************************/
_l_shift_32bit:
	MOV		A, R4
	JZ		l_shift_32bit_out

l_shift_32bit_out_sub:
	MOV		A, R0
	CLR		C
	RLC		A
	MOV		R0, A

	MOV		A, R1
	RLC		A
	MOV		R1, A

	MOV		A, R2
	RLC		A
	MOV		R2, A

	MOV		A, R3
	RLC		A
	MOV		R3, A

	DJNZ	R4, l_shift_32bit_out_sub
l_shift_32bit_out:
	RET
#endif



#ifdef ADD_32_DEF
/*******************************************************************************
Function:32bit add subroutine
Input:	 R0, R1, R2, R3, R4, R5, R6, R7
	 From low to high:r0->r3, r4->r7	
Output:	 R0, R1, R2, R3	
********************************************************************************/
_add_32bit:
	MOV		A, R0
	ADD		A, R4
	MOV		R0, A

	MOV		A, R1
	ADDC	A, R5
	MOV		R1, A
	
	MOV		A, R2
	ADDC	A, R6
	MOV		R2, A

	MOV		A, R3
	ADDC	A, R7
	MOV		R3, A
	RET		 
#endif



#ifdef SECTOR_REV_DEF
PUBLIC	_sector_word_reverse
/*******************************************************************************
restore the normal format, the same as wwinhex
********************************************************************************/
_sector_word_reverse:
	MOV		DPTR, #FS_FCOMBUF
	MOV		R7, #00H

loop_reverse_sector:
	MOVX	A, @DPTR
	MOV		R0, A
	INC		DPTR
	MOVX	A, @DPTR
	DEC		DPL
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, R0
	MOVX	@DPTR, A
	INC		DPTR
	DJNZ	R7, loop_reverse_sector
	RET
#endif



#ifdef MERGE_BYTE_DEF
PUBLIC	__merge_2_byte
/*******************************************************************************
ui16 merge_2_byte(uc8 lo, unsigned hi)
					R7			 R5 
merge two byte into one word(8 bit --> 16 bit)
********************************************************************************/
__merge_2_byte:
	MOV		A, R5
	MOV		R6, A
	RET
#endif



#ifdef VOICE_NAM_CHK_DEF
/*******************************************************************************
check if dir name is WAV_REC
input:DPH, DPL(point to next entry)
output:if identical, set c;if not, clr c
********************************************************************************/
_chk_dir_name:
	MOV		R0, DPL
	MOV		R1, DPH
	CLR		C
	MOV		A, DPL	
	SUBB	A, #32
	MOV		DPL, A
	MOV		A, DPH
	SUBB	A, #0
	MOV		DPH, A

	MOVX	A, @DPTR
	CJNE	A, #4FH, dir_name_not_eq
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #56H, dir_name_not_eq
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #43H, dir_name_not_eq
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #49H, dir_name_not_eq
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #20H, dir_name_not_eq
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #45H, dir_name_not_eq
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #20H, dir_name_not_eq
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #20H, dir_name_not_eq
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #20H, dir_name_not_eq
	INC		DPTR
	MOVX	A, @DPTR
	CJNE	A, #20H, dir_name_not_eq
	SETB	C
	JMP		chk_dirname_ret
dir_name_not_eq:
	CLR		C
chk_dirname_ret:
	MOV		DPL, R0
	MOV		DPH, R1
	RET
#endif



#ifdef CHK_DEL_ITEM_DEF
/*******************************************************************************
input:DPH, DPL(point to next entry)
output:if to be delete item, clr c;if not, setb c
********************************************************************************/
_chk_delete_item:
	MOV		R0, DPL
	MOV		R1, DPH
	CLR		C
	MOV		A, DPL	
	SUBB	A, #31
	MOV		DPL, A
	MOV		A, DPH
	SUBB	A, #0
	MOV		DPH, A
	
	MOVX	A, @DPTR
	CJNE	A, #0E5H, chk_del_set_r7
	CLR		C
	RET	

chk_del_set_r7:
	SETB	C
	MOV		DPL, R0
	MOV		DPH, R1	
	RET
#endif



#ifdef EXT_ENTRT_FAT_DEF
/*******************************************************************************
extend dir fdt, get new cluster, in newly-got cluster
fat position write ffff or ffffffff
********************************************************************************/
_extend_entry_update_fat_1:
	MOV		R0, uc_fs_cur_cluster1
	MOV		R1, uc_fs_cur_cluster2

	JB		b_fs_fat_type_flg, ex_entry_fat_ffff_32
	CALL	_cluster_2_fat_addr
	MOV		A, #0FFH
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A

	JMP		ex_entry_fat_upd

ex_entry_fat_ffff_32:
	MOV		R2, uc_fs_cur_cluster3
	CALL	_cluster_2_fat_addr

	MOV		A, #0FFH
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	MOV		A, #0FH
	INC		DPTR
	MOVX	@DPTR, A
	MOV		A, #0FFH
	INC		DPTR
	MOVX	@DPTR, A

ex_entry_fat_upd:
	MOV		sfc_logblkaddr0, R3
	MOV		sfc_logblkaddr1, R4
	MOV		sfc_logblkaddr2, #00H
	MOV		sfc_logblkaddr3, #00H
write_fat_sector:
	SETB	b_sfc_immediate_wr_flg
	JMP		_write_sector

/***************************************************************
extend dir fdt, get new cluster, let old cluster point to newly-got
cluster, form link
uc_fs_offset_in_sec1:DPL
uc_fs_offset_in_sec2:DPH
uc_fs_tmp_var2:sfc_logblkaddr0
uc_fs_tmp_var3:sfc_logblkaddr1
uc_fs_tmp_var7:sfc_logblkaddr2
***************************************************************/
_extend_entry_update_fat_2:
	MOV		sfc_logblkaddr0, uc_fs_tmp_var2
	MOV		sfc_logblkaddr1, uc_fs_tmp_var3
	MOV		sfc_logblkaddr2, uc_fs_tmp_var7
;	MOV		sfc_logblkaddr3, uc_fs_tmp_var8
	MOV		DPTR,#uc_fs_tmp_var8
	MOVX	A,@DPTR
	MOV		sfc_logblkaddr3,A
	CALL	_fs_read_sector

	MOV		DPL, uc_fs_offset_in_sec1
	MOV		DPH, uc_fs_offset_in_sec2

	JB		b_fs_fat_type_flg, fat32_fat_wr_link22
	MOV		A, DPL
	CLR		C
	SUBB	A, #1
	MOV		DPL, A
	MOV		A, DPH
	SUBB	A, #00H
	MOV		DPH, A
	
	MOV		A, uc_fs_cur_cluster2
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, uc_fs_cur_cluster1
	MOVX	@DPTR, A
	JMP		write_fat_sector

fat32_fat_wr_link22:
	MOV		A, DPL
	CLR		C
	SUBB	A, #3
	MOV		DPL, A
	MOV		A, DPH
	SUBB	A, #00H
	MOV		DPH, A
	
	MOV		A, uc_fs_cur_cluster2
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, uc_fs_cur_cluster1
	MOVX	@DPTR, A
	CLR		A
	INC		DPTR
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, uc_fs_cur_cluster3
	MOVX	@DPTR, A
	JMP		write_fat_sector
#endif



#ifdef CLR_512_DEF
/*******************************************************************************
********************************************************************************/
_clr_512byte:
	MOV		DPTR, #FS_FCOMBUF
	MOV		R0, #32
	MOV		R1, #16
	CLR		A
clr_cycle:
	MOVX	@DPTR, A
	INC		DPTR
	DJNZ	R0, clr_cycle
	MOV		R0, #32
	DJNZ	R1, clr_cycle
	RET
#endif



#ifdef WR_ENTRY_DEF
/*******************************************************************************
********************************************************************************/
_wr_entry_sub1:
	CALL	wr_entry_sub1_1
	INC		DPTR
	CLR		A
	MOVX	@DPTR, A
	INC		DPTR

	JB		b_fs_fat_type_flg, wr_wavrec_clusthi
	MOVX	@DPTR, A
	JMP		wav_rec_clsthi_over

wr_wavrec_clusthi:
	MOV		A, uc_fs_cur_cluster3
	MOVX	@DPTR, A

wav_rec_clsthi_over:
	INC		DPTR
	CALL	wr_entry_sub1_2	
	INC		DPTR
	MOV		A, uc_fs_cur_cluster2
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, uc_fs_cur_cluster1
	MOVX	@DPTR, A
	INC		DPTR
	CLR		A
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	RET


wr_entry_sub1_1:
	MOV		A, #20H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #20H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #10H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #20H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #0FH
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #00H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #5CH
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #53H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #35H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #46H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #35H
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, #46H
	MOVX	@DPTR, A
	RET 


wr_entry_sub1_2:
	MOV		A, #5CH
	MOVX	@DPTR, A	
	INC		DPTR
	MOV		A, #54H
	MOVX	@DPTR, A	
	INC		DPTR
	MOV		A, #35H
	MOVX	@DPTR, A	
	INC		DPTR
	MOV		A, #46H
	MOVX	@DPTR, A
	RET
#endif



#ifdef CLST_2_FAT_DEF
/*******************************************************************************
cluster -> position in FAT
input: R0(cluster1), R1(cluster2), R2(cluster3)
output:R3(fat address lo), R4(fat address hi)
       DPTR
********************************************************************************/
_cluster_2_fat_addr:
	MOV		A, R0
	MOV		R7, A		;save cluster1

	JB		b_fs_fat_type_flg, cluster_2_fat_addr_32
	;CLUSTER_LO is the offset in one sector
	;CLUSTER_HI is the sector number in fat
	MOV		DPTR, #uc_fs_fat1_begin_1
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, R1
	ADD		A, B
	MOV		R3, A

	MOV		DPTR, #uc_fs_fat1_begin_2
	MOVX	A, @DPTR
	ADDC	A, #0
	MOV		R4, A
	;above get fat sector address of current cluster 

	MOV		A, R0
	MOV		B, #2
	MUL		AB
	MOV		DPL, A
	MOV		A, #HIGH FS_FCOMBUF			
	ADD		A, B
	MOV		DPH, A			;goto fat old cluster position, wr link
	RET

cluster_2_fat_addr_32:
	;default R0-R2 input
	MOV		R3, #0
	MOV		R4, #7
	CALL	_r_shift_32bit

	MOV		DPTR, #uc_fs_fat1_begin_1
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, R0
	ADD		A, B
	MOV		R3, A

	MOV		DPTR, #uc_fs_fat1_begin_2
	MOVX	A, @DPTR
	MOV		B, A
	MOV		A, R1
	ADDC	A, B
	MOV		R4, A	

	MOV		A, R7
	ANL		A, #01111111B
	MOV		B, #4
	MUL		AB
	MOV		DPL, A
	MOV		A, #HIGH FS_FCOMBUF			
	ADD		A, B
	MOV		DPH, A			;goto fat new cluster position, wr 0x0fffffff
	RET
#endif



#ifdef WR_REC_FAT_DEF
PUBLIC	_wr_wav_rec_fat
/*******************************************************************************
********************************************************************************/
_wr_wav_rec_fat:
	MOV		R0, uc_fs_cur_cluster1
	MOV		R1, uc_fs_cur_cluster2
	MOV		R2, uc_fs_cur_cluster3
_fat_clst_wr_end_sub:
	CALL	_cluster_2_fat_addr
	JB		b_fs_fat_type_flg, cf_wr_fat_ffff_32

	MOV		A, #0FFH
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A

	JMP		cf_update_fat

cf_wr_fat_ffff_32:
	MOV		A, #0FFH
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	MOV		A, #0FH
	INC		DPTR
	MOVX	@DPTR, A
	MOV		A, #0FFH
	INC		DPTR
	MOVX	@DPTR, A

cf_update_fat:
	MOV		sfc_logblkaddr0, R3
	MOV		sfc_logblkaddr1, R4
	MOV		sfc_logblkaddr2, #00H
	MOV		sfc_logblkaddr3, #00H
	SETB	b_sfc_immediate_wr_flg
	JMP		_write_sector	
#endif



#ifdef FS_GET_FREE_SPACE_DEF
PUBLIC  _get_free_sector_space

EXTRN CODE (_analyze_nf_fat)

#ifdef TARGET_Record_2_Init2
	EXTRN CODE (_analyze_sd_fat)
#endif
/*******************************************************************************
get the free space in disk,sector address
use global var:uc_fs_tmp_var1,uc_fs_tmp_var2,uc_fs_tmp_var3
********************************************************************************/
_get_free_sector_space:
	MOV		uc_fs_tmp_var4, #00H
	MOV		uc_fs_tmp_var3, #00H
	MOV		uc_fs_tmp_var2, #00H
	MOV		uc_fs_tmp_var1, #00H

	MOV		DPTR, #uc_fs_fat1_begin_1
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr0, A
	MOV		DPTR, #uc_fs_fat1_begin_2
	MOVX	A, @DPTR
	MOV		sfc_logblkaddr1, A
	MOV		sfc_logblkaddr2, #00H
	MOV		sfc_logblkaddr3, #00H
	
	MOV		DPTR, #uc_fs_fat1_begin_1
	MOVX	A, @DPTR
	MOV		R0, A
	MOV		DPTR, #uc_fs_fat1_begin_2
	MOVX	A, @DPTR
	MOV		R1, A
	MOV		DPTR, #uc_fs_fat2_begin_1
	MOVX	A, @DPTR
	MOV		R2, A
	MOV		DPTR, #uc_fs_fat2_begin_2
	MOVX	A, @DPTR
	MOV		R3, A
	CLR		C
	MOV		A, R2
	SUBB	A, R0
	MOV		DPTR,  #analytic_fat_length1
	MOVX	@DPTR, A

	MOV		A, R3
	SUBB	A, R1
	MOV		DPTR,  #analytic_fat_length2
	MOVX	@DPTR, A

	CLR		b_exit_program_flg

   	#ifndef TARGET_Record_2_Init2 	//for system only display nf capacity
	CALL	_analyze_nf_fat
	#else
	JB		b_sd_flash_flg, analyze_sd_fat_1
	CALL	_analyze_nf_fat
	JMP		get_sapce_begin
analyze_sd_fat_1:	
	CALL	_analyze_sd_fat	
get_sapce_begin:
	#endif		

	JNB		b_exit_program_flg,get_space	
	RET
	
get_space:
	CLR		C							;FAT scan is over
	MOV		A, uc_fs_tmp_var1
	SUBB	A, #2
	MOV		R0, A
	MOV		A, uc_fs_tmp_var2
	SUBB	A, #0
	MOV		R1, A
	MOV		A, uc_fs_tmp_var3
	SUBB	A, #0
	MOV		R2, A						;uesd cluster - 2
	MOV		A, uc_fs_tmp_var4
	SUBB	A, #0
	MOV		R3, A
	
	CALL	_sec_to_clu_exponent
	MOV		A, R7
	MOV		R4, A
	CALL	_l_shift_32bit				;useed sector

	MOV		uc_fs_tmp_var1, R0
	MOV		uc_fs_tmp_var2, R1
	MOV		uc_fs_tmp_var3, R2
	MOV		uc_fs_tmp_var4, R3

	JB		b_sd_flash_flg, get_sd_spare_space	 		
	MOV		DPTR, #uc_fs_fat_sector_max_num1
	MOVX	A, @DPTR
	MOV		R0, A
	MOV		DPTR, #uc_fs_fat_sector_max_num2
	MOVX	A, @DPTR
	MOV		R1, A
	MOV		DPTR, #uc_fs_fat_sector_max_num3
	MOVX	A, @DPTR
	MOV		R2, A
	MOV		DPTR, #uc_fs_fat_sector_max_num4
	MOVX	A, @DPTR
	MOV		R3, A
	JMP		begin_caculate_space

get_sd_spare_space:	
	MOV		DPTR, #uc_fs_sd_fat_sector_max_num1
	MOVX	A, @DPTR
	MOV		R0, A
	MOV		DPTR, #uc_fs_sd_fat_sector_max_num2
	MOVX	A, @DPTR
	MOV		R1, A
	MOV		DPTR, #uc_fs_sd_fat_sector_max_num3
	MOVX	A, @DPTR
	MOV		R2, A
	MOV		DPTR, #uc_fs_sd_fat_sector_max_num4
	MOVX	A, @DPTR
	MOV		R3, A

begin_caculate_space:
	MOV		R4, uc_fs_tmp_var1
	MOV		R5, uc_fs_tmp_var2
	MOV		R6, uc_fs_tmp_var3
	MOV		R7, uc_fs_tmp_var4

	CALL	_sub_32bit

	CJNE	R3, #0FFH, not_lt_0
	JB		b_sd_flash_flg, obtain_sd_spare_space1
	MOV		DPTR, #ul_fs_spare_space
	JMP		obtain_begin1
obtain_sd_spare_space1:
	MOV		DPTR, #ul_fs_sd_spare_space
obtain_begin1:	
	CLR		A
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	INC		DPTR
	MOVX	@DPTR, A
	RET
	
not_lt_0:
	JB		b_sd_flash_flg, obtain_sd_spare_space2
	MOV		DPTR, #ul_fs_spare_space
	JMP		obtain_begin2
obtain_sd_spare_space2:
	MOV		DPTR, #ul_fs_sd_spare_space
obtain_begin2:	
	MOV		A, R3
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, R2
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, R1
	MOVX	@DPTR, A
	INC		DPTR
	MOV		A, R0
	MOVX	@DPTR, A
exit_program:
	RET
#endif

#ifdef FS_UPDATE_FAT_SRHADDR_DEF
PUBLIC	__seek_free_cluster_from_cbuf

EXTRN CODE (_analyze_nf_fat)
EXTRN CODE (_analyze_sd_fat)
/*******************************************************************************
_seek_free_cluster_from_cbuf(uc8 fat_addr_lo, uc8 fat_addr_hi);
Description:seek blank cluster from fat address [fat_addr_hi, fat_addr_lo]
return:1, ok;0, fail
output:uc_fs_cur_cluster1, uc_fs_cur_cluster2, uc_fs_cur_cluster3

uc_fs_tmp_var1, uc_fs_tmp_var4, uc_fs_tmp_var5, uc_fs_tmp_var6
********************************************************************************/
__seek_free_cluster_from_cbuf:
	MOV		sfc_logblkaddr0, R7
	MOV		sfc_logblkaddr1, R5
	MOV		sfc_logblkaddr2, #0
	MOV		sfc_logblkaddr3, #0
	
	CLR		C
	MOV		DPTR, #uc_fs_fat2_begin_1
	MOVX	A, @DPTR
	SUBB	A, R7
	MOV		DPTR,#analytic_fat_length1
	MOVX	@DPTR, A
	
	MOV		DPTR, #uc_fs_fat2_begin_2
	MOVX	A, @DPTR
	SUBB	A, R5
	MOV		DPTR,#analytic_fat_length2
	MOVX	@DPTR, A
	
	CLR		b_exit_program_flg
	CLR		b_get_free_capacity

	JB		b_sd_flash_flg, analyze_sd_fat_2
	JMP		_analyze_nf_fat
	
analyze_sd_fat_2:
	JMP		_analyze_sd_fat	
	
#endif

#endif
END