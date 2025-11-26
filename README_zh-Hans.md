[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver DS2431

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/ds2431/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

DS2431是一个1024位、1-Wire EEPROM芯片，由四个256位的存储页组成。数据被写入8字节的草稿行，经过验证，然后复制到EEPROM存储器。作为一项特殊功能，四个存储页可以单独进行写保护或置于EPROM仿真模式，在该模式下，位只能从1状态更改为0状态。DS2431通过单总线1-Wire总线进行通信。通信遵循标准1-Wire协议。每个设备都有自己不可更改且唯一的64位ROM序列号，该序列号在工厂激光打印到芯片中。序列号用于在多点1-Wire网络环境中对设备进行寻址。

LibDriver DS2431是LibDriver推出的DS2431全功能驱动，该驱动提供跳过ROM模式读写、匹配ROM模型读写，高速模式读写，简易模式读写和搜索ROM等功能并且它符合MISRA标准。

### 目录

  - [说明](#说明)
  - [安装](#安装)
  - [使用](#使用)
    - [example basic](#example-basic)
    - [example match](#example-match)
    - [example search](#example-search)
  - [文档](#文档)
  - [贡献](#贡献)
  - [版权](#版权)
  - [联系我们](#联系我们)

### 说明

/src目录包含了LibDriver DS2431的源文件。

/interface目录包含了LibDriver DS2431与平台无关的onewire总线模板。

/test目录包含了LibDriver DS2431驱动测试程序，该程序可以简单的测试芯片必要功能。

/example目录包含了LibDriver DS2431编程范例。

/doc目录包含了LibDriver DS2431离线文档。

/datasheet目录包含了DS2431数据手册。

/project目录包含了常用Linux与单片机开发板的工程样例。所有工程均采用shell脚本作为调试方法，详细内容可参考每个工程里面的README.md。

/misra目录包含了LibDriver MISRA代码扫描结果。

### 安装

参考/interface目录下与平台无关的onewire总线模板，完成指定平台的onewire总线驱动。

将/src目录，您使用平台的接口驱动和您开发的驱动加入工程，如果您想要使用默认的范例驱动，可以将/example目录加入您的工程。

### 使用

您可以参考/example目录下的编程范例完成适合您的驱动，如果您想要使用默认的编程范例，以下是它们的使用方法。

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

### 文档

在线文档: [https://www.libdriver.com/docs/ds2431/index.html](https://www.libdriver.com/docs/ds2431/index.html)。

离线文档: /doc/html/index.html。

### 贡献

请参考CONTRIBUTING.md。

### 版权

版权 (c) 2015 - 现在 LibDriver 版权所有

MIT 许可证（MIT）

特此免费授予任何获得本软件副本和相关文档文件（下称“软件”）的人不受限制地处置该软件的权利，包括不受限制地使用、复制、修改、合并、发布、分发、转授许可和/或出售该软件副本，以及再授权被配发了本软件的人如上的权利，须在下列条件下：

上述版权声明和本许可声明应包含在该软件的所有副本或实质成分中。

本软件是“如此”提供的，没有任何形式的明示或暗示的保证，包括但不限于对适销性、特定用途的适用性和不侵权的保证。在任何情况下，作者或版权持有人都不对任何索赔、损害或其他责任负责，无论这些追责来自合同、侵权或其它行为中，还是产生于、源于或有关于本软件以及本软件的使用或其它处置。

### 联系我们

请联系lishifenging@outlook.com。