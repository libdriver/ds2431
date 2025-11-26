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
 * @file      driver_ds2431_search.c
 * @brief     driver ds2431 search source file
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
  
#include "driver_ds2431_search.h"

static ds2431_handle_t gs_handle;        /**< ds2431 handle */

/**
 * @brief  search example init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t ds2431_search_init(void)
{
    uint8_t res;
    
    /* link interface function */
    DRIVER_DS2431_LINK_INIT(&gs_handle, ds2431_handle_t);
    DRIVER_DS2431_LINK_BUS_INIT(&gs_handle, ds2431_interface_init);
    DRIVER_DS2431_LINK_BUS_DEINIT(&gs_handle, ds2431_interface_deinit);
    DRIVER_DS2431_LINK_BUS_READ(&gs_handle, ds2431_interface_read);
    DRIVER_DS2431_LINK_BUS_WRITE(&gs_handle, ds2431_interface_write);
    DRIVER_DS2431_LINK_DELAY_MS(&gs_handle, ds2431_interface_delay_ms);
    DRIVER_DS2431_LINK_DELAY_US(&gs_handle, ds2431_interface_delay_us);
    DRIVER_DS2431_LINK_ENABLE_IRQ(&gs_handle, ds2431_interface_enable_irq);
    DRIVER_DS2431_LINK_DISABLE_IRQ(&gs_handle, ds2431_interface_disable_irq);
    DRIVER_DS2431_LINK_DEBUG_PRINT(&gs_handle, ds2431_interface_debug_print);
    
    /* ds2431 init */
    res = ds2431_init(&gs_handle);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: init failed.\n");
        
        return 1;
    }
    
    return 0;
}

/**
 * @brief         search example find
 * @param[in]     *rom pointer to a rom buffer
 * @param[in,out] *num pointer to a number buffer
 * @return        status code
 *                - 0 success
 *                - 1 search failed
 * @note          none
 */
uint8_t ds2431_search(uint8_t (*rom)[8], uint8_t *num)
{
    /* search rom*/
    if (ds2431_search_rom(&gs_handle, rom, num) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  search example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t ds2431_search_deinit(void)
{
    /* ds2431 close */
    if (ds2431_deinit(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}
