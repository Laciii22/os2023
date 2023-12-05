#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

void pci_init() {
    // Remove the unused typedef
    // typedef uint64 pagetable_t;

    // Replace magic constants with meaningful names
    const uint64 e1000_regs = 0x40000000L;
    uint32 *ecam = (uint32 *)0x30000000L;

    // Configure PCI devices
    for (int dev = 0; dev < 32; dev++) {
        int bus = 0;
        int func = 0;
        int offset = 0;
        uint32 off = (bus << 16) | (dev << 11) | (func << 8) | (offset);
        volatile uint32 *base = ecam + off;
        uint32 id = base[0];

        // Check if the device is an e1000
        if (id == 0x100e8086) {
            // Command and status register.
            // Bit 0: I/O access enable, Bit 1: Memory access enable, Bit 2: Enable mastering
            base[1] = 7;
            __sync_synchronize();

            // Reset the BARs to get their sizes
            for (int i = 0; i < 6; i++) {
                uint32 old = base[4 + i];
                base[4 + i] = 0xffffffff;
                __sync_synchronize();
                base[4 + i] = old;
            }

            // Tell the e1000 to reveal its registers at physical address 0x40000000
            base[4 + 0] = e1000_regs;

            // Initialize the e1000
            e1000_init((uint32 *)e1000_regs);
        }
    }
}

