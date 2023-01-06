# Arm-Bit-Banding

Implementation the ARM bit-banding mechanism which exist in Cortex-M3 and Cortex-M4 CPUs.

Bit-banding maps a complete word of memory onto a single bit in the bit-band region. For example, writing to one of the alias words sets or clears the corresponding bit in
the bit-band region. This enables every individual bit in the bit-banding region to be directly accessible from a word-aligned address using a single LDR instruction. It also
enables individual bits to be toggled without performing a read-modify-write sequence of instructions.

The processor memory map includes two bit-band regions. These occupy the lowest 1MB of the SRAM and Peripheral memory regions respectively. These bit-band regions map each word in
an alias region of memory to a bit in a bit-band region of memory

## Warning

> In the current moment implement work only with SRAM bit-banding region.

## Installation

For installation you need to do 2 simply steps:

- Copy necessary `arm_bit_banding.h` file to your project
- Include `arm_bit_banding.h` into your source code.

Also this library requered definition of `SRAM1_BB_BASE` - SRAM base address in the bit-band region and `SRAM1_BASE` - SRAM base address in the alias region. You need define it manually or include special header files provided by vendor. For example `stm32f407xx.h` from STM.

## Usage

Current library define special data type `flag_t` for work with bit-banding memory region.
This is was done because I like ***"Explicit is better than implicit"*** principle and want that when you read source code it was clear that something unusual happens there and you need to be aware.

The `flag_t` type is implementing of ARM bit-banding mechanism. This data type  store the alias address calculated for corresponding bit in the bit-band region.

So the first step what you need to do if you want use this library is create `flag_t` variable:

```c
uint8_t state = 0;
flag_t error_flag = bb_make_flag(&state, 3);
```

> As flag variable you can use only `uin8_t` variables!

And after this you can use this flag variable for atomic bit manipulation:

```c
//Set bit
bb_set(error_flag);

//Clear bit
bb_clear(error_flag);

//Read bit
bool is_error = bb_tst(error_flag);
```
