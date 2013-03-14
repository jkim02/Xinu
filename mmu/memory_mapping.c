#include "mmu.h"
#include <stddef.h>

/**
 * Gets the L1 index from a virtual address
 *
 * @param addr teh virtual address to use
 *
 * @return the index into the L1 table this will exist
 */
unsigned int get_L1_index(void * addr) {
    return (((unsigned int)addr) & 0xFFF00000) >> 20;
}

/**
 * Gets the L2 index from a virtual address
 *
 * @param addr the address to use
 *
 * @return the index into the L2 table this will exist
 */
unsigned int get_L2_index(void * addr) {
    return (((unsigned int)addr) & 0x000FF000) >> 12;
}

/**
 * Fills a single entry inside of an L1 table
 *
 * @param table the L1 table to use, this needs to be already mapped into virtual memory space
 * @param index the index into the table the description you are setting is
 * @param type the type of L1 table entry this is (always 0b01)
 * @param domain domain to use (always 0b0000)
 * @param IMP implementation specific bit (always 0b0)
 * @param base_addr the base address of the corresponding L2 table location in the virtual memory space
 */
void fill_L1_index(struct mmu_L1_4k_desc * table, unsigned int index, unsigned int type, unsigned int domain,
                   unsigned int IMP, void * base_addr) {
    table[index].type = type;
    table[index].SBZ = 0;
    table[index].domain = domain;
    table[index].IMP = IMP;
    table[index].L2_base_addr = (((unsigned int)base_addr) >> 10);
}

/**
 * Fills a single entry inside of an L2 table
 *
 * @param table the L2 table to use, this needs to be already mapped into virtual memory space
 * @param index the index into the table the description you are setting is
 * @param type the type of L2 table entry thsi is (always 0b10)
 * @param bufferable flag saying whether this page is bufferable or not
 * @param cacheable flag saying whether this page is cacheable or not
 * @param ap the access protections to use for this page
 * @param base_addr the base address of the corresponding physical address that is being mapped
 */
void fill_L2_index(struct mmu_L2_4k_desc * table, unsigned int index, unsigned int type, unsigned int bufferable,
                   unsigned int cacheable, unsigned int ap, void * base_addr) {
    table[index].type = type;
    table[index].bufferable = bufferable;
    table[index].cacheable = cacheable;
    table[index].ap0 = table[index].ap1 = table[index].ap2 = table[index].ap3 = ap;
    table[index].phy_base_addr = (((unsigned int)base_addr) >> 12);
}

/**
 * Fills a page of memory with repeating data, used to fill empty tables in
 *
 * @param page the page number to fill in
 * @param data the address of data to fill with
 * @param data_size the size of each copt of data stored
 */
void fill_page_with_data(unsigned int page, void * data, unsigned int data_size) {
    void * phys;
    struct mmu_L1_4k_desc * L1_table;
    struct mmu_L2_4k_desc * L2_table;
    unsigned int L1_index;
    unsigned int L2_index;
    unsigned int i;
    unsigned int j;
    unsigned int entries;

    //get physical address of the page
    phys = page_number_to_address(page);

    //set up a temporary manual mapping into kernel memory
    L1_index = get_L1_index((void*)MMU_TEMP_MAP);
    L2_index = get_L2_index((void*)MMU_TEMP_MAP);

    L1_table = (struct mmu_L1_4k_desc *)get_page_table_addr();
    fill_L1_index(L1_table, L1_index, MMU_L1_COURSE_TYPE, 0, 0, (void*)MMU_TEMP_L2_ADDR);
    
    L2_table = (struct mmu_L2_4k_desc *)(L1_table[L1_index].L2_base_addr << 10);
    fill_L2_index(L2_table, L2_index, MMU_L2_SMALL_TYPE, 0, 0, MMU_AP_USER_NONE, phys);

    //write the data out to the page
    entries = PAGE_SIZE / data_size;
    for(i = 0; i < entries; ++i)
        for(j = 0; j < data_size; ++j)
            ((char *)MMU_TEMP_MAP)[data_size*i+j] = ((char *)data)[j];

    //remove the manual mapping from the mmu (done here!)
    L2_table[L2_index].type = MMU_L2_NOMAP_TYPE;
    L1_table[L1_index].type = MMU_L1_NOMAP_TYPE;
}

/**
 * Fills in a page with an empty L1 table
 *
 * @param page the page number to fill in
 */
void fill_page_with_empty_L1_table(unsigned int page) {
    struct mmu_L1_4k_desc temp;
    void * addr;
    
    temp.type = MMU_L1_NOMAP_TYPE;
    temp.SBZ = 0;
    temp.domain = 0;
    temp.IMP = 0;
    temp.L2_base_addr = 0;

    addr = get_phys_addr(&temp);

    fill_page_with_data(page, addr, sizeof(struct mmu_L1_4k_desc));
}

/**
 * Fills in a page with an empty L2 table
 *
 * @param page the page number to fill in
 */
void fill_page_with_empty_L2_table(unsigned int page) {
    struct mmu_L2_4k_desc temp;
    void * addr;

    temp.type = MMU_L2_NOMAP_TYPE;
    temp.bufferable = 0;
    temp.cacheable = 0;
    temp.ap0 = temp.ap1 = temp.ap2 = temp.ap3 = MMU_AP_USER_NONE;
    temp.phy_base_addr = 0;

    addr = get_phys_addr(&temp);

    fill_page_with_data(page, addr, sizeof(struct mmu_L2_4k_desc));
}

/**
 * Creates a mapping from a virtual address to a virtual one in the current page table
 *
 * @param phys the physical address to map to
 * @param virt the virtual address to map from
 */
void create_map(void * phys, void * virt, unsigned int ap) {
    void * virt_temp; //temporary virt address to check L1 table for entries
    unsigned int index; //index into a table
    struct mmu_L1_4k_desc * L1_table;
    unsigned int new_page;
    unsigned int i;

    //virt_temp is virt address bounded to the next lowest 4MB boundry
    virt_temp = (void *)((unsigned int)virt & 0xFFC00000);

    //get the index into the L1 table the virt_temp address is
    index = ((unsigned int)virt_temp & 0xFFF00000) >> 20;

    //get the L1 table addr
    L1_table = (struct mmu_L1_4k_desc *)get_page_table_addr();

    //check for the mapping at the 4M boundry
    if(L1_table[index].type == MMU_L1_NOMAP_TYPE) {
        //allocate a page and give it to the next 4MB mapping of L1 tables
        new_page = alloc_page();
        
        //loop through the 4 L1 entries and add the L2 tables
        for(i = 0; i < 4; ++i) {
            L1_table[index+i].type = MMU_L1_COURSE_TYPE;
            L1_table[index+i].L2_base_addr = (new_page << PAGE_SHIFT) + (i << 10);
        }

        //fill in the new page with blank L2 entries
        fill_page_with_empty_L2_table(new_page);
    }

    //
}

/**
 * Removes a mapping from virtual to phsyical addressing
 *
 * @param virt the virtual address to remove
 */
void remove_map(void * virt) {
}

/**
 * Gets the physical address from the virtual address
 *
 * @param virt the virtual address to get the mapping for
 *
 * @return the physical address that it maps to or SYSERR if it does not exist
 */
void * get_phys_addr(void * virt) {
    struct mmu_L1_4k_desc * L1_table;
    struct mmu_L1_4k_desc * L1_desc;
    struct mmu_L2_4k_desc * L2_table;
    struct mmu_L2_4k_desc * L2_desc;

    //if the mmu is disabled, return the address as it will be treated as physical
    if(!mmu_is_enabled())
        return virt;

    //get the base of the page table
    L1_table = (struct mmu_L1_4k_desc *)get_page_table_addr();

    //get the L1 descriptor
    L1_desc = &(L1_table[((unsigned int)virt & 0xFFF00000) >> 20]);

    //check L1 description to make sure it is valid
    if(L1_desc->type == MMU_L1_NOMAP_TYPE)
        return SYSERR;

    //get the L2 table
    L2_table = (struct mmu_L2_4k_desc *)(L1_desc->L2_base_addr << 10);

    //get the L2 descriptor
    L2_desc = &(L2_table[((unsigned int)virt & 0x000FF000) >> 12]);

    //check L2 description to make sure it is valid
    if(L2_desc->type == MMU_L2_NOMAP_TYPE)
        return SYSERR;

    //return the physical address
    return (void *)((L2_desc->phy_base_addr << 12) | ((unsigned int)virt & 0x00000FFF));
}
