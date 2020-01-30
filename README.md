# Logger

https://JimFawcett.github.io/Logger.html

Send string messages to one or more std::iostreams concurrently.
Provides two loggers, each with iterfaces and object factories:
<ol>
    <li>
        TestLogger&lt;Level&gt; - Level determinse whether msg is sent
        <ul>
            <li>
                Leval:all
            <li>
                Leval:debug
            <li>
                Level:demo
            <li>
                Level:results
        </ul>
    <li>
        ITestLogger&lt;Level&gt; - interface
    <li>
        std::unique_ptr&gt;ITestLogger&lt;L&gt;&gt; createLogger()
    <li>
        ITestLogger&gt;L&lt;&amp; getSingletonLogger&lt;N&gt;() - parameter N supports additional singletons
    <li>
        QTestLogger&lt;Level&gt; - writes to queue, child thread writes to streams
    <li>
        IQTestLogger&lt;Level&gt; - interface
    <li>
        std::unique_ptr&lt;IQTestLogger&gt; createQLogger()
    <li>
        IQTestLogger&lt;L&gt;&amp; getSingletonQLogger&lt;N&gt;()
  
