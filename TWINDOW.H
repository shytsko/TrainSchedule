//Листинг А.1
//Заголовочный файл twindow.h

#ifndef _TWINDOW_H_
#define _TWINDOW_H_

#include "biosio.h"

// цвета


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

//Направление прокрутки
#define UPSCROLL 6
#define DOWNSCROLL 7


//Класс поля ввода данных
class  field
	{
public:
	char *fmask;          // маска поля ввода данных
	int fprot;            // защита поля
	char *fbuff;          // буфер поля
	int frow;             // строка поля
	int fcol;             // столбец поля
	int (*fvalid)(char *);// функция проверки корректности данных

	struct field *fnxt;   // следующее поле выдачи
	struct field *fprv;   // предыдущее поле выдачи

	field(int cl, int rw, char *msk, char *bf);//Конструктор
	void field_window(char *, int, int);
	inline void protect(int s)   {fprot=s;};
	inline void validate(int (*v)(char *))  {fvalid=v;};
	};


//Класс окна
class window
	{
	int visible;       // истина, если окно видимо
	int  hidden;       // истина, если окно скрыто
	char *sav;         // указывает на блок сохранения окна
	char *title;       // указывает на заголовок окна
	int  col;          // координата x левого верхнего угла
	int  row;          // координата y левого верхнего угла
	int  width;        // ширина окна
	int  height;       // высота окна
	int  curx;        // позиция  х  курсора
	int  cury;        // позиция y курсора
	int  select;       // указатель выбора
	int  color[4];    // цвета окна
	int  pnormal;      // предыдущий нормальный цвет
	window *next;      // указывает на следующее окно
	window *prev;      // указывает на предыдущее окно
	field  *fhead;     // указывает на 1-е поле ввода данных
	field  *ftail;     // указывает на последнее поле ввода данных
	int insert_mode; // режим вставки, ВКЛ/ВЫКЛ


	struct 	{
		char nw, ne, se, sw, side, line;
	} wcs;           //Символы рамки

	static window *listhead;//указатель на начало списка
	static window *listtail;//Указатель на конец списка

	void acline(int); //заменяет яркость строки, указываемой select
	inline int clr(int bg,int fg,int in)
		{return ((fg)|(bg<<4)|in);};//формирование кода цветов
	inline int vad(int x,int y)
		{return((y)*160+(x)*2);};//Вычисление смещения в видеопамяти
	inline void displ(int x,int y,int a,int c)
		{vpoke(x+col, y+row, a, c);};//Вывод символа в видеопамять
	inline int dget(int x,int y)
		{return vpeek(x+col,y+row);};
					//Считывание символа из видеопамяти
	void wframe(void);              //изображение окно
	void dtitle(void);              //Вывод заголовка
	void wsave(void);               //сохранение экрана
	void wrstr(void);               //восстановление экрана
	void add_list(void);            //Добавление окна в список
	void beg_list(void);            //Добавление окна в начало списка
	void remove_list(void);         //Удаление окна из списка
	void addfield(field *fld);      //Допавления поля ввода
	void disp_field(char *bf, char *msk);//отображение данных в поле
	void data_value(field *fld);
	void insert_status();           //Проверка режима ввода
	int read_field(field *fld);     //чтение поля с клавиатуры


public:
	window(int, int, int, int);     //Конструктор
	~window ();                     //Деструктор
	void set_border(int bt);        //Установка типа рамки
	void set_colors(int, int, int, int); //Установка цветов
	void set_title(char *t) {title = t;};//Установка заголовка
	void display(void);             //Отображение на экране
	void hide(void);                //Скрыть
	void wprintf(char *, ...);      //Вывод форматированной строки
	void wputchar(int);             //Вывод символа
	void clear(void);               //Очистка
	void wcursor(int, int);         //Установка курсора
	int get_selection(void);        //Выбор в окне
	inline void reverse_video(void)
		{color[3]=color[2];};   //Выделение
	inline void normal_video(void)
		{color[3]=pnormal;};    //Отмена выделения

	void init_template(void);       //Инициализация шаблона
	field *establish_field(int, int, char *, char *);
					//Добавление поля ввода
	void clear_template(void);      //Очистка шаблона
	void field_tally(void);         //отображение всех полей в окне
	int data_entry(void);           //Процесс ввода в поле
	void wprompt(int, int, char *); //отображение приглашения к вводу
	inline void accent(void)
		{acline(color[ACCENT]);};//Выделение строки
	inline void deaccent(void)
		{acline(color[NORMAL]);};//Снятие выделения
	void scroll(byte);               //Прокрутка
	inline int get_height(void)
		{return height;};        //Получение высоты окна
	inline int get_width(void)
		{return width;};         //Получение ширины окна
	inline int get_select(void)
		{return select;};    //Получение текущего указателя выбора
	inline int get_curx(void)
		{return curx;};          //Получение позиции х курсора
	inline int get_cury(void)
		{return cury;};          //Получение позиции у курсора
	inline void selinc(void)
		{select++;};             //Инкремент указателя выбора
	inline void seldec(void)
		{select--;};             //Декремент указателя выбора
	inline void setsel(int i)
		{select=i;};             //Установка указателя выбора
	};





//Класс пункта меню
class menuhitem
{
public:
	char name[10];     //Заголовок горизонтального меню
	int countvitem;    //Количество элементов в вертикальном меню
	char vmenu[10][15];//Строки вертикального меню
};




//Класс меню
class tmenu
	{
protected:
	menuhitem hmenu[6];    //Пункты горизонтального меню
	int counthitem;        //Количество пунктов в горизонтальном меню
	window *menuwnd;       //Окно меню

	void open_menu(void);  //Инициализация горизонтального меню
	int gethmenu(void);    //Выборка из горизонтальногоь меню
	int getvmn(void);      //Всплывающее вертикальное меню
	void haccent(int);     //Управление отображением выбранных элементов
				//горизонтального меню
	void dimension(int *ht, int *wd); //Вычисление высоты и ширины меню
	void light(int d);     //Отображение в соответствии с параметром
				//d элемента горизонтального меню
public:
	int hsel;              //указатель горизонтального меню
	int vsel;              //Указатель вертикального меню

	tmenu();               //Конструктор
	int menu_select(void); //Выбор в меню
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