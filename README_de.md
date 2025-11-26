[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver DS2431
[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/ds2431/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE) 

Der DS2431 ist ein 1024-Bit-EEPROM-Chip mit 1-Wire-Schnittstelle, der in vier Speicherseiten à 256 Bit unterteilt ist. Daten werden zunächst in einen 8-Byte-Zwischenspeicher geschrieben, verifiziert und anschließend in den EEPROM-Speicher kopiert. Eine Besonderheit ist der individuelle Schreibschutz der vier Speicherseiten oder der EPROM-Emulationsmodus, in dem Bits nur von 1 auf 0 geändert werden können. Der DS2431 kommuniziert über den 1-Wire-Bus (Single-Conductor). Die Kommunikation folgt dem Standard-1-Wire-Protokoll. Jedes Gerät besitzt eine eigene, unveränderliche und eindeutige 64-Bit-ROM-Registrierungsnummer, die werkseitig per Laser in den Chip eingraviert wird. Diese Registrierungsnummer dient zur Adressierung des Geräts in einer 1-Wire-Netzwerkumgebung mit mehreren Anschlüssen.

LibDriver DS2431 ist ein umfassender Treiber für den DS2431, entwickelt von LibDriver. Er bietet Lese- und Schreibfunktionen im Skip-, Match-ROM-, Overdrive- und Resume-Modus sowie ROM-Suche und weitere Funktionen. LibDriver ist MISRA-konform.

### Inhaltsverzeichnis

  - [Anweisung](#Anweisung)
  - [Installieren](#Installieren)
  - [Nutzung](#Nutzung)
    - [example basic](#example-basic)
    - [example match](#example-match)
    - [example search](#example-search)
  - [Dokument](#Dokument)
  - [Beitrag](#Beitrag)
  - [Lizenz](#Lizenz)
  - [Kontaktieren Sie uns](#Kontaktieren-Sie-uns)

### Anweisung

/src enthält LibDriver DS2431-Quelldateien.

/interface enthält die plattformunabhängige Vorlage LibDriver DS2431 onewire.

/test enthält den Testcode des LibDriver DS2431-Treibers und dieser Code kann die erforderliche Funktion des Chips einfach testen.

/example enthält LibDriver DS2431-Beispielcode.

/doc enthält das LibDriver DS2431-Offlinedokument.

/Datenblatt enthält DS2431-Datenblatt.

/project enthält den allgemeinen Beispielcode für Linux- und MCU-Entwicklungsboards. Alle Projekte verwenden das Shell-Skript, um den Treiber zu debuggen, und die detaillierten Anweisungen finden Sie in der README.md jedes Projekts.

/misra enthält die Ergebnisse des LibDriver MISRA Code Scans.

### Installieren

Verweisen Sie auf eine plattformunabhängige onewire-Schnittstellenvorlage und stellen Sie Ihren Plattform-onewire-Treiber fertig.

Fügen Sie das Verzeichnis /src, den Schnittstellentreiber für Ihre Plattform und Ihre eigenen Treiber zu Ihrem Projekt hinzu. Wenn Sie die Standardbeispieltreiber verwenden möchten, fügen Sie das Verzeichnis /example zu Ihrem Projekt hinzu.

### Nutzung

Sie können auf die Beispiele im Verzeichnis /example zurückgreifen, um Ihren eigenen Treiber zu vervollständigen. Wenn Sie die Standardprogrammierbeispiele verwenden möchten, erfahren Sie hier, wie Sie diese verwenden.

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

### Dokument

Online-Dokumente: [https://www.libdriver.com/docs/ds2431/index.html](https://www.libdriver.com/docs/ds2431/index.html).

Offline-Dokumente: /doc/html/index.html.

### Beitrag

Bitte beachten Sie CONTRIBUTING.md.

### Lizenz

Urheberrechte © (c) 2015 - Gegenwart LibDriver Alle Rechte vorbehalten



Die MIT-Lizenz (MIT)



Hiermit wird jeder Person kostenlos die Erlaubnis erteilt, eine Kopie zu erhalten

dieser Software und zugehörigen Dokumentationsdateien (die „Software“) zu behandeln

in der Software ohne Einschränkung, einschließlich, aber nicht beschränkt auf die Rechte

zu verwenden, zu kopieren, zu modifizieren, zusammenzuführen, zu veröffentlichen, zu verteilen, unterzulizenzieren und/oder zu verkaufen

Kopien der Software und Personen, denen die Software gehört, zu gestatten

dazu eingerichtet werden, unter folgenden Bedingungen:



Der obige Urheberrechtshinweis und dieser Genehmigungshinweis müssen in allen enthalten sein

Kopien oder wesentliche Teile der Software.



DIE SOFTWARE WIRD "WIE BESEHEN" BEREITGESTELLT, OHNE JEGLICHE GEWÄHRLEISTUNG, AUSDRÜCKLICH ODER

STILLSCHWEIGEND, EINSCHLIESSLICH, ABER NICHT BESCHRÄNKT AUF DIE GEWÄHRLEISTUNG DER MARKTGÄNGIGKEIT,

EIGNUNG FÜR EINEN BESTIMMTEN ZWECK UND NICHTVERLETZUNG VON RECHTEN DRITTER. IN KEINEM FALL DARF DAS

AUTOREN ODER URHEBERRECHTSINHABER HAFTEN FÜR JEGLICHE ANSPRÜCHE, SCHÄDEN ODER ANDERE

HAFTUNG, OB AUS VERTRAG, DELIKT ODER ANDERWEITIG, ENTSTEHEND AUS,

AUS ODER IM ZUSAMMENHANG MIT DER SOFTWARE ODER DER VERWENDUNG ODER ANDEREN HANDLUNGEN MIT DER

SOFTWARE.

### Kontaktieren Sie uns

Bitte senden Sie eine E-Mail an lishifenging@outlook.com.