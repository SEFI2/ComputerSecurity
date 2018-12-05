CS481-2018 Lab1 (50 pt for 10%)
===============

Through this lab, we will have an experience on writing an exploit that triggers
some bugs in a malicious way. As these are much more difficult and time
consuming on modern systems, we will be working with a bit older, 32-bit
systems with some security features turned off.

Before getting started, you may want to get pwnddb, by running
scripts/get-pwndbg.sh.

(10pt) Exercise 1: Memory layout @ layout
--------------------------

Here we first check the layout of a process running on Linux. Prior to get
started, turn off ASLR with this command.

```bash
  $ sudo sysctl -w kernel.randomize_va_space=0
```
And then generate executables.
```
  $ make
```
Now you have `layout` and `layout-insecure`

> (2pt) Q1. Run the ./layout. What it prints is in fact the `/proc/<pid>/maps` file.
> Can you locate the stack and heap? Please describe how and where.

> (2pt) Q2. We all are aware that a program also has "global" variables and code.
> How can we locate them? Please not here that the rightmost column shows
> which file the memory content is generated from and the second from left
> column shows the permission that the process has for those memory regions.
>
> (hint): the second from the left column, that looks like r-xp indicates the
> access permission that the program has, to those memory regions. Thinking
> of global variables, which permission should the program have? Can we 
> overwrite (update) the global variables? 
> Also, the three memory regions that are loaded from our layout binary
> should include the "code". Which permission should the code memory be
> associated with?

Why do they look like that? The kernel `libc` have a set of routines commonly
called as *loader* that builds up such a memory layout before the function
*main* is called. The executable file, in this case an *ELF* file, contains
how the loader should build up the layout.

```
  $ readelf -S layout
```

shows some information about the components of the `layout` ELF file. As you
see here, there are a lot of sections in it, but here we just want to check
`text`, `data`, and `rodata` sections, which are mapped to the memory by loader.
`text` is where the program code is stored, `data` is for writable global
variables, and `rodata` is for read-only data.

> (2pt) Q3. The memory map that `layout` prints shows three memory regions mapped
> from the content of `layout`. Describe which section each region is generated
> from, from the permission that the program has.
>
> (hint) As out loader should respect the permission of sections in the ELF
> file, you can find out the pairs by comparing the permissions they are
> associated with.

Finally, let's see how insecure a program in old days by running
`layout-insecure`. As you can see from `Makefile`, the only difference is
the compiler flags.

> (2pt) Q4. What differences can you identify from `layout-insecure`,
> compared to `layout`?
>
> (hint) Looking at the Makefile, layout-insecure is built with an additional
> flag, -z execstack, which direct the loader to turn off DEP on stack.

Lastly, let's see the impact of the address space layout randomization,
which is enabled by default on all modern computer systems.

Run the following command and run `layout` several times.

```
sudo sysctl -w kernel.randomize_va_space=2
```

> (2pt) Q5. What differences do you observe?


*Please make sure to turn ASLR off again for the rest of this lab.*

(15pt) Exercise 2: The first buffer overflow @ bof
---------------------------

Now we should have an idea of how does a program's memory look like.
Then let's see the impact of undefined behaviors by causing and observing
the result of *buffer overflows*.

> (3pt) Q1. Let's take a look at `bof.c`. What's the problem? Where and why
the program may access undefined memory?
>
> (hint) As the name indicates, you may want to locate where buffer overflow
> happens.

> (3pt) Q2. `bof` prints the value of `is_admin` at the end. What input should
we give to make it print something other than 2?
>
> (hint) Apparently, is_admin will be allocated after the buffer. You can try
> either brute-force approach in which you just give inputs randomly, or 
> use gdb to find out the offset from the buggy buffer to the variable.

> (3pt) Q3. Does the program crash, showing something like Segmentation Fault?
If not, how can we crash it? If it does, how can we avoid it?
>
> (hint) Interestingly, buffer overflow does not always crash the program, but
> you can make it, by giving it a really long input. Why?

> (3pt) Q4. Now let's try to make it print 1. What should we do? Take a look at
driver.c to get an idea and write something (code) that allows you to do so.
>
> (hint) Typically people write a script to trigger this kind of bug
> conveniently, but here let's stick to C. But why am I telling you to use 
> a driver code at this moment? The reason is that, to make the value 1,
> we should pass the value 1 somehow. As far as I know, we cannot type a
> character of ascii value 1.

> (3pt) Q5. Can we make it print 0? If possible, write something to do so. If not,
>    describe why.
>
> (hint) Either by typing or using a driver program, you are sending a string to
> the program bof. What happens when bof meets 0, which we want to write to
> is_admin?

(10pt) Exercise 3: Control-flow hijacking @ hijack
----------------------------------

What is control-flow? You can think it of as a sequence of instructions
that are really executed. Given a program, as you may already know,
your processor may not run the program in order, if there is a branch
like `if`, `switch`, or `for`. Similarly, calling and returning from a function
also makes the processor to jump to another code location. For more information,
you can either take compiler courses or Google "control-flow graph".

If an attacker can run and interact with a process on a victim machine,
what program would the attacker want the process to run? It would be shell,
like `bash`, `dash` or `zsh`. The attacker can then invoke any program allowed by
the OS kernel.

Let's take a look at hijack.c. Conveniently, there is a function that turns
the program itself into a shell, though the program never calls it. The program
has the exact buffer overflow bug like the one we've seen earlier, but flag
is in global memory so we cannot corrupt it to trick the program to call
`shellcode()`. What can we do?

When a function returns, it uses an address which is stored in the stack!
As here the function that we want to call, `shellcode()` is a simple one that
doesn't have any arguments and does not return, we can in fact trick the program
to call the function rather than returning to its caller.

> (5pt) Q1. Let's locate the return address. How? You have two options. Firstly,
if you are familiar with gdb, you can do so. By observing where the program
returns to, you can find out the address that the function returns to, and
learn where is it stored within the stack. In fact, it's just where `ebp` points
to. Otherwise, you can edit the `hijack.c` itself to dump and edit the stack
contents.
>
> (hint) Let's use pwndbg. Then it in fact shows you the return address of
> the main function at "BACKTRACE" section. You may see 
> f 1 <addr> __libc_start_main+247, where <addr> is the return address.
> To make sure, you can set a breakpoint at the end of main function
> (e.g. b 34) and then check if the fuction really returns to the address
> by single stepping (s).
> Then where is the return address? You can in fact locate the return address
> that you found around the stack, but to be more accurate, you can look for
> the behavior of x86 ret instruction (where it loads the return address from).

> (5pt) Q2. Now we located the memory location we should corrupt. Write and describe
a code to drive `hijack` to call `shellcode()` for you. You may want to reuse
the code you wrote for Exercise 2, to fill with `driver.c`.
>


(15pt) Exercise 4: Shellcode @ shellcode
---------------------------

Life is not as easy as it was in Exercise 4. So we need to learn how
to bring our own shellcode, if there isn't one.

It's a much more difficult job in real world these days, but it was easier
when not so many people were aware of it. If we can execute something in the
stack, we put our shellcode, that invokes `/bin/sh` for us, there.

Typically we need to write one at assembly level, as it should run under
an unusual environment. However, as we don't strictly assume the background in
writing stuffs in assembly level, we are in fact providing you with one.

`shellcode.c` shows something that we want in C, but we cannot use it directly.
`shellcode.S` is the one prepared for you, to play with overflow bugs.

From `shellcode.c` and `shellcode.S` we can see what we need to do to have
a shell. On Linux, there is a system
call that turns the caller process into a process
running another program, of which an example is execve.

[[http://man7.org/linux/man-pages/man2/execve.2.html]]

  $ man execve

```c
int execve(const char *filename, char *const argv[], char *const envp[]);
```

execve loads the program "filename" with command line argument argv and
environment variables envp. If an attacker tricks the victim program
to call the system call with appropriate arguments ("/bin/sh"), the attacker
gets can potentially turn the program into a shell that will run commands
for herself.

> (5pt) Q1. Test the shell code by passing it to driver, in particular,
```
$ make
$ ./driver shellcode.bin
```
> Are you getting the shell? (thought you already have one..).
> Describe how does the driver gives you another shell.
>
> (hint) Take a look at shellcode.S, which is the source of shellcode.bin.
> What it does is to call execve system call with argument "/bin/sh",
> which turns itself into a process of "/bin/sh".

Let's now connect out experiences in buffer overflow and the shellcode
that we have.

> (5pt) Q2. This time the name of vulnerable program is injection. It has
the bug that we are now familiar with and sill has convenient routine
that copies incoming shellcode into its memory. Edit driver2.c to
inject and jump to the shellcode.
>
> (hint) From hijack excercise, we now know how can we redirect the process
> to a particular code address, and from the previous question, we know
> what and how can we put our own code into the victim process' memory.
> What you need to do is to combine them.
>


> (5pt) Q3. (Challenging) Take a look at shellcode.bin using hex editor. Can we pass
it over the standard input (stdin)? Or by typing (which means we can only give
some ascii strings)? Describe why it is not possible, and fix the shellcode
so that we can pass it over the stdin. In reality, we can provide only the
strings to the vulnerable program, in many cases. Use driver3.c to test,
which reads from stdin and jump to the incoming payload, conveniently for us.
>
> (hint) This was intended to be a pretty time consuming question, unless you
> cheat. However, I was told that there is a critical hole in this question,
> which make it much easier to answer. You can go with that answer, but make
> sure to explain wht it works.

