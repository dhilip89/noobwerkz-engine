I am developing a solid, small-API, easy-to-use cross-platform game engine. This toolkit is made specifically to reduce wasted time (*cough cough* aside from the time it took to design and build this software *cough cough*.) Much of it consists of integrating external libraries, most of which are in C++11. It currently runs on Linux and Android. We can expect Windows, Mac, and iOS support quite soon. As a design goal, the legalese is designed to be extremely liberal (Apache 2.0.)

Still WIP. Not yet recommended for use, as this project is currently having major features implemented and it's being done in what I feel is the right way (TM.) Please feel free to bookmark this repo and come by later because this thing is living with me and cool stuff will soon come out of it. :) Progress comes at a rather steady pace; I plan on using this to back many of my own efforts and code quality reflects this fact. Writing my own code makes me happy so part of my life revolves around being able to do this.

Documentation is rather sparse at the moment, and consists of code comments, this page, and issues on the tracker (of which there are many.) The issues are well-modularized and specific, thoughtfully labeled, and properly organized into milestones. High-quality tutorials and real unit tests will come when things are more or less set in stone.

Current version: 0.0.1.1, AKA: Fresh "Beginnings"

Specific features:
```
A highly tunable gradient mapping shader by default. Comes with three XY-tileable textures blended into one.
Triplanar shader for non-UV mapped objects. Texture reads are kept to a minimum (3) by using the gradient mapping technique.
Provides clean interfaces over to those libraries and tools on top, with varying degrees of safety. These choices are left to the programmer.
```

Currently brings in (among others):
```
Bgfx
glfw
Bullet Physics
Freetype
polyvox
Lemon Graph Library 
Assimp (and a working mesh loader, too! =P)
cppformat
Cereal
AngelScript
```

To clone:
```
git clone --recursive https://github.com/ColinGilbert/noobwerkz-engine.git
```

Structure:
```
The current setup uses a platform-specific starter program to launch the main application.

There are many libraries - mostly math and scientific libraries - in the engine/lib directory. Not all of them are being used. This needs a cleanup, but at the moment this is still my person dev repo. Will modularize out soon.

Most engine code written in engine/common directory.

Short scripts with tiny names are used to do routine work (compiles, archivals, cleanups.) Will eventually bring it into C++ and AngelScript.
```

Platforms:
```
Current setup uses linux as main controller.
Ports soon.
```

Little *nix scripts (currently runs on bash but I don't think uses any bashisms):
```
sandbox/b for build. Attempts to build source.
sandbox/bb for big build. Wipes the binaries and builds from scratch.
sandbox/s for shaders. Builds the full set of shaders from sandbox/shader_src and places them into the sandbox/shaders directory
sandbox/g for git. Adds changes to git and commits them.
```

Things to do in the immediate future:
```
Continue refining the simulation world.
AI
Sound
Serialization
```

Medium future:
```
Ports to Win, MacOS, iOS, Emscripten, NaCL
Test framework
Destructible worlds
```

Long-term:
```
Middleware between disparate OS'es: Overall aim is awesome devops.
I intend on splitting this project into two parts: A middleware platform and a 3D/console engine.
The focus is for people wanting to set up a proper lab, with a specific minimum hardware investment being the established norm. Will include support for interfacing with *nix, Android, Windows, Apple, and Steam ecosystems (as well as others, possibly.)
Also, as I obtain a whole bunch of networking certs and learn secure coding, the code here will also become a facet of those learning experiences.
Shall come with blueprints for suggested network layouts.

Automated testing/simulation
Controlling state across networks
Data Analysis/DataViz
Other business needs
```
