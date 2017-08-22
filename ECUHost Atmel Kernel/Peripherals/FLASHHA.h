/**
 * \file
 *
 * \brief Embedded Flash service for SAM.
 *
 * Copyright (c) 2011-2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

 #define FLASH_RC_OK      0
 #define FLASH_RC_YES     1
 #define FLASH_RC_NO      2
 #define FLASH_RC_INVALID 4
 #define FLASH_RC_ERROR   8


/**
 * \brief Initialize the flash service.
 *
 * \param ul_mode FLASH_ACCESS_MODE_128 or FLASH_ACCESS_MODE_64.
 * \param ul_fws The number of wait states in cycle (no shift).
 *
 * \return 0 if successful; otherwise returns an error code.
 */
uint32_t flash_init(uint32_t, uint32_t);


/**
 * \brief Set flash wait state.
 *
 * \param ul_address Flash bank start address.
 * \param ul_fws The number of wait states in cycle (no shift).
 *
 * \return 0 if successful; otherwise returns an error code.
 */
uint32_t flash_set_wait_state(uint32_t, uint32_t);


/**
 * \brief Set flash wait state.
 *
 * \param ul_address Flash bank start address.
 * \param ul_fws The number of wait states in cycle (no shift).
 *
 * \return 0 if successful; otherwise returns an error code.
 */
uint32_t flash_set_wait_state_adaptively(uint32_t);


/**
 * \brief Get flash wait state.
 *
 * \param ul_address Flash bank start address.
 *
 * \return The number of wait states in cycle (no shift).
 */
uint32_t flash_get_wait_state(uint32_t);


/**
 * \brief Get flash descriptor.
 *
 * \param ul_address Flash bank start address.
 * \param pul_flash_descriptor Pointer to a data buffer to store flash descriptor.
 * \param ul_size Data buffer size in DWORD.
 *
 * \return The actual descriptor length.
 */
uint32_t flash_get_descriptor(uint32_t,	uint32_t*, uint32_t);


/**
 * \brief Get flash total page count for the specified bank.
 *
 * \note The flash descriptor must be fetched from flash_get_descriptor
 * function first.
 *
 * \param pul_flash_descriptor Pointer to a flash descriptor.
 *
 * \return The flash total page count.
 */
uint32_t flash_get_page_count(const uint32_t*);


/**
 * \brief Get flash page count per region (plane) for the specified bank.
 *
 * \note The flash descriptor must be fetched from flash_get_descriptor
 * function first.
 *
 * \param pul_flash_descriptor Pointer to a flash descriptor.
 *
 * \return The flash page count per region (plane).
 */
uint32_t flash_get_page_count_per_region(const uint32_t*);


/**
 * \brief Get flash region (plane) count for the specified bank.
 *
 * \note The flash descriptor must be fetched from flash_get_descriptor
 * function first.
 *
 * \param pul_flash_descriptor Pointer to a flash descriptor.
 *
 * \return The flash region (plane) count.
 */
uint32_t flash_get_region_count(const uint32_t*);


/**
 * \brief Erase the entire flash.
 *
 * \note Only the flash bank including ul_address will be erased. If there are
 * two flash banks, we need to call this function twice with each bank start
 * address.
 *
 * \param ul_address  Flash bank start address.
 *
 * \return 0 if successful; otherwise returns an error code.
 */
uint32_t flash_erase_all(uint32_t);


/**
 * \brief Write a data buffer on flash.
 *
 * \note This function works in polling mode, and thus only returns when the
 * data has been effectively written.
 * \note For dual bank flash, this function doesn't support cross write from
 * bank 0 to bank 1. In this case, flash_write must be called twice (ie for
 * each bank).
 *
 * \param ul_address Write address.
 * \param p_buffer Data buffer.
 * \param ul_size Size of data buffer in bytes.
 * \param ul_erase_flag Flag to set if erase first.
 *
 * \return 0 if successful, otherwise returns an error code.
 */
uint32_t flash_write(uint32_t, const void*,	uint32_t, uint32_t);


/**
 * \brief Lock all the regions in the given address range. The actual lock
 * range is reported through two output parameters.
 *
 * \param ul_start Start address of lock range.
 * \param ul_end End address of lock range.
 * \param pul_actual_start Start address of the actual lock range (optional).
 * \param pul_actual_end End address of the actual lock range (optional).
 *
 * \return 0 if successful, otherwise returns an error code.
 */
uint32_t flash_lock(uint32_t, uint32_t, uint32_t*, uint32_t*);


/**
 * \brief Unlock all the regions in the given address range. The actual unlock
 * range is reported through two output parameters.
 *
 * \param ul_start Start address of unlock range.
 * \param ul_end End address of unlock range.
 * \param pul_actual_start Start address of the actual unlock range (optional).
 * \param pul_actual_end End address of the actual unlock range (optional).
 *
 * \return 0 if successful, otherwise returns an error code.
 */
uint32_t flash_unlock(uint32_t, uint32_t, uint32_t*, uint32_t*);


/**
 * \brief Get the number of locked regions inside the given address range.
 *
 * \param ul_start Start address of range
 * \param ul_end End address of range.
 *
 * \return The number of locked regions inside the given address range.
 */
uint32_t flash_is_locked(uint32_t, uint32_t);


/**
 * \brief Set the given GPNVM bit.
 *
 * \param ul_gpnvm GPNVM bit index.
 *
 * \return 0 if successful; otherwise returns an error code.
 */
uint32_t flash_set_gpnvm(uint32_t);


/**
 * \brief Clear the given GPNVM bit.
 *
 * \param ul_gpnvm GPNVM bit index.
 *
 * \return 0 if successful; otherwise returns an error code.
 */
uint32_t flash_clear_gpnvm(uint32_t);


/**
 * \brief Check if the given GPNVM bit is set or not.
 *
 * \param ul_gpnvm GPNVM bit index.
 *
 * \retval 1 If the given GPNVM bit is currently set.
 * \retval 0 If the given GPNVM bit is currently cleared.
 */
uint32_t flash_is_gpnvm_set(uint32_t);


/**
 * \brief Set security bit.
 *
 * \return 0 if successful; otherwise returns an error code.
 */
uint32_t flash_enable_security_bit(void);


/**
 * \brief Check if the security bit is set or not.
 *
 * \retval 1 If the security bit is currently set.
 * \retval 0 If the security bit is currently cleared.
 */
uint32_t flash_is_security_bit_enabled(void);


/**
 * \brief Read the flash unique ID.
 *
 * \param pul_data Pointer to a data buffer to store 128-bit unique ID.
 * \param ul_size Data buffer size in DWORD.
 *
 * \return 0 if successful; otherwise returns an error code.
 */
uint32_t flash_read_unique_id(uint32_t*, uint32_t);
