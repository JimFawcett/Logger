# Logger

https://JimFawcett.github.io/Logger.html

Send string messages to one or more std::iostreams concurrently.
Provides two loggers, each with iterfaces and object factories:
1. TestLogger&lt;Level&gt; - Level determinse whether msg is sent
  a. Leval:all
  b. Leval:debug
  c. Level:demo
  d. Level:results
2. ITestLogger - interface
3. std::unique_ptr<ITestLogger<L>> createLogger()
4. ITestLogger<L>& getSingletonLogger<N>() - parameter N supports additional singletons
  
5. QTestLogger<Level> - writes to queue, child thread writes to streams
6. IQTestLogger - interface
7. std::unique_ptr<IQTestLogger> createQLogger()
8. IQTestLogger<L>& getSingletonQLogger<N>()
  
