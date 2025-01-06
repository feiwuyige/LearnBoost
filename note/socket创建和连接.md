# socket创建和连接

1. 网络编程的基本步骤：

   * 服务器：
     * `socket` : 创建 `socket` 对象
     * `bind` ：绑定本机 `ip + port` 
     * `listen` ：监听来电，如果有连接，则建立连接
     * `accept` ：再创建一个 `socket` 对象用来收发消息。有多个客户端，所以对于不同的客户端，服务器用不同的 `socket` 对象来进行收发消息。
     *  `read, write` ：进行消息的收发
   * 客户端：
     * `socket` ：创建 `socket` 对象
     * `connect` ：根据服务端的 `ip + port` ，发起连接请求
     *  `read，wirte` ：建立连接之后，收发消息

   **重点概念**：

   * `socket` ：一个抽象化的接口，提供了网络通信的标准方法。通过 Socket，应用程序可以与操作系统通信以访问底层的网络功能。本质是操作系统分配的一个资源，通常表示为一个文件描述符（在 Linux/Unix 中）或句柄（在 Windows 中）

2. 终端节点 `endpoint`：一个用于标识网络通信的目标或来源的逻辑概念。它定义了通信的一个端点，包括网络协议、IP 地址和端口号等信息。可以把终端节点看作是一个网络上的“通信地址”，用于唯一标识一台设备上的某个具体服务。

3. 创建终端节点：
   ```cpp
   boost::asio::tcp::endpoint ep(ip_address, port_num);
   ```

4. 创建 `socket` :

   * 创建上下文 `io context`
   * 选择协议
   * 生成 `socket` 
   * 打开 `socket`

   ```cpp
   boost::asio::io_context ios;
   boost::asio::ip::tcp protocol = boost::asio::ip::tcp::v4();
   boost::asio::tcp::socket sock(ios);
   boost::system::error_code ec;
   sock.open(protocol, ec);
   ```

   上述 `socket` 是建立通信时候使用的，对于服务端，还需要建立一个接受连接的 `socket`：
   ```cpp
   boost::asio::io_context ios;
   boost::asio::ip::tcp protocol = boost::asio::ip::tcp::v4();
   boost::asio::tcp::acceptor acceptor(ios);
   boost::system::error_code ec;
   acceptor.open(protocol, ec);
   ```

   **重点概念:**

   * `io_context` (https://stackoverflow.com/questions/60997939/what-exacty-is-io-context)：

     > **The io_context class provides the core I/O functionality for users of the asynchrnous(异步) I/O objects**.
     >
     > [`io_context`](https://www.boost.org/doc/libs/develop/doc/html/boost_asio/reference/io_context.html) contains the state required to run the [event-loop](https://en.wikipedia.org/wiki/Event_loop) in the thread using platform-specific calls, such as [`select`](http://man7.org/linux/man-pages/man2/select.2.html) or [`epoll`](http://man7.org/linux/man-pages/man7/epoll.7.html). Normally, there can be one (active) event loop per thread.

     提供了网络操作所需的底层资源：

     * 监听底层操作系统的 I/O 事件，并在事件完成以后调用相应的回调函数
     * 维护一个事件队列，通过调用 `run` 方法来启动事件循环，如果队列中没有事件，`run` 会阻塞并等待新的事件发生。
     * `socket` 使用 `io_context` 来注册事件，并依赖它来通知事件的发生

5. 绑定 `acceptor` ：对于服务端的处理接受操作的 `socket` ，需要将其绑定到一个终端节点，所有连接这个终端节点的连接都可以被接受到。
   ```cpp
   boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::any(), port_num);
   boost::asio::tcp::acceptor acceptor(ios, ep.protocol());
   boost::system::error_code ec;
   acceptor.bind(ep, ec); //绑定
   ```

6. 客户端连接指定端点：
   ```cpp
   boost::asio::tcp::socket sock(ios, ep.protocol());
   sock.connect(ep); //连接
   ```

7. 服务端接受连接：
   ```cpp
   acceptor.listen(BACKLOG_SIZE); //可以监听的连接数
   boost::asio::ip::tcp::socket sock(ios); //创建一个新 socket，用其进行连接
   acceptor.accept(sock);
   ```

   



