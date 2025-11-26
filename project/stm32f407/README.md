### 1. Chip

#### 1.1 Chip Info

Chip Name: STM32F407ZGT6.

Extern Oscillator: 8MHz.

UART Pin: TX/RX PA9/PA10.

DATA Pin: PA8.

### 2. Development and Debugging

#### 2.1 Integrated Development Environment

LibDriver provides both Keil and IAR integrated development environment projects.

MDK is the Keil ARM project and your Keil version must be 5 or higher.Keil ARM project needs STMicroelectronics STM32F4 Series Device Family Pack and you can download from https://www.keil.com/dd2/stmicroelectronics/stm32f407zgtx.

EW is the IAR ARM project and your IAR version must be 9 or higher.

#### 2.2 Serial Port Parameter

Baud Rate: 115200.

Data Bits : 8.

Stop Bits: 1.

Parity: None.

Flow Control: None.

#### 2.3 Serial Port Assistant

We use '\n' to wrap lines.If your serial port assistant displays exceptions (e.g. the displayed content does not divide lines), please modify the configuration of your serial port assistant or replace one that supports '\n' parsing.

### 3. DS2431

#### 3.1 Command Instruction

1. Show ds2431 chip and driver information.

    ```shell
    ds2431 (-i | --information)
    ```

2. Show ds2431 help.

   ```shell
   ds2431 (-h | --help)
   ```

3. Show ds2431 pin connections of the current board.

   ```shell
   ds2431 (-p | --port)
   ```

4. Run ds2431 register test.

   ```shell
   ds2431 (-t reg | --test=reg)
   ```

5.  Run ds2431 read test, num means the test times.

   ```shell
   ds2431 (-t read | --test=read) [--times=<num>]
   ```

6. Run ds2431 search test.

   ```shell
   ds2431 (-t search | --test=search)
   ```

7. Run ds2431 skip read function, address is hexadecimal.

   ```shell
   ds2431 (-e skip-read | --example=skip-read) [--addr=<hex>]
   ```

8. Run ds2431 skip write function, address is hexadecimal and data is hexadecimal.

   ```shell
   ds2431 (-e skip-write | --example=skip-write) [--addr=<hex>] [--data=<hex>]
   ```
   
9. Run ds2431 skip config function.

   ```shell
   ds2431 (-e skip-config | --example=skip-config)
   ```

10. Run ds2431 match read function, code is the 8 bytes hexadecimal rom and address is hexadecimal.

       ```shell
       ds2431 (-e match-read | --example=match-read) [--rom=<code>] [--addr=<hex>]
       ```

11. Run ds2431 match write function, code is the 8 bytes hexadecimal rom, address is hexadecimal and data is hexadecimal.

       ```shell
       ds2431 (-e match-write | --example=match-write) [--rom=<code>] [--addr=<hex>] [--data=<hex>]
       ```

12. Run ds2431 match config function, code is the 8 bytes hexadecimal rom.

       ```shell
       ds2431 (-e match-config | --example=match-config) [--rom=<code>]
       ```

13. Run ds2431 search rom function.

       ```shell
       ds2431 (-e search | --example=search)
       ```

#### 3.2 Command Example

```shell
ds2431 -i

ds2431: chip is Maxim Integrated DS2431.
ds2431: manufacturer is Maxim Integrated.
ds2431: interface is GPIO.
ds2431: driver version is 1.0.
ds2431: min supply voltage is 2.8V.
ds2431: max supply voltage is 5.2V.
ds2431: max current is 0.80mA.
ds2431: max temperature is 85.0C.
ds2431: min temperature is -40.0C.
```

```shell
ds2431 -p

ds2431: DQ pin connected to GPIOA PIN8.
```

```shell
ds2431 -t reg

ds2431: chip is Maxim Integrated DS2431.
ds2431: manufacturer is Maxim Integrated.
ds2431: interface is GPIO.
ds2431: driver version is 1.0.
ds2431: min supply voltage is 2.8V.
ds2431: max supply voltage is 5.2V.
ds2431: max current is 0.80mA.
ds2431: max temperature is 85.0C.
ds2431: min temperature is -40.0C.
ds2431: start register test.
ds2431: ds2431_set_mode/ds2431_get_mode test.
ds2431: set skip rom mode.
ds2431: check mode ok.
ds2431: set match rom mode.
ds2431: check mode ok.
ds2431: set overdrive skip rom mode.
ds2431: check mode ok.
ds2431: set overdrive match rom mode.
ds2431: check mode ok.
ds2431: set resume mode.
ds2431: check mode ok.
ds2431: ds2431_set_rom/ds2431_get_rom test.
ds2431: set rom.
ds2431: check rom ok.
ds2431: ds2431_read_memory_config/ds2431_write_memory_config test.
ds2431: page0 protection control is 0x00.
ds2431: page1 protection control is 0x00.
ds2431: page2 protection control is 0x00.
ds2431: page3 protection control is 0x00.
ds2431: copy protection is 0x00.
ds2431: factory byte is 0x55.
ds2431: user byte 0 is 0x00.
ds2431: user byte 1 is 0x00.
ds2431: check memory config ok.
ds2431: finish register test.
```

```shell
ds2431 -t search

ds2431: chip is Maxim Integrated DS2431.
ds2431: manufacturer is Maxim Integrated.
ds2431: interface is GPIO.
ds2431: driver version is 1.0.
ds2431: min supply voltage is 2.8V.
ds2431: max supply voltage is 5.2V.
ds2431: max current is 0.80mA.
ds2431: max temperature is 85.0C.
ds2431: min temperature is -40.0C.
ds2431: start search test.
ds2431: search rom...
ds2431: find 1 rom.
ds2431: rom 2DA97D5D53000046.
ds2431: finish search test.
```

```shell
ds2431 -t read --times=3

ds2431: chip is Maxim Integrated DS2431.
ds2431: manufacturer is Maxim Integrated.
ds2431: interface is GPIO.
ds2431: driver version is 1.0.
ds2431: min supply voltage is 2.8V.
ds2431: max supply voltage is 5.2V.
ds2431: max current is 0.80mA.
ds2431: max temperature is 85.0C.
ds2431: min temperature is -40.0C.
ds2431: start read test.
ds2431: set skip rom.
ds2431: write/read/copy scratchpad test.
ds2431: data check passed.
ds2431: 0x10 address check passed.
ds2431: read_memory test.
ds2431: data check passed.
ds2431: set overdrive skip rom.
ds2431: write/read/copy scratchpad test.
ds2431: data check passed.
ds2431: 0x60 address check passed.
ds2431: read_memory test.
ds2431: data check passed.
ds2431: set match rom.
ds2431: write/read/copy scratchpad test.
ds2431: data check passed.
ds2431: 0x68 address check passed.
ds2431: read_memory test.
ds2431: data check passed.
ds2431: set overdrive match rom.
ds2431: write/read/copy scratchpad test.
ds2431: data check passed.
ds2431: 0x58 address check passed.
ds2431: read_memory test.
ds2431: data check passed.
ds2431: set resume mode.
ds2431: write/read/copy scratchpad test.
ds2431: data check passed.
ds2431: 0x18 address check passed.
ds2431: read_memory test.
ds2431: data check passed.
ds2431: set overdrive match rom.
ds2431: write/read/copy scratchpad test.
ds2431: data check passed.
ds2431: 0x60 address check passed.
ds2431: read_memory test.
ds2431: data check passed.
ds2431: set skip rom.
ds2431: passed.
ds2431: passed.
ds2431: passed.
ds2431: set overdrive skip rom mode.
ds2431: passed.
ds2431: passed.
ds2431: passed.
ds2431: set match rom mode.
ds2431: passed.
ds2431: passed.
ds2431: passed.
ds2431: set overdrive match rom mode.
ds2431: passed.
ds2431: passed.
ds2431: passed.
ds2431: set resume mode.
ds2431: passed.
ds2431: passed.
ds2431: passed.
ds2431: set overdrive resume mode.
ds2431: passed.
ds2431: passed.
ds2431: passed.
ds2431: finish read test.
```

```shell
ds2431 -e search

ds2431: find 1 rom(s).
ds2431: 1/1 is 2DA97D5D53000046.
```

```shell
ds2431 -e skip-read --addr=0x06

ds2431: address 0x06 read data 0x68.
```

```shell
ds2431 -e skip-write --addr=0x06 --data=0x68

ds2431: address 0x06 write data 0x68.
```

```shell
ds2431 -e skip-config

ds2431: page0 protection control is 0x00.
ds2431: page1 protection control is 0x00.
ds2431: page2 protection control is 0x00.
ds2431: page3 protection control is 0x00.
ds2431: copy protection is 0x00.
ds2431: factory byte is 0x55.
ds2431: user byte 0 is 0x00.
ds2431: user byte 1 is 0x00.
```

```shell
ds2431 -e match-read --rom=2DA97D5D53000046 --addr=0x12

ds2431: address 0x12 read data 0xB1.
```

```shell
ds2431 -e match-write --rom=2DA97D5D53000046 --addr=0x12 --data=0xB1

ds2431: address 0x12 write data 0xB1.
```

```shell
ds2431 -e match-config --rom=2DA97D5D53000046

ds2431: page0 protection control is 0x00.
ds2431: page1 protection control is 0x00.
ds2431: page2 protection control is 0x00.
ds2431: page3 protection control is 0x00.
ds2431: copy protection is 0x00.
ds2431: factory byte is 0x55.
ds2431: user byte 0 is 0x00.
ds2431: user byte 1 is 0x00.
```

```shell
ds2431 -h

Usage:
  ds2431 (-i | --information)
  ds2431 (-h | --help)
  ds2431 (-p | --port)
  ds2431 (-t reg | --test=reg)
  ds2431 (-t read | --test=read) [--times=<num>]
  ds2431 (-t search | --test=search)
  ds2431 (-e skip-read | --example=skip-read) [--addr=<hex>]
  ds2431 (-e skip-write | --example=skip-write) [--addr=<hex>] [--data=<hex>]
  ds2431 (-e skip-config | --example=skip-config)
  ds2431 (-e match-read | --example=match-read) [--rom=<code>] [--addr=<hex>]
  ds2431 (-e match-write | --example=match-write) [--rom=<code>] [--addr=<hex>] [--data=<hex>]
  ds2431 (-e match-config | --example=match-config) [--rom=<code>]
  ds2431 (-e search | --example=search)

Options:
      --addr=<hex>               Set the read or write address and it is hexadecimal.([default: 0x00])
      --data=<hex>               Set the write data and it is hexadecimal.([default: 0x00])
  -e <skip-read | skip-write | skip-config | match-read | match-write | match-config | search>,
      --example=<skip-read | skip-write | skip-config | match-read | match-write | match-config | search>
                                 Run the driver example.
  -h, --help                     Show the help.
  -i, --information              Show the chip information.
  -p, --port                     Display the pin connections of the current board.
      --rom=<code>               Set the rom with the length of 8 and it is hexadecimal.([default: 0000000000000000])
  -t <reg | read | search>, --test=<reg | read | search>
                                 Run the driver test.
      --times=<num>              Set the running times.([default: 3])
```

