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
 * @file      driver_ds2431_register_test.c
 * @brief     driver ds2431 register test source file
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

#include "driver_ds2431_register_test.h"

static ds2431_handle_t gs_handle;        /**< ds2431 handle */

/**
 * @brief  register test
 * @return status code
 *         - 0 success
 *         - 1 test failed
 * @note   none
 */
uint8_t ds2431_register_test(void)
{
    uint8_t res;
    uint8_t i;
    uint8_t rom[8];
    uint8_t rom_check[8];
    ds2431_info_t info;
    ds2431_mode_t mode;
    ds2431_config_control_t config;
    ds2431_config_control_t config_check;
    
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
    
    /* start register test */
    ds2431_interface_debug_print("ds2431: start register test.\n");
    
    /* ds2431 init */
    res = ds2431_init(&gs_handle);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: init failed.\n");
       
        return 1;
    }
    
    /* ds2431_set_mode/ds2431_get_mode test */
    ds2431_interface_debug_print("ds2431: ds2431_set_mode/ds2431_get_mode test.\n");
    
    /* skip rom */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_SKIP_ROM);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: set skip rom mode.\n");
    res = ds2431_get_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: get mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: check mode %s.\n", (mode == DS2431_MODE_SKIP_ROM) ? "ok" : "error");
    
    /* match rom */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_MATCH_ROM);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: set match rom mode.\n");
    res = ds2431_get_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: get mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: check mode %s.\n", (mode == DS2431_MODE_MATCH_ROM) ? "ok" : "error");
    
    /* overdrive skip rom mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_OVERDRIVE_SKIP_ROM);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: set overdrive skip rom mode.\n");
    res = ds2431_get_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: get mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: check mode %s.\n", (mode == DS2431_MODE_OVERDRIVE_SKIP_ROM) ? "ok" : "error");
    
    /* overdrive match rom mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_OVERDRIVE_MATCH_ROM);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: set overdrive match rom mode.\n");
    res = ds2431_get_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: get mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: check mode %s.\n", (mode == DS2431_MODE_OVERDRIVE_MATCH_ROM) ? "ok" : "error");
    
    /* resume mode */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_RESUME);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: set resume mode.\n");
    res = ds2431_get_mode(&gs_handle, &mode);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: get mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: check mode %s.\n", (mode == DS2431_MODE_RESUME) ? "ok" : "error");
    
    /* ds2431_set_rom/ds2431_get_rom test */
    ds2431_interface_debug_print("ds2431: ds2431_set_rom/ds2431_get_rom test.\n");
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
    ds2431_interface_debug_print("ds2431: set rom.\n");
    res = ds2431_get_rom(&gs_handle, (uint8_t *)rom_check);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: get rom failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    for (i = 0; i < 8; i++)
    {
        if (rom[i] != rom_check[i])
        {
            res = 1;
        }
    }
    ds2431_interface_debug_print("ds2431: check rom %s.\n", (res == 0) ? "ok" : "error");
    
    /* ds2431_read_memory_config/ds2431_write_memory_config test */
    ds2431_interface_debug_print("ds2431: ds2431_read_memory_config/ds2431_write_memory_config test.\n");
    
    /* skip rom */
    res = ds2431_set_mode(&gs_handle, DS2431_MODE_SKIP_ROM);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: set mode failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read memory config */
    res = ds2431_read_memory_config(&gs_handle, &config);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: read memory config failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: page0 protection control is 0x%02X.\n", config.page0_protection_control);
    ds2431_interface_debug_print("ds2431: page1 protection control is 0x%02X.\n", config.page1_protection_control);
    ds2431_interface_debug_print("ds2431: page2 protection control is 0x%02X.\n", config.page2_protection_control);
    ds2431_interface_debug_print("ds2431: page3 protection control is 0x%02X.\n", config.page3_protection_control);
    ds2431_interface_debug_print("ds2431: copy protection is 0x%02X.\n", config.copy_protection);
    ds2431_interface_debug_print("ds2431: factory byte is 0x%02X.\n", config.factory_byte);
    ds2431_interface_debug_print("ds2431: user byte 0 is 0x%02X.\n", config.user_byte_0);
    ds2431_interface_debug_print("ds2431: user byte 1 is 0x%02X.\n", config.user_byte_1);
    
    /* write memory config */
    res = ds2431_write_memory_config(&gs_handle, &config);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: write memory config failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    
    /* read memory config */
    res = ds2431_read_memory_config(&gs_handle, &config_check);
    if (res != 0)
    {
        ds2431_interface_debug_print("ds2431: read memory config failed.\n");
        (void)ds2431_deinit(&gs_handle);
        
        return 1;
    }
    ds2431_interface_debug_print("ds2431: check memory config %s.\n", 
                                (memcmp((uint8_t *)&config, (uint8_t *)&config_check, sizeof(ds2431_config_control_t)) == 0) ? "ok" : "error");
    
    /* finish register test */
    ds2431_interface_debug_print("ds2431: finish register test.\n");
    (void)ds2431_deinit(&gs_handle);
    
    return 0;
}
