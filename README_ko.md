[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver DS2431

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/ds2431/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

DS2431은 1024비트 1-Wire EEPROM 칩으로, 각각 256비트씩 4개의 메모리 페이지로 구성됩니다. 데이터는 8바이트 스크래치패드에 기록되고 검증된 후 EEPROM 메모리에 복사됩니다. 특별한 기능으로, 4개의 메모리 페이지는 개별적으로 쓰기 금지되거나 EPROM 에뮬레이션 모드로 전환될 수 있습니다. EPROM 에뮬레이션 모드에서는 비트를 1에서 0으로만 변경할 수 있습니다. DS2431은 단일 도체 1-Wire 버스를 통해 통신하며, 표준 1-Wire 프로토콜을 따릅니다. 각 장치는 변경 불가능한 고유한 64비트 ROM 등록 번호를 가지며, 이 번호는 칩에 공장에서 레이저로 각인됩니다. 이 등록 번호는 멀티드롭 1-Wire 네트워크 환경에서 장치의 주소를 지정하는 데 사용됩니다.

LibDriver DS2431은 LibDriver에서 출시한 DS2431용 모든 기능을 갖춘 드라이버입니다. 스킵 모드 읽기/쓰기, 매치 ROM 모드 읽기/쓰기, 오버드라이브 모드 읽기/쓰기, 재개 모드 읽기/쓰기, ROM 검색 및 추가 기능을 제공합니다. LibDriver는 MISRA를 준수합니다.

### 콘텐츠

  - [설명](#설명)
  - [설치](#설치)
  - [사용](#사용)
    - [example basic](#example-basic)
    - [example match](#example-match)
    - [example search](#example-search)
  - [문서](#문서)
  - [기고](#기고)
  - [저작권](#저작권)
  - [문의하기](#문의하기)

### 설명

/src 디렉토리에는 LibDriver DS2431의 소스 파일이 포함되어 있습니다.

/interface 디렉토리에는 LibDriver DS2431용 플랫폼 독립적인 onewire버스 템플릿이 포함되어 있습니다.

/test 디렉토리에는 LibDriver DS2431드라이버 테스트 프로그램이 포함되어 있어 칩의 필요한 기능을 간단히 테스트할 수 있습니다.

/example 디렉토리에는 LibDriver DS2431프로그래밍 예제가 포함되어 있습니다.

/doc 디렉토리에는 LibDriver DS2431오프라인 문서가 포함되어 있습니다.

/datasheet 디렉토리에는 DS2431데이터시트가 있습니다.

/project 디렉토리에는 일반적으로 사용되는 Linux 및 마이크로컨트롤러 개발 보드의 프로젝트 샘플이 포함되어 있습니다. 모든 프로젝트는 디버깅 방법으로 셸 스크립트를 사용하며, 자세한 내용은 각 프로젝트의 README.md를 참조하십시오.

/misra 에는 LibDriver misra 코드 검색 결과가 포함됩니다.

### 설치

/interface 디렉토리에서 플랫폼 독립적인 onewire버스 템플릿을 참조하여 지정된 플랫폼에 대한 onewire버스 드라이버를 완성하십시오.

/src 디렉터리, 플랫폼용 인터페이스 드라이버 및 자체 드라이버를 프로젝트에 추가합니다. 기본 예제 드라이버를 사용하려면 /example 디렉터리를 프로젝트에 추가합니다.

### 사용

/example 디렉터리의 예제를 참조하여 자신만의 드라이버를 완성할 수 있습니다. 기본 프로그래밍 예제를 사용하려는 경우 사용 방법은 다음과 같습니다.

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

### 문서

온라인 문서: [https://www.libdriver.com/docs/ds2431/index.html](https://www.libdriver.com/docs/ds2431/index.html).

오프라인 문서: /doc/html/index.html.

### 기고

CONTRIBUTING.md 를 참조하십시오.

### 저작권

저작권 (c) 2015 - 지금 LibDriver 판권 소유

MIT 라이선스(MIT)

이 소프트웨어 및 관련 문서 파일("소프트웨어")의 사본을 얻은 모든 사람은 이에 따라 무제한 사용, 복제, 수정, 통합, 출판, 배포, 2차 라이선스를 포함하여 소프트웨어를 처분할 수 있는 권리가 부여됩니다. 소프트웨어의 사본에 대한 라이선스 및/또는 판매, 그리고 소프트웨어가 위와 같이 배포된 사람의 권리에 대한 2차 라이선스는 다음 조건에 따릅니다.

위의 저작권 표시 및 이 허가 표시는 이 소프트웨어의 모든 사본 또는 내용에 포함됩니다.

이 소프트웨어는 상품성, 특정 목적에의 적합성 및 비침해에 대한 보증을 포함하되 이에 국한되지 않는 어떠한 종류의 명시적 또는 묵시적 보증 없이 "있는 그대로" 제공됩니다. 어떤 경우에도 저자 또는 저작권 소유자는 계약, 불법 행위 또는 기타 방식에 관계없이 소프트웨어 및 기타 소프트웨어 사용으로 인해 발생하거나 이와 관련하여 발생하는 청구, 손해 또는 기타 책임에 대해 책임을 지지 않습니다.

### 문의하기

연락주세요lishifenging@outlook.com.