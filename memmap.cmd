--heap_size=0x1000
--stack_size=0x400

MEMORY
{
    L2RAM     : origin = 0x00800000, length = 0x00040000   /* 256KB internal RAM */
    DDR       : origin = 0x80000000, length = 0x08000000   /* Example external DDR */
}

SECTIONS
{
    .text     > L2RAM
    .const    > L2RAM
    .fardata  > L2RAM
    .cinit    > L2RAM
    .stack    > L2RAM
    .sysmem   > L2RAM
    .far      > L2RAM
    .switch   > L2RAM
    .cio      > L2RAM
    .bss      > L2RAM
    .data     > L2RAM
    .neardata > L2RAM
    .rodata   > L2RAM
}
