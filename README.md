# Using a Tiny Code Reader on an Arduino

Example code that shows how to interface an Arduino to [Useful 
Sensor's](https://usefulsensors.com) 
[Tiny Code Reader](https://usfl.ink/tcr) board over I2C.

## Introduction

The Tiny Code Reader is a small, low-cost hardware module that reads QR codes.
It's designed to be a simple way to provision a system, for example by 
providing the wifi network name and password, or to provide input when there's
no keyboard. 

There's a [detailed developer guide](https://usfl.ink/tcr_dev)
available, but this project has sample code that shows you specifically how to 
get the module up and running with a board using the Arduino IDE. It has been
tested with an Arduino Nano BLE Sense 33, but it should work with any board.

## Building

Open the `tiny_code_reader_arduino.ino` file in the Arduino IDE, and press 
upload to build and flash the example onto your board.

## Wiring Information

You'll need to consult your board's documentation to find out what the pins or
connectors used for the I2C interface are. Ideally you should find a board or
breakout that supports a standard connector like Qwiic or Grove, so you can
attach a cable from the sensor's Qwiic port.

## Running

Once the sketch is uploaded you should start to see information in the Serial
Monitor about the codes it spots, or messages about any errors. If you hold the
sensor so that it's pointing at a QR code you should see output like this:

```
Found 'http://en.m.wikipedia.org'
```
To get this output, I opened [an example QR code](https://en.wikipedia.org/wiki/QR_code#/media/File:QR_code_for_mobile_English_Wikipedia.svg)
on my phone and held it about fifteen centimeters or six inches from the 
module, facing the camera.

## Troubleshooting

The first thing to check is that the sensor is receiving power through the
`VDD` and `GND` wires. The simplest way to test this is to look at the LED on
the front of the module. Whenever it has power, it should be flashing blue
multiple times a second. If this isn't happening then there's likely to be a
wiring issue with the power connections.

## Arduino I2C Length Limits

Some older Arduino boards have a hard-coded 32-byte limit for I2C reads. This
[has caused problems with our sensors](https://github.com/usefulsensors/person_sensor_arduino/issues/2)
since we often need to return larger payloads, like URLs. The current
implementation tries to read in chunks to work around these problems but if you
are seeing incorrect results, you can try changing this constant in the
`tiny_code_reader.h` header file:

```C
const int maxBytesPerChunk = 64;
```

Changing it to 32 bytes may help on older boards. This isn't the default though
because some newer boards don't handle the chunked reading correctly.