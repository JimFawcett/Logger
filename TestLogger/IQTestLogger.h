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

  template<Level L = Level::all>
  inline std::unique_ptr<IQTestLogger<L>> createQLogger(std::ostream* pStrm = &std::cout);

  template<size_t N = 0, Level L = Level::all>
  inline IQTestLogger<L>& getSingletonQLogger(std::ostream* pStrm = &std::cout);
}