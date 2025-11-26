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
 * @file      driver_ds2431_basic.h
 * @brief     driver ds2431 basic header file
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
  
#ifndef DRIVER_DS2431_BASIC_H
#define DRIVER_DS2431_BASIC_H

#include "driver_ds2431_interface.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup ds2431_example_driver ds2431 example driver function
 * @brief    ds2431 example driver modules
 * @ingroup  ds2431_driver
 * @{
 */

/**
 * @brief ds2431 basic example default definition
 */
#define DS2431_BASIC_DEFAULT_MODE        DS2431_MODE_SKIP_ROM        /**< skip rom mode */

/**
 * @brief  basic example init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t ds2431_basic_init(void);

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t ds2431_basic_deinit(void);

/**
 * @brief      basic example read
 * @param[in]  address input address
 * @param[out] *data pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t ds2431_basic_read(uint8_t address, uint8_t *data, uint8_t len);

/**
 * @brief     basic example write
 * @param[in] address input address
 * @param[in] *data pointer to a data buffer
 * @param[in] len data length
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t ds2431_basic_write(uint8_t address, uint8_t *data, uint8_t len);

/**
 * @brief     basic example write memory config
 * @param[in] *config pointer to a ds2431 config control structure
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t ds2431_basic_write_memory_config(ds2431_config_control_t *config);

/**
 * @brief      basic example read memory config
 * @param[out] *config pointer to a ds2431 config control structure
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t ds2431_basic_read_memory_config(ds2431_config_control_t *config);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
