//Листинг А.6
//Заголовочный файл biosio.h

#ifndef _BIOSIO_H_
#define _BIOSIO_H_

typedef unsigned char byte;



void setcursor(byte x, byte y);
void getcursor(byte *x, byte *y, int *cur);
void set_cursor_type(int);
void fill_screen(char c, byte atr);
int vmode(void);
int scroll_lock(void);
int get_key(void);

void vpoke(byte x, byte y, byte atr, char chr);
int vpeek(byte x, byte y);
void displtime(void);

#define clear_screen() fill_screen(' ', 7)

#endif
