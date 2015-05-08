#include "os.h"
#include <string.h>
#include <stdlib.h>

#define STATE_LOGIN 0
#define STATE_BROWSE 1
#define STATE_UNLOCK 2
#define STATE_SD_WAIT 3
#define STATE_UNLOCKED 4
#define STATE_START 5
#define PLACEHOLDER '7'
#define PLACEHOLDER_COUNT 8
#define FILE_CHUNK_SIZE  16
#define MAX_FILE_SIZE  512

int transition(int);
int process_login(void);
int process_browse(void);
int process_unlock(void);
int process_sd_wait(void);
int process_unlocked(void);
int process_start(void);

void select_file(const char* fname);


static char* filename;
static FIL File;

void main(void) {
    os_init();

    dirtree_set_select_func(select_file);
    os_add_task( transition,  100, STATE_START);

    sei();
    for(;;){}
}

int transition(int state) {

    switch (state) {
        case STATE_START:
            return process_start();
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

int process_start(void) {
    display_string("Waiting for SD card");
    return STATE_SD_WAIT;
}

int process_sd_wait(void) {
    display_string(".");
    if (get_switch_long(_BV(OS_CD))) {
        clear_screen();
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
        display_char('*');
    }
    if (get_switch_press(_BV(SWE))) {
        pass[length++] = 'E';
        display_char('*');
    }
    if (get_switch_press(_BV(SWS))) {
        pass[length++] = 'S';
        display_char('*');
    }
    if (get_switch_press(_BV(SWW))) {
        pass[length++] = 'W';
        display_char('*');
    }
    // TODO rotary encoder should add letters to pass too

    if (get_switch_press(_BV(SWC))) {
        load_private_key(pass);
        // TODO maybe verify correct pass
        return STATE_BROWSE;
    }

    return STATE_LOGIN;
}

volatile uint8_t browse_done = 0;
int process_browse(void) {
    dirtree_show(1);
    static uint8_t dodgy_switch_delay = 0;

    if (get_switch_press(_BV(SWN))) {
        dodgy_switch_delay = 1;
        dirtree_move_up();
    }

    if (get_switch_press(_BV(SWE) | _BV(SWC))) {
        dodgy_switch_delay = 1;
        dirtree_expand();
    }

    if (get_switch_press(_BV(SWS))) {
        dodgy_switch_delay = 1;
        dirtree_move_down();
    }

    if (get_switch_press(_BV(SWW))) {
        dodgy_switch_delay = 1;
        dirtree_contract();
    }

    if (get_switch_rpt(_BV(SWN))) {
        if (dodgy_switch_delay == 0)
            dirtree_move_up();
        else
            dodgy_switch_delay = 0;
    }

    if (get_switch_rpt(_BV(SWS))) {
        if (dodgy_switch_delay == 0)
            dirtree_move_down();
        else
            dodgy_switch_delay = 0;
    }

    int16_t delta = os_enc_delta();
    if (delta < 0) {
        dirtree_move_up();
    } else if (delta > 0) {
        dirtree_move_down();
    }

    if (browse_done) {
        browse_done = 0;
        return STATE_UNLOCK;
    } else {
        return STATE_BROWSE;
    }
}

void select_file(const char* fname) {
    dirtree_show(0);
    clear_screen();
    display_string("Decrypting password...");
    free(filename);
    filename = malloc(sizeof(char)*256);
    strcpy(filename, fname);
    browse_done = 1;
}

int process_unlock(void) {
    // TODO unlock mechanism
    return STATE_UNLOCKED;
}

int process_unlocked(void) {
    // load password into memory
    char* password = malloc(sizeof(char)*MAX_FILE_SIZE);
    size_t bytes = 0;
    f_mount(&FatFs, "", 0);
    int status;
    if ((status = f_open(&File, filename, FA_READ)) == FR_OK) {
        UINT b = 0;
        while(f_read(&File, password+bytes, FILE_CHUNK_SIZE, &b) == FR_OK) {
            if (!b) {
                break;
            }
            bytes += b;
        }
        f_close(&File);
        decrypt(password, bytes);
        send_text(password);
    } else {
        display_string("Failed reading "); 
        display_string(filename);
        display_string(" with error ");
        display_char(status + '0');
        display_string("\n");
        _delay_ms(2000);
    }

    free(password);
    return STATE_BROWSE;
}
