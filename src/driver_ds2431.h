/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      driver_ds2431.h
 * @brief     driver ds2431 header file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2025-12-20
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2025/12/20  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#ifndef DRIVER_DS2431_H
#define DRIVER_DS2431_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup ds2431_driver ds2431 driver function
 * @brief    ds2431 driver modules
 * @{
 */

/**
 * @addtogroup ds2431_advance_driver
 * @{
 */

/**
 * @brief ds2431 max search size definition
 */
#ifndef DS2431_MAX_SEARCH_SIZE
    #define DS2431_MAX_SEARCH_SIZE        64        /**< max 64 devices */
#endif

/**
 * @}
 */

/**
 * @addtogroup ds2431_base_driver
 * @{
 */

/**
 * @brief ds2431 mode enumeration definition
 */
typedef enum
{
    DS2431_MODE_SKIP_ROM            = 0x00,        /**< skip rom mode */
    DS2431_MODE_OVERDRIVE_SKIP_ROM  = 0x01,        /**< overdrive skip rom mode */
    DS2431_MODE_MATCH_ROM           = 0x02,        /**< match rom mode */
    DS2431_MODE_OVERDRIVE_MATCH_ROM = 0x03,        /**< overdrive match rom mode */
    DS2431_MODE_RESUME              = 0x04,        /**< resume mode */
    DS2431_MODE_OVERDRIVE_RESUME    = 0x05,        /**< overdrive resume mode */
} ds2431_mode_t;

/**
 * @brief ds2431 type enumeration definition
 */
typedef enum
{
    DS2431_TYPE_MATCH_ROM           = 0x00,        /**< match rom */
    DS2431_TYPE_OVERDRIVE_MATCH_ROM = 0x01,        /**< overdrive match rom */
} ds2431_type_t;

/**
 * @brief ds2431 config enumeration definition
 */
typedef enum
{
    DS2431_CONFIG_EPROM_MODE         = 0xAA,        /**< eprom mode */
    DS2431_CONFIG_WRITE_PROTECT_MODE = 0x55,        /**< write protect mode */
} ds2431_config_t;

/**
 * @brief ds2431 config control structure definition
 */
typedef struct ds2431_config_control_s
{
    uint8_t page0_protection_control;        /**< page0 protection control */
    uint8_t page1_protection_control;        /**< page1 protection control */
    uint8_t page2_protection_control;        /**< page2 protection control */
    uint8_t page3_protection_control;        /**< page3 protection control */
    uint8_t copy_protection;                 /**< copy protection */
    uint8_t factory_byte;                    /**< factory byte */
    uint8_t user_byte_0;                     /**< user byte 0 */
    uint8_t user_byte_1;                     /**< user byte 1 */
} ds2431_config_control_t;

/**
 * @brief ds2431 handle structure definition
 */
typedef struct ds2431_handle_s
{
    uint8_t (*bus_init)(void);                              /**< point to a bus_init function address */
    uint8_t (*bus_deinit)(void);                            /**< point to a bus_deinit function address */
    uint8_t (*bus_read)(uint8_t *value);                    /**< point to a bus_read function address */
    uint8_t (*bus_write)(uint8_t value);                    /**< point to a bus_write function address */
    void (*delay_ms)(uint32_t ms);                          /**< point to a delay_ms function address */
    void (*delay_us)(uint32_t us);                          /**< point to a delay_us function address */
    void (*enable_irq)(void);                               /**< point to an enable_irq function address */
    void (*disable_irq)(void);                              /**< point to a disable_irq function address */
    void (*debug_print)(const char *const fmt, ...);        /**< point to a debug_print function address */
    uint8_t inited;                                         /**< inited flag */
    uint8_t mode;                                           /**< chip mode */
    uint8_t rom[8];                                         /**< chip mode */
} ds2431_handle_t;

/**
 * @brief ds2431 info structure definition
 */
typedef struct ds2431_info_s
{
    char chip_name[32];                /**< chip name */
    char manufacturer_name[32];        /**< manufacturer name */
    char interface[8];                 /**< chip interface name */
    float supply_voltage_min_v;        /**< chip min supply voltage */
    float supply_voltage_max_v;        /**< chip max supply voltage */
    float max_current_ma;              /**< chip max current */
    float temperature_min;             /**< chip min operating temperature */
    float temperature_max;             /**< chip max operating temperature */
    uint32_t driver_version;           /**< driver version */
} ds2431_info_t;

/**
 * @}
 */

/**
 * @defgroup ds2431_link_driver ds2431 link driver function
 * @brief    ds2431 link driver modules
 * @ingroup  ds2431_driver
 * @{
 */

/**
 * @brief     initialize ds2431_handle_t structure
 * @param[in] HANDLE pointer to a ds2431 handle structure
 * @param[in] STRUCTURE ds2431_handle_t
 * @note      none
 */
#define DRIVER_DS2431_LINK_INIT(HANDLE, STRUCTURE)         memset(HANDLE, 0, sizeof(STRUCTURE))

/**
 * @brief     link bus_init function
 * @param[in] HANDLE pointer to a ds2431 handle structure
 * @param[in] FUC pointer to a bus_init function address
 * @note      none
 */
#define DRIVER_DS2431_LINK_BUS_INIT(HANDLE, FUC)           (HANDLE)->bus_init = FUC

/**
 * @brief     link bus_deinit function
 * @param[in] HANDLE pointer to a ds2431 handle structure
 * @param[in] FUC pointer to a bus_deinit function address
 * @note      none
 */
#define DRIVER_DS2431_LINK_BUS_DEINIT(HANDLE, FUC)         (HANDLE)->bus_deinit = FUC

/**
 * @brief     link bus_read function
 * @param[in] HANDLE pointer to a ds2431 handle structure
 * @param[in] FUC pointer to a bus_read function address
 * @note      none
 */
#define DRIVER_DS2431_LINK_BUS_READ(HANDLE, FUC)           (HANDLE)->bus_read = FUC

/**
 * @brief     link bus_write function
 * @param[in] HANDLE pointer to a ds2431 handle structure
 * @param[in] FUC pointer to a bus_write function address
 * @note      none
 */
#define DRIVER_DS2431_LINK_BUS_WRITE(HANDLE, FUC)          (HANDLE)->bus_write = FUC

/**
 * @brief     link delay_ms function
 * @param[in] HANDLE pointer to a ds2431 handle structure
 * @param[in] FUC pointer to a delay_ms function address
 * @note      none
 */
#define DRIVER_DS2431_LINK_DELAY_MS(HANDLE, FUC)           (HANDLE)->delay_ms = FUC

/**
 * @brief     link delay_us function
 * @param[in] HANDLE pointer to a ds2431 handle structure
 * @param[in] FUC pointer to a delay_us function address
 * @note      none
 */
#define DRIVER_DS2431_LINK_DELAY_US(HANDLE, FUC)           (HANDLE)->delay_us = FUC

/**
 * @brief     link enable_irq function
 * @param[in] HANDLE pointer to a ds2431 handle structure
 * @param[in] FUC pointer to an enable_irq function address
 * @note      none
 */
#define DRIVER_DS2431_LINK_ENABLE_IRQ(HANDLE, FUC)         (HANDLE)->enable_irq = FUC

/**
 * @brief     link disable_irq function
 * @param[in] HANDLE pointer to a ds2431 handle structure
 * @param[in] FUC pointer to a disable_irq function address
 * @note      none
 */
#define DRIVER_DS2431_LINK_DISABLE_IRQ(HANDLE, FUC)        (HANDLE)->disable_irq = FUC

/**
 * @brief     link debug_print function
 * @param[in] HANDLE pointer to a ds2431 handle structure
 * @param[in] FUC pointer to a debug_print function address
 * @note      none
 */
#define DRIVER_DS2431_LINK_DEBUG_PRINT(HANDLE, FUC)        (HANDLE)->debug_print = FUC

/**
 * @}
 */

/**
 * @defgroup ds2431_base_driver ds2431 base driver function
 * @brief    ds2431 base driver modules
 * @ingroup  ds2431_driver
 * @{
 */

/**
 * @brief      get chip's information
 * @param[out] *info pointer to a ds2431 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t ds2431_info(ds2431_info_t *info);

/**
 * @brief     initialize the chip
 * @param[in] *handle pointer to a ds2431 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 bus initialization failed
 *            - 2 handle is NULL
 *            - 3 linked functions is NULL
 *            - 4 reset failed
 * @note      none
 */
uint8_t ds2431_init(ds2431_handle_t *handle);

/**
 * @brief     close the chip
 * @param[in] *handle pointer to a ds2431 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 bus deinit failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ds2431_deinit(ds2431_handle_t *handle);

/**
 * @brief     set the chip mode
 * @param[in] *handle pointer to a ds2431 handle structure
 * @param[in] mode chip mode
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ds2431_set_mode(ds2431_handle_t *handle, ds2431_mode_t mode);

/**
 * @brief      get the chip mode
 * @param[in]  *handle pointer to a ds2431 handle structure
 * @param[out] *mode pointer to a chip mode buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ds2431_get_mode(ds2431_handle_t *handle, ds2431_mode_t *mode);

/**
 * @brief     set the handle rom
 * @param[in] *handle pointer to a ds2431 handle structure
 * @param[in] *rom pointer to a rom buffer
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ds2431_set_rom(ds2431_handle_t *handle, uint8_t rom[8]);

/**
 * @brief      get the chip rom
 * @param[in]  *handle pointer to a ds2431 handle structure
 * @param[out] *rom pointer to a rom buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ds2431_get_rom(ds2431_handle_t *handle, uint8_t rom[8]);

/**
 * @brief     run rom match
 * @param[in] *handle pointer to a ds2431 handle structure
 * @param[in] type match type
 * @param[in] *rom pointer to a rom buffer
 * @return    status code
 *            - 0 success
 *            - 1 match failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ds2431_rom_match(ds2431_handle_t *handle, ds2431_type_t type, uint8_t rom[8]);

/**
 * @brief      read data
 * @param[in]  *handle pointer to a ds2431 handle structure
 * @param[in]  address input address
 * @param[out] *data pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 address and len are invalid
 * @note       none
 */
uint8_t ds2431_read(ds2431_handle_t *handle, uint8_t address, uint8_t *data, uint8_t len);

/**
 * @brief     write data
 * @param[in] *handle pointer to a ds2431 handle structure
 * @param[in] address input address
 * @param[in] *data pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 address and len are invalid
 * @note      none
 */
uint8_t ds2431_write(ds2431_handle_t *handle, uint8_t address, uint8_t *data, uint8_t len);

/**
 * @brief     copy scratchpad
 * @param[in] *handle pointer to a ds2431 handle structure
 * @param[in] address input address
 * @return    status code
 *            - 0 success
 *            - 1 copy scratchpad failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 address >= 0x0080
 *            - 5 address is invalid
 * @note      none
 */
uint8_t ds2431_copy_scratchpad(ds2431_handle_t *handle, uint16_t address);

/**
 * @brief      write scratchpad
 * @param[in]  *handle pointer to a ds2431 handle structure
 * @param[in]  address input address
 * @param[in]  *data pointer to a data buffer
 * @param[out] *crc16 pointer to a crc16 buffer
 * @return     status code
 *             - 0 success
 *             - 1 write scratchpad failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 address >= 0x0080
 *             - 5 address is invalid
 *             - 6 crc16 check error
 * @note       none
 */
uint8_t ds2431_write_scratchpad(ds2431_handle_t *handle, uint16_t address, uint8_t data[8], uint16_t *crc16);

/**
 * @brief      read scratchpad
 * @param[in]  *handle pointer to a ds2431 handle structure
 * @param[out] *address pointer to an address buffer
 * @param[out] *data pointer to a data buffer
 * @param[out] *crc16 pointer to a crc16 buffer
 * @return     status code
 *             - 0 success
 *             - 1 read scratchpad failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 status is error
 *             - 5 crc16 check error
 * @note       none
 */
uint8_t ds2431_read_scratchpad(ds2431_handle_t *handle, uint16_t *address, uint8_t data[8], uint16_t *crc16);

/**
 * @brief      read memory
 * @param[in]  *handle pointer to a ds2431 handle structure
 * @param[in]  address input address
 * @param[out] *data pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 read memory failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 *             - 4 address and len are invalid
 * @note       none
 */
uint8_t ds2431_read_memory(ds2431_handle_t *handle, uint16_t address, uint8_t *data, uint16_t len);

/**
 * @}
 */

/**
 * @defgroup ds2431_advance_driver ds2431 advance driver function
 * @brief    ds2431 advance driver modules
 * @ingroup  ds2431_driver
 * @{
 */

/**
 * @brief         search the ds2431 rom
 * @param[in]     *handle pointer to a ds2431 handle structure
 * @param[out]    **rom pointer to a rom array
 * @param[in,out] *num pointer to an array size buffer
 * @return        status code
 *                - 0 success
 *                - 1 search rom failed
 * @note          none
 */
uint8_t ds2431_search_rom(ds2431_handle_t *handle, uint8_t (*rom)[8], uint8_t *num);

/**
 * @brief      read memory config
 * @param[in]  *handle pointer to a ds2431 handle structure
 * @param[out] *config pointer to a ds2431 config control structure
 * @return     status code
 *             - 0 success
 *             - 1 read memory config failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t ds2431_read_memory_config(ds2431_handle_t *handle, ds2431_config_control_t *config);

/**
 * @brief     write memory config
 * @param[in] *handle pointer to a ds2431 handle structure
 * @param[in] *config pointer to a ds2431 config control structure
 * @return    status code
 *            - 0 success
 *            - 1 write memory config failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ds2431_write_memory_config(ds2431_handle_t *handle, ds2431_config_control_t *config);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
