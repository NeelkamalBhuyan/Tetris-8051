#include <at89c5131.h>
#include "lcd.h"		//Header file with LCD interfacing functions
#include "serial.c"	//C file with UART interfacing functions

sbit LED=P1^7;
sbit sw = P1^0;
char state=0x03;
int val;
int rot=0;
char t=0x03;

char print_block_init(char cursor)
{
	char i;
	val = (int)(state)%t;
	for (i=cursor;(i&(0x0F))<15;i++)
	{
		if (val==0)
		{
			if (rot==0)
			{
				lcd_cmd(i+1);
				lcd_write_char(' ');
			}
			else
			{
				lcd_cmd((0x80|(i&(0x0F)))+1);
				lcd_write_char(' ');
				lcd_cmd((0xC0|(i&(0x0F)))+1);
				lcd_write_char(' ');
			}
		}
		else if (val==1)
		{
			if (rot==0)
			{
				lcd_cmd((0x80|(i&(0x0F)))+1);
				lcd_write_char(' ');
				lcd_cmd((0xC0|(i&(0x0F)))+2);
				lcd_write_char(' ');
			}
			else if (rot==1)
			{
				lcd_cmd((0x80|(i&(0x0F)))+2);
				lcd_write_char(' ');
				lcd_cmd((0xC0|(i&(0x0F)))+1);
				lcd_write_char(' ');
			}
			else if (rot==2)
			{
				lcd_cmd((0x80|(i&(0x0F)))+2);
				lcd_write_char(' ');
				lcd_cmd((0xC0|(i&(0x0F)))+2);
				lcd_write_char(' ');
			}
			else
			{
				lcd_cmd((0x80|(i&(0x0F)))+1);
				lcd_write_char(' ');
				lcd_cmd((0xC0|(i&(0x0F)))+1);
				lcd_write_char(' ');
			}
		}
		else
		{
			lcd_cmd(i);
			lcd_write_char(' ');
			lcd_cmd(i+1);
			lcd_write_char(' ');
		}
	}
	if (val==0)
	{
		if (rot==0)
		{
			lcd_cmd(cursor);
			lcd_write_string("**");
			msdelay(200);
		}
		else
		{
			lcd_cmd(0x80+(cursor & 0x0F));
			lcd_write_string("*");
			lcd_cmd(0xC0+(cursor & 0x0F));
			lcd_write_string("*");
			msdelay(200);
		}
	}
	else if (val==1)
	{
		if (rot==0)
		{
			lcd_cmd(0x80+(cursor & 0x0F));
			lcd_write_string("*");
			lcd_cmd(0xC0+(cursor & 0x0F));
			lcd_write_string("**");
			msdelay(200);
		}
		else if (rot==1)
		{
			lcd_cmd(0x80+(cursor & 0x0F));
			lcd_write_string("**");
			lcd_cmd(0xC0+(cursor & 0x0F));
			lcd_write_string("*");
			msdelay(200);
		}
		else if (rot==2)
		{
			lcd_cmd(0x80+(cursor & 0x0F));
			lcd_write_string("**");
			lcd_cmd((0xC0+(cursor & 0x0F))+1);
			lcd_write_string("*");
			msdelay(200);
		}
		else
		{
			lcd_cmd(0x80+(cursor & 0x0F));
			lcd_write_string("*");
			lcd_cmd((0xC0+(cursor & 0x0F))-1);
			lcd_write_string("**");
			msdelay(200);
		}
	}
	else if(val==2)
	{
		lcd_cmd(cursor);
		lcd_write_string("*");
		msdelay(200);
	}
	else
	{
	}
	return cursor;
}


//Main function
void main(void)
{
	unsigned char temp;
	unsigned char ch=0;
	char cursor_now=0x8F;
	char prev_cursor1 = 0x80-1;
	char prev_cursor2 = 0xC0-1;
	char cursor_alt = 0xFF;
	char a,b = 0x00;
	int count=0;
	unsigned char score;
	//Initialize port P1 for output from P1.7-P1.4
	P1 = 0x0F;
	
	//Call initialization functions
	lcd_init();
	uart_init();
	
	//These strings will be printed in terminal software
	transmit_string("Game On\r\n");
	
	
	lcd_cmd(0x83);
	lcd_write_string("Game On");
	msdelay(200);
	lcd_init();
	msdelay(200);
	
	cursor_now=0x8F;
	cursor_now = print_block_init(cursor_now);
	cursor_now = cursor_now-0x01;
	
	while(1)
	{
			while(1)
			{
				ch = receive_char();
				//val = (int)(state)%3;
				a = prev_cursor1&(0x0F);
				b = prev_cursor2&(0x0F);
				switch(ch)
				{
					case 'q'://lcd_test();
									 //transmit_string("LCD tested\r\n");
									 transmit_char(ch);
									 if ((cursor_now&(0xF0))==0xC0)
									 {
										 if (val!=1)
										 {
											 lcd_cmd(cursor_now+1);
											 lcd_write_char(' ');
											 lcd_cmd(cursor_now+2);
											 lcd_write_char(' ');
											 cursor_now = (cursor_now&(0x0F))|(0x80);
										 }
									 }
									 LED=1;
									 cursor_now = print_block_init(cursor_now);
									 LED=0;
									 break;
					
					case 'a'://led_test();
									 //transmit_string("LED tested\r\n");
									 transmit_char(ch);
									 if ((cursor_now&(0xF0))==0x80)
									 {
										 if (val!=1)
										 {
											 lcd_cmd(cursor_now+1);
											 lcd_write_char(' ');
											 lcd_cmd(cursor_now+2);
											 lcd_write_char(' ');
											 cursor_now = (cursor_now&(0x0F))|(0xC0);
										 }
									 }
									 LED=1;
									 cursor_now = print_block_init(cursor_now);
									 LED=0;
									 break;
					case 'r':transmit_char(ch);
									 if (val==0)
									 {
										 if (rot==0)
										 {
											 rot=1;
											 cursor_now = (cursor_now&(0x0F))|(0x80);
										 }
										 else
										 {
											 rot=0;
											 lcd_cmd(((cursor_now&(0x0F))|(0xC0))+1);
											 lcd_write_char(' ');
											 cursor_now = (cursor_now&(0x0F))|(0x80);
										 }
									 }
									 if (val==1)
									 {
										 if (rot==0)
										 {
											 rot = 1;
										 }
										 else if (rot==1)
										 {
											 rot = 2;
										 }
										 else if (rot==2)
										 {
											 rot = 3;
											 cursor_now = cursor_now + 1;
										 }
										 else
										 {
											 rot = 0;
											 cursor_now = cursor_now - 1;
										 }
									 }
									 LED=1;
									 cursor_now = print_block_init(cursor_now);
									 LED=0;
									 break;
									 
								
					default: transmit_char(ch);//transmit_string("Incorrect test. Pass correct number\r\n");
									 transmit_char(ch);
									 //print_block_init(cursor,line);
									 break;
					
				}
				cursor_now = cursor_now-1;
				if (val==1)
				{
					if (rot==0)
					{
						cursor_alt = (cursor_now&(0x0F))|(0xC0);
					}
					else if (rot==1)
					{
						cursor_alt = (cursor_now&(0x0F))|(0xC0);
					}
					else if (rot==2)
					{
						cursor_alt = (cursor_now&(0x0F))|(0xC0) + 1;
					}
					else
					{
						cursor_alt = (cursor_now&(0x0F))|(0xC0)-1;
					}
				}
				else if (val==0 && rot==1)
				{
					cursor_alt = (cursor_now&(0x0F))|(0xC0);
				}
				else
				{
					cursor_alt=0xFF;
				}
				if ((cursor_now==prev_cursor1) || (cursor_now==prev_cursor2) || (cursor_alt==prev_cursor2))
				{
					if (val==0)
					{
						if (rot==0)
						{
							if (cursor_now==prev_cursor1)
							{
								prev_cursor1 = cursor_now+2;
							}
							else
							{
								prev_cursor2 = cursor_now+2;
							}
						}
						else
						{
							if (cursor_now==prev_cursor1)
							{
								prev_cursor2 = ((cursor_now&(0x0F))|(0xC0))+1;
								prev_cursor1 = cursor_now+1;
							}
							else
							{
								prev_cursor1 = ((prev_cursor2&(0x0F))|(0x80))+1;
								prev_cursor2 = prev_cursor2+1;
							}
						}
					}
					else if (val==1)
					{
						if (rot==0)
						{
							if (cursor_now==prev_cursor1)
							{
								prev_cursor2 = ((prev_cursor1&(0x0F))|(0xC0))+2;
								prev_cursor1 = cursor_now+1;
							}
							else
							{
								prev_cursor1 = ((prev_cursor2&(0x0F))|(0x80))+1;
								prev_cursor2 = prev_cursor2+2;
							}
						}
						else if (rot==1)
						{
							if (cursor_now==prev_cursor1)
							{
								prev_cursor2 = ((prev_cursor1&(0x0F))|(0xC0))+1;
								prev_cursor1 = cursor_now+2;
							}
							else
							{
								prev_cursor1 = ((prev_cursor2&(0x0F))|(0x80))+2;
								prev_cursor2 = prev_cursor2+1;
							}
						}
						else if (rot==2)
						{
							if (cursor_now==prev_cursor1)
							{
								prev_cursor2 = ((prev_cursor1&(0x0F))|(0xC0))+2;
								prev_cursor1 = cursor_now+2;
							}
							else
							{
								prev_cursor1 = ((prev_cursor2&(0x0F))|(0x80))+1;
								prev_cursor2 = prev_cursor2+1;
							}
						}
						else
						{
							if (cursor_now==prev_cursor1)
							{
								prev_cursor2 = ((prev_cursor1&(0x0F))|(0xC0))+1;
								prev_cursor1 = cursor_now+1;
							}
							else
							{
								prev_cursor1 = ((prev_cursor2&(0x0F))|(0x80))+2;
								prev_cursor2 = prev_cursor2+2;
							}
						}
					}						
					else
					{
						if (cursor_now==prev_cursor1)
						{
							prev_cursor1 = cursor_now+1;
						}
						else
						{
							prev_cursor2 = cursor_now+1;
						}
					}
					if (prev_cursor1==0x8F && prev_cursor2==0xCF)
					{
						prev_cursor1 = 0x7F;
						prev_cursor2 = 0xBF;
						lcd_init();
						msdelay(200);
					}
					count = count+1;
					break;
				}
			}
			msdelay(100);
			temp = state<<3;
			temp = temp&(0x08);
			temp = state^temp;
			temp = temp&(0x08);
			state = state>>1;
			state = (state)&(0x07);
			state = state|temp;
			
			cursor_now=0x8F;
			
			rot=0;
			
			if (prev_cursor1==0x7F)
			{
				if(prev_cursor2==0xBF)
				{
					lcd_cmd(0x84);
					lcd_write_string("Score");
					int_to_string(count, &score);
					msdelay(10);
					lcd_cmd(0xC5);
					lcd_write_string(&score);
					msdelay(5000);
					break;
				}
			}
			if (prev_cursor2==0xBF)
			{
				if(prev_cursor1==0x7F)
				{
					lcd_cmd(0x84);
					lcd_write_string("Score");
					int_to_string(count, &score);
					msdelay(10);
					lcd_cmd(0xC5);
					lcd_write_string(&score);
					msdelay(5000);
					break;
				}
			}
	}
}

