#ifndef _MG876051H
#define _MG876051H 1
/********************************************************************/
#ifndef CRYSTALFREQ
 #ifndef _EM6T
  #define CRYSTALFREQ 22118400
 #else
  #define CRYSTALFREQ 44236800
 #endif
#endif
#ifndef INSFREQ
 #define INSFREQ (CRYSTALFREQ/12)
#endif
#ifndef TIMERREQ
 #define TIMERFREQ (CRYSTALFREQ/12)
#endif
/********************************************************************/
#ifndef _MAXUBUAD
 #define _MAXUBUAD CRYSTALFREQ/192
#endif
#define _B57600  (256-(_MAXUBUAD/57600))
#define _B38400  (256-(_MAXUBUAD/38400))
#define _B19200  (256-(_MAXUBUAD/19200))
#define _B9600   (256-(_MAXUBUAD/9600))
#define _B4800   (256-(_MAXUBUAD/4800))
#define _B2400   (256-(_MAXUBUAD/2400))
/********************************************************************/
#ifdef _UseT0SpiltLongTimer
extern unsigned char _tsc;
extern unsigned char _osc;
extern bit _msf;
extern bit _hsf;
extern bit _tsf;
extern bit _osf;

/********************************************************************/
#ifdef _T0_BaseUnderMs 
 extern unsigned char _hsc;
 extern bit _msDiv;
 #define __FREQBASE__ (65536-((TIMERFREQ/1000)-14))
 #define __VTH (__FREQBASE__/256)
 #define __VTL (__FREQBASE__%256)
 #define _HSdv 9
#else
/********************************************************************/
 #define __FREQBASE__ (65536-((TIMERFREQ/100)-10))
 #define __VTH (__FREQBASE__/256)
 #define __VTL (__FREQBASE__%256)
#endif
/********************************************************************/
 #define _TSdv 9
 #define _OSdv 9
 #define init876051() CKCON=0;CKCON2&=0xc0;CKCON2|=((CRYSTALFREQ/1000000)-1);\
 PCON|=0x80;TMOD=0x21;TCON=0;TH1=_B19200;SCON=0x52;TH0=__VTH;TL0=__VTL;PT0=1;\
 IE=0x82;TCON=0x50
#else
#define init876051() CKCON=0;CKCON2&=0xc0;CKCON2|=((CRYSTALFREQ/1000000)-1);\
 PCON|=0x80;TMOD=0x21;TCON=0;TH1=_B19200;SCON=0x52;PT0=1;IE=0;TCON=0x40
#endif
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/


/* for Megawin MG87FL(E)2051/4051/6051 */
/*-----------------------------------------------------------------------------
Standard SFRs
-----------------------------------------------------------------------------*/

/* 8051 Registers */

sfr P1     = 0x90;
sfr P3     = 0xB0;
sfr PSW    = 0xD0;
sfr ACC    = 0xE0;
sfr B      = 0xF0;
sfr SP     = 0x81;
sfr DPL    = 0x82;
sfr DPH    = 0x83;
sfr PCON   = 0x87;
sfr TCON   = 0x88;
sfr TMOD   = 0x89;
sfr TL0    = 0x8A;
sfr TL1    = 0x8B;
sfr TH0    = 0x8C;
sfr TH1    = 0x8D;
sfr IE     = 0xA8;
sfr IP     = 0xB8;
sfr SCON   = 0x98;
sfr SBUF   = 0x99;

// (PSW)
sbit CY    = PSW^7;
sbit AC    = PSW^6;
sbit F0    = PSW^5;
sbit RS1   = PSW^4;
sbit RS0   = PSW^3;
sbit OV    = PSW^2;
sbit P     = PSW^0;

// (TCON)
sbit TF1   = TCON^7;
sbit TR1   = TCON^6;
sbit TF0   = TCON^5;
sbit TR0   = TCON^4;
sbit IE1   = TCON^3;
sbit IT1   = TCON^2;
sbit IE0   = TCON^1;
sbit IT0   = TCON^0;

// (IE)
sbit EA    = IE^7;
sbit EAC   = IE^6;
sbit ES    = IE^4;
sbit ET1   = IE^3;
sbit EX1   = IE^2;
sbit ET0   = IE^1;
sbit EX0   = IE^0;

// (IP)
sbit PAC   = IP^6;
sbit PS    = IP^4;
sbit PT1   = IP^3;
sbit PX1   = IP^2;
sbit PT0   = IP^1;
sbit PX0   = IP^0;

// (SCON)
sbit SM0   = SCON^7;
sbit SM1   = SCON^6;
sbit SM2   = SCON^5;
sbit REN   = SCON^4;
sbit TB8   = SCON^3;
sbit RB8   = SCON^2;
sbit TI    = SCON^1;
sbit RI    = SCON^0;

// (P3)
sbit P37   = P3^7;
sbit P35   = P3^5;
sbit P34   = P3^4;
sbit P33   = P3^3;
sbit P32   = P3^2;
sbit P31   = P3^1;
sbit P30   = P3^0;

// (P1)
sbit P17   = P1^7;
sbit P16   = P1^6;
sbit P15   = P1^5;
sbit P14   = P1^4;
sbit P13   = P1^3;
sbit P12   = P1^2;
sbit P11   = P1^1;
sbit P10   = P1^0;

// (P3 alternate)
sbit RD    = P3^7;
sbit T1    = P3^5;
sbit T0    = P3^4;
sbit INT1  = P3^3;
sbit INT0  = P3^2;
sbit TXD   = P3^1;
sbit RXD   = P3^0;

/*-----------------------------------------------------------------------------
New-defined SFRs
-----------------------------------------------------------------------------*/

/* MG87FL(E)2051/4051/6051 Extensions */

sfr AUXR   = 0x8E;
sfr SADDR  = 0xA9;
sfr SADEN  = 0xB9;
sfr IPH    = 0xB7;
sfr WDTCR  = 0xE1;
sfr AUXR1  = 0xA2;
sfr XICON  = 0xC0;
sfr P4     = 0xE8;
sfr CKCON  = 0xC7;
sfr CKCON2 = 0xBF;
sfr CKCON3 = 0x8F;
sfr ACSR   = 0x97;
sfr P3WKPE = 0xD6;
sfr P1WKPE = 0xD7;
sfr CCON   = 0xD8;
sfr CMOD   = 0xD9;
sfr CCAP0L = 0xEA;
sfr CCAP0H = 0xFA;

// for ISP
sfr IFD    = 0xE2;
sfr IFADRH = 0xE3;
sfr IFADRL = 0xE4;
sfr IFMT   = 0xE5;
sfr SCMD   = 0xE6;
sfr ISPCR  = 0xE7;

// (SCON)
sbit FE    = SCON^7;

// (XICON)
sbit PTC   = XICON^7;
sbit PX3   = XICON^7;
sbit EX3   = XICON^6;
sbit IE3   = XICON^5;
sbit IT3   = XICON^4;
sbit PX2   = XICON^3;
sbit EX2   = XICON^2;
sbit IE2   = XICON^1;
sbit IT2   = XICON^0;

// (CCON)
sbit CF    = CCON^7;
sbit CR    = CCON^6;

// (P4)
sbit P43   = P4^3;
sbit P42   = P4^2;

// (P4 alternate)
sbit INT2  = P4^3;
sbit INT3  = P4^2;

#define intINT0		0
#define intT0		1
#define intINT1		2
#define intT1		3
#define intUART1	4
#define intUART 	4
#define intINT2		6
#define intINT3		7
#define intPWM		7
/********************************************************************/
#ifndef clrwdt
#define clrwdt() 	WDTCR|=0x10
#endif
/********************************************************************/
#ifndef swReset
#define swReset() 	ISPCR|=0x20
#endif
#define JumpToISP()	ISPCR=0x40;ISPCR=0x60
#define JumpToAP()	ISPCR=0;ISPCR=0x20
/********************************************************************/
#define _IAPLOC		6144
#define _xIAPLOC	0x1800
/**** IAP 0x1800 ~ 0x1bff / 6144 ~ 7167 *****************************/
#define IAP_enable()  ISPCR|=0x80
#define IAP_disable() ISPCR&=0x7f
#define _ISP_trigger() clrwdt();SCMD=0x46;SCMD=0xb9;clrwdt()
#define _ISP_PutAddr(a) IFADRH=a>>8;IFADRL=a
#define _ISP_PutByte(d) IFD=d
#define _ISP_GetByte()	IFD
#define	_ISP_Standby()	IFMT=0
#define	_ISP_Read()	IFMT=1
#define	_ISP_Write()	IFMT=2
#define	_ISP_Erase()	IFMT=3
#define	_ISP_LBRead()	IFMT=4
#define	_ISP_LBWrite()	IFMT=5
#define	_ISP_Success()	((ISPCR&0x10)==0)
 #ifdef _USINGISP
  extern unsigned char IAP_page_erase(unsigned int page_address);
  extern unsigned char IAP_byte_program(unsigned int byte_address, unsigned char dta);
  extern unsigned char IAP_byte_read(unsigned int byte_address);
 #endif
#endif