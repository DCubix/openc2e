# Openc2e

Openc2e is an open-source game engine intended to
run **all** the games in the _Creatures_ series.

Original development ceased back in 2010.

# Games

Intended games:

* Creatures (1996)
* Creatures 2 (1998)
* Creatures 3 (1999)
* Docking Station (2001)
* Creatures Playground (1999)
* Creatures Adventures (2000)

# Status

## Working

* Agents (COBs) work
* Imperfect physics
* Creatures biochemestry
* Sound effects

## Todo

* Actual creatures (C3+ creatures work somewhat, C1 and C2 ones don't work at all)
* Physics improvements (Especially C3+)
* Network ability (the Docking Station warp)
* Serialization (world saving, creature exporting, ...)
* Music

There are lots of [open issues on the old Google Code issue tracker](https://code.google.com/archive/p/openc2e/issues) that still need solving.

# Compiling

Even though the codebase has been stagnant for over 6 years, it still compiles well (with a few patches).

On Ubuntu 16.04 you'll need these packages:

```bash
sudo apt-get install build-essential qt4-qmake libqt4-dev
sudo apt-get install libsdl-gfx1.2-dev libsdl-net1.2-dev libsdl-ttf2.0-dev
sudo apt-get install libyaml-perl re2c libalut-dev
sudo apt-get install libsdl-mixer1.2-dev libboost1.58-all-dev
```

On Ubuntu 18.10 you'll need a different libboost package:

```bash
sudo apt-get install libsdl-mixer1.2-dev libboost1.63-all-dev
```

Then, you can create a new build directory and compile:

```bash
mkdir build
cd build
cmake ..
make
```

At the end, the compilation might fail when handling documentation,
but it'll still have created the *openc2e* executable.

Now you only need to do this:

```bash
./openc2e -d /path/to/Creatures2/
```

