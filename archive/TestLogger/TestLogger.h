#pragma once
/////////////////////////////////////////////////////////////////////////
// TestLogger.h - Logs to multiple streams                             //
//                                                                     //
// Jim Fawcett, Emeritus Teaching Professor, EECS, Syracuse University //
/////////////////////////////////////////////////////////////////////////
/*
   Package Responsibilities:
  ---------------------------
   Package provides ITestLogger interface, TestLogger class, and two factories:
   - Write log messages to multiple streams
   - Accept strings or messages convertible to string
   - TestLogger<N> provides:
     - post(msg) and postDated(msg)
     - addStream(pStrm), removeStream(pStrm), streamCount()
     - clear()
     - setPrefix(prfx) and setSuffix(suffx)

   Requires:
  -----------
   - C++17 option:
     Properties > C/C++ > Language > C++ Language Standard 
                > ISO C++17 standard (/std:c++17)
   - Catch C Exception:
     Properties > C/C++ > Code Generation > Enable C++ Exceptions 
                > Yes with SEH Exceptions (/EHa)

   Dependencies:
  ---------------
   ITestLogger.h
   TestLogger.h, TestLogger.cpp (only for demonstration)
   DateTime.h, DateTime.cpp
   TypeTraits.h

   Maintenance History:
  ----------------------
   ver 1.0 : 26 Jan 2020
   - first release
*/

#include "ITestLogger.h"
#include "../DateTime/DateTime.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <memory>

namespace Test {

  /////////////////////////////////////////////////////////
  // TestLogger class
  // - note virtual public inheritance

  template<size_t N = 0, Level L = Level::all>
  class TestLogger : virtual public ITestLogger<N, L> {
  public:
    using Streams = std::vector<std::ostream*>;

    TestLogger() {}
    TestLogger(std::ostream* pStrm) {
      addStream(pStrm);
    }
    virtual ~TestLogger();
    virtual void addStream(std::ostream* pOstream) override;
    virtual bool removeStream(std::ostream* pStrm) override;
    virtual void clear() override;
    virtual size_t streamCount() override;
    virtual ITestLogger<N, L>& post(const std::string& msg) override;
    virtual ITestLogger<N, L>& postDated(const std::string& msg) override;
    virtual ITestLogger<N, L>& setPrefix(const std::string& prefix) override;
    virtual ITestLogger<N, L>& setSuffix(const std::string& suffix) override;
    virtual std::string type() override;
  protected:
    void corePost(const std::string& msg);
    Streams streams_;
    std::string prefix_ = "\n  ";
    std::string suffix_ = "";
    Utilities::DateTime dt;
    std::string composite_;
  };

  /*-- remove all streams, closing file streams --*/
  template<size_t N, Level L>
  TestLogger<N, L>::~TestLogger() {
    clear();
  }
  /*-- add ostream pointer, opens new log channel --*/
  template<size_t N, Level L>
  void TestLogger<N, L>::addStream(std::ostream* pOstream) {
    streams_.push_back(pOstream);
  }
  /*-- remove ostream pointer, closes log channel --*/
  template<size_t N, Level L>
  bool TestLogger<N, L>::removeStream(std::ostream* pStrm) {
    try {
      std::ofstream* pFile = dynamic_cast<std::ofstream*>(pStrm);
      if (pFile != nullptr) {
        std::cout << "\n -- closing file --";
        pFile->close();
        std::cout.flush();
      }
    }
    catch (std::exception & ex) {
      std::cout << "\n--" << ex.what();
    }
    catch (...) {
      std::cout << "\n -- SEH exception";
      // Stream in heap already deleted by std::unique_ptr
      // That happens when logger goes out of scope
    }
    Streams::iterator iter = std::find(streams_.begin(), streams_.end(), pStrm);
    if (iter == streams_.end())
      return false;
    streams_.erase(iter);
    return true;
  }
  /*-- remove all streams, reset prefix and suffix --*/
  template<size_t N, Level L>
  void TestLogger<N, L>::clear() {
    for (auto pStrm : streams_)
      removeStream(pStrm);
    prefix_ = "\n  ";
    suffix_ = "";
  }
  /*-- return number of open log channels --*/
  template<size_t N, Level L>
  size_t TestLogger<N, L>::streamCount() {
    return streams_.size();
  }
  /*-- private write log message to all channels --*/
  template<size_t N, Level L>
  void TestLogger<N, L>::corePost(const std::string& msg) {
    if (levelValue(L) & levelValue(logLevel)) {
      composite_ = prefix_ + msg + suffix_;
      for (auto pStrm : streams_) {
        (*pStrm) << composite_;
      }
    }
  }
  /*-- write log message to all channels --*/
  template<size_t N, Level L>
  ITestLogger<N, L>& TestLogger<N, L>::post(const std::string& msg) {
    corePost(msg);
    return *this;
  }
  /*-- write dated log message to all channels --*/
  template<size_t N, Level L>
  ITestLogger<N, L>& TestLogger<N, L>::postDated(const std::string& msg) {
    composite_ = msg + " : " + dt.now();
    corePost(composite_);
    return *this;
  }
  /*-- set new message prefix --*/
  template<size_t N, Level L>
  ITestLogger<N, L>& TestLogger<N, L>::setPrefix(const std::string& prefix) {
    prefix_ = prefix;
    return *this;
  }
  /*-- set new message suffix --*/
  template<size_t N, Level L>
  ITestLogger<N, L>& TestLogger<N, L>::setSuffix(const std::string& suffix) {
    suffix_ = suffix;
    return *this;
  }
  /*-- set new message suffix --*/
  template<size_t N, Level L>
  std::string TestLogger<N, L>::type() {
    return "N = " + std::to_string(N) + ", L = " + levelType(L);
  }

  /////////////////////////////////////////////////
  // Logger factory functions
  // - return pointer or reference typed as ITestLogger<N> interface

  /*-- return std::unique_ptr<IQTestLogger> bound to a new instance --*/

  template<size_t N = 0, Level L = Level::all>
  inline std::unique_ptr<ITestLogger<N, L>> createLogger(std::ostream* pStrm = &std::cout) {
    auto pLogger = std::unique_ptr<TestLogger<N, L>>(new TestLogger<N, L>());
    pLogger->addStream(pStrm);
    return pLogger;
  }
  /*-- return reference to static instance --*/

  template<size_t N = 0, Level L = Level::all>
  inline ITestLogger<N, L>& getSingletonLogger(std::ostream* pStrm = &std::cout) {
    static TestLogger<N, L> logger;
    if (logger.streamCount() == 0)
      logger.addStream(pStrm);
    return logger;
  }

  /////////////////////////////////////////////////
  // Common Logger Definitions

  template<size_t N = 0>
  using DebugLogger = TestLogger<N, Level::debug>;
  template<size_t N = 0>
  using DemoLogger = TestLogger<N, Level::demo>;
  template<size_t N = 0>
  using ResultsLogger = TestLogger <N, Level::results>;
  template<size_t N = 0>
  using AllLogger = TestLogger<N, Level::all>;

  inline std::string makeTitle(const std::string& tl) {
    return "\n  " + tl + "\n " + std::string(tl.size() + 2, '=');
  }

  inline std::string maketitle(const std::string& tl) {
    return "\n  " + tl + "\n " + std::string(tl.size() + 2, '-');
  }

  /////////////////////////////////////////////////
  // Open file helper methods

  /*-----------------------------------------------
    - opens filestream passed as pointer
    - user required to declare std::ofstream
      and pass its address
  */
  inline bool openFile(const std::string& filename, std::ofstream* pOstrm) {
    pOstrm->open(filename, std::ios::out);
    return pOstrm->good();
  }
  /*-----------------------------------------------
    - open filestream and return std::pair of
      - bool success
      - std::unique_ptr<std::ofstream> to stream
        in native heap
      - allows user to open file with just name
      - user does not need to delete pointer
  */
  using FileReturn = std::pair<bool, std::unique_ptr<std::ofstream>>;

  inline FileReturn openFile(const std::string& filename) {
    std::ofstream* pFile = new std::ofstream(filename, std::ios::out);
    return std::pair{ pFile->good(), std::unique_ptr<std::ofstream>(pFile) };
  }

  /////////////////////////////////////////////////
  // LogMessage class
  // - required to have cast to string method

  class LogMessage : public std::string {
  public:
    LogMessage(const std::string& preamble) {
      *this += preamble;
    }
    LogMessage& add(const std::string& content) {
      *this += content;
      return *this;
    }
    operator std::string() {
      return static_cast<std::string>(*this);
    }
    std::string toString() {
      return static_cast<std::string>(*this);
    }
    void clear() {
      *this = static_cast<LogMessage>("");
    }
  };
}