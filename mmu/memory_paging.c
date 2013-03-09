#include "mmu.h"
#include <stddef.h>

/**
 * Sets the bit in the PAT as 1 to denote an allocated page
 *
 * @param page the page number to set as allocated
 */
void set_page_alloc(unsigned int page) {
    unsigned int * PAT = (unsigned int *)PAT_ADDRESS;
    unsigned int mask = 1 << (page & 0x1F);
    unsigned int index = (page & 0xFFFFFFE0) >> 5;

    PAT[index] |= mask;
}

/**
 * Sets the bit in the PAT as 0 to denote a deallocated page
 */
void unset_page_alloc(unsigned int page) {
    unsigned int * PAT = (unsigned int *)PAT_ADDRESS;
    unsigned int mask = ~(1 << (page & 0x1F));
    unsigned int index = (page & 0xFFFFFFE0) >> 5;

    PAT[index] &= mask;
}

/**
 * Allocates a single page of memory with no alignment
 *
 * @return page number allocated in physical memory
 */
unsigned int alloc_page() {
    return alloc_page_align(1);
}

/**
 * Allocates a single page aligned to a count of page boundries
 *
 * @param align the page number to align to
 *
 * @return page number allocated in physical memory
 */
unsigned int alloc_page_align(unsigned int align) {
    //TODO get the memory size in here
    unsigned int total_pages = 128*1024*1024/PAGE_SIZE; //get number of pages
    unsigned int ret; //the page number that gets returned
    unsigned int * PAT = (unsigned int *)PAT_ADDRESS; //the array of bits for allocated pages

    //loop through until you find a page that is free
    for(ret = 0; ret < total_pages; ret += align) //increasing by align ensures alignment
        if((PAT[(ret & 0xFFFFFFE0) >> 5] & (1 << (ret & 0x1F))) == 0)
            break;

    //if there are no free pages of the correct alignment, return SYSERR
    if(ret >= total_pages)
        return SYS_ERR;

    //allocate the page in the table
    set_page_alloc(ret);

    //return the page number
    return ret;
}

/**
 * Deallocates a page of memory
 *
 * @param page the page the free
 */
void dealloc_page(unsigned int page) {
    //check to see if page is outside of physical memory
    if(page_number_to_address(page) == SYSERR)
        return;

    //free the page in the PAT
    unset_page_alloc(page);
}

/**
 * Changes the page number ot a physical address (null *)
 *
 * @param page the page number to change to address
 *
 * @return physical memory address associated with the page
 */
void * page_number_to_address(unsigned int page) {
    //calculate base memory address of the page
    void * mem = PAGE_SIZE << PAGE_SHIFT;

    //check against the size of the memory
    //TODO used constant for memory size
    if(((unsigned int)mem) >= 128*1024*1024)
        return SYSERR;

    //return the memory address
    return mem;
}
