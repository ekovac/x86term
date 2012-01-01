x86term
=======
Turning useless PCs into slightly less useless PCs since 2011.

_x86term_ is a terminal emulator that runs without the benefit of an operating system for ease of use and deployment on legacy computers too old to run even the most modest of Linux distros. If you've ever wanted the joys of a glass-screen terminal but can't scrounge up a DEC VT on eBay, this is the software package for you.

It's also in part a proof-of-concept for demonstrating the flexibility of Paul Evans' libvterm library, which is a necessary component of this project. More details in the 

System Requirements
-------------------
A PC-AT(X) computer with at least 300K or so of main memory, a floppy drive or hard disk of approximately the same size, a VGA-compatible video device, a reasonably sane BIOS, and a standard BIOS-connected UART.

How To Build/Use
------------

Binary builds to come once things actually work.

It's highly suggested that you install qemu and/or bochs if you plan to develop for _x86term_.

After cloning a copy of x86term's code, in a sibling directory checkout Paul Evans' libvterm thus:
     bzr checkout  http://bazaar.leonerd.org.uk/c/libvterm
It's critical that you not build libvterm using his Makefile, as options need to be passed to prohibit the inclusion of your OS's library files.

Inside the _x86term_ directory, you should be able to run `make` and get the basic kernel ELF image, which is bootable by any multiboot compliant bootloader. `make qemu` will launch a qemu instance using _stdio_ as the serial port and booting the _x86term_ kernel, and provides a handy tool for testing. `make floppy.img` will create an image file which you can (theoretically) write to a floppy disk and use to boot your soon-to-be glass terminal.

Current Limitations
-------------------

Right now, it crashes on real hardware and in Bochs, but runs almost kinda sorta correctly in qemu. We're working on that, since we're not competing to be the most bloated, overly-complex software terminal emulator.

Credit
------
I wouldn't have known where to start with this project if it weren't for the folks over at [OSDev](http://wiki.osdev.org/) and for James Molloy's [wonderful tutorial.](http://www.jamesmolloy.co.uk/tutorial_html/index.html). I've liberally borrowed code fragments, though only after ensuring I understood it (and writing my own horrible, hackish rough equivalents to make sure I understood it.) Hopefully no one will be offended, I'll try to go through and insert proper citations as this project nears maturity.

I'd also like to thank my friend [Paul "LeoNerd" Evans](http://www.leonerd.org.uk/) for his terminal emulation library, without which this project would've been too tedious to bother with for such a small payoff. 

Legal
-----

This software is released under the terms of the General Public License v3. A copy is included in the source tree as required by the license.

No guarantees are made about the fitness of this software for any purpose, explicit or implied. This software may eat your data, shave your eyebrows, give your pets fleas, and urinate in your gas tank, in any order and combination.
