# fortuna-pass
Somewhat Secure Password Manager for the LaFortuna AVR board.

## Features
- One master password to rule them all.
- Browse the filesystem on the SD card to select the password you want to use.
- Pretends to be a keyboard so that it can send the password directly to your computer. No need to type it in manually!
- Special characters only work if the keyboard language is set to US in your operating system!

## Security
- [x] Uses AES-256 for password encryption
- [x] Uses the insecure ECB mode of operation
- [x] Keeps your master password floating somewhere in memory
- [x] Keeps the passwords you used floating somewhere in memory
- [ ] Written by a cryptography expert

## Usage
Clone the repository and follow build instructions to flash program onto LaFortuna.
Come up with a combination of directional keys and half-circle rotations for the master password and add some passwords as per the Adding Passwords section.
Whenever you want to use it, plug in the LaFortuna with the SD card plugged in, enter your master password and confirm it by clicking the centre button, then select the file corresponding to the password you want.
It will now send your password to the computer it is connected to over USB.
You probably want to disconnect the LaFortuna once you're done.

## Build Instructions
    git submodule init
    git submodule update
    make upload

## Adding Passwords
Requires Python 2 and PyCrypto.
`encrypt.py` takes the AES password as argument and outputs the encrypted plaintext fed in through stdin on stdout.
The allowed characters in passwords are `N`, `E`, `S`, `W` for the directional buttons and `L`, `R` for turning the wheel by 180 degrees left (counter-clockwise) and right (clockwise) respectively.
Pipe the the output into a file on the SD card, stick it into the LaFortuna and you're good to go.
Example: `python2 encrypt.py 'NNESWLLRR' <<< 'oh no my secret is out!' > foo.txt`

## Attribution
- `dirtree.*` by @VisualMelon with permission
- FortunaOS by Klaus-Peter Zauner, adapted from Steve Gunn (taken from exercise 6 and lcd library from exercise 7)
- AVR-Crypto-Lib by Das Labor
- LUFA USB stack by Dean Camera
- Remaining code, unless otherwise listed, might be mine
