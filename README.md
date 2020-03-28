# Schwarzerblitz 3D Fighting Game Engine

Schwarzerblitz ([link to GameJolt page](https://gamejolt.com/games/schwarzerblitz/242456)) is a 
free (as in beer) 3D fighting game project built by me, Andrea "Jens" Demetrio, in the time spanning from 2016 to 2020.
It was originally intended as a stress relief project, a hobby I curated while pursuing my Ph.D. in physics, 
but with time it evolved into a full fledged game, which achieved a moderate success.
Since now I am moving on to work on other projects, I have decided to publish the source code of the engine 
under a [**GPLv3 license**](https://www.gnu.org/licenses/gpl-3.0.html), with the goal of sharing it with
other developers who might be going down the same path and need a reference or just something to look up.

## Disclaimer

This source code is *ugly*. Like, **ugly**. I started writing it in 2016, with just an academic
C++ knowledge - which is pretty good for small projects or working on physics experiment, but not the
best while trying to write properly structured code. As a result, there are several code smells and habits
you should not pick up. Now that I program for a living, I can *actually* realize how bad were some decisions
I took back in 2016, but I have never had the energies or time to refactor it, especially because it worked well
enough for my purposes. There are some hundreds of warning lying around I never cared fixing, so I apologize in
advance for this. However, this code works and runs a delivered game fine, which can be considered a success.

### Known limitations
- The engine **doesn't have** any online capabilities, as I never managed to find the time/will to add them. Please consider
this aspect before using this engine for your game. 
- Due to non-stellar optimizations, when this engine runs with 3D models with more than 5000 triangles, it might
not reach stable 60fps on older systems. 
- The only 3D model format that the engine can use for skeletal models (i.e. characters) is .x (directX skeletal mesh file) without normal maps. 
- irrlicht is **not** multithread. Implementing multi-threading for resource loading has not been an option.

## Requirements

The engine uses a custom version of the [irrlicht engine](http://irrlicht.sourceforge.net/) (nicknamed Schwarzerlicht)
as a rendering engine and input manager, and SFML 2.3.2 as a backend for audio. 
This project needs [DirectX 9 redistributable](https://www.microsoft.com/en-us/download/details.aspx?id=8109) to run 
and the files included can be used with Visual Studio 2019 (I cannot provide project files for previous versions).
Some redistributable for older versions (e.g. [the 2015 redistributable](https://www.microsoft.com/it-it/download/details.aspx?id=48145) 
might also be needed to run some of the DLLs included.
Concerning the Schwarzerlicht engine, only headers and binaries are included in this repository. Please, contact
me in case you want/need to have access to the source code.
This engine runs only on Windows and uses some Windows-specific libraries and headers. No support for Unix system is scheduled.

## License
Please, refer to the license files in the *license* folder, these contain all the needed details. Long story short: my code is released under a 
[*GPLv3 license*](https://www.gnu.org/licenses/gpl-3.0.html), the irrlicht engine and SFML have their own
licenses untouched, the assets (unless specified in the file *licens/credits.txt*) **cannot** be redistributed
or uploaded in another repository - specifically, the characters, stage and music files - all rights are 
reserved to their respective creator and they have been used with permission.
You are allowed to create commercial derivatives of this engine as long as you comply with the above license terms 
(quick summary: GPLv3 means that if you distribute this or your own version of this engine, even for free, 
you must also convey the entirety of the source code to the end users).
If you need a more permissive license, please contact me directly.