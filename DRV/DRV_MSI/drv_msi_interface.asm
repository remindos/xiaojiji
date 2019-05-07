/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: drv_msi.asm
Creator: aj.zhao					Date: 2009-06-11
Description: msi interface subroutine
Others: msi api functions: 
  1. MSI_Init()
  2. _read_msi_sector()
  3. _write_msi_sector()
Version: V0.3
History:
	V0.3	2010-04-13		aj.zhao
			1.init SD Card/MMC twice for stability
	V0.2	2010-01-20		aj.zhao
			1. set the bit"b_read_block_flg", for distinguish the opration is read or write
	V0.1	2009-06-11		aj.zhao
			1. build this  module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef MSI_API_DEF

#include "sh86278_sfr_reg.inc"
#include "sh86278_api_macro.inc"
#include "drv_msi_var_ext.inc"

MSI_API_FUNC	SEGMENT	CODE
				RSEG	MSI_API_FUNC

#ifdef MSI_API_INIT_DEF
EXTRN	CODE	(_init_sd_mmc_card)
PUBLIC	MSI_Init

/*******************************************************************************
Function: MSI_Init()
Description: init sd or mmc
Calls:
Global:uc_msi_status -- sd/mmc status (0: sd/mmc is ok; others: error)
       b_msi_sd1_mmc0_flg -- card flag (0:MMC; 1:SD Card)
Input: Null
Output:CY -- 1:ok
             0:error
Others:
********************************************************************************/
MSI_Init:
#ifdef	MSI_API_MULTI_SEC_DEF
	CLR		b_msi_rd_status_flg
	CLR		b_msi_wr_status_flg
#endif

	mReset_WatchDog
	CALL	_init_sd_mmc_card
	MOV		A, uc_msi_status
 	JZ		init_successful
	CALL	_init_sd_mmc_card
	MOV		A, uc_msi_status
	JNZ		init_aborted
	
init_successful:	
	SETB	C
	RET
init_aborted:
	CLR		C
	RET
#endif


#ifdef	MSI_API_READ_SECTOR_DEF
PUBLIC	_read_msi_sector
EXTRN	CODE	(_read_msi_sec_sub)
/*******************************************************************************
Function: _read_msi_sector()
Description: read 512 bytes data from SD/MMC
Calls:
Global:uc_msi_sec_addr_0,uc_msi_sec_addr_1,uc_msi_sec_addr_2,uc_msi_sec_addr_3
       uc_msi_occ_buf_flg: 1--cbuf
                           2--dbuf0
                           3--dbuf1
                           4--fbuf
       uc_msi_byte_offset_0,uc_msi_byte_offset_1:buffer offset(word)
Input: Null
Output:CY:	1--read ok
            0--read error
Others:
********************************************************************************/
_read_msi_sector:
	mSet_MSClock_22M
	SETB	b_read_block_flg

	CALL	_read_msi_sec_sub
	MOV		sfc_errcode, #01H
	RET
#endif


#ifdef	MSI_API_WRITE_SECTOR_DEF
PUBLIC	_write_msi_sector
EXTRN	CODE	(_write_msi_sec_sub)
/*******************************************************************************
Function: _write_msi_sector()
Description: write 512 bytes data to the SD/MMC
Calls:
Global:uc_msi_sec_addr_0,uc_msi_sec_addr_1,uc_msi_sec_addr_2,uc_msi_sec_addr_3
       b_msi_sd1_mmc0_flg: 1--SD Card
                           0--MMC
       uc_msi_occ_buf_flg: 1--cbuf
                           2--dbuf0
                           3--dbuf1
                           4--fbuf
       uc_msi_byte_offset_0,uc_msi_byte_offset_1:buffer offset(word)
Input: Null
Output:CY:	1--read ok
            0--read error
Others:
********************************************************************************/
_write_msi_sector:
	mSet_MSClock_6M
	CLR		b_read_block_flg

	CALL	_write_msi_sec_sub
	MOV		sfc_errcode, #01H
	RET
#endif

#ifdef	MSI_API_MULTI_SEC_DEF
PUBLIC	_read_msi_multisec_api
PUBLIC	_write_msi_multisec_api
PUBLIC	_msi_end_write
PUBLIC	_msi_end_read
EXTRN	CODE	(_read_msi_multisector_b)
EXTRN	CODE	(_read_msi_multisector)
EXTRN	CODE	(_read_msi_multisector_e)
EXTRN	CODE	(_write_msi_multisector_b)
EXTRN	CODE	(_write_msi_multisector)
EXTRN	CODE	(_write_msi_multisector_e)

/*******************************************************************************
Function: _read_msi_multisec_api()
Description: read 512 bytes data from SD/MMC
Calls:
Global:uc_msi_sec_addr_0,uc_msi_sec_addr_1,uc_msi_sec_addr_2,uc_msi_sec_addr_3
       uc_msi_occ_buf_flg: 1--cbuf
                           2--dbuf0
                           3--dbuf1
                           4--fbuf
       uc_msi_byte_offset_0,uc_msi_byte_offset_1:buffer offset(word)
       b_msi_rd_status_flg:	1--begin read
                            2--continue read
                            3--read end
Input: b_rd_first_page_flg_sd:	0--read middle page
				1--read first page(will check address if continue)
       b_msi_rd_status_flg:	0--read end or no read
       				1--read continue
Output:CY:			1--read ok
            			0--read error
Others:
********************************************************************************/
_read_msi_multisec_api:
	mSet_MSClock_22M
	CLR		b_read_block_flg

	JB		b_msi_rd_status_flg, continue_read
	CALL	_read_msi_multisector_b
	SETB	b_msi_rd_status_flg
	JMP		rd_multi_ret

continue_read:
	CALL	_read_msi_multisector		
rd_multi_ret:
	MOV		sfc_errcode, #01H
	RET

_msi_end_read:
	JNB		b_msi_rd_status_flg, do_end_read_ret		
	CALL	_read_msi_multisector_e
	CLR		b_msi_rd_status_flg
do_end_read_ret:
	RET



/*******************************************************************************
Function: _write_msi_multisec_api()
Description: write 512 bytes data to the SD/MMC
Calls:
Global:uc_msi_sec_addr_0,uc_msi_sec_addr_1,uc_msi_sec_addr_2,uc_msi_sec_addr_3
       b_msi_sd1_mmc0_flg: 1--SD Card
                           0--MMC
       uc_msi_occ_buf_flg: 1--cbuf
                           2--dbuf0
                           3--dbuf1
                           4--fbuf
       uc_msi_byte_offset_0,uc_msi_byte_offset_1:buffer offset(word)
       b_msi_wr_status_flg:	1--begin read
                            2--continue read
                            3--read end

modify:
		b_msi_write_multi_flg:
								0--write end or no write
								1--continue write
Input: wr_first_page, stage_flag

Output:CY:		1--read ok
            	0--read error
Others:
********************************************************************************/
_write_msi_multisec_api:
	mSet_MSClock_6M
	CLR		b_read_block_flg

	JB		b_msi_wr_status_flg, continue_write
	CALL	_write_msi_multisector_b
	SETB	b_msi_wr_status_flg
	JMP		write_msi_ret
	
continue_write:
	CALL	_write_msi_multisector
write_msi_ret:
	MOV		sfc_errcode, #01H
	RET

_msi_end_write:
	JNB		b_msi_wr_status_flg, do_end_write_ret	
	CALL	_write_msi_multisector_e
	CLR		b_msi_wr_status_flg
do_end_write_ret:
	RET

#endif
		

#endif
	END