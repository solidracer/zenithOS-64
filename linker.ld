ENTRY(_start)

SECTIONS
{
	. = 2M;

    __start = .;

    .boot : 
    {
        *(.boot)
        *(.boot.text)
        *(.boot.rodata)
    }

    .boot.data ALIGN(4K) : 
    {
        *(.boot.data)
        *(.boot.bss)
    }

    __physend = .;

    . += 0xFFFFFFFF80000000;

	.text ALIGN(4K) : AT(ADDR(.text) - 0xFFFFFFFF80000000)
	{
		*(.text*)
	}

	/* Read-only data. */
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xFFFFFFFF80000000)
	{
		*(.rodata*)
	}

	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT(ADDR(.data) - 0xFFFFFFFF80000000)
	{
		*(.data*)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xFFFFFFFF80000000)
	{
		*(COMMON)
		. = ALIGN(16);
		*(.bss*)
	}

    __end = . - 0xFFFFFFFF80000000;

	/DISCARD/ : {
		*(.comment*)
		*(.note*)
		*(.eh_frame*)
	}
}
