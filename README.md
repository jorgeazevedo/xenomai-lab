Xenomai Lab
===========

A Platform for Digital Real-Time Control.

Installation
------------

A fairly standard procedure.

### Xenomai

Xenomai Lab needs Xenomai installed on the target system. Xenomai 2.5.5.2 and 
2.5.6 are known to be supported. 

In addition to this, Xenomai Lab needs non-root real-time access. This means 
that the user running Xenomai Lab should **not** need to use sudo to run 
xenomai real-time applications. The default Xenomai installation does not 
enable this, so you might need to configure your system accordingly.

Doing this is rather straithforward. It is merely a case of adding the user
to the xenomai group

`sudo usermod -a -G xenomai USERNAME`

and passing the xenomai group id as a boot parameter to the xenomai nucleus.
The group id can be found in /etc/groups. For example, running

`cat /etc/group | grep xenomai`

would return

`xenomai:x:123:USERNAME`

which means the xenomai group id is 123, and has for members only USERNAME.

In GRUB2 systems, passing the group id is a matter of editing /etc/default/grub,
for example

`sudo gedit /etc/default/grub`

And edit the line

`GRUB_CMDLINE_LINUX_DEFAULT="quiet splash"`

so that it has the argument at the end

`GRUB_CMDLINE_LINUX_DEFAULT="quiet splash xeno_nucleus.xenomai_gid=125"`

Finally, updating grub will load the new configuration

`sudo update-grub`

### Dependencies

`sudo apt-get install libboost-graph-dev qt4-dev-tools build-essential`

### Compile Xenomai Lab:

`qmake`

`make`

### Install

`sudo make install`

### Uninstall

`sudo make uninstall`
