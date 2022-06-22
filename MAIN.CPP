//Листинг А.8
//Модуль main.cpp

#include <stdlib.h>
#include "train.h"
#include "twindow.h"


void main (void)
{
sched sc("schedule.dat");

fill_screen('▒', 7);
sc.run();
fill_screen(' ', 7);
}