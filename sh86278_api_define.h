/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: sh86278_subroutine_macro.h
Creator: zj.zong					Date:2009-04-22
Description: define part of subroutine as MACRO
Others:
Version: V0.1
History:
	V0.1	2009-04-22		zj.zong
			1. build this module
********************************************************************************/
#ifndef sh86278_SUBROUTINE_MACRO
#define sh86278_SUBROUTINE_MACRO

#define mMPEG_Exit_Rest				MR_MEMSEL4 |= 0x01; CR_MPEGIRQ_H = 0; CR_MPEGIRQ_L = 0; CR_MPEGIRQEN_H = 0xff; CR_MPEGIRQEN_L = 0xff; MR_MPEGRST = 0x5a;
#define mMPEG_Enter_Rest			MR_MPEGRST = 0xa5; MR_MEMSEL4 &= 0xfc;
#define mMPEG_Disable				MR_MPEGRST = 0xa5; Api_Delay_1ms(1); MR_MEMSEL4 = 0x00; MR_MODEN2 &= 0xcf;	

#define mMPEG_Occupy_Dbuf0			MR_MEMSEL4 |= 0x04;	
#define mMPEG_Occupy_Dbuf1			MR_MEMSEL4 |= 0x10;
#define mMPEG_Occupy_Cbuf			MR_MEMSEL4 |= 0x01;	
#define mMPEG_Occupy_Dbuf1Cbuf		MR_MEMSEL4 |= 0x11;	
#define mMPEG_Occupy_DbufCbuf		MR_MEMSEL4 |= 0x15;	
#define mMPEG_Occupy_Cpm			MR_MEMSEL4 |= 0x40;		
#define mMPEG_Occupy_CbufCpm		MR_MEMSEL4 |= 0x41;		

#define mMPEG_Release_Dbuf0			while(MR_MEMSEL4 & 0x0c){MR_MEMSEL4 &= 0xf3;}	
#define mMPEG_Release_Dbuf1			while(MR_MEMSEL4 & 0x30){MR_MEMSEL4 &= 0xcf;}
#define mMPEG_Release_Dbuf			while(MR_MEMSEL4 & 0x3c){MR_MEMSEL4 &= 0xc3;}	
#define mMPEG_Release_Cbuf			MR_MEMSEL4 &= 0xc0;	
#define mMPEG_Release_Cpm			MR_MEMSEL4 &= 0x3f;	
	
#define mLCM_Occupy_Dbuf0			MR_MEMSEL2 &= 0xf8; MR_MEMSEL2 |= 0x02;	
#define mLCM_Occupy_Dbuf1			MR_MEMSEL2 &= 0xf8; MR_MEMSEL2 |= 0x03;	
#define mLCM_Release_Buf			while(MR_MEMSEL2 & 0x07){MR_MEMSEL2 &= 0xf8;}

#define mSet_MCUClock_1M			MR_SYSCLK &= 0xf8;
#define mSet_MCUClock_12M			MR_SYSCLK = MR_SYSCLK & 0xf8 | 0x01;
#define mSet_MCUClock_22M			MR_SYSCLK = MR_SYSCLK & 0xf8 | 0x02;
#define mSet_MCUClock_33M			MR_SYSCLK = MR_SYSCLK & 0xf8 | 0x03;
#define mSet_MCUClock_38M			MR_SYSCLK = MR_SYSCLK & 0xf8 | 0x04;

#define mReset_WatchDog				MR_WDT = 0xff;
#define mReset_PC_Stack				SP = STACK - 1;	
#endif