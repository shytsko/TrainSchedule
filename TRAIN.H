//Листинг А.3
//Заголовочный файл train.h

#ifndef _TRAIN_H_
#define _TRAIN_H_
#include <time.h>
#include "twindow.h"


//Признаки дней следования
struct BDAYS
{
unsigned pn:1;        //Понедельник
unsigned vt:1;        //Вторник
unsigned sr:1;        //Среда
unsigned cht:1;       //Четверг
unsigned pt:1;        //Пятница
unsigned sb:1;        //Суббота
unsigned vs:1;        //Воскресенье
unsigned even:1;      //По четным числам
unsigned odd:1;       //По нечетным числам
unsigned all:1;       //Ежедневно
};



union DAYS	      //Обединение для доступа
{                     //к признакам дней следования
BDAYS bdays;          //различными способами
int   idays;
};



//Класс записи расписания
class train
{
public:
	char num[4];         //Номер
	char dir[31];        //Маршрут
	DAYS days;           //Дни следования
	char time[5];        //Время
	double cost;         //Стоимость билета
	unsigned emp_seats;  //Количество свободных мест
	char str[81];        //Массив для представления записи
			     //в виде строки
	struct train *next;  //Указатель на следующую запись
	struct train *prev;  //Указатель  на предыдущую запись
	train(void);         //Конструктор
	int edit(void);      //редактирование
	void trtostr(void);  //формирование стороки
	void viewinfo(void); //Вывод информации
};


class sched
{
	train *head;		     //Указатель на список записи
	train *curtr;                //Указатель на текущую запись
	unsigned counter;            //Счетчик записей
	enum
	{noundo, add, edit, del
		} undopr; 	     //Признак последней
				     //операции редактирования
	train *undotr;               //Указатель для отмены действия
	train editundo;              //Сохранение редактируемой записи
	int add_del_pr;		     //Признак возможности отмены действия
	tm *curtime;     	     //Текущее время
	int ex;                      //Признак завершения работы
	tmenu mainmenu;              //Меню
	char fname[13];              //Имя файла для сохранения
	void add_train(train *tr);   //Добавить запись в список
	void remove_train(train *tr);//Удаление записи из списка
	void load(const char *filename);//Загрузка расписания из файла
	void save(const char *filename);//Сохранение расписания в файл
public:
	sched(const char *filename); //Конструктор
	~sched();                    //Деструктор
	void addnewtrain(void);      //Добавление новой записи
	void editcurtr(void);        //Редактирование текущей записи
	void print(void);            //Вывод на печать
	void search(void);           //Поиск
	void deletecurtr(void);      //удаление текущей записи
	void undo (void);            //Отмена последнего действия
	void about(void);            //Вывод информации о программе
	void key(void);              //Вывод справки
	void menu();                 //Вызов меню
	void quit(void);             //Выход
	void run(void);              //Запуск
};



#endif