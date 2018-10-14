# VexV5Doom

This is a port of id Software's DOOM for the Vex V5 Robot Brain, using the [PROS](https://github.com/purduesigbots/pros) kernel.

It's based on floppes' [stm32doom](https://github.com/floppes/stm32doom) port which uses [Chocolate Doom](https://github.com/chocolate-doom/chocolate-doom)'s codebase.

## How to use
* Clone the repo:
````
git clone https://github.com/sealj553/VexV5Doom.git
````
* Build and upload to the V5:
````
cd VexV5Doom
prosv5 make
prosv5 upload
````
* Prepare the microSD card

The microSD card needs to be formatted as fat32.
Then copy the `usd` folder (which contains the WAD file) to the microSD card
* Insert the microSD card and start the game

## Features
* Play with the Vex controller
* Save and load games on the microSD card
* Load standard doom WAD files

## Controls
This can be seen and changed in `src/doom/chocdoom/i_video.c` in function `I_GetEvent`

left analog x = turn left/right

left analog y = move forward/back

right analog x = strafe left/right

a = fire

b = use

x = enter

y = escape

dpad = arrow keys

l1 = prev weapon

r1 = next weapon

## Limitations
Due to hardware limitations, the following is not possible on the V5:
* Sound
* Multiplayer
* Probably other things
