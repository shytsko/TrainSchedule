//���⨭� �.4
//����� train.cpp

#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <string.h>
#include <time.h>
#include "twindow.h"
#include "train.h"
#include "keys.h"



//���ᨢ ��ப, ᮮ⢥������� ��� ᫥�������
const char *day[] =   {"��",
		 "��",
		 "��",
		 "��",
		 "��",
		 "��",
		 "�",
		 "��",
		 "����",
		 "�����",
		 "",
		};



//��������� ����� �ᯨᠭ��
sched::sched(const char *filename)
{
head = NULL;
counter = 0;
undopr = noundo;
add_del_pr = TRUE;
ex = FALSE;

strcpy(fname, filename);
load(filename);
}



//�������� �ᯨᠭ��
void sched::~sched()
{
save(fname);
if (head != NULL)
	{
	while (head->next)
		{
		head = head->next;
		delete head->prev;
		}
	delete head;
	}
}




//���������� ����� �����
void sched::addnewtrain(void)
{
train *tr=new train;

if (tr == NULL)
	{
	error_message("�� 墠⠥� �����!");
	get_key();
	return;
	}

if(tr->edit())
	{
	add_train(tr);
	undotr = tr;
	undopr = add;
	add_del_pr = TRUE;
	}
}


//������஢���� ⥪�饩 �����
void sched::editcurtr(void)
{
editundo = *curtr;
editundo.next = editundo.prev = NULL;
if (curtr->edit())
	{
	remove_train(curtr);
	add_train(curtr);
	undopr = edit;
	undotr = curtr;
	add_del_pr = TRUE;
	}
}



//    �����
void sched::print(void)
{
train *tr = head;

while (tr != NULL)
	{
	fprintf(stdprn, "     %s\n", tr->str);
	tr = tr->next;
	}
}


//     ����
void sched::search(void)
{
window *wnd;
train *tr = head;
train **trmas;
char dirbuf1[] = "                              ";
char dirbuf2[] = "                              ";
char *db1, *db2, *ch;

char datebuf[9];
char datemsk[] = "__:__:____";
char dirmsk[] = "______________________________";
struct tm date;
int c = 0, i;
int sel=0;
int key;
int usl1 = 0, usl2 =0;
time_t ct;

//���� ������ ��� ���᪠
trmas = new train* [counter];
ct = time(NULL);
curtime = localtime(&ct);
sprintf(datebuf, "%02d%02d%04d",
	curtime->tm_mday,
	curtime->tm_mon+1,
	curtime->tm_year+1900);
wnd = new window(10, 5, 10, 60);
wnd->set_colors(ALL, MAGENTA, GREEN, BRIGHT);
wnd->set_colors(ACCENT, WHITE, BLACK, DIM);
wnd->set_title(" ������ ����� ��� ���᪠ ");
wnd->init_template();
wnd->establish_field(25, 1, dirmsk, dirbuf1);
wnd->establish_field(25, 2, datemsk, datebuf);
wnd->display();
wnd->wprompt(5, 1, "�������:");
wnd->wprompt(5, 2, "���:");

if (wnd->data_entry() == ENTER)
	{
	//��ࠡ�⪠ ᯨ᪠
	date.tm_year = atoi(datebuf+4)-1900;
	datebuf[4] = '\0';
	date.tm_mon = atoi(datebuf+2)-1;
	datebuf[2] = '\0';
	date.tm_mday = atoi(datebuf);
	date.tm_hour = 0;
	date.tm_min = 0;
	date.tm_sec = 0;
	date.tm_isdst = 0;
	mktime(&date);
	db1 = dirbuf1 + strspn(dirbuf1, " ");
	if (*db1)
		{
		ch = db1 + strlen(db1)-1;
		while(*ch == ' ' && ch !=db1)
			ch--;
		*(ch+1) = '\0';
		}
	while (tr != NULL)
		{
		strcpy(dirbuf2, tr->dir);
		db2 = dirbuf2 + strspn(dirbuf2, " ");
		usl1 = (int)strstr(db2, db1);
		usl2 = tr->days.bdays.all;
		if (!usl2)
			{
			usl2 = (date.tm_mday%2) ?
					tr->days.bdays.odd:
					tr->days.bdays.even;
			if (!usl2)
				switch (date.tm_wday)
				{
				case 0: usl2 = tr->days.bdays.vs;
					break;
				case 1: usl2 = tr->days.bdays.pn;
					break;
				case 2: usl2 = tr->days.bdays.vt;
					break;
				case 3: usl2 = tr->days.bdays.sr;
					break;
				case 4: usl2 = tr->days.bdays.cht;
					break;
				case 5: usl2 = tr->days.bdays.pt;
					break;
				case 6: usl2 = tr->days.bdays.sb;
				}
			}
		if (usl1 && usl2)
			{
			trmas[c] = tr;
			c++;
			}
		tr = tr->next;
		}
	}
wnd->hide();
delete wnd;
//�뢮� ���ଠ樨
if (c)
	{
	wnd = new window(0, 3, 22, 80);
	wnd->set_colors(ALL, BLUE, GREEN, DIM);
	wnd->set_colors(ACCENT, BLACK, WHITE, BRIGHT);
	wnd->set_title(" �������� ���᪠ ");
	wnd->display();

	for (i=0; i<wnd->get_height()-2 && i<c; i++)
		{
		wnd->wcursor(0, i);
		wnd->wprintf("%s", trmas[i]->str);
		}
	while (key != ESC)
		{
		wnd->accent();
		setcursor(0, 26);
		key = get_key();
		wnd->deaccent();
		switch (key)
			{
			case UP:
				if(sel>0)
					{
					sel--;
					if (wnd->get_select() > 1)
						wnd->seldec();
					else
						{
						wnd->scroll(DOWNSCROLL);
						wnd->wcursor(0, 0);
						wnd->wprintf("%s", tr->str);
						}
					}
					break;
			case DOWN:
				if(sel<c-1)
					{
					sel++;
					if (wnd->get_select() < wnd->get_height()-2)
						wnd->selinc();
					else
						{
						wnd->scroll(UPSCROLL);
						wnd->wcursor(0, wnd->get_height()-3);
						wnd->wprintf("%s", curtr->str);
						}
					}
				break;
			case ENTER:
				trmas[sel]->viewinfo();
				break;
			case F9:
				for(i = 0; i<c; i++)
					fprintf(stdprn, "     %s\n",
						trmas[i]->str);
			}
		}
	wnd->hide();
	delete wnd;
	}
delete []trmas;
}



//�������� ⥪�饩 �����
void sched::deletecurtr(void)
{
remove_train(curtr);
undopr = del;
undotr = curtr;
add_del_pr = TRUE;
}




//�⬥�� ��᫥���� ����⢨�
void sched::undo (void)
{
switch (undopr)
	{
	case noundo:
		return;
	case add:
		remove_train(undotr);
		break;
	case edit:
		remove_train(undotr);
		*undotr = editundo;
		add_train(undotr);
		break;
	case del:
		add_train(undotr);
	}
undopr = noundo;
add_del_pr = TRUE;
}



//�뢮� ���ଠ樨 � �ணࠬ��
void sched::about(void)
{
window *wnd;

wnd = new window(10, 5, 13, 60);
wnd->set_colors(ALL, AQUA, BLACK, DIM);
wnd->display();
wnd->wprompt(5, 1, "�ணࠬ�� �ࠢ����� �ᯨᠭ��� �������");
wnd->wprompt(5, 3, "���ᮢ�� ࠡ��");
wnd->wprompt(5, 4, "�� ���樯���� ���");
wnd->wprompt(5, 5, "��㤥�� ��㯯� 100501");
wnd->wprompt(5, 6, "������� ���筮�� ���祭�� �����");
wnd->wprompt(5, 7, "���쪮 ��������");
wnd->wprompt(5, 9, "2004 �.");
get_key();
wnd->hide();
delete wnd;
}



//�뢮� ���ଠ樨 � �㭪樮������ �������
void sched::key(void)
{
window *wnd;

wnd = new window(10, 5, 13, 60);
wnd->set_colors(ALL, AQUA, BLACK, DIM);
wnd->display();
wnd->wprompt(5, 1, "F1  - ������");
wnd->wprompt(5, 2, "F3  - �������� ����� ������");
wnd->wprompt(5, 3, "F4  - ������஢��� ⥪���� ������");
wnd->wprompt(5, 4, "F5  - ����");
wnd->wprompt(5, 5, "F6  - �⬥�� ��᫥����� ����⢨�");
wnd->wprompt(5, 6, "F8  - ������� ⥪���� ������");
wnd->wprompt(5, 7, "F9  - �����");
wnd->wprompt(5, 8, "F10 - �맮� ����");
get_key();
wnd->hide();
delete wnd;
}


//�맮� ����
void sched::menu(void)
{
if (mainmenu.menu_select())
	{
	switch (mainmenu.hsel)
		{
		case 1: if (mainmenu.vsel==1)
				print();
			else
				quit();
			break;
		case 2: switch (mainmenu.vsel)
				{
				case 1:	undo();
					break;
				case 2:	editcurtr();
					break;
				case 3: addnewtrain();
					break;
				case 4: deletecurtr();
				}
			break;
		case 3: search();
			break;
		case 4: if (mainmenu.vsel==1)
				key();
			else
				about();
			break;
		}
	}
}




//��室
void sched::quit(void)
{
ex = TRUE;
}



//����� ࠡ��� � �ᯨᠭ���
void sched::run(void)
{
window wnd(0, 3, 22, 80);

int c = 0;
int sel;
int i;
train *tr;


wnd.set_colors(ALL, BLUE, GREEN, DIM);
wnd.set_colors(ACCENT, BLACK, WHITE, BRIGHT);
wnd.set_title(" ��ᯨᠭ�� �������� ������� ");
wnd.display();

//���� ��ࠡ�⪨ ����������

while (!ex)
	{
	if (add_del_pr)
		{         //���������� ����
		wnd.clear();
		tr = head;
		for (i=0; i<wnd.get_height()-2 && i<counter; i++)
			{
			wnd.wcursor(0, i);
			wnd.wprintf("%s", tr->str);
			tr = tr->next;
			}
		wnd.setsel(1);
		curtr = head;
		add_del_pr = 0;
		sel = 0;
		}
	wnd.accent();
	setcursor(0, 26);
	c = get_key();
	wnd.deaccent();
	switch (c)
		{
		case UP:   //�।���� ������
			if(sel>0)
				{
				sel--;
				curtr = curtr->prev;
				if (wnd.get_select() > 1)
					wnd.seldec();
				else
					{
					wnd.scroll(DOWNSCROLL);
					wnd.wcursor(0, 0);
					wnd.wprintf("%s", curtr->str);
					}
			}
			break;
		case DOWN:  //�������� ������
			if(sel<counter-1)
				{
				sel++;
				curtr = curtr->next;
				if (wnd.get_select() < wnd.get_height()-2)
					wnd.selinc();
				else
					{
					wnd.scroll(UPSCROLL);
					wnd.wcursor(0, wnd.get_height()-3);
					wnd.wprintf("%s", curtr->str);
					}
				}
			break;
		case ENTER:  //��ᬮ�� ���ଠ樨 � ⥪�饩 �����
			curtr->viewinfo();
			break;
		case F1:     //������
			key();
			break;
		case F3:     //�������� ����� ������
			addnewtrain();
			break;
		case F4:     //������஢��� ⥪���� ������
			curtr->edit();
			wnd.wcursor(0, wnd.get_select()-1);
			wnd.wprintf("%s", curtr->str);
			break;
		case F5:     //����
			search();
			break;
		case F6:     //�⬥�� ��᫥���� ����⢨�
			undo();
			break;
		case F8:     //������� ⥪���� ������
			deletecurtr();
			break;
		case F9:     //����� �ᯨᠭ��
			print();
			break;
		case F10:    	//����
			menu();
			break;
		default:
			putchar(BELL);
			break;
		}
	}
wnd.hide();
}



//���������� ����� � ᯨ᮪
void sched::add_train(train *tr)
{
train *tr1=head;
int t1, t2;

if (tr1 == NULL)
	{
	head=tr;
	}
else
	{
	t1 = atoi(tr->time);
	t2 = atoi(tr1->time);
	while (t1 > t2 && tr1->next != NULL)
		{
		tr1 = tr1->next;
		t1 = atoi(tr->time);
		t2 = atoi(tr1->time);
		}

	if(t1<t2)
		{
		tr->next = tr1;
		tr->prev = tr1->prev;
		tr1->prev = tr;
		if (tr->prev != NULL)
			tr->prev->next = tr;
		if (tr1 == head)
			head = tr;

		}
	else
		{
		tr->prev = tr1;
		tr->next = tr1->next;
		tr1->next = tr;
		if (tr->next != NULL)
			tr->next->prev = tr;
		}
	}
counter++;
}


//�������� ����� �� ᯨ᪠
void sched::remove_train(train *tr)
{
if (tr->next != NULL)
	tr->next->prev = tr->prev;
if (tr->prev != NULL)
	tr->prev->next = tr->next;
if (head == tr)
	head = tr->next;
tr->next = tr->prev = NULL;
counter--;
}



//����㧪� �ᯨᠭ��
void sched::load(const char *filename)
{
train *tr, rtr;
FILE *f;
char lab[] = "Train schedule";
char rlab[20];
unsigned c;

if ((f=fopen(filename, "rb"))==NULL)
	{
	error_message("���������� ������ 䠩�!");
	get_key();
	clear_message();
	}
else
	{
	fread(rlab, sizeof(char), strlen(lab)+1, f);
	if(strcmp(lab, rlab))
		{
		error_message("������ �ଠ� 䠩��!");
		get_key();
		clear_message();
		return;
		}
	fread(&c, sizeof(int), 1, f);
	while(c)
		{
		fread(&rtr, sizeof(train), 1, f);
		rtr.next = rtr.prev = NULL;
		tr = new train;
		*tr = rtr;
		add_train(tr);
		c--;
		}

	}
fclose(f);
}



//���࠭���� �ᯨᠭ��
void sched::save(const char *filename)
{
train *tr = head;
FILE *f;
char lab[] = "Train schedule";
unsigned c=counter;

if ((f=fopen(filename, "wb"))==NULL)
	{
	error_message("���������� ᮧ���� 䠩�!");
	get_key();
	clear_message();
	}
else
	{
	fwrite(lab, sizeof(char), strlen(lab)+1, f);
	fwrite(&c, sizeof(int), 1, f);
	while(c)
		{
		fwrite(tr, sizeof(train), 1, f);
		tr = tr->next;
		c--;
		}
	}
fclose(f);
}


//��������� �����
train::train(void)
{
strcpy(num, "   ");
strcpy(dir, "                              ");
strcpy(time, "0000");
days.idays = 0;
cost = 0.0;
emp_seats = 0;
}



//������஢���� �����
int train::edit(void)
{
window *wnd;
train tr=*this;
char msk[] = "______________________________";
char tmsk[] = "__:__";
int i;
int e = FALSE;


char dbuf[31] = "                              ";
char cbuf[11] = "          ";
char sbuf[4] = "   ";

i=sprintf(dbuf, "%s %s %s %s %s %s %s %s %s %s",
	tr.days.bdays.pn ? day[1] : day[10],
	tr.days.bdays.vt ? day[2] : day[10],
	tr.days.bdays.sr ? day[3] : day[10],
	tr.days.bdays.cht ? day[4] : day[10],
	tr.days.bdays.pt ? day[5] : day[10],
	tr.days.bdays.sb ? day[6] : day[10],
	tr.days.bdays.vs ? day[0] : day[10],
	tr.days.bdays.even ? day[7] : day[10],
	tr.days.bdays.odd ? day[8] : day[10],
	tr.days.bdays.all ? day[9] : day[10]);
for(;i<30;i++)
	dbuf[i] = ' ';
dbuf[30] = '\0';
sprintf(cbuf, "%10.2f", tr.cost);
sprintf(sbuf, "%3d", tr.emp_seats);

wnd = new window(10, 5, 10, 60);
wnd->set_colors(ALL, MAGENTA, GREEN, BRIGHT);
wnd->set_colors(ACCENT, WHITE, BLACK, DIM);
wnd->set_title(" ������஢���� ����� ");
wnd->init_template();
wnd->establish_field(25, 1, msk+27, tr.num);
wnd->establish_field(25, 2, msk, tr.dir);
wnd->establish_field(25, 3, tmsk, tr.time);
wnd->establish_field(25, 4, msk, dbuf);
wnd->establish_field(25, 5, msk+20, cbuf);
wnd->establish_field(25, 6, msk+27, sbuf);
wnd->display();
wnd->wprompt(5, 1, "�����:");
wnd->wprompt(5, 2, "�������:");
wnd->wprompt(5, 3, "�६�:");
wnd->wprompt(5, 4, "��� ᫥�������:");
wnd->wprompt(5, 5, "�⮨����� �����:");
wnd->wprompt(5, 6, "��������� ����:");

if (wnd->data_entry()==ENTER)
	{
	tr.days.bdays.vs = strstr(dbuf, day[0]) ? 1: 0;
	tr.days.bdays.pn = strstr(dbuf, day[1]) ? 1: 0;
	tr.days.bdays.vt = strstr(dbuf, day[2]) ? 1: 0;
	tr.days.bdays.sr = strstr(dbuf, day[3]) ? 1: 0;
	tr.days.bdays.cht = strstr(dbuf, day[4]) ? 1: 0;
	tr.days.bdays.pt = strstr(dbuf, day[5]) ? 1: 0;
	tr.days.bdays.sb = strstr(dbuf, day[6]) ? 1: 0;
	tr.days.bdays.even = strstr(dbuf, day[7]) ? 1: 0;
	tr.days.bdays.odd = strstr(dbuf, day[8]) ? 1: 0;
	tr.days.bdays.all = strstr(dbuf, day[9]) ? 1: 0;
	tr.cost = atof(cbuf);
	tr.emp_seats = atoi(sbuf);
	tr.trtostr();
	*this=tr;
	e = TRUE;
	}
wnd->hide();
delete wnd;
return e;
}


//��ନ஢���� ��ப� � ���ଠ樥� � �����
void train::trtostr(void)
{
char d[31];

sprintf(d, "%s %s %s %s %s %s %s %s %s %s",
	days.bdays.pn ? day[1] : day[10],
	days.bdays.vt ? day[2] : day[10],
	days.bdays.sr ? day[3] : day[10],
	days.bdays.cht ? day[4] : day[10],
	days.bdays.pt ? day[5] : day[10],
	days.bdays.sb ? day[6] : day[10],
	days.bdays.vs ? day[0] : day[10],
	days.bdays.even ? day[7] : day[10],
	days.bdays.odd ? day[8] : day[10],
	days.bdays.all ? day[9] : day[10]);
sprintf(str, "%3s %30s %c%c:%c%c %30s", num, dir,
	time[0], time[1], time[2], time[3],d);
}




//��ᬮ�� ���ଠ樨 � �����
void train::viewinfo()
{
window *wnd = new window(10, 5, 10, 60);;
char dbuf[31] = "                              ";


sprintf(dbuf, "%s %s %s %s %s %s %s %s %s %s",
	days.bdays.pn ? day[1] : day[10],
	days.bdays.vt ? day[2] : day[10],
	days.bdays.sr ? day[3] : day[10],
	days.bdays.cht ? day[4] : day[10],
	days.bdays.pt ? day[5] : day[10],
	days.bdays.sb ? day[6] : day[10],
	days.bdays.vs ? day[0] : day[10],
	days.bdays.even ? day[7] : day[10],
	days.bdays.odd ? day[8] : day[10],
	days.bdays.all ? day[9] : day[10]);


wnd->set_colors(ALL, MAGENTA, GREEN, BRIGHT);
wnd->display();
wnd->wprompt(5, 1, "�����:");
wnd->wprompt(5, 2, "�������:");
wnd->wprompt(5, 3, "�६�:");
wnd->wprompt(5, 4, "��� ᫥�������:");
wnd->wprompt(5, 5, "�⮨����� �����:");
wnd->wprompt(5, 6, "��������� ����:");
wnd->wprompt(25, 1, num);
wnd->wprompt(25, 2, dir);
wnd->wcursor(25,3);
wnd->wprintf("%c%c:%c%c ",
	time[0], time[1],
	time[2], time[3]);
wnd->wprompt(25, 4, dbuf);
wnd->wcursor(25, 5);
wnd->wprintf("%.2f", cost);
wnd->wcursor(25, 6);
wnd->wprintf("%d", emp_seats);
setcursor(0, 26);
get_key();
wnd->hide();
delete wnd;
}

