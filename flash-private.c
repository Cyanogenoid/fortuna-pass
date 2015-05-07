#include "os.h"
#include <string.h>
#include <stdlib.h>
#include <avr/eeprom.h>

extern char eep_priv_key[];
void main(void) {
    os_init();

    LED_OFF;


    char key[256] = AES_KEY;
    char buffer[PRIVATE_KEY_LENGTH_BYTE] = PRIVATE_KEY;
    aes256_ctx_t context = {{0}};
    aes256_init(key, &context);
    int i;
    for (i = 0; i < PRIVATE_KEY_LENGTH_BYTE; i += 16) {
        aes256_enc(buffer+i, &context);
    }

    eeprom_update_block(buffer, eep_priv_key, PRIVATE_KEY_LENGTH_BYTE);


    LED_ON;
    for(;;){}
}
