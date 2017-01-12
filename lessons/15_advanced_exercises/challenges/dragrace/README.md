Drag Race
---------

Simple reverse the password, and buffer overflow RIP overwrite to shellcode
stored on the executable heap.

# Question Text

```
This bad binary is all T. All Shade.

nc play.spgame.site 1345
```

*Creator -  amon (@nn_amon)*

# Setup Guide

0. Install docker on the hosting system
1. Replace the flag in distribute/flag
2. Build the docker image with: `sh dockerbuild.sh`
3. Replace the port 1345 with your desired port in dockerrun.sh
4. Start the docker image: `sh dockerrun.sh`
5. Test the connectivity with netcat.

# Exploit Details

The binary strcpys up to 512 bytes from a location in the heap onto a stack that
has 128 bytes allocated for a character buffer in the function 'violet'.
However, you need to pass a check on the first 8 bytes of the input. The binary
provides the address of the heap buffer.

If you reverse the binary, you can retrieve the values that pass the check on
the first 8 bytes of the input. Now, you can overwrite RIP by smashing the stack
and place your shellcode at index 8 of the input then jump to that location.

Working exploit is in service/exploit.py
