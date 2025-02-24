//���⨭� �.3
//���������� 䠩� train.h

#ifndef _TRAIN_H_
#define _TRAIN_H_
#include <time.h>
#include "twindow.h"


//�ਧ���� ���� ᫥�������
struct BDAYS
{
unsigned pn:1;        //�������쭨�
unsigned vt:1;        //��୨�
unsigned sr:1;        //�।�
unsigned cht:1;       //��⢥�
unsigned pt:1;        //��⭨�
unsigned sb:1;        //�㡡��
unsigned vs:1;        //����ᥭ�
unsigned even:1;      //�� ��� �᫠�
unsigned odd:1;       //�� ����� �᫠�
unsigned all:1;       //���������
};



union DAYS	      //���������� ��� ����㯠
{                     //� �ਧ����� ���� ᫥�������
BDAYS bdays;          //ࠧ���묨 ᯮᮡ���
int   idays;
};



//����� ����� �ᯨᠭ��
class train
{
public:
	char num[4];         //�����
	char dir[31];        //�������
	DAYS days;           //��� ᫥�������
	char time[5];        //�६�
	double cost;         //�⮨����� �����
	unsigned emp_seats;  //������⢮ ᢮������ ����
	char str[81];        //���ᨢ ��� �।�⠢����� �����
			     //� ���� ��ப�
	struct train *next;  //�����⥫� �� ᫥������ ������
	struct train *prev;  //�����⥫�  �� �।����� ������
	train(void);         //���������
	int edit(void);      //।���஢����
	void trtostr(void);  //�ନ஢���� ��ப�
	void viewinfo(void); //�뢮� ���ଠ樨
};


class sched
{
	train *head;		     //�����⥫� �� ᯨ᮪ �����
	train *curtr;                //�����⥫� �� ⥪���� ������
	unsigned counter;            //���稪 ����ᥩ
	enum
	{noundo, add, edit, del
		} undopr; 	     //�ਧ��� ��᫥����
				     //����樨 ।���஢����
	train *undotr;               //�����⥫� ��� �⬥�� ����⢨�
	train editundo;              //���࠭���� ।����㥬�� �����
	int add_del_pr;		     //�ਧ��� ���������� �⬥�� ����⢨�
	tm *curtime;     	     //����饥 �६�
	int ex;                      //�ਧ��� �����襭�� ࠡ���
	tmenu mainmenu;              //����
	char fname[13];              //��� 䠩�� ��� ��࠭����
	void add_train(train *tr);   //�������� ������ � ᯨ᮪
	void remove_train(train *tr);//�������� ����� �� ᯨ᪠
	void load(const char *filename);//����㧪� �ᯨᠭ�� �� 䠩��
	void save(const char *filename);//���࠭���� �ᯨᠭ�� � 䠩�
public:
	sched(const char *filename); //���������
	~sched();                    //��������
	void addnewtrain(void);      //���������� ����� �����
	void editcurtr(void);        //������஢���� ⥪�饩 �����
	void print(void);            //�뢮� �� �����
	void search(void);           //����
	void deletecurtr(void);      //㤠����� ⥪�饩 �����
	void undo (void);            //�⬥�� ��᫥����� ����⢨�
	void about(void);            //�뢮� ���ଠ樨 � �ணࠬ��
	void key(void);              //�뢮� �ࠢ��
	void menu();                 //�맮� ����
	void quit(void);             //��室
	void run(void);              //�����
};



#endif