[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver DS2431

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/ds2431/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

The DS2431 is a 1024-bit, 1-Wire EEPROM chip organized as four memory pages of 256 bits each. Data is written to an 8-byte scratchpad, verified, and then copied to the EEPROM memory. As a special feature, the four memory pages can individually be write protected or put in EPROM-emulation mode, where bits can only be changed from a 1 to a 0 state. The DS2431 communicates over the single-conductor 1-Wire bus. The communication follows the standard 1-Wire protocol. Each device has its own unalterable and unique 64-bit ROM registration number that is factory lasered into the chip. The registration number is used to address the device in a multidrop, 1-Wire net environment.

LibDriver DS2431 is a full-featured driver for DS2431, launched by LibDriver. It provides skip mode reading and writing, match ROM mode reading and writing, overdrive mode reading and writing, resume mode reading and writing, ROM searching and additional features. LibDriver is MISRA compliant.

### Table of Contents

  - [Instruction](#Instruction)
  - [Install](#Install)
  - [Usage](#Usage)
    - [example basic](#example-basic)
    - [example match](#example-match)
    - [example search](#example-search)
  - [Document](#Document)
  - [Contributing](#Contributing)
  - [License](#License)
  - [Contact Us](#Contact-Us)

### Instruction

/src includes LibDriver DS2431 source files.

/interface includes LibDriver DS2431 onewire platform independent template.

/test includes LibDriver DS2431 driver test code and this code can test the chip necessary function simply.

/example includes LibDriver DS2431 sample code.

/doc includes LibDriver DS2431 offline document.

/datasheet includes DS2431 datasheet.

/project includes the common Linux and MCU development board sample code. All projects use the shell script to debug the driver and the detail instruction can be found in each project's README.md.

/misra includes the LibDriver MISRA code scanning results.

### Install

Reference /interface onewire platform independent template and finish your platform onewire driver.

Add the /src directory, the interface driver for your platform, and your own drivers to your project, if you want to use the default example drivers, add the /example directory to your project.

### Usage

You can refer to the examples in the /example directory to complete your own driver. If you want to use the default programming examples, here's how to use them.

#### example basic

```C
#include "driver_ds2431_basic.h"

uint8_t res;
uint8_t buffer[128];
ds2431_config_control_t config;

/* init */
res = ds2431_basic_init();
if (res != 0)
{
    return 1;
}

...
    
/* read data */
res = ds2431_basic_read(0x0000, buffer, 128);
if (res != 0)
{
    (void)ds2431_basic_deinit();

    return 1;
}

...

/* write data */
res = ds2431_basic_write(0x0000, buffer, 128);
if (res != 0)
{
    (void)ds2431_basic_deinit();

    return 1;
}

...

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

...
    
/* deinit */
(void)ds2431_basic_deinit();

return 0;
```

#### example match

```C
#include "driver_ds2431_match.h"

uint8_t res;
uint8_t rom[8] = {0};
uint8_t buffer[128];
ds2431_config_control_t config;

/* init */
res = ds2431_match_init();
if (res != 0)
{
    return 1;
}

...
    
/* read data */
res = ds2431_match_read((uint8_t *)rom, 0x0000, buffer, 128);
if (res != 0)
{
    (void)ds2431_match_deinit();

    return 1;
}

...

/* write data */
res = ds2431_match_write((uint8_t *)rom, 0x0000, buffer, 128);
if (res != 0)
{
    (void)ds2431_match_deinit();

    return 1;
}

...

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

...
    
/* deinit */
(void)ds2431_match_deinit();

return 0;
```

#### example search

```C
#include "driver_ds2431_search.h"

uint8_t res, i, j;
uint8_t rom[8][8];
uint8_t num;

/* init */
res = ds2431_search_init();
if (res != 0)
{
    return 1;
}

...
    
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

...
    
/* deinit */
(void)ds2431_search_deinit();

return 0;
```

### Document

Online documents: [https://www.libdriver.com/docs/ds2431/index.html](https://www.libdriver.com/docs/ds2431/index.html).

Offline documents: /doc/html/index.html.

### Contributing

Please refer to CONTRIBUTING.md.

### License

Copyright (c) 2015 - present LibDriver All rights reserved



The MIT License (MIT) 



Permission is hereby granted, free of charge, to any person obtaining a copy

of this software and associated documentation files (the "Software"), to deal

in the Software without restriction, including without limitation the rights

to use, copy, modify, merge, publish, distribute, sublicense, and/or sell

copies of the Software, and to permit persons to whom the Software is

furnished to do so, subject to the following conditions: 



The above copyright notice and this permission notice shall be included in all

copies or substantial portions of the Software. 



THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR

IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,

FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE

AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER

LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,

OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE

SOFTWARE. 

### Contact Us

Please send an e-mail to lishifenging@outlook.com.