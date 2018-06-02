#include<intrins.h>

// void delay_2us(unsigned char TWOus)
// {	char i;
// 	
// 	for(i=0;i<TWOus;i++)
// 	{
// 		_nop_();_nop_();_nop_();_nop_();_nop_();
// 		_nop_();_nop_();
// 	}
// }

// void delay_2us(unsigned char TWOus)
// {	unsigned char i;
// 	
// 	for(i=0;i<TWOus;i++)
// 	{
// 		_nop_();_nop_();_nop_();
// 		_nop_();_nop_();_nop_();
// 		_nop_();
// 	}
// }

void delay_2us(unsigned char TWOus)
{	unsigned char i;
	
	for(i=TWOus;i>0;--i)
	{
		_nop_();_nop_();_nop_();
	}
}

void delay_ms(unsigned int time)
{
  unsigned int n;
    while(time>0)
     {
       n=114*5;
       while(n>0) n--;
       time--;
     }   
}

// void delay_77us(void)
// {
//        unsigned int x;
//        x=12*5;
//        while(x>0) x--;
// }

// void delay_4us(void)
// {
//  //空白副程式約4us
// 	delay_77us();
// }

bit read_1bit(void)
{
 bit DATA0;
 DQ_IO=0;
 delay_2us(2);
 DQ_IO=1;
 delay_2us(2);
 DATA0=DQ_IO;
 delay_2us(39);
 return(DATA0);
}

unsigned char read_1byte(void)
{
  unsigned char i,j,DATA;
   DATA=0;
   for (i=0;i<8;i++)
   {
			j=read_1bit();
			DATA=(DATA>>1)|(j<<7);
   }
   return(DATA);
} 
void reset_DS(void)
 {
  DQ_IO=0; // 需大於480us
  delay_ms(1);
  DQ_IO=1; // 需大於480us(包含等待回應脈波)
  delay_ms(1);
 }

void write_1byte(unsigned char DATA)
 {
  unsigned char i;
  bit DATA0;
  {
		for (i =0;i<8;i++)
			{
			DATA0=DATA&0x01;
			if (DATA0==1)
			{
				 DQ_IO=0;
				 delay_2us(2);
				 DQ_IO=1;
				 delay_2us(39);
			}
			else
			{
				 DQ_IO=0;
				 delay_2us(39);
				 DQ_IO=1;
				 delay_2us(2);
			} 
			DATA=DATA>>1;
   }
  }
}



char read_temp (void)
{
  char TEMP;
  reset_DS();
  write_1byte(0xaa);
  TEMP=read_1byte();
  return (TEMP);
}


void display (char TEMP)
{
 if (TEMP<0)
    {
	 NUMBER[0]=11;
	 TEMP=-TEMP;
	 }
 else 
     {
	  NUMBER[0]=TEMP/100;
	  if (NUMBER[0]==0)
	      NUMBER[0]=10;
	 }
  NUMBER[1]=(TEMP/10)%10;
  NUMBER[2]=TEMP%10;
}



