//���⨭� �.1
//���������� 䠩� twindow.h

#ifndef _TWINDOW_H_
#define _TWINDOW_H_

#include "biosio.h"

// 梥�


#define RED   4
#define GREEN 2
#define BLUE  1
#define WHITE (RED+GREEN+BLUE)
#define YELLOW (RED+GREEN)
#define AQUA (GREEN+BLUE)
#define MAGENTA (RED+BLUE)
#define BLACK 0
#define BRIGHT 8
#define DIM 0

#define BORDER 0
#define TITLE  1
#define ACCENT 2
#define NORMAL 3
#define ALL 4

#define TRUE 1
#define FALSE 0
#define ERROR -1
#define OK 0

#define FIELDCHAR '_'

//���ࠢ����� �ப��⪨
#define UPSCROLL 6
#define DOWNSCROLL 7


//����� ���� ����� ������
class  field
	{
public:
	char *fmask;          // ��᪠ ���� ����� ������
	int fprot;            // ���� ����
	char *fbuff;          // ���� ����
	int frow;             // ��ப� ����
	int fcol;             // �⮫��� ����
	int (*fvalid)(char *);// �㭪�� �஢�ન ���४⭮�� ������

	struct field *fnxt;   // ᫥���饥 ���� �뤠�
	struct field *fprv;   // �।��饥 ���� �뤠�

	field(int cl, int rw, char *msk, char *bf);//���������
	void field_window(char *, int, int);
	inline void protect(int s)   {fprot=s;};
	inline void validate(int (*v)(char *))  {fvalid=v;};
	};


//����� ����
class window
	{
	int visible;       // ��⨭�, �᫨ ���� ������
	int  hidden;       // ��⨭�, �᫨ ���� ����
	char *sav;         // 㪠�뢠�� �� ���� ��࠭���� ����
	char *title;       // 㪠�뢠�� �� ��������� ����
	int  col;          // ���न��� x ������ ���孥�� 㣫�
	int  row;          // ���न��� y ������ ���孥�� 㣫�
	int  width;        // �ਭ� ����
	int  height;       // ���� ����
	int  curx;        // ������  �  �����
	int  cury;        // ������ y �����
	int  select;       // 㪠��⥫� �롮�
	int  color[4];    // 梥� ����
	int  pnormal;      // �।��騩 ��ଠ��� 梥�
	window *next;      // 㪠�뢠�� �� ᫥���饥 ����
	window *prev;      // 㪠�뢠�� �� �।��饥 ����
	field  *fhead;     // 㪠�뢠�� �� 1-� ���� ����� ������
	field  *ftail;     // 㪠�뢠�� �� ��᫥���� ���� ����� ������
	int insert_mode; // ०�� ��⠢��, ���/����


	struct 	{
		char nw, ne, se, sw, side, line;
	} wcs;           //������� ࠬ��

	static window *listhead;//㪠��⥫� �� ��砫� ᯨ᪠
	static window *listtail;//�����⥫� �� ����� ᯨ᪠

	void acline(int); //������� �મ��� ��ப�, 㪠�뢠���� select
	inline int clr(int bg,int fg,int in)
		{return ((fg)|(bg<<4)|in);};//�ନ஢���� ���� 梥⮢
	inline int vad(int x,int y)
		{return((y)*160+(x)*2);};//���᫥��� ᬥ饭�� � ����������
	inline void displ(int x,int y,int a,int c)
		{vpoke(x+col, y+row, a, c);};//�뢮� ᨬ���� � �����������
	inline int dget(int x,int y)
		{return vpeek(x+col,y+row);};
					//���뢠��� ᨬ���� �� ����������
	void wframe(void);              //����ࠦ���� ����
	void dtitle(void);              //�뢮� ���������
	void wsave(void);               //��࠭���� ��࠭�
	void wrstr(void);               //����⠭������� ��࠭�
	void add_list(void);            //���������� ���� � ᯨ᮪
	void beg_list(void);            //���������� ���� � ��砫� ᯨ᪠
	void remove_list(void);         //�������� ���� �� ᯨ᪠
	void addfield(field *fld);      //���������� ���� �����
	void disp_field(char *bf, char *msk);//�⮡ࠦ���� ������ � ����
	void data_value(field *fld);
	void insert_status();           //�஢�ઠ ०��� �����
	int read_field(field *fld);     //�⥭�� ���� � ����������


public:
	window(int, int, int, int);     //���������
	~window ();                     //��������
	void set_border(int bt);        //��⠭���� ⨯� ࠬ��
	void set_colors(int, int, int, int); //��⠭���� 梥⮢
	void set_title(char *t) {title = t;};//��⠭���� ���������
	void display(void);             //�⮡ࠦ���� �� ��࠭�
	void hide(void);                //������
	void wprintf(char *, ...);      //�뢮� �ଠ�஢����� ��ப�
	void wputchar(int);             //�뢮� ᨬ����
	void clear(void);               //���⪠
	void wcursor(int, int);         //��⠭���� �����
	int get_selection(void);        //�롮� � ����
	inline void reverse_video(void)
		{color[3]=color[2];};   //�뤥�����
	inline void normal_video(void)
		{color[3]=pnormal;};    //�⬥�� �뤥�����

	void init_template(void);       //���樠������ 蠡����
	field *establish_field(int, int, char *, char *);
					//���������� ���� �����
	void clear_template(void);      //���⪠ 蠡����
	void field_tally(void);         //�⮡ࠦ���� ��� ����� � ����
	int data_entry(void);           //����� ����� � ����
	void wprompt(int, int, char *); //�⮡ࠦ���� �ਣ��襭�� � �����
	inline void accent(void)
		{acline(color[ACCENT]);};//�뤥����� ��ப�
	inline void deaccent(void)
		{acline(color[NORMAL]);};//���⨥ �뤥�����
	void scroll(byte);               //�ப��⪠
	inline int get_height(void)
		{return height;};        //����祭�� ����� ����
	inline int get_width(void)
		{return width;};         //����祭�� �ਭ� ����
	inline int get_select(void)
		{return select;};    //����祭�� ⥪�饣� 㪠��⥫� �롮�
	inline int get_curx(void)
		{return curx;};          //����祭�� ����樨 � �����
	inline int get_cury(void)
		{return cury;};          //����祭�� ����樨 � �����
	inline void selinc(void)
		{select++;};             //���६��� 㪠��⥫� �롮�
	inline void seldec(void)
		{select--;};             //���६��� 㪠��⥫� �롮�
	inline void setsel(int i)
		{select=i;};             //��⠭���� 㪠��⥫� �롮�
	};





//����� �㭪� ����
class menuhitem
{
public:
	char name[10];     //��������� ��ਧ��⠫쭮�� ����
	int countvitem;    //������⢮ ������⮢ � ���⨪��쭮� ����
	char vmenu[10][15];//��ப� ���⨪��쭮�� ����
};




//����� ����
class tmenu
	{
protected:
	menuhitem hmenu[6];    //�㭪�� ��ਧ��⠫쭮�� ����
	int counthitem;        //������⢮ �㭪⮢ � ��ਧ��⠫쭮� ����
	window *menuwnd;       //���� ����

	void open_menu(void);  //���樠������ ��ਧ��⠫쭮�� ����
	int gethmenu(void);    //�롮ઠ �� ��ਧ��⠫쭮��� ����
	int getvmn(void);      //�ᯫ뢠�饥 ���⨪��쭮� ����
	void haccent(int);     //��ࠢ����� �⮡ࠦ����� ��࠭��� ������⮢
				//��ਧ��⠫쭮�� ����
	void dimension(int *ht, int *wd); //���᫥��� ����� � �ਭ� ����
	void light(int d);     //�⮡ࠦ���� � ᮮ⢥��⢨� � ��ࠬ��஬
				//d ������� ��ਧ��⠫쭮�� ����
public:
	int hsel;              //㪠��⥫� ��ਧ��⠫쭮�� ����
	int vsel;              //�����⥫� ���⨪��쭮�� ����

	tmenu();               //���������
	int menu_select(void); //�롮� � ����
	};

#define NW       (wcs.nw)
#define NE       (wcs.ne)
#define SE       (wcs.se)
#define SW       (wcs.sw)
#define SIDE     (wcs.side)
#define LINE     (wcs.line)


void error_message(char *);
void clear_message(void);

#endif