#include <stdio.h>
#include <comedilib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <curses.h>

#include "outp_ncurses.h"
#include "display2.h"


// declare helper functions
int  keycontrol( comedi_t *device, unsigned channel, int data );
void update_display( unsigned channel, double voltage );

int main(int argc, char *argv[])
{
    int doloop  = 1;
    int channel = 0;
    
    double voltage;
    
    comedi_set_global_oor_behavior(COMEDI_OOR_NUMBER);
   
    comedi_t *device = comedi_open(COMEDI_FILE);
    
    if( device == NULL )
    {
        comedi_perror(COMEDI_FILE);
        return 1;
    }
    
    // max and range are the same for all channels
    lsampl_t       max_data = comedi_get_maxdata(device, COMEDI_SUB_DEVICE, 0);
    comedi_range  *range    = comedi_get_range(device, COMEDI_SUB_DEVICE, 0, 0);

    int data = max_data/2;

    keycontrol(device, channel, data);

    winitial();
    display_iface();
    
    voltage = comedi_to_phys(data, range, max_data);
    update_display(channel, voltage);

    //----------------------------reading control key--------//

    while(doloop)
    {
        switch( getch() )
        {
        case KEY_UP:
            data += 10;

            if( data > max_data )
            {
                data = max_data;
                mvprintw(17, 15, "NOTE! Peak Voltage!");
            }
            else
            {
                mvprintw(17 ,15, "                      ");
            }

            voltage = comedi_to_phys(data, range, max_data);
            update_display(channel, voltage);
            keycontrol(device, channel, data);
            break;

        case KEY_DOWN:
            data -= 10;

            if( data < 0 )
            {
                data = 0;
                mvprintw(17, 15, "NOTE! Minimum Voltage!");
            }
            else
            {
                mvprintw(17, 15, "                      ");
            }
            
            voltage = comedi_to_phys(data, range, max_data);
            update_display(channel, voltage);
            keycontrol(device, channel, data);
            break;

        case KEY_LEFT:
            channel--;

            if( channel < 0 )
            {
                channel=0;
                mvprintw(17, 15, "NOTE! Out of control!");
            }
            else
            {
                mvprintw(17, 15, "                      ");
            }
            
            voltage = comedi_to_phys(data, range, max_data);
            update_display(channel, voltage);
            keycontrol(device, channel, data);
            break;

        case KEY_RIGHT:
            channel++;

            if( channel >= NUM_CHANNELS )
            {
                channel--;
                mvprintw(17, 15, "NOTE! Out of control!");
            }
            else
            {
                mvprintw(17, 15, "                      ");
            }
            
            voltage = comedi_to_phys(data, range, max_data);
            update_display(channel, voltage);
            keycontrol(device, channel, data);
            break;

        case 113:     // Key Q
            doloop=0;
            break;

        default:
            break;
        }

    } /*while loop is finished*/

    endwin();/*close curse*/
    
    for( channel=0; channel < NUM_CHANNELS; ++channel )
    { 
      keycontrol( device, channel, max_data/2 );
    }
    
    comedi_close(device);

    return 0;
}

//-----------output control------------//

int keycontrol(comedi_t *device, unsigned channel, int data )
{
	int ret = comedi_data_write(device, 
				    COMEDI_SUB_DEVICE, 
				    channel, 
				    0, 
				    0, 
				    data);

	if(ret < 0)
	{
		comedi_perror(COMEDI_FILE);
		exit(1);
	}
    
	return ret;
}

/*-----update display---------*/

void update_display( unsigned channel, double voltage )
{
	const char *choices[] = { "1st ch", "2nd ch", "3rd ch", "4th ch" };

	curs_set(0);
	mvprintw( 15, 12, "          " );
	mvprintw( 15, 12, "%s", choices[channel] );
	mvprintw( 16, 26, "%6.2g", voltage );
	
	refresh();
}
