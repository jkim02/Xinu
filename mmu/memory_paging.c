#include "mmu.h"
#include <stddef.h>

/**
 * Sets the bit in the PAT as 1 to denote an allocated page
 *
 * @param page the page number to set as allocated
 */
void set_page_alloc(unsigned int page) {
    unsigned int * PAT;
    unsigned int mask;
    unsigned int index;

    PAT = (unsigned int *)PAT_ADDRESS;
    mask = 1 << (page & 0x1F);
    index = (page & 0xFFFFFFE0) >> 5;

    //TODO get current process, change its PAT

    PAT[index] |= mask;
}

/**
 * Sets the bit in the PAT as 0 to denote a deallocated page
 */
void unset_page_alloc(unsigned int page) {
    unsigned int * PAT;
    unsigned int mask;
    unsigned int index;

    PAT = (unsigned int *)PAT_ADDRESS;
    mask = ~(1 << (page & 0x1F));
    index = (page & 0xFFFFFFE0) >> 5;

    //TODO get current process, change its PAT

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

unsigned int alloc_page_align(unsigned int align) {
    return alloc_pages_align(1,align);
}

unsigned int alloc_pages(unsigned int amount) {
    return alloc_pages_align(amount,1);
}

/**
 * Allocates a single page aligned to a count of page boundries
 *
 * @param align the page number to align to
 *
 * @return page number allocated in physical memory
 */
unsigned int alloc_pages_align(unsigned int amount, unsigned int align) {
    //TODO get the memory size in here
    unsigned int total_pages = 128*1024*1024/PAGE_SIZE; //get number of pages
    unsigned int ret; //the page number that gets returned
    unsigned int * PAT = (unsigned int *)PAT_ADDRESS; //the array of bits for allocated pages
    unsigned int i;
    unsigned int index;
    unsigned int mask;
    bool done;

    //loop through until you find a page that is free
    for(ret = 0; ret < total_pages; ret += align) { //increasing by align ensures alignment
        done = 0; //set to false
        for(i = 0; i < amount; ++i) {
            index = ((ret + i) & 0xFFFFFFE0) >> 5;
            mask = (ret + i) & 0x1F;
            if((PAT[index] & mask) != 0)
                break;
            if(i == amount - 1)
                done = 1; //set to true
        }
        if(done)
            break;
    }

    //if there are no free pages of the correct alignment, return SYSERR
    if(ret >= total_pages)
        return SYSERR;

    //allocate the page in the table
    for(i = 0; i < amount; ++i)
        set_page_alloc(ret + i);

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
    if((int)page_number_to_address(page) == SYSERR)
        return;

    //free the page in the PAT
    unset_page_alloc(page);
}

/**
 * Deallocates a group of pages that are contiguous in physical memory
 *
 * @param page the base page number to start freeing from
 * @param amount the amount of pages to free
 */
void dealloc_pages(unsigned int page, unsigned int amount) {
    unsigned int i;

    //loop through the number of pages
    for(i = 0; i < amount; ++i)
        //free each page individually
        dealloc_page(page+i);
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
    void * mem = (void *)(PAGE_SIZE << PAGE_SHIFT);

    //check against the size of the memory
    //TODO used constant for memory size
    if(((unsigned int)mem) >= 128*1024*1024)
        return (void *)SYSERR;

    //return the memory address
    return mem;
}
