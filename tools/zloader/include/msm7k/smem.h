
#ifndef _SMEM_H_
#define _SMEM_H_

#define SZ_DIAG_ERR_MSG 0xC8
#define ID_DIAG_ERR_MSG 6
#define ID_HEAP_INFO    3
#define ID_SMD_CHANNELS 13
#define ID_SHARED_STATE 82
#define BREW_AMSS_HEAP_SIZE 138

struct smem_heap_info
{
	unsigned initialized;
	unsigned free_offset;
	unsigned heap_remaining;
	unsigned reserved;
};

struct smem_heap_entry
{
	unsigned allocated;
	unsigned offset;
	unsigned size;
	unsigned reserved;
};

struct smem_proc_comm
{
	unsigned command;
	unsigned status;
	unsigned data1;
	unsigned data2;
};

struct smem_shared
{
	struct smem_proc_comm proc_comm[4];
	unsigned version[32];
	struct smem_heap_info heap_info;
	struct smem_heap_entry heap_toc[BREW_AMSS_HEAP_SIZE];
};	
	
struct smsm_shared
{
	unsigned host;
	unsigned state;
};

void dump_smem_info(void);

#endif
