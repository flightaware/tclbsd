# tclbsd, a package for interfacing to various BSD UNIX system calls and library routines (and works pretty well with Linux, too)

**tclbsd** provides a way for Tcl programs to exercise many interesting and useful system calls and library routines available to C programs on versions of BSD UNIX such as FreeBSD.  Many of the interfaces also work with Linux and other non-BSD UNIX variants as well.

This package is a freely available open source package under the "Berkeley"
license, same as Tcl.  You can do virtually anything you like with it, such as 
modifying it, redistributing it, and selling it either in whole or in part.  
See the file "license.terms" for complete information.

tclbsd was written by Karl Lehenbauer.

## Using the BSD interface

package require BSD

`::bsd::getloadavg` returns the number of processes in the system run queue as a Tcl list, representing averages over the last 1, 5 and 15 minutes.

`::bsd::rlimit` lets you get and set the current soft and hard limits for various resources.  You can specify a 64-bit integer value for any of the limits or "unlimited" to indicate that there should be no limit.  Soft limits result in a signal that can be handled in a variety of ways through TclX's signal handler, while hard limits cause the operating system to terminate the program.

`::bsd::rlimit` allows you to set maximum virtual memory limits, the maximum size of a coredump, the maximum amount of CPU time, maximum data segment size, largest-sized file that can be created, maximum number of bytes that can locked into memory, maximum number of files that can be open, the maximum amount of physical memory a process may occupy, and many others.  It is a Tcl-based interface to the `getrlimit` and `setrlimit` system calls.

`::bsd::rusage` returns resource usage for the current process.  Resources reported include user and system CPU time, page fault and reclaims, swaps, number of filesystem I/Os, number of interprocess communications messages sent and received, number of signals received, context switches, and so forth.  It is a Tcl interface to the BSD `getrusage` system call.

`::bsd::cptime` fills an array with key-value pairs representing system, user, nice, interrupt, idle and total clock counts.  Successive calls with the same array as an argument will calculate the cpu time of the various elements as a percentage over the range of elapsed time since the prior call, returning the percentages as a list of key-value pairs and updating the array with the latest sampled values.  This is a more instantaneous way to just CPU utlization rather than load average, which is a weighted average that changes slowly over time.

`::bsd::uptime` returns the number of CPU seconds that the system has been up, as a floating point number.

`::bsd::setproctitle` sets the process title that appears on the *ps* command.

`::bsd::statfs` and `::bsd::getfsstat` returns a considerable amount of information about mounted filesystems including the underlying filesystem block size, the optimal transfer block size, the total number of data blocks and free blocks, inodes and free inodes, the filesystem type, mount point and name, and a number of attributes such as that it is read only, synchronous, restricts executable, SUID, symlinks, whether or not the time of last access is updated upon an access, whether cluster reads and writes are supported, and more.  They are Tcl-level interfaces to the BSD `statfs` and `getfsstat` system calls.

`::bsd::syslog` provides an interface to the syslog library to provide a way to write messages to the system log.  As with the syslog library routines, priority and facility can be specified and log options can be set.

`::bsd::getkey` reads a single key from the user's terminal and returns it as an integer value.  An optional timeout specifies the number of seconds to wait for a keypress.

Please examine the "doc" directory for complete documentation.

## Bugs

None known at this time.

## Contents

Makefile.in	Makefile template.  The configure script uses this file to
		produce the final Makefile.

README		This file

aclocal.m4	Generated file.  Do not edit.  Autoconf uses this as input
		when generating the final configure script.  See "tcl.m4"
		below.

configure	Generated file.  Do not edit.  This must be regenerated
		anytime configure.in or tclconfig/tcl.m4 changes.

configure.in	Configure script template.  Autoconf uses this file as input
		to produce the final configure script.

generic/bsd.c	Tcl BSD interface source code
generic/tclbsd.c

generic/bsd.h	Header files for the BSD interface.


tclconfig/	This directory contains various template files that build
		the configure script.  They should not need modification.

	install-sh	Program used for copying binaries and script files
			to their install locations.

	tcl.m4		Collection of Tcl autoconf macros.  Included by
			aclocal.m4 to define SC_* macros.

## UNIX Build

Building under most UNIX systems is easy, just run the configure script
and then run make. 

	$ cd tclbsd
	$ ./configure
	$ make
	$ make install


## Windows build

Probably irrelevant but might be buildable under Cygwin and some of the functions might work.

## Installation

The BSD interface package installs like so:

         $exec_prefix
          /       \
        lib       bin
         |         |
   PACKAGEx.y   (dependent .dll files on Windows)
         |
  pkgIndex.tcl (.so|.dll files)

The main .so library file gets installed in the versioned PACKAGE
directory, which is OK on all platforms because it will be directly
referenced with by 'load' in the pkgIndex.tcl file.  Dependent DLL files on
Windows must go in the bin directory (or other directory on the user's
PATH) in order for them to be found.

FlightAware
---
FlightAware has released over a dozen applications  (under the free and liberal BSD license) into the open source community. FlightAware's repositories are available on GitHub for public use, discussion, bug reports, and contribution. Read more at https://flightaware.com/about/code/

