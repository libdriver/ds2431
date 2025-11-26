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
 * @file      main.c
 * @brief     main source file
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

#include "driver_ds2431_basic.h"
#include "driver_ds2431_match.h"
#include "driver_ds2431_search.h"
#include "driver_ds2431_register_test.h"
#include "driver_ds2431_read_test.h"
#include "driver_ds2431_search_test.h"
#include "shell.h"
#include "clock.h"
#include "delay.h"
#include "uart.h"
#include "getopt.h"
#include <stdlib.h>
#include <math.h>

/**
 * @brief global var definition
 */
uint8_t g_buf[256];        /**< uart buffer */
volatile uint16_t g_len;   /**< uart buffer length */

/**
 * @brief     ds2431 full function
 * @param[in] argc arg numbers
 * @param[in] **argv arg address
 * @return    status code
 *            - 0 success
 *            - 1 run failed
 *            - 5 param is invalid
 * @note      none
 */
uint8_t ds2431(uint8_t argc, char **argv)
{
    int c;
    int longindex = 0;
    const char short_options[] = "hipe:t:";
    const struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"information", no_argument, NULL, 'i'},
        {"port", no_argument, NULL, 'p'},
        {"example", required_argument, NULL, 'e'},
        {"test", required_argument, NULL, 't'},
        {"addr", required_argument, NULL, 1},
        {"data", required_argument, NULL, 2},
        {"rom", required_argument, NULL, 3},
        {"times", required_argument, NULL, 4},
        {NULL, 0, NULL, 0},
    };
    char type[33] = "unknown";
    uint8_t addr = 0;
    uint8_t data = 0x00;
    uint32_t times = 3;
    uint8_t rom[8] = {0};
    
    /* if no params */
    if (argc == 1)
    {
        /* goto the help */
        goto help;
    }
    
    /* init 0 */
    optind = 0;
    
    /* parse */
    do
    {
        /* parse the args */
        c = getopt_long(argc, argv, short_options, long_options, &longindex);
        
        /* judge the result */
        switch (c)
        {
            /* help */
            case 'h' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "h");
                
                break;
            }
            
            /* information */
            case 'i' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "i");
                
                break;
            }
            
            /* port */
            case 'p' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "p");
                
                break;
            }
            
            /* example */
            case 'e' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "e_%s", optarg);
                
                break;
            }
            
            /* test */
            case 't' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "t_%s", optarg);
                
                break;
            }
            
            /* addr */
            case 1 :
            {
                char *p;
                uint16_t l;
                uint16_t i;
                uint64_t hex_data;

                /* set the data */
                l = strlen(optarg);

                /* check the header */
                if (l >= 2)
                {
                    if (strncmp(optarg, "0x", 2) == 0)
                    {
                        p = optarg + 2;
                        l -= 2;
                    }
                    else if (strncmp(optarg, "0X", 2) == 0)
                    {
                        p = optarg + 2;
                        l -= 2;
                    }
                    else
                    {
                        p = optarg;
                    }
                }
                else
                {
                    p = optarg;
                }
                
                /* init 0 */
                hex_data = 0;

                /* loop */
                for (i = 0; i < l; i++)
                {
                    if ((p[i] <= '9') && (p[i] >= '0'))
                    {
                        hex_data += (p[i] - '0') * (uint32_t)pow(16, l - i - 1);
                    }
                    else if ((p[i] <= 'F') && (p[i] >= 'A'))
                    {
                        hex_data += ((p[i] - 'A') + 10) * (uint32_t)pow(16, l - i - 1);
                    }
                    else if ((p[i] <= 'f') && (p[i] >= 'a'))
                    {
                        hex_data += ((p[i] - 'a') + 10) * (uint32_t)pow(16, l - i - 1);
                    }
                    else
                    {
                        return 5;
                    }
                }
                
                /* set the address */
                addr = hex_data & 0xFF;
                
                break;
            }
            
            /* data */
            case 2 :
            {
                char *p;
                uint16_t l;
                uint16_t i;
                uint64_t hex_data;

                /* set the data */
                l = strlen(optarg);

                /* check the header */
                if (l >= 2)
                {
                    if (strncmp(optarg, "0x", 2) == 0)
                    {
                        p = optarg + 2;
                        l -= 2;
                    }
                    else if (strncmp(optarg, "0X", 2) == 0)
                    {
                        p = optarg + 2;
                        l -= 2;
                    }
                    else
                    {
                        p = optarg;
                    }
                }
                else
                {
                    p = optarg;
                }
                
                /* init 0 */
                hex_data = 0;

                /* loop */
                for (i = 0; i < l; i++)
                {
                    if ((p[i] <= '9') && (p[i] >= '0'))
                    {
                        hex_data += (p[i] - '0') * (uint32_t)pow(16, l - i - 1);
                    }
                    else if ((p[i] <= 'F') && (p[i] >= 'A'))
                    {
                        hex_data += ((p[i] - 'A') + 10) * (uint32_t)pow(16, l - i - 1);
                    }
                    else if ((p[i] <= 'f') && (p[i] >= 'a'))
                    {
                        hex_data += ((p[i] - 'a') + 10) * (uint32_t)pow(16, l - i - 1);
                    }
                    else
                    {
                        return 5;
                    }
                }
                
                /* set the data */
                data = hex_data & 0xFF;
                
                break;
            }
            
            /* rom */
            case 3 :
            {
                uint8_t i;
                
                /* check the flag */
                if (strlen(optarg) != 16)
                {
                    return 5;
                }
                
                /* set the rom */
                for (i = 0; i < 8; i++)
                {
                    uint8_t temp;
                    
                    if ((optarg[i * 2 + 0] <= '9') && (optarg[i * 2 + 0] >= '0'))
                    {
                        temp = (optarg[i * 2 + 0] - '0') * 16;
                    }
                    else
                    {
                        temp = (optarg[i * 2 + 0] - 'A' + 10) * 16;
                    }
                    if ((optarg[i * 2 + 1] <= '9') && (optarg[i * 2 + 1] >= '0'))
                    {
                        temp += optarg[i * 2 + 1] - '0';
                    }
                    else
                    {
                        temp += optarg[i * 2 + 1] - 'A' + 10;
                    }
                    rom[i] = temp;
                }
                
                break;
            }
            
            /* running times */
            case 4 :
            {
                /* set the times */
                times = atol(optarg);
                
                break;
            } 
            
            /* the end */
            case -1 :
            {
                break;
            }
            
            /* others */
            default :
            {
                return 5;
            }
        }
    } while (c != -1);

    /* run the function */
    if (strcmp("t_reg", type) == 0)
    {
        /* run reg test */
        if (ds2431_register_test() != 0)
        {
            return 1;
        }
        
        return 0;
    }
    else if (strcmp("t_read", type) == 0)
    {
        /* run read test */
        if (ds2431_read_test(times) != 0)
        {
            return 1;
        }
        
        return 0;
    }
    else if (strcmp("t_search", type) == 0)
    {
        /* run search test */
        if (ds2431_search_test() != 0)
        {
            return 1;
        }
        
        return 0;
    }
    else if (strcmp("e_skip-read", type) == 0)
    {
        uint8_t res;
        
        /* init */
        res = ds2431_basic_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* read data */
        res = ds2431_basic_read(addr, &data, 1);
        if (res != 0)
        {
            (void)ds2431_basic_deinit();
            
            return 1;
        }
        
        /* output */
        ds2431_interface_debug_print("ds2431: address 0x%02X read data 0x%02X.\n", addr, data);
        
        /* deinit */
        (void)ds2431_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_skip-write", type) == 0)
    {
        uint8_t res;
        
        /* init */
        res = ds2431_basic_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* write data */
        res = ds2431_basic_write(addr, &data, 1);
        if (res != 0)
        {
            (void)ds2431_basic_deinit();
            
            return 1;
        }
        
        /* output */
        ds2431_interface_debug_print("ds2431: address 0x%02X write data 0x%02X.\n", addr, data);
        
        /* deinit */
        (void)ds2431_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_skip-config", type) == 0)
    {
        uint8_t res;
        ds2431_config_control_t config;
        
        /* init */
        res = ds2431_basic_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* read data */
        res = ds2431_basic_read_memory_config(&config);
        if (res != 0)
        {
            (void)ds2431_basic_deinit();
            
            return 1;
        }
        
        /* output */
        ds2431_interface_debug_print("ds2431: page0 protection control is 0x%02X.\n", config.page0_protection_control);
        ds2431_interface_debug_print("ds2431: page1 protection control is 0x%02X.\n", config.page1_protection_control);
        ds2431_interface_debug_print("ds2431: page2 protection control is 0x%02X.\n", config.page2_protection_control);
        ds2431_interface_debug_print("ds2431: page3 protection control is 0x%02X.\n", config.page3_protection_control);
        ds2431_interface_debug_print("ds2431: copy protection is 0x%02X.\n", config.copy_protection);
        ds2431_interface_debug_print("ds2431: factory byte is 0x%02X.\n", config.factory_byte);
        ds2431_interface_debug_print("ds2431: user byte 0 is 0x%02X.\n", config.user_byte_0);
        ds2431_interface_debug_print("ds2431: user byte 1 is 0x%02X.\n", config.user_byte_1);
        
        /* deinit */
        (void)ds2431_basic_deinit();
        
        return 0;
    }
    else if (strcmp("e_match-read", type) == 0)
    {
        uint8_t res;
        
        /* init */
        res = ds2431_match_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* read data */
        res = ds2431_match_read((uint8_t *)rom, addr, &data, 1);
        if (res != 0)
        {
            (void)ds2431_match_deinit();
            
            return 1;
        }
        
        /* output */
        ds2431_interface_debug_print("ds2431: address 0x%02X read data 0x%02X.\n", addr, data);
        
        /* deinit */
        (void)ds2431_match_deinit();
        
        return 0;
    }
    else if (strcmp("e_match-write", type) == 0)
    {
        uint8_t res;
        
        /* init */
        res = ds2431_match_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* write data */
        res = ds2431_match_write((uint8_t *)rom, addr, &data, 1);
        if (res != 0)
        {
            (void)ds2431_match_deinit();
            
            return 1;
        }
        
        /* output */
        ds2431_interface_debug_print("ds2431: address 0x%02X write data 0x%02X.\n", addr, data);
        
        /* deinit */
        (void)ds2431_match_deinit();
        
        return 0;
    }
    else if (strcmp("e_match-config", type) == 0)
    {
        uint8_t res;
        ds2431_config_control_t config;
        
        /* init */
        res = ds2431_match_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* read data */
        res = ds2431_match_read_memory_config((uint8_t *)rom, &config);
        if (res != 0)
        {
            (void)ds2431_match_deinit();
            
            return 1;
        }
        
        /* output */
        ds2431_interface_debug_print("ds2431: page0 protection control is 0x%02X.\n", config.page0_protection_control);
        ds2431_interface_debug_print("ds2431: page1 protection control is 0x%02X.\n", config.page1_protection_control);
        ds2431_interface_debug_print("ds2431: page2 protection control is 0x%02X.\n", config.page2_protection_control);
        ds2431_interface_debug_print("ds2431: page3 protection control is 0x%02X.\n", config.page3_protection_control);
        ds2431_interface_debug_print("ds2431: copy protection is 0x%02X.\n", config.copy_protection);
        ds2431_interface_debug_print("ds2431: factory byte is 0x%02X.\n", config.factory_byte);
        ds2431_interface_debug_print("ds2431: user byte 0 is 0x%02X.\n", config.user_byte_0);
        ds2431_interface_debug_print("ds2431: user byte 1 is 0x%02X.\n", config.user_byte_1);
        
        /* deinit */
        (void)ds2431_match_deinit();
        
        return 0;
    }
    else if (strcmp("e_search", type) == 0)
    {
        uint8_t res, i, j;
        uint8_t rom[8][8];
        uint8_t num;
        
        /* init */
        res = ds2431_search_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* search */
        num = 8;
        res = ds2431_search((uint8_t (*)[8])rom, (uint8_t *)&num);
        if (res != 0)
        {
            (void)ds2431_search_deinit();
            
            return 1;
        }
        
        /* output */
        ds2431_interface_debug_print("ds2431: find %d rom(s).\n", num);
        for (i = 0; i < num; i++)
        {
            ds2431_interface_debug_print("ds2431: %d/%d is ", (uint32_t)(i + 1), (uint32_t)num);
            for (j = 0; j < 8; j++)
            {
                ds2431_interface_debug_print("%02X", rom[i][j]);
            }
            ds2431_interface_debug_print(".\n");
        }
        
        /* deinit */
        (void)ds2431_search_deinit();
        
        return 0;
    }
    else if (strcmp("h", type) == 0)
    {
        help:
        ds2431_interface_debug_print("Usage:\n");
        ds2431_interface_debug_print("  ds2431 (-i | --information)\n");
        ds2431_interface_debug_print("  ds2431 (-h | --help)\n");
        ds2431_interface_debug_print("  ds2431 (-p | --port)\n");
        ds2431_interface_debug_print("  ds2431 (-t reg | --test=reg)\n");
        ds2431_interface_debug_print("  ds2431 (-t read | --test=read) [--times=<num>]\n");
        ds2431_interface_debug_print("  ds2431 (-t search | --test=search)\n");
        ds2431_interface_debug_print("  ds2431 (-e skip-read | --example=skip-read) [--addr=<hex>]\n");
        ds2431_interface_debug_print("  ds2431 (-e skip-write | --example=skip-write) [--addr=<hex>] [--data=<hex>]\n");
        ds2431_interface_debug_print("  ds2431 (-e skip-config | --example=skip-config)\n");
        ds2431_interface_debug_print("  ds2431 (-e match-read | --example=match-read) [--rom=<code>] [--addr=<hex>]\n");
        ds2431_interface_debug_print("  ds2431 (-e match-write | --example=match-write) [--rom=<code>] [--addr=<hex>] [--data=<hex>]\n");
        ds2431_interface_debug_print("  ds2431 (-e match-config | --example=match-config) [--rom=<code>]\n");
        ds2431_interface_debug_print("  ds2431 (-e search | --example=search)\n");
        ds2431_interface_debug_print("\n");
        ds2431_interface_debug_print("Options:\n");
        ds2431_interface_debug_print("      --addr=<hex>               Set the read or write address and it is hexadecimal.([default: 0x00])\n");
        ds2431_interface_debug_print("      --data=<hex>               Set the write data and it is hexadecimal.([default: 0x00])\n");
        ds2431_interface_debug_print("  -e <skip-read | skip-write | skip-config | match-read | match-write | match-config | search>,\n");
        ds2431_interface_debug_print("      --example=<skip-read | skip-write | skip-config | match-read | match-write | match-config | search>\n");
        ds2431_interface_debug_print("                                 Run the driver example.\n");
        ds2431_interface_debug_print("  -h, --help                     Show the help.\n");
        ds2431_interface_debug_print("  -i, --information              Show the chip information.\n");
        ds2431_interface_debug_print("  -p, --port                     Display the pin connections of the current board.\n");
        ds2431_interface_debug_print("      --rom=<code>               Set the rom with the length of 8 and it is hexadecimal.([default: 0000000000000000])\n");
        ds2431_interface_debug_print("  -t <reg | read | search>, --test=<reg | read | search>\n");
        ds2431_interface_debug_print("                                 Run the driver test.\n");
        ds2431_interface_debug_print("      --times=<num>              Set the running times.([default: 3])\n");
        
        return 0;
    }
    else if (strcmp("i", type) == 0)
    {
        ds2431_info_t info;
        
        /* print ds2431 info */
        ds2431_info(&info);
        ds2431_interface_debug_print("ds2431: chip is %s.\n", info.chip_name);
        ds2431_interface_debug_print("ds2431: manufacturer is %s.\n", info.manufacturer_name);
        ds2431_interface_debug_print("ds2431: interface is %s.\n", info.interface);
        ds2431_interface_debug_print("ds2431: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        ds2431_interface_debug_print("ds2431: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        ds2431_interface_debug_print("ds2431: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        ds2431_interface_debug_print("ds2431: max current is %0.2fmA.\n", info.max_current_ma);
        ds2431_interface_debug_print("ds2431: max temperature is %0.1fC.\n", info.temperature_max);
        ds2431_interface_debug_print("ds2431: min temperature is %0.1fC.\n", info.temperature_min);
        
        return 0;
    }
    else if (strcmp("p", type) == 0)
    {
        /* print pin connection */
        ds2431_interface_debug_print("ds2431: DQ pin connected to GPIOA PIN8.\n");
        
        return 0;
    }
    else
    {
        return 5;
    }
}

/**
 * @brief main function
 * @note  none
 */
int main(void)
{
    uint8_t res;
    
    /* stm32f407 clock init and hal init */
    clock_init();
    
    /* delay init */
    delay_init();
    
    /* uart init */
    uart_init(115200);
    
    /* shell init && register ds2431 function */
    shell_init();
    shell_register("ds2431", ds2431);
    uart_print("ds2431: welcome to libdriver ds2431.\n");
    
    while (1)
    {
        /* read uart */
        g_len = uart_read(g_buf, 256);
        if (g_len != 0)
        {
            /* run shell */
            res = shell_parse((char *)g_buf, g_len);
            if (res == 0)
            {
                /* run success */
            }
            else if (res == 1)
            {
                uart_print("ds2431: run failed.\n");
            }
            else if (res == 2)
            {
                uart_print("ds2431: unknown command.\n");
            }
            else if (res == 3)
            {
                uart_print("ds2431: length is too long.\n");
            }
            else if (res == 4)
            {
                uart_print("ds2431: pretreat failed.\n");
            }
            else if (res == 5)
            {
                uart_print("ds2431: param is invalid.\n");
            }
            else
            {
                uart_print("ds2431: unknown status code.\n");
            }
            uart_flush();
        }
        delay_ms(100);
    }
}
