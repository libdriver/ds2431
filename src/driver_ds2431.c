/**
 * Copyright (c) 2015 - presend LibDriver All rights reserved
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
 * @file      driver_ds2431.c
 * @brief     driver ds2431 source file
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

#include "driver_ds2431.h"

/**
 * @brief chip information definition
 */
#define CHIP_NAME                 "Maxim Integrated DS2431"        /**< chip name */
#define MANUFACTURER_NAME         "Maxim Integrated"               /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        2.8f                             /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        5.25f                            /**< chip max supply voltage */
#define MAX_CURRENT               0.8f                             /**< chip max current */
#define TEMPERATURE_MIN           -40.0f                           /**< chip min operating temperature */
#define TEMPERATURE_MAX           85.0f                            /**< chip max operating temperature */
#define DRIVER_VERSION            1000                             /**< driver version */

/**
 * @brief chip command definition
 */
#define DS2431_CMD_SEARCH_ROM                 0xF0        /**< search rom command */
#define DS2431_CMD_READ_ROM                   0x33        /**< read rom command */
#define DS2431_CMD_MATCH_ROM                  0x55        /**< match rom command */
#define DS2431_CMD_OVERDRIVE_MATCH_ROM        0x69        /**< overdrive match rom command */
#define DS2431_CMD_SKIP_ROM                   0xCC        /**< skip rom command */
#define DS2431_CMD_OVERDRIVE_SKIP_ROM         0x3C        /**< overdrive skip rom command */
#define DS2431_CMD_RESUME                     0xA5        /**< resume command */
#define DS2431_CMD_WRITE_SCRATCHPAD           0x0F        /**< write scratchpad command */
#define DS2431_CMD_READ_SCRATCHPAD            0xAA        /**< read scratchpad command */
#define DS2431_CMD_COPY_SCRATCHPAD            0x55        /**< copy scratchpad command */
#define DS2431_CMD_READ_MEMORY                0xF0        /**< read memory command */

/**
 * @brief     crc16 update
 * @param[in] input input crc16
 * @param[in] data input data
 * @return    calculated crc16
 * @note      none
 */
static uint16_t a_ds2431_crc16_update(uint16_t input, uint8_t data)
{
    uint8_t i;
    uint16_t crc;
    
    crc = input;                          /* set input */
    for (i = 0; i < 8; i++)               /* 8 bits */
    {
        uint8_t mix;
        
        mix = (crc ^ data) & 0x01;        /* set mix */
        crc >>= 1;                        /* right shift */
        if (mix != 0)                     /* check mix */
        {
            crc ^= 0xA001U;               /* xor */
        }
        data >>= 1;                       /* right shift */
    }
    
    return crc;                           /* return crc */
}

/**
 * @brief     reset the chip
 * @param[in] *handle pointer to a ds2431 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 reset failed
 * @note      none
 */
static uint8_t a_ds2431_reset(ds2431_handle_t *handle)
{
    uint8_t retry = 0;
    uint8_t res;
    
    handle->disable_irq();                                              /* disable irq */
    if (handle->bus_write(0) != 0)                                      /* write 0 */
    {
        handle->enable_irq();                                           /* enable irq */
        handle->debug_print("ds2431: bus write failed.\n");             /* write failed */
        
        return 1;                                                       /* return error */
    }
    handle->delay_us(550);                                              /* wait 550 us */
    if (handle->bus_write(1) != 0)                                      /* write 1 */
    {
        handle->enable_irq();                                           /* enable irq */
        handle->debug_print("ds2431: bus write failed.\n");             /* write failed */
        
        return 1;                                                       /* return error */
    }
    handle->delay_us(15);                                               /* wait 15 us */
    res = 1;                                                            /* reset res */
    while ((res != 0) && (retry < 200))                                 /* wait 200 us */
    {
        if (handle->bus_read((uint8_t *)&res) != 0)                     /* read 1 bit */
        {
            handle->enable_irq();                                       /* enable irq */
            handle->debug_print("ds2431: bus read failed.\n");          /* read failed */
            
            return 1;                                                   /* return error */
        }
        retry++;                                                        /* retry times++ */
        handle->delay_us(1);                                            /* delay 1 us */
    }
    if (retry >= 200)                                                   /* if retry times is over 200 times */
    {
        handle->enable_irq();                                           /* enable irq */
        handle->debug_print("ds2431: bus read no response.\n");         /* no response */
        
        return 1;                                                       /* return error */
    }
    else
    {
        retry = 0;                                                      /* reset retry */
    }
    res = 0;                                                            /* reset res */
    while ((res == 0)&& (retry < 240))                                  /* wait 240 us */
    {
        if (handle->bus_read((uint8_t *)&res) != 0)                     /* read one bit */
        {
            handle->enable_irq();                                       /* enable irq */
            handle->debug_print("ds2431: bus read failed.\n");          /* read failed */
            
            return 1;                                                   /* return error */
        }
        retry++;                                                        /* retry times++ */
        handle->delay_us(1);                                            /* delay 1 us */
    }
    if (retry >= 240)                                                   /* if retry times is over 240 times */
    {
        handle->enable_irq();                                           /* enable irq */
        handle->debug_print("ds2431: bus read no response.\n");         /* no response */
        
        return 1;                                                       /* return error */
    }
    handle->enable_irq();                                               /* enable irq */
    
    return 0;                                                           /* success return 0 */
}

/**
 * @brief      read one bit from the chip
 * @param[in]  *handle pointer to a ds2431 handle structure
 * @param[out] *data pointer to a data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read bit failed
 * @note       none
 */
static uint8_t a_ds2431_read_bit(ds2431_handle_t *handle, uint8_t *data)
{
    if (handle->bus_write(0) != 0)                                  /* write 0 */
    {
        handle->debug_print("ds2431: bus write failed.\n");         /* write failed */
        
        return 1;                                                   /* return error */
    }
    handle->delay_us(2);                                            /* wait 2 us */
    if (handle->bus_write(1) != 0)                                  /* write 1 */
    {
        handle->debug_print("ds2431: bus write failed.\n");         /* write failed */
        
        return 1;                                                   /* return error */
    }
    handle->delay_us(12);                                           /* wait 12 us */
    if (handle->bus_read(data) != 0)                                /* read 1 bit */
    {
        handle->debug_print("ds2431: bus read failed.\n");          /* read failed */
        
        return 1;                                                   /* return error */
    }
    handle->delay_us(50);                                           /* wait 50 us */
    
    return 0;                                                       /* success return 0 */
}

/**
 * @brief      read one byte from the chip
 * @param[in]  *handle pointer to a ds2431 handle structure
 * @param[out] *byte pointer to a byte buffer
 * @return     status code
 *             - 0 success
 *             - 1 read byte failed
 * @note       none
 */
static uint8_t a_ds2431_read_byte(ds2431_handle_t *handle, uint8_t *byte)
{
    uint8_t i, j;
    
    *byte = 0;                                                              /* set byte 0 */
    handle->disable_irq();                                                  /* disable irq */
    for (i = 0; i < 8; i++)                                                 /* 8 bits */
    {
        if (a_ds2431_read_bit(handle, (uint8_t *)&j) != 0)                  /* read 1 bit */
        {
            handle->enable_irq();                                           /* enable irq */
            handle->debug_print("ds2431: bus read byte failed.\n");         /* read byte failed */
            
            return 1;                                                       /* return error */
        }
        *byte = (j << 7) | ((*byte) >> 1);                                  /* set MSB */
    }
    handle->enable_irq();                                                   /* enable irq */
    
    return 0;                                                               /* success return 0 */
}

/**
 * @brief     write one byte to the chip
 * @param[in] *handle pointer to a ds2431 handle structure
 * @param[in] byte written byte
 * @return    status code
 *            - 0 success
 *            - 1 write byte failed
 * @note      none
 */
static uint8_t a_ds2431_write_byte(ds2431_handle_t *handle, uint8_t byte)
{
    uint8_t j;
    uint8_t test_b;
    
    handle->disable_irq();                                                  /* disable irq */
    for (j = 0; j < 8; j++)                                                 /* run 8 times, 8 bits = 1 Byte */
    {
        test_b = byte & 0x01;                                               /* get 1 bit */
        byte = byte >> 1;                                                   /* right shift 1 bit */
        if (test_b != 0)                                                    /* write 1 */
        {
            if (handle->bus_write(0) != 0)                                  /* write 0 */
            {
                handle->enable_irq();                                       /* enable irq */
                handle->debug_print("ds2431: bus write failed.\n");         /* write failed */
                
                return 1;                                                   /* return error */
            }
            handle->delay_us(2);                                            /* wait 2 us */
            if (handle->bus_write(1) != 0)                                  /* write 1 */
            {
                handle->enable_irq();                                       /* enable irq */
                handle->debug_print("ds2431: bus write failed.\n");         /* write failed */
                
                return 1;                                                   /* return error */
            }
            handle->delay_us(65);                                           /* wait 65 us */
        }
        else                                                                /* write 0 */
        {
            if (handle->bus_write(0) != 0)                                  /* write 0 */
            {
                handle->enable_irq();                                       /* enable irq */
                handle->debug_print("ds2431: bus write failed.\n");         /* write failed */
                
                return 1;                                                   /* return error */
            }
            handle->delay_us(65);                                           /* wait 65 us */
            if (handle->bus_write(1) != 0)                                  /* write 1 */
            {
                handle->enable_irq();                                       /* enable irq */
                handle->debug_print("ds2431: bus write failed.\n");         /* write failed */
                
                return 1;                                                   /* return error */
            }
            handle->delay_us(2);                                            /* wait 2 us */
        }
    }
    handle->enable_irq();                                                   /* enable irq */
    
    return 0;                                                               /* success return 0 */
}

/**
 * @brief     reset overdrive
 * @param[in] *handle pointer to a ds2431 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 reset failed
 * @note      none
 */
static uint8_t a_ds2431_reset_overdrive(ds2431_handle_t *handle)
{
    uint8_t retry = 0;
    uint8_t res;
    
    handle->disable_irq();                                              /* disable irq */
    if (handle->bus_write(0) != 0)                                      /* write 0 */
    {
        handle->enable_irq();                                           /* enable irq */
        handle->debug_print("ds2431: bus write failed.\n");             /* write failed */
        
        return 1;                                                       /* return error */
    }
    handle->delay_us(70);                                               /* wait 70 us */
    if (handle->bus_write(1) != 0)                                      /* write 1 */
    {
        handle->enable_irq();                                           /* enable irq */
        handle->debug_print("ds2431: bus write failed.\n");             /* write failed */
        
        return 1;                                                       /* return error */
    }
    handle->delay_us(2);                                                /* wait 2 us */
    res = 1;                                                            /* reset res */
    while ((res != 0) && (retry < 20))                                  /* wait 20 us */
    {
        if (handle->bus_read((uint8_t *)&res) != 0)                     /* read 1 bit */
        {
            handle->enable_irq();                                       /* enable irq */
            handle->debug_print("ds2431: bus read failed.\n");          /* read failed */
            
            return 1;                                                   /* return error */
        }
        retry++;                                                        /* retry times++ */
        handle->delay_us(1);                                            /* delay 1 us */
    }
    if (retry >= 20)                                                    /* if retry times is over 20 times */
    {
        handle->enable_irq();                                           /* enable irq */
        handle->debug_print("ds2431: bus read no response.\n");         /* no response */
        
        return 1;                                                       /* return error */
    }
    else
    {
        retry = 0;                                                      /* reset retry */
    }
    res = 0;                                                            /* reset res */
    while ((res == 0)&& (retry < 20))                                   /* wait 20 us */
    {
        if (handle->bus_read((uint8_t *)&res) != 0)                     /* read one bit */
        {
            handle->enable_irq();                                       /* enable irq */
            handle->debug_print("ds2431: bus read failed.\n");          /* read failed */
            
            return 1;                                                   /* return error */
        }
        retry++;                                                        /* retry times++ */
        handle->delay_us(1);                                            /* delay 1 us */
    }
    if (retry >= 20)                                                    /* if retry times is over 20 times */
    {
        handle->enable_irq();                                           /* enable irq */
        handle->debug_print("ds2431: bus read no response.\n");         /* no response */
        
        return 1;                                                       /* return error */
    }
    handle->enable_irq();                                               /* enable irq */
    
    return 0;                                                           /* success return 0 */
}

/**
 * @brief      read bit overdrive
 * @param[in]  *handle pointer to a ds2431 handle structure
 * @param[out] *data pointer to a data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read bit failed
 * @note       none
 */
static uint8_t a_ds2431_read_bit_overdrive(ds2431_handle_t *handle, uint8_t *data)
{
    if (handle->bus_write(0) != 0)                                  /* write 0 */
    {
        handle->debug_print("ds2431: bus write failed.\n");         /* write failed */
        
        return 1;                                                   /* return error */
    }
    handle->delay_us(1);                                            /* wait 1 us */
    if (handle->bus_write(1) != 0)                                  /* write 1 */
    {
        handle->debug_print("ds2431: bus write failed.\n");         /* write failed */
        
        return 1;                                                   /* return error */
    }
    if (handle->bus_read(data) != 0)                                /* read 1 bit */
    {
        handle->debug_print("ds2431: bus read failed.\n");          /* read failed */
        
        return 1;                                                   /* return error */
    }
    handle->delay_us(10);                                           /* wait 10 us */
    
    return 0;                                                       /* success return 0 */
}

/**
 * @brief      read byte overdrive
 * @param[in]  *handle pointer to a ds2431 handle structure
 * @param[out] *byte pointer to a byte buffer
 * @return     status code
 *             - 0 success
 *             - 1 read byte failed
 * @note       none
 */
static uint8_t a_ds2431_read_byte_overdrive(ds2431_handle_t *handle, uint8_t *byte)
{
    uint8_t i, j;
    
    *byte = 0;                                                              /* set byte 0 */
    handle->disable_irq();                                                  /* disable irq */
    for (i = 0; i < 8; i++)                                                 /* 8 bits */
    {
        if (a_ds2431_read_bit_overdrive(handle, (uint8_t *)&j) != 0)        /* read 1 bit */
        {
            handle->enable_irq();                                           /* enable irq */
            handle->debug_print("ds2431: bus read byte failed.\n");         /* read byte failed */
            
            return 1;                                                       /* return error */
        }
        *byte = (j << 7) | ((*byte) >> 1);                                  /* set MSB */
    }
    handle->enable_irq();                                                   /* enable irq */
    
    return 0;                                                               /* success return 0 */
}

/**
 * @brief     write byte overdrive
 * @param[in] *handle pointer to a ds2431 handle structure
 * @param[in] byte written byte
 * @return    status code
 *            - 0 success
 *            - 1 write byte failed
 * @note      none
 */
static uint8_t a_ds2431_write_byte_overdrive(ds2431_handle_t *handle, uint8_t byte)
{
    uint8_t j;
    uint8_t test_b;
    
    handle->disable_irq();                                                  /* disable irq */
    for (j = 0; j < 8; j++)                                                 /* run 8 times, 8 bits = 1 Byte */
    {
        test_b = byte & 0x01;                                               /* get 1 bit */
        byte = byte >> 1;                                                   /* right shift 1 bit */
        if (test_b != 0)                                                    /* write 1 */
        {
            if (handle->bus_write(0) != 0)                                  /* write 0 */
            {
                handle->enable_irq();                                       /* enable irq */
                handle->debug_print("ds2431: bus write failed.\n");         /* write failed */
                
                return 1;                                                   /* return error */
            }
            handle->delay_us(1);                                            /* wait 1 us */
            if (handle->bus_write(1) != 0)                                  /* write 1 */
            {
                handle->enable_irq();                                       /* enable irq */
                handle->debug_print("ds2431: bus write failed.\n");         /* write failed */
                
                return 1;                                                   /* return error */
            }
            handle->delay_us(10);                                           /* wait 10 us */
        }
        else                                                                /* write 0 */
        {
            if (handle->bus_write(0) != 0)                                  /* write 0 */
            {
                handle->enable_irq();                                       /* enable irq */
                handle->debug_print("ds2431: bus write failed.\n");         /* write failed */
                
                return 1;                                                   /* return error */
            }
            handle->delay_us(10);                                           /* wait 10 us */
            if (handle->bus_write(1) != 0)                                  /* write 1 */
            {
                handle->enable_irq();                                       /* enable irq */
                handle->debug_print("ds2431: bus write failed.\n");         /* write failed */
                
                return 1;                                                   /* return error */
            }
            handle->delay_us(2);                                            /* wait 2 us */
        }
    }
    handle->enable_irq();                                                   /* enable irq */
    
    return 0;                                                               /* success return 0 */
}

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
uint8_t ds2431_set_mode(ds2431_handle_t *handle, ds2431_mode_t mode)
{
    if (handle == NULL)                  /* check handle */
    {
        return 2;                        /* return error */
    }
    if (handle->inited != 1)             /* check handle initialization */
    {
        return 3;                        /* return error */
    }
    
    handle->mode = (uint8_t)mode;        /* set mode */
    
    return 0;                            /* success return 0 */
}

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
uint8_t ds2431_get_mode(ds2431_handle_t *handle, ds2431_mode_t *mode)
{
    if (handle == NULL)                            /* check handle */
    {
        return 2;                                  /* return error */
    }
    if (handle->inited != 1)                       /* check handle initialization */
    {
        return 3;                                  /* return error */
    }
    
    *mode = (ds2431_mode_t)(handle->mode);         /* get mode */
    
    return 0;                                      /* success return 0 */
}

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
uint8_t ds2431_set_rom(ds2431_handle_t *handle, uint8_t rom[8])
{
    if (handle == NULL)                  /* check handle */
    {
        return 2;                        /* return error */
    }
    if (handle->inited != 1)             /* check handle initialization */
    {
        return 3;                        /* return error */
    }
    
    memcpy(handle->rom, rom , 8);        /* copy rom */
    
    return 0;                            /* success return 0 */
}

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
uint8_t ds2431_get_rom(ds2431_handle_t *handle, uint8_t rom[8])
{
    uint8_t i;
    
    if (handle == NULL)                                                 /* check handle */
    {
        return 2;                                                       /* return error */
    }
    if (handle->inited != 1)                                            /* check handle initialization */
    {
        return 3;                                                       /* return error */
    }
    
    if (a_ds2431_reset(handle) != 0)                                    /* reset bus */
    {
        handle->debug_print("ds2431: bus rest failed.\n");              /* reset bus failed */
        
        return 1;                                                       /* return error */
    }
    if (a_ds2431_write_byte(handle, DS2431_CMD_READ_ROM) != 0)          /* write read rom command */
    {
        handle->debug_print("ds2431: write command failed.\n");         /* write command failed */
        
        return 1;                                                       /* return error */
    }
    for (i = 0; i < 8; i++)                                             /* read 8 bytes */
    {
        if (a_ds2431_read_byte(handle, (uint8_t *)&rom[i]) != 0)        /* read 1 byte */
        {
            handle->debug_print("ds2431: read rom failed.\n");          /* read failed */
            
            return 1;                                                   /* return error */
        }
    }
    
    return 0;                                                           /* success return 0 */
}

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
uint8_t ds2431_copy_scratchpad(ds2431_handle_t *handle, uint16_t address)
{
    uint8_t i;
    uint8_t response;
    
    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }
    if (address >= 0x0080)                                                     /* check address */
    {
        handle->debug_print("ds2431: address >= 0x0080.\n");                   /* address >= 0x0080 */
        
        return 4;                                                              /* return error */
    }
    if ((address % 8) != 0)                                                    /* check address */
    {
        handle->debug_print("ds2431: address is invalid.\n");                  /* address is invalid */
        
        return 5;                                                              /* return error */
    }
    
    if (handle->mode == DS2431_MODE_SKIP_ROM)                                  /* skip rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_SKIP_ROM) != 0)             /* send skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte(handle, DS2431_CMD_COPY_SCRATCHPAD) != 0)      /* write copy scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, 0x07) != 0)                            /* write es */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        handle->delay_ms(10);                                                  /* delay 10ms */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xAA)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_SKIP_ROM)                   /* overdrive skip rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_OVERDRIVE_SKIP_ROM) != 0)   /* send overdrive skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_COPY_SCRATCHPAD) != 0)    /* write copy scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle, 0x07) != 0)                  /* write es */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        handle->delay_ms(10);                                                  /* delay 10ms */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xAA)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_MATCH_ROM)                            /* if we use match rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_MATCH_ROM) != 0)            /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }    
        for (i = 0; i < 8; i++)
        {
            if (a_ds2431_write_byte(handle, handle->rom[i]) != 0)              /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        if (a_ds2431_write_byte(handle, DS2431_CMD_COPY_SCRATCHPAD) != 0)      /* write copy scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, 0x07) != 0)                            /* write es */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        handle->delay_ms(10);                                                  /* delay 10ms */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xAA)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_MATCH_ROM)                  /* if overdrive match rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_OVERDRIVE_MATCH_ROM) != 0)  /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }    
        for (i = 0; i < 8; i++)
        {
            if (a_ds2431_write_byte_overdrive(handle, handle->rom[i]) != 0)    /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_COPY_SCRATCHPAD) != 0)    /* write copy scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle, 0x07) != 0)                  /* write es */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        handle->delay_ms(10);                                                  /* delay 10ms */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xAA)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_RESUME)                               /* resume mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_RESUME) != 0)               /* send resume command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte(handle, DS2431_CMD_COPY_SCRATCHPAD) != 0)      /* write copy scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, 0x07) != 0)                            /* write es */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        handle->delay_ms(10);                                                  /* delay 10ms */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xAA)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_RESUME)                     /* if use overdrive resume mode */
    {
        if (a_ds2431_reset_overdrive(handle) != 0)                             /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle, DS2431_CMD_RESUME) != 0)     /* send resume command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_COPY_SCRATCHPAD) != 0)    /* write copy scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle, 0x07) != 0)                  /* write es */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        handle->delay_ms(10);                                                  /* delay 10ms */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xAA)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else
    {
        handle->debug_print("ds2431: mode invalid.\n");                        /* ds2431 mode is invalid */
        
        return 1;                                                              /* return error */
    }
}

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
uint8_t ds2431_write_scratchpad(ds2431_handle_t *handle, uint16_t address, uint8_t data[8], uint16_t *crc16)
{
    uint8_t i;
    uint8_t response;
    uint8_t buf[2];
    uint16_t crc;
    
    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }
    if (address >= 0x0080)                                                     /* check address */
    {
        handle->debug_print("ds2431: address >= 0x0080.\n");                   /* address >= 0x0080 */
        
        return 4;                                                              /* return error */
    }
    if ((address % 8) != 0)                                                    /* check address */
    {
        handle->debug_print("ds2431: address is invalid.\n");                  /* address is invalid */
        
        return 5;                                                              /* return error */
    }
    
    if (handle->mode == DS2431_MODE_SKIP_ROM)                                  /* skip rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_SKIP_ROM) != 0)             /* send skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte(handle, DS2431_CMD_WRITE_SCRATCHPAD) != 0)     /* write scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_WRITE_SCRATCHPAD);         /* calculate part 1 */
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 0) & 0xFF);               /* calculate part 2 */
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 8) & 0xFF);               /* calculate part 3 */
        for (i = 0; i < 8; i++)                                                /* write 8 bytes */
        {
            if (a_ds2431_write_byte(handle, data[i]) != 0)                     /* write data */
            {
                handle->debug_print("ds2431: write data failed.\n");           /* write data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 4 */
        }
        if (a_ds2431_read_byte(handle, &buf[0]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 5 */
        if (a_ds2431_read_byte(handle, &buf[1]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 6 */
        *crc16 = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];                 /* set crc16 */
        *crc16 = ~(*crc16);                                                    /* invert */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 6;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_SKIP_ROM)                   /* overdrive skip rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_OVERDRIVE_SKIP_ROM) != 0)   /* send overdrive skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_WRITE_SCRATCHPAD) != 0)   /* write scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_WRITE_SCRATCHPAD);         /* calculate part 1 */
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 0) & 0xFF);               /* calculate part 2 */
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 8) & 0xFF);               /* calculate part 3 */
        for (i = 0; i < 8; i++)                                                /* write 8 bytes */
        {
            if (a_ds2431_write_byte_overdrive(handle, data[i]) != 0)           /* write data */
            {
                handle->debug_print("ds2431: write data failed.\n");           /* write data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 4 */
        }
        if (a_ds2431_read_byte_overdrive(handle, &buf[0]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 5 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[1]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 6 */
        *crc16 = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];                 /* set crc16 */
        *crc16 = ~(*crc16);                                                    /* invert */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 6;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_MATCH_ROM)                            /* if we use match rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_MATCH_ROM) != 0)            /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }    
        for (i = 0; i < 8; i++)
        {
            if (a_ds2431_write_byte(handle, handle->rom[i]) != 0)              /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte(handle, DS2431_CMD_WRITE_SCRATCHPAD) != 0)     /* write scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_WRITE_SCRATCHPAD);         /* calculate part 1 */
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 0) & 0xFF);               /* calculate part 2 */
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 8) & 0xFF);               /* calculate part 3 */
        for (i = 0; i < 8; i++)                                                /* write 8 bytes */
        {
            if (a_ds2431_write_byte(handle, data[i]) != 0)                     /* write data */
            {
                handle->debug_print("ds2431: write data failed.\n");           /* write data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 4 */
        }
        if (a_ds2431_read_byte(handle, &buf[0]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 5 */
        if (a_ds2431_read_byte(handle, &buf[1]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 6 */
        *crc16 = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];                 /* set crc16 */
        *crc16 = ~(*crc16);                                                    /* invert */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 6;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_MATCH_ROM)                  /* if overdrive match rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_OVERDRIVE_MATCH_ROM) != 0)  /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }    
        for (i = 0; i < 8; i++)
        {
            if (a_ds2431_write_byte_overdrive(handle, handle->rom[i]) != 0)    /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_WRITE_SCRATCHPAD) != 0)   /* write scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_WRITE_SCRATCHPAD);         /* calculate part 1 */
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 0) & 0xFF);               /* calculate part 2 */
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 8) & 0xFF);               /* calculate part 3 */
        for (i = 0; i < 8; i++)                                                /* write 8 bytes */
        {
            if (a_ds2431_write_byte_overdrive(handle, data[i]) != 0)           /* write data */
            {
                handle->debug_print("ds2431: write data failed.\n");           /* write data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 4 */
        }
        if (a_ds2431_read_byte_overdrive(handle, &buf[0]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 5 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[1]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 6 */
        *crc16 = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];                 /* set crc16 */
        *crc16 = ~(*crc16);                                                    /* invert */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 6;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_RESUME)                               /* resume mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_RESUME) != 0)               /* send resume command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte(handle, DS2431_CMD_WRITE_SCRATCHPAD) != 0)     /* write scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_WRITE_SCRATCHPAD);         /* calculate part 1 */
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 0) & 0xFF);               /* calculate part 2 */
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 8) & 0xFF);               /* calculate part 3 */
        for (i = 0; i < 8; i++)                                                /* write 8 bytes */
        {
            if (a_ds2431_write_byte(handle, data[i]) != 0)                     /* write data */
            {
                handle->debug_print("ds2431: write data failed.\n");           /* write data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 4 */
        }
        if (a_ds2431_read_byte(handle, &buf[0]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 5 */
        if (a_ds2431_read_byte(handle, &buf[1]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 6 */
        *crc16 = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];                 /* set crc16 */
        *crc16 = ~(*crc16);                                                    /* invert */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 6;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_RESUME)                     /* if use overdrive resume mode */
    {
        if (a_ds2431_reset_overdrive(handle) != 0)                             /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle, DS2431_CMD_RESUME) != 0)     /* send resume command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_WRITE_SCRATCHPAD) != 0)   /* write scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_WRITE_SCRATCHPAD);         /* calculate part 1 */
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 0) & 0xFF);               /* calculate part 2 */
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 8) & 0xFF);               /* calculate part 3 */
        for (i = 0; i < 8; i++)                                                /* write 8 bytes */
        {
            if (a_ds2431_write_byte_overdrive(handle, data[i]) != 0)           /* write data */
            {
                handle->debug_print("ds2431: write data failed.\n");           /* write data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 4 */
        }
        if (a_ds2431_read_byte_overdrive(handle, &buf[0]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 5 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[1]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 6 */
        *crc16 = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];                 /* set crc16 */
        *crc16 = ~(*crc16);                                                    /* invert */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 6;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else
    {
        handle->debug_print("ds2431: mode invalid.\n");                        /* ds2431 mode is invalid */
        
        return 1;                                                              /* return error */
    }
}

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
uint8_t ds2431_read_scratchpad(ds2431_handle_t *handle, uint16_t *address, uint8_t data[8], uint16_t *crc16)
{
    uint8_t i;
    uint8_t ta;
    uint8_t response;
    uint8_t buf[2];
    uint16_t crc;
    
    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }
    
    if (handle->mode == DS2431_MODE_SKIP_ROM)                                  /* skip rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_SKIP_ROM) != 0)             /* send skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte(handle, DS2431_CMD_READ_SCRATCHPAD) != 0)      /* read scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_READ_SCRATCHPAD);          /* calculate part 1 */
        if (a_ds2431_read_byte(handle, &buf[0]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 2 */
        if (a_ds2431_read_byte(handle, &buf[1]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 3 */
        if (a_ds2431_read_byte(handle, &ta) != 0)                              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, ta);                                  /* calculate part 4 */
        *address = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];               /* set address */
        if (ta != 0x07)                                                        /* check status */
        {
            handle->debug_print("ds2431: status is error.\n");                 /* status is error */
            
            return 4;                                                          /* return error */
        }
        
        for (i = 0; i < 8; i++)                                                /* read 8 bytes */
        {
            if (a_ds2431_read_byte(handle, &data[i]) != 0)                     /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 5 */
        }
        if (a_ds2431_read_byte(handle, &buf[0]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 6 */
        if (a_ds2431_read_byte(handle, &buf[1]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 7 */
        *crc16 = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];                 /* set crc16 */
        *crc16 = ~(*crc16);                                                    /* invert */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 5;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_SKIP_ROM)                   /* overdrive skip rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_OVERDRIVE_SKIP_ROM) != 0)   /* send overdrive skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_READ_SCRATCHPAD) != 0)    /* read scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_READ_SCRATCHPAD);          /* calculate part 1 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[0]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 2 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[1]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 3 */
        if (a_ds2431_read_byte_overdrive(handle, &ta) != 0)                    /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, ta);                                  /* calculate part 4 */
        *address = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];               /* set address */
        if (ta != 0x07)                                                        /* check status */
        {
            handle->debug_print("ds2431: status is error.\n");                 /* status is error */
            
            return 4;                                                          /* return error */
        }
        
        for (i = 0; i < 8; i++)                                                /* read 8 bytes */
        {
            if (a_ds2431_read_byte_overdrive(handle, &data[i]) != 0)           /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 5 */
        }
        if (a_ds2431_read_byte_overdrive(handle, &buf[0]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 6 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[1]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 7 */
        *crc16 = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];                 /* set crc16 */
        *crc16 = ~(*crc16);                                                    /* invert */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 5;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_MATCH_ROM)                            /* if we use match rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_MATCH_ROM) != 0)            /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }    
        for (i = 0; i < 8; i++)
        {
            if (a_ds2431_write_byte(handle, handle->rom[i]) != 0)              /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte(handle, DS2431_CMD_READ_SCRATCHPAD) != 0)      /* read scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_READ_SCRATCHPAD);          /* calculate part 1 */
        if (a_ds2431_read_byte(handle, &buf[0]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 2 */
        if (a_ds2431_read_byte(handle, &buf[1]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 3 */
        if (a_ds2431_read_byte(handle, &ta) != 0)                              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, ta);                                  /* calculate part 4 */
        *address = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];               /* set address */
        if (ta != 0x07)                                                        /* check status */
        {
            handle->debug_print("ds2431: status is error.\n");                 /* status is error */
            
            return 4;                                                          /* return error */
        }
        
        for (i = 0; i < 8; i++)                                                /* read 8 bytes */
        {
            if (a_ds2431_read_byte(handle, &data[i]) != 0)                     /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 5 */
        }
        if (a_ds2431_read_byte(handle, &buf[0]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 6 */
        if (a_ds2431_read_byte(handle, &buf[1]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 7 */
        *crc16 = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];                 /* set crc16 */
        *crc16 = ~(*crc16);                                                    /* invert */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 5;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_MATCH_ROM)                  /* if overdrive match rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_OVERDRIVE_MATCH_ROM) != 0)  /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }    
        for (i = 0; i < 8; i++)
        {
            if (a_ds2431_write_byte_overdrive(handle, handle->rom[i]) != 0)    /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_READ_SCRATCHPAD) != 0)    /* read scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_READ_SCRATCHPAD);          /* calculate part 1 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[0]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 2 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[1]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 3 */
        if (a_ds2431_read_byte_overdrive(handle, &ta) != 0)                    /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, ta);                                  /* calculate part 4 */
        *address = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];               /* set address */
        if (ta != 0x07)                                                        /* check status */
        {
            handle->debug_print("ds2431: status is error.\n");                 /* status is error */
            
            return 4;                                                          /* return error */
        }
        
        for (i = 0; i < 8; i++)                                                /* read 8 bytes */
        {
            if (a_ds2431_read_byte_overdrive(handle, &data[i]) != 0)           /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 5 */
        }
        if (a_ds2431_read_byte_overdrive(handle, &buf[0]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 6 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[1]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 7 */
        *crc16 = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];                 /* set crc16 */
        *crc16 = ~(*crc16);                                                    /* invert */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 5;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_RESUME)                               /* resume mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_RESUME) != 0)               /* send resume command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte(handle, DS2431_CMD_READ_SCRATCHPAD) != 0)      /* read scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_READ_SCRATCHPAD);          /* calculate part 1 */
        if (a_ds2431_read_byte(handle, &buf[0]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 2 */
        if (a_ds2431_read_byte(handle, &buf[1]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 3 */
        if (a_ds2431_read_byte(handle, &ta) != 0)                              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, ta);                                  /* calculate part 4 */
        *address = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];               /* set address */
        if (ta != 0x07)                                                        /* check status */
        {
            handle->debug_print("ds2431: status is error.\n");                 /* status is error */
            
            return 4;                                                          /* return error */
        }
        
        for (i = 0; i < 8; i++)                                                /* read 8 bytes */
        {
            if (a_ds2431_read_byte(handle, &data[i]) != 0)                     /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 5 */
        }
        if (a_ds2431_read_byte(handle, &buf[0]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 6 */
        if (a_ds2431_read_byte(handle, &buf[1]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 7 */
        *crc16 = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];                 /* set crc16 */
        *crc16 = ~(*crc16);                                                    /* invert */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 5;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_RESUME)                     /* if use overdrive resume mode */
    {
        if (a_ds2431_reset_overdrive(handle) != 0)                             /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle, DS2431_CMD_RESUME) != 0)     /* send resume command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_READ_SCRATCHPAD) != 0)    /* read scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_READ_SCRATCHPAD);          /* calculate part 1 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[0]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 2 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[1]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 3 */
        if (a_ds2431_read_byte_overdrive(handle, &ta) != 0)                    /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, ta);                                  /* calculate part 4 */
        *address = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];               /* set address */
        if (ta != 0x07)                                                        /* check status */
        {
            handle->debug_print("ds2431: status is error.\n");                 /* status is error */
            
            return 4;                                                          /* return error */
        }
        
        for (i = 0; i < 8; i++)                                                /* read 8 bytes */
        {
            if (a_ds2431_read_byte_overdrive(handle, &data[i]) != 0)           /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 5 */
        }
        if (a_ds2431_read_byte_overdrive(handle, &buf[0]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 6 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[1]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 7 */
        *crc16 = (uint16_t)((uint16_t)(buf[1]) << 8) | buf[0];                 /* set crc16 */
        *crc16 = ~(*crc16);                                                    /* invert */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 5;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else
    {
        handle->debug_print("ds2431: mode invalid.\n");                        /* ds2431 mode is invalid */
        
        return 1;                                                              /* return error */
    }
}

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
uint8_t ds2431_read_memory(ds2431_handle_t *handle, uint16_t address, uint8_t *data, uint16_t len)
{
    uint16_t i;
    
    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }
    if ((address + len) > 0x0080)                                              /* check address */
    {
        handle->debug_print("ds2431: address and len are invalid.\n");         /* address and len are invalid */
        
        return 4;                                                              /* return error */
    }
    
    if (handle->mode == DS2431_MODE_SKIP_ROM)                                  /* skip rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_SKIP_ROM) != 0)             /* send skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte(handle, DS2431_CMD_READ_MEMORY) != 0)          /* read memory command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        for (i = 0; i < len; i++)                                              /* loop */
        {
            if (a_ds2431_read_byte(handle, &data[i]) != 0)                     /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_SKIP_ROM)                   /* overdrive skip rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_OVERDRIVE_SKIP_ROM) != 0)   /* send overdrive skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_READ_MEMORY) != 0)        /* read memory command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        for (i = 0; i < len; i++)                                              /* loop */
        {
            if (a_ds2431_read_byte_overdrive(handle, &data[i]) != 0)           /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_MATCH_ROM)                            /* if we use match rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_MATCH_ROM) != 0)            /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }    
        for (i = 0; i < 8; i++)
        {
            if (a_ds2431_write_byte(handle, handle->rom[i]) != 0)              /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        if (a_ds2431_write_byte(handle, DS2431_CMD_READ_MEMORY) != 0)          /* read memory command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        for (i = 0; i < len; i++)                                              /* loop */
        {
            if (a_ds2431_read_byte(handle, &data[i]) != 0)                     /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_MATCH_ROM)                  /* if overdrive match rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_OVERDRIVE_MATCH_ROM) != 0)  /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }    
        for (i = 0; i < 8; i++)
        {
            if (a_ds2431_write_byte_overdrive(handle, handle->rom[i]) != 0)    /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_READ_MEMORY) != 0)        /* read memory command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        for (i = 0; i < len; i++)                                              /* loop */
        {
            if (a_ds2431_read_byte_overdrive(handle, &data[i]) != 0)           /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_RESUME)                               /* resume mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_RESUME) != 0)               /* send resume command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte(handle, DS2431_CMD_READ_MEMORY) != 0)          /* read memory command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        for (i = 0; i < len; i++)                                              /* loop */
        {
            if (a_ds2431_read_byte(handle, &data[i]) != 0)                     /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_RESUME)                     /* if use overdrive resume mode */
    {
        if (a_ds2431_reset_overdrive(handle) != 0)                             /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle, DS2431_CMD_RESUME) != 0)     /* send resume command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_READ_MEMORY) != 0)        /* read memory command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        for (i = 0; i < len; i++)                                              /* loop */
        {
            if (a_ds2431_read_byte_overdrive(handle, &data[i]) != 0)           /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
        }
        
        return 0;                                                              /* success return 0 */
    }
    else
    {
        handle->debug_print("ds2431: mode invalid.\n");                        /* ds2431 mode is invalid */
        
        return 1;                                                              /* return error */
    }
}

/**
 * @brief      ds2431 read
 * @param[in]  *handle pointer to a ds2431 handle structure
 * @param[in]  address input address
 * @param[out] *data pointer to a data buffer
 * @param[in]  len data length
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
static uint8_t a_ds2431_read(ds2431_handle_t *handle, uint16_t address, uint8_t *data, uint16_t len)
{
    uint16_t i;
    
    if (handle->mode == DS2431_MODE_SKIP_ROM)                                  /* skip rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_SKIP_ROM) != 0)             /* send skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte(handle, DS2431_CMD_READ_MEMORY) != 0)          /* read memory command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        for (i = 0; i < len; i++)                                              /* loop */
        {
            if (a_ds2431_read_byte(handle, &data[i]) != 0)                     /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_SKIP_ROM)                   /* overdrive skip rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_OVERDRIVE_SKIP_ROM) != 0)   /* send overdrive skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_READ_MEMORY) != 0)        /* read memory command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        for (i = 0; i < len; i++)                                              /* loop */
        {
            if (a_ds2431_read_byte_overdrive(handle, &data[i]) != 0)           /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_MATCH_ROM)                            /* if we use match rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_MATCH_ROM) != 0)            /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }    
        for (i = 0; i < 8; i++)                                                /* write rom */
        {
            if (a_ds2431_write_byte(handle, handle->rom[i]) != 0)              /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        if (a_ds2431_write_byte(handle, DS2431_CMD_READ_MEMORY) != 0)          /* read memory command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        for (i = 0; i < len; i++)                                              /* loop */
        {
            if (a_ds2431_read_byte(handle, &data[i]) != 0)                     /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_MATCH_ROM)                  /* if overdrive match rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_OVERDRIVE_MATCH_ROM) != 0)  /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }    
        for (i = 0; i < 8; i++)                                                /* write rom */
        {
            if (a_ds2431_write_byte_overdrive(handle, handle->rom[i]) != 0)    /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_READ_MEMORY) != 0)        /* read memory command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        for (i = 0; i < len; i++)                                              /* loop */
        {
            if (a_ds2431_read_byte_overdrive(handle, &data[i]) != 0)           /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_RESUME)                               /* resume mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_RESUME) != 0)               /* send resume command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte(handle, DS2431_CMD_READ_MEMORY) != 0)          /* read memory command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        for (i = 0; i < len; i++)                                              /* loop */
        {
            if (a_ds2431_read_byte(handle, &data[i]) != 0)                     /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_RESUME)                     /* if use overdrive resume mode */
    {
        if (a_ds2431_reset_overdrive(handle) != 0)                             /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle, DS2431_CMD_RESUME) != 0)     /* send resume command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_READ_MEMORY) != 0)        /* read memory command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        for (i = 0; i < len; i++)                                              /* loop */
        {
            if (a_ds2431_read_byte_overdrive(handle, &data[i]) != 0)           /* read data */
            {
                handle->debug_print("ds2431: read data failed.\n");            /* read data failed */
                
                return 1;                                                      /* return error */
            }
        }
        
        return 0;                                                              /* success return 0 */
    }
    else
    {
        handle->debug_print("ds2431: mode invalid.\n");                        /* ds2431 mode is invalid */
        
        return 1;                                                              /* return error */
    }
}

/**
 * @brief     ds2431 write
 * @param[in] *handle pointer to a ds2431 handle structure
 * @param[in] address input address
 * @param[in] *data pointer to a data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
static uint8_t a_ds2431_write(ds2431_handle_t *handle, uint16_t address, uint8_t data[8])
{
    uint8_t i;
    uint8_t response;
    uint8_t buf[2];
    uint16_t crc;
    
    if (handle->mode == DS2431_MODE_SKIP_ROM)                                  /* skip rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_SKIP_ROM) != 0)             /* send skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte(handle, DS2431_CMD_WRITE_SCRATCHPAD) != 0)     /* write scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_WRITE_SCRATCHPAD);         /* calculate part 1 */
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 0) & 0xFF);               /* calculate part 2 */
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 8) & 0xFF);               /* calculate part 3 */
        for (i = 0; i < 8; i++)                                                /* write 8 bytes */
        {
            if (a_ds2431_write_byte(handle, data[i]) != 0)                     /* write data */
            {
                handle->debug_print("ds2431: write data failed.\n");           /* write data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 4 */
        }
        if (a_ds2431_read_byte(handle, &buf[0]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 5 */
        if (a_ds2431_read_byte(handle, &buf[1]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 6 */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_SKIP_ROM) != 0)             /* send skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte(handle, DS2431_CMD_COPY_SCRATCHPAD) != 0)      /* write copy scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, 0x07) != 0)                            /* write es */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        handle->delay_ms(10);                                                  /* delay 10ms */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xAA)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_SKIP_ROM)                   /* overdrive skip rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_OVERDRIVE_SKIP_ROM) != 0)   /* send overdrive skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_WRITE_SCRATCHPAD) != 0)   /* write scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_WRITE_SCRATCHPAD);         /* calculate part 1 */
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 0) & 0xFF);               /* calculate part 2 */
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 8) & 0xFF);               /* calculate part 3 */
        for (i = 0; i < 8; i++)                                                /* write 8 bytes */
        {
            if (a_ds2431_write_byte_overdrive(handle, data[i]) != 0)           /* write data */
            {
                handle->debug_print("ds2431: write data failed.\n");           /* write data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 4 */
        }
        if (a_ds2431_read_byte_overdrive(handle, &buf[0]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 5 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[1]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 6 */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_reset_overdrive(handle) != 0)                             /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_OVERDRIVE_SKIP_ROM) != 0) /* send overdrive skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_COPY_SCRATCHPAD) != 0)    /* write copy scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle, 0x07) != 0)                  /* write es */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        handle->delay_ms(10);                                                  /* delay 10ms */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xAA)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_MATCH_ROM)                            /* if we use match rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_MATCH_ROM) != 0)            /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }    
        for (i = 0; i < 8; i++)                                                /* write rom */
        {
            if (a_ds2431_write_byte(handle, handle->rom[i]) != 0)              /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte(handle, DS2431_CMD_WRITE_SCRATCHPAD) != 0)     /* write scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_WRITE_SCRATCHPAD);         /* calculate part 1 */
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 0) & 0xFF);               /* calculate part 2 */
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 8) & 0xFF);               /* calculate part 3 */
        for (i = 0; i < 8; i++)                                                /* write 8 bytes */
        {
            if (a_ds2431_write_byte(handle, data[i]) != 0)                     /* write data */
            {
                handle->debug_print("ds2431: write data failed.\n");           /* write data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 4 */
        }
        if (a_ds2431_read_byte(handle, &buf[0]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 5 */
        if (a_ds2431_read_byte(handle, &buf[1]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 6 */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_MATCH_ROM) != 0)            /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        for (i = 0; i < 8; i++)                                                /* write rom */
        {
            if (a_ds2431_write_byte(handle, handle->rom[i]) != 0)              /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        if (a_ds2431_write_byte(handle, DS2431_CMD_COPY_SCRATCHPAD) != 0)      /* write copy scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, 0x07) != 0)                            /* write es */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        handle->delay_ms(10);                                                  /* delay 10ms */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xAA)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_MATCH_ROM)                  /* if overdrive match rom mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_OVERDRIVE_MATCH_ROM) != 0)  /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }    
        for (i = 0; i < 8; i++)                                                /* write rom */
        {
            if (a_ds2431_write_byte_overdrive(handle, handle->rom[i]) != 0)    /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_WRITE_SCRATCHPAD) != 0)   /* write scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_WRITE_SCRATCHPAD);         /* calculate part 1 */
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 0) & 0xFF);               /* calculate part 2 */
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 8) & 0xFF);               /* calculate part 3 */
        for (i = 0; i < 8; i++)                                                /* write 8 bytes */
        {
            if (a_ds2431_write_byte_overdrive(handle, data[i]) != 0)           /* write data */
            {
                handle->debug_print("ds2431: write data failed.\n");           /* write data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 4 */
        }
        if (a_ds2431_read_byte_overdrive(handle, &buf[0]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 5 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[1]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 6 */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_reset_overdrive(handle) != 0)                             /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_OVERDRIVE_SKIP_ROM) != 0) /* send overdrive skip rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_COPY_SCRATCHPAD) != 0)    /* write copy scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle, 0x07) != 0)                  /* write es */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        handle->delay_ms(10);                                                  /* delay 10ms */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xAA)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_RESUME)                               /* resume mode */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_RESUME) != 0)               /* send resume command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte(handle, DS2431_CMD_WRITE_SCRATCHPAD) != 0)     /* write scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_WRITE_SCRATCHPAD);         /* calculate part 1 */
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 0) & 0xFF);               /* calculate part 2 */
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 8) & 0xFF);               /* calculate part 3 */
        for (i = 0; i < 8; i++)                                                /* write 8 bytes */
        {
            if (a_ds2431_write_byte(handle, data[i]) != 0)                     /* write data */
            {
                handle->debug_print("ds2431: write data failed.\n");           /* write data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 4 */
        }
        if (a_ds2431_read_byte(handle, &buf[0]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 5 */
        if (a_ds2431_read_byte(handle, &buf[1]) != 0)                          /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 6 */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_RESUME) != 0)               /* send resume command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte(handle, DS2431_CMD_COPY_SCRATCHPAD) != 0)      /* write copy scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 0) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, (address >> 8) & 0xFF) != 0)           /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, 0x07) != 0)                            /* write es */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        handle->delay_ms(10);                                                  /* delay 10ms */
        if (a_ds2431_read_byte(handle, &response) != 0)                        /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xAA)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else if (handle->mode == DS2431_MODE_OVERDRIVE_RESUME)                     /* if use overdrive resume mode */
    {
        if (a_ds2431_reset_overdrive(handle) != 0)                             /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle, DS2431_CMD_RESUME) != 0)     /* send resume command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        crc = 0;                                                               /* crc init 0 */
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_WRITE_SCRATCHPAD) != 0)   /* write scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, DS2431_CMD_WRITE_SCRATCHPAD);         /* calculate part 1 */
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 0) & 0xFF);               /* calculate part 2 */
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, (address >> 8) & 0xFF);               /* calculate part 3 */
        for (i = 0; i < 8; i++)                                                /* write 8 bytes */
        {
            if (a_ds2431_write_byte_overdrive(handle, data[i]) != 0)           /* write data */
            {
                handle->debug_print("ds2431: write data failed.\n");           /* write data failed */
                
                return 1;                                                      /* return error */
            }
            crc = a_ds2431_crc16_update(crc, data[i]);                         /* calculate part 4 */
        }
        if (a_ds2431_read_byte_overdrive(handle, &buf[0]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[0]);                              /* calculate part 5 */
        if (a_ds2431_read_byte_overdrive(handle, &buf[1]) != 0)                /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        crc = a_ds2431_crc16_update(crc, buf[1]);                              /* calculate part 6 */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xFF)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        if (crc != 0xB001U)                                                    /* check crc16 */
        {
            handle->debug_print("ds2431: crc16 check error.\n");               /* crc16 check error */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_reset_overdrive(handle) != 0)                             /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* reset bus failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle, DS2431_CMD_RESUME) != 0)     /* send resume command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        
        if (a_ds2431_write_byte_overdrive(handle,
                                          DS2431_CMD_COPY_SCRATCHPAD) != 0)    /* write copy scratchpad command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 0) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle,
                                         (address >> 8) & 0xFF) != 0)          /* write address lsb */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte_overdrive(handle, 0x07) != 0)                  /* write es */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }
        handle->delay_ms(10);                                                  /* delay 10ms */
        if (a_ds2431_read_byte_overdrive(handle, &response) != 0)              /* read byte */
        {
            handle->debug_print("ds2431: read data failed.\n");                /* read data failed */
            
            return 1;                                                          /* return error */
        }
        if (response != 0xAA)                                                  /* check response */
        {
            handle->debug_print("ds2431: response error.\n");                  /* response error */
            
            return 1;                                                          /* return error */
        }
        
        return 0;                                                              /* success return 0 */
    }
    else
    {
        handle->debug_print("ds2431: mode invalid.\n");                        /* ds2431 mode is invalid */
        
        return 1;                                                              /* return error */
    }
}

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
uint8_t ds2431_read_memory_config(ds2431_handle_t *handle, ds2431_config_control_t *config)
{
    uint8_t res;
    uint8_t buf[8];
    
    if (handle == NULL)                               /* check handle */
    {
        return 2;                                     /* return error */
    }
    if (handle->inited != 1)                          /* check handle initialization */
    {
        return 3;                                     /* return error */
    }
    
    res = a_ds2431_read(handle, 0x80, buf, 8);        /* read config */
    if (res != 0)                                     /* check the result */
    {
        return 1;                                     /* return error */
    }
    config->page0_protection_control = buf[0];        /* set page0 protection control */
    config->page1_protection_control = buf[1];        /* set page1 protection control */
    config->page2_protection_control = buf[2];        /* set page2 protection control */
    config->page3_protection_control = buf[3];        /* set page3 protection control */
    config->copy_protection = buf[4];                 /* set copy protection */
    config->factory_byte = buf[5];                    /* set factory byte */
    config->user_byte_0 = buf[6];                     /* set user byte 0 */
    config->user_byte_1 = buf[7];                     /* set user byte 1 */
    
    return 0;                                         /* success return 0 */
}

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
uint8_t ds2431_write_memory_config(ds2431_handle_t *handle, ds2431_config_control_t *config)
{
    uint8_t res;
    uint8_t buf[8];
    
    if (handle == NULL)                               /* check handle */
    {
        return 2;                                     /* return error */
    }
    if (handle->inited != 1)                          /* check handle initialization */
    {
        return 3;                                     /* return error */
    }
    
    buf[0] = config->page0_protection_control;        /* set page0 protection control */
    buf[1] = config->page1_protection_control;        /* set page1 protection control */
    buf[2] = config->page2_protection_control;        /* set page2 protection control */
    buf[3] = config->page3_protection_control;        /* set page3 protection control */
    buf[4] = config->copy_protection;                 /* set copy protection */
    buf[5] = config->factory_byte;                    /* set factory byte */
    buf[6] = config->user_byte_0;                     /* set user byte 0 */
    buf[7] = config->user_byte_1;                     /* set user byte 1 */
    res = a_ds2431_write(handle, 0x80, buf);          /* write config */
    if (res != 0)                                     /* check the result */
    {
        return 1;                                     /* return error */
    }
    
    return 0;                                         /* success return 0 */
}

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
uint8_t ds2431_read(ds2431_handle_t *handle, uint8_t address, uint8_t *data, uint8_t len)
{
    uint8_t res;
    
    if (handle == NULL)                                                       /* check handle */
    {
        return 2;                                                             /* return error */
    }
    if (handle->inited != 1)                                                  /* check handle initialization */
    {
        return 3;                                                             /* return error */
    }
    if ((address + len) > 0x80)                                               /* check address */
    {
        handle->debug_print("ds2431: address and len are invalid.\n");        /* address and len are invalid */
        
        return 4;                                                             /* return error */
    }
    
    res = a_ds2431_read(handle, address, data, len);                          /* read data */
    if (res != 0)                                                             /* check the result */
    {
        return 1;                                                             /* return error */
    }
    
    return 0;                                                                 /* success return 0 */
}

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
uint8_t ds2431_write(ds2431_handle_t *handle, uint8_t address, uint8_t *data, uint8_t len)
{
    uint8_t res;
    uint8_t i;
    uint32_t pos;
    uint32_t off;
    uint32_t remain;
    uint8_t buffer[8 + 1];
    
    if (handle == NULL)                                                       /* check handle */
    {
        return 2;                                                             /* return error */
    }
    if (handle->inited != 1)                                                  /* check handle initialization */
    {
        return 3;                                                             /* return error */
    }
    if ((address + len) > 0x80)                                               /* check address */
    {
        handle->debug_print("ds2431: address and len are invalid.\n");        /* address and len are invalid */
        
        return 4;                                                             /* return error */
    }
    
    pos = address / 8;                                                        /* set pos */
    off = address % 8;                                                        /* set off */
    remain = 8 - off;                                                         /* set remain */
    if (len <= remain)                                                        /* check length */
    {
        remain = len;                                                         /* set remain */
    }
    while(1)                                                                  /* loop */
    {    
        if (remain != 8)                                                      /* check remain */
        {
            res = a_ds2431_read(handle, pos * 8, buffer, 8);                  /* read data */
            if (res != 0)                                                     /* check the result */
            {
                return 1;                                                     /* return error */
            }
            for (i = 0; i < remain; i++)                                      /* write remain */
            {
                buffer[i + off] = data[i];                                    /* copy data */
            }
            res = a_ds2431_write(handle, pos * 8, buffer);                    /* write data */
            if (res != 0)                                                     /* check the result */
            {
                return 1;                                                     /* return error */
            }
        }
        else
        {
            res = a_ds2431_write(handle, address, data);                      /* write data */
            if (res != 0)                                                     /* check the result */
            {
                return 1;                                                     /* return error */
            }
        } 
   
        if (len == remain)                                                    /* check length length*/
        {
            break;                                                            /* break loop */
        }
        else
        {
            pos++;                                                            /* position++ */
            off = 0;                                                          /* set offset */
            data += remain;                                                   /* data + remain */
            address += remain;                                                /* addr + remain */
            len -= remain;                                                    /* len - remain */
            if (len > 8)                                                      /* check length */
            {
                remain = 8;                                                   /* set 8 */
            }
            else
            {
                remain = len;                                                 /* set length */
            }
        }
    }
    
    return 0;                                                                 /* success return 0 */
}

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
uint8_t ds2431_rom_match(ds2431_handle_t *handle, ds2431_type_t type, uint8_t rom[8])
{
    uint8_t i;
    
    if (handle == NULL)                                                        /* check handle */
    {
        return 2;                                                              /* return error */
    }
    if (handle->inited != 1)                                                   /* check handle initialization */
    {
        return 3;                                                              /* return error */
    }
    
    if (type == DS2431_TYPE_MATCH_ROM)                                         /* match rom */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_MATCH_ROM) != 0)            /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }    
        for (i = 0; i < 8; i++)
        {
            if (a_ds2431_write_byte(handle, rom[i]) != 0)                      /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
    }
    else                                                                       /* overdrive match rom */
    {
        if (a_ds2431_reset(handle) != 0)                                       /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
        if (a_ds2431_write_byte(handle, DS2431_CMD_OVERDRIVE_MATCH_ROM) != 0)  /* write match rom command */
        {
            handle->debug_print("ds2431: write command failed.\n");            /* write command failed */
            
            return 1;                                                          /* return error */
        }    
        for (i = 0; i < 8; i++)                                                /* write rom */
        {
            if (a_ds2431_write_byte_overdrive(handle, rom[i]) != 0)            /* send rom */
            {
                handle->debug_print("ds2431: write command failed.\n");        /* write command */
                
                return 1;                                                      /* return error */
            }
        }
        
        if (a_ds2431_reset_overdrive(handle) != 0)                             /* reset bus */
        {
            handle->debug_print("ds2431: bus reset failed.\n");                /* bus reset failed */
            
            return 1;                                                          /* return error */
        }
    }
    
    return 0;                                                                  /* success return 0 */
}

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
uint8_t ds2431_init(ds2431_handle_t *handle)
{
    if (handle == NULL)                                                /* check handle */
    {
        return 2;                                                      /* return error */
    }
    if (handle->debug_print == NULL)                                   /* check debug_print */
    {
        return 3;                                                      /* return error */
    }
    if (handle->bus_init == NULL)                                      /* check bus_init */
    {
        handle->debug_print("ds2431: bus_init is null.\n");            /* bus_init is null */
        
        return 3;                                                      /* return error */
    }
    if (handle->bus_deinit == NULL)                                    /* check bus_deinit */
    {
        handle->debug_print("ds2431: bus_deinit is null.\n");          /* bus_read is null */
        
        return 3;                                                      /* return error */
    }
    if (handle->bus_read == NULL)                                      /* check bus_read */
    {
        handle->debug_print("ds2431: bus_read is null.\n");            /* bus_read is null */
        
        return 3;                                                      /* return error */
    }
    if (handle->bus_write == NULL)                                     /* check bus_write */
    {
        handle->debug_print("ds2431: bus_write is null.\n");           /* bus_write is null */
        
        return 3;                                                      /* return error */
    }
    if (handle->delay_ms == NULL)                                      /* check delay_ms */
    {
        handle->debug_print("ds2431: delay_ms is null.\n");            /* delay_ms is null */
        
        return 3;                                                      /* return error */
    }
    if (handle->delay_us == NULL)                                      /* check delay_us */
    {
        handle->debug_print("ds2431: delay_us is null.\n");            /* delay_us is null */
       
        return 3;                                                      /* return error */
    }
    if (handle->enable_irq == NULL)                                    /* check enable_irq */
    {
        handle->debug_print("ds2431: enable_irq is null.\n");          /* enable_irq is null */
        
        return 3;                                                      /* return error */
    }
    if (handle->disable_irq == NULL)                                   /* check disable_irq */
    {
        handle->debug_print("ds2431: disable_irq is null.\n");         /* disable_irq is null */
        
        return 3;                                                      /* return error */
    }
    
    if (handle->bus_init() != 0)                                       /* initialize bus */
    {
        handle->debug_print("ds2431: bus init failed.\n");             /* bus innit failed */
        
        return 1;                                                      /* return error */
    }
    if (a_ds2431_reset(handle) != 0)                                   /* reset chip */
    {
        handle->debug_print("ds2431: reset failed.\n");                /* reset chip failed */
        (void)handle->bus_deinit();                                    /* close bus */
        
        return 4;                                                      /* return error */
    }
    handle->inited = 1;                                                /* flag finish initialization */
    
    return 0;                                                          /* success return 0 */
}

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
uint8_t ds2431_deinit(ds2431_handle_t *handle)
{
    if (handle == NULL)                                          /* check handle */
    {
        return 2;                                                /* return error */
    }
    if (handle->inited != 1)                                     /* check handle initialization */
    {
        return 3;                                                /* return error */
    }
    
    if (handle->bus_deinit() != 0)                               /* close bus */
    {
        handle->debug_print("ds2431: deinit failed.\n");         /* deinit failed */
        
        return 1;                                                /* return error */
    }   
    handle->inited = 0;                                          /* flag close */
    
    return 0;                                                    /* success return 0 */
}

/**
 * @brief      read 2 bits from the bus
 * @param[in]  *handle pointer to a ds2431 handle structure
 * @param[out] *data pointer to a data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
static uint8_t a_ds2431_read_2bit(ds2431_handle_t *handle, uint8_t *data)    
{
    uint8_t i;
    uint8_t res;
    
    *data = 0;                                                          /* reset data */
    handle->disable_irq();                                              /* disable irq */
    for (i = 0; i < 2; i++)                                             /* read 2 bit */
    {
        *data <<= 1;                                                    /* left shift 1 */
        if (a_ds2431_read_bit(handle, (uint8_t *)&res) != 0)            /* read one bit */
        {
            handle->enable_irq();                                       /* enable irq */
            handle->debug_print("ds2431: read bit failed.\n");          /* read a bit failed */
            
            return 1;                                                   /* return error */
        }
        *data = (*data) | res;                                          /* get 1 bit */
    }
    handle->enable_irq();                                               /* enable irq */
    
    return 0;                                                           /* success return 0 */
}

/**
 * @brief     write 1 bit to the bus
 * @param[in] *handle pointer to a ds2431 handle structure
 * @param[in] bit written bit
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
static uint8_t a_ds2431_write_bit(ds2431_handle_t *handle, uint8_t bit)
{    
    handle->disable_irq();                                          /* disable irq */
    if (handle->bus_write(0) != 0)                                  /* write 0 */
    {
        handle->enable_irq();                                       /* enable irq */
        handle->debug_print("ds2431: write bit failed.\n");         /* write bit failed */
        
        return 1;                                                   /* return error */
    }
    handle->delay_us(12);                                           /* wait 12 us */
    if (handle->bus_write(bit) != 0)                                /* write bit */
    {
        handle->enable_irq();                                       /* enable irq */
        handle->debug_print("ds2431: write bit failed.\n");         /* write bit failed */
        
        return 1;                                                   /* return error */
    } 
    handle->delay_us(30);                                           /* wait 30 us */
    if (handle->bus_write(1) != 0)                                  /* write 1 */
    {
        handle->enable_irq();                                       /* enable irq */
        handle->debug_print("ds2431: write bit failed.\n");         /* write bit failed */
        
        return 1;                                                   /* return error */
    }
    handle->delay_us(5);                                            /* wait 5 us */
    handle->enable_irq();                                           /* enable irq */
    
    return 0;                                                       /* success return 0 */
}

/**
 * @brief         search the ds2431 bus
 * @param[in]     *handle pointer to a ds2431 handle structure
 * @param[out]    **pid pointer to a rom array
 * @param[in]     cmd command
 * @param[in,out] *number pointer to an array size buffer
 * @return        status code
 *                - 0 success
 *                - 1 search failed
 * @note          none
 */
static uint8_t a_ds2431_search(ds2431_handle_t *handle, uint8_t (*pid)[8], uint8_t cmd, uint8_t *number)
{     
    uint8_t k, l = 0, conflict_bit, m, n;
    uint8_t buffer[DS2431_MAX_SEARCH_SIZE];
    uint8_t ss[64];
    uint8_t s = 0;
    uint8_t num = 0;
    
    if ((*number) > DS2431_MAX_SEARCH_SIZE)                                               /* check number */
    {
        handle->debug_print("ds2431: number is over DS2431_MAX_SEARCH_SIZE.\n");          /* number is over */
        
        return 1;                                                                         /* return error */
    }
    memset((uint8_t *)buffer, 0, DS2431_MAX_SEARCH_SIZE);                                 /* clear buffer */
    memset((uint8_t *)ss, 0, sizeof(uint8_t) * 64);                                       /* clear buffer */
    do
    {
        if (a_ds2431_reset(handle) != 0)                                                  /* reset bus */
        {
            handle->debug_print("ds2431: reset failed.\n");                               /* reset bus failed */
            
            return 1;                                                                     /* return error */
        }
        if (a_ds2431_write_byte(handle, cmd) != 0)                                        /* write 1 byte */
        {
            handle->debug_print("ds2431: write command failed.\n");                       /* write command failed */
            
            return 1;                                                                     /* return error */
        }
        for (m = 0; m < 8; m++)                                                           /* read 8 byte */
        {
            for (n = 0; n < 8; n++)                                                       /* read 8 bit */
            {
                if (a_ds2431_read_2bit(handle, (uint8_t *)&k) != 0)                       /* read 2 bit */
                {
                    handle->debug_print("ds2431: read 2bit failed.\n");                   /* read 2 bit failed */
                    
                    return 1;                                                             /* return error */
                }
                k = k & 0x03;                                                             /* get valid bits */
                s = s >> 1;                                                               /* right shift 1 bit */
                if (k == 0x01)                                                            /* 0000 0001 */
                {
                    if (a_ds2431_write_bit(handle, 0) != 0)                               /* write 0 */
                    {
                        handle->debug_print("ds2431: write bit failed.\n");               /* write bit failed */
                        
                        return 1;                                                         /* return error */
                    }
                    ss[(m * 8 + n)] = 0;                                                  /* set 0 */
                }
                else if (k == 0x02)                                                       /* 0000 0010 */
                {
                    s = s | 0x80;                                                         /* set 7 bit */
                    if (a_ds2431_write_bit(handle, 1) != 0)                               /* write 1 */
                    {
                        handle->debug_print("ds2431: write bit failed.\n");               /* write bit failed */
                        
                        return 1;                                                         /* return error */
                    }
                    ss[(m * 8 + n)] = 1;                                                  /* set 1 */
                }    
                else if (k == 0x00)                                                       /* if 0000 */
                {
                    conflict_bit = (uint8_t)(m * 8 + n + 1);                              /* flag conflict bit */
                    if (conflict_bit > buffer[l])                                         /* check buffer */
                    {
                        if (a_ds2431_write_bit(handle, 0) != 0)                           /* write 0 */
                        {
                            handle->debug_print("ds2431: write bit failed.\n");           /* write bit failed */
                            
                            return 1;                                                     /* return error */
                        }
                        ss[(m * 8 + n)] = 0;                                              /* set 0 */
                        buffer[++l] = conflict_bit;                                       /* set conflict bit */
                    }
                    else if (conflict_bit < buffer[l])                                    /* if > buffer */
                    {
                        s = s|((ss[(m * 8 + n)] & 0x01) << 7);                            /* get s */
                        if (a_ds2431_write_bit(handle, ss[(m*8+n)]) != 0)                 /* write data */
                        {
                            handle->debug_print("ds2431: write bit failed.\n");           /* write bit failed */
                            
                            return 1;                                                     /* return error */
                        }
                    }    
                    else if (conflict_bit == buffer[l])                                   /* if == buffer */
                    {
                        s = s | 0x80;                                                     /* set 7 bit */
                        if (a_ds2431_write_bit(handle, 1) != 0)                           /* write 1 */
                        {
                            handle->debug_print("ds2431: write bit failed.\n");           /* write bit failed */
                            
                            return 1;                                                     /* return error */
                        }
                        ss[(m * 8 + n)] = 1;                                              /* set 1 */
                        l = l-1;                                                          /* l-- */
                    }
                    else
                    {
                        
                    }
                }
                else
                {
                    *number = num;                                                        /* save num */
                    
                    return 0;                                                             /* success return 0 */
                }
                handle->delay_us(5);                                                      /* delay 5 us */
            }
            pid[num][m] = s;                                                              /* save s */
            s = 0;                                                                        /* reset s */
        }
        num++;                                                                            /* num++ */
        if (num > (*number))                                                              /* check num range */
        {
            break;                                                                        /* break */
        }
    } while (buffer[l] != 0);                                                             /* check buffer[l] */
    *number = num;                                                                        /* set number */
    
    return 0;                                                                             /* success return 0 */
}

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
uint8_t ds2431_search_rom(ds2431_handle_t *handle, uint8_t (*rom)[8], uint8_t *num)
{
    if (handle == NULL)                                                    /* check handle */
    {
        return 2;                                                          /* return error */
    }
    if (handle->inited != 1)                                               /* check handle initialization */
    {
        return 3;                                                          /* return error */
    }
    
    return a_ds2431_search(handle, rom, DS2431_CMD_SEARCH_ROM, num);       /* return search result */
}

/**
 * @brief      get chip's information
 * @param[out] *info pointer to a ds2431 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t ds2431_info(ds2431_info_t *info)
{
    if (info == NULL)                                               /* check handle */
    {
        return 2;                                                   /* return error */
    }
    
    memset(info, 0, sizeof(ds2431_info_t));                         /* initialize ds2431 info structure */
    strncpy(info->chip_name, CHIP_NAME, 32);                        /* copy chip name */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 32);        /* copy manufacturer name */
    strncpy(info->interface, "GPIO", 8);                            /* copy interface name */
    info->supply_voltage_min_v = SUPPLY_VOLTAGE_MIN;                /* set minimal supply voltage */
    info->supply_voltage_max_v = SUPPLY_VOLTAGE_MAX;                /* set maximum supply voltage */
    info->max_current_ma = MAX_CURRENT;                             /* set maximum current */
    info->temperature_max = TEMPERATURE_MAX;                        /* set minimal temperature */
    info->temperature_min = TEMPERATURE_MIN;                        /* set maximum temperature */
    info->driver_version = DRIVER_VERSION;                          /* set driver version */
    
    return 0;                                                       /* success return 0 */
}
