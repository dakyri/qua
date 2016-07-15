#QUA

Multi channel recorder, sequencer, algorithmic sound engine and VST host.

Original version in pure C was written for an Atari ST and later Falcon around 1994, based on some code that I'd used for a static installation. This version ran from the command line without any gui, but processed more or less the internal scripting language as it stands now.

On the BeBox, around 1997, the command line version was extended with a gui, some automatic interface generation based on those scripts, facilities somewhat like a standard daw timeline (a la cubase). This version was in C++ but a very original dialect, and still held a lot of pure C code. It ended up on an x86 beos box in 2001 and saw a lot of use for my own music. It handled joystick input midi, serial ports, multichannel io and midi in one grand mashup. Also featured inter app messaging within its paradigm, a convenient feature of BeOS, and I used this to drive my own, and other 3rd party synths. Still kept a very tiny footprint resource wise.

From 2003 I ported this code to windows, separating the code from its interface, building an  so the core of Qua was back to being a gui agnostic, and added VST and ASIO support, but could run from a command line: this version eventually had working MFC gui.

From 2015, I felt there was enough interesting ideas here to port to android to use with some of the code I'd been working with there. At this point, I stripped all the system dependencies from the core and moved to pure c++ stl facility for synchronization and threading as much as possible. The BeOS dependencies have been stripped ... if it ever ends up on any modern BeOS variant  it will need backporting. Currently, revising the code to bring it into a modern dialect of c++ closer to c++11, but there is still a lot of unavoidable reference to the C heritage.
