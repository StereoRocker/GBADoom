## PicoDoom

A port of prBoom to the RP2040. The actual Pico has only 2MB flash, I'm developing on a Pimoroni Tiny2040 with 8MB flash until I think of something.

This port is derived from [doomhack's GBADoom](https://github.com/doomhack/GBADoom)

**What's hot?**

- Framerate is mostly stable at around 30fps. Occasionally dips to 25fps.

- Sound is generated on-chip with PWM. Runs at 11025Hz, just like the original audio.

- ~Supports Doom Shareware, Retail, Ultimate and Doom2 IWADS.~

- ~Renderer is largely intact. Z-Depth lighting is gone and there is mip-mapping but it's otherwise complete.~

- ~Monster behaviour is all intact. (I.e sound propagation etc.)~



- ~Sound and music support. Big thanks to BloodShedder for his Chiptune Doom MOD files.~

**What's not?**

- Demo compatibility is broken.

- Only running at 240x160px. I'd like to see this increase to 320x240px. Right now, the increased framebuffer size causes failures 

- ~General optimisation. We're never going to get a perfect 35FPS but I think there is still another 25% left without changing the visual quality/correctness/game behaviour. For reference, the first time I ran a build under the emulator it ran at about 3FPS.~ The RP2040 is significantly faster than a GBA, the limiting factor is by far going to be the display I choose to target.

- Although it is based on prBoom, most of the engine enhancements (dehacked, limit removing etc) have been reverted back to Vanilla. This is either for memory or performance reasons. Sadly, NUTS.wad and Okuplok are right out!

- No multiplayer. 


## Cheats:
~~ **Chainsaw:** L, UP, UP, LEFT, L, SELECT, SELECT, UP  ~~
~~ **God mode:** UP, UP, DOWN, DOWN, LEFT, LEFT, RIGHT, RIGHT  ~~
~~ **Ammo & Keys:** L, LEFT, R, RIGHT, SELECT,UP, SELECT, UP  ~~
~~ **Ammo:** R, R, SELECT,R, SELECT,UP, UP, LEFT  ~~
~~ **No Clipping:** UP, DOWN, LEFT, RIGHT, UP, DOWN, LEFT, RIGHT  ~~
~~ **Invincibility:** A, B, L, R, L, R, SELECT, SELECT  ~~
~~ **Berserk:** B, B, R, UP, A, A, R, B  ~~
~~ **Invisibility:** A, A, SELECT,B, A, SELECT, L, B  ~~
~~ **Auto-map:** L, SELECT,R, B, A, R, L, UP  ~~
~~ **Lite-Amp Goggles:** DOWN,LEFT, R, LEFT, R, L, L, SELECT  ~~
~~ **Exit Level:** LEFT,R, LEFT, L, B, LEFT, RIGHT, A  ~~
~~ **Enemy Rockets (Goldeneye):** A, B, L, R, R, L, B, A  ~~
~~ **Toggle FPS counter:** A, B, L, UP, DOWN, B, LEFT, LEFT  ~~

Controls do now exist, however, the SELECT button isn't implemented. Cheats where this isn't required, probably work.

## Controls:  

I built a "controller" specifically for this. 8 buttons map to the D-Pad, A, B, L and R buttons from the GBA port.

## Building:

```mkdir build
cd build
cmake ..
make
```

Optionally, add `-j<num>` to the `make` command to enable multi-processing. Replacing `<num>` with the number of cores your system has is a good start.

I have also left some useful Visual Studio Code specific files in the repo. If you open Visual Studio Code on the root directory, you should find yourself in a good position to build the main project and also debug with a picoprobe.
