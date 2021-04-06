#include <at89c5131.h>
#include "lcd.h"		//Header file with LCD interfacing functions
#include "serial.c"	//C file with UART interfacing functions

//sbit LED=P1^7;
sbit sw = P1^0;
char state=0x03;		// LSFR state
int val;						// Type of block
int rot=0;					// Type of orientation (when applicable)
//char t=0x03;

char print_block_init(char cursor)
{
	char i;
	val = (int)(state)%3;							// block type calculated
	for (i=cursor;(i&(0x0F))<15;i++)	// loop for clearing the area of LCD lying to the right of the moving block
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
	if (val==0)										// block of type ** OR  *
	{															// 											*
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
	else if (val==1)							// block of type	*  OR	** OR ** OR  *
	{															//								**		*			 *	  **
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
	else if(val==2)											// block of type *
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
	unsigned int max_score = 0;					// highest score
	char a1;														// first digit of score in ASCII
	char a0;														// second digit of score in ASCII
	unsigned char n1 = 0;								// keep track of upper nibble of prev_cursor1, if equal to 9, prev_cursor1 has crossed LCD
	unsigned char n2 = 0;								// keep track of upper nibble of prev_cursor2, if equal to 13 (=0x0D), prev_cursor1 has crossed LCD
	char m1 = 0;												// keep track of upper nibble of prev_cursor1, for line change purposes
	char m2 = 0;												// keep track of upper nibble of prev_cursor2, for line change purposes
	char done=0;												// done=0, game going on, done=1, game is finished
	unsigned char temp;									// used in calculation of next state of LFSR
	unsigned char ch=0;									// input character from keyboard
	char cursor_now=0x8F;								// current cursor position on LCD, where block gets printed
	char prev_cursor1 = 0x80-1;					// used for keeping track of how far we can go on Line 1 of LCD
	char prev_cursor2 = 0xC0-1;					// used for keeping track of how far we can go on Line 2 of LCD
	char cursor_alt = 0xFF;							// keeps track of leftmost most position of block on 2nd line of LCD, not active for 1-line blocks
	//char a,b = 0x00;
	unsigned int count=0;								// count number of blocks present on the LCD = score
	//unsigned char score[5];
	//Initialize port P1 for output from P1.7-P1.4
	P1 = 0x0F;
	
	//Call initialization functions
	lcd_init();
	uart_init();
	
	//These strings will be printed in terminal software
	//transmit_string("Game On\r\n");
	
	
	//lcd_cmd(0x83);
	//lcd_write_string("Game On");
	//msdelay(200);
	//lcd_init();	
	while (1)
	{
		//state=0x03;
		n1 = 0;
		n2 = 0;
		done=0;
		ch=0;
		prev_cursor1 = 0x80-1;
		prev_cursor2 = 0xC0-1;
		cursor_alt = 0xFF;
		count=0;
		
		cursor_now=0x8F;
		cursor_now = print_block_init(cursor_now);
		cursor_now = cursor_now-0x01;
		while(state!=0)
		{
				while(1)
				{
					ch = receive_char();
					switch(ch)
					{
						case 'q':
										 transmit_char(ch);														// display character pressed on keyboard, on the Realterm window
										 if ((cursor_now&(0xF0))==0xC0)								// go to second line only if we are on 1st line
										 {
											 if (val!=1 && rot==0)											// go to second line only if block is a one line block
											 {
												 lcd_cmd(cursor_now+1);
												 lcd_write_char(' ');											// clear space occupied prior to line change
												 lcd_cmd(cursor_now+2);
												 lcd_write_char(' ');												// clear space occupied prior to line change (2nd time needed for ** block)
												 cursor_now = (cursor_now&(0x0F))|(0x80);		// keep cursor fixed, change line number only
											 }
										 }
										 //LED=1;
										 cursor_now = print_block_init(cursor_now);
										 //LED=0;
										 break;
						
						case 'a':
										 transmit_char(ch);														// display character pressed on keyboard, on the Realterm window
										 if ((cursor_now&(0xF0))==0x80)								// go to first line only if we are on 2nd line
										 {
											 if (val!=1 && rot==0)											// go to first line only if block is a one line block
											 {
												 lcd_cmd(cursor_now+1);
												 lcd_write_char(' ');											// clear space occupied prior to line change
												 lcd_cmd(cursor_now+2);
												 lcd_write_char(' ');												// clear space occupied prior to line change (2nd time needed for ** block)
												 cursor_now = (cursor_now&(0x0F))|(0xC0); 	// keep cursor fixed, change line number only
											 }
										 }
										 //LED=1;
										 cursor_now = print_block_init(cursor_now);			// print block
										 //LED=0;
										 break;
						case 'r':transmit_char(ch);															// display character pressed on keyboard, on the Realterm window
										 if (val==0)																		
										 {
											 if (rot==0)																	// for ** block
											 {
												 rot=1;
												 cursor_now = (cursor_now&(0x0F))|(0x80);		// cursor on line 1 now
											 }
											 else																					// for * block
											 {																						//     *
												 rot=0;
												 lcd_cmd(((cursor_now&(0x0F))|(0xC0))+1);
												 lcd_write_char(' ');												// clear the * on the second line at same cursor position
												 cursor_now = (cursor_now&(0x0F))|(0x80);		// cursor on line 1 now
											 }
										 }
										 if (val==1)																		// block of type	*  OR	** OR ** OR  *
										 {																							//								**		*			 *	  **
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
												 cursor_now = cursor_now + 1;						//cursor_now needs to move to the right
											 }
											 else
											 {
												 rot = 0;
												 cursor_now = cursor_now - 1;						// cursor_now needs to move to the left
											 }
										 }
										 //LED=1;
										 cursor_now = print_block_init(cursor_now);				// print block
										 //LED=0;
										 break;
										 
									
						default: transmit_char(ch);															// any other character input, no change, block will just move to left
										 break;
						
					}
					cursor_now = cursor_now-1;															// cursor_now moved to left
					if (val==1)																							// cursor_alt will be activated for 2-line blocks
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
							cursor_alt = ((cursor_now&(0x0F))-1)|(0xC0);
						}
					}
					else if (val==0 && rot==1)														// cursor_alt will be activated for 2-line blocks
					{
						cursor_alt = (cursor_now&(0x0F))|(0xC0);
					}
					else
					{
						cursor_alt=0xFF;																		// if it is a one-line block, cursor_alt is deactivated
					}
					
					m1 = prev_cursor1&(0xF0);															// track upper nibble of prev_cursor1, needed for alignment in case of 2-line blocks
					m1 = m1>>4;
					m1 = m1&(0x0F);
					m2 = m1+4;																						// track upper nibble of prev_cursor2, needed for alignment in case of 2-line blocks
					m2 = m2<<4;
					if ((cursor_now==prev_cursor1) || (cursor_now==prev_cursor2) || (cursor_alt==prev_cursor2)) // stop moving left if cursor_now has reached prev_cursor1
					{																																														// OR prev_cursor2 OR cursor_alt (for 2-line blocks) has reached prev_cursor2
						if (val==0)																														// now updating pre_cursor1 and prev_cursor2
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
									prev_cursor2 = ((cursor_now&(0x0F))|(m2))+1;
									prev_cursor1 = cursor_now+1;
								}
								else
								{
									prev_cursor1 = ((prev_cursor2&(0x0F))|(m1))+1;
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
									prev_cursor2 = ((prev_cursor1&(0x0F))|(m2))+2;
									prev_cursor1 = cursor_now+1;
								}
								else
								{
									prev_cursor1 = ((prev_cursor2&(0x0F))|(m1))+1;
									prev_cursor2 = prev_cursor2+2;
								}
							}
							else if (rot==1)
							{
								if (cursor_now==prev_cursor1)
								{
									prev_cursor2 = ((prev_cursor1&(0x0F))|(m2))+1;
									prev_cursor1 = cursor_now+2;
								}
								else
								{
									prev_cursor1 = ((prev_cursor2&(0x0F))|(m1))+2;
									prev_cursor2 = prev_cursor2+1;
								}
							}
							else if (rot==2)
							{
								if (cursor_now==prev_cursor1)
								{
									prev_cursor2 = ((prev_cursor1&(0x0F))|(m2))+2;
									prev_cursor1 = cursor_now+2;
								}
								else
								{
									prev_cursor1 = ((prev_cursor2&(0x0F))|(m1))+1;
									prev_cursor2 = prev_cursor2+1;
								}
							}
							else
							{
								if (cursor_now==prev_cursor1)
								{
									prev_cursor2 = ((prev_cursor1&(0x0F))|(m2))+1;
									prev_cursor1 = cursor_now+1;
								}
								else
								{
									prev_cursor1 = ((prev_cursor2&(0x0F))|(m1))+2;
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
						n1 = (unsigned char)(prev_cursor1&(0xF0));				// upper nibble of prev_cursor1
						n1=n1>>4;
						n2 = (unsigned char)(prev_cursor2&(0xF0));				// upper nibble of prev_cursor2
						n2=n2>>4;
						if (n1==9 || n2==13)															// n1=9 OR n2=13 (0x0D) implies prev_cursor1>0x8F OR prev_cursor2>0xCF, implying LCD is full
						{
							done=1;																					// LCD is full, so current game is finished
							if (max_score<count)														// highest score updated
							{
								max_score = count;
							}
							lcd_init();																			// clear screen
							//msdelay(10);
							lcd_cmd(0x83);
							lcd_write_string("Score=");
							a1 = 48 + (count/10);														// first digit of score
							a0 = 48 + (count%10);														// second digit of score
							lcd_cmd(0x8A);
							lcd_write_char(a1);
							lcd_cmd(0x8B);
							lcd_write_char(a0);
							
							lcd_cmd(0xC1);
							lcd_write_string("Max Score=");
							a1 = 48 + (max_score/10);												// first digit of highest score
							a0 = 48 + (max_score%10);												// second digit of highest score
							lcd_cmd(0xCC);
							lcd_write_char(a1);
							lcd_cmd(0xCD);
							lcd_write_char(a0);
							
							msdelay(5000);																	// display that for 5 seconds
							lcd_init();
							
							break;																					// break out of while loop
						}
						count = count+1;																	// score updated, note that this does not get execcuted if LCD gets full
						break;																						// break of current block cycle
					}
				}
				temp = state<<3;																			// calculation of next LFSR state
				temp = temp&(0x08);
				temp = state^temp;
				temp = temp&(0x08);
				state = state>>1;
				state = (state)&(0x07);
				state = state|temp;
				
				cursor_now=0x8F;																			// cursor reset
				rot=0;																								// rotation reset
				
				if (done==1)																					// break out of curnent game if LCD gets full
				{
					break;
				}
		}
	}
}

