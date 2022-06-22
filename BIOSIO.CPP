//Литинг А.7
//Модуль biosio.cpp

#pragma  inline
#include <dos.h>
#include <time.h>
#include <stdio.h>
#include "biosio.h"

extern const char *day[];


// Установка позиции курсора

void setcursor(byte x, byte y)
{
asm     {
	mov ax, 0200h
	mov bx, 0
	mov dh, y
	mov dl, x
	int 10h
	}
}


//Получение позиции и типа курсора

void getcursor(byte *x, byte *y, int *cur)
{
asm 	{
	mov ax, 0300h
	mov bx, 0
	int 10h
	mov bx, x
	mov [bx], dl
	mov bx, y
	mov [bx], dh
	mov bx, cur
	mov [bx], cx
	}
}


//Установка типа курсора

void set_cursor_type(int t)
{
asm {
	mov ax, 0100h
	mov bx, 0
	mov cx, t
	int 10h
	}
}



// Заполнить экран символом

void fill_screen(char c, byte atr)
{
setcursor(0, 0);
asm	{
	mov al, c
	mov ah, 9
	mov bh, 0
	mov bl, atr
	mov cx, 2000;
	int 10h
	}
}

// возвратить режим работы видеоадаптера

int vmode()
{
asm	{
	mov ah, 15
	int 10h
	}
return _AL;
}



/* принять символ от клавиатуры */
int get_key()
{

l1:
asm    	mov ah, 01h   //Проверка наличия символа в очереди
asm	int 16h
asm	jnz l2
displtime();         //Если буфер пуст, вывести текущее время и дату
asm	int 28h      //Передать управление резидентным программам
asm	jmp l1

l2:
asm	mov ah, 0
asm	int 16h

return _AX;
}


//Запись символа и атрибута в позицию x, y

void vpoke(byte x, byte y, byte atr,  char chr)
{
setcursor(x, y);
asm	{
	mov ah, 09h
	mov al, chr
	mov bh, 0
	mov bl, atr
	mov cx, 1
	int 10h
	}
}

// считать код символа и его атрибуты из видеопамяти

int vpeek(byte x, byte y)
{
setcursor(x, y);
asm	{
	mov ah, 08h
	mov bh, 0
	int 10h
	}
return _AX;

}



//Отображение текужего времени и даты
void displtime(void)
{
char far *vb = (char far*)0xb8000000l;
char strtime[25];
time_t ct;
int i, col = 80;
int oldx, oldy, oldcur;
ct = time(NULL);
tm *curtime=localtime(&ct);
i = sprintf(strtime, "%02d:%02d:%02d  %02d.%02d.%04d  %2s",
	curtime->tm_hour,
	curtime->tm_min,
	curtime->tm_sec,
	curtime->tm_mday,
	curtime->tm_mon+1,
	curtime->tm_year+1900,
	day[curtime->tm_wday]
	);
do	{
	i--;
	col--;
	*(vb+col*2) = strtime[i];
	*(vb+col*2+1) = '\12';
	} while (i);
}

