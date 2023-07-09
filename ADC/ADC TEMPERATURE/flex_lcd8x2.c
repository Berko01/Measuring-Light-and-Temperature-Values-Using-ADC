// flex_lcd.c yazan:PCM Programmer
// A�a��daki pinleri istedi�iniz
// gibi devrenize g�re de�i�tirebilirsiniz
#define LCD_DB4 PIN_B0
#define LCD_DB5 PIN_B1
#define LCD_DB6 PIN_B2
#define LCD_DB7 PIN_B3
 
#define LCD_E PIN_B5
#define LCD_RS PIN_B4
#define LCD_RW PIN_B6
//LCDyi sadece 6 pin ile kullanacaksan�z
// R/W pinini �aseye ba�lay�n�z ve
// a�a��daki sat�r� siliniz yada devre d���na al�n�z
//#define USE_LCD_RW 1
 
//========================================
 
#define lcd_type 2 // 0=5x7, 1=5x10, 2=2 sat�r bildirimleri
#define lcd_line_two 0x40 // 2. sat�r�n LCD RAM adresi
int8 const LCD_INIT_STRING[4] =
{
 0x20 | (lcd_type << 2), // LCD fonksiyonlar�: 4-bit, 2 sat�r, 5x8 nokta
 0xc, // Display on
 1, // Ekran temizle
 6 // kursor� art�r
 };
 
//-------------------------------------
void lcd_send_nibble(int8 nibble)
{
//onluk say�y� ikilik say� sistemine �evirme
 output_bit(LCD_DB4, !!(nibble & 1));
 output_bit(LCD_DB5, !!(nibble & 2));
 output_bit(LCD_DB6, !!(nibble & 4));
 output_bit(LCD_DB7, !!(nibble & 8));
 
delay_cycles(1);
 output_high(LCD_E);
 delay_us(2);
 output_low(LCD_E);
}
#ifdef USE_LCD_RW
int8 lcd_read_nibble(void)
{
int8 retval;
#bit retval_0 = retval.0
#bit retval_1 = retval.1
#bit retval_2 = retval.2
#bit retval_3 = retval.3
 
retval = 0;
 
output_high(LCD_E);
delay_cycles(1);
 
retval_0 = input(LCD_DB4);
retval_1 = input(LCD_DB5);
retval_2 = input(LCD_DB6);
retval_3 = input(LCD_DB7);
 
output_low(LCD_E);
 
return(retval);
}
#endif
 
//---------------------------------------
//LCD'den byte bilgi okunur.
 
#ifdef USE_LCD_RW
int8 lcd_read_byte(void)
{
int8 low;
int8 high;
 
output_high(LCD_RW);
delay_cycles(1);
 
high = lcd_read_nibble();
 
low = lcd_read_nibble();
 
return( (high<<4) | low);
}
#endif
 
//----------------------------------------
//LCD'ye byte bilgi g�nderilir.
void lcd_send_byte(int8 address, int8 n)
{
output_low(LCD_RS);
 
#ifdef USE_LCD_RW
while(bit_test(lcd_read_byte(),7)) ;
#else
delay_us(60);
#endif
 
if(address)
 output_high(LCD_RS);
else
 output_low(LCD_RS);
 
 delay_cycles(1);
 
#ifdef USE_LCD_RW
output_low(LCD_RW);
delay_cycles(1);
#endif
 
output_low(LCD_E);
 
lcd_send_nibble(n >> 4);
lcd_send_nibble(n & 0xf);
}
 
//----------------------------
void lcd_init(void)
{
int8 i;
 
output_low(LCD_RS);
 
#ifdef USE_LCD_RW
output_low(LCD_RW);
#endif
 
output_low(LCD_E);
 
delay_ms(15);
 
for(i=0 ;i < 3; i++)
 {
 lcd_send_nibble(0x03);
 delay_ms(5);
 }
 
lcd_send_nibble(0x02);
 
for(i=0; i < sizeof(LCD_INIT_STRING); i++)
 {
 lcd_send_byte(0, LCD_INIT_STRING[i]);
 
#ifndef USE_LCD_RW
 delay_ms(5);
 #endif
 }
 
}
 
//----------------------------
 
void lcd_gotoxy(int8 x, int8 y)
{
int8 address;
 
if(y != 1)
 address = lcd_line_two;
else
 address=0;
 
address += x-1;
lcd_send_byte(0, 0x80 | address);
}
 
//-----------------------------
void lcd_putc(char c)
{
 switch(c)
 {
 case '\f':
 lcd_send_byte(0,1);
 delay_ms(2);
 break;
 
 case '\n':
 lcd_gotoxy(1,2);
 break;
 
 case '\b':
 lcd_send_byte(0,0x10);
 break;
 
 default:
 lcd_send_byte(1,c);
 break;
 }
}
 
//------------------------------
#ifdef USE_LCD_RW
char lcd_getc(int8 x, int8 y)
{
char value;
 
lcd_gotoxy(x,y);
 
while(bit_test(lcd_read_byte(),7));
 
output_high(LCD_RS);
value = lcd_read_byte();
output_low(lcd_RS);
 
return(value);
}
#endif
