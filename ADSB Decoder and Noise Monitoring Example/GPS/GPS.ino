#include <gp20u7.h>

// initialize the library with the serial port to which the device is
// connected
GP20U7 gps = GP20U7(Serial1);

// Set up a Geolocation variable to track your current Location
Geolocation currentLocation;

void setup() {
  Serial.begin(115200);
  // Start the GPS module
  gps.begin();
}

void loop() {
  // The call to read() checks for new location information and returns 1
  // if new data is available and 0 if it isn't. It should be called
  // repeatedly.
  if(gps.read()){
    currentLocation = gps.getGeolocation();
    Serial.print(currentLocation.latitude,6);
    Serial.print(",");
    Serial.print(currentLocation.longitude,6);
    Serial.println();
  }
}
