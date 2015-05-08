import sys
import math
from Crypto.Cipher import AES


key_lookup = {
    'N' : 1,
    'E' : 2,
    'S' : 3,
    'W' : 4,
    'L' : 5,
    'R' : 6,
}

def string_to_aes_pass(s):
    buf = bytearray(32)
    low_bits = True
    length = 0

    for c in s:
        if length > 32:
            break
        if low_bits:
            buf[length] |= key_lookup[c];
        else:
            buf[length] |= key_lookup[c] << 4;
            length += 1;
        low_bits = not low_bits
    return bytes(buf)


aes_password = string_to_aes_pass(sys.argv[1])
plaintext = sys.stdin.read()

cipher = AES.new(aes_password)
target_length = int(math.ceil(len(plaintext)/16.0)*16)
ciphertext = cipher.encrypt(plaintext.ljust(target_length, '\x00'))
print(ciphertext)
