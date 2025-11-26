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
 * @file      driver_ds2431_search.h
 * @brief     driver ds2431 search header file
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
  
#ifndef DRIVER_DS2431_SEARCH_H
#define DRIVER_DS2431_SEARCH_H

#include "driver_ds2431_interface.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @addtogroup ds2431_example_driver
 * @{
 */

/**
 * @brief  search example init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t ds2431_search_init(void);

/**
 * @brief         search example find
 * @param[in]     *rom pointer to a rom buffer
 * @param[in,out] *num pointer to a number buffer
 * @return        status code
 *                - 0 success
 *                - 1 search failed
 * @note          none
 */
uint8_t ds2431_search(uint8_t (*rom)[8], uint8_t *num);

/**
 * @brief  search example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t ds2431_search_deinit(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
