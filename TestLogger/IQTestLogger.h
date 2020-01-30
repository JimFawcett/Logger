#pragma once
/////////////////////////////////////////////////////////////////////////
// IQTestLogger.h - Queued Logger interface                            //
// ver 1.1                                                             //
// Jim Fawcett, Emeritus Teaching Professor, EECS, Syracuse University //
/////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include "ITestLogger.h"

///////////////////////////////////////////////////////////
// IQTestLogger<T>
// - note virtual public inheritance

namespace Test {

  template<Level L>
  struct IQTestLogger : virtual public ITestLogger<L> {
    virtual ~IQTestLogger() {};
    virtual void wait() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual double elapsedMicroseconds() = 0;
  };
}