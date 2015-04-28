#include "os.h"
#include <string.h>
#include <stdlib.h>

#define STATE_LOGIN 0
#define STATE_BROWSE 1
#define STATE_UNLOCK 2
#define STATE_SD_WAIT 3


int transition(int);
int process_login(void);
int process_browse(void);
int process_unlock(void);
int process_sd_wait(void);

void main(void) {
    os_init();

    os_add_task( transition,  100, STATE_SD_WAIT);

    sei();
    for(;;){}
}

int transition(int state) {

    switch (state) {
        case STATE_LOGIN:
            return process_login();
        case STATE_BROWSE:
            return process_browse();
        case STATE_UNLOCK:
            return process_unlock();
        case STATE_SD_WAIT:
            return process_sd_wait();
    }

}

int process_sd_wait(void) {
    if (get_switch_long(_BV(OS_CD))) {
        return STATE_LOGIN;
    }

    return STATE_SD_WAIT;
}

int process_login(void) {
    return STATE_LOGIN;
}

int process_browse(void) {
    return STATE_BROWSE;
}

int process_unlock(void) {
    return STATE_UNLOCK;
}
