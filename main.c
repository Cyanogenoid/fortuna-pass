#include "os.h"
#include <string.h>
#include <stdlib.h>


int check_switches(int);


void main(void) {
    os_init();


    os_add_task( check_switches,  100, 1);

    sei();
    for(;;){}
    
}

int check_switches(int state) {

    if (get_switch_press(_BV(SWN))) {
        char* data = "secret";
        const uint8_t chunks = 1;
        char* data_string = (char*) malloc(16*chunks*sizeof(char) + 1);
        strcpy(data_string, data);

        // create keyschedule
        char* key = "password";
        aes256_ctx_t context = {
            .key = {0},
        };
        aes256_init(&key, &context);

        display_string(data);
        display_string("\n");
        int i;
        for (i = 0; i < 16*chunks; i += 16) {
            aes256_enc(&(data[i]), &context);
        }
        display_string(data);
        display_string("\n");
        for (i = 0; i < 16*chunks; i += 16) {
            aes256_dec(&(data[i]), &context);
        }
        display_string(data);
        display_string("\n");

        free(data_string);
    }

    if (get_switch_press(_BV(SWC))) {
        // TODO load from EEPROM
        // make key
        bigint_word_t e_val = 17;
        uint16_t n_val = 3233;
        bigint_word_t n_arr[] = {n_val & 0xFF, n_val >> 8};
        uint16_t d_val = 2753;
        bigint_word_t d_arr[] = {d_val & 0xFF, d_val >> 8};


        bigint_t e = {
            .length_W = 1,
            .info = 0,
            .wordv = &e_val,
        };
        bigint_t n = {
            .length_W = 2,
            .info = 0,
            .wordv = n_arr,
        };
        bigint_t d = {
            .length_W = 2,
            .info = 0,
            .wordv = d_arr,
        };

        bigint_adjust(&e);
        bigint_adjust(&n);
        bigint_adjust(&d);

        rsa_publickey_t pub_key = {
            .exponent = e,
            .modulus = n,
        };
        rsa_privatekey_t priv_key= {
            .n = 1,
            .modulus = n,
            .components = &d,
        };


        char* secret = "O";
        bigint_length_t bytes = 1+1;
        char* data_string = (char*) malloc(bytes*sizeof(char));
        strcpy(data_string, secret);

        bigint_t data = {
            .length_W = bytes-1,
            .info = 0,
            .wordv = (bigint_word_t*) data_string,
        };
        bigint_adjust(&data);

        display_string("1: ");
        display_string((char*)(data.wordv)); // plaintext
        display_string("\n");

        display_uint16(data.wordv[0]);
        display_char('_');
        display_uint16(data.wordv[1]);
        display_string("\n");

        rsa_enc(&data, &pub_key);
        display_string("2: ");
        display_string((char*)(data.wordv)); // encrypted
        display_string("\n");

        display_uint16(data.wordv[0]);
        display_char('_');
        display_uint16(data.wordv[1]);
        display_string("\n");

        rsa_dec(&data, &priv_key);
        display_string("3: ");
        display_string((char*)(data.wordv)); // decrypted
        display_string("\n");

        display_uint16(data.wordv[0]);
        display_char('_');
        display_uint16(data.wordv[1]);
        display_string("\n");

    }

    return state;
}
