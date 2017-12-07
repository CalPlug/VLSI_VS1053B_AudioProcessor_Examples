#ifndef TOI_GP20U7
#define TOI_GP20U7

#ifndef SPARK
#include "../../platforms/platform.h"
#include "../../api/gps.h"
#include "tinygps.h"
#else
#include "platform.h"
#include "gps.h"
#include "tinygps.h"
#endif

class GP20U7 : public GeolocationDevice
{
  private:
    PlatformStream *_stream;
    TinyGPS _gps;

  public:
    GP20U7(PlatformStream &s);
    GP20U7(PlatformStream *s);
    Geolocation getGeolocation(void);
    int read(void);
    void begin(void);
};

#endif
