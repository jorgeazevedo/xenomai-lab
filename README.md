
Xenomai Lab
===========

A Platform for Digital Real-Time Control.

Installation
------------

A fairly standard procedure.

### Xenomai

Xenomai Lab needs Xenomai installed on the target system. Xenomai 2.5.5.2 and 
2.5.6 are known to be supported. To install Xenomai please refer to the [wiki](http://www.xenomai.org/index.php/Building_Debian_packages).

In addition to this, Xenomai Lab needs non-root real-time access. Instructions
for this are available [here](http://www.xenomai.org/index.php/Non-root_RT), but
an abridged tutorial for Ubuntu or other Debian based systems is available
in the Appendix section.

### Dependencies

    $ sudo apt-get install libboost-graph-dev qt4-dev-tools build-essential

### Compile Xenomai Lab:

    $ qmake

    $ make

### Install

    $ sudo make install

### Uninstall

    $ sudo make uninstall

Working with Blocks
-------------------

### Create a new block

Xenomai Lab comes bundled with several useful blocks, but adding new ones is
easy. All blocks are stored in `~/.xenomailab/blocks`. To add a new one, simply
run the `newblock.sh` script in that folder and add your new block to 
`blocks.conf`.

Here's an example

    $ cd ~/.xenomailab/blocks/
    $ ./newblock.sh example_block
    $ gedit blocks.conf

Then add your block to list either under Real-Time, if it's a xenomai task,
 or Non Real-Time, if it's a standard linux program.

Continuing the example

    [Real-Time]
    ...
    example_bock=square

All done!

### Add some block settings

One of the most powerfull features of Xenomai Lab's blocks is the ability to
change some operational settings in runtime. The procedure to edit or add new
settings to your own block is not complex per say, but it is somewhat
extensive.

There are **four types** of settings:

* Strings (_char[]_)
* Integer (_int_)
* Doubles (_double_)
* Matrices (_Matrix_)

A block setting lives in **4 distinct places** inside the block folder.

Lets illustrate these four places using our `example_block` from the previous
section.

#### example\_block.conf

The `.conf` carries the **default value** of all settings. When you drag a new
block to the canvas in Xenomai Lab, its settings will be initialized with the
values specified in this file. Here's an example `.conf` exemplifying the four
type of settings

    [Operation]
    aString=PWM
    anInt=40
    aDouble=40.0
    aMatrix=[1 0 0; 0 1 0; 0 0 1]
    
    [Task]
    Priority=99

Notice that these variables must be a single word, with **no** spaces.

#### example\_block\_settings.h

This file contains both the declaration of the `struct` which holds the
settings, and the declaration of the `gs` variable used to access these settings
from your code.

    struct global_settings{

        char aString[40];
        int anInt;
        double aDouble;
        Matrix aMatrix;

        int task_prio;//Real time task priority (0-99, higher is greater)
    };
    extern struct global_settings* gs;

Accessing these variables is as straightforward as it seems. To
print the `aString` variable, the code would be

    printf("aString:%s\n",gs->aString);

#### example\_block\_settings.c

This file contains some unavoidable boilerplate code. Functions `load_gs` and
`unload_gs` load and store the settings from the `.conf`, respectively.
In effect, these are the functions that map the text written in the `.conf`
with the variables declared in the `struct`. The variable name in the `.conf`
**does not** need to be the same as the variable name in the struct, but it
is a good practice to use the same name.

Continuing our example

    void load_gs(void){
        get_string("Operation","aString",gs->aString);
        get_double("Operation","anInt",&gs->anInt);
        get_double("Operation","aDouble",&gs->aDouble);
        get_matrix("Operation","aMatrix",&gs->aMatrix);

        get_int("Task","Priority",&gs->task_prio);
    }
    void unload_gs(void){
        store_string("Operation", "aString", gs->aString);
        store_double("Operation", "anInt", gs->anInt);
        store_double("Operation", "aDouble", gs->aDouble);
        store_matrix("Operation","aMatrix",&gs->aMatrix);

        store_int("Task","Priority",gs->task_prio);
    }

#### example\_block\_settings\_proj/example\_block\_settings\_proj/mainwindow.cpp

Finally, we need a GUI to edit them. The `setSettings` member of the `MainWindow`
class contains all the different settings.

The only detail that may **not** work as expected is the string. A 
string setting is presented in the GUI as a combo box, **not** as a text entry.
You don't write a string with the keyboard, you **select** it from a list. 

The `signal_generator` block uses this setting to allow a user to choose
between different types of waves. A block that outputs to a serial port might
use a string to allow the user to choose between `/dev/ttyUSB0` or `/dev/ttyUSB1`.

Continuing with our example

    void MainWindow::setSettings()
    {
    newComboBox("aString:",QStringList("Str1") << "Str2" << "Str3",gs->wave);
    newEntry("anInt:",&gs->anInt);
    newEntry("aDouble:",&gs->aDouble);
    newEntry("aMatrix:",&gs->aMatrix);
    }


### Using libraries

Using a library involves creating two files and editing a makefile. If the 
block you're programming is very simple, then using a library is overkill.
As a general rule of thumb though, decoupling the block code from its 
functionality is considered a good practice because

 * If the block API changes, your interface may need to change, but your functionality code remains unaltered.
 * The block code follows a strict standard and messing with it can generate unexpected bugs. Decoupling assures that the block is almost _always_ innocent and bugs can be localized to your own library.
 * A group of blocks can and sometimes _should_ use the same library. This avoids duplication of common code and centralization can greatly increase legibility in some cases, e.g. hardware I/O blocks, control alghorithms,etc.

Here's an example. If you intend to program a block that adds a DC offset
to incoming signals, creating a library is overkill. If you intend to program
high-pass, low-pass and band-pass block filters, then you should create
a library with 3 functions and have each individual block call one of them.

To make your custom block use a .h/.c couple is a matter of altering its
makefile. The recommended way is to leave your .h/.c library in
`~/.xenomailab/include` and add it to the `DEP` variable in the Makefile.

Using the `example_block` introduced in the last section, we should edit
`~/.xenomailab/blocks/example\_block/Makefile` to use an `example_library`

    DEP=template.c template_settings.c $(INCLUDE)rt_block_io.c $(INCLUDE)settings.c $(INCLUDE)strmap.c $(INCLUDE)mtrx.c $(INCLUDE)example_library.c

### Using the Oscilloscope

As of yet, the oscilloscope is not deployment ready. However, due to its
usefullness I have packaged it in `data/oscilloscope.tar.gz`.

Installing the oscilloscope is a two-step process.

#### Install Qwt

The Oscilloscope requires [Qwt 6.0](http://qwt.sourceforge.net) or greater
installed on the system.

To install Qwt, download it from the official site and extract the tarball
to a folder. In that folder, run

    $ qmake
    $ make
    $ sudo make install

Afterwards, add the library to ldconfig. To do this, find out Qwt's
installation path

    cd /usr/local/qwt*/lib/
    pwd

This returns something like `/usr/local/qwt-6.0.1/lib`. Add this string to

    sudo vim /etc/ld.so.conf.d/qwt.conf

Afterwards, call ldconfig to refresh all available libraries

    sudo ldconfig

To confirm this worked

    ldconfig -p | grep qwt

Which should return something akin to 

    libqwtmathml.so.6 (libc6) => /usr/local/qwt-6.0.1/lib/libqwtmathml.so.6
    libqwtmathml.so (libc6) => /usr/local/qwt-6.0.1/lib/libqwtmathml.so
    libqwt.so.6 (libc6) => /usr/local/qwt-6.0.1/lib/libqwt.so.6
    libqwt.so (libc6) => /usr/local/qwt-6.0.1/lib/libqwt.so


#### Install the block

Extract `data/oscilloscope.tar.gz` to `~/.xenomailab/blocks/` and add the
oscilloscope to the **non real-time** list of blocks

    sudo gedit ~/.xenomailab/blocks.conf

So that

    [Non Real-Time]
    ...
    oscilloscope=square

Appendix
--------

### Enable non-root user access to real-time in Ubuntu

Enabling non-root real-time access in Ubuntu (or other similiar Debian based
distros) consists in merely two steps. One must add the intended user to
the xenomai group, and then pass the xenomai group id to xenomai nucleus during
boot.

Adding a user to the xenomai group can be doing via usermod

    $ sudo usermod -a -G xenomai USERNAME

To confirm, we can check /etc/groups with

    $ cat /etc/group | grep xenomai

which would yield

    xenomai:x:123:USERNAME

This means the xenomai group id is **123**, and has for members only USERNAME.

To pass this argument to the nucleus is a question of adding the parameter to
grub. For example

    $ sudo gedit /etc/default/grub

And edit the line

    GRUB_CMDLINE_LINUX_DEFAULT="quiet splash"

so that it has the argument at the end

    GRUB_CMDLINE_LINUX_DEFAULT="quiet splash xeno_nucleus.xenomai_gid=125"

Finally, updating grub will load the new configuration

    $ sudo update-grub

Copyright and license
---------------------

Copyright (C) 2012 Jorge Azevedo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
