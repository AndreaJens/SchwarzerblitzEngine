# Schwarzerblitz - 3D Fighting Game Engine

Schwarzerblitz ([link to GameJolt page](https://gamejolt.com/games/schwarzerblitz/242456)) is a 
free (as in beer) 3D fighting game project built by me, Andrea "Jens" Demetrio, in the time spanning from 2016 to 2020.
It was originally intended as a stress relief project, a hobby I curated while pursuing my Ph.D. in physics, 
but with time it evolved into a fully fledged game, which achieved a certain degree of success and a couple dedicated tourneys.
Since now I am moving on to work on other projects, I have decided to publish the source code of the engine 
under a **[BSD 3 clauses license](https://fossa.com/blog/open-source-software-licenses-101-bsd-3-clause-license/)**, with the goal of sharing it with
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
- A guide to how to export models for the engine can be found [here](https://steamcommunity.com/sharedfiles/filedetails/?id=2147297081). Alternatively, YouTube video guides to export characters for the engine are available [here](https://youtu.be/74qcxNz4rtg) and [here](https://youtu.be/Uj1hG3JtTPw) respectively.
A YouTube video guide on how to import models from the .x format is available [here](https://youtu.be/R9b0D9O4sm8).
- irrlicht is **not** multithread. Implementing multi-threading for resource loading has not been an option.

## Notes

The engine uses a custom version of the [irrlicht engine](http://irrlicht.sourceforge.net/) (nicknamed Schwarzerlicht)
as a rendering engine and input manager, and SFML 2.3.2 as a backend for audio. 
This project can be run and compiled using Visual Studio 2017 or 2019 (I cannot provide project files for previous versions).
Some redistributable for older versions might also be needed to run some of the DLLs included.
Concerning the Schwarzerlicht engine, only headers and binaries are included in this repository. See [this repository](https://github.com/AndreaJens/irrlicht4schwarzerblitz) if you want to compile it for yourself and/or edit it.

This engine runs only on Windows and uses some Windows-specific libraries and headers. 
The engine has been developed on Windows 10, but it could probably be adapted for development on Windows 8.1 or previous. 
I cannot offer support for this port, as I don't have access to the relevant SDK. Please, notice that no support for Unix system is provided or planned.

## Installation
- Download and install the [DirectX 9 redistributable](https://www.microsoft.com/en-us/download/details.aspx?id=8109);
- Download and install the [Visual C++ 2015 redistributable](https://www.microsoft.com/it-it/download/details.aspx?id=48145);
- Clone or download this repository;
- Open the solution file with Visual Studio (schwarzerblitz_engine/Schwarzerblitz Engine.sln);
- Switch the configuration to Release / Win32 (or Debug, in case of need to debug some features);
- Compile and run the engine with the sample resources;
- The executable is placed inside the schwarzerblitz_engine/bin directory;
- While running in Debug mode, an additional terminal window displays some diagnostic messages;
- If you want to compile your own version of the irrlicht DLL, [the full source code is available here](https://github.com/AndreaJens/irrlicht4schwarzerblitz). You'll need to replace the .lib and .dll files in the relevant folders.

## License
Please, refer to the license files in the *license* folder, these contain all the needed details. Long story short: my code is released under a 
[*BSD 3 clauses license*](https://fossa.com/blog/open-source-software-licenses-101-bsd-3-clause-license/), the irrlicht engine and SFML have their own
licenses untouched, the assets (unless specified in the file *licens/credits.txt*) **cannot** be redistributed
or uploaded to another repository - specifically, the characters, stage and music files - all rights are 
reserved to their respective creator and they have been used with permission.
You are allowed to create commercial derivatives of this engine as long as you comply with the above license terms 
If you need a more permissive license, please contact me directly.

### About the precvious licensing
Originally, this project was released under a GPL3 license, but I have failed to take into account the fact that irrlicht links to the DirectX9 SDK (arguably not open source). Thus, to allow people to link to any version of irrlicht and to legally distribute this code, I have decided to re-license my full project as a permisive BSD 3-clauses license instead. **This license still DOESN'T cover the graphic, sound and 3D assets, those are "all rights reserved" and cannot be re-released with your version of the engine**.

## Contacts and links
You can contact me through [Twitter](https://twitter.com/AndreaDProjects) or the [Schwarzerblitz Discord Server](https://discord.gg/vXKrvpV).
