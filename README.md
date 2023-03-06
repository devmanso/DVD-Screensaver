# DVD-Screensaver
A simple DVD Screensaver app made in SDL2, with Lua scripting capabilites!

## Credits:
Credit to Thomas Lively (@tlively on github), for sdl_seminar/hello4_animation.c
I have modified some of his code, but it is mostly the same, I have added bindings
for this game to the Lua scripting language.

## Prerequisites:
- you must have the Lua interpreter installed on your system, I'm using 5.4.1, but any version of Lua 5.1 and above should be just fine.
- you must have SDL2 and SDL2_image installed on your system, you should use your Linux distro's package manager to do this.
- or if your on MacOS X, install brew and then install SDl2 and SDl2_image.
- A C99 complaint compiler.

## How do I build this?
To build a binary and run this, open your teminal and cd or navigate to wherever you have this on your system.

Next, enter 'make game' - this will produce the binary, given that there are no errors in your code.

Finally, to run the binary, enter './play'

This project is liscensed under the MIT Liscense (see LICENSE.md)

SDL2 is liscensed under the zlib Liscense
https://www.libsdl.org/license.php

IDK what Lua is liscensed under, but here is the official lua website I guess.
https://www.lua.org/