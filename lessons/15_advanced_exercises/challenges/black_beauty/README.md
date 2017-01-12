Drag Race
---------

Lack of negative number validation and writing data outside of the designated
buffer to overwrite a char array pointer to get a write what anywhere primitive.

# Question Text

```
With a bestselling book under her belt and over fifty million copies sold, Anna
Sewell has decided to go into the stock market. Please help her figure out her
stock market ticker.

nc play.spgame.site 1350
```

*Creator -  amon (@nn_amon)*

# Setup Guide

0. Install docker on the hosting system
1. Replace the flag in distribute/flag
2. Build the docker image with: `sh dockerbuild.sh`
3. Replace the port 1350 with your desired port in dockerrun.sh
4. Start the docker image: `sh dockerrun.sh`
5. Test the connectivity with netcat.

# Exploit Details

Setting the offset to -8 allows us to write the the symbol member of the
ticker\_tape struct. Now, we can write the address of the src\_file global
variable to the member byte by byte. After we have overwritten the char array
pointer, we can choose to change the symbol to overwrite the src\_file variable
with "/bin/sh". Finally, we trigger the shell by executing the view source
command.

Working exploit in service/exploit.py.

