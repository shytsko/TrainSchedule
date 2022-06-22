//Листинг А.2
//Модуль twindow.cpp

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <dos.h>
#include <mem.h>
#include <stdlib.h>
#include <string.h>
#include "twindow.h"
#include "keys.h"
#include "biosio.h"

#define TABS 4
#define SCRNHT 25
#define SCRNWIDTH 80
#define ON  1
#define OFF 0
#define ERROR -1


#ifdef __cplusplus

  int max (int value1, int value2);

  int max(int value1, int value2)
  {
     return ( (value1 > value2) ? value1 : value2);
  }

  int min (int value1, int value2);

  int min(int value1, int value2)
  {
     return ( (value1 > value2) ? value2 : value1);
  }

#endif

int endstroke(int c);


window* window::listhead; //Выделение памяти для статических
window* window::listtail; //переменных класса window



//Конструктор окна
window::window(int x, int y, int h, int w)
{

// параметры по умолчанию
title = "";
height = min(h, SCRNHT);
width = min(w, SCRNWIDTH);
col = max(0, min(x, SCRNWIDTH-width));
row = max(0, min(y, SCRNHT-height));
curx = 0;
cury = 0;
select = 1;
set_border(0);
visible = 0;
hidden = 1;
prev = next = NULL;
fhead = ftail = NULL;
color[BORDER]=color[TITLE]=color[NORMAL] =
				 clr(BLACK, WHITE, BRIGHT);
color[ACCENT] = clr(WHITE, BLACK, DIM);
if ((sav = new char[width * height * 2]) == (char *) 0)
	     exit(0);
insert_mode = TRUE;
add_list();
}


//Установка типа рамки
void window::set_border(int bt)
{
switch (bt) {
	case 0:
		NW='┌'; NE='┐'; SE='┘'; SW='└';
		SIDE='│'; LINE='─'; 	   //одинарная линия
		break;
	case 1:
		NW='╔';	NE='╗';	SE='╝';	SW='╚';
		SIDE='║'; LINE='═'; 	   //двойная линия
		break;
	case 2:
		NW='╓';	NE='╖';	SE='╜';	SW='╙';
		SIDE='║'; LINE='─';	   //одинарный верх, двойные бока
		break;
	case 3:
		NW='╒';	NE='╕';	SE='╛';	SW='╘';
		SIDE='│'; LINE='═';	   //двойной верх, одинарные бока
		break;
	case 4:
		NW='┬';	NE='┬';	SE='┘';	SW='└';
		SIDE='│'; LINE='─'; 	   //выталкиваемое вниз меню
	}
}



// установить цвета
void window::set_colors(int area, int bg, int fg, int inten)
{

if (area == ALL)
	while (area)
		color[--area] = clr(bg, fg, inten);
else
	color[area] = clr(bg, fg, inten);
pnormal = color[NORMAL];
}




//Вывод окна на экран
void window::display(void)
{
if (hidden)
	{
	remove_list();
	beg_list();
	visible = 1;
	hidden = 0;
	wsave();
	clear();
	wframe();
	wcursor(0,0);
	}
}



//Деструктор окна
window::~window()
{
if (hidden)
	{
	init_template();
	delete []sav;
	remove_list();
	}
}




// скрыть окно
void window::hide()
{

if (visible)
	{
	remove_list();
	add_list();
	hidden = 1;
	visible = 0;
	wrstr();
	}
}



// очистить область окна
void window::clear(void)
{

byte atr=color[NORMAL];
byte xb=col + 1;
byte yb=row + 1;
byte xe=col + width - 2;
byte ye=row + height - 2;


asm 	{
	mov ah, 6
	mov al, 0
	mov bh, atr
	mov cl, xb
	mov ch, yb
	mov dl, xe
	mov dh, ye
	int 16
	}
wcursor(0,0);
}



//изобразить окно
void window::wframe()
{
register int x1, y1;


//заголовок окна
displ(0, 0, color[BORDER], NW);
dtitle();
displ(width-1, 0, color[BORDER], NE);
// боковые стороны окна
for (y1 = 1; y1 < height-1; y1++)
	{
	displ(0, y1, color[BORDER], SIDE);
	displ(width-1, y1, color[BORDER], SIDE);
	}
//низ окна
displ(0, y1, color[BORDER], SW);
for (x1 = 1; x1 < width-1; x1++)
	displ(x1, y1, color[BORDER], LINE);
displ(x1, y1, color[BORDER], SE);
}


//Вывод заголовка окна
void window::dtitle(void)
{
int x1 = 1, i, ln;
char *s = title;


if (s)
	{
	ln = strlen(s);
	if (ln > width-2)
		i = 0;
	else
		i = ((width-2-ln) / 2);
	if (i > 0)
		while (i--)
			displ(x1++, 0, color[BORDER], LINE);
	while (*s && x1 < width-1)
		displ(x1++, 0, color[TITLE], *s++);
	}
while (x1 < width-1)
	displ(x1++, 0, color[BORDER], LINE);
     }




//Вывод форматированной строки в окно
void window::wprintf(char *ln, ...)
{

char dlin [100], *dl = dlin;

if (visible)
	{
	va_list ap;
	va_start(ap, ln);
	vsprintf(dlin, ln, ap);
	va_end(ap);
	while (*dl)
		wputchar(*dl++);
	}
}

//Вывод символа в окно
void window::wputchar(int c)
{


if (visible)
{
switch (c)
	{
	case '\n':
		if (cury == height-3)
			scroll(UPSCROLL);
		else
			cury++;
		curx = 0;
		wcursor(curx, cury);
		break;
	case '\t':
		do
			displ((curx++)+3, cury+1, color[NORMAL], ' ');
		while ((curx%TABS) && (curx+1) < width-1);
		wcursor(curx, cury);
		break;
	default: ;
		if ((curx+1) < width-1)
			{
			displ(curx+1, cury+1, color[NORMAL], c);
			curx++;
			}
		wcursor(curx, cury);
		break;
	}
}
}


// установить курсор окна
void window::wcursor(int x, int y)
{

if (visible && x < width-1 && y < height-1)
	{
	curx = x;
	cury = y;
	setcursor(col+x+1, row+y+1);
	}
}



//произвести оконный выбор
int window::get_selection(void)
{
int c = 0;

select = 1;
while (c != ESC && c != ENTER && c != LEFT && c != RIGHT)
	{
	accent();
	c = get_key();
	deaccent();
	switch (c)
		{
		case UP:
			if (select > 1)
				select--;
			else
				select = height-2;
			break;
		case DOWN:
			if (select < height-2)
				select++;
			else
				select = 1;
			break;
		}
	}
return  c == ENTER ?  select : c == ESC ? 0 : c;
}





// прокручивает содержимое окна вверх или вниз
void window::scroll(byte dir)
{

byte atr=color[NORMAL];
byte xb=col + 1;
byte yb=row + 1;
byte xe=col + width - 2;
byte ye=row + height - 2;

if (visible)
	{
	asm 	{
		mov ah, dir
		mov al, 1
		mov bh, atr
		mov cl, xb
		mov ch, yb
		mov dl, xe
		mov dh, ye
		int 16
		}
	}
}



//сохранить предыдущее содержимое экрана
void window::wsave(void)
{
int *bf=(int*) sav;
int offs=((row*80)+col)*2;
int h=height;
int w=width;
int dsi=(80-w)*2;

asm	{
	mov si, offs
	mov di, bf
	push es
	mov ax, ds
	mov es, ax
	push ds
	mov ax, 0b800h
	mov ds, ax
	push cx
	mov cx, h
	}
l:
asm	{
	push cx
	mov cx, w
	rep movsw
	add si, dsi
	pop cx
	loop l
	pop cx
	pop ds
	pop es
	}

}


// восстанавливает видеопамять из буфера сохранения

void window::wrstr (void)
{
int *bf=(int*) sav;
int offs=((row*80)+col)*2;
int h=height;
int w=width;
int ddi=(80-w)*2;

asm	{
	mov di, offs
	mov si, bf
	push es
	mov ax, 0b800h
	mov es, ax
	push cx
	mov cx, h
	}
l:
asm	{
	push cx
	mov cx, w
	rep movsw
	add di, ddi
	pop cx
	loop l
	pop cx
	pop es
	}

}




// заменяет яркость строки, указываемой select
void window::acline(int set)
{
int x, ch;


for (x = 1; x < width - 1; x++)
	{
	ch = dget(x, select) & 255;
	displ(x, select, set, ch);
	}

}


// ФУНКЦИИ ОБРАБОТКИ СПИСКА

// добавляет окно в конец списка
void window::add_list(void)
{
if (listtail)
	{
	prev = listtail;
	listtail->next = this;
	}
listtail = this;
if (!listhead)
	listhead = this;
}

// добавляет окно в начало списка
void window::beg_list(void)
{
if (listhead)
	{
	next = listhead;
	listhead->prev = this;
	}
listhead = this;
if (!listtail)
	listtail = this;
}

// удаляет окно из списка
void window::remove_list(void)
{
if (next)
	next->prev = prev;
if (prev)
	prev->next = next;
if (listhead == this)
	listhead = next;
if (listtail == this)
	listtail = prev;
next = prev = NULL;
}





window *ewnd=NULL;

// сообщение об ошибках
void error_message(char *s)
{
ewnd=new window(50, 22, 3, max(10, strlen(s)+2));
ewnd->set_colors(ALL, RED, YELLOW, BRIGHT);
ewnd->set_title(" Ошибка! ");
ewnd->display();
ewnd->wprintf(s);

putchar(BELL);
}


void clear_message()
{
if (ewnd)
	ewnd->hide();
	delete ewnd;
ewnd = NULL;
}



//Функции обработки данных
//Инициализция шаблона
void window::init_template(void)
{
field *fld=fhead, *fl;

while (fld)
	{
	fl = fld->fnxt;
//	fld->~field;
	fld = fl;
	}
fhead = NULL;
}



//установка полей шаблона
field* window::establish_field(int cl, int rw, char *msk, char *bf)

{
field *fld=new field(cl, rw, msk, bf);

addfield(fld);
return fld;
}



//добавление поля в список
void window::addfield(field *fld)
{
if (ftail)
	{
	fld->fprv = ftail;
	ftail->fnxt = fld;
	}
ftail = fld;
if (!fhead)
	fhead = fld;
}


//отображение данных в полях
void window::disp_field(char *bf, char *msk)
{
while (*msk)
	{
	wputchar(*msk != FIELDCHAR ? *msk : *bf++);
	msk++;
	}
}



//отображение значений данных в полях
void window::data_value(field *fld)
{
wcursor(fld->fcol, fld->frow);
disp_field(fld->fbuff, fld->fmask);
}



//отображение всех полей в окне
void window::field_tally()
{
field *fld= fhead;

while (fld != NULL)
	{
	data_value(fld);
	fld = fld->fnxt;
	}
}



//очистка всех полей шаблона
void window::clear_template()
{
field *fld= fhead;
char *bf, *msk;

while (fld != NULL)
	{
	bf = fld->fbuff;
	msk = fld->fmask;
	while (*msk)
		{
		if (*msk == FIELDCHAR)
			*bf++ = ' ';
		msk++;
		}
	fld = fld->fnxt;
	}
field_tally();
}





//установка режима вставки/замены курсора
void window::insert_status()
{
set_cursor_type(insert_mode ? 0x0607:0x0106);
}


//чтение поля с клавиатуры
int window::read_field(field *fld)
{
char *mask = fld->fmask, *buff = fld->fbuff;
int done = FALSE, c, column, bcol;

column = fld->fcol;
while (*mask != FIELDCHAR)
	{
	column++;
	mask++;
	}
bcol=column;
//Обработка клавиатуры
while (TRUE)
	{
	wcursor(column, fld->frow);
	c = get_key();
	clear_message();
	switch (c)
		{
		case BS:   //Удаление символа слева от курсора
			if (column == bcol)
				break;
			if (insert_mode)
				{
				movmem(buff, buff-1, strlen(buff));
				*(buff+strlen(buff)-1) = ' ';
				}
			else
				*(buff-1) = ' ';
			do
				{
				column--;
				mask--;
				} while (mask!=fld->fmask &&
					 *mask != FIELDCHAR);
			buff--;
			wcursor(column, fld->frow);
			disp_field(buff, mask);
			break;
		case DEL:  //Удаление символа в позиции курсора
			if (insert_mode)
				{
				movmem(buff+1, buff, strlen(buff));
				*(buff+strlen(buff)) = ' ';
				}
			else
				*buff = ' ';
			disp_field(buff, mask);
			break;
		case RIGHT:  //Сдвинуть курсор вправо
			if (*(mask+1))
				{
				do
					{
					column++;
					mask++;
					} while (*mask &&
						*mask != FIELDCHAR);
				buff++;
				}
			break;
		case LEFT:   //Сдвинуть курсор влево
			if (column>bcol)
				{
				do
					{
					column--;
					mask--;
					} while (mask!=fld->fmask &&
						 *mask != FIELDCHAR);
				buff--;
				}
			break;
		case INS:    //Переключение режимов замена/вставка
			insert_mode ^= TRUE;
			insert_status();
			break;
		default:
			if (endstroke(c)) //Проверка на окончание ввода
				{
				done = TRUE;
				break;
				}
			if ((char)c == 0)
				break;
			//Отображение символа на экране
			if (insert_mode)
				{
				movmem(buff, buff+1, strlen(buff)-1);
				disp_field(buff, mask);
				wcursor(column, fld->frow);
				}
			*buff++ = c;
			wputchar(c);
			do
				{
				column++;
				mask++;
			} while (*mask && *mask != FIELDCHAR);
			if (! *mask)
				c = DOWN;
			break;
		}
	if (!*mask)
		done = TRUE;
	if (done)
		break;
	}

wcursor(fld->fcol, fld->frow);
disp_field(fld->fbuff, fld->fmask);

return c;
}



//Процесс ввода данных в шаблон окна
int window::data_entry()
{
field *fld;
int exitcode, isvalid, done=FALSE;

field_tally();
fld = fhead;
//--накопление данных, поступающих с клавиатуры на экране
while (fld != NULL && done == FALSE)
	{
	wcursor(fld->fcol, fld->frow);
	if (fld->fprot ==FALSE)
		{
		reverse_video();
		data_value(fld);       //Ввод в поле
		wcursor(fld->fcol, fld->frow);
		exitcode = read_field(fld);
		isvalid = (exitcode != ESC && fld->fvalid) ?
			(*(fld->fvalid))(fld->fbuff) : OK;
		}
	else
		{
		exitcode = DOWN;
		isvalid = OK;
		}
	if (isvalid == OK)
		{
		normal_video();
		data_value(fld);
		switch (exitcode)
			{     // передано редактору
			case DOWN: //Сдуущее поле
			case TAB:
				fld = fld->fnxt;
				if (fld == NULL)
					fld = fhead;
				break;
			case UP:   //Предыдущее поле
				fld = fld->fprv;
				if (fld == NULL)
					fld = ftail;
				break;
			case ENTER:  //Завершение ввода
			case ESC:
				done = TRUE;
				break;
			}
		}
	}
return exitcode;
}



//отображение приглашения к вводу
void window::wprompt(int x, int y, char *s)
{
wcursor(x, y);
wprintf(s);
}



//Конструктор поля ввода
void field::field(int cl, int rw, char *msk, char *bf)
{
fmask = msk;
frow = rw;
fcol = cl;
fbuff = bf;
fprot = 0;
fnxt = fprv = NULL;
fvalid  =NULL;
}


//проверка значения с на код клавиши завершения
int endstroke(int c)
{
switch (c)
	{
	case ENTER:
	case ESC:
	case UP:
	case DOWN:
	case TAB:
	    return TRUE;
	default:
	    return FALSE;
   }
}




//Конструктор меню
tmenu::tmenu()
{
counthitem=4;

strcpy(hmenu[0].name,"Файл");
hmenu[0].countvitem=2;
strcpy(hmenu[0].vmenu[0],"Печать");
strcpy(hmenu[0].vmenu[1],"Выход");

strcpy(hmenu[1].name,"Редактор");
hmenu[1].countvitem=4;
strcpy(hmenu[1].vmenu[0],"Отмена");
strcpy(hmenu[1].vmenu[1],"Редактировать");
strcpy(hmenu[1].vmenu[2],"Добавить");
strcpy(hmenu[1].vmenu[3],"Удалить");

strcpy(hmenu[2].name,"Поиск");
hmenu[2].countvitem=1;
strcpy(hmenu[2].vmenu[0],"Поиск");

strcpy(hmenu[3].name,"?");
hmenu[3].countvitem=2;
strcpy(hmenu[3].vmenu[0],"Клавиши");
strcpy(hmenu[3].vmenu[1],"О программе");
}


//Отображение и обработка меню
int tmenu::menu_select(void)
{

byte sx, sy;
int i=FALSE;
hsel=1;
getcursor(&sx, &sy, NULL);
setcursor(0, 26);
open_menu();       //Отображение горизонтального меню

while ((hsel = gethmenu()) != 0)  //Выбор в гор. меню
	{
	vsel = 1;
				   //Выбор в вертикальном меню
	if ((vsel = getvmn()) != 0)
		{
		i=TRUE;
		break;
		}
	}
menuwnd->hide();
delete menuwnd;
setcursor(sx, sy);
return i;
}


//Инициализация горизонтального меню
void tmenu::open_menu()
{
int i;

menuwnd = new window(0, 0, 3, 80);
menuwnd->set_colors(ALL, BLUE, AQUA, BRIGHT);
menuwnd->set_colors(ACCENT, WHITE, BLACK, DIM);
menuwnd->display();
for (i=0;i<counthitem;i++)
	menuwnd->wprintf("%-12.12s", hmenu[i].name);
setcursor(0, 26);
}




//Выборка из горизонтальногоь меню
int tmenu::gethmenu()
{
int sel;
light(1);
while (TRUE)
	{
	switch (sel = get_key())
		{
		case RIGHT:
		case LEFT:
			haccent(sel);
			break;
		case ESC:
			return 0;
		case ENTER:
			return hsel;
		default:
			putchar(BELL);
			break;
		}
	}
}



//Всплывающее вертикальное меню
int tmenu::getvmn()
{
window *vmenuwnd;
menuhitem *hi;
int ht = 10, wd = 20, r, i,j;
char **mp;
while (1)
	{
	dimension(&ht, &wd);
	vmenuwnd = new window(2+(hsel-1)*12, 2, ht, wd);
	vmenuwnd->set_colors(ALL, BLUE, AQUA, BRIGHT);
	vmenuwnd->set_colors(ACCENT, WHITE, BLACK, DIM);
	vmenuwnd->set_border(4);
	vmenuwnd->display();
	hi=hmenu+hsel-1;
	i=hi->countvitem;
	r=0;
	for(j=0;j<i;j++)
		{
		vmenuwnd->wcursor(0, r++);
		vmenuwnd->wprintf("%s", hi->vmenu[j]);
		}
	vsel = vmenuwnd->get_selection();
	vmenuwnd->hide();
	delete vmenuwnd;
	if (vsel == RIGHT || vsel == LEFT)
		{
		haccent(vsel);
		vsel = 1;
		}
	else
		return vsel;
	}
}


//Управление отображением выбранных элементов
//горизонтального меню
void tmenu::haccent(int sel)
{
switch (sel)
	{
	case RIGHT:
		light(0);
		if (hsel == counthitem)
			hsel = 1;
		else
			hsel++;
		light(1);
		break;
	case LEFT:
		light(0);
		if (hsel == 1)
			hsel=counthitem;
		else
			--hsel;
		light(1);
		break;
	default:
		break;
	}
hsel;
}


//Вычисление высоты и ширины меню
void tmenu::dimension(int *ht, int *wd)
{
*wd = 0;
menuhitem *hi=hmenu+hsel-1;
for (int j=0;j<hi->countvitem;j++)
	*wd = max(*wd, strlen(hi->vmenu[j]));
*ht = hi->countvitem+2;
*wd += 2;
}


//Отображение в соответствии с параметром
//d элемента горизонтального меню
void tmenu::light(int d)
{
if (d)
	menuwnd->reverse_video();
menuwnd->wcursor((hsel-1)*12, 0);
menuwnd->wprintf(hmenu[hsel-1].name);
menuwnd->normal_video();
setcursor(0, 26);
}
