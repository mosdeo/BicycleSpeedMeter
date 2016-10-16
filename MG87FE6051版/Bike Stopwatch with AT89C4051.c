#include <REG_MG87FL2051-6051.H>
#include <stdio.H>

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
#define BF   P17
#define LCDP P1
//==== LCM.h����include�b�H�W�Uregister����
#include <LCM.h>
//==== �ŧi LCM �M��register ���� ===


//==== �ŧi �t�ת� �M��register ========
#define MagneticSW P32 		//�e���ϩʶ}��,start/stop ����P3.2(INT0)
/*�ŧiT0�p�ɬ����ŧi*/  // THx TLx �p��Ѧ� 7-9�� 
#define  count_M1   3979			// T0(MODE 1)���p�q��,1mSec 
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
	IE=0x00;
	EA=ET0=EX0=ES=1;
	IP=0x00;
	PT0=1;  // TF0���u��(for mSec�p��)
  SCON=0x70; /*Serial Port mode2*/
  TCON=0x00,// 0000 0000 �]�wINT0 �Ħ��Ĳ�o(6-4��)
	//���i�Φ��Ĳ�o,�_�h��Ӻ��K�g�L���ɶ����|���_ <- �ثe���Ĳ�o�����u�����D
	TMOD=0x21;				// 0010 0001,T1��mode 2�BT0��mode 1
  TH0=TH_M1; TL0=TL_M1;	// �]�mT0�p�ƶq��8�줸�B�C8�줸 
	TH1=TL1=250; 
	
	MagneticSW=1;		// �W��MagneticSW��J 
	
	init_LCM();
	NewType();
	write_inst(0x01);	//�M���ù�
	write_inst(0x80);write_string(MSG_Welcome_Line1);
	write_inst(0xC0);write_string(MSG_Welcome_Line2);
	delay1ms(1000);
	
	write_inst(0x01);	//�M���ù�
	write_inst(0x80);write_string(MSG_Welcome_Line3);
	write_inst(0xC0);write_string(MSG_Welcome_Line4);
	delay1ms(1000);
	
	TR1=1;TR0=1;					// �Ұ�T1,T0



	while(1)
	{	 
		// Visable WDT
		P42=~P42;
		
		DS1821_j++;
		if(127==(DS1821_j&127)) //�C128�����j(16��)���s��l�ƿù��A��ܿù��u���渨���D
		{	
			init_LCM();
		}
		
		if(7==(DS1821_j&7))	//	8�����j(1��)�~�˴��@���ū�
		{	P43=0; //	Visable DS1821 action start
			reset_DS();
			write_1byte(0xee);
			TEMP=read_temp();
			display(TEMP);
			P43=1; //	Visable DS1821 action stop
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
		unsigned char ucQUERY;	
	
//       if(TI==1)
// 	    { TI=0;
// 				for (n=0;n<3;n++)
// 				{	SBUF=TABLE[NUMBER[n]];
// 					while(TI==0);
// 					TI=0;
// 				}
//       }
			 
      if(RI)
			{		
				ucQUERY=SBUF;
				RI=0;
				
				switch(ucQUERY)
				{
					case 'A':
					 //work=1;
						P43=0;
						break;
					case 'B':
						//work=0;
						P43=1;
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
{	//P43=1; //Visable extern interrput
	unCycleCounter++; //�C�����_�N�[�@��
	unDisp_mSec=mSecCounter; //�s�U�ƭ�
	mSecCounter=0;		// �k�s
	while(MagneticSW==0);				// ���ݩ�}PB7
	debouncer();				// ���u��
	//P43=0;	//Visable extern interrput
}								// int 0���_�Ƶ{������  
//===���u�����=====================================
void debouncer(void)			// ���u����ƶ}�l 
{	int i;						// �ŧi�ܼ�i
	for(i=0;i<2400;i++); 		// �s��2400���A��20ms
}								// ���u����Ƶ��� 

// 40km/hr => 0.1867��/��I26"�樮
// ���D�ɳt�F�� (0.1867/0.02)*40=373.4km/hr
// �_�h��20ms�����u�������Ƥ��v�T�p�t