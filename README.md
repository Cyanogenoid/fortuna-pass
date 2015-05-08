# fortuna-pass
Somewhat Secure Password Manager for the LaFortuna AVR board.

## Features
- One master password to rule them all.
- Browse the filesystem on the SD card to select the password you want to use.
- Pretends to be a keyboard so that it can send the password directly to your computer. No need to type it in manually!

## Security
- [x] Uses AES-256 for storing passwords
- [x] Uses the insecure ECB mode of operation
- [x] Keeps your master password floating somewhere in memory
- [x] Keeps the passwords you used floating somewhere in memory
- [ ] Written by a cryptography expert

## Build Instructions
    git submodule init
    git submodule update
    make upload

## Adding Passwords
Requires Python 2 and PyCrypto.
`encrypt.py` takes the AES password as argument and outputs the encrypted stdin on stdout.
Pipe it into a file on the SD card that stick into the LaFortuna and you're good to go.
Example: `python encrypt.py "NESW" <<< "oh no my secret is out!" > foo.txt`

## Attribution
- `dirtree.*` by @VisualMelon with permission
- FortunaOS by Klaus-Peter Zauner, adapted from Steve Gunn (taken from exercise 6 and lcd library from exercise 7)
- AVR-Crypto-Lib by Das Labor
- LUFA USB stack by Dean Camera
- Remaining code, unless otherwise listed, might be mine
