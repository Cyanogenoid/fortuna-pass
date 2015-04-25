#include "os.h"


int check_switches(int);


void main(void) {
    os_init();


    os_add_task( check_switches,  100, 1);

    sei();
    for(;;){}
    
}

int check_switches(int state) {

    if (get_switch_press(_BV(SWC))) {
        send_text("the quick brown fox jumps over the lazy dog 0123456789");
    }

}
