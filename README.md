# The Basekernel Operating System Kernel

Basekernel is a simple operating system kernel for research, teaching, and fun.

Basekernel is *not* a complete operating system, but it is a starting
point for those who wish to study and develop new operating system code.
If you want to build something *different* than Windows, Linux, or Multics,
Basekernel may be a good place to try out your new ideas.

Basekernel can boot an Intel PC-compatible machine (or VM) in 32-bit protected
mode, with support for VESA framebuffer graphics, ATA hard disks, ATAPI optical
devices, basic system calls, and paged virtual memory.  From there, it's
your job to write filesystem code, a windowing system, or whatever you like.

Our goal is to keep Basekernel simple and easy to understand,
so that others can build on it and take it in different directions.
We encourage you to fork the code, give it a new name,
and develop something completely new on top of it.

To be clear, this is raw low-level code, and there is no guarantee that
it will work on your particular machine, or work at all. If you fix bugs
or solve build problems, we would be happy to accept your contributions.

## How to Build Basekernel on Linux

To avoid potential system-dependent behavior changes, you will
want to use a cross-compiler. A cross-compiler is a compiler intended to
run on one system, but compile code for a different one. In this case,
your computer is the former system ("host"), and basekernel is the
latter system ("target").

The following script will build a suitable cross-compiler
for Basekernel in the `cross` subdirectory:

```
./build-cross-compiler.sh
```

This may take a long time to run, so go get a cup of coffee.
You only need to do this once before building basekernel.

Once the build is complete, add the cross compiler to your PATH:

```
export PATH="$PWD/cross/bin:$PATH"
```

And then build Basekernel itself:

```
cd src
make
```

## How to Build Basekernel on Mac OSX

Although less tested, it is possible to build basekernel on OSX.
First install [MacPorts](https://guide.macports.org/#installing.macports)
and then use it to install a cross compiler:

```
port install i686-elf-gcc
port install i686-elf-binutils
```

And also the `mkisofs` tool, which substitutes for `genisoimage`:

```
port install cdrtools
export ISOGEN=mkisofs
```

Then build Basekernel as usual:

```
cd src
make
```

## Running Basekernel in a Virtual Machine

Once the build is complete, you should see a file `basekernel.iso`
which is a bootable CD-ROM image containing the boot program,
Basekernel itself, and a small number of sample programs.
You can browse the contents of this CD-ROM using any standard utility.

To execute the operating system, use a virtual machine technology
like VMWare, VirtualBox, or QEMU, and direct it to use that image.

For example, if you are using QEMU, this command-line should do it:

```
qemu-system-i386 -cdrom basekernel.iso
```

You should see something like this:

<img src=screenshot.png align=center>

After some initial boot messages, you will see the kernel shell prompt.
This allows you to take some simple actions before running the first
user level program.  For example, read the boot messages to see
which ata unit the cdrom is mounted on.  Then, use the <tt>mount</tt> command
to mount the cdrom filesystem on that unit:

<pre>
mount 2
</pre>

Use the <tt>list</tt> command to examine the root directory:

<pre>
list /
</pre>

And use the <tt>run</tt> command to run a program:

<pre>
run /SAVER.EXE
</pre>

If you want to try it on a real machine, burn the CD-ROM image
to a physical disc and reboot the machine.  It may or may not work!

## Compatibility

Basekernel compilation on linux has been tested with the following compilers:  

| Compiler | Status                   |
|----------|--------------------------|
| GCC 4.7  | :heavy_check_mark: Works |
| GCC 5.4  | :heavy_check_mark: Works |
| GCC 6.3  | :heavy_check_mark: Works |
| GCC 7.1  | :heavy_check_mark: Works |

Basekernel has been tested with the following VM hypervisors:

| VM Package    | Status                      | Notes                                          |
|---------------|-----------------------------|------------------------------------------------|
| qemu          | :heavy_check_mark: Works    | No known issues                                |
| VirtualBox    | :warning: Works with issues | Mouse does not work, struct copies fail        |
| VMware Player | :x: [Fails](https://github.com/dthain/basekernel/issues/38)                   | No suitable video mode found at boot           |

## Hacks, Projects, and Other Ideas

Now that you have the code running, here is a whole raft of ideas to
try, starting some single-line code changes to more elaborate projects:

* Modify main.c to display a custom startup message to welcome the user.

* Tweak console.c to display things in a color of your choosing.

* Examine the rtc.c module, and write a little code in main.c
that will display a real-time clock.

* Modify the infinite loop in main.c to display a kernel command
prompt, and read some commands from the user to do things like 
show system statistics, display the current time, reboot, etc.

* Add some functions for drawing lines and shapes to graphics.c, and then use
in main.c to make an interesting status display for your operating system.

* Modify the console code so that it scrolls the display up when you
reach the bottom, rather than clearing and starting over.

* Try writing to an address somewhere into the (unmapped) user address space
above 0x80000000.  Notice how you get a page fault, and the system halts?
Modify the page fault handler to automatically allocate a fresh memory page
and map it into the process each time this happens.

* Implement proper memory resource management.  Create some system calls
that allow each process to request/release memory in an orderly manner,
and map it into the process in a suitable location.  Establish a policy
for how many pages each process can get, and what happens when memory
is exhausted.

* Write some easily-recognizable data to a file, then mount it as a virtual disk
image in your virtual machine of choice.  Use the ATA driver to load and
display the raw data on the virtual disk.

* Write a read-write driver for a Unix-style filesystem with proper inodes
and directory entries.  Copy data from your CDROM filesystem into the Unix
filesystem, which you can now save across boot sessions.

* Implement loadable programs.  Using either a filesystem or just a raw disk
image file, load a user-level program compiled outside the kernel,
and execute it.  The simplest way is to compile straight to binary format
with a load address of 0x80000000, and then fall through process_switch().
A more sophisticated way is to create an A.OUT or ELF format executable,
and then write the code to load that into the kernel.

* Create a display system for multiple processes.  Building on the bitmapped
graphics module, create a set of virtual displays that allow each process
to display independently without interfering with each other.

* Port the system to 64-bit mode.  Start by modifying kernelcore.S to make
a second jump from 32 to 64 bit mode, then update all of the types and
kernel structures to accommodate the new mode.

* Create a driver for the PCI bus, which enumerates all devices and their configurations.

* Create an AHCI driver for SATA disks.

* Create an OHCI or EHCI driver for USB devices.

## Machine Organization

The following memory organization is used throughout the code.
To the extent possible, these definitions have been collected
in memorylayout.h, but changing any of them may require some
some significant code surgery.

### Physical Memory Layout

```
0000 0000 Reserved area for initial interrupt vector and BIOS data.
0000 7C00 (BOOTBLOCK_START) 512 bytes where bootblock.S is initially loaded.
0000 fff0 (INTERUPT_STACK_TOP) Initial location of kernel/interrupt stack
          until paging and user processes allocate their own stacks.
0001 0000 (KERNEL_START) Start of kernel code and data in kernelcore.S
0010 0000 (KMALLOC_START) Start of kernel memory managed by kmalloc().
0020 0000 (MAIN_MEMORY_START)  Start of memory pages managed by memory.c
???? ???? Location of the video buffer, determined by video BIOS at runtime.
          Care must be taken in memory allocation and pagetable setup
          to avoid stomping on this area.
```

### Virtual Memory Layout

```
0000 0000 First 2GB of VM space for all processes is directly mapped to
          physical memory in kernel mode.  That way, kernel space is
          inaccessible in user mode, but kernel code can run correctly
          with paging activated.
8000 0000 (PROCESS_ENTRY_POINT) The upper 2GB of VM space for all processes
          is private to that process.  Each page of VM here is mapped to
          physical page allocated by memory.c.
ffff fff0 (PROCESS_STACK_INIT) The high end of the user space is designated
          for the user level stack, which grows down towards the middle
          of memory.
```

### Segmentation

```
		Start	Length		Type	Privilege
Segment 1	0	4GB		CODE	SUPERVISOR
Segment 2	0	4GB		DATA	SUPERVISOR
Segment 3	0	4GB		CODE	USER
Segment 4	0	4GB		DATA	USER
Segment 5	tss	sizeof(tss)	TSS	SUPERVISOR
```

For technical reasons, the X86 requires that segmentation be turned on whenever paging is used.  A few key places in interrupt handling and system startup require the use of segments.  So, we set up four segments that span all of memory, and use the one with the desired privilege wherever a segment is required.  The fifth segement identifies the Task State Structure (TSS) which is where the initial setup information for entering protected mode is stored.

## Features

### Graphics

Each process has an array of up to five windows. When a process is created,
it inherits the windows of its parent, except for the initial process which has one window that fills the screen.
A process may create new windows through the `draw_create` system call, which creates a new window that is a subset of a window
already open by the process. In this way, a process' windows are sandboxed - a process cannot create a window larger than
what its parent has passed on to it. Windows are reference counted, and when all processes owning a window are dead, 
the window is freed.

Draws are issued to the window through the `draw_write` system call. The `draw_write` system call sends a null-terminated buffer of 
`graphics_command`s to the kernel, which then performs all of the draw operations through one system call. There is a standard
buffer for the `graphics_command`s provided in `user-io`, which also provides functions for filling the buffer.
The `draw_write` call is stateful, and within a call the window being drawn to is set by a command, and which window
is being drawn to persists until either a command switches which window is being drawn to or it reaches the  end
of the `graphics_command` buffer. The current foreground drawing color is persistent through even calls to `draw_write`, and
will only change when a command is issued to change it.

Each `graphics_command` is a struct containing an integer identifying the type of command followed by an array of four integers
providing the arguments for the command. Not every command uses all four arguments, and those that don't simply ignore the
extra arguments. The list of valid commands is as follows:  
```
GRAPHICS_WINDOW  W        sets the draw window to W
GRAPHICS_COLOR   R G B    sets the draw color to (R, G, B)
GRAPHICS_RECT    X Y W H  draws a rectangle with width W and height H at (X, Y)
GRAPHICS_CLEAR   X Y W H  clears a rectangle with width W and height H at (X, Y)
GRAPHICS_LINE    X Y W H  draws a line starting at (X, Y) that travels W pixels horizontally and H pixels vertically
GRAPHICS_TEXT    X Y S    draws a string S at (X, Y) (note that S is a char*, so strings are passed by reference)
```

### Processes

A process is created by the `process_run` system call, which takes the path to the program to run, an array of string arguments,
and the number of arguments. These arguments must be under 255 characters in length, and are placed above the process stack
at the time the process is created.

Processes contain a process id (pid) as well as a process parent id (ppid). The pid is allocated by a next fit search
through an array of 1024 available ids. This array also serves to allow easy lookup of processes by id.

A process is killed in any of three cases:  
    1. It performs the `exit` system call.  
    2. It is killed by another process by the `process_kill` system call.  
    3. Its parent is killed.  

When a process is killed, it is removed from any process queue it might be in (i.e. the ready queue or an I/O queue) and placed in the
grave queue. Additionally, any child of that process is killed (as well as any of their children, etc.). A process in the grave queue
is not cleaned up immediately, however. An immediate parent of a process can get the exit status of a process by using the
`process_wait` system call. This will return the exit records of the first of that process' dead children in the grave queue.
`process_wait` takes a timeout argument, and will wait for as long as the timeout before returning with no process dead.
`process_wait` does not clean the records of a process. Instead, `process_reap` must be called to discard a process completely.
When a process is reaped, its windows' reference counters are decreased, its pagetable and kernel object are freed, and its pid is
opened for reuse.


## References

The [OS Development Wiki][1] is an excellent source of sample code and general orientation toward the topics that you need to learn. However, to get the details of each element right, you will need to dig into the source documents for each component of the system:

The [Intel CPU Manuals][2] are the definitive CPU reference.  Especially focus on the first five chapters of the * System Programming Guide * to fully understand the virtual memory model and exception handling.

The `doc` directory contains a few hard to find PDF specifications.
While they describe older hardware, standardized hardware has a high
degree of backwards compatibility, so these techniques continue to work.

[ATA Attachment-3 Interface][3] describes the interface
to traditional ATA disks, including both I/O registers and commands
interpreted by the disk.

[PhoenixBIOS Programmer's Guide][4] describes the BIOS
functions available in the early boot environment, while the system is
still in 16-bit mode.  These are used by bootblock.S and kernelcore.S
to load data from disk and initialize the system.

[VESA BIOS Extension (VBE)][5] describes extensions
to the BIOS interrupts specifically for setting and querying graphics
modes.  This is used by kernelcore.S just before jumping to protected mode.

[Super I/O Controller][6] describes a typical chipset
that provides basic support devices such as keyboard, serial ports,
timer, and real-time clock.  These can be accessed in any processor mode.
While your machine may not have this *specific* chipset, the basic
functions are widely compatible.

[1]: http://wiki.osdev.org
[2]: http://www.intel.com/content/www/us/en/processors/architectures-software-developer-manuals.html
[3]: doc/ata-3-std.pdf
[4]: doc/phoenix-bios.pdf
[5]: doc/vesa-bios.pdf
[6]: doc/super-io.pdf
