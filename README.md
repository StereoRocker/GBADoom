## PicoDoom

A port of prBoom to the RP2040. The actual Pico has only 2MB flash, I'm developing on a Pimoroni Tiny2040 with 8MB flash until I think of something.

This port is derived from [doomhack's GBADoom](https://github.com/doomhack/GBADoom)

**What's hot?**

- ~Supports Doom Shareware, Retail, Ultimate and Doom2 IWADS.~

- ~Renderer is largely intact. Z-Depth lighting is gone and there is mip-mapping but it's otherwise complete.~

- ~Monster behaviour is all intact. (I.e sound propagation etc.)~

- ~Framerate is pretty variable. Simple areas run at ~35fps. Complex areas (Eg: E4M2) chug along at about 10 FPS. It's running around the same as the original GBA Doom1 and Doom2 ports. Doom1 Episodes 1-3 are all completely playable. Episode 4 chugs.~

- ~Sound and music support. Big thanks to BloodShedder for his Chiptune Doom MOD files.~

**What's not?**

- The port does not yet run.

- Demo compatibility is broken.

- ~General optimisation. We're never going to get a perfect 35FPS but I think there is still another 25% left without changing the visual quality/correctness/game behaviour. For reference, the first time I ran a build under the emulator it ran at about 3FPS.~ The RP2040 is significantly faster than a GBA, the limiting factor is by far going to be the display I choose to target.

- Although it is based on prBoom, most of the engine enhancements (dehacked, limit removing etc) have been reverted back to Vanilla. This is either for memory or performance reasons. Sadly, NUTS.wad and Okuplok are right out!

- No multiplayer. 


## Cheats:
~**Chainsaw:** L, UP, UP, LEFT, L, SELECT, SELECT, UP  ~
~**God mode:** UP, UP, DOWN, DOWN, LEFT, LEFT, RIGHT, RIGHT  ~
~**Ammo & Keys:** L, LEFT, R, RIGHT, SELECT,UP, SELECT, UP  ~
~**Ammo:** R, R, SELECT,R, SELECT,UP, UP, LEFT  ~
~**No Clipping:** UP, DOWN, LEFT, RIGHT, UP, DOWN, LEFT, RIGHT  ~
~**Invincibility:** A, B, L, R, L, R, SELECT, SELECT  ~
~**Berserk:** B, B, R, UP, A, A, R, B  ~
~**Invisibility:** A, A, SELECT,B, A, SELECT, L, B  ~
~**Auto-map:** L, SELECT,R, B, A, R, L, UP  ~
~**Lite-Amp Goggles:** DOWN,LEFT, R, LEFT, R, L, L, SELECT  ~
~**Exit Level:** LEFT,R, LEFT, L, B, LEFT, RIGHT, A  ~
~**Enemy Rockets (Goldeneye):** A, B, L, R, R, L, B, A  ~
~**Toggle FPS counter:** A, B, L, UP, DOWN, B, LEFT, LEFT  ~

To be updated when controls exist.

## Controls:  

I haven't figured this out yet.

## Building:

TBC. Right now I'm using Visual Studio Code, the built-in debugging support via a Picoprobe is pretty awesome.

Theoretically I should have a CMake file here in the root. For now, open VSCode on the source directory. Cleaning the build system will be completed later.