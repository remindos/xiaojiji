#include "sh86278_compiler.h"

#ifdef FILE_SYSTEM_FUNC_DEF
#include "sh86278_sfr_reg.inc"
#include "api_fs_var_ext.inc"
#include "api_fs.h"
#include "hwdriver_resident.h"



#ifdef FS_ANALYZE_BUF
ANALYZE_BUF	SEGMENT		CODE
				RSEG	ANALYZE_BUF
				
EXTRN BIT (b_sd_flash_flg)
					
PUBLIC _anlaysis_buffer

//X_ADDR_CBUF
_anlaysis_buffer:
	CLR		b_found_blank_cluster

	JB		b_sd_flash_flg, set_sd_buf_dptr
	MOV		A, R7
	RL		A
	MOV		DPH, A
	MOV		DPL, #00
	JMP		loop_sector	

set_sd_buf_dptr:
	MOV		DPH, #00
	MOV		DPL, #00
	
loop_sector:
	MOV		R7,#128	
	JB		b_fs_fat_type_flg,loop_srh_used_clst
	MOV		R7,#0
			
loop_srh_used_clst:
	JB		b_fs_fat_type_flg, fat32_srh_used	;b_fs_fat_type_flg: 1--Fat32; 0--FAT16
	
	MOVX	A, @DPTR
	INC		DPTR
	MOV		R0, A

	MOVX	A, @DPTR
	INC		DPTR	
	ORL		A, R0		
	
	JNZ		not_free_clst_cnt

	SETB	b_found_blank_cluster

	JB		b_get_free_capacity, chk_if_one_sector_end
	SETB	b_exit_program_flg	
	RET


fat32_srh_used:									;FAT32
	MOVX	A, @DPTR
	INC		DPTR
	MOV		R0, A

	MOVX	A, @DPTR
	INC		DPTR
	ORL		A, R0
	MOV		R0, A

	MOVX	A, @DPTR
	INC		DPTR
	ORL		A, R0
	MOV		R0, A
	
	MOVX	A, @DPTR
	INC		DPTR
	ORL		A, R0

	JNZ		not_free_clst_cnt
	
	SETB	b_found_blank_cluster

	JB		b_get_free_capacity, chk_if_one_sector_end 
	SETB	b_exit_program_flg
	RET		
	
	
not_free_clst_cnt:
	JNB		b_get_free_capacity, chk_if_one_sector_end

	MOV		A, uc_fs_tmp_var1
	ADD		A, #1
	MOV		uc_fs_tmp_var1, A

	MOV		A, uc_fs_tmp_var2
	ADDC	A, #0
	MOV		uc_fs_tmp_var2, A

	MOV		A, uc_fs_tmp_var3
	ADDC	A, #0
	MOV		uc_fs_tmp_var3, A					;uc_fs_tmp_var1,2,3,4 cumulation

	MOV		A, uc_fs_tmp_var4
	ADDC	A, #0
	MOV		uc_fs_tmp_var4, A					;uc_fs_tmp_var1,2,3,4 cumulation

chk_if_one_sector_end:
	DJNZ	R7,loop_srh_used_clst
	DJNZ	R6,loop_sector
	RET
	
	
_set_read_parameters:
	CLR			A
	MOV			buf_offset0, A
	MOV			buf_offset1, A
	MOV			sfc_occ_buf_flag, #SFC_APP_CBUF			
	RET	
#endif


#ifdef FS_NF_FAT_ANALYZE 

ANALYZE_NF_FAT	SEGMENT		CODE
				RSEG		ANALYZE_NF_FAT
						
	
EXTRN BIT (b_multi_page_flag)	

EXTRN DATA	(ef_page_size)
EXTRN XDATA	(start_sector_no)
EXTRN XDATA	(first_rcv_number)
EXTRN XDATA	(middle_times_h)
EXTRN XDATA	(middle_times)
EXTRN XDATA	(last_rcv_number)

EXTRN CODE	(_div41_sub)
EXTRN CODE	(_div31_sub)
EXTRN CODE	(_media_readpage)
EXTRN CODE	(_sub_32bit)
EXTRN CODE	(_sec_to_clu_exponent)

PUBLIC	_analyze_nf_fat

_analyze_nf_fat:
	/*
		begin_free_sector = 0xffff;  //invalid 
	*/

	MOV		DPTR, #uc_fs_fat_srh_addr1
	MOV		A, #0FFH
	MOVX	@DPTR, A
	
	MOV		DPTR, #uc_fs_fat_srh_addr2
	MOV		A, #0FFH
	MOVX	@DPTR, A

	SETB		b_multi_page_flag

	MOV			R6, sfc_logblkaddr3
	MOV			R5, sfc_logblkaddr2
	MOV			R4, sfc_logblkaddr1
	MOV			R3, sfc_logblkaddr0
	MOV			R7, ef_page_size
	CALL		_div41_sub			
	
	MOV			DPTR, #start_sector_no
	MOV			A, R6
	MOVX		@DPTR, A

	MOV			A, ef_page_size
set_first_rcv_number:
	CLR 		C
	SUBB 		A, R6
	
	MOV			R5, A
	MOV			DPTR, #first_rcv_number
	MOVX		@DPTR,A
		
	CLR			C
	MOV			DPTR,#analytic_fat_length1
	MOVX		A,@DPTR
	SUBB	 	A, R5							
	MOV			R3, A		

	MOV			DPTR,#analytic_fat_length2
	MOVX		A,@DPTR
	SUBB		A, #00H
	MOV			R4, A
	
	JC			datalen_lt_rcvnumber	
	JZ			chk_r1_eq_0		
   	JMP			datalen_gt_rcvnumber


chk_r1_eq_0:	
	CJNE		R3, #00H, datalen_gt_rcvnumber	
	JMP			datalen_eq_rcvnumber


datalen_gt_rcvnumber:
	MOV			R5, #00H
	MOV			R7, ef_page_size
	CALL		_div31_sub

	MOV			DPTR, #middle_times_h					
	MOV			A, R4
	MOVX		@DPTR, A
	
	MOV			DPTR, #middle_times					
	MOV			A, R3
	MOVX		@DPTR, A
	
	MOV			DPTR, #last_rcv_number
	MOV			A, R5
	MOVX		@DPTR, A

	;if (last_rcv_number ==0) {middle_times--,last_rcv_number=4/8}
	JNZ 		last_modify_ok	

	MOV			A, ef_page_size				
	MOVX		@DPTR, A

	CALL		_middle_times_sub_1
	
	
last_modify_ok:
	JMP			set_page_flag
	
;---------------------------------------------------------
datalen_lt_rcvnumber:
	MOV			DPTR,#analytic_fat_length2
	MOVX		A,@DPTR
	MOV			DPTR, #first_rcv_number
	MOVX		@DPTR, A			
	
	
datalen_eq_rcvnumber:
	CLR			b_multi_page_flag
	
;---------------------------------------------------------
set_page_flag:
	CALL	_set_read_parameters
	CALL	_media_readpage
	
	MOV		DPTR, #start_sector_no
	MOVX	A, @DPTR
	MOV		R7, A
	
	MOV		DPTR, #first_rcv_number
	MOVX	A, @DPTR
	MOV		R6, A

	CALL	_anlaysis_buffer

	JNB		b_found_blank_cluster,  nf_not_found_blank_cluster_beg
	
	MOV		DPTR, #uc_fs_fat_srh_addr2
	MOVX	A, @DPTR
	CJNE	A, #0FFH, nf_check_exit_program_beg

nf_check_exit_program_beg:		
	MOV		DPTR, #uc_fs_fat_srh_addr1
	MOV		A, sfc_logblkaddr0
	MOVX	@DPTR, A
	
	MOV		DPTR, #uc_fs_fat_srh_addr2
	MOV		A, sfc_logblkaddr1
	MOVX	@DPTR, A	
	JB		b_exit_program_flg, nf_exit_program_beg

nf_not_found_blank_cluster_beg:
	JB		b_multi_page_flag, 	_proc_middle_stage
nf_exit_program_beg:	
	RET
		
		
_proc_middle_stage:
	CLR		C
	MOV		A, sfc_logblkaddr0
	ADD		A, ef_page_size
	MOV		sfc_logblkaddr0, A

	MOV		A, sfc_logblkaddr1
	ADDC	A, #00H
	MOV		sfc_logblkaddr1, A

	MOV		A, sfc_logblkaddr2
	ADDC	A, #00H
	MOV		sfc_logblkaddr2, A
	 
	MOV		A, sfc_logblkaddr3
	ADDC	A, #00H
	MOV		sfc_logblkaddr3, A
	
	MOV		DPTR, #middle_times
	MOVX	A, @DPTR
	MOV		R0, A	

	MOV		DPTR, #middle_times_h
	MOVX	A, @DPTR
	ORL		A,  R0
	JZ		_proc_last_stage
	
	CALL	_set_read_parameters
	CALL	_media_readpage
	
	MOV		R7, #00
	MOV		R6, ef_page_size

	CALL	_anlaysis_buffer

	/*
	if (begin_free_sector = = 0xffff &&  b_found_blank_cluster = = 1)
	{
		begin_free_sector = sfc_logblkaddr3,2,1,0 - pagesize;
	}
*/	
	
	JNB		b_found_blank_cluster,  nf_not_found_blank_cluster_mid
	
	MOV		DPTR, #uc_fs_fat_srh_addr2
	MOVX	A, @DPTR
	CJNE	A, #0FFH, nf_check_exit_program_mid

	CLR		C
	MOV		A, sfc_logblkaddr0
	SUBB	A, ef_page_size
	MOV		DPTR, #uc_fs_fat_srh_addr1
	MOVX	@DPTR, A

	MOV		A, sfc_logblkaddr1
	SUBB	A, #00H
	MOV		DPTR, #uc_fs_fat_srh_addr2
	MOVX	@DPTR, A

nf_not_found_blank_cluster_mid:
nf_check_exit_program_mid:
	JB		b_exit_program_flg, exit_program
	
	CALL	_middle_times_sub_1
	JMP		_proc_middle_stage
	
_proc_last_stage:
	CALL	_set_read_parameters
	CALL	_media_readpage
	
	MOV		R7, #00
	MOV		DPTR, #last_rcv_number
	MOVX	A, @DPTR
	MOV		R6, A
	CALL	_anlaysis_buffer
	
	JNB		b_found_blank_cluster,  exit_program
	
	MOV		DPTR, #uc_fs_fat_srh_addr2
	MOVX	A, @DPTR
	CJNE	A, #0FFH, exit_program
		
	CLR		C
	MOV		A, sfc_logblkaddr0
	SUBB	A, ef_page_size
	MOV		DPTR, #uc_fs_fat_srh_addr1
	MOVX	@DPTR, A

	MOV		A, sfc_logblkaddr1
	SUBB	A, #00H
	MOV		DPTR, #uc_fs_fat_srh_addr2
	MOVX	@DPTR, A
		
exit_program:
	RET
	
	
_middle_times_sub_1:
	MOV			DPTR, #middle_times			
	MOVX		A, @DPTR
	CLR			C
	SUBB		A, #01H
	MOVX			@DPTR, A
	CJNE		A, #0FFH,	_not_dec_middle_timens_h
	
	MOV			DPTR, #middle_times_h			
	MOVX		A, @DPTR
	DEC			A
	MOVX		@DPTR, A			
_not_dec_middle_timens_h:	
	RET	
#endif


#ifdef FS_SD_FAT_ANALYZE 

ANALYZE_SD_FAT	SEGMENT		CODE
				RSEG		ANALYZE_SD_FAT
				
EXTRN	XDATA	(uc_msi_block_num)
EXTRN	CODE	(_read_msi_multisec_api)
EXTRN	CODE	(_msi_end_read)

				
PUBLIC	_analyze_sd_fat				
				
_analyze_sd_fat:
	/*
		begin_free_sector = 0xffff;  //invalid 
	*/

	MOV		DPTR, #uc_fs_sd_fat_srh_addr1
	MOV		A, #0FFH
	MOVX	@DPTR, A
	
	MOV		DPTR, #uc_fs_sd_fat_srh_addr2
	MOV		A, #0FFH
	MOVX	@DPTR, A

	MOV		A, #28		//CBUF is 7k*16bit size
	MOV		DPTR, #uc_msi_block_num	//its unit is 512 bytes
	MOVX	@DPTR, A
	
continue_multi_rd:	
	
	CLR		C
	MOV		DPTR, #analytic_fat_length1
	MOVX	A, @DPTR
	SUBB	A, #28	
	MOV		B, A
	JC		chk_length2
	MOVX	@DPTR, A	//update length1
	JMP		multi_rd_begin
		
chk_length2:	
	MOV		DPTR, #analytic_fat_length2
	MOVX	A, @DPTR
	SUBB	A, #0
	JC		update_msi_block_num
	MOVX	@DPTR, A	//update length2
	MOV		A, B
	MOV		DPTR, #analytic_fat_length1
	MOVX	@DPTR, A
	JMP		multi_rd_begin
	
update_msi_block_num:

	MOV		DPTR, #analytic_fat_length1
	MOVX	A, @DPTR
	
	MOV		DPTR, #uc_msi_block_num
	MOVX	@DPTR, A
	
	CLR		A
	MOV		DPTR, #analytic_fat_length1
	MOVX	@DPTR, A
	
multi_rd_begin:	
	CALL	_set_read_parameters
	
	CALL	_read_msi_multisec_api		//msi read 14K data
	
	CALL	_msi_end_read

	MOV		R7, #00
	MOV		DPTR, #uc_msi_block_num
	MOVX	A, @DPTR
	MOV		R6, A

	CALL	_anlaysis_buffer

	JNB		b_found_blank_cluster, check_sd_goon_srh 

	MOV		DPTR, #uc_fs_sd_fat_srh_addr2
	MOVX	A, @DPTR
	CJNE	A, #0FFH, check_sd_goon_srh
		
	MOV	  	A, sfc_logblkaddr1
	MOV		DPTR, #uc_fs_sd_fat_srh_addr2
	MOVX	@DPTR, A

	MOV	  	A, sfc_logblkaddr0
	MOV		DPTR, #uc_fs_sd_fat_srh_addr1
	MOVX	@DPTR, A				

check_sd_goon_srh:
	JB		b_exit_program_flg, exit_sd_program 

	CLR		C
	MOV		DPTR, #uc_msi_block_num
	MOVX	A, @DPTR
	ADD		A, sfc_logblkaddr0
	MOV		sfc_logblkaddr0,A

	MOV		A, sfc_logblkaddr1
	ADDC	A, #0
	MOV	 	sfc_logblkaddr1,A

	MOV		DPTR, #analytic_fat_length2
	MOVX	A, @DPTR
	CJNE	A, #00H,continue_multi_rd 

	MOV		DPTR, #analytic_fat_length1
	MOVX	A, @DPTR
	CJNE	A, #00H, continue_multi_rd

exit_sd_program:	
	RET
					
				
#endif				

#endif

END


