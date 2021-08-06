#include <curses.h>


void winitial(void)
{
    initscr();
    clear();
    cbreak();
    noecho();
    nonl();
    keypad(stdscr, TRUE);
    box(stdscr, ACS_VLINE, ACS_HLINE);   /*draw a box on the new win*/
}


void display_iface(void)
{
    curs_set(0);

    attron(A_BOLD);
    mvprintw(1,5," ***      ***      *    *    *****   ****     ***    *    ");
    mvprintw(2,5,"*   *    *   *     * *  *      *     *   *   *   *   *    ");
    mvprintw(3,5,"*       *     *    *  * *      *     *  *   *     *  *    ");
    mvprintw(4,5,"*   *    *   *     *   **      *     *   *   *   *   *    ");
    mvprintw(5,5," ***      ***      *    *      *     *    *   ***    *****");


    mvprintw(6,7, "         /\\                ");
    mvprintw(7,7, "    ____/  \\____       PRESS --> TO INCREASE THE CHANNEL");
    mvprintw(8,7, "   /            \\      PRESS <-- TO DECREASE THE CHANNEL");
    mvprintw(9,7,"   \\___      ___/      PRESS KEY_UP TO INCREASE THE OUTPUT VOLTAGE");
    mvprintw(10,7,"        \\  /           PRESS KEY_DOWN TO DECREASE THE OUTPUT VOLTAGE");
    mvprintw(11,7,"         \\/            q: quit this program ");



    mvprintw(14,5,"/-----------CURRENT STATUE------------------\\ ");
    mvprintw(15,5,"|                 is under control          | ");
    mvprintw(16,5,"|         Voltage is        v               | ");
    mvprintw(17,5,"|                                           | ");
    mvprintw(18,5,"\\-------------------------------------------/ ");

    attroff(A_BOLD);
}

/*
void update_display()
{
comedi_range * comedi_get_range(comedi_t * device, unsigned int subdevice
, unsigned int channel, unsigned int range);

char *choice_result=choices[ch];

rang_output=comedi_get_range(device,subdevice,ch,0);
 maxdata_output=comdedi_get_maxdata(device,subdevice,ch);
 double real_voltage=comedi_to_phys(data,range_output,maxdata_output);


curs_set(0);
mvprintw(18,12,"%s",choice_result);
mvprintw(19,26,"%d",data);*/









