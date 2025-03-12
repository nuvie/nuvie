# Nuvie - An Engine for Ultima6, Martian Dreams and Savage Empire.

Nuvie has moved to the ScummVM project
--------------------------------------
The Nuvie project has a new home as an engine inside the [ScummVM](https://www.scummvm.org/) application. [Github Repo](https://github.com/scummvm/scummvm) 
Further developments will be done there.

ScummVM offers a range of benefits to Nuvie. More supported platforms, MT-32 audio support. Extensive shader support etc.

Please raise any issues with the ScummVM [bug tracker](https://bugs.scummvm.org/) 

---

Windows: [![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/nuvie/nuvie?branch=master&svg=true)](https://ci.appveyor.com/project/yuv422/nuvie)

http://nuvie.sourceforge.net/
Version 0.5

![Nuvie](docs/images/nuvie.png)


What is Nuvie
-------------

Nuvie (Pronounced *New-Vee*) is an open source engine for Origin's games
Ultima 6, Martian Dreams and The Savage Empire. It uses the original data files
from the games. Nuvie currently runs on Linux, Mac OS X and Windows. It should
be able to run on any platform that is supported by the SDL library.

Ultima 6 is completable and we are currently adding support for Martian Dreams
and Savage Empire.

Compiling Nuvie
---------------

Nuvie requires LibSDL to compile. http://www.libsdl.org/

Unix (various)

Compiling should (hopefully) be as simple as

```
./configure
make
```
Win32

Nuvie can be compiled for win32 using MinGW


Using Nuvie
-----------

Config
------

Nuvie can be configured by editing the configuration file.

The config file will be automatically created the first time you launch
Nuvie on Windows and Mac OS X environments.

On Windows Vista/7/8 the config file (nuvie.cfg) is located at
`\Users\username\AppData\Roaming\Nuvie\`

On Windows 2000/XP the config file (nuvie.cfg) is located at
`\Documents and Settings\username\Application Data\Nuvie\`

On Windows NT4 the config file (nuvie.cfg) is located at
`<windir>\Profiles\username\Application Data\Nuvie\`

On Windows 95/98/ME the config file (nuvie.cfg) lives alongside
the nuvie.exe file.

On Mac OS X the configuration file is called "Nuvie Preferences" and lives in
the following directory.
`~/Library/Preferences/Nuvie Preferences`

Linux users will need to manually create a config file in either `./nuvie.cfg`
or `~/.nuvierc`

A sample config file has been included nuvie.cfg.sample

Keyboard commands
-----------------

All the keyboard commands from the original game should be available in Nuvie.

Additional keys.

Quit. `q`

Loading/Saving games. `s`.

Mouse
-----

It is possible to drag and drop items from your inventory.

FM-Towns U6
-----------

Nuvie supports the english language speech and sfx from the FM-Towns version
of Ultima VI. You can enable FM-Towns support with the following options in the
config file.

Set `config/ultima6/townsdir` to the location of your FM-Towns U6 data files.
Set `config/ultima6/sfx` to `towns`.

Scalers
-------

Nuvie currently supports the following scalers.

Note! These scaler names are case-sensitive in the config file.

* Point
* Interlaced
* 2xSaI
* Super2xSaI
* Scale2x
* SuperEagle
* BiLinear
* BiLinearPlus
* BilinearInterlaced
* BilinearHalfInterlaced
* BilinearQuarterInterlaced

Helping
-------

We are always looking for people to help out with the project. If you feel you
can help out in any way please contact us.

Contact
-------

Email: `efry <AT> users.sourceforge.net`
IRC: #nuvie on freenode.net
Message board: http://nuvie.sourceforge.net/phorum

Credits and Thanks
------------------

See the "AUTHORS" file for info on the Nuvie team.

Nuvie would not have been possible without help from the following people.

J. P. Morris aka DOUG the Eagle <jpm@it-he.org>
http://www.it-he.org/ultima6.htm

Rackne <rackne@red5.graf.torun.pl>
http://www.graf.torun.pl/~rackne/u6like.html

Nodling <nodling@yahoo.com>
http://www.geocities.com/nodling/ultima/ultima.html

Jim Ursetto <jim@3e8.org>
http://3e8.org/hacks/ultima6/

The exult team.
http://exult.sourceforge.net/

Pentagram.
http://pentagram.sourceforge.net/

ScummVM.
http://www.scummvm.org/

Everyone on freenode #nuvie

And of course Lord British along with all the talented people at Origin.

