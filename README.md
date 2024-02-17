# Leizur

![maxresdefault](https://github.com/NeoMaster831/Leizur/assets/72684393/62c731ff-d7f6-4409-9a1a-28d53ac625cd)

> *as fast as you can,*

Leizur is open-source osu!lazer cheat, is under development.
It is currently going on private.

# Requirements

As osu anticheat is on usermode, you can realize that if you go kernel, it'll be bypassed.
So I did. To read process' memory, we use Hooked-`NtQueryCompositionSurfaceStatistics`.

I protected my IAT table to 

To use, you'll need to check out [this](https://github.com/NeoMaster831/rwDriver).

Also, it uses DirectX 9 and GDI+, if you can't run you should check those.

# How to use

Go project, build, and use. It requires [Visual Studio 2022](https://visualstudio.microsoft.com/ko/vs/).

## To-Dos
- [ ] Make Parser - *96.1%* (It's almost done, but it has so many bugs)
+ - [ ] Make Catmull Curve Parser (who tf uses catmull nowadays...)
- [ ] Make Utilities - *10%*
- [ ] Make Cheats - 0.1%
+ - [ ] Make Aim Assist - 1%
- [x] Make Bypass - **DONE!**
+ - [ ] Make IAT Hook for protecting SetCursorPos, or something 
- [ ] Make UI - *20%*
- [ ] Make GDI - *20.1%*
