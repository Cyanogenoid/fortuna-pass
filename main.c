#include "os.h"
#include <string.h>
#include <stdlib.h>

#define STATE_LOGIN 0
#define STATE_BROWSE 1
#define STATE_UNLOCK 2
#define STATE_SD_WAIT 3
#define STATE_UNLOCKED 4
#define PLACEHOLDER '7'
#define PLACEHOLDER_COUNT 8

int transition(int);
int process_login(void);
int process_browse(void);
int process_unlock(void);
int process_sd_wait(void);
int process_unlocked(void);

void main(void) {
    os_init();

    os_add_task( transition,  100, STATE_SD_WAIT);

    sei();
    for(;;){}
}

int transition(int state) {

    switch (state) {
        case STATE_SD_WAIT:
            return process_sd_wait();
        case STATE_LOGIN:
            return process_login();
        case STATE_BROWSE:
            return process_browse();
        case STATE_UNLOCK:
            return process_unlock();
        case STATE_UNLOCKED:
            return process_unlocked();
    }

}

int process_sd_wait(void) {
    if (get_switch_long(_BV(OS_CD))) {
        display_string("Enter your password:\n");
        // TODO maybe check that FAT can be loaded
        return STATE_LOGIN;
    }

    return STATE_SD_WAIT;
}

int process_login(void) {
    static char pass[256] = {0};
    static uint8_t length = 0;

    // TODO find more compact encoding to allow longer pass
    if (get_switch_press(_BV(SWN))) {
        pass[length++] = 'N';
    }
    if (get_switch_press(_BV(SWE))) {
        pass[length++] = 'E';
    }
    if (get_switch_press(_BV(SWS))) {
        pass[length++] = 'S';
    }
    if (get_switch_press(_BV(SWW))) {
        pass[length++] = 'W';
    }
    // TODO rotary encoder should add letters to pass too

    if (get_switch_press(_BV(SWC))) {
        display_string(pass);
        display_string("\n");
        load_private_key(pass);
        // TODO maybe verify correct pass
        return STATE_BROWSE;
    }

    return STATE_LOGIN;
}

int process_browse(void) {
    // TODO start filesystem navigator
    return STATE_BROWSE;
}

int process_unlock(void) {
    static uint8_t back_count = PLACEHOLDER_COUNT;

    if (get_switch_press(_BV(SWC))) {
        // push out placeholder characters
        char buffer[PLACEHOLDER_COUNT+1] = {PLACEHOLDER};
        send_text(buffer);
    }
    if (get_switch_press(_BV(SWW))) {
        back_count = PLACEHOLDER_COUNT;
        return STATE_BROWSE;
    }
    while (os_enc_delta()) { // TODO figure out how enc_delta works
        // push out backspaces to remove placeholders and no more
    }
    if (back_count == 0) {
        back_count = PLACEHOLDER_COUNT;
        return STATE_UNLOCKED;
    }

    return STATE_UNLOCK;
}

int process_unlocked(void) {
    // load password into memory
    char password[] = {};
    size_t bytes = 0;

    decrypt(password, bytes);
    send_text(password);
    return STATE_BROWSE;
}
