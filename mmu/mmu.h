#ifndef MMU_H
#define MMU_H

//#define MMU_ENABLE
#ifdef MMU_ENABLE
    #ifdef IO_ADDR
        #undef IO_ADDR
    #endif
    #define IO_ADDR 0x3F000000
#endif

#define PAGE_SIZE           4096 //size of the pages to be used
#define PAGE_SHIFT          12   //number of bits to shift for pages

#define MMU_L1_ALIGN        0x4000 //16k alignment
#define MMU_L2_ALIGN        0x0400 //1K  alignment

#define PAT_ADDRESS         0x00009000 //page allocation table address
#define MMU_TEMP_MAP        0x3E000000 //when mapping a temporary page, use this address
#define MMU_TEMP_L2_ADDR    0x0000A000 //temporary L2 table address

#define MMU_L1_NOMAP_TYPE   0x0 //a none mapping to the second level of tables
#define MMU_L1_COURSE_TYPE  0x1 //mapping to a course second level of tables

#define MMU_L2_NOMAP_TYPE   0x0 //no mapping to physical memory
#define MMU_L2_SMALL_TYPE   0x2 //mapping to small pages (4k)

#define MMU_AP_USER_NONE    0x1 //no user access permission
#define MMU_AP_USER_READ    0x2 //read only user access permission
#define MMU_AP_USER_RW      0x3 //read/write user access permission

struct mmu_L1_4k_desc {
    unsigned int type           : 2 ; //should always be 0b01
    unsigned int SBZ            : 3 ; //should always be 0

    //need to set domain 0 to 0b01 to check the L2 table access permissions
    unsigned int domain         : 4 ; //should always be 0

    unsigned int IMP            : 1 ; //should always be 0
    unsigned int L2_base_addr   : 22; //level 2 table base address
}; //should be 32 bits in size

struct mmu_L2_4k_desc {
    unsigned int type           : 2 ; //should always be 0b10
    unsigned int bufferable     : 1 ;
    unsigned int cacheable      : 1 ;

    //in practice, these will all be set to the same permissions
    unsigned int ap0            : 2 ; //access permission for first 1k
    unsigned int ap1            : 2 ; //access permission for second 1k
    unsigned int ap2            : 2 ; //access permission for third 1k
    unsigned int ap3            : 2 ; //access permission for fourth 1k

    unsigned int phy_base_addr  : 20; //physical base address
}; //should be 32 bits in size

/* thoughts on the virtual memory setup thus far
 - Lower 1GB Virtual Paging (0x00000000 - 0x3FFFFFFF) is kernel mapping
 - Upper 3GB Virtual Pagins (0x40000000 - 0xFFFFFFFF) is user mapping

 - First 16MB (0x00000000 - 0x01000000) is identity mapped and is allocated to the kernel (1/8 of memory)
 - Kernel should start at 64K (0x00010000)
 - First 4 Pages (0x00000000 - 0x00003FFF) in memory is the Kernel L1 table
 - Next 4 pages (0x00004000 - 0x00007FFF) in memory is the Kernel L2 table for 16MB mapping of kernel memory
 - Next Page (0x00008000 - 0x00008FFF) in memory is the kernel L2 table for the i/o mapping
 - Next page (0x00009000 - 0x00009FFF) in memory is the page allocation table
 - Next page (0x0000A000 - 0x0000AFFF) in memory is for temporary L2 table mapping
 - Next 5 pages (0x0000B000 - 0x0000FFFF) in memory are completly free (20K)

 - I/O is changed to occupy just under user space (0x3F000000 - 0x3FFFFFFF)
   - getting mapped from 0x20000000 - 0x20FFFFFF

 - 
*/

//memory page routines
void set_page_alloc(unsigned int page); //sets the bit in PAT for page as used
void unset_page_alloc(unsigned int page); //sets the bit in PAT for the page as free
unsigned int alloc_page(void); //allocates a single page in the page allocation table
unsigned int alloc_page_align(unsigned int align); //allocates a page that is aligned to a certain number of pages
unsigned int alloc_pages(unsigned int amount); //allocates a number of contiguous pages
unsigned int alloc_pages_align(unsigned int amount, unsigned int align); //allocate number of continuous aigned pages
void dealloc_page(unsigned int page); //frees a page to use
void dealloc_pages(unsigned int page, unsigned int amount); //frees block of pages to use
void * page_number_to_address(unsigned int page); //gets the physical memory address from the page number

//mmu mapping routines
unsigned int get_L1_index(void * addr); //gets the L1 index from a virtual memory address
unsigned int get_L2_index(void * addr); //gets the L2 index from a virtual memory address
struct mmu_L1_4k_desc * get_L1_desc(void * addr); //gets the L1 description for the memory address
struct mmu_L2_4k_desc * get_L2_desc(void * addr); //gets the L2 description for the memory address
void fill_L1_index(struct mmu_L1_4k_desc * table, unsigned int index, unsigned int type, unsigned int domain,
                   unsigned int IMP, void * base_addr); //fills in an L1 description
void fill_L2_index(struct mmu_L2_4k_desc * table, unsigned int index, unsigned int type, unsigned int bufferable,
                   unsigned int cacheable, unsigned int ap, void * base_addr); //fills in an L2 description
void fill_page_with_data(unsigned int page, void * data, unsigned int data_size); //fills page with repeating data
void fill_page_with_empty_L1_table(unsigned int page); //fills a page with empty L1 table
void fill_page_with_empty_L2_table(unsigned int page); //fills a page with empty L2 table
void create_map(void * phys, void * virt, unsigned int ap); //creates a mapping from virt->phys address
void remove_map(void * virt); //removes the virtual address mapping
void * get_phys_addr(void * virt); //does a software page walk to get the physcial address from a virtual one

//mmu routines
void * get_page_table_addr(void); //gets the addr of the currently used page table
void * get_page_table_addr_phys(void); //gets the physical address of the current page table
void set_page_table_addr(void * table); //sets the addr of the page table to use
void mmu_init(void); //initializes the data for the mmu table
void mmu_enable(void); //enables the mmu
void mmu_disable(void); //disables the mmu
unsigned int mmu_is_enabled(void); //determines if the mmu is enabled or not

#endif
