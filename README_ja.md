[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver DS2431

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/ds2431/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

DS2431は、1024ビットの1-Wire EEPROMチップで、256ビットのメモリページを4つ備えています。データは8バイトのスクラッチパッドに書き込まれ、検証された後、EEPROMメモリにコピーされます。特別な機能として、4つのメモリページは個別に書き込み保護するか、ビットを1から0への変更のみを可能にするEPROMエミュレーションモードに設定できます。DS2431は単導体の1-Wireバスを介して通信します。通信は標準の1-Wireプロトコルに準拠しています。各デバイスには、工場でチップにレーザー刻印された、変更不可能な固有の64ビットROM登録番号が割り当てられています。この登録番号は、マルチドロップの1-Wireネット環境でデバイスをアドレス指定するために使用されます。

LibDriver DS2431は、LibDriverがリリースしたDS2431用のフル機能ドライバです。スキップモードの読み書き、マッチROMモードの読み書き、オーバードライブモードの読み書き、レジュームモードの読み書き、ROM検索などの追加機能を提供します。LibDriverはMISRAに準拠しています。

### 目次

  - [説明](#説明)
  - [インストール](#インストール)
  - [使用](#使用)
    - [example basic](#example-basic)
    - [example match](#example-match)
    - [example search](#example-search)
  - [ドキュメント](#ドキュメント)
  - [貢献](#貢献)
  - [著作権](#著作権)
  - [連絡して](#連絡して)

### 説明

/ srcディレクトリには、LibDriver DS2431のソースファイルが含まれています。

/ interfaceディレクトリには、LibDriver DS2431用のプラットフォームに依存しないonewireバステンプレートが含まれています。

/ testディレクトリには、チップの必要な機能を簡単にテストできるLibDriver DS2431ドライバーテストプログラムが含まれています。

/ exampleディレクトリには、LibDriver DS2431プログラミング例が含まれています。

/ docディレクトリには、LibDriver DS2431オフラインドキュメントが含まれています。

/ datasheetディレクトリには、DS2431データシートが含まれています。

/ projectディレクトリには、一般的に使用されるLinuxおよびマイクロコントローラー開発ボードのプロジェクトサンプルが含まれています。 すべてのプロジェクトは、デバッグ方法としてシェルスクリプトを使用しています。詳細については、各プロジェクトのREADME.mdを参照してください。

/ misraはLibDriver misraコードスキャン結果を含む。

### インストール

/ interfaceディレクトリにあるプラットフォームに依存しないonewireバステンプレートを参照して、指定したプラットフォームのonewireバスドライバを完成させます。

/src ディレクトリ、プラットフォームのインターフェイス ドライバー、および独自のドライバーをプロジェクトに追加します。デフォルトのサンプル ドライバーを使用する場合は、/example ディレクトリをプロジェクトに追加します。

### 使用

/example ディレクトリ内のサンプルを参照して、独自のドライバーを完成させることができます。 デフォルトのプログラミング例を使用したい場合の使用方法は次のとおりです。

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

```c
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

```c
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

### ドキュメント

オンラインドキュメント: [https://www.libdriver.com/docs/ds2431/index.html](https://www.libdriver.com/docs/ds2431/index.html)。

オフラインドキュメント: /doc/html/index.html。

### 貢献

CONTRIBUTING.mdを参照してください。

### 著作権

著作権（c）2015-今 LibDriver 全著作権所有

MITライセンス（MIT）

このソフトウェアおよび関連するドキュメントファイル（「ソフトウェア」）のコピーを取得した人は、無制限の使用、複製、変更、組み込み、公開、配布、サブライセンスを含む、ソフトウェアを処分する権利を制限なく付与されます。ソフトウェアのライセンスおよび/またはコピーの販売、および上記のようにソフトウェアが配布された人の権利のサブライセンスは、次の条件に従うものとします。

上記の著作権表示およびこの許可通知は、このソフトウェアのすべてのコピーまたは実体に含まれるものとします。

このソフトウェアは「現状有姿」で提供され、商品性、特定目的への適合性、および非侵害の保証を含むがこれらに限定されない、明示または黙示を問わず、いかなる種類の保証もありません。 いかなる場合も、作者または著作権所有者は、契約、不法行為、またはその他の方法で、本ソフトウェアおよび本ソフトウェアの使用またはその他の廃棄に起因または関連して、請求、損害、またはその他の責任を負わないものとします。

### 連絡して

お問い合わせくださいlishifenging@outlook.com。