void delay_ms(unsigned int time)
{
  unsigned int n;
    while(time>0)
     {
       n=114;
       while(n>0) n--;
       time--;
     }   
}inline;

void delay_77us(void)
{
       unsigned int x;
       x=12;
       while(x>0) x--;
}inline;

void delay_4us(void)
{
 //空白副程式約4us
}inline;
bit read_1bit(void)
{
  bit DATA0;
  DQ_IO=0;
  delay_4us();
  DQ_IO=1;
  delay_4us();
  DATA0=DQ_IO;
  delay_77us();
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
    for(i =0;i<8;i++)
    {
      DATA0=DATA&0x01;
      if (DATA0==1)
      {
        DQ_IO=0;
        delay_4us();
        DQ_IO=1;
        delay_77us();
      }
      else
      {
        DQ_IO=0;
        delay_77us();
        DQ_IO=1;
        delay_4us();
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
  if(TEMP<0)
  {
    NUMBER[0]=11;
    TEMP=-TEMP;
  }
  else 
  {
    NUMBER[0]=TEMP/100;
    if(NUMBER[0]==0)
      NUMBER[0]=10;
  }
  NUMBER[1]=(TEMP/10)%10;
  NUMBER[2]=TEMP%10;
}



