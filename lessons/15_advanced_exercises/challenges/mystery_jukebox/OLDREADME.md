Diapers Simulator
-----------------

All server side files are in 'service'.
All files to be distributed to the players are in 'distribute'. Please add the
server address and port in distribute/description.

# Exploit Details

There is a vulnerability in the line:

`memcpy(diaper_profile.brand, "Bunny Rabbit", 12);`

The struct member length for the brand is 12 so this means that there is no null
terminator for that member. It looks like it is correct because the member for
wetness intentionally contains null bytes.

There is also an integer underflow on the wetness. You can decrement wetness
until it underflows and hits -1. When you do that, all the null bytes in the
wetness member disappears.

Changing the brand uses strlen to check how much to fread so now, there is a
possibility of overflowing the brand member into the brand message member.

The brand message member is used in a format string vulnerability here:

`printf(diaper_obj->brand_message);`

Use the format string vulnerability to leak address of printf and calculate libc
base. Then replace puts in GOT with system. On the next call of puts:

`puts("Shhhhh goodnight sleepy baby... go to b;ed");`

This will spawn an `ed` calculator instance which lets you get shell by doing
the following statement:

`!sh`

Working exploit is in service/exploit.py

# Deployment Instructions

0. Install docker on the hosting system
1. Replace the flag in distribute/flag
2. Build the docker image with: `sh dockerbuild.sh`
3. Replace the port 1343 with your desired port in dockerrun.sh
4. Start the docker image: `sh dockerrun.sh`
5. Test the connectivity with netcat.

libc should have the following hash d58eb4bfe204b6332b9ab3394ea943ef otherwise
replace the libc in distribute.

Cheers,
- amon.
