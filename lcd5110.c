// LCD Nokia 5110 Interface
// Author: Luan Ferreira Reis de Jesus

#include <msp430g2553.h>
#include "font_table.h"

#define RST	BIT0
#define CE	BIT1
#define DC	BIT2
#define DIN	BIT3
#define CLK	BIT4
#define BL	BIT5


void delay_us(unsigned int us)
{
    while (us)
    {
        // 1 for 1 Mhz set 16 for 16 MHz
        __delay_cycles(1); 
        us--;
    }
}


void delay_ms(unsigned int ms)
{
    while (ms)
    {
        // 1000 for 1MHz and 16000 for 16MHz
        __delay_cycles(1000);
        ms--;
    }
}


void configure_clocks()
{
    // Stop watchdog
    WDTCTL = WDTPW + WDTHOLD;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
}


void send_byte(unsigned char byte)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        P1OUT &= (~CLK);
        if (byte > 127) {
            P1OUT |= DIN;
        }
        else
        {
            P1OUT &= (~DIN);
        }
        
        // rise clock
        P1OUT |= (CLK);
        
        // shift byte
        byte = byte << 1;
    }
    P1OUT &= (~CLK);
}


void send_data(unsigned char data)
{
    P1OUT |= DC;
    send_byte(data);
}


void send_command(unsigned char cmd)
{
    P1OUT &= (~DC);
    send_byte(cmd);
}


void init_lcd()
{
    P1DIR |= RST | CE | DC | DIN | CLK | BL;
    
    P1OUT = RST | CE | DC | ~DIN | ~CLK | ~BL;
    delay_ms(50);
    
    // reset the controller
    P1OUT &= (~RST);
    delay_us(1);
    P1OUT |= (RST);
    
    // start
    P1OUT &= (~CE);
    send_command(0x21);
    send_command(0x99);    // set LCD Vop (contrast)
    send_command(0x20);    // LCD basic commands
    send_command(0x0C);
    
    // turn on backlight
    //P1OUT |= (BL);
}


void clear_display()
{
    unsigned short i;
    for(i = 0; i < 504; i++)
    {
        send_data(0x00);
    }
}


void set_cursor(unsigned char x, unsigned char y)
{
    unsigned char x_addr = 0x80 | x;
    unsigned char y_addr = 0x40 | y;
    send_command(0x20);
    send_command(x_addr);
    send_command(y_addr);
}


void print_char(char c)
{
    unsigned char i;
    for (i = 0; i < 5; i++)
    {
        send_data(font[c - 0x20][i]);
    }
    send_data(0x00);
}


void print_string(char *str)
{
    while(*str)
    {
        print_char(*str);
        str++;
    }
}


int main()
{   
    //configure the clock frequency
    configure_clocks();

    // init lcd
    init_lcd();
    
    // clear the display
    clear_display();

    set_cursor(12, 2);
    print_string("Olá Mundo");
    set_cursor(12, 3);
    print_string("Ola Mundo!");
    
    P1OUT |= CE;
    while (1)
    {
        
    }
}