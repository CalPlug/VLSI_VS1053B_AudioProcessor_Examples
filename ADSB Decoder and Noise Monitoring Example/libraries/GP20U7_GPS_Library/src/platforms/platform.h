#ifndef TOI_PLATFORM_H
#define TOI_PLATFORM_H

// ARDUINO
#if defined(ARDUINO) && ARDUINO > 100
#define ARDUINO_PLATFORM

#include <Arduino.h>

typedef Stream PlatformStream;

// PARTICLE FAMILY
// The particle compiler uses a PLATFORM_ID to distinguish between core, 
// photon, electron, etc. For now just key off PLATFORM_ID but can be
// expanded later if breaking hardware differences arise.
#elif defined(SPARK)
#include "application.h"
typedef Stream PlatformStream;


#else
// GENERIC LINUX

#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>

#include <stdint.h>
#include <time.h>

typedef unsigned char byte;

class PlatformStream{

  public:
    virtual ~PlatformStream(void){};
    virtual int available(void) = 0;
    virtual int read(void) = 0;
    //virtual void flush(void) = 0;
    //virtual size_t write(uint8_t c) = 0;
};

class LinuxSerial : public PlatformStream{
  private:
    int _fd;
    char _device[64];

    struct termios _tio_original_config;
    struct termios _tio_config;
    struct sigaction _saio;

  public:
    LinuxSerial(const char *device);
    ~LinuxSerial(void);
    void begin(int baud);
    void end(void);
    int available(void);
    int read(void);
    int getfd(void);
};

#endif
#endif
