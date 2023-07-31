#ifndef INCLUDE_TINY_CODE_READER_H
#define INCLUDE_TINY_CODE_READER_H

// Definitions for the Useful Sensors Tiny Code Reader module.
// Includes the standard I2C address of the sensor, constants for the
// configuration commands, and the data structures used to communicate results
// to the main system.
// See the full developer guide at https://usfl.ink/tcr_dev for more information.

#include <stdint.h>

#include <Wire.h>

// The I2C address of the tiny code reader board.
#define TINY_CODE_READER_I2C_ADDRESS (0x0c)

// Configuration commands for the sensor. Write this as a byte to the I2C bus
// followed by a second byte as an argument value.
#define TINY_CODE_READER_REG_LED_STATE        (0x01)

// The following structures represent the data format returned from the code
// reader over the I2C communication protocol. The C standard doesn't
// guarantee the byte-wise layout of a regular struct across different
// platforms, so we add the non-standard (but widely supported) __packed__
// attribute to ensure the layouts are the same as the wire representation.

// The results returned from the module have a 16-bit unsigned integer
// representing the length of the code content, followed by 254 bytes
// containing the content itself, commonly as a UTF-8 string.
#define TINY_CODE_READER_CONTENT_BYTE_COUNT (254)
typedef struct __attribute__ ((__packed__)) {
    uint16_t content_length;
    uint8_t content_bytes[TINY_CODE_READER_CONTENT_BYTE_COUNT];
} tiny_code_reader_results_t;

// Fetch the latest results from the sensor. Returns false if the read didn't
// succeed.
inline bool tiny_code_reader_read(tiny_code_reader_results_t* results) {
    // For an explanation of why we're doing the read in chunks see 
    // https://www.arduino.cc/reference/en/language/functions/communication/wire/
    // In particular: "The Wire library implementation uses a 32 byte buffer, 
    // therefore any communication should be within this limit. Exceeding 
    // bytes in a single transmission will just be dropped."
    // I missed this warning in my initial implementatiom, which caused 
    // https://github.com/usefulsensors/person_sensor_arduino/issues/2 on
    // older boards like the Uno.
    const int maxBytesPerChunk = 64;
    const int totalBytes = sizeof(tiny_code_reader_results_t);
    int8_t* results_bytes = (int8_t*)(results);
    int index = 0;
    while (index < totalBytes) {
        const int bytesRemaining = totalBytes - index;
        const int bytesThisChunk = min(bytesRemaining, maxBytesPerChunk);
        const int endIndex = index + bytesThisChunk;
        const bool isLastChunk = (bytesRemaining <= maxBytesPerChunk);
        Wire.requestFrom(TINY_CODE_READER_I2C_ADDRESS, bytesThisChunk, isLastChunk);
        for (; index < endIndex; ++index) {
            if (Wire.available() < 1) {
                Serial.print("Only ");
                Serial.print(index);
                Serial.print(" bytes available on I2C, but we need ");
                Serial.println(bytesThisChunk);
                return false;
            }
            results_bytes[index] = Wire.read();
        }
    }
    // Make sure the content string is null terminated. Older firmware didn't
    // guarantee this, but all post-prototype modules do.
    if (results->content_length >= TINY_CODE_READER_CONTENT_BYTE_COUNT) {
        results->content_length = (TINY_CODE_READER_CONTENT_BYTE_COUNT - 1);
    }
    results->content_bytes[results->content_length] = 0;
    return true;
}

// Writes the value to the sensor register over the I2C bus.
inline void person_sensor_write_reg(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(TINY_CODE_READER_I2C_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

#endif  // INCLUDE_TINY_CODE_READER_H