import sys
import math
from Crypto.Cipher import AES

aes_password = sys.argv[1]
plaintext = sys.stdin.read()

cipher = AES.new(aes_password.ljust(32, '\x00'))
target_length = int(math.ceil(len(plaintext)/16.0)*16)
ciphertext = cipher.encrypt(plaintext.ljust(target_length, '\x00'))
print(ciphertext)
