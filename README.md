# Leizur

![maxresdefault](https://github.com/NeoMaster831/Leizur/assets/72684393/62c731ff-d7f6-4409-9a1a-28d53ac625cd)

> *as fast as you can,*

Leizur is open-source osu!lazer cheat, is under development.
It is currently going on private.

# Requirements

As osu anticheat is on usermode, you can realize that if you go kernel, it'll be bypassed.
So I did. To read process' memory, we use Hooked-`NtQueryCompositionSurfaceStatistics`.

In fact, they do not have anticheat in lazer yet, so actually we don't have to bypass.
I think lazer pp is kinda too early.. as they should block slient cheaters...

To use, you'll need to check out [this](https://github.com/NeoMaster831/rwDriver).

Also, it uses DirectX 9 and GDI+, if you can't run you should check those.

# How to use

Go project, build, and use. It requires [Visual Studio 2022](https://visualstudio.microsoft.com/ko/vs/).

## To-Dos
- [ ] Make Parser - *90%* (It's almost done, but it has so many bugs)
- [ ] Make Utilities - *10%*
- [ ] Make Cheats
- [x] Make Bypass - **DONE!**
- [ ] Make UI - *20%*
- [ ] Make GDI - *20%*
