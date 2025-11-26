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
 * @file      driver_ds2431_read_test.c
 * @brief     driver ds2431 read test source file
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

#include "driver_ds2431_read_test.h"
#include <stdlib.h>

static ds2431_handle_t gs_handle;        /**< ds2431 handle */
static uint8_t gs_buffer[128];           /**< data buffer */
static uint8_t gs_buffer_check[128];     /**< check buffer */

/**
 * @brief     read test
 * @param[in] times test times
 * @return    status code
 *            - 0 success
 *            - 1 test failed
 * @note      none
 */
uint8_t ds2431_read_test(uint32_t times)
{
    uint8_t res;
    uint8_t j;
    uint16_t crc16;
    uint16_t crc16_check;
    uint16_t addr;
    uint16_t addr_check;
    uint32_t i;
    uint8_t rom[8];
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
    
    /* ds2431 init */
    res = ds2431_init(&gs_handle);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: init failed.\n");
       
        return 1;
    }
    
    /* start read test */
    ds2431_interface_debug_print("ds2431: start read test.\n");
    
    /* set skip rom */
    ds2431_interface_debug_print("ds2431: set skip rom.\n"); 
    
    /* skip rom mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_SKIP_ROM);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* make test data */
    for (j = 0; j < 8; j++)
    {
        gs_buffer[j]  = rand() % 0xFFU;
    }
    
    /* write/read/copy scratchpad test */
    ds2431_interface_debug_print("ds2431: write/read/copy scratchpad test.\n");
    
    /* write scratchpad */
    addr = (rand() % 128) / 8 * 8;
    res = ds2431_write_scratchpad(&gs_handle, addr, gs_buffer, &crc16);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: write scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read scratchpad */
    res = ds2431_read_scratchpad(&gs_handle, &addr_check, gs_buffer_check, &crc16_check);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: read scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* check data */
    for (j = 0; j < 8; j++)
    {
        /* check buffer */
        if (gs_buffer[j] != gs_buffer_check[j])
        {
            ds2431_interface_debug_print("ds2431: check failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
    }
    ds2431_interface_debug_print("ds2431: data check passed.\n");
    
    /* check address */
    if (addr_check != addr)
    {
        ds2431_interface_debug_print("ds2431: address check failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: 0x%02X address check passed.\n", addr);
    
    /* copy scratchpad */
    res = ds2431_copy_scratchpad(&gs_handle, addr);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: copy scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read_memory test */
    ds2431_interface_debug_print("ds2431: read_memory test.\n");
    
    /* read memory */
    memset(gs_buffer_check, 0, sizeof(uint8_t) * 128);
    res = ds2431_read_memory(&gs_handle, addr, gs_buffer_check, 8);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: read memory failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* check data */
    for (j = 0; j < 8; j++)
    {
        /* check buffer */
        if (gs_buffer[j] != gs_buffer_check[j])
        {
            ds2431_interface_debug_print("ds2431: check failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
    }
    ds2431_interface_debug_print("ds2431: data check passed.\n");
    
    /* set overdrive skip rom */
    ds2431_interface_debug_print("ds2431: set overdrive skip rom.\n"); 
    
    /* overdrive skip rom mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_OVERDRIVE_SKIP_ROM);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* make test data */
    for (j = 0; j < 8; j++)
    {
        gs_buffer[j]  = rand() % 0xFFU;
    }
    
    /* write/read/copy scratchpad test */
    ds2431_interface_debug_print("ds2431: write/read/copy scratchpad test.\n");
    
    /* write scratchpad */
    addr = (rand() % 128) / 8 * 8;
    res = ds2431_write_scratchpad(&gs_handle, addr, gs_buffer, &crc16);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: write scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read scratchpad */
    res = ds2431_read_scratchpad(&gs_handle, &addr_check, gs_buffer_check, &crc16_check);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: read scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* check data */
    for (j = 0; j < 8; j++)
    {
        /* check buffer */
        if (gs_buffer[j] != gs_buffer_check[j])
        {
            ds2431_interface_debug_print("ds2431: check failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
    }
    ds2431_interface_debug_print("ds2431: data check passed.\n");
    
    /* check address */
    if (addr_check != addr)
    {
        ds2431_interface_debug_print("ds2431: address check failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: 0x%02X address check passed.\n", addr);
    
    /* copy scratchpad */
    res = ds2431_copy_scratchpad(&gs_handle, addr);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: copy scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read_memory test */
    ds2431_interface_debug_print("ds2431: read_memory test.\n");
    
    /* read memory */
    memset(gs_buffer_check, 0, sizeof(uint8_t) * 128);
    res = ds2431_read_memory(&gs_handle, addr, gs_buffer_check, 8);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: read memory failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* check data */
    for (j = 0; j < 8; j++)
    {
        /* check buffer */
        if (gs_buffer[j] != gs_buffer_check[j])
        {
            ds2431_interface_debug_print("ds2431: check failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
    }
    ds2431_interface_debug_print("ds2431: data check passed.\n");
    
    /* set rom */
    res = ds2431_get_rom(&gs_handle, (uint8_t *)rom);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: get rom failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    res = ds2431_set_rom(&gs_handle, (uint8_t *)rom);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set match rom */
    ds2431_interface_debug_print("ds2431: set match rom.\n"); 
    
    /* match rom mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_MATCH_ROM);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* make test data */
    for (j = 0; j < 8; j++)
    {
        gs_buffer[j]  = rand() % 0xFFU;
    }
    
    /* write/read/copy scratchpad test */
    ds2431_interface_debug_print("ds2431: write/read/copy scratchpad test.\n");
    
    /* write scratchpad */
    addr = (rand() % 128) / 8 * 8;
    res = ds2431_write_scratchpad(&gs_handle, addr, gs_buffer, &crc16);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: write scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read scratchpad */
    res = ds2431_read_scratchpad(&gs_handle, &addr_check, gs_buffer_check, &crc16_check);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: read scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* check data */
    for (j = 0; j < 8; j++)
    {
        /* check buffer */
        if (gs_buffer[j] != gs_buffer_check[j])
        {
            ds2431_interface_debug_print("ds2431: check failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
    }
    ds2431_interface_debug_print("ds2431: data check passed.\n");
    
    /* check address */
    if (addr_check != addr)
    {
        ds2431_interface_debug_print("ds2431: address check failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: 0x%02X address check passed.\n", addr);
    
    /* copy scratchpad */
    res = ds2431_copy_scratchpad(&gs_handle, addr);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: copy scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read_memory test */
    ds2431_interface_debug_print("ds2431: read_memory test.\n");
    
    /* read memory */
    memset(gs_buffer_check, 0, sizeof(uint8_t) * 128);
    res = ds2431_read_memory(&gs_handle, addr, gs_buffer_check, 8);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: read memory failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* check data */
    for (j = 0; j < 8; j++)
    {
        /* check buffer */
        if (gs_buffer[j] != gs_buffer_check[j])
        {
            ds2431_interface_debug_print("ds2431: check failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
    }
    ds2431_interface_debug_print("ds2431: data check passed.\n");
    
    /* set overdrive match rom */
    ds2431_interface_debug_print("ds2431: set overdrive match rom.\n"); 
    
    /* overdrive match rom mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_OVERDRIVE_MATCH_ROM);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* make test data */
    for (j = 0; j < 8; j++)
    {
        gs_buffer[j]  = rand() % 0xFFU;
    }
    
    /* write/read/copy scratchpad test */
    ds2431_interface_debug_print("ds2431: write/read/copy scratchpad test.\n");
    
    /* write scratchpad */
    addr = (rand() % 128) / 8 * 8;
    res = ds2431_write_scratchpad(&gs_handle, addr, gs_buffer, &crc16);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: write scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read scratchpad */
    res = ds2431_read_scratchpad(&gs_handle, &addr_check, gs_buffer_check, &crc16_check);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: read scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* check data */
    for (j = 0; j < 8; j++)
    {
        /* check buffer */
        if (gs_buffer[j] != gs_buffer_check[j])
        {
            ds2431_interface_debug_print("ds2431: check failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
    }
    ds2431_interface_debug_print("ds2431: data check passed.\n");
    
    /* check address */
    if (addr_check != addr)
    {
        ds2431_interface_debug_print("ds2431: address check failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: 0x%02X address check passed.\n", addr);
    
    /* copy scratchpad */
    res = ds2431_copy_scratchpad(&gs_handle, addr);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: copy scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read_memory test */
    ds2431_interface_debug_print("ds2431: read_memory test.\n");
    
    /* read memory */
    memset(gs_buffer_check, 0, sizeof(uint8_t) * 128);
    res = ds2431_read_memory(&gs_handle, addr, gs_buffer_check, 8);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: read memory failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* check data */
    for (j = 0; j < 8; j++)
    {
        /* check buffer */
        if (gs_buffer[j] != gs_buffer_check[j])
        {
            ds2431_interface_debug_print("ds2431: check failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
    }
    ds2431_interface_debug_print("ds2431: data check passed.\n");
    
    /* set resume mode */
    ds2431_interface_debug_print("ds2431: set resume mode.\n"); 
    
    /* match rom */
    res = ds2431_rom_match(&gs_handle, DS2431_TYPE_MATCH_ROM, rom);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: rom match failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* resume mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_RESUME);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* make test data */
    for (j = 0; j < 8; j++)
    {
        gs_buffer[j]  = rand() % 0xFFU;
    }
    
    /* write/read/copy scratchpad test */
    ds2431_interface_debug_print("ds2431: write/read/copy scratchpad test.\n");
    
    /* write scratchpad */
    addr = (rand() % 128) / 8 * 8;
    res = ds2431_write_scratchpad(&gs_handle, addr, gs_buffer, &crc16);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: write scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read scratchpad */
    res = ds2431_read_scratchpad(&gs_handle, &addr_check, gs_buffer_check, &crc16_check);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: read scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* check data */
    for (j = 0; j < 8; j++)
    {
        /* check buffer */
        if (gs_buffer[j] != gs_buffer_check[j])
        {
            ds2431_interface_debug_print("ds2431: check failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
    }
    ds2431_interface_debug_print("ds2431: data check passed.\n");
    
    /* check address */
    if (addr_check != addr)
    {
        ds2431_interface_debug_print("ds2431: address check failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: 0x%02X address check passed.\n", addr);
    
    /* copy scratchpad */
    res = ds2431_copy_scratchpad(&gs_handle, addr);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: copy scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read_memory test */
    ds2431_interface_debug_print("ds2431: read_memory test.\n");
    
    /* read memory */
    memset(gs_buffer_check, 0, sizeof(uint8_t) * 128);
    res = ds2431_read_memory(&gs_handle, addr, gs_buffer_check, 8);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: read memory failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* check data */
    for (j = 0; j < 8; j++)
    {
        /* check buffer */
        if (gs_buffer[j] != gs_buffer_check[j])
        {
            ds2431_interface_debug_print("ds2431: check failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
    }
    ds2431_interface_debug_print("ds2431: data check passed.\n");
    
    /* set overdrive match rom */
    ds2431_interface_debug_print("ds2431: set overdrive match rom.\n"); 
    
    /* overdrive match rom */
    res = ds2431_rom_match(&gs_handle, DS2431_TYPE_OVERDRIVE_MATCH_ROM, rom);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: rom match failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* overdrive resume mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_OVERDRIVE_RESUME);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* make test data */
    for (j = 0; j < 8; j++)
    {
        gs_buffer[j]  = rand() % 0xFFU;
    }
    
    /* write/read/copy scratchpad test */
    ds2431_interface_debug_print("ds2431: write/read/copy scratchpad test.\n");
    
    /* write scratchpad */
    addr = (rand() % 128) / 8 * 8;
    res = ds2431_write_scratchpad(&gs_handle, addr, gs_buffer, &crc16);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: write scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read scratchpad */
    res = ds2431_read_scratchpad(&gs_handle, &addr_check, gs_buffer_check, &crc16_check);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: read scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* check data */
    for (j = 0; j < 8; j++)
    {
        /* check buffer */
        if (gs_buffer[j] != gs_buffer_check[j])
        {
            ds2431_interface_debug_print("ds2431: check failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
    }
    ds2431_interface_debug_print("ds2431: data check passed.\n");
    
    /* check address */
    if (addr_check != addr)
    {
        ds2431_interface_debug_print("ds2431: address check failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: 0x%02X address check passed.\n", addr);
    
    /* copy scratchpad */
    res = ds2431_copy_scratchpad(&gs_handle, addr);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: copy scratchpad failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read_memory test */
    ds2431_interface_debug_print("ds2431: read_memory test.\n");
    
    /* read memory */
    memset(gs_buffer_check, 0, sizeof(uint8_t) * 128);
    res = ds2431_read_memory(&gs_handle, addr, gs_buffer_check, 8);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: read memory failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* check data */
    for (j = 0; j < 8; j++)
    {
        /* check buffer */
        if (gs_buffer[j] != gs_buffer_check[j])
        {
            ds2431_interface_debug_print("ds2431: check failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
    }
    ds2431_interface_debug_print("ds2431: data check passed.\n");
    
    /* skip rom mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_SKIP_ROM);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: set skip rom.\n"); 
    
    for (i = 0; i < times; i++)
    {
        /* make test data */
        for (j = 0; j < 128; j++)
        {
            gs_buffer[j]  = rand() % 0xFFU;
        }
        
        /* write data */
        res = ds2431_write(&gs_handle, 0, gs_buffer, 128);
        if (res != 0)
        {
            ds2431_interface_debug_print("ds2431: write failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
        
        /* read data */
        res = ds2431_read(&gs_handle, 0, gs_buffer_check, 128);
        if (res != 0)
        {
            ds2431_interface_debug_print("ds2431: read failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
        
        /* check data */
        for (j = 0; j < 128; j++)
        {
            /* check buffer */
            if (gs_buffer[j] != gs_buffer_check[j])
            {
                ds2431_interface_debug_print("ds2431: check failed.\n");
                (void)ds2431_deinit(&gs_handle);
                
                return 1;
            }
        }
        
        /* output */
        ds2431_interface_debug_print("ds2431: passed.\n"); 
        
        /* delay 1000ms */
        ds2431_interface_delay_ms(1000);
    }
    
    /* overdrive skip rom mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_OVERDRIVE_SKIP_ROM);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: set overdrive skip rom mode.\n"); 
    
    for (i = 0; i < times; i++)
    {
        /* make test data */
        for (j = 0; j < 128; j++)
        {
            gs_buffer[j]  = rand() % 0xFFU;
        }
        
        /* write data */
        res = ds2431_write(&gs_handle, 0, gs_buffer, 128);
        if (res != 0)
        {
            ds2431_interface_debug_print("ds2431: write failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
        
        /* read data */
        res = ds2431_read(&gs_handle, 0, gs_buffer_check, 128);
        if (res != 0)
        {
            ds2431_interface_debug_print("ds2431: read failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
        
        /* check data */
        for (j = 0; j < 128; j++)
        {
            /* check buffer */
            if (gs_buffer[j] != gs_buffer_check[j])
            {
                ds2431_interface_debug_print("ds2431: check failed.\n");
                (void)ds2431_deinit(&gs_handle);
                
                return 1;
            }
        }
        
        /* output */
        ds2431_interface_debug_print("ds2431: passed.\n"); 
        
        /* delay 1000ms */
        ds2431_interface_delay_ms(1000);
    }
    
    /* set rom */
    res = ds2431_get_rom(&gs_handle, (uint8_t *)rom);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: get rom failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    res = ds2431_set_rom(&gs_handle, (uint8_t *)rom);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* match rom mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_MATCH_ROM);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: set match rom mode.\n"); 
    
    for (i = 0; i < times; i++)
    {
        /* make test data */
        for (j = 0; j < 128; j++)
        {
            gs_buffer[j]  = rand() % 0xFFU;
        }
        
        /* write data */
        res = ds2431_write(&gs_handle, 0, gs_buffer, 128);
        if (res != 0)
        {
            ds2431_interface_debug_print("ds2431: write failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
        
        /* read data */
        res = ds2431_read(&gs_handle, 0, gs_buffer_check, 128);
        if (res != 0)
        {
            ds2431_interface_debug_print("ds2431: read failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
        
        /* check data */
        for (j = 0; j < 128; j++)
        {
            /* check buffer */
            if (gs_buffer[j] != gs_buffer_check[j])
            {
                ds2431_interface_debug_print("ds2431: check failed.\n");
                (void)ds2431_deinit(&gs_handle);
                
                return 1;
            }
        }
        
        /* output */
        ds2431_interface_debug_print("ds2431: passed.\n"); 
        
        /* delay 1000ms */
        ds2431_interface_delay_ms(1000);
    }
    
    /* overdrive match rom mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_OVERDRIVE_MATCH_ROM);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: set overdrive match rom mode.\n"); 
    
    for (i = 0; i < times; i++)
    {
        /* make test data */
        for (j = 0; j < 128; j++)
        {
            gs_buffer[j]  = rand() % 0xFFU;
        }
        
        /* write data */
        res = ds2431_write(&gs_handle, 0, gs_buffer, 128);
        if (res != 0)
        {
            ds2431_interface_debug_print("ds2431: write failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
        
        /* read data */
        res = ds2431_read(&gs_handle, 0, gs_buffer_check, 128);
        if (res != 0)
        {
            ds2431_interface_debug_print("ds2431: read failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
        
        /* check data */
        for (j = 0; j < 128; j++)
        {
            /* check buffer */
            if (gs_buffer[j] != gs_buffer_check[j])
            {
                ds2431_interface_debug_print("ds2431: check failed.\n");
                (void)ds2431_deinit(&gs_handle);
                
                return 1;
            }
        }
        
        /* output */
        ds2431_interface_debug_print("ds2431: passed.\n"); 
        
        /* delay 1000ms */
        ds2431_interface_delay_ms(1000);
    }
    
    /* resume mode */
    res = ds2431_rom_match(&gs_handle, DS2431_TYPE_MATCH_ROM, rom);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: rom match failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set resume mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_RESUME);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: set resume mode.\n"); 
    
    for (i = 0; i < times; i++)
    {
        /* make test data */
        for (j = 0; j < 128; j++)
        {
            gs_buffer[j]  = rand() % 0xFFU;
        }
        
        /* write data */
        res = ds2431_write(&gs_handle, 0, gs_buffer, 128);
        if (res != 0)
        {
            ds2431_interface_debug_print("ds2431: write failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
        
        /* read data */
        res = ds2431_read(&gs_handle, 0, gs_buffer_check, 128);
        if (res != 0)
        {
            ds2431_interface_debug_print("ds2431: read failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
        
        /* check data */
        for (j = 0; j < 128; j++)
        {
            /* check buffer */
            if (gs_buffer[j] != gs_buffer_check[j])
            {
                ds2431_interface_debug_print("ds2431: check failed.\n");
                (void)ds2431_deinit(&gs_handle);
                
                return 1;
            }
        }
        
        /* output */
        ds2431_interface_debug_print("ds2431: passed.\n"); 
        
        /* delay 1000ms */
        ds2431_interface_delay_ms(1000);
    }
    
    /* overdrive resume mode */
    res = ds2431_rom_match(&gs_handle, DS2431_TYPE_OVERDRIVE_MATCH_ROM, rom);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: rom match failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* set overdrive resume mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_OVERDRIVE_RESUME);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: set overdrive resume mode.\n"); 
    
    for (i = 0; i < times; i++)
    {
        /* make test data */
        for (j = 0; j < 128; j++)
        {
            gs_buffer[j]  = rand() % 0xFFU;
        }
        
        /* write data */
        res = ds2431_write(&gs_handle, 0, gs_buffer, 128);
        if (res != 0)
        {
            ds2431_interface_debug_print("ds2431: write failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
        
        /* read data */
        res = ds2431_read(&gs_handle, 0, gs_buffer_check, 128);
        if (res != 0)
        {
            ds2431_interface_debug_print("ds2431: read failed.\n");
            (void)ds2431_deinit(&gs_handle);
            
            return 1;
        }
        
        /* check data */
        for (j = 0; j < 128; j++)
        {
            /* check buffer */
            if (gs_buffer[j] != gs_buffer_check[j])
            {
                ds2431_interface_debug_print("ds2431: check failed.\n");
                (void)ds2431_deinit(&gs_handle);
                
                return 1;
            }
        }
        
        /* output */
        ds2431_interface_debug_print("ds2431: passed.\n"); 
        
        /* delay 1000ms */
        ds2431_interface_delay_ms(1000);
    }
    
    /* finish read test */
    ds2431_interface_debug_print("ds2431: finish read test.\n");
    (void)ds2431_deinit(&gs_handle);
    
    return 0;
}
