//���⨭� �.2
//����� twindow.cpp

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


window* window::listhead; //�뤥����� ����� ��� ����᪨�
window* window::listtail; //��६����� ����� window



//��������� ����
window::window(int x, int y, int h, int w)
{

// ��ࠬ���� �� 㬮�砭��
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


//��⠭���� ⨯� ࠬ��
void window::set_border(int bt)
{
switch (bt) {
	case 0:
		NW='�'; NE='�'; SE='�'; SW='�';
		SIDE='�'; LINE='�'; 	   //�����ୠ� �����
		break;
	case 1:
		NW='�';	NE='�';	SE='�';	SW='�';
		SIDE='�'; LINE='�'; 	   //������� �����
		break;
	case 2:
		NW='�';	NE='�';	SE='�';	SW='�';
		SIDE='�'; LINE='�';	   //������� ����, ������ ����
		break;
	case 3:
		NW='�';	NE='�';	SE='�';	SW='�';
		SIDE='�'; LINE='�';	   //������� ����, ������� ����
		break;
	case 4:
		NW='�';	NE='�';	SE='�';	SW='�';
		SIDE='�'; LINE='�'; 	   //��⠫�������� ���� ����
	}
}



// ��⠭����� 梥�
void window::set_colors(int area, int bg, int fg, int inten)
{

if (area == ALL)
	while (area)
		color[--area] = clr(bg, fg, inten);
else
	color[area] = clr(bg, fg, inten);
pnormal = color[NORMAL];
}




//�뢮� ���� �� ��࠭
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



//�������� ����
window::~window()
{
if (hidden)
	{
	init_template();
	delete []sav;
	remove_list();
	}
}




// ����� ����
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



// ������ ������� ����
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



//����ࠧ��� ����
void window::wframe()
{
register int x1, y1;


//��������� ����
displ(0, 0, color[BORDER], NW);
dtitle();
displ(width-1, 0, color[BORDER], NE);
// ������ ��஭� ����
for (y1 = 1; y1 < height-1; y1++)
	{
	displ(0, y1, color[BORDER], SIDE);
	displ(width-1, y1, color[BORDER], SIDE);
	}
//��� ����
displ(0, y1, color[BORDER], SW);
for (x1 = 1; x1 < width-1; x1++)
	displ(x1, y1, color[BORDER], LINE);
displ(x1, y1, color[BORDER], SE);
}


//�뢮� ��������� ����
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




//�뢮� �ଠ�஢����� ��ப� � ����
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

//�뢮� ᨬ���� � ����
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


// ��⠭����� ����� ����
void window::wcursor(int x, int y)
{

if (visible && x < width-1 && y < height-1)
	{
	curx = x;
	cury = y;
	setcursor(col+x+1, row+y+1);
	}
}



//�ந����� ������ �롮�
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





// �ப��稢��� ᮤ�ন��� ���� ����� ��� ����
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



//��࠭��� �।��饥 ᮤ�ন��� ��࠭�
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


// ����⠭�������� ����������� �� ���� ��࠭����

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




// ������� �મ��� ��ப�, 㪠�뢠���� select
void window::acline(int set)
{
int x, ch;


for (x = 1; x < width - 1; x++)
	{
	ch = dget(x, select) & 255;
	displ(x, select, set, ch);
	}

}


// ������� ��������� ������

// �������� ���� � ����� ᯨ᪠
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

// �������� ���� � ��砫� ᯨ᪠
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

// 㤠��� ���� �� ᯨ᪠
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

// ᮮ�饭�� �� �訡���
void error_message(char *s)
{
ewnd=new window(50, 22, 3, max(10, strlen(s)+2));
ewnd->set_colors(ALL, RED, YELLOW, BRIGHT);
ewnd->set_title(" �訡��! ");
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



//�㭪樨 ��ࠡ�⪨ ������
//���樠����� 蠡����
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



//��⠭���� ����� 蠡����
field* window::establish_field(int cl, int rw, char *msk, char *bf)

{
field *fld=new field(cl, rw, msk, bf);

addfield(fld);
return fld;
}



//���������� ���� � ᯨ᮪
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


//�⮡ࠦ���� ������ � �����
void window::disp_field(char *bf, char *msk)
{
while (*msk)
	{
	wputchar(*msk != FIELDCHAR ? *msk : *bf++);
	msk++;
	}
}



//�⮡ࠦ���� ���祭�� ������ � �����
void window::data_value(field *fld)
{
wcursor(fld->fcol, fld->frow);
disp_field(fld->fbuff, fld->fmask);
}



//�⮡ࠦ���� ��� ����� � ����
void window::field_tally()
{
field *fld= fhead;

while (fld != NULL)
	{
	data_value(fld);
	fld = fld->fnxt;
	}
}



//���⪠ ��� ����� 蠡����
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





//��⠭���� ०��� ��⠢��/������ �����
void window::insert_status()
{
set_cursor_type(insert_mode ? 0x0607:0x0106);
}


//�⥭�� ���� � ����������
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
//��ࠡ�⪠ ����������
while (TRUE)
	{
	wcursor(column, fld->frow);
	c = get_key();
	clear_message();
	switch (c)
		{
		case BS:   //�������� ᨬ���� ᫥�� �� �����
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
		case DEL:  //�������� ᨬ���� � ����樨 �����
			if (insert_mode)
				{
				movmem(buff+1, buff, strlen(buff));
				*(buff+strlen(buff)) = ' ';
				}
			else
				*buff = ' ';
			disp_field(buff, mask);
			break;
		case RIGHT:  //�������� ����� ��ࠢ�
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
		case LEFT:   //�������� ����� �����
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
		case INS:    //��४��祭�� ०���� ������/��⠢��
			insert_mode ^= TRUE;
			insert_status();
			break;
		default:
			if (endstroke(c)) //�஢�ઠ �� ����砭�� �����
				{
				done = TRUE;
				break;
				}
			if ((char)c == 0)
				break;
			//�⮡ࠦ���� ᨬ���� �� ��࠭�
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



//����� ����� ������ � 蠡��� ����
int window::data_entry()
{
field *fld;
int exitcode, isvalid, done=FALSE;

field_tally();
fld = fhead;
//--���������� ������, ����㯠��� � ���������� �� ��࠭�
while (fld != NULL && done == FALSE)
	{
	wcursor(fld->fcol, fld->frow);
	if (fld->fprot ==FALSE)
		{
		reverse_video();
		data_value(fld);       //���� � ����
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
			{     // ��।��� ।�����
			case DOWN: //����饥 ����
			case TAB:
				fld = fld->fnxt;
				if (fld == NULL)
					fld = fhead;
				break;
			case UP:   //�।��饥 ����
				fld = fld->fprv;
				if (fld == NULL)
					fld = ftail;
				break;
			case ENTER:  //�����襭�� �����
			case ESC:
				done = TRUE;
				break;
			}
		}
	}
return exitcode;
}



//�⮡ࠦ���� �ਣ��襭�� � �����
void window::wprompt(int x, int y, char *s)
{
wcursor(x, y);
wprintf(s);
}



//��������� ���� �����
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


//�஢�ઠ ���祭�� � �� ��� ������ �����襭��
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




//��������� ����
tmenu::tmenu()
{
counthitem=4;

strcpy(hmenu[0].name,"����");
hmenu[0].countvitem=2;
strcpy(hmenu[0].vmenu[0],"�����");
strcpy(hmenu[0].vmenu[1],"��室");

strcpy(hmenu[1].name,"�������");
hmenu[1].countvitem=4;
strcpy(hmenu[1].vmenu[0],"�⬥��");
strcpy(hmenu[1].vmenu[1],"������஢���");
strcpy(hmenu[1].vmenu[2],"��������");
strcpy(hmenu[1].vmenu[3],"�������");

strcpy(hmenu[2].name,"����");
hmenu[2].countvitem=1;
strcpy(hmenu[2].vmenu[0],"����");

strcpy(hmenu[3].name,"?");
hmenu[3].countvitem=2;
strcpy(hmenu[3].vmenu[0],"������");
strcpy(hmenu[3].vmenu[1],"� �ணࠬ��");
}


//�⮡ࠦ���� � ��ࠡ�⪠ ����
int tmenu::menu_select(void)
{

byte sx, sy;
int i=FALSE;
hsel=1;
getcursor(&sx, &sy, NULL);
setcursor(0, 26);
open_menu();       //�⮡ࠦ���� ��ਧ��⠫쭮�� ����

while ((hsel = gethmenu()) != 0)  //�롮� � ���. ����
	{
	vsel = 1;
				   //�롮� � ���⨪��쭮� ����
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


//���樠������ ��ਧ��⠫쭮�� ����
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




//�롮ઠ �� ��ਧ��⠫쭮��� ����
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



//�ᯫ뢠�饥 ���⨪��쭮� ����
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


//��ࠢ����� �⮡ࠦ����� ��࠭��� ������⮢
//��ਧ��⠫쭮�� ����
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


//���᫥��� ����� � �ਭ� ����
void tmenu::dimension(int *ht, int *wd)
{
*wd = 0;
menuhitem *hi=hmenu+hsel-1;
for (int j=0;j<hi->countvitem;j++)
	*wd = max(*wd, strlen(hi->vmenu[j]));
*ht = hi->countvitem+2;
*wd += 2;
}


//�⮡ࠦ���� � ᮮ⢥��⢨� � ��ࠬ��஬
//d ������� ��ਧ��⠫쭮�� ����
void tmenu::light(int d)
{
if (d)
	menuwnd->reverse_video();
menuwnd->wcursor((hsel-1)*12, 0);
menuwnd->wprintf(hmenu[hsel-1].name);
menuwnd->normal_video();
setcursor(0, 26);
}
