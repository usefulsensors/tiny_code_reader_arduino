#include <Wire.h>

#include "tiny_code_reader.h"

// How long to wait between reading the sensor. The sensor can be read as
// frequently as you like, but the results only change at about 5FPS, so
// waiting for 200ms is reasonable.
const int32_t SAMPLE_DELAY_MS = 200;

void setup() {
  // You need to make sure you call Wire.begin() in setup, or the I2C access
  // below will fail.
  Wire.begin();
  Serial.begin(115200);
}

void loop() {
  tiny_code_reader_results_t results = {};
  // Perform a read action on the I2C address of the sensor to get the
  // current face information detected.
  if (!tiny_code_reader_read(&results)) {
    Serial.println("No person sensor results found on the i2c bus");
    delay(SAMPLE_DELAY_MS);
    return;
  }

  if (results.content_length == 0) {
    Serial.println("No code found");
  } else {
    Serial.print("Found '");
    Serial.print((char*)results.content_bytes);
    Serial.println("'\n");
  }

  delay(SAMPLE_DELAY_MS);
}
