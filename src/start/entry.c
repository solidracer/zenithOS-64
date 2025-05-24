#include <zenithBoot.h>
#include "../def.h"
#include "../io.h"
#include "../memutil.h"

extern zenith_memory_map_t memmap;
extern zenith_boot_info_t info;

#define TOPAGES(size) (((size) + 0xFFF) >> 12)

static uint64_t *early_pml4;

static struct {
    uint64_t next;
    efi_memory_descr_t *descr;
    uint32_t numpage;
} pmm = {0};

static void find_region(void) {
    for (uint32_t i = 0;i<memmap.entries;i++) {
        efi_memory_descr_t *desc = (efi_memory_descr_t*)(TOADDR(memmap.map) + i * memmap.desc_size);
        if (desc->type == 7 && desc->numpage > pmm.numpage) {
            pmm.numpage = desc->numpage;
            pmm.next = desc->physaddr;
            pmm.descr = desc;
        }
    }
}

static void *allocate_page(void) {
    if (pmm.numpage <= 0)
        return NULL;
    void *ptr = TOPTR(pmm.next);
    pmm.next += 0x1000;
    pmm.numpage--;
    memset(ptr, 0, 0x1000);
    return ptr;
}

static void map_page(uint64_t virt_addr, uint64_t phys_addr, uint64_t flags) {
    uint64_t pml4_idx = (virt_addr >> 39) & 0x1FF;
    uint64_t pdpt_idx = (virt_addr >> 30) & 0x1FF;
    uint64_t pd_idx   = (virt_addr >> 21) & 0x1FF;
    uint64_t pt_idx   = (virt_addr >> 12) & 0x1FF;

    uint64_t *pdpt;
    if (!(early_pml4[pml4_idx] & 0x1)) {
        pdpt = allocate_page();
        early_pml4[pml4_idx] = ((uint64_t)pdpt) | flags | 0x1;
    } else {
        pdpt = (uint64_t *)(early_pml4[pml4_idx] & ~0xFFFULL);
    }

    uint64_t *pd;
    if (!(pdpt[pdpt_idx] & 0x1)) {
        pd = allocate_page();
        pdpt[pdpt_idx] = ((uint64_t)pd) | flags | 0x1;
    } else {
        pd = (uint64_t *)(pdpt[pdpt_idx] & ~0xFFFULL);
    }

    uint64_t *pt;
    if (!(pd[pd_idx] & 0x1)) {
        pt = allocate_page();
        pd[pd_idx] = ((uint64_t)pt) | flags | 0x1;
    } else {
        pt = (uint64_t *)(pd[pd_idx] & ~0xFFFULL);
    }

    pt[pt_idx] = (phys_addr & ~0xFFFULL) | flags | 0x1;
}

static void map_region(uint64_t virtaddr, uint64_t physaddr, uint64_t size, uint64_t flags) {
    size = TOPAGES(size);
    for (uint64_t i = 0;i<size;i++)
        map_page(virtaddr + i * 0x1000, physaddr + i * 0x1000, flags);
}

extern char __end[], __start[], __physend[];

extern char high_entry[];

void low_entry(void) {
    find_region();

    early_pml4 = allocate_page();

    map_region(0xFFFFFFFF80000000 + TOADDR(__start), TOADDR(__start), TOADDR(__end) - TOADDR(__start), 0x2);

    map_region(TOADDR(__start), TOADDR(__start), TOADDR(__physend) - TOADDR(__start), 0x2);

    map_region(ALIGND(TOADDR(memmap.map), 0x1000), ALIGND(TOADDR(memmap.map), 0x1000), memmap.entries * memmap.desc_size, 0x2);

    uint64_t fbsiz = info.height * info.ppsl * sizeof(uint32_t);

    map_region(TOADDR(info.fb), TOADDR(info.fb), fbsiz, 0x12);

    asm volatile("mov %0, %%cr3" :: "r"(early_pml4) : "memory");

    /* start the kernel */
    asm volatile("jmp *%0" :: "r"(high_entry));
}
