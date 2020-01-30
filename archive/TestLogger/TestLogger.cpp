/////////////////////////////////////////////////////////////////////////
// TestLogger.cpp - Logging to multiple streams                        //
//                  Demonstrates TestLogger<T> and QTestLogger<T>      //
//                                                                     //
// Jim Fawcett, Emeritus Teaching Professor, EECS, Syracuse University //
/////////////////////////////////////////////////////////////////////////

#include "TestLogger.h"
#include "QTestLogger.h"
#include "../TestUtilities/TestAssertions.h"
#include "../Display/Display.h"

int main() {

  using namespace Test;

  logLevel = Level::all;  // This should be a program run invariant.
                          // We will change here to test Level processing.

  std::cout << makeTitle("Testing Logger");

  TestLogger(&std::cout).post("-- logging to std::cout --");  // temporary logger
  TestLogger logger(&std::cout);  // equivalent to TestLogger<0,Level::all> via default params
  logger.postDated("Test log #1").post("first message").post("second message");
  logger.post("first message");
  logger.post("second message");
  logger.post("");

  logger.setPrefix("\n  -- ").setSuffix(" --");
  logger.postDated("Test log #2");
  logger.post("a message");
  std::cout << "\n";

  std::cout << "\n  logger has " << logger.streamCount() << " streams";
  logger.clear();
  std::cout << "\n  after clearing logger has " << logger.streamCount() << " streams";
  logger.addStream(&std::cout);

  logger.post("\n  -- testing level --");
  DebugLogger<0> debugLogger;
  debugLogger.addStream(&std::cout);
  DemoLogger<0> demoLogger;
  demoLogger.addStream(&std::cout);
  ResultsLogger<0> resultsLogger;
  resultsLogger.addStream(&std::cout);
  using AllLogger = TestLogger<0, Level::all>;
  AllLogger allLogger;
  allLogger.addStream(&std::cout);

  logLevel = Level::all;
  logger.post("logLevel = " + levelType(logLevel));
  debugLogger.post("debugLogger here");
  demoLogger.post("demoLogger here");
  resultsLogger.post("resultsLogger here");
  allLogger.post("allLogger here");

  logLevel = Level::debug;
  logger.post("logLevel = " + levelType(logLevel));
  debugLogger.post("debugLogger here");
  demoLogger.post("demoLogger here");
  resultsLogger.post("resultsLogger here");
  allLogger.post("allLogger here");

  logLevel = Level::demo;
  logger.post("logLevel = " + levelType(logLevel));
  debugLogger.post("debugLogger here");
  demoLogger.post("demoLogger here");
  resultsLogger.post("resultsLogger here");
  allLogger.post("allLogger here");

  logLevel = Level::results;
  logger.post("logLevel = " + levelType(logLevel));
  debugLogger.post("debugLogger here");
  demoLogger.post("demoLogger here");
  resultsLogger.post("resultsLogger here");
  allLogger.post("allLogger here");
  logLevel = Level::all;

  logger.post("\n  -- logging LogMessages --");
  LogMessage msg("github message");
  logger.postDated(msg);
  msg.clear();
  msg.add("send message").add("/with another part").add("/and still another part");
  logger.post(msg);

  logger.post("\n  -- logging to file stream --");
  std::ofstream oStrm;
  if (openFile("test.log", &oStrm)) {
    std::cout << "\n  opened file \"test.log\"";
    logger.addStream(&oStrm);
    logger.postDated("logging to std::cout and file test.log");
    logger.post("end of test log");
  }
  msg.clear();
  msg.add("\n  after leaving openFile scope, streamCount = ");
  msg.add(std::to_string(logger.streamCount()));
  logger.post(msg);
  logger.post("posting after leaving addStream scope");
  logger.clear();

  logger.addStream(&std::cout);
  logger.post("\n  -- logging to std::cout and two file streams --");
  std::ofstream strm2;
  if (openFile("newtest.log", &strm2)) {
    logger.addStream(&strm2);
  }
  FileReturn fr = openFile("newertest.log");
  auto pStrm = std::move(fr.second);
  if (fr.first) {
    if (pStrm->good())
      logger.addStream(pStrm.get());
  }
  logger.setPrefix("\n  ");
  logger.setSuffix("");
  logger.post("posting after leaving addStream with unique_ptr scope");

  logger.post("\n-- show logger type --");
  logger.post(logger.type() + "\n");
  logger.clear();  // should see two files close

  logger.addStream(&std::cout);
  logger.post("\n-- use factory and interface --");
  auto pLogger = createLogger<1,Level::results>();
  pLogger->postDated("factory logger log");
  pLogger->post("log message #1");
  pLogger->setPrefix("\n --").setSuffix(" --");
  pLogger->post("message with new prefix and suffix");
  putline();

  logger.post(makeTitle("Testing QLogger"));
  logger.post("-- logging to std::cout --");
  QTestLogger<0> qlogger;
  qlogger.addStream(&std::cout);
  qlogger.postDated("Test log #1").post("first message");
  qlogger.post("second message").post("");

  qlogger.setPrefix("\n  -- ").setSuffix(" --");
  qlogger.postDated("Test log #2");
  qlogger.post("a message");
  qlogger.wait();  // without this wait() some qlogger posts may arrive after the std::cout, below

  std::cout << "\n  qlogger has " << qlogger.streamCount() << " streams";
  qlogger.clear();
  std::cout << "\n  after clearing qlogger has " << qlogger.streamCount() << " streams";

  qlogger.addStream(&std::cout);
  qlogger.post("\n  -- logging LogMessages --");
  LogMessage qMsg("github message");
  qlogger.postDated(qMsg);
  qMsg.clear();
  qMsg.add("send message").add("/with another part").add("/and still another part");
  qlogger.post(qMsg);

  qlogger.post("\n  -- logging to file stream --");
  std::ofstream oQStrm;
  if (openFile("test.log", &oQStrm)) {
    std::cout << "\n  opened file \"test.log\"";
    qlogger.addStream(&oQStrm);
    qlogger.postDated("logging to std::cout and file test.log");
    qlogger.post("end of test log");
  }
  //qlogger.wait();
  qlogger.post(
    "\n  after leaving openFile scope, streamCount = " + 
    std::to_string(qlogger.streamCount())
  );
  qlogger.post("posting after leaving addStream scope");
  qlogger.clear();

  qlogger.addStream(&std::cout);
  qlogger.post("\n  -- logging to std::cout and two file streams --");
  std::ofstream qstrm2;
  if (openFile("newtest.log", &qstrm2)) {
    qlogger.addStream(&qstrm2);
  }
  FileReturn qfr = openFile("newertest.log");
  auto pQStrm = std::move(qfr.second);
  if (qfr.first) {
    if (pQStrm->good())
      qlogger.addStream(pQStrm.get());
  }
  qlogger.setPrefix("\n  ").setSuffix("");
  qlogger.post("posting after leaving addStream with unique_ptr scope");
  qlogger.post("\n  -- show QLogger type --");
  qlogger.post(qlogger.type() + "\n");
  qlogger.clear();

  auto pQlogger = createQLogger();
  pQlogger->post("\n  -- use factory and interface --");
  pQlogger->postDated("factory Qlogger log");
  pQlogger->post("log message #1");
  pQlogger->setPrefix("\n --").setSuffix(" --");
  pQlogger->post("message with new prefix and suffix");
  pQlogger->setPrefix("\n  ");
  pQlogger->setSuffix("");
  pQlogger->post("");
  pQlogger->wait();  // need to ensure that pQlogger and qSlogger posts don't interleave

  auto& qSlogger = getSingletonQLogger();
  qSlogger.post("-- use singleton factory and interface --");
  qSlogger.postDated("factory qSlogger log");
  qSlogger.post("log message #1");
  qSlogger.setPrefix("\n --").setSuffix(" --");
  qSlogger.post("message with new prefix and suffix");
  qSlogger.wait();

  pQlogger->post("\n  -- use timer --");
  pQlogger->setPrefix("\n  ").setSuffix("");
  std::ofstream tfstrm;
  openFile("timed.log", &tfstrm);
  pQlogger->addStream(&tfstrm);
  pQlogger->post("starting timer");
  pQlogger->start();
  pQlogger->post("measuring time for this post");
  double d = pQlogger->elapsedMicroseconds();
  std::string dStr = std::to_string(d);
  size_t index = dStr.find('.');
  dStr = dStr.substr(0, index);
  pQlogger->post("elapsed time in microsec = " + dStr);
  pQlogger->wait();
  std::cout << "\n  after posting and waiting:";
  std::cout << "\n  elapsed microsecs = " << pQlogger->elapsedMicroseconds();
  putline(2);
}