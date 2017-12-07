#include "gp20u7.h"
// TODO: Handle uninitialized _currentLocation

GP20U7::GP20U7(PlatformStream &s){
  _stream = &s;
}

GP20U7::GP20U7(PlatformStream *s){
  _stream = s;
}

Geolocation GP20U7::getGeolocation(){
    return _currentLocation;
}

int GP20U7::read(){
  long lat, lon;
  unsigned long fix_age;

  while(_stream->available()){
    int c = _stream->read();
    if(_gps.encode(c)){
      _gps.get_position(&lat, &lon, &fix_age);

      // Map tinygps values into Geolocation struct
      _currentLocation.latitude = lat / 1000000.0;
      _currentLocation.longitude = lon / 1000000.0;
      return 1;
    }
  }

  return 0;
}

void GP20U7::begin(){
  #if defined (ARDUINO_PLATFORM)
    reinterpret_cast<HardwareSerial *>(_stream)->begin(9600);
  #endif
}
