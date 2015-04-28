#include "os.h"
#include <string.h>
#include <stdlib.h>

#define STATE_LOGIN 0
#define STATE_BROWSE 1
#define STATE_UNLOCK 2


int transition(int);
int process_login(void);
int process_browse(void);
int process_unlock(void);

void main(void) {
    os_init();

    os_add_task( transition,  100, STATE_LOGIN);

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
    }

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
