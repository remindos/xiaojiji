/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: sh86278_sfr_reg.h
Creator: zj.zong					Date: 2009-04-10
Description: MCU Register List for "*.c"
Others: 
Version: V0.1
History:
	
	V0.1	2009-03-25		zj.zong
		 	1. Original.
********************************************************************************/
#ifndef _REG_C_H_
#define _REG_C_H_

sfr SP   		=0x81;
sfr DPL0		=0x82;
sfr DPH0		=0x83;
sfr PCON		=0x87;
sfr MR_IE0		=0xa8;
sfr MR_IE1		=0xb1;
sfr IPH1		=0xb2;
sfr IPL1		=0xb3;
sfr IPH0		=0xb7;
sfr IPL0		=0xb8;
sfr PSW			=0xd0;
sfr ACC			=0xe0;
sfr	B			=0xf0;
sfr FPGMCTL_ID	=0xf9;
sfr WTST		=0xfa;

/*=====================================================
;<<<<<<<<<<<<<<<          SFR          >>>>>>>>>>>>>>>
;=====================================================*/

/*--------------------------------------
;Regulator
;--------------------------------------*/
sfr	MR_LDOOUT		=0x80;		


/*--------------------------------------
;System
;--------------------------------------*/
sfr	MR_BSCON		=0x84;
sfr	MR_SYSCLK	  	=0x88;
sfr	MR_MPEGRST   	=0x89;
sfr	MR_MODEN1   	=0x8A;
sfr	MR_MODEN2   	=0x8B;
sfr	MR_IOCON		=0x8C;
sfr	MR_WDT      	=0x8D;
sfr	MR_RESET    	=0x8E;
sfr	MR_PMUCON   	=0x8F;
sfr	MR_CLKOUT		=0xB9;


/*--------------------------------------
;SFC
;--------------------------------------*/
sfr	MR_SFCCNT		=0x85;
sfr	MR_SFCCR0		=0x90;
sfr	MR_SFCCR1		=0x91;
sfr	MR_SFCCR2		=0x92;
sfr	MR_SFCCR3		=0x93;
sfr	MR_SFCCR4		=0x94;
sfr	MR_SFCCR5		=0x95;
sfr	MR_SFCCR6		=0x96;
sfr	MR_SFCCR7		=0x97;
sfr	MR_SFCCR8		=0x98;
sfr	MR_SFCCR9		=0x99;
sfr	MR_SFCCR10		=0x9A;
sfr	MR_SFCCR11		=0x9B;
sfr	MR_SFCCR12		=0x9C;
sfr	MR_SFCCR13		=0x9D;
sfr	MR_SFCCR14		=0x9E;
sfr	MR_SFCCR15		=0x9F;
sfr	MR_SFCCON1		=0xA9;
sfr	MR_SFCCON2		=0xAA;
sfr	MR_SFCIMAL		=0xAB;
sfr	MR_SFCIMAH		=0xAC;
sfr	MR_SFCPCL	   	=0xAD;
sfr	MR_SFCPCH	   	=0xAE;
sfr	MR_SFCTIME		=0xAF;
sfr	MR_ECCCON		=0xB0;

/*--------------------------------------
;Memory
;--------------------------------------*/
sfr MR_MEMSEL1		=0xa1;
sfr MR_MEMSEL2		=0xa2;
sfr MR_MEMSEL3		=0xa3;
sfr MR_MEMSEL4		=0xa4;
sfr MR_MEMSTA1		=0xa5;
sfr MR_MEMSTA2		=0xa6;
sfr MR_MEMSTA3		=0xa7;


/*--------------------------------------
;Timer
;--------------------------------------*/
sfr MR_TMCON		=0xb4;
sfr MR_TMPLCL		=0xb5;
sfr MR_TMPLCH		=0xb6;


/*;--------------------------------------
;USB
;--------------------------------------*/
sfr	MR_USBCON       =0xBA;
sfr	MR_USBMAINL     =0xBB;
sfr	MR_USBMAINH     =0xBC;
sfr	MR_USBINTMSKL   =0xBD;
sfr	MR_USBINTMSKH   =0xBE;
sfr	MR_USBINTSORL   =0xBF;
sfr	MR_USBINTSORH   =0xC0;
sfr	MR_USBRSTG      =0xC1;
sfr	MR_IEP0CON      =0xC2;
sfr	MR_OEP0CON      =0xC3;
sfr	MR_IEP1ADDR     =0xC4;
sfr	MR_IEP1CON      =0xC5;
sfr	MR_OEP2ADDR     =0xC6;
sfr	MR_OEP2CON      =0xC7;
sfr	MR_IEP0CNT      =0xC8;
sfr	MR_OEP0CNT      =0xC9;
sfr	MR_IEP1CNTL     =0xCA;
sfr	MR_IEP1CNTH     =0xCB;
sfr	MR_OEP2CNTL     =0xCC;
sfr	MR_OEP2CNTH     =0xCD;


/*--------------------------------------
;ADC
;--------------------------------------*/
sfr MR_ADCCON		=0xce;
sfr MR_ADCGAIN		=0xcf;
sfr MR_ADCDATH		=0xd1;
sfr MR_ADCDATL		=0xd2;


/*--------------------------------------
;GPIO
;--------------------------------------*/
sfr	MR_PATYPE       =0xD3;
sfr	MR_PADATA       =0xD4;
sfr	MR_PAIEN        =0xD5;
sfr	MR_PAIRQ        =0xD6;
sfr	MR_PAPEN        =0xD7;
sfr	MR_PAPULL       =0xD8;
sfr MR_PBTYPE		=0xd9;
sfr MR_PBDATA		=0xda;
sfr MR_PBPEN		=0xdb;
sfr MR_PBPULL		=0xdc;
sfr	MR_PCTYPE       =0xDD;
sfr	MR_PCDATA       =0xDE;


/*--------------------------------------
;LCMI
;--------------------------------------*/
sfr MR_LCMCON		=0xe1;
sfr MR_LDMDAT		=0xe2;
sfr MR_LCMIMAL		=0xf1;
sfr MR_LCMIMAH		=0xf2;
sfr MR_LCMNUML		=0xf3;
sfr MR_LCMNUMH		=0xf4;
sfr MR_LCMTIME1		=0xf5;
sfr MR_LCMTIME2		=0xf6;
sfr MR_LCMTIME3		=0xf7;
sfr MR_LCMTIME4		=0xf8;


/*--------------------------------------
;MSI
;--------------------------------------*/
sfr MR_MSSTA		=0xdf;
sfr MR_MSIMAL		=0xe5;
sfr MR_MSIMAH		=0xe6;
sfr MR_MSCON1		=0xe7;
sfr MR_MSCON2		=0xe8;
sfr MR_MSCON3		=0xe9;
sfr MR_MSCON4		=0xea;
sfr MR_MSCMD		=0xeb;
sfr MR_MSARGU1		=0xec;
sfr MR_MSARGU2		=0xed;
sfr MR_MSARGU3		=0xee;
sfr MR_MSARGU4		=0xef;

#endif