# Logger

https://JimFawcett.github.io/Logger.html

Send string messages to one or more std::iostreams concurrently.
Provides two loggers, each with iterfaces and object factories:
1. TestLogger&lt;Level&gt; - Level determinse whether msg is sent

  a. Leval:all
  
  b. Leval:debug
  
  c. Level:demo
  
  d. Level:results
  
2. ITestLogger&lt;Level&gt; - interface

3. std::unique_ptr&gt;ITestLogger&lt;L&gt;&gt; createLogger()

4. ITestLogger&gt;L&lt;&amp; getSingletonLogger&lt;N&gt;() - parameter N supports additional singletons
  
5. QTestLogger&lt;Level&gt; - writes to queue, child thread writes to streams

6. IQTestLogger&lt;Level&gt; - interface

7. std::unique_ptr&lt;IQTestLogger&gt; createQLogger()

8. IQTestLogger&lt;L&gt;&amp; getSingletonQLogger&lt;N&gt;()
  
