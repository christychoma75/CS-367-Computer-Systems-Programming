 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "memory_system.h"

 void initialize(){}
 void teardown(){}
  
// implements TLB, 4 way set Associative, # sets = 2^10 = 1024
int tlbValid[1024], tlbValid2[1024], tlbValid3[1024], tlbValid4[1024];
int tlbTag[1024], tlbTag2[1024], tlbTag3[1024], tlbTag4[1024];
int tlbPpn[1024], tlbPpn2[1024], tlbPpn3[1024], tlbPpn4[1024];

// implements Page Table, has entry for every vpn -> 2^18 = 262144
int pageTable[262144];
int pageTableValid[262144];

// counter used to keep track of most recently accessed
int timerCounter[4];  
int timerCounterl1[2];

/* This function takes a virtual address an int and returns 
 * a corresponding physical address as an int.
 *
 * Conditions:
 *  1) If the input address is too large, log the virt_address with
 *       the ILLEGALVIRTUAL code and return -1
 *  2) If the PPN is in the TLB, log the physical address generated with the 
 *       ADDRESS_FROM_TLB code.
 *  3) If the PPN is not in the TLB, but is in the Page Table, then log the
 *       physical address generated with the ADDRESS_FROM_PAGETABLE code and then
 *       update the TLB.
 *  4) If there is a Page Fault, use the handle_page_fault(vpn) API
 *       function and then log the physical address generated with the 
 *       PAGEFAULT code, then update the Page Table, then update the TLB.
 *  For any case 2-4, return the physical address.
 */
  
int get_physical_address(int virt_address) {

    // largest possible value -> 24 bits or 2^24 
    if(virt_address > 16777216)
    {
    	log_entry(ILLEGALVIRTUAL, virt_address);
	return -1;
    }
    int phys_address;  
    int ppn;  // 10 bits
   	
    // 0000 0000 0000 0000 0011 1111  -> vpo = 6 bits valid
    int maskVpo = 0x00003f;

    // 0000 0000 0011 1111 1111  -> index = 10 bits valid
    int maskIndex = 0x003ff;   
    
    // vpo = ppo = 6 bits
    int vpo = maskVpo & virt_address;
    int ppo = vpo; 	

    // vpn = 18 bits (tag + index)
    int vpn = virt_address >> 6;

    // index = 10 bits
    int vindex = maskIndex & vpn;

    // tag = 8 bits
    int vtag = vpn >> 10;

    // check for tlb cache hit
    if((tlbTag[vindex] == vtag && tlbValid[vindex] == 1) ||
    	(tlbTag2[vindex] == vtag && tlbValid2[vindex] == 1) || 
	(tlbTag3[vindex] == vtag && tlbValid3[vindex] == 1) || 
	(tlbTag4[vindex] == vtag && tlbValid4[vindex] == 1))
    {	
	// check for matching tag and updates corresponding tlb
	// entry with ppn, also updates timer
	if(tlbTag[vindex] == vtag) 
	{
		ppn = tlbPpn[vindex];
		timerCounter[0]++;
	}
	else if(tlbTag2[vindex] == vtag) 
	{
		ppn = tlbPpn2[vindex];
		timerCounter[1]++;
	}	
	else if(tlbTag3[vindex] == vtag)
	{
		ppn = tlbPpn3[vindex];
		timerCounter[2]++;
	}
	else if(tlbTag4[vindex] == vtag)
	{
		ppn = tlbPpn4[vindex];
		timerCounter[3]++;
	}
       
        // gets physical address using ppn and ppo
	phys_address = (ppn << 6) | ppo;

	// found in TLB
	log_entry(ADDRESS_FROM_TLB, phys_address); 
	return phys_address;
    }
	
    // Checks conditions for Cache miss. If tlb entry is invalid,
    // set valid to 1, set tag, and update timer
    if(tlbValid[vindex] == 0)
    {  
 	tlbValid[vindex] = 1;	// update tlb
        tlbTag[vindex] = vtag;  
	timerCounter[0] = 0;
    }
    else if(tlbTag[vindex] != vtag && (tlbValid2[vindex] == 0))
    {
 	tlbValid2[vindex] = 1;	// update tlb
        tlbTag2[vindex] = vtag;
	timerCounter[1] = 0;
    }
    else if(tlbTag2[vindex] != vtag && (tlbValid3[vindex] == 0))
    {
 	tlbValid3[vindex] = 1;	// update tlb
        tlbTag3[vindex] = vtag;
	timerCounter[2] = 0;
    }
    else if(tlbTag3[vindex] != vtag && (tlbValid4[vindex] == 0))
    {
 	tlbValid4[vindex] = 1;	// update tlb
        tlbTag4[vindex] = vtag;
	timerCounter[3] = 0;
    }
    // this condition means that all entries are filled in tlb and needs
    // to be replaced. If all entries are valid replace one least recently accessed
    else 
    {
	// gets the index of the smallest element of counter array
	int smallest = timerCounter[0];  // assume first element is smallest
	int indexSmallest = 0;		 // assume first index is smallest
	for(int i = 0; i < 3; i++)
	{
		if(smallest > timerCounter[i + 1])
		{
			smallest = timerCounter[i + 1];
			indexSmallest = i + 1;
		}
	}

	// Update corresponding tlb tag given smallest index
	if(indexSmallest = 0){
		tlbTag[vindex] = vtag;
	}
	else if(indexSmallest = 1){
		tlbTag2[vindex] = vtag;
	}
	else if(indexSmallest = 2){
		tlbTag3[vindex] = vtag;
	}
	else{
		tlbTag4[vindex] = vtag;
	}
    }
  
    // Checks page table. if not found in page table, get ppn
    if(pageTableValid[vpn] == 0)  
    {
        pageTableValid[vpn] = 1;
	pageTable[vpn] = handle_page_fault(vpn);  // updates pagetable

	if(tlbTag[vindex] == vtag)
	{
		tlbPpn[vindex] = pageTable[vpn];  // updates TLB with ppn
	}
	else if(tlbTag2[vindex] == vtag)
	{
		tlbPpn2[vindex] = pageTable[vpn];
	}
	else if(tlbTag3[vindex] == vtag)
	{
		tlbPpn3[vindex] = pageTable[vpn];
	}
	else if(tlbTag4[vindex] == vtag)
	{
		tlbPpn4[vindex] = pageTable[vpn];
	}

	ppn = pageTable[vpn];

	// gets physical address given ppn and ppo
	phys_address = (ppn << 6) | ppo;

	// not found in page table
	log_entry(PAGEFAULT, phys_address);   
    }
    else  
    {
 	ppn = pageTable[vpn];
        phys_address = (ppn << 6) | ppo;

	// found in page table
	log_entry(ADDRESS_FROM_PAGETABLE, phys_address);
    }
    return phys_address;
}

/* This function takes a physical address as an int and returns 
 *  a corresponding byte of data as a char.  
 *
 * Conditions:
 *  1) If the data is in the cache, log the byte from the cache with the 
 *       DATA_FROM_CACHE code and then return the byte.
 *  2) If there is a Cache Miss, use the get_word(phys_address) API function as needed
 *       to fully update the cache entry, and then log the byte at the requested address
 *       with the DATA_FROM_MEMORY code, then update the Cache.
 *  For any case, return the byte retrieved.
 */

// implements L1 cache, 2^5 = 32 entries, 4 arrays for 4 different offsets
int l1Valid[32], l1Valid2[32];
int l1Tag[32], l1Tag2[32];
int l1DataZero[32], l1DataZero2[32];
int l1DataOne[32], l1DataOne2[32];
int l1DataTwo[32], l1DataTwo2[32];
int l1DataThree[32], l1DataThree2[32];
 

char get_byte(int phys_address) {
   char byte; // Variable for the byte you will fetch.

   // 0000 0001 1111 0000 -> index = 5 valid bits
   int maskIndex = 0x01f0;

   // 0000 0000 0000 1111 -> offset = 4 valid bits
   int maskOffset = 0x000f;
   
   // 1111 1110 0000 0000 -> tag = 9 valid bits
   int tag = phys_address >> 9;  
   int index = (maskIndex & phys_address) >> 4;
   int offset = maskOffset & phys_address;

   int mask0 = 0x000000ff;
   int mask1 = 0x0000ff00;
   int mask2 = 0x00ff0000;
   int mask3 = 0xff000000;

   // checks condition for l1 cache hit
   if((l1Valid[index] == 1 && l1Tag[index] == tag) || 
   	l1Valid2[index] == 1 && l1Tag2[index] == tag)
   {
	// sets byte to corresponding data given the offset and updates timer
	if(offset == 0)
	{ 
		// for 2 way set associative, checks tag
	        if(l1Tag[index] == tag)
		{
			byte = l1DataZero[index];
			timerCounterl1[0]++;
		}
		else
		{
			byte = l1DataZero2[index];
			timerCounterl1[1]++;
		}
	}
	else if(offset == 1)
	{
		if(l1Tag[index] == tag)
		{
			byte = l1DataOne[index];
			timerCounterl1[0]++;
		}
		else
		{
			byte = l1DataOne2[index];
			timerCounterl1[1]++;
		}
	}
	else if(offset == 2)
	{
		if(l1Tag[index] == tag)
		{
			byte = l1DataTwo[index];
			timerCounterl1[0]++;
		}
		else
		{
			byte = l1DataTwo2[index];
			timerCounterl1[1]++;
		}
	}
	else if(offset == 3)
	{
		if(l1Tag[index] == tag)
		{
			byte = l1DataThree[index];
			timerCounterl1[0]++;
		}
		else
		{
			byte = l1DataThree2[index];
			timerCounterl1[1]++;
		}
	}

	// found in cache
	log_entry(DATA_FROM_CACHE, byte);
	return byte;
   }
   else
   {
	// checks conditions for cache miss
	if(l1Valid[index] == 0)
	{
 	     l1Valid[index] = 1;	// updates valid bit
      	     l1Tag[index] = tag;	// updates tag
	     timerCounterl1[0] = 0;	// updates timer

	     // cache entry holds 16 bytes of data, updates data with 4 
	     // possible offset entries 
	     int temp = get_word(phys_address);
	     l1DataZero[index] = temp & mask0;
             l1DataOne[index] = (temp & mask1) >> 8;
       	     l1DataTwo[index] = (temp & mask2) >> 16;
	     l1DataThree[index] = (temp & mask3) >> 24;
	 }
	 else if(l1Valid2[index] == 0)
	 // repeats same conditions for 2 way set associative
	 {
	     l1Valid2[index] = 1;
      	     l1Tag2[index] = tag;
	     timerCounterl1[1] = 0;

	     int temp = get_word(phys_address);
	     l1DataZero2[index] = temp & mask0;
             l1DataOne2[index] = (temp & mask1) >> 8;
       	     l1DataTwo2[index] = (temp & mask2) >> 16;
	     l1DataThree2[index] = (temp & mask3) >> 24;
	 }
	 // this condition means all entries are filled in l1 cache and needs
	 // to be replaced. Of all entires are valid replace least recently accessed
	 else 
	 {
	     // gets smallest index out of 2 elements in counter array
	     int smallest = timerCounterl1[0];
	     int smallestIndex = 0;
	     if(smallest > timerCounterl1[1])
	     {
		smallestIndex = 1;
	     }

	     // updates corresponding tag and data given smallest index
	     if(smallestIndex = 0){
		l1Tag[index] = tag;
                int temp = get_word(phys_address);
	    	l1DataZero[index] = temp & mask0;
             	l1DataOne[index] = (temp & mask1) >> 8;
       	     	l1DataTwo[index] = (temp & mask2) >> 16;
	     	l1DataThree[index] = (temp & mask3) >> 24;
	     }
	     else{
	     	l1Tag2[index] = tag;
	        int temp = get_word(phys_address);
	    	l1DataZero2[index] = temp & mask0;
             	l1DataOne2[index] = (temp & mask1) >> 8;
       	     	l1DataTwo2[index] = (temp & mask2) >> 16;
	     	l1DataThree2[index] = (temp & mask3) >> 24;
	     }
	 }
   }

   // gets the corresponding byte given the offset
   if(offset == 0) 
   {
   	if(l1Tag[index] == tag){
		byte = l1DataZero[index];
	}
	else{
		byte = l1DataZero2[index];	// for 2 way set associative
	}
   }
   else if(offset == 1)
   {
   	if(l1Tag[index] == tag){
		byte = l1DataOne[index];
	}
	else{
		byte = l1DataOne2[index];
	}
   }
   else if(offset == 2)
   {
   	if(l1Tag[index] == tag){
		byte = l1DataTwo[index];
	}
	else{
		byte = l1DataTwo2[index];
	}
   }
   else
   {
   	if(l1Tag[index] == tag){
		byte = l1DataThree[index];
	}
	else{
		byte = l1DataThree2[index];
	}
   }
   
   // l1 cache miss
   log_entry(DATA_FROM_MEMORY, byte);
   return byte;
}

