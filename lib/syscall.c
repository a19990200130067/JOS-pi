// System call stubs.

#include <inc/syscall.h>
#include <inc/lib.h>

static inline int32_t
syscall(int num, int check, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
	int32_t ret;

	uint32_t args[5];
	args[0] = a1;
	args[1] = a2;
	args[2] = a3;
	args[3] = a4;
	args[4] = a5;	

	asm volatile("mov r0, %[syscall_num]\n"
		     "mov r1, %[args]\n"
		     "ldr r2, [r1, #4]\n"
		     "ldr r3, [r1, #8]\n"
		     "ldr r4, [r1, #0xc]\n"
		     "ldr r5, [r1, #0x10]\n"
		     "ldr r1, [r1]\n"
		     "swi #10\n"
		     "mov %[result], r0\n"
		     : [result] "=r" (ret)
		     : [syscall_num] "r" (num),
		       [args] "r" (args)
		     : "cc", "memory");

	if(check && ret > 0)
		panic("syscall %d returned %d (> 0)", num, ret);

	return ret;
}

void
sys_cputs(const char *s, size_t len)
{
	syscall(SYS_cputs, 0, (uint32_t)s, len, 0, 0, 0);
}

int
sys_cgetc(void)
{
	return syscall(SYS_cgetc, 0, 0, 0, 0, 0, 0);
}

int
sys_env_destroy(envid_t envid)
{
	return syscall(SYS_env_destroy, 1, envid, 0, 0, 0, 0);
}

envid_t
sys_getenvid(void)
{
	 return syscall(SYS_getenvid, 0, 0, 0, 0, 0, 0);
}

void
sys_yield(void)
{
	syscall(SYS_yield, 0, 0, 0, 0, 0, 0);
}

int
sys_page_alloc(envid_t envid, void *va, int perm)
{
	return syscall(SYS_page_alloc, 1, envid, (uint32_t) va, perm, 0, 0);
}

int
sys_page_map(envid_t srcenv, void *srcva, envid_t dstenv, void *dstva, int perm)
{
	return syscall(SYS_page_map, 1, srcenv, (uint32_t) srcva, dstenv, (uint32_t) dstva, perm);
}

int
sys_page_unmap(envid_t envid, void *va)
{
	return syscall(SYS_page_unmap, 1, envid, (uint32_t) va, 0, 0, 0);
}

// sys_exofork is inlined in lib.h

int
sys_env_set_status(envid_t envid, int status)
{
	return syscall(SYS_env_set_status, 1, envid, status, 0, 0, 0);
}

int
sys_env_set_trapframe(envid_t envid, struct Trapframe *tf)
{
	return syscall(SYS_env_set_trapframe, 1, envid, (uint32_t) tf, 0, 0, 0);
}

int
sys_env_set_pgfault_upcall(envid_t envid, void *upcall)
{
	return syscall(SYS_env_set_pgfault_upcall, 1, envid, (uint32_t) upcall, 0, 0, 0);
}

int
sys_ipc_try_send(envid_t envid, uint32_t value, void *srcva, int perm)
{
	return syscall(SYS_ipc_try_send, 0, envid, value, (uint32_t) srcva, perm, 0);
}

int
sys_ipc_recv(void *dstva)
{
	return syscall(SYS_ipc_recv, 1, (uint32_t)dstva, 0, 0, 0, 0);
}

unsigned int
sys_time_msec(void)
{
	return (unsigned int) syscall(SYS_time_msec, 0, 0, 0, 0, 0, 0);
}







// network syscal api
int
sys_net_try_send(char *data, int len)
{
	return syscall(SYS_net_try_send, 1, (uint32_t) data, len, 0, 0, 0); 
}

int
sys_net_try_receive(char *data, int *len)
{
	return syscall(SYS_net_try_receive, 1, (uint32_t) data, (uint32_t) len, 0, 0, 0); 
}

int
sys_get_mac(uint32_t *low, uint32_t *high) 
{
	return syscall(SYS_get_mac, 1, (uint32_t) low, (uint32_t) high, 0, 0, 0);
}

