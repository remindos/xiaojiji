/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_rec_encode_lib.asm
Creator: zj.zong					Date: 2009-06-02
Description: encode library, compress 16bit into 4bit
Others:
Version: V0.1
History:
	V0.1	2009-06-02		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_RECORD_ENCODE_LIB_DEF


EXTRN	DATA	(ui_enc_indata)
EXTRN	DATA	(uc_enc_step_idx)
EXTRN	DATA	(uc_enc_thecode)
EXTRN	DATA	(ui_enc_sample)
EXTRN	DATA	(uc_enc_sample_expand)
EXTRN	DATA	(ui_enc_step)
EXTRN	DATA	(ui_enc_step_sft1)
EXTRN	DATA	(uc_enc_diff_buf)
EXTRN	DATA	(uc_enc_effdiff_buf)
EXTRN	DATA	(ui_enc_step_sft2)

PUBLIC Ap_Generate_Nibble

ADPCM_ENC_SEG	SEGMENT		CODE 
				RSEG		ADPCM_ENC_SEG
/*******************************************************************************
Function: Ap_Generate_Nibble()
Description: encode library, compress 16bit into 4bit
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
Ap_Generate_Nibble:
	MOV		R3, ui_enc_indata
	MOV		R4, ui_enc_indata + 1	

	MOV		A,  R3
	RLC		A
	SUBB	A,	ACC
	MOV		R2, A						;in maybe +,or -, so expand sign

	MOV		R6, ui_enc_sample
	MOV		R7, ui_enc_sample + 1	

	MOV		A,  R6
	RLC		A
	SUBB	A, ACC
	MOV		R5, A						; ui_enc_sample maybe +,or -, so expand sign
	
	MOV		uc_enc_sample_expand, R5
	MOV		ui_enc_sample, R6
	MOV		ui_enc_sample + 1, R7

	CALL	_subb24

 	MOV		uc_enc_diff_buf, R2
 	MOV		uc_enc_diff_buf + 1, R3
	MOV		uc_enc_diff_buf + 2, R4		;uc_enc_diff_buf = in - ui_enc_sample
				
	MOV		uc_enc_thecode, #0x00		;code = 0;
	MOV		A, R2
 	JNB		ACC.7, diff_ge0

	MOV		uc_enc_thecode, #0x08
	MOV		A, R2
	MOV		R5, A
	MOV		A, R3
	MOV		R6, A
	MOV		A, R4
	MOV		R7, A
	MOV		R2, #0x00
	MOV		R3, #0x00
	MOV		R4, #0x00
	
	CALL	_subb24

	MOV		uc_enc_diff_buf, R2
 	MOV		uc_enc_diff_buf + 1, R3
	MOV		uc_enc_diff_buf + 2, R4


diff_ge0:
	MOV		A, uc_enc_step_idx		
	RL		A			
	MOV		DPTR, #IMA_STEPTABLE	
	MOVC	A, @A+DPTR
	MOV		ui_enc_step,A
	
	MOV		A, uc_enc_step_idx
	RL		A
	INC		A
	MOVC	A, @A+DPTR
	MOV		ui_enc_step + 1, A			;ui_enc_step = IMA_STEPTABLE[uc_enc_step_idx];

	MOV		R6, ui_enc_step
	MOV		R7, ui_enc_step + 1

	CALL    _rrc_16
	MOV     ui_enc_step_sft1, R6
	MOV     ui_enc_step_sft1 + 1, R7	;ui_enc_step_sft1 = (ui_enc_step >> 1);

	CALL	_rrc_16
	MOV		ui_enc_step_sft2, R6
	MOV		ui_enc_step_sft2+1, R7		;ui_enc_step_sft2 = (ui_enc_step >> 2);

	CALL	_rrc_16
	MOV		uc_enc_effdiff_buf, #0x00;
	MOV		uc_enc_effdiff_buf + 1, R6
	MOV		uc_enc_effdiff_buf + 2, R7	;uc_enc_effdiff_buf = (ui_enc_step >> 3);

	MOV		R6, ui_enc_step
	MOV		R7, ui_enc_step+1
	MOV     R1, #0x4
	CALL 	func_if_diff_ge_step

	MOV		R6, ui_enc_step_sft1		;ui_enc_step>>=1	
	MOV		R7, ui_enc_step_sft1+1   
	MOV     R1, #0x2
	CALL 	func_if_diff_ge_step

	MOV		R6, ui_enc_step_sft2		;ui_enc_step>>=1
	MOV		R7, ui_enc_step_sft2+1
	MOV     R1, #0x1  					;if (uc_enc_diff_buf >= ui_enc_step) 
	CALL 	func_if_diff_ge_step		;code |= 1;	

	MOV		R2, uc_enc_sample_expand
	MOV		R3, ui_enc_sample
	MOV		R4, ui_enc_sample + 1
	MOV		R5, uc_enc_effdiff_buf
	MOV		R6, uc_enc_effdiff_buf + 1
	MOV		R7, uc_enc_effdiff_buf + 2

	MOV		A, uc_enc_thecode
	JNB		ACC.3, code1_bit3_0		;if( code1 & 8 )
 	CALL	_subb24					;ui_enc_sample -= uc_enc_effdiff_buf
	JMP		sample_clamp

code1_bit3_0:						;else
	CALL	_add24					;ui_enc_sample += uc_enc_effdiff_buf

sample_clamp:
	CALL	clamp_sample			;clamp_sample(ui_enc_sample);

	MOV		ui_enc_sample, R3
	MOV		ui_enc_sample + 1, R4
	
	MOV		A, uc_enc_thecode
	MOV		DPTR, #IMA_INDEXTABLE
	MOVC	A, @A+DPTR
	ADD		A, uc_enc_step_idx
	MOV		uc_enc_step_idx, A		;uc_enc_step_idx += IMA_INDEXTABLE[code1];

	JB		ACC.7, stepindex_eq0

	CLR		C
	SUBB	A, #88
	JNC		stepindex_eq88
	RET

stepindex_eq88:
	MOV		uc_enc_step_idx, #88
	RET

stepindex_eq0:
	MOV		uc_enc_step_idx, #0	
	RET


/*******************************************************************************
if (uc_enc_diff_buf >= ui_enc_step) 
{
   code |= 4/2/1;
   uc_enc_diff_buf -= ui_enc_step;
   uc_enc_effdiff_buf += ui_enc_step;
}

in:  uc_enc_diff_buf,uc_enc_effdiff_buf, code1, R1(4/2/1)
     ui_enc_step(R6,R7)	
********************************************************************************/
func_if_diff_ge_step:
	MOV		R2, uc_enc_diff_buf
	MOV		R3, uc_enc_diff_buf + 1
	MOV		R4, uc_enc_diff_buf + 2
	MOV		R5, #0x00

	CALL 	_subb24
	
	JNC		diff_ge_step					;if(uc_enc_diff_buf>=ui_enc_step)
	RET


diff_ge_step:
	MOV		uc_enc_diff_buf, R2 	
	MOV		uc_enc_diff_buf + 1, R3 	
	MOV		uc_enc_diff_buf + 2, R4			;uc_enc_diff_buf -= ui_enc_step;

	MOV		R2, uc_enc_effdiff_buf
	MOV		R3, uc_enc_effdiff_buf + 1
	MOV		R4, uc_enc_effdiff_buf + 2

	CALL	_add24

	MOV		uc_enc_effdiff_buf, R2
	MOV		uc_enc_effdiff_buf + 1, R3
	MOV		uc_enc_effdiff_buf + 2, R4    	;uc_enc_effdiff_buf += ui_enc_step		
	MOV		A, R1
	ORL		uc_enc_thecode, A 				;code1 |= 4/2/1
	RET
	


clamp_sample:
	MOV		A, R2
	JB		ACC.7, sample_lt_0 	

	; if( result > 32767 )
	;    R2 R3 R4
	;    xx xx xx	
	; -  00 7F FF 
	; ------------
	;

	SETB	C
	MOV		A, R3
	SUBB	A, #0x7F

	MOV		A, R2
	XRL		A, #0x80
	SUBB	A, #0x80
	JC		exit_sample_clamp

result_0x7fff:
	MOV		R3, #0x7f
	MOV		R4, #0xff 	
	JMP		exit_sample_clamp

sample_lt_0:
	CLR		C
	MOV		A, R3
	SUBB	A, #0x80
	MOV		A, R2
	XRL		A, #0x80
	SUBB	A, #0x7F
	JNC		exit_sample_clamp 

result_0x8000:
	MOV		R3, #0x80
	MOV		R4, #0x00
	
exit_sample_clamp:
	RET


/*******************************************************************************
	R6R7 >>1
********************************************************************************/
_rrc_16:
	CLR		C
	MOV		A, R6
	RRC		A
	MOV		R6, A

	MOV		A, R7
	RRC		A
	MOV		R7, A
	RET


/*******************************************************************************
	R3R4 = R3R4 + R6R7	
********************************************************************************/
_add16:
	CLR		C
	MOV		A, R4		;R4 = R4 + R7
	ADD		A, R7
 	MOV		R4, A

	MOV		A, R3
	ADDC	A, R6
 	MOV		R3, A		;R3 = R3 + R6 + C
	RET


/*******************************************************************************
	R2R3R4 = R2R3R4 + R5R6R7	
********************************************************************************/
_add24:
	CALL	_add16
	MOV		A, R2
	ADDC	A, R5
 	MOV		R2, A		;R2 = R2 + R5 + C
	RET


/*******************************************************************************
	R3R4 = R3R4 - R6R7	
********************************************************************************/
_subb16:
	CLR		C
	MOV		A, R4
	SUBB	A, R7
	MOV		R4, A		;R4 = R4 - R7

	MOV		A, R3
	SUBB	A, R6
	MOV		R3, A		;R3 = R3 - R6 - C  
	RET  


/*******************************************************************************
	R2R3R4 = R2R3R4 - R5R6R7	
********************************************************************************/
_subb24:
 	CALL 	_subb16
	MOV		A, R2
	SUBB	A, R5
	MOV		R2, A		;R2 = R2 - R5 - C  
	RET



IMA_INDEXTABLE: DB -1, -1, -1, -1, 2, 4, 6, 8;
				DB -1, -1, -1, -1, 2, 4, 6, 8;	

IMA_STEPTABLE:	
		DW  7, 8, 9, 10, 11, 12, 13, 14;
		DW  16, 17, 19, 21, 23, 25, 28, 31;
		DW  34, 37, 41, 45, 50, 55, 60, 66;
		DW  73, 80, 88, 97, 107, 118, 130, 143;
		DW  157, 173, 190, 209, 230, 253, 279, 307;
		DW  337, 371, 408, 449, 494, 544, 598, 658;
		DW  724, 796, 876, 963, 1060, 1166, 1282, 1411;
		DW  1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024;
		DW  3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484;
		DW  7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899;
		DW  15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794;
		DW  32767;


#endif
END