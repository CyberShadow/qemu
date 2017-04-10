/*
 * STi7105 board description
 *
 * Copyright (c) 2017 Vladimir Panteleev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
/*
   Shix 2.0 board by Alexis Polti, described at
   https://web.archive.org/web/20070917001736/perso.enst.fr/~polti/realisations/shix20

   More information in target/sh4/README.sh4
*/
#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu-common.h"
#include "cpu.h"
#include "hw/hw.h"
#include "hw/sh4/sh.h"
#include "sysemu/sysemu.h"
#include "sysemu/qtest.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "exec/address-spaces.h"
#include "qemu/error-report.h"

static void sti7105_init(MachineState *machine)
{
    MemoryRegion *sysmem = get_system_memory();
    MemoryRegion *emi = g_new(MemoryRegion, 1);
    MemoryRegion *lmi = g_new(MemoryRegion, 1);
    
    /* Allocate memory space */
    memory_region_init_ram(emi, NULL, "sti7105.emi", 0x08000000, &error_fatal);
    vmstate_register_ram_global(emi);
    memory_region_add_subregion(sysmem, 0x00000000, emi);

    memory_region_init_ram(lmi, NULL, "sti7105.lmi", 0x40000000, &error_fatal);
    vmstate_register_ram_global(lmi);
    memory_region_add_subregion(sysmem, 0x40000000, lmi);

    if (machine->firmware) {
        int fw_size = load_image_targphys(machine->firmware,
            0x00000000, 0x08000000);
        if (fw_size == -1) {
            error_printf("unable to load firmware image '%s'\n",
                machine->firmware);
            exit(1);
        }
        fprintf(stderr, "Firmware loaded (%d bytes)\n", fw_size);
    }

    const char *cpu_model = machine->cpu_model;
    SuperHCPU *cpu;

    if (!cpu_model)
        cpu_model = "any";

    cpu = cpu_sh4_init(cpu_model);
    if (cpu == NULL) {
        fprintf(stderr, "Unable to find CPU definition\n");
        exit(1);
    }

    cpu->env.pc = 0;
}

static void sti7105_machine_init(MachineClass *mc)
{
    mc->desc = "STi7105";
    mc->init = sti7105_init;
}

DEFINE_MACHINE("STi7105", sti7105_machine_init)
