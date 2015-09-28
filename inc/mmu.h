#ifndef JOS_INC_MMU_H
#define JOS_INC_MMU_H

/*
 * This file defines the definitions for the Cortex-A7 MMU
 */


/*
 *
 *	Part 1.  Paging data structures and constants.
 *
 */

// A linear address 'la' has a three-part structure as follows:
//
// +--------12------+-------8-------+---------12----------+
// | Page Directory |   Page Table  | Offset within Page  |
// |      Index     |      Index    |                     |
// +----------------+---------------+---------------------+
//  \--- PDX(la) --/ \-- PTX(la) --/ \---- PGOFF(la) ----/
//  \---------- PGNUM(la) ---------/
//
// The PDX, PTX, PGOFF, and PGNUM macros decompose linear addresses as shown.
// To construct a linear address la from PDX(la), PTX(la), and PGOFF(la),
// use PGADDR(PDX(la), PTX(la), PGOFF(la)).

// page number field of address
#define PGNUM(la)	(((uintptr_t) (la)) >> PTXSHIFT)

// page directory index
#define PDX(la)		((((uintptr_t) (la)) >> PDXSHIFT) & 0xFFF)

// page table index
#define PTX(la)		((((uintptr_t) (la)) >> PTXSHIFT) & 0x0FF)

// offset in page
#define PGOFF(la)	(((uintptr_t) (la)) & 0xFFF)

// construct linear address from indexes and offset
#define PGADDR(d, t, o)	((void*) ((d) << PDXSHIFT | (t) << PTXSHIFT | (o)))

// Page directory and page table constants.
#define NPDENTRIES	4096		// page directory entries per page directory
#define NPTENTRIES	256		// page table entries per page table

#define PGSIZE		4096		// bytes mapped by a page
#define PGSHIFT		12		// log2(PGSIZE)

#define PTSIZE		(PGSIZE*NPTENTRIES) // bytes mapped by a page directory entry
#define PTSHIFT		20		// log2(PTSIZE)
#define PTSTRIDE        (NPTENTRIES*sizeof(pte_t))

#define PTXSHIFT	12		// offset of PTX in a linear address
#define PDXSHIFT	20		// offset of PDX in a linear address

#define PGALIGN         PGSIZE
#define PTALIGN         (NPDENTRIES * sizeof(uint32_t))

// Page table/directory entry flags.
#define PDE_P           0x001   // Present
#define PDE_PT          PDE_P
#define PDE_SEC         0x002   // Section

#define PTE_P		0x002	// Present
#define PTE_W		0x020	// Writeable
#define PTE_SMALLPG     0x002
#define PTE_LARGEPG     0x001
#define PTE_PWT		0x008	// Write-Through
#define PTE_PCD		0x010	// Cache-Disable

// PTE AP permission bits, there are two different formats
// AP[2:0] format
#define PTE_NW		0x200	// Not Writeable
#define PTE_U		0x020	// User
#define PTE_U_LIMIT     0x010   
// AP[2:1] format
#define PTE_3BIT_NW     0x200   // Writeable
#define PTE_3BIT_U      0x020   // User
#define PTE_A		0x010	// Accessed

#define PTE_D		0x040	// Dirty
#define PTE_PS		0x080	// Page Size
#define PTE_G		0x100	// Global

// The PTE_AVAIL bits aren't used by the kernel or interpreted by the
// hardware, so user processes are allowed to set them arbitrarily.
#define PTE_AVAIL	0xE00	// Available for software use

// Flags in PTE_SYSCALL may be used in system calls.  (Others may not.)
#define PTE_SYSCALL	(PTE_AVAIL | PTE_P | PTE_W | PTE_U)

// Address in page table and page directory entry
#define PTE_ADDR(pte)	((physaddr_t) (pte) & ~0xFFF)
#define PDE_ADDR(pde)   ((physaddr_t) (pde) & ~0x3FF)

// Control Register flags
#define C1_MMU          0x001 // enable / disable MMU
#define C1_DCACHE       0x004
#define C1_ICACHE       0x1000
#define C1_AFE          0x20000000 // Use this to enable the access flag

#define C2_TTBCR_N      0x0

#endif /* !JOS_INC_MMU_H */
