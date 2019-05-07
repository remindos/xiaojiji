/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: sh86278_sfr_reg.inc
Creator: zj.Zong					Date: 2009-04-10
Description: MCU Register List for "*.asm"
Others: 
Version: V0.1
History:
	
	V0.1	2009-03-25		zj.Zong
		 	1. Original.
********************************************************************************/
#ifndef _REG_ASM_H_
#define _REG_ASM_H_

MR_IE0		EQU	0a8H
MR_IE1		EQU	0b1H
;=====================================================
;<<<<<<<<<<<<<<<          SFR          >>>>>>>>>>>>>>>
;=====================================================

;--------------------------------------
;Regulator
;--------------------------------------
MR_LDOOUT		EQU	80H		


;--------------------------------------
;System
;--------------------------------------
MR_BSCON		EQU	84H
MR_SYSCLK		EQU	88H
MR_MPEGRST		EQU	89H
MR_MODEN1		EQU	8AH
MR_MODEN2		EQU	8BH
MR_IOCON		EQU	8CH
MR_WDT			EQU	8DH
MR_RESET		EQU	8EH
MR_PMUCON		EQU	8FH
MR_CLKOUT		EQU	0B9H


;--------------------------------------
;SFC
;--------------------------------------
MR_SFCCNT		EQU	85H
MR_SFCCR0		EQU	90H
MR_SFCCR1		EQU	91H
MR_SFCCR2		EQU	92H
MR_SFCCR3		EQU	93H
MR_SFCCR4		EQU	94H
MR_SFCCR5		EQU	95H
MR_SFCCR6		EQU	96H
MR_SFCCR7		EQU	97H
MR_SFCCR8		EQU	98H
MR_SFCCR9		EQU	99H
MR_SFCCR10		EQU	9AH
MR_SFCCR11		EQU	9BH
MR_SFCCR12		EQU	9CH
MR_SFCCR13		EQU	9DH
MR_SFCCR14		EQU	9EH
MR_SFCCR15		EQU	9FH
MR_SFCCON1		EQU	0A9H
MR_SFCCON2		EQU	0AAH
MR_SFCIMAL		EQU	0ABH
MR_SFCIMAH		EQU	0ACH
MR_SFCPCL		EQU	0ADH
MR_SFCPCH		EQU	0AEH
MR_SFCTIME		EQU	0AFH
MR_ECCCON		EQU	0B0H

;--------------------------------------
;Memory
;--------------------------------------
MR_MEMSEL1		EQU	0A1H
MR_MEMSEL2		EQU	0A2H
MR_MEMSEL3		EQU	0A3H
MR_MEMSEL4		EQU	0A4H
MR_MEMSTA1		EQU	0A5H
MR_MEMSTA2		EQU	0A6H
MR_MEMSTA3		EQU	0A7H


;--------------------------------------
;Timer
;--------------------------------------
MR_TMCON		EQU	0B4H
MR_TMPLCL		EQU	0B5H
MR_TMPLCH		EQU	0B6H


;--------------------------------------
;USB
;--------------------------------------
MR_USBCON		EQU	0BAH
MR_USBMAINL		EQU	0BBH
MR_USBMAINH		EQU	0BCH
MR_USBINTMSKL	EQU	0BDH
MR_USBINTMSKH	EQU	0BEH
MR_USBINTSORL	EQU	0BFH
MR_USBINTSORH	EQU	0C0H
MR_USBRSTG		EQU	0C1H
MR_IEP0CON		EQU	0C2H
MR_OEP0CON		EQU	0C3H
MR_IEP1ADDR		EQU	0C4H
MR_IEP1CON		EQU	0C5H
MR_OEP2ADDR		EQU	0C6H
MR_OEP2CON		EQU	0C7H
MR_IEP0CNT		EQU	0C8H
MR_OEP0CNT		EQU	0C9H
MR_IEP1CNTL		EQU	0CAH
MR_IEP1CNTH		EQU	0CBH
MR_OEP2CNTL		EQU	0CCH
MR_OEP2CNTH		EQU	0CDH


;--------------------------------------
;ADC
;--------------------------------------
MR_ADCCON		EQU	0CEH
MR_ADCGAIN		EQU	0CFH
MR_ADCDATH		EQU	0D1H
MR_ADCDATL		EQU	0D2H


;--------------------------------------
;GPIO
;--------------------------------------
MR_PATYPE		EQU	0D3H
MR_PADATA		EQU	0D4H
MR_PAIEN		EQU	0D5H
MR_PAIRQ		EQU	0D6H
MR_PAPEN		EQU	0D7H
MR_PAPULL		EQU	0D8H
MR_PBTYPE		EQU	0D9H
MR_PBDATA		EQU	0DAH
MR_PBPEN		EQU	0DBH
MR_PBPULL		EQU	0DCH
MR_PCTYPE		EQU	0DDH
MR_PCDATA		EQU	0DEH


;--------------------------------------
;LCMI
;--------------------------------------
MR_LCMCON		EQU	0E1H
MR_LDMDAT		EQU	0E2H
MR_LCMIMAL		EQU	0F1H
MR_LCMIMAH		EQU	0F2H
MR_LCMNUML		EQU	0F3H
MR_LCMNUMH		EQU	0F4H
MR_LCMTIME1		EQU	0F5H
MR_LCMTIME2		EQU	0F6H
MR_LCMTIME3		EQU	0F7H
MR_LCMTIME4		EQU	0F8H


;--------------------------------------
;MSI
;--------------------------------------
MR_MSSTA		EQU	0DFH
MR_MSIMAL		EQU	0E5H
MR_MSIMAH		EQU	0E6H
MR_MSCON1		EQU	0E7H
MR_MSCON2		EQU	0E8H
MR_MSCON3		EQU	0E9H
MR_MSCON4		EQU	0EAH
MR_MSCMD		EQU	0EBH
MR_MSARGU1		EQU	0ECH
MR_MSARGU2		EQU	0EDH
MR_MSARGU3		EQU	0EEH
MR_MSARGU4		EQU	0EFH


;=====================================================
;<<<<<<<<<<<<<<<          ERA          >>>>>>>>>>>>>>>
;=====================================================
;--------------------------------------
;USB
;--------------------------------------
MR_IEP0DAT1				EQU	7C00H
MR_IEP0DAT2             EQU	7C01H
MR_IEP0DAT3             EQU	7C02H
MR_IEP0DAT4             EQU	7C03H
MR_IEP0DAT5             EQU	7C04H
MR_IEP0DAT6             EQU	7C05H
MR_IEP0DAT7             EQU	7C06H
MR_IEP0DAT8             EQU	7C07H
MR_IEP0DAT9             EQU	7C08H
MR_IEP0DAT10            EQU	7C09H
MR_IEP0DAT11            EQU	7C0AH
MR_IEP0DAT12            EQU	7C0BH
MR_IEP0DAT13            EQU	7C0CH
MR_IEP0DAT14            EQU	7C0DH
MR_IEP0DAT15            EQU	7C0EH
MR_IEP0DAT16            EQU	7C0FH
MR_IEP0DAT17            EQU	7C10H
MR_IEP0DAT18            EQU	7C11H
MR_IEP0DAT19            EQU	7C12H
MR_IEP0DAT20            EQU	7C13H
MR_IEP0DAT21            EQU	7C14H
MR_IEP0DAT22            EQU	7C15H
MR_IEP0DAT23            EQU	7C16H
MR_IEP0DAT24            EQU	7C17H
MR_IEP0DAT25            EQU	7C18H
MR_IEP0DAT26            EQU	7C19H
MR_IEP0DAT27            EQU	7C1AH
MR_IEP0DAT28            EQU	7C1BH
MR_IEP0DAT29            EQU	7C1CH
MR_IEP0DAT30            EQU	7C1DH
MR_IEP0DAT31            EQU	7C1EH
MR_IEP0DAT32            EQU	7C1FH
MR_IEP0DAT33            EQU	7C20H
MR_IEP0DAT34            EQU	7C21H
MR_IEP0DAT35            EQU	7C22H
MR_IEP0DAT36            EQU	7C23H
MR_IEP0DAT37            EQU	7C24H
MR_IEP0DAT38            EQU	7C25H
MR_IEP0DAT39            EQU	7C26H
MR_IEP0DAT40            EQU	7C27H
MR_IEP0DAT41            EQU	7C28H
MR_IEP0DAT42            EQU	7C29H
MR_IEP0DAT43            EQU	7C2AH
MR_IEP0DAT44            EQU	7C2BH
MR_IEP0DAT45            EQU	7C2CH
MR_IEP0DAT46            EQU	7C2DH
MR_IEP0DAT47            EQU	7C2EH
MR_IEP0DAT48            EQU	7C2FH
MR_IEP0DAT49            EQU	7C30H
MR_IEP0DAT50            EQU	7C31H
MR_IEP0DAT51            EQU	7C32H
MR_IEP0DAT52            EQU	7C33H
MR_IEP0DAT53            EQU	7C34H
MR_IEP0DAT54            EQU	7C35H
MR_IEP0DAT55            EQU	7C36H
MR_IEP0DAT56            EQU	7C37H
MR_IEP0DAT57            EQU	7C38H
MR_IEP0DAT58            EQU	7C39H
MR_IEP0DAT59            EQU	7C3AH
MR_IEP0DAT60            EQU	7C3BH
MR_IEP0DAT61            EQU	7C3CH
MR_IEP0DAT62            EQU	7C3DH
MR_IEP0DAT63            EQU	7C3EH
MR_IEP0DAT64            EQU	7C3FH
MR_OEP0DAT1             EQU	7C40H
MR_OEP0DAT2             EQU	7C41H
MR_OEP0DAT3             EQU	7C42H
MR_OEP0DAT4             EQU	7C43H
MR_OEP0DAT5             EQU	7C44H
MR_OEP0DAT6             EQU	7C45H
MR_OEP0DAT7             EQU	7C46H
MR_OEP0DAT8             EQU	7C47H
MR_OEP0DAT9             EQU	7C48H
MR_OEP0DAT10            EQU	7C49H
MR_OEP0DAT11            EQU	7C4AH
MR_OEP0DAT12            EQU	7C4BH
MR_OEP0DAT13            EQU	7C4CH
MR_OEP0DAT14            EQU	7C4DH
MR_OEP0DAT15            EQU	7C4EH
MR_OEP0DAT16            EQU	7C4FH
MR_OEP0DAT17            EQU	7C50H
MR_OEP0DAT18            EQU	7C51H
MR_OEP0DAT19            EQU	7C52H
MR_OEP0DAT20            EQU	7C53H
MR_OEP0DAT21            EQU	7C54H
MR_OEP0DAT22            EQU	7C55H
MR_OEP0DAT23            EQU	7C56H
MR_OEP0DAT24            EQU	7C57H
MR_OEP0DAT25            EQU	7C58H
MR_OEP0DAT26            EQU	7C59H
MR_OEP0DAT27            EQU	7C5AH
MR_OEP0DAT28            EQU	7C5BH
MR_OEP0DAT29            EQU	7C5CH
MR_OEP0DAT30            EQU	7C5DH
MR_OEP0DAT31            EQU	7C5EH
MR_OEP0DAT32            EQU	7C5FH
MR_OEP0DAT33            EQU	7C60H
MR_OEP0DAT34            EQU	7C61H
MR_OEP0DAT35            EQU	7C62H
MR_OEP0DAT36            EQU	7C63H
MR_OEP0DAT37            EQU	7C64H
MR_OEP0DAT38            EQU	7C65H
MR_OEP0DAT39            EQU	7C66H
MR_OEP0DAT40            EQU	7C67H
MR_OEP0DAT41            EQU	7C68H
MR_OEP0DAT42            EQU	7C69H
MR_OEP0DAT43            EQU	7C6AH
MR_OEP0DAT44            EQU	7C6BH
MR_OEP0DAT45            EQU	7C6CH
MR_OEP0DAT46            EQU	7C6DH
MR_OEP0DAT47            EQU	7C6EH
MR_OEP0DAT48            EQU	7C6FH
MR_OEP0DAT49            EQU	7C70H
MR_OEP0DAT50            EQU	7C71H
MR_OEP0DAT51            EQU	7C72H
MR_OEP0DAT52            EQU	7C73H
MR_OEP0DAT53            EQU	7C74H
MR_OEP0DAT54            EQU	7C75H
MR_OEP0DAT55            EQU	7C76H
MR_OEP0DAT56            EQU	7C77H
MR_OEP0DAT57            EQU	7C78H
MR_OEP0DAT58            EQU	7C79H
MR_OEP0DAT59            EQU	7C7AH
MR_OEP0DAT60            EQU	7C7BH
MR_OEP0DAT61            EQU	7C7CH
MR_OEP0DAT62            EQU	7C7DH
MR_OEP0DAT63            EQU	7C7EH
MR_OEP0DAT64            EQU	7C7FH


;--------------------------------------
;SFC
;--------------------------------------
MR_ECC1					EQU	7D00H	
MR_ECC2                 EQU	7D01H	
MR_ECC3                 EQU	7D02H	
MR_ECC4                 EQU	7D03H	
MR_ECC5                 EQU	7D04H	
MR_ECC6                 EQU	7D05H	
MR_ECC7                 EQU	7D06H	
MR_ECC8                 EQU	7D07H	
MR_ECC9                 EQU	7D08H	
MR_ECC10                EQU	7D09H	
MR_ECC11                EQU	7D0AH	
MR_ECC12                EQU	7D0BH	
MR_ECC13                EQU	7D0CH	
MR_ECC14                EQU	7D0DH	
MR_ECC15                EQU	7D0EH	
MR_ECC16                EQU	7D0FH	
MR_ECC17                EQU	7D10H	
MR_ECC18                EQU	7D11H	
MR_ECC19                EQU	7D12H	
MR_ECC20                EQU	7D13H	
MR_ECC21                EQU	7D14H	
MR_ECC22                EQU	7D15H	
MR_ECC23                EQU	7D16H	
MR_ECC24                EQU	7D17H	
MR_ECC25                EQU	7D18H	
MR_ECC26                EQU	7D19H
MR_ECC27				EQU	7D1AH
MR_ECC28				EQU	7D1BH
MR_ECC29				EQU	7D1CH
MR_ECC30				EQU	7D1DH
MR_ECC31				EQU	7D1EH
MR_ECC32				EQU	7D1FH
MR_ECC33				EQU	7D20H
MR_ECC34				EQU	7D21H
MR_ECC35				EQU	7D22H
MR_ECC36				EQU	7D23H
MR_ECC37				EQU	7D24H
MR_ECC38				EQU	7D25H
MR_ECC39				EQU	7D26H
MR_ECC40				EQU	7D27H
MR_ECC41				EQU	7D28H
MR_ECC42				EQU	7D29H
MR_ECC43				EQU	7D2AH
MR_ECC44				EQU	7D2BH
MR_ECC45				EQU	7D2CH
MR_ECC46				EQU	7D2DH
MR_ECC47				EQU	7D2EH
MR_ECC48				EQU	7D2FH
MR_ECC49				EQU	7D30H
MR_ECC50				EQU	7D31H
MR_ECC51				EQU	7D32H
MR_ECC52				EQU	7D33H
MR_ECC53				EQU	7D34H
	
MR_ERR1L                EQU	7D40H	
MR_ERR1H                EQU	7D41H	
MR_ERR1B                EQU	7D42H	
MR_ERR2L                EQU	7D43H	
MR_ERR2H                EQU	7D44H	
MR_ERR2B                EQU	7D45H	
MR_ERR3L                EQU	7D46H	
MR_ERR3H                EQU	7D47H	
MR_ERR3B                EQU	7D48H	
MR_ERR4L                EQU	7D49H	
MR_ERR4H                EQU	7D4AH	
MR_ERR4B                EQU	7D4BH	
MR_ERR5L                EQU	7D4CH	
MR_ERR5H                EQU	7D4DH	
MR_ERR5B                EQU	7D4EH	
MR_ERR6L                EQU	7D4FH	
MR_ERR6H                EQU	7D50H	
MR_ERR6B                EQU	7D51H	
MR_ERR7L                EQU	7D52H	
MR_ERR7H                EQU	7D53H	
MR_ERR7B                EQU	7D54H	
MR_ERR8L                EQU	7D55H	
MR_ERR8H                EQU	7D56H	
MR_ERR8B                EQU	7D57H	
MR_ERR9L                EQU	7D58H	
MR_ERR9H                EQU	7D59H	
MR_ERR9B                EQU	7D5AH	
MR_ERR10L               EQU	7D5BH	
MR_ERR10H               EQU	7D5CH	
MR_ERR10B               EQU	7D5DH	
MR_ERR11L               EQU	7D5EH	
MR_ERR11H               EQU	7D5FH	
MR_ERR11B               EQU	7D60H	
MR_ERR12L               EQU	7D61H	
MR_ERR12H               EQU	7D62H	
MR_ERR12B               EQU	7D63H	
MR_ERR13L               EQU	7D64H	
MR_ERR13H               EQU	7D65H	
MR_ERR13B               EQU	7D66H	
MR_ERR14L               EQU	7D67H	
MR_ERR14H               EQU	7D68H	
MR_ERR14B               EQU	7D69H	
MR_ERR15L               EQU	7D6AH	
MR_ERR15H               EQU	7D6BH	
MR_ERR15B               EQU	7D6CH	
MR_ERR16L               EQU	7D6DH	
MR_ERR16H               EQU	7D6EH	
MR_ERR16B               EQU	7D6FH
MR_ERR17L               EQU	7D70H	
MR_ERR17H               EQU	7D71H	
MR_ERR17B               EQU	7D72H
MR_ERR18L               EQU	7D73H	
MR_ERR18H               EQU	7D74H	
MR_ERR18B               EQU	7D75H
MR_ERR19L               EQU	7D76H	
MR_ERR19H               EQU	7D77H	
MR_ERR19B               EQU	7D78H
MR_ERR20L               EQU	7D79H	
MR_ERR20H               EQU	7D7AH	
MR_ERR20B               EQU	7D7BH
MR_ERR21L               EQU	7D7CH	
MR_ERR21H               EQU	7D7DH	
MR_ERR21B               EQU	7D7EH
MR_ERR22L               EQU	7D7FH	
MR_ERR22H               EQU	7D80H	
MR_ERR22B               EQU	7D81H
MR_ERR23L               EQU	7D82H	
MR_ERR23H               EQU	7D83H	
MR_ERR23B               EQU	7D84H
MR_ERR24L               EQU	7D85H	
MR_ERR24H               EQU	7D86H	
MR_ERR24B               EQU	7D87H
MR_ERR25L               EQU	7D88H	
MR_ERR25H               EQU	7D89H	
MR_ERR25B               EQU	7D8AH
MR_ERR26L               EQU	7D8BH	
MR_ERR26H               EQU	7D8CH	
MR_ERR26B               EQU	7D8DH
MR_ERR27L               EQU	7D8EH	
MR_ERR27H               EQU	7D8FH	
MR_ERR27B               EQU	7D90H
MR_ERR28L               EQU	7D91H	
MR_ERR28H               EQU	7D92H	
MR_ERR28B               EQU	7D93H
MR_ERR29L               EQU	7D94H	
MR_ERR29H               EQU	7D95H	
MR_ERR29B               EQU	7D96H
MR_ERR30L               EQU	7D97H	
MR_ERR30H               EQU	7D98H	
MR_ERR30B               EQU	7D99H


;--------------------------------------
;MSI
;--------------------------------------
MR_MSRP1		EQU	7CD0H
MR_MSRP2                EQU	7CD1H
MR_MSRP3                EQU	7CD2H
MR_MSRP4                EQU	7CD3H
MR_MSRP5                EQU	7CD4H
MR_MSRP6                EQU	7CD5H
MR_MSRP7                EQU	7CD6H
MR_MSRP8                EQU	7CD7H
MR_MSRP9                EQU	7CD8H
MR_MSRP10               EQU	7CD9H
MR_MSRP11               EQU	7CDAH
MR_MSRP12               EQU	7CDBH
MR_MSRP13               EQU	7CDCH
MR_MSRP14               EQU	7CDDH
MR_MSRP15               EQU	7CDEH
MR_MSRP16               EQU	7CDFH
MR_MSRP17               EQU	7CE0H
MR_MSTOUTL              EQU	7CE1H
MR_MSTOUTH              EQU	7CE2H


;--------------------------------------
;DAC
;--------------------------------------
MR_DACI					EQU	7CE6H
MR_DACCLK               EQU	7CE7H
MR_DACSRST              EQU	7CE8H
MR_DACPCON              EQU	7CE9H
MR_AVCL                 EQU	7CEAH
MR_AVCR                 EQU	7CEBH
MR_MVCL                 EQU	7CECH
MR_MVCR                 EQU	7CEDH
MR_DACSET               EQU	7CEEH
MR_DACSTA               EQU	7CEFH
MR_DACOPT1              EQU	7CF0H
MR_DACOPT2              EQU	7CF1H
MR_DACOPT3H             EQU	7CF2H
MR_DACOPT3L             EQU	7CF3H
MR_DACOPT4H             EQU	7CF4H
MR_DACOPT4L             EQU	7CF5H
MR_DACOPT5              EQU	7CF6H


;=====================================================
;<<<<<<<<<<<<<<<          CRA          >>>>>>>>>>>>>>>
;=====================================================

CR_MCUIRQEN_H		EQU	7800H	
CR_MCUIRQEN_L		EQU	7801H
CR_MPEGIRQEN_H		EQU	7802H
CR_MPEGIRQEN_L		EQU	7803H
CR_MCUIRQ_H			EQU	7804H
CR_MCUIRQ_L			EQU	7805H
CR_MPEGIRQ_H		EQU	7806H
CR_MPEGIRQ_L		EQU	7807H
CR_MTDCMD_H			EQU	7808H
CR_MTDCMD_L			EQU	7809H
CR_DTMCMD_H			EQU	780AH
CR_DTMCMD_L			EQU	780BH
CR_MTDDAT1_H		EQU	780CH
CR_MTDDAT1_L		EQU	780DH
CR_MTDDAT2_H		EQU	780EH
CR_MTDDAT2_L		EQU	780FH
CR_DTMDAT1_H		EQU	7810H
CR_DTMDAT1_L		EQU	7811H
CR_DTMDAT2_H		EQU	7812H
CR_DTMDAT2_L		EQU	7813H
CR_DTMDAT3_H		EQU	7814H
CR_DTMDAT3_L		EQU	7815H
CR_DTMDAT4_H		EQU	7816H
CR_DTMDAT4_L		EQU	7817H

#endif