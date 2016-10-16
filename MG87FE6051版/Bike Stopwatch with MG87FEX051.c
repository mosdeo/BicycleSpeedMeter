#include <REG_MG87FL2051-6051.H>
#include <stdio.h>


//==== �ŧi DS1821 �M��register ========
#define DQ_IO P37
unsigned char NUMBER[3];
char code TABLE[13]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x20,0x2d,0x2e};//  �ť�- .
char TEMP;
bit bdata work=0;
//==== DS1821.h����include�b�H�W�Uregister����
#include <DS1821.h>
//==== �ŧi DS1821 �M��register ���� ===


//==== �ŧi LCM �M��register ========
#define RS P35
#define RW P34 //0�g1Ū
#define LCD_Enable  P33	//�t�tĲ�o
//�H�U���define�ϥΪ�Port�����ۦP
#define BF   P10
#define LCDP P1
//==== LCM.h����include�b�H�W�Uregister����
#include <LCM.h>
//==== �ŧi LCM �M��register ���� ===


//==== �ŧi �t�ת� �M��register ========
#define MagneticSW P32 		//�e���ϩʶ}��,start/stop ����P3.2(INT0)
/*�ŧiT0�p�ɬ����ŧi*/  // THx TLx �p��Ѧ� 7-9�� 
#define  count_M1   3694		// T0(MODE 1)���p�q��,1mSec @ 44326800Hz,SYSCLK/12
#define  TH_M1  (65636-count_M1)>>8// T0(MODE 1)�p�q��8�줸 
#define  TL_M1  (65636-count_M1)&0x00FF// T0(MODE 1)�p�q�C8�줸
/* �ŧi���ܼ� */
unsigned int  mSecCounter=0;		// �C��Ĳ�o���j(�C��)mSec��
unsigned int  unDisp_mSec=65536;		// ��ܥΪ�mSec��
unsigned int  unDispSpeed=0;			// ��ܥΪ��t�v
unsigned int  unDispMileage=0;		//��ܥΪ��֭p���{
unsigned int  unCycleCounter=0;		// �֭p���,�p�⨽�{��,26in*pi*65535=136km(�̤j�p�{)
void debouncer(void); 		// �ŧi���u�����
//==== �ŧi �t�ת� �M��register���� ========


//==== �}���e�� ========
unsigned char code MSG_Welcome_Line1[]="  NTNU Lin Kao-Yuan ";
unsigned char code MSG_Welcome_Line2[]="Bicycle's MCU System";
unsigned char code MSG_Welcome_Line3[]="     0958889115";
unsigned char code MSG_Welcome_Line4[]="  mosdeo@gmail.com";
//==== �}���e�� ========

unsigned char StringForLCD_Line1[13]="";
unsigned char StringForLCD_Line2[19]="";
unsigned char countMOD=0;	

// void P42XOR(void);
// void GetTempByDS1821(void);
// void TxSpeed(void);
// void TxTempearture(void);
// void putnum(unsigned int inputNum);

#include "function.h"

main()
{
	IE=0x00;
	EA=ET0=EX0=ES=1;
	IP=0x00;
	PT0=1;  // TF0���u��(for mSec�p��)
  SCON=0x70; /*Serial Port mode2*/
  TCON=0x00,// 0000 0000 �]�wINT0 �Ħ��Ĳ�o(6-4��)
	IT0=1;//���i�Φ��Ĳ�o,�_�h��Ӻ��K�g�L���ɶ����|���_
	
	TMOD=0x21;				// 0010 0001,T1��mode 2�BT0��mode 1
  TH0=TH_M1; TL0=TL_M1;	// �]�mT0�p�ƶq��8�줸�B�C8�줸 
	TH1=TL1=250; 
	
	MagneticSW=1;		// �W��MagneticSW��J 
	
	init_LCM();	//���N�M���ù�
	NewType();
	
	write_inst(0x80);write_string(MSG_Welcome_Line1);
	write_inst(0xC0);write_string(MSG_Welcome_Line2);
	write_inst(0x94);write_string(MSG_Welcome_Line3);
	write_inst(0xD4);write_string(MSG_Welcome_Line4);
	delay1ms(3000);
	
	TR1=1;TR0=1;					// �Ұ�T1,T0


	while(1)
	{			
		// Visable WDT
		P43=~P43;
		
		//=== �������p��n�b�M���ù��e�����A���M��Ӧh�ɶ�put char��LCD�A�ù��e���{��
		unDispSpeed=(unsigned int)(7468/unDisp_mSec); //�p��t��
		unDispMileage=(unsigned int)(2.075*unCycleCounter
// 																	unCycleCounter<<1	//*2
// 															   +unCycleCounter>>4	//*0.0625
// 																 +unCycleCounter>>6	//*0.015625
																); //�p��֭p���{,���*2.075
		
		// ���ͷǳ���ܦb�Ĥ@�G�檺�r��
		sprintf(StringForLCD_Line1,"Speed:%2dkm/h",unDispSpeed);
		if(9999>=unDispMileage)
			sprintf(StringForLCD_Line2,"Total:%4dm,Temp:%c%c",unDispMileage,TABLE[NUMBER[1]],TABLE[NUMBER[2]]);
		else
			sprintf(StringForLCD_Line2,"Total:%7dm,T:%c%c",unDispMileage,TABLE[NUMBER[1]],TABLE[NUMBER[2]]);
		
		NewType();
		init_LCM(); //���N�M���ù�
		//�Ĥ@�����
			write_inst(0x80);write_string(StringForLCD_Line1);
		//�ĤG�����
			write_inst(0xC0);write_string(StringForLCD_Line2); write_inst(0xD3);write_char(0x00); //��ܫ�C   
		
		
		//�ĤT,�|�������
		if(64>countMOD%128){
		  write_inst(0x94);write_string(MSG_Welcome_Line1);
			write_inst(0xD4);write_string(MSG_Welcome_Line2);
		}
		else{
			write_inst(0x94);write_string(MSG_Welcome_Line3);
			write_inst(0xD4);write_string(MSG_Welcome_Line4);
		}
		++countMOD;
		
		
			// ���ӭ��n����T�̫����
		  //(countMOD>>4)%5 ,�NcountMOD���W�v���H16,��0~4���g���ܤ�
			write_inst(0x8D +(countMOD>>4)%5);write_char(0x01); //���"�L"
 			write_inst(0x8E +(countMOD>>4)%5);write_char(0x02); //���"��"
 			write_inst(0x8F +(countMOD>>4)%5);write_char(0x03); //���"��"
		
		GetTempByDS1821();
		
		delay1ms(25);
		
		
	}

}


void serial_INT(void) interrupt 4
{//PC�^���ūץ�
		unsigned char ucQUERY;	
	
		if(RI)
		{		
			ucQUERY=SBUF;
			RI=0;
			
			switch(ucQUERY)
			{
				case '3':
					TxAll();
					break;
				case 'L':
					P42XOR();
					break;
				case 'T':
					TxTempearture();
					break;
				case 'S':
					TxSpeed();
					break;
				default:
					break;
			}
		}
}

//== T0���_�Ƶ{��- �@��p�ƾ��A���_����̰�(PT0=1) ===================
void T0_1s(void) interrupt 1// T0���_�Ƶ{���}�l 
{	TH0=TH_M1; TL0=TL_M1;	// �]�mT0�p�ƶq��8�줸�B�C8�줸 
	mSecCounter++;
	
	// �W�L5��S�P��(==�ɳt�C��1.5km/hr)�N�ɳt=0
	(5000<=mSecCounter)&&(unDisp_mSec=65534,mSecCounter=65534);
}							// T0���_�Ƶ{������  
//==int0���_�Ƶ{��- �X�� �P�� ==================
void int0_sw(void) interrupt 0	// int0���_�Ƶ{���}�l 
{	
	unDisp_mSec=mSecCounter; //�s�U�ƭ�
	mSecCounter=0;		// �k�s
	unCycleCounter++; //�C�����_�N�[�@��
	
}								// int 0���_�Ƶ{������  
// //===���u�����=====================================
// void debouncer(void)			// ���u����ƶ}�l 
// {	int i;						// �ŧi�ܼ�i
// 	for(i=0;i<2400;i++); 		// �s��2400���A��20ms
// }								// ���u����Ƶ��� 

// // 40km/hr => 0.1867��/��I26"�樮
// // ���D�ɳt�F�� (0.1867/0.02)*40=373.4km/hr
// // �_�h��20ms�����u�������Ƥ��v�T�p�t

