/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: api_communication_cmd.asm
Creator: zj.zong					Date: 2009-04-14
Description: there is communicational commands between mcu and core
Others: 
Version: V0.1
History:

	V0.1	2009-04-14 		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef API_COMMUNICANT_COMMAND_DEF
#include "sh86278_sfr_reg.inc"
#include "ui_resident.h"
#include "ui_resident_var_ext.inc"

API_COMMUNICANT_CMD		SEGMENT	CODE
					RSEG	API_COMMUNICANT_CMD
EXTRN DATA (uc_mpeg_state)

/*******************************************************
only bit7 and bit5 is valid
*******************************************************/

#define	IRQ_CMD			BIT7_MASK
#define	IRQ_SPECIAL1	BIT5_MASK

#define	CR_CMD_START_DEC		0x01	//be used to Begin decoding for mp3/mmv/jpeg/adpcm/bmp
#define	CR_CMD_CONTROL_DEC		0x02	//be used to inform MPEG to pause /stop/continue decoding
#define	CR_CMD_RST_DEC			0x03	//be used to reset decoder for mp3/jpeg/mmv/adpcm decode
#define	CR_CMD_DEC_MP3_HEADER	0x04	//be used to start decoding 5s, and get mp3 bitrate and sample rate
#define	CR_CMD_SET_EQ			0x05	//be used to set EQ effect for mp3 decoder
#define	CR_CMD_SET_LCD_PARA		0x06	//be used to inform core the width and height of Lcd
#define	CR_CMD_PLAY_AUDIO_DATA	0x07	//be used to inform Core to send out the audio data via sport
#define	CR_CMD_SET_FREQ_FADE	0x08	//be used to Inform MPEG to enable /disable fade operation or display of dynamic freqs
#define	CR_CMD_VIDEO_FAST		0x09	//be used to inform MPEG to fast forward/backward when playing AMV/MMV
#define CR_CMD_CHANGE_MPEG_CLK	0x0A	//be used to inform MPEG to change clock to 77MHz for play in lowspeed SD/MMC


#ifdef API_CMD_START_DEC
PUBLIC	Api_Command_StartDec_MP3
PUBLIC	Api_Command_StartDec_Wav
/*******************************************************************************
be used to Begin decoding for mp3/mmv/adpcm
********************************************************************************/
Api_Command_StartDec_MP3:
Api_Command_StartDec_Wav:
	CALL	_check_cmd_flg
	CLR		A
	MOV		DPTR, #CR_MTDDAT1_L
	MOVX	@DPTR, A
	MOV		DPTR, #CR_MTDDAT1_H
	MOVX	@DPTR, A
	MOV		DPTR, #CR_MTDDAT2_L
	MOVX	@DPTR, A

send_start_dec_cmd:
	MOV		A, #CR_CMD_START_DEC
	MOV		DPTR, #CR_MTDCMD_L
	MOVX	@DPTR, A
	RET
#endif



#ifdef API_CMD_CONTROL_DEC
PUBLIC	Api_Command_PauseDec
PUBLIC	Api_Command_StopDec
PUBLIC	Api_Command_ContinueDec
/*******************************************************************************
be used to inform MPEG to pause /stop/continue decoding
********************************************************************************/
Api_Command_PauseDec:
	CALL	_check_cmd_flg
	MOV		A, #01H
	JMP		send_control_cmd

Api_Command_StopDec:
	CALL	_check_cmd_flg
	MOV		A, #02H
	JMP		send_control_cmd

Api_Command_ContinueDec:
	CALL	_check_cmd_flg
	MOV		A, #04H

send_control_cmd:
	MOV		DPTR, #CR_MTDDAT1_L
	MOVX	@DPTR, A
	MOV		A, #CR_CMD_CONTROL_DEC
	MOV		DPTR, #CR_MTDCMD_L
	MOVX	@DPTR, A
	RET
#endif


#ifdef CMD_API_PLAY_AUDIO_DATA
PUBLIC	Api_Command_PlayAudio_Data
/*******************************************************************************
be used to inform Core to send out the audio data via sport
********************************************************************************/
Api_Command_PlayAudio_Data:
	CALL	_check_cmd_flg
	MOV		A, #CR_CMD_PLAY_AUDIO_DATA
	MOV		DPTR, #CR_MTDCMD_L
	MOVX	@DPTR, A
	RET
#endif

#ifdef API_CMD_RESET_DEC
PUBLIC	Api_Command_ResetDec_MP3
PUBLIC	Api_Command_ResetDec_MP3_AB
PUBLIC	Api_Command_ResetDec_Wav
PUBLIC	Api_Command_ResetDec_Wav_AB
/*******************************************************************************
be used to reset decoder for mp3/adpcm decode
********************************************************************************/
Api_Command_ResetDec_MP3:
	CALL	_check_cmd_flg
	MOV		A, #01H
	JMP		send_rst_cmd
	
Api_Command_ResetDec_MP3_AB:
	CALL	_check_cmd_flg
	MOV		A, #09H
	JMP		send_rst_cmd
	
Api_Command_ResetDec_Wav:
	CALL	_check_cmd_flg
	MOV		A, #02H
	JMP		send_rst_cmd
	
Api_Command_ResetDec_Wav_AB:
	CALL	_check_cmd_flg
	MOV		A, #0aH
	JMP		send_rst_cmd
send_rst_cmd:
	MOV		DPTR, #CR_MTDDAT1_L
	MOVX	@DPTR, A
	MOV		A, #CR_CMD_RST_DEC
	MOV		DPTR, #CR_MTDCMD_L
	MOVX	@DPTR, A
	RET
#endif



#ifdef API_CMD_DEC_MP3_DEADER
PUBLIC	Api_Command_Dec_MP3Header
/*******************************************************************************
be used to start decoding 5s, and get mp3 bitrate and sample rate
********************************************************************************/
Api_Command_Dec_MP3Header:
	CALL	_check_cmd_flg
	MOV		A, #CR_CMD_DEC_MP3_HEADER
	MOV		DPTR, #CR_MTDCMD_L
	MOVX	@DPTR, A
	RET
#endif



#ifdef API_CMD_SET_EQ
PUBLIC	_Api_Command_Set_EQ
/*******************************************************************************
be used to set EQ effect for mp3 decoder
input is uc_mp3_eq_mode
********************************************************************************/
_Api_Command_Set_EQ:
	CALL	_check_cmd_flg
	MOV		A, R7
	MOV		DPTR, #CR_MTDDAT1_L
	MOVX	@DPTR, A
	
	MOV		A, #CR_CMD_SET_EQ
	MOV		DPTR, #CR_MTDCMD_L
	MOVX	@DPTR, A
	RET
#endif



#ifdef API_CMD_SET_LCD_PARA
PUBLIC	Api_Command_Set_LcdPara
/*******************************************************************************
be used to inform core the width and height of Lcd
********************************************************************************/
Api_Command_Set_LcdPara:
	CALL	_check_cmd_flg
	MOV		DPTR, #uc_lcd_width
	MOVX	A, @DPTR
	MOV		DPTR, #CR_MTDDAT1_L
	MOVX	@DPTR, A
	MOV		DPTR, #uc_lcd_height
	MOVX	A, @DPTR
	MOV		DPTR, #CR_MTDDAT1_H
	MOVX	@DPTR, A

	MOV		A, #CR_CMD_SET_LCD_PARA
	MOV		DPTR, #CR_MTDCMD_L
	MOVX	@DPTR, A
	RET
#endif

#ifdef API_CMD_SET_FREQ_FADE
PUBLIC	Api_Command_Ena_FreqFade
PUBLIC	Api_Command_Ena_MP3Freq
PUBLIC	Api_Command_Dis_MP3Freq
PUBLIC	Api_Command_Ena_Fade
PUBLIC	Api_Command_Dis_Fade
/*******************************************************************************
be used to Inform MPEG to enable/disable fade(in or out) operation
or display of dynamic freqs
********************************************************************************/
Api_Command_Ena_MP3Freq:
	CALL	_check_cmd_flg
	MOV		A, #01H
	JMP		send_freq_fade_cmd

Api_Command_Ena_Fade:
	CALL	_check_cmd_flg
	MOV		A, #02H
	JMP		send_freq_fade_cmd

Api_Command_Ena_FreqFade:
	CALL	_check_cmd_flg
	MOV		A, #03H
	JMP		send_freq_fade_cmd

Api_Command_Dis_MP3Freq:
Api_Command_Dis_Fade:
	CALL	_check_cmd_flg
	MOV		A, #00H

send_freq_fade_cmd:
	MOV		DPTR, #CR_MTDDAT1_L
	MOVX	@DPTR, A
	MOV		A, #CR_CMD_SET_FREQ_FADE
	MOV		DPTR, #CR_MTDCMD_L
	MOVX	@DPTR, A
	RET
#endif


#ifdef API_CMD_CHANGE_MPEG_CLK
PUBLIC	Api_Command_MpegClk_Normal
PUBLIC	Api_Command_MpegClk_High
/*******************************************************************************
be used to inform MPEG to change clock to 77MHz for play in lowspeed SD/MMC
********************************************************************************/
Api_Command_MpegClk_Normal:
	CALL	_check_cmd_flg
	MOV		A, #00H
	JMP		send_mepg_clk_cmd

Api_Command_MpegClk_High:
	CALL	_check_cmd_flg
	MOV		A, #01H

send_mepg_clk_cmd:
	MOV		DPTR, #CR_MTDDAT1_L
	MOVX	@DPTR, A
	MOV		A, #CR_CMD_CHANGE_MPEG_CLK
	MOV		DPTR, #CR_MTDCMD_L
	MOVX	@DPTR, A
	RET
#endif

#ifdef API_CMD_FILL_OR_DIS_DBUF_OVER

PUBLIC	Api_Cmd_Dbuf0_Filled_ValidData
PUBLIC	Api_Cmd_Dbuf1_Filled_ValidData
PUBLIC	Api_Cmd_Dbuf0_Filled_InvalidData
PUBLIC	Api_Cmd_Dbuf1_Filled_InvalidData
PUBLIC	Api_Cmd_Dbuf0_DisplayFinish
PUBLIC	Api_Cmd_Dbuf1_DisplayFinish
/********************************************************************************
decbuf1 & decbuf0 is filled with new data or display over
********************************************************************************/
Api_Cmd_Dbuf0_Filled_ValidData:
	CALL	_check_cmd_flg
	MOV		A, #00H
	JMP		set_sp1_irq

Api_Cmd_Dbuf1_Filled_ValidData:
	CALL	_check_cmd_flg
	MOV		A, #01H
	JMP		set_sp1_irq

Api_Cmd_Dbuf0_Filled_InvalidData:
	CALL	_check_cmd_flg
	MOV		A, #02H
	JMP		set_sp1_irq

Api_Cmd_Dbuf1_Filled_InvalidData:
	CALL	_check_cmd_flg
	MOV		A, #03H
	JMP		set_sp1_irq
   	
Api_Cmd_Dbuf0_DisplayFinish:
	CALL	_check_cmd_flg
	MOV		A, #80H
	JMP		set_sp1_irq
   	
Api_Cmd_Dbuf1_DisplayFinish:
	CALL	_check_cmd_flg
	MOV		A, #81H
   	
set_sp1_irq:
	MOV		DPTR, #CR_MTDDAT1_L
	MOVX	@DPTR, A
	MOV		DPTR, #CR_MCUIRQ_H
	MOV		A, #IRQ_SPECIAL1
	MOVX	@DPTR, A
	RET
#endif



/********************************************************************************
check irq flag
********************************************************************************/
_check_cmd_flg:
	MOV		MR_WDT, #0FFH
	CLR		C
	MOV		A, uc_mpeg_state
	SUBB	A, #0A5H
	JZ		mpeg_error
	MOV		DPTR, #CR_MCUIRQ_H
	MOVX	A, @DPTR
	JNZ		_check_cmd_flg
mpeg_error:
	RET
	
#endif
	END