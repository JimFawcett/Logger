#pragma once
/////////////////////////////////////////////////////////////////////////
// QTestLogger.h - Logs to multiple streams using post queue           //
//                                                                     //
// Jim Fawcett, Emeritus Teaching Professor, EECS, Syracuse University //
/////////////////////////////////////////////////////////////////////////
/*
   Package Responsibilities:
  ---------------------------
   Package provides IQTestLogger interface, QTestLogger class, and two factories:
   - Write log messages to multiple streams
   - Accept strings or messages convertible to string
   QTestLogger<N> posts to write queue.  Child thread deQs and writes to streams.
   - QTestLogger<N> provides:
     - post(msg) and postDated(msg)
     - addStream(pStrm), removeStream(pStrm), streamCount()
     - clear()
     - start(), stop(), and elapsedMicroseconds()
     - wait()
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
   IQTestLogger.h
   QTestLogger.h
   ITestLogger.h
   TestLogger.h, TestLogger.cpp (only for demonstration)
   DateTime.h, DateTime.cpp
   TypeTraits.h

   Maintenance History:
  ----------------------
   ver 1.0 : 26 Jan 2020
   - first release, 
     starting new version change due to redesign
*/

#include "IQTestLogger.h"
#include "TestLogger.h"
#include "../DateTime/DateTime.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../type_traits/TypeTraits.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <thread>
#include <memory>

namespace Test {

  /*-----------------------------------------------------------------
    The pragma below disables a warning about inheriting by dominance. 
    This code is correct. It uses multiple inheritance of classes 
    sharing a common base class.  The base classes here both derive
    from ITestLogger.  The code uses virtual inheritance to avoid
    duplicating that root base in QTestLogger.

    Normally I would avoid this structure needing virtual inheritance,
    but it is important that users of QTestLogger get full access to
    all its operations via the interface IQTestLogger, so IQTestLogger 
    needs to inherit from ITestLogger, as does TestLogger.

    QTestLogger implements IQTestLogger and inherits from TestLogger
    to share TestLogger's implementation.

    IQTestLogger inherits ITestLogger to provide access to all logger ops.
       ITestLogger is a shared root base class
    TestLogger implements ITestLogger to provide access to TestLogger ops.
  */
#pragma warning(disable : 4250)

  /////////////////////////////////////////////////////////
  // QTestLogger class

  template<size_t N = 0, Level L = Level::all>
  class QTestLogger : public IQTestLogger<N,L>, public TestLogger<N,L> {
  public:

    QTestLogger() {
      wthread = std::move(std::thread(&QTestLogger<N>::writeThreadProc, this));
    }
    QTestLogger(std::ostream& pStrm) {
      wthread = std::move(std::thread(&QTestLogger<N>::writeThreadProc, this));
      this->addStream(pStrm);
    }
    virtual ~QTestLogger();
    virtual void wait();
    virtual void start();
    virtual void stop();
    virtual double elapsedMicroseconds();
    virtual void clear() override;
    virtual ITestLogger<N,L>& post(const std::string& msg) override;
    virtual ITestLogger<N, L>& postDated(const std::string& msg) override;
  protected:
    void corePost(const std::string& msg);
    std::thread wthread;
    BlockingQueue<std::string> writeQ_;
    void writeThreadProc();
  };

  /*-- remove all streams, closing file streams --*/
  template<size_t N, Level L>
  QTestLogger<N, L>::~QTestLogger() {
    writeQ_.enQ("stop");
    if (wthread.joinable())
      wthread.join();
    clear();
  }
  /*-- wait for writeQ to empty --*/
  template<size_t N, Level L>
  void QTestLogger<N, L>::wait() {
    while (writeQ_.size() > 0)
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  /*-- start timer --*/
  template<size_t N, Level L>
  void QTestLogger<N, L>::start() {
    this->dt.start();
  }
  /*-- stop timer --*/
  template<size_t N, Level L>
  void QTestLogger<N, L>::stop() {
    this->dt.stop();
  }
  /*-- timer elapsed microseconds --*/
  template<size_t N, Level L>
  double QTestLogger<N, L>::elapsedMicroseconds() {
    return this->dt.elapsedMicroseconds();
  }
  /*-- remove all streams, reset prefix and suffix --*/
  template<size_t N, Level L>
  void QTestLogger<N, L>::clear() {
    wait();
    for (auto pStrm : TestLogger<N>::streams_)
      this->removeStream(pStrm);
    TestLogger<N>::prefix_ = "\n  ";
    TestLogger<N>::suffix_ = "";
  }
  /*-- function executed by write thread --*/
  template<size_t N, Level L>
  void QTestLogger<N, L>::writeThreadProc() {
    while (true) {
      std::string msg = writeQ_.deQ();
      if (msg == "stop")
        break;
      for (auto pStrm : TestLogger<N>::streams_) {
        (*pStrm) << msg;
      }
    }
  }
  /*-- write log message to all channels --*/
  template<size_t N, Level L>
  void QTestLogger<N, L>::corePost(const std::string& msg) {
    this->composite_ = this->prefix_ + msg + this->suffix_;
    writeQ_.enQ(this->composite_);
  }
  /*-- write log message to all channels --*/
  template<size_t N, Level L>
  ITestLogger<N, L>& QTestLogger<N, L>::post(const std::string& msg) {
    corePost(msg);
    return *this;
  }
  /*-- write dated log message to all channels --*/
  template<size_t N, Level L>
  ITestLogger<N, L>& QTestLogger<N, L>::postDated(const std::string& msg) {
    this->composite_ = msg + " : " + this->dt.now();
    corePost(this->composite_);
    return *this;
  }

  /////////////////////////////////////////////////
  // Logger factory functions
  // - return pointer or reference typed as IQTestLogger<N> interface

  /*-- return std::unique_ptr<IQTestLogger> bound to a new instance --*/
  template<size_t N = 0, Level L = Level::all>
  inline std::unique_ptr<IQTestLogger<N, L>> createQLogger(std::ostream* pStrm = &std::cout) {
    auto pQLogger = std::unique_ptr<QTestLogger<N, L>>(new QTestLogger<N>());
    pQLogger->addStream(pStrm);
    return pQLogger;
  }
  /*-- return reference to single static instance of logger --*/
  template<size_t N = 0, Level L = Level::all>
  inline IQTestLogger<N, L>& getSingletonQLogger(std::ostream* pStrm = &std::cout) {
    static QTestLogger<N, L> logger;
    if (logger.streamCount() == 0)
      logger.addStream(pStrm);
    return logger;
  }
}