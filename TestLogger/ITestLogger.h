#pragma once
/////////////////////////////////////////////////////////////////////////
// ITestLogger.h - Logger interface                                    //
//                                                                     //
// Jim Fawcett, Emeritus Teaching Professor, EECS, Syracuse University //
/////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

namespace Test {

  enum class Level { 
    results = 1, demo = 2, debug = 4, all = 7 
  };

  inline size_t levelValue(Level l) { 
    return static_cast<size_t>(l); 
  }

  inline std::string levelType(Level l) {
    switch (l) {
    case Level::all:
      return "Level::all";
    case Level::debug:
      return "Level::debug";
    case Level::demo:
      return "Level::demo";
    case Level::results:
      return "Level::results";
    default:
      return "whhaaattttt?";
    }
  }

  Level logLevel = Level::all;

  template<size_t N, Level L>
  struct ITestLogger {
    virtual ~ITestLogger() {}
    virtual void addStream(std::ostream* pOstream) = 0;
    virtual bool removeStream(std::ostream* pOstream) = 0;
    virtual size_t streamCount() = 0;
    virtual ITestLogger<N,L>& post(const std::string& msg) = 0;
    virtual ITestLogger<N, L>& postDated(const std::string& msg) = 0;
    virtual ITestLogger<N, L>& setPrefix(const std::string& prefix) = 0;
    virtual ITestLogger<N, L>& setSuffix(const std::string& suffix) = 0;
    virtual void clear() = 0;
    virtual std::string type() = 0;
  };
}