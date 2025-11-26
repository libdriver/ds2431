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
 * @file      driver_ds2431_search_test.c
 * @brief     driver ds2431 search test source file
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

#include "driver_ds2431_search_test.h"

static ds2431_handle_t gs_handle;        /**< ds2431 handle */

/**
 * @brief  search test
 * @return status code
 *         - 0 success
 *         - 1 test failed
 * @note   none
 */
uint8_t ds2431_search_test(void)
{
    uint8_t res, i, num;
    uint8_t rom[3][8];
    ds2431_info_t info;
   
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
   
    /* get ds2431 info */
    res = ds2431_info(&info);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: get info failed.\n");
       
        return 1;
    }
    else
    {
        /* print ds2431 info */
        ds2431_interface_debug_print("ds2431: chip is %s.\n", info.chip_name);
        ds2431_interface_debug_print("ds2431: manufacturer is %s.\n", info.manufacturer_name);
        ds2431_interface_debug_print("ds2431: interface is %s.\n", info.interface);
        ds2431_interface_debug_print("ds2431: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        ds2431_interface_debug_print("ds2431: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        ds2431_interface_debug_print("ds2431: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        ds2431_interface_debug_print("ds2431: max current is %0.2fmA.\n", info.max_current_ma);
        ds2431_interface_debug_print("ds2431: max temperature is %0.1fC.\n", info.temperature_max);
        ds2431_interface_debug_print("ds2431: min temperature is %0.1fC.\n", info.temperature_min);
    }
    
    /* start search test */
    ds2431_interface_debug_print("ds2431: start search test.\n");
    
    /* ds2431 init */
    res = ds2431_init(&gs_handle);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: init failed.\n");
       
        return 1;
    }
    
    /* search rom */
    ds2431_interface_debug_print("ds2431: search rom...\n");
    
    /* max find 3 */
    num = 3;
    
    /* search rom */
    res = ds2431_search_rom(&gs_handle, (uint8_t (*)[8])rom, (uint8_t *)&num);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: search rom failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: find %d rom.\n", num);
    
    /* print found rom */
    for (i = 0; i < num; i++)
    {
        char cmd_buf[128];
        
        memset(cmd_buf, 0, 128);
        strcat(cmd_buf, "ds2431: rom ");
        for (uint8_t j=0; j<8; j++)
        {
            char hex_buf[6];
            
            memset((char *)hex_buf, 0 ,sizeof(char)*6);
            (void)snprintf((char *)hex_buf, 6, "%02X", rom[i][j]);
            strcat((char *)cmd_buf, (char *)hex_buf);
        }
        ds2431_interface_debug_print("%s.\n",cmd_buf);
    }
    
    /* finish search test */
    ds2431_interface_debug_print("ds2431: finish search test.\n");
    (void)ds2431_deinit(&gs_handle);
    
    return 0;
}
