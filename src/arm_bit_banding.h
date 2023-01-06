/**
 * \file
 * \author  Aleksander Kovalchuk (aliaksander.kavalchuk@gmail.com)
 * \date    2023-01-02
 * \brief   Implementation the ARM bit-banding mechanism which exist in Cortex-M3
 *          and Cortex-M4 CPUs.
 *
 *          Bit-banding maps a complete word of memory onto a single bit in the bit-band region.
 *          For example, writing to one of the alias words sets or clears the corresponding bit in
 *          the bit-band region. This enables every individual bit in the bit-banding region to be
 *          directly accessible from a word-aligned address using a single LDR instruction. It also
 *          enables individual bits to be toggled without performing a read-modify-write sequence of
 *          instructions.
 *
 *          The processor memory map includes two bit-band regions. These occupy the lowest 1MB of the
 *          SRAM and Peripheral memory regions respectively. These bit-band regions map each word in
 *          an alias region of memory to a bit in a bit-band region of memory
 */

#pragma once

//_____ I N C L U D E S _______________________________________________________
#include <stdbool.h>
#include <stdint.h>

//_____ C O N F I G S  ________________________________________________________
/*! \cond PRIVATE */
#if !defined(SRAM1_BB_BASE)
    #error "The SRAM1 base address in the alis region undefined. Please define\
            SRAM1_BB_BASE address!"
#endif

#if !defined(SRAM1_BASE)
    #error "The SRAM1 base address in the bit-band region undefined. Please define\
            SRAM1_BASE address!"
#endif
/*! \endcond */
//_____ D E F I N I T I O N S _________________________________________________
/**
 * \brief Special data type for atomic bit operations by using ARM bit-banding
 *        mechanism in SRAM memory region.
 */
typedef volatile uint32_t* flag_t;

/// Invalid value for `flag_t` data type
#define INVALID_FLAG (flag_t)NULL
//_____ M A C R O S ___________________________________________________________
/**
 * \brief   Calculation of position of the target bit in the bit-band SRAM memory region
 *
 * \param[in]   byte_offset: number of the byte in the bit-band region that
 *                          contains the targeted bit
 * \param[in]   bit_num: bit position (0-7) of the targeted bit
 */
#define BIT_WORD_OFFSET(byte_offset, bit_num) (((byte_offset)*32) + ((bit_num)*4))

/**
 * \brief Calculation of address of the word in the alias memory region that maps to
 *        the targeted bit.
 *
 * \param[in]   byte_offset: number of the byte in the bit-band region that
 *                           contains the targeted bit
 * \param[in]   bit_num: bit position (0-7) of the targeted bit
 */
#define BIT_WORD_ADDR(byte_offset, bit_num) (flag_t)(((SRAM1_BB_BASE) + BIT_WORD_OFFSET(byte_offset, bit_num)))

/**
 * \brief Calculation byte offset from SRAM bit-band region base for given address.
 *
 * \param[in]   ADDR: address of selected store flags variable.
 */
#define BYTE_OFFSET(ADDR) (((uint32_t)(ADDR) - (uint32_t)(SRAM1_BASE)))

/**
 * \brief Calculation of address of the word in the alias SRAM memory region that maps to
 *        the targeted bit for given variable
 *
 * \param[in]   ADDR: address of selected store flags variable.
 * \param[in]   BIT: bit position in selected flags variable.
 */
#define MAKE_FLAG(ADDR, BIT) (flag_t)(BIT_WORD_ADDR(BYTE_OFFSET(ADDR), (BIT)))

//_____ V A R I A B L E S _____________________________________________________

//_____ P U B L I C  F U N C T I O N S_________________________________________
/**
 * \brief Create special data type for atomic bit operations for SRAM memory.
 *
 * The `flag_t` type is implementing of ARM bit-banding mechanism. This function
 * is calculates the alias address for corresponding bit in the bit-band
 * region.
 *
 * \note For more information please see paragraph 3.7 in Cortex-M3/4 Technical
 *       Reference Manual
 *
 * \warning As flag variable you can use only `uin8_t` variables!
 *
 *\code
 * uint8_t state = 0;
 * flag_t error_flag = abb_make(&state, 3);
 *\endcode
 *
 * \param[in]   addr: address of selected store flags variable.
 * \param[in]   bit: bit number in this variable
 *
 * \return special type `flag_t` for bit-banding flags.
 */
static inline flag_t abb_make(volatile uint8_t* addr, uint8_t bit)
{
    return (bit < 8) ? MAKE_FLAG(addr, bit) : INVALID_FLAG;
}

/**
 * \brief Set flag using ARM bit-banding mechanism.
 *
 * \param[in] flag: selected flag.
 *
 * \return none
 */
static inline void abb_set(flag_t flag)
{
    *flag = 1;
}

/**
 * \brief Clear flag using ARM bit-banding mechanism.
 *
 * \param[in] flag: selected flag.
 *
 * \return none
 */
static inline void abb_clear(flag_t flag)
{
    *flag = 0;
}

/**
 * \brief Read flag using ARM bit-banding mechanism.
 *
 * \param[in] flag: selected flag.
 *
 * \return true if flag set
 *         false if flag clear
 */
static inline bool abb_tst(const flag_t flag)
{
    return *flag == 1;
}