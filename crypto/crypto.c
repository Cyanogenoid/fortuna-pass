#include "crypto.h"

#include <avr/eeprom.h>


#define PRIVATE_KEY "\x"
#define PRIVATE_KEY_LENGTH 2048

static rsa_privatekey_t priv_key;
char eep_priv_key[PRIVATE_KEY_LENGTH] EEMEM = (char*) PRIVATE_KEY;


void load_private_key(char* key) {
	// load
	size_t length = PRIVATE_KEY_LENGTH/8/sizeof(char);
	char* buffer = (char*) malloc(length);
	eeprom_read_block(buffer, eep_priv_key, length);
	// decrypt with AES
    aes256_ctx_t context = {{0}};
    aes256_init(key, &context);
    size_t i;
    for (i = 0; i < length; i += 16) {
		aes256_dec(buffer[i], &context);
	}
	// make into bigint

}

void decrypt(void* buffer, size_t length_B) {

}
