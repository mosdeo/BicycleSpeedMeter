#include <AT89X52.H>
#include <stdio.H>

//==== �ŧi DS1821 �M��register ========
sbit DS1821_Vcc=P0^0;
sbit DQ_IO=P0^1;
sbit DS1821_GND=P0^2;
unsigned char NUMBER[3];
char code TABLE[13]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x20,0x2d,0x2e};//  �ť�- .
char TEMP;
bit bdata work=0;
//==== DS1821.h����include�b�H�W�Uregister����
#include <DS1821.h>
//==== �ŧi DS1821 �M��register ���� ===


//==== �ŧi LCM �M��register ========
#define RS P0_4
#define RW P0_5 //0�g1Ū
#define LCD_Enable  P0_6	//�t�tĲ�o
//�H�U���define�ϥΪ�Port�����ۦP
#define BF P2_7
#define LCDP P2
//==== LCM.h����include�b�H�W�Uregister����
#include <LCM.h>
//==== �ŧi LCM �M��register ���� ===


//==== �ŧi �t�ת� �M��register ========
sbit MagneticSW_Vcc=P3^1;
sbit MagneticSW=P3^2; 		//�e���ϩʶ}��,start/stop ����P3.2(INT0)(�w���N����PB7)
sbit MagneticSW_GND=P3^3;
/*�ŧiT0�p�ɬ����ŧi*/  // THx TLx �p��Ѧ� 7-9�� 
#define  count_M1   1000			// T0(MODE 1)���p�q��,1mSec 
#define  TH_M1  (65636-count_M1)>>8// T0(MODE 1)�p�q��8�줸 
#define  TL_M1  (65636-count_M1)&0x00FF// T0(MODE 1)�p�q�C8�줸
unsigned char Line1_MsgOfLCD[16]=""; //��ܦbLCD�Ĥ@�檺�r��
/* �ŧi���ܼ� */
unsigned int  mSecCounter=0;		// �C��Ĳ�o���j(�C��)mSec��
unsigned int  unDisp_mSec=65536;		// ��ܥΪ�mSec��
unsigned int  unDispSpeed=0;			// ��ܥΪ��t�v
unsigned int  unDispMileage=0;		//��ܥΪ��֭p���{
unsigned int  unCycleCounter=0;		// �֭p���,�p�⨽�{��,26in*pi*65535=136km(�̤j�p�{)
void debouncer(void); 		// �ŧi���u�����
//==== �ŧi �t�ת� �M��register���� ========


//==== �}���e�� ========
unsigned char code MSG_Welcome_Line1[]="NTNU MT Lin-G.Y.";
unsigned char code MSG_Welcome_Line2[]="Rider MCU System";
unsigned char code MSG_Welcome_Line3[]="TEL: 0958889115 ";
unsigned char code MSG_Welcome_Line4[]="mosdeo@gmail.com";
//==== �}���e�� ========

unsigned char StringForLCD_Line1[16]="";
unsigned char StringForLCD_Line2[16]="";
unsigned char DS1821_j; //for "for loop"

main()
{
	IE=0x83;	// 1000 0011,�ҥ�INT0�BTF0(6-4��)
	IP=0x00;PT0=1;  // TF0���u��(for mSec�p��)
  SCON=0x70;
  TCON=0x00,// 0000 0000 �]�wINT0 �Ħ��Ĳ�o(6-4��)
	//���i�Φ��Ĳ�o,�_�h��Ӻ��K�g�L���ɶ����|���_ <- �ثe���Ĳ�o�����u�����D
	TMOD=0x21;				// 0010 0001,T1��mode 2�BT0��mode 1
  TH0=TH_M1; TL0=TL_M1;	// �]�mT0�p�ƶq��8�줸�B�C8�줸 
	TH1=0xFD; //Baud rate
  EX1=0; //�~�����_INT1( P3.3)����
	
	//	DS1821�q���]�w
	DS1821_Vcc=1;
	DS1821_GND=0;

	// �ϩʶ}���q���]�w
	//MagneticSW_Vcc=1;
	MagneticSW_GND=0;
	
	MagneticSW=1;				// �W��MagneticSW��J 
	P0_3=1;			// �W��SW��J
	
	init_LCM();
	NewType();
	write_inst(0x01);	//�M���ù�
	write_inst(0x80);write_string(MSG_Welcome_Line1);
	write_inst(0xC0);write_string(MSG_Welcome_Line2);
	delay_ms(2000);
	
	write_inst(0x01);	//�M���ù�
	write_inst(0x80);write_string(MSG_Welcome_Line3);
	write_inst(0xC0);write_string(MSG_Welcome_Line4);
	delay_ms(2000);
	
	TR1=1;TR0=1;					// �Ұ�T1,T0����MagSW�Ĥ@��Ĳ�o�~�Ұ�(7-7��)


/*--------------------------------------------
/*sbit EX0  = 0xA8;       /* 1=Enable External interrupt 0 */
/*sbit ET0  = 0xA9;       /* 1=Enable Timer 0 interrupt */
/*sbit EX1  = 0xAA;       /* 1=Enable External interrupt 1 */
/*sbit ET1  = 0xAB;       /* 1=Enable Timer 1 interrupt */
/*sbit ES   = 0xAC;       /* 1=Enable Serial port interrupt */
/*sbit ET2  = 0xAD;       /* 1=Enable Timer 2 interrupt */

/*sbit EA   = 0xAF;       /* 0=Disable all interrupts */

//IE=0;
//EA=1;
//ET0=1;
//EX0=1;


	while(1)
	{	 
		// Visable WDT
		P1_5=~P1_5;
		P1_6=~P1_5;
		
		DS1821_j++;
		if(127==(DS1821_j&127)) //�C128�����j(16��)���s��l�ƿù��A��ܿù��u���渨���D
		{	
			init_LCM();
			NewType();
		}
		
		if(7==(DS1821_j&7))	//	8�����j(1��)�~�˴��@���ū�
		{	P1_7=0; //	Visable DS1821 action start
			reset_DS();
			write_1byte(0xee);
			TEMP=read_temp();
			display(TEMP);
			if(1==work)TI=1; // PC���U���O�~���o�g��C���_
			P1_7=1; //	Visable DS1821 action stop
		}
		
		
		//=== �������p��n�b�M���ù��e�����A���M��Ӧh�ɶ�put char��LCD�A�ù��e���{��
		unDispSpeed=(unsigned int)(6883/unDisp_mSec); //�p��t��
		unDispMileage=(unsigned int)(2.075*unCycleCounter); //�p��֭p���{
		// ���ͷǳ���ܦb�Ĥ@�G�檺�r��
		sprintf(StringForLCD_Line1,"Speed:%2dkm/h",unDispSpeed);
		sprintf(StringForLCD_Line2,"Total:%5dm%c%c%c",unDispMileage,TABLE[NUMBER[0]],TABLE[NUMBER[1]],TABLE[NUMBER[2]]);
		
		write_inst(0x01);	//�M���ù�
			write_inst(0x80);write_string(StringForLCD_Line1); //�Ĥ@�����
			write_inst(0xC0);write_string(StringForLCD_Line2); //�ĤG�����
			write_inst(0xCF);write_char(0x00); //��ܫ�C
		
			// ���ӭ��n����T�̫����
			write_inst(0x8D);write_char(0x01); //���"�L"
 			write_inst(0x8E);write_char(0x02); //���"��"
 			write_inst(0x8F);write_char(0x03); //���"��"
			delay1ms(125);
			

		
		
	}

}

void serial_INT(void) interrupt 4
{//PC�^���ūץ�
    unsigned char n;
      if(TI==1)
	    { TI=0;
				for (n=0;n<3;n++)
				{	SBUF=TABLE[NUMBER[n]];
					while(TI==0);
					TI=0;
				}
      }
			 
      if(RI==1)
			{		RI=0;
					switch(SBUF)
					 {
						 case 0x41:
						 work=1;
						 //LED=0;
								 break;
						 case 0x42:
						 work=0;
						 //LED=1;
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
{	P1_3=0; //Visable extern interrput
	unCycleCounter++; //�C�����_�N�[�@��
	unDisp_mSec=mSecCounter; //�s�U�ƭ�
	mSecCounter=0;		// �k�s
	while(MagneticSW==0);				// ���ݩ�}PB7
	debouncer();				// ���u��
	P1_3=1;	//Visable extern interrput
}								// int 0���_�Ƶ{������  
//===���u�����=====================================
void debouncer(void)			// ���u����ƶ}�l 
{	int i;						// �ŧi�ܼ�i
	for(i=0;i<2400;i++); 		// �s��2400���A��20ms
}								// ���u����Ƶ��� 

// 40km/hr => 0.1867��/��I26"�樮
// ���D�ɳt�F�� (0.1867/0.02)*40=373.4km/hr
// �_�h��20ms�����u�������Ƥ��v�T�p�t