# write_SH1106_Display

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates basic usage of I2C driver by running two tasks on I2C bus:

1. Read external I2C sensor, here we take the BH1750 ambient light sensor (GY-30 module) for an example.

If you have a new I2C application to go (for example, read the temperature data from external sensor with I2C interface), try this as a basic template, then add your own code.

## How to use example

### Hardware Required

To run this example, you should have one ESP development board (e.g. ESP32-WROVER Kit) or ESP core board (e.g. ESP32-DevKitC). Optionally, you can also connect an external sensor. Here we choose the BH1750 just as an example. BH1750 is a digital ambient light sensor. For more information about it, you can read the [datasheet](http://rohmfs.rohm.com/en/products/databook/datasheet/ic/sensor/light/bh1721fvc-e.pdf) of this sensor.


#### Pin Assignment(esp32s3):

**Note:** The following pin assignments are used by default, you can change these  in the `menuconfig` .

|                           | SDA    | SCL    |
| ------------------------- | ------ | ------ |
| ESP32-S3 I2C Master       | GPIO1  | GPIO2  |
| ESP32-S3 I2C Slave        | GPIO4  | GPIO5  | Not used for sensor
| BH1750 Sensor             | SDA    | SCL    |

- slave:
  - GPIO4 is assigned as the data signal of I2C slave port
  - GPIO5 is assigned as the clock signal of I2C slave port
- master:
  - GPIO1 is assigned as the data signal of I2C master port
  - GPIO2 is assigned as the clock signal of I2C master port

- Connection:
  - connect GPIO1 with GPIO4
  - connect GPIO2 with GPIO5
  - connect SDA/SCL of BH1750 sensor with GPIO1/GPIO2

**Note:** It is recommended to add external pull-up resistors for SDA/SCL pins to make the communication more stable, though the driver will enable internal pull-up resistors.

### Configure the project

Open the project configuration menu (`idf.py menuconfig`). Then go into `Example Configuration` menu.

- In the `I2C Master` submenu, you can set the pin number of SDA/SCL according to your board. Also you can modify the I2C port number and freauency of the master.
- In the `BH1750 Sensor` submenu, you can choose the slave address of BH1750 accroding to the pin level of ADDR pin (if the pin level of ADDR is low then the address is `0x23`, otherwise it is `0x5c`). Here you can also control the operation mode of BH1750, each mode has a different resolution and measurement time. For example, in the `One Time L-Resolution` mode, the resolution is 4 Lux and measurement time is typically 16ms (higher resolution means longer measurement time). For more information, you can consult the datasheet of BH1750.

- So pull the ADDR pin of the BH1750 Sensor LOW.


### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (6495) i2c-example: TASK[1] test cnt: 1
*******************
TASK[1]  MASTER READ SENSOR( BH1750 )
*******************
data_h: 01
data_l: d0
sensor val: 386.67 [Lux]
I (6695) i2c-example: TASK[0] test cnt: 2
*******************
TASK[0]  MASTER READ SENSOR( BH1750 )
*******************
data_h: 01
data_l: d0
sensor val: 386.67 [Lux]

## Troubleshooting

- BH1750 has two I2C address, which is decided by the voltage level of `ADDR` pin at start up. Make sure to check your schemetic before run this example.

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
