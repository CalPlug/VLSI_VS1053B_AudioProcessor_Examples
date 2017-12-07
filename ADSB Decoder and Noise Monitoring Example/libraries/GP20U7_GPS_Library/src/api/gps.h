#ifndef TOI_API_GPS
#define TOI_API_GPS

typedef struct _Geolocation
{
  double latitude;
  double longitude;
} Geolocation, *PGeolocation;

class GeolocationDevice{
public:
  virtual ~GeolocationDevice(){}
  virtual Geolocation getGeolocation(void) = 0;
  virtual int read(void) = 0; // used for polling only
  virtual void begin(void) = 0;

protected:
  Geolocation _currentLocation;
};

#endif
