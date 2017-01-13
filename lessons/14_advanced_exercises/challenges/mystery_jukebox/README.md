Mystery Jukebox
---------------

Mixture of struct member overflow, and format string vulnerability. Leverage the
format string vulnerability to overwrite GOT entries.

# Question Text

```
Listen to some Postmodern Jukebox (https://youtu.be/NTmk0Pqk6hs) while playing
with this Mystery Jukebox program!

nc play.spgame.site 1343
```

*Creator -  amon (@nn_amon)*

# Setup Guide

0. Install docker on the hosting system
1. Replace the flag in distribute/flag
2. Build the docker image with: `sh dockerbuild.sh`
3. Replace the port 1343 with your desired port in dockerrun.sh
4. Start the docker image: `sh dockerrun.sh`
5. Test the connectivity with netcat.

libc should have the following hash d58eb4bfe204b6332b9ab3394ea943ef otherwise
replace the libc in distribute.

# Exploit Details

There is a vulnerability in the line:

`memcpy(musics_profile.brand, "Follow Rabbit", 12);`

The struct member length for the brand is 12 so this means that there is no null
terminator for that member. It looks like it is correct because the member for
volume intentionally contains null bytes.

There is also an integer underflow on the volume. You can decrement volume
until it underflows and hits -1. When you do that, all the null bytes in the
volume member disappears.

Changing the brand uses strlen to check how much to fread so now, there is a
possibility of overflowing the brand member into the brand message member.

The brand message member is used in a format string vulnerability here:

`printf(musics_obj->brand_message);`

Use the format string vulnerability to leak address of printf and calculate libc
base. Then replace puts in GOT with system. On the next call of puts:

`puts("Ohhhhhhh you got it alllll the way down;ed");`

This will spawn an `ed` calculator instance which lets you get shell by doing
the following statement:

`!sh`

Working exploit is in service/exploit.py
