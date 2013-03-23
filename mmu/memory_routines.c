#define MMU_ON          0
/*return page table address*/
void * get_page_table_addr(void){}
/*return the physical address*/
void * get_page_table_addr_phys(void){}
/*sets address of the page table to use*/
void * set_page_table_addr(void * table){}
/**
Turn on the MMU by loading a 1 i the first bit of co procerssor 15.
Hopefully.

**/

void mmu_enable(void){
 asm("
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r0, #0x1
    mcr p15, 0, r0, c1, c0, 0
    ");

}

/**
Turn the MMU off by loading a 0 in the first bit of co processor 15.
Hopefully.
**/
void mmu_disable(void){
asm("
    mrc p15, 0, r0, c1, c0, 0
    and r0, r0, #0x0
    mcr p15, 0, r0, c1, c0, 0

    ");

}

void mmu_init(void){

}

unsigned int mmu_is_enabled(void){
    return MMU_ON; 
}

}
