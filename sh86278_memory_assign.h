/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: sh86278_memory_assign.h
Creator: zj.zong			Date: 2009-04-10
Description: define memory address(BIT, DATA, IDATA, XDATA)
Others:
Version: V0.1
History:
	V0.1	2009-04-10		zj.zong
			1. build this module
********************************************************************************/
#ifndef SH86278_MEMORY_ASSIGN_DEF
#define SH86278_MEMORY_ASSIGN_DEF


/***************************XDATA***************************/
#define	X_ADDR_CBUF			0x0000
#define X_ADDR_DBUF0		0x4000
#define X_ADDR_DBUF1		0x5000
#define X_ADDR_FBUF			0x6000
#define X_ADDR_IBUF			0x7000

#define X_ADDR_FCOMBUF		0x6600

#define X_ADDR_FBUF_COMMON	0x6080		//for PM1 resident defined Variable
#define X_ADDR_IBUF_COMMON	0x7420		//for each mode defined Variable

#define X_ADDR_FS			0x60D0		//for file system
#define X_ADDR_FS_FATBUF	0x7200		//for file system write file

#define X_ADDR_MSI			0x63D8		//for MSI Driver

#define	X_ADDR_DAC			0x7CE6		//for DAC register
#define	X_ADDR_CRA			0x7800		//for Communication register

#define FCOMBUF_OFFSET_W	(X_ADDR_FCOMBUF-X_ADDR_FBUF)/2	//ef common buf offset is 0x300 word
#define FCOMBUF_OFFSET_B	X_ADDR_FCOMBUF-X_ADDR_FBUF		//ef common buf offset is 0x600 byte

#define RAIT_PARA_BASE		X_ADDR_CBUF	//rait data buffer

#define X_ADDR_REC_ENC_BUF		X_ADDR_CBUF + 0x1800	//0x1800 word
#define REC_ENC_BUF_OFFSET_W	0x1800

/***************************DATA****************************/
#define	D_ADDR_COMMON		08H
#define D_ADDR_FS			30H
#define D_ADDR_SFC			48H


/***************************BIT*****************************/
#define B_ADDR_MSI			25H.0
#define B_ADDR_MSI_MULTI	2AH.3
#define	B_ADDR_COMMON		25H.3
#define	B_ADDR_FS			27H.3


/***************************IDATA***************************/
#define I_ADDR_SFC			080H		//0EH
#define I_ADDR_FS			095H		//08FH		//10H
#define I_ADDR_COMMON		0A5H		//12H
#define	I_ADDR_STACK		0C0H		//0B7H



#endif