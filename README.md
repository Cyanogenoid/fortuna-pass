# fortuna-pass
Somewhat Secure Password Manager for the LaFortuna AVR board.

## Security
- [x] Uses AES-256 for storing passwords
- [x] Uses the insecure ECB mode of operation
- [x] Keeps your master password floating somewhere in memory
- [x] Keeps the passwords you used floating somewhere in memory
- [ ] Written by a cryptography expert

## Build Instructions
    git submodule init
    git submodule update
    git apply bigint-no-debug.patch
    make upload

## Adding Passwords
Requires Python 2 and PyCrypto.
`encrypt.py` takes the AES password as argument and outputs the encrypted stdin on stdout.
Pipe it into a file on the SD card that stick into the LaFortuna and you're good to go.

## Attribution
- `dirtree.*` by @VisualMelon with permission
- FortunaOS by Klaus-Peter Zauner, adapted from Steve Gunn (taken from exercise 6 and lcd library from exercise 7)
- AVR-Crypto-Lib by Das Labor
- LUFA USB stack by Dean Camera
- Remaining code, unless otherwise listed, might be mine
