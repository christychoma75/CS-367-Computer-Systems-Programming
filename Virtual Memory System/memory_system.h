#ifndef MEMORY_SYSTEM_H
#define MEMORY_SYSTEM_H

#define OFFSET_BITS		6  // VPO and PPO
#define PPN_BITS 		10  
#define VPN_BITS 		18 

#define TLB_TAG_BITS		8  // TLB
#define TLB_INDEX_BITS		10

#define CACHE_TAG_BITS		7  // L1 Cache
#define CACHE_INDEX_BITS 	5
#define CACHE_OFFSET_BITS	4

#define TLB_SET_ASSOC 		4  // Cache Designs
#define CACHE_SET_ASSOC 	2

//   For the logging system
#define NEW_ADDRESS 		0
#define ADDRESS_FROM_TLB 	1
#define ADDRESS_FROM_PAGETABLE 	2
#define DATA_FROM_CACHE 	3
#define DATA_FROM_MEMORY 	4
#define ILLEGALVIRTUAL 		5
#define PHYSICALERROR 		6
#define ILLEGALVPN 		7
#define PAGEFAULT 		8

#define MAX_ADDR  1 << (VPN_BITS + OFFSET_BITS)
#define IN_BUF_SIZE 64

void initialize();
void teardown();
int get_physical_address(int);
char get_byte(int);

extern int handle_page_fault(int);
extern int get_word(int);
int get_int();
void  start_logging(), stop_logging(), log_entry(int,int);

#endif
