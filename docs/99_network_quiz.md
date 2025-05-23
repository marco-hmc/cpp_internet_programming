1/socket 和 epoll 的关系区别?(不太懂他意思,反正对 epoll 的底层进行了阐述)

IO 模型主要有哪些？阻塞和非阻塞？
同步与异步的区别？
Select，poll 和 epoll 的区别？

Pthread_cond_signal 和 pthread_cond_broadcast 的区别 TCP 三次握手和四次挥手及各自的状态？
TCP 如果两次握手会出什么问题？
那三次握手又会造成什么问题？
有什么好的解决方法没？
TCP 四次挥手为什么要有 TIME_WAIT 状态？
为什么？

#### **网络通信部分**

- 网络通信部分,主要有两大块,

1. 第一个是程序启动时,与服务端的交互;
2. 第二个就是文件下载与分享的 P2P 网络.

Prerequisites:

1. 具体的就是 Socket 的各种 API 函数,以及基于这些 API 逻辑的组合
2. 当然可能也会用到操作系统平台所特有的网络 API 函数,如 **WSAAsyncSelect** 网络模型.

Design Points:

1. 网络通信部分如何与 UI 部分进行数据交换:
   是使用队列?
   如果使用队列,多线程之间如何保持资源的一致性和解决资源竞态,使用 Event/CriticalSection/Mutex/Semaphore 等?
   全局变量?
   或者相应的 Windows 操作平台提供的特殊通信技术,如 PostMessage 函数/管道?

2/epoll 和 select 的区别
3/epoll 的高效,有几种工作模式( LT/ET)
5/TIMEWAIT 是什么,为什么要设置 TIMEWAIT 状态
6/TCP 的可靠性是如何实现的?(流量控制/拥塞控制/确认序号/校验???)

2/交给 sub 线程的文件描述符如何回收的?
3/tcp 粘包

4/tcp 三次握手 /2 次/4 次?
5/tcp 可靠性的实现原因
6/滑动窗口的作用

8/tcp 是如何处理粘包?

9/tcp 是如何关闭的?就是说 4 次挥手过程

10/如果 tcp 关闭的第四个分节没有被收到,那么怎么办?

11/tcp 的拥塞控制过程

3/这个 io 复用模型跟普通多线程/多进程的区别
答: io 复用,多个链接复用一个线程,而普通多线程是一个链接一个线程,所以必然创建线程的数目就多,现成的创建是有开销的,所以 io 复用这种开销小.

4/网络传输的数据协议
主要有 3 个段,前 8bit 是数据 type,中间是 length,最后是具体的 data.

5/如果协议是这样.如果两个数据包连在一起,怎么分开?
答:在首部再加几个 bit,作为隔离位.

6/如何实现你的 fd 是负载均衡的
答:分类处理

7/多个用户访问同一个资源,如何加锁
答:不太明白他的意思,epoll 内部处理事件是同步依次执行的,而且这里的锁是线程资源

1.A 机器的进程给 B 机器的进程发送数据，经历哪些网络层?如何找到目标进程?
基本答出来了。

2.如何解决粘包和半包问题?实际写过这方面的网络通信程序吗?
答得不是很清楚。未写过。

3.对于 http 协议，如何判断当前的数据是否足够一个完整的 http 数据包?
面试者答的由 TCP 层保证，答得不对，读者的实践经历写了实现一个简易 Web 服务器，未回答出来如何解析 http 数据包。

5.select 函数的用法基本未答出来，

6.epoll 模型的水平模式和边缘模式有什么区别?
说不清楚，与简历中描述的不符合。

7.什么是 reactor 模型?什么是 proactor 模型?
基本答出来了。

9.多线程之间同步用过哪些锁?实际写过这个代码?哪些情形会造成死锁?
基本答不出来。

10.Redis 常用的数据类型有哪些?
说不完整。

11.设计一个表结构高效地存储一篇帖子下面的评论以及评论的回复,写出来。

12. os 是如何支持线程和进程操作的？原理是什么？
13. os 中的中断
14. 什么是上下文切换，操作系统是怎么做的上下文切换?

15. I/0 多路复用是什么
16. http 接口的函数用过吗
    网络库 libevent;

## 服务器端发数据时，如果对端一直不收，怎么办？

这类问题一般出现在跨部门尤其是与外部开发人员合作的时候。假设现在有这样一种情况，我们的服务器提供对外的服务，指定好了协议，然后对外提供服务，客户端由外部人员去开发，由于存在太多的不确定性，如果我们在给对端（客户端）发送数据时，对端因为一些问题（可能是逻辑 bug 或者其他的一些问题）一直不从 socket 系统缓冲区中收取数据，而服务器端可能定期产生一些数据需要发送给客户端，再发了一段时间后，由于 TCP 窗口太小，导致数据发送不出去，这样待发送的数据会在服务器端对应的连接的发送缓冲区中积压，如果我们不做任何处理，很快系统就会因为缓冲区过大内存耗尽，导致服务被系统杀死。

对于这种情况，我们一般建议从以下几个方面来增加一些防御措施：

1. 设置每路发送连接的发送缓冲区大小上限（如 2 M，或者小于这个值），当某路连接上的数据发送不出去的时候，即将数据存入发送缓冲区时，先判断一下缓冲区最大剩余空间，如果剩余空间已经小于我们要放入的数据大小，也就是说缓冲区中数据大小会超过了我们规定的上限，则认为该连接出现了问题，关闭该路连接并回收相应的资源（如清空缓冲区、回收套接字资源等）。示例代码如下：

   ```
   //outputBuffer_为发送缓冲区对象
   size_t remainingLen = outputBuffer_.remainingBytes();
   //如果加入到缓冲区中的数据长度超出了发送缓冲区最大剩余量
   if (remainingLen < dataToAppend.length())
   {
         forceClose()
         return
   }

   outputBuffer_.append(static_cast<const char*>(dataToAppend.c_str()), dataToAppend.length());
   ```

2. 还有另外一种场景，当有一部分数据已经积压在发送缓冲区了，此后服务器端未产生新的待发送的数据，此时如果不做任何处理，发送缓冲区的数据会一直积压，但是发送缓冲区的数据容量也不会超过上限。如果不做任何处理的话，该数据会一直在缓冲区中积压，白白浪费系统资源。对于这种情况一般我们会设置一个定时器，每隔一段时间（如 3 秒）去检查一下各路连接的发送缓冲区中是否还有数据未发送出去，也就是说如果一个连接超过一定时间内还存在未发送出去的数据，我们也认为该连接出现了问题，我们可以关闭该路连接并回收相应的资源（如清空缓冲区、回收套接字资源等）。示例代码如下：

   ```
   //每3秒检测一次
   const int SESSION_CHECK_INTERVAL = 3000;

   SetTimer(SESSION_CHECK_TIMER_ID, SESSION_CHECK_INTERVAL);

   void CSessionManager::OnTimer()
   {
       for (auto iter = m_mapSession.begin(); iter != m_mapSession.end(); ++iter)
       {
           if (!CheckSession(iter->value))
           {
               //关闭session，回收相关的资源
               iter->value->ForceClose();

               iter = m_mapSession.erase(iter);
           }
       }
   }

   void CSessionManager::CheckSession(CSession* pSession)
   {
       if (!pSession->GetConnection().OutputBuffer.IsEmpty())
           return false;

       return true;
   }
   ```

   上述代码，每隔 3 秒检测所有的 Session 的对应的 Connection 对象，如果发现发送缓冲区非空，说明该连接中发送缓冲区中数据已经驻留 3 秒了，将该连接关闭并清理资源。

## 网络通信中收发数据的正确姿势

在网络通信中，我们可能既要通过 socket 去发送数据也要通过 socket 来收取数据。那么一般的网络通信框架是如何收发数据的呢？注意，这里讨论的范围是基于各种 IO 复用函数（select、poll、epoll 等）来判断 socket 读写来收发数据，其他情形比较简单，这里就不提了。

我们这里以服务器端为例。服务器端接受客户端连接后，产生一个与客户端连接对应的 socket（Linux 下也叫 fd，为了叙述方便，以后称之为 clientfd），我们可以通过这个 clientfd 收取从客户端发来的数据，也可以通过这个 clientfd 将数据发往客户端。但是收与发在操作流程上是有明显的区别的。

#### 收数据的正确姿势

对于收数据，当接受连接成功得到 clientfd 后，我们会将该 clientfd 绑定到相应的 IO 复用函数上并监听其可读事件。不同的 IO 复用函数可读事件标志不一样，例如对于 poll 模型，可读标志是 POLLIN，对于 epoll 模型，可读事件标志是 EPOLLIN。当可读事件触发后，我们调用 recv 函数从 clientfd 上收取数据（这里不考虑出错的情况），根据不同的网络模式我们可能会收取部分，或一次性收完。收取到的数据我们会放入接收缓冲区内，然后做解包操作。这就是收数据的全部“姿势”。对于使用 epoll 的 LT 模式（水平触发模式），我们每次可以只收取部分数据；但是对于 ET 模式（边缘触发模式），我们必须将本次收到的数据全部收完。

> ET 模式收完的标志是 recv 或者 read 函数的返回值是 -1，错误码是 EWOULDBLOCK，针对 Windows 和 Linux 下区别，前面章节已经详细地说过了。

这就是读数据的全部姿势。流程图如下：

![](../imgs/sendway1.webp)

#### 发数据的正确姿势

对于发数据，除了 epoll 模型的 ET 模式外，epoll 的 LT 模式或者其他 IO 复用函数，我们通常都不会去注册监听该 clientfd 的可写事件。这是因为，只要对端正常收数据，一般不会出现 TCP 窗口太小导致 send 或 write 函数无法写的问题。因此大多数情况下，clientfd 都是可写的，如果注册了可写事件，会导致一直触发可写事件，而此时不一定有数据需要发送。故而，如果有数据要发送一般都是调用 send 或者 write 函数直接发送，如果发送过程中， send 函数返回 -1，并且错误码是 EWOULDBLOCK 表明由于 TCP 窗口太小数据已经无法写入时，而仍然还剩下部分数据未发送，此时我们才注册监听可写事件，并将剩余的服务存入自定义的发送缓冲区中，等可写事件触发后再接着将发送缓冲区中剩余的数据发送出去，如果仍然有部分数据不能发出去，继续注册可写事件，当已经无数据需要发送时应该立即移除对可写事件的监听。这是目前主流网络库的做法。

流程图如下：

![](../imgs/sendway2.webp)

上述逻辑示例如下：

**直接尝试发送消息处理逻辑：**

```
/**
 *@param data 待发送的数据
 *@param len  待发送数据长度
 */
void TcpConnection::sendMessage(const void* data, size_t len)
{
    int32_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    if (state_ == kDisconnected)
    {
        LOGW("disconnected, give up writing");
        return;
    }

    // 当前未监听可写事件，且发送缓冲区中没有遗留数据
    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
    {
        //直接发送数据
        nwrote = sockets::write(channel_->fd(), data, len);
        if (nwrote >= 0)
        {
            remaining = len - nwrote;
        }
        else // nwrote < 0
        {
            nwrote = 0;
            //错误码不等于EWOULDBLOCK说明发送出错了
            if (errno != EWOULDBLOCK)
            {
                LOGSYSE("TcpConnection::sendInLoop");
                if (errno == EPIPE || errno == ECONNRESET)
                {
                    faultError = true;
                }
            }
        }
    }

    //发送未出错且还有剩余字节未发出去
    if (!faultError && remaining > 0)
    {
        //将剩余部分加入发送缓冲区
        outputBuffer_.append(static_cast<const char*>(data) + nwrote, remaining);
        if (!channel_->isWriting())
        {
            //注册可写事件
            channel_->enableWriting();
        }
    }
}
```

**不能全部发出去监听可写事件后，可写事件触发后处理逻辑：**

```
//可写事件触发后会调用handleWrite()函数
void TcpConnection::handleWrite()
{
    //将发送缓冲区中的数据发送出去
    int32_t n = sockets::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
    if (n > 0)
    {
        //发送多少从发送缓冲区移除多少
        outputBuffer_.retrieve(n);
        //如果发送缓冲区中已经没有剩余，则移除监听可写事件
        if (outputBuffer_.readableBytes() == 0)
        {
            //移除监听可写事件
            channel_->disableWriting();

            if (state_ == kDisconnecting)
            {
                shutdown();
            }
        }
    }
    else
    {
        //发数据出错处理
        LOGSYSE("TcpConnection::handleWrite");
        handleClose();
    }
}
```

对于 epoll LT 模式注册监听一次可写事件后，可写事件触发后，尝试发送数据，如果数据此时还不能全部发送完，不用再次注册可写事件；如果是 epoll 的 ET 模式，注册监听可写事件后，可写事件触发后，尝试发送数据，如果数据此时还不能全部发送完，需要再次注册可写事件以便让可写事件下次再次触发（给予再次发数据的机会）。当然，这只是理论上的情况，实际开发中，如果一段数据反复发送都不能完全发送完（例如对端先不收，后面每隔很长时间再收一个字节），我们可以设置一个最大发送次数或最大发送总时间，超过这些限定，我们可以认为对端出了问题，应该立即清空发送缓冲区并关闭连接。

本节的标题是“收发数据的正确姿势”，其实还可以换一种说法，即“检测网络事件的正确姿势”，这里意指检测一个 fd 的读写事件的区别（对于侦听 fd，只检测可读事件）：

- 在 select、poll 和 epoll 的 LT 模式下，可以直接设置检测 fd 的可读事件；
- 在 select、poll 和 epoll 的 LT 模式下不要直接设置检测 fd 的可写事件，应该先尝试发送数据，因为 TCP 窗口太小发不出去再设置检测 fd 的可写事件，一旦数据发出去应立即取消对可写事件的检测。
- 在 epoll 的 ET 模式下，需要发送数据时，每次都要设置检测可写事件。

## 非阻塞模式下 send 和 recv 函数的返回值

我们来总结一下 **send** 和 **recv** 函数的各种返回值意义：

|   返回值 n    |                                            返回值含义                                             |
| :-----------: | :-----------------------------------------------------------------------------------------------: |
|    大于 0     |                                         成功发送 n 个字节                                         |
|       0       |                                           对端关闭连接                                            |
| 小于 0（ -1） | 出错或者被信号中断或者对端 TCP 窗口太小数据发不出去（send）或者当前网卡缓冲区已无数据可收（recv） |

我们来逐一介绍下这三种情况：

- **返回值大于 0**

  对于 **send** 和 **recv** 函数返回值大于 **0**，表示发送或接收多少字节，需要注意的是，在这种情形下，我们一定要判断下 send 函数的返回值是不是我们期望发送的缓冲区长度，而不是简单判断其返回值大于 0。举个例子：

  ```
  1int n = send(socket, buf, buf_length, 0)；
  2if (n > 0)
  3{
  4  printf("send data successfully\n");
  5}
  ```

  很多新手会写出上述代码，虽然返回值 n 大于 0，但是实际情形下，由于对端的 TCP 窗口可能因为缺少一部分字节就满了，所以返回值 n 的值可能在 (0, buf_length] 之间，当 0 < n < buf_length 时，虽然此时 send 函数是调用成功了，但是业务上并不算正确，因为有部分数据并没发出去。你可能在一次测试中测不出 n 不等于 buf_length 的情况，但是不代表实际中不存在。所以，建议要么认为返回值 n 等于 buf_length 才认为正确，要么在一个循环中调用 send 函数，如果数据一次性发不完，记录偏移量，下一次从偏移量处接着发，直到全部发送完为止。

```
1  //推荐的方式一
2  int n = send(socket, buf, buf_length, 0)；
3  if (n == buf_length)
4  {
5      printf("send data successfully\n");
6  }
```

```
 1//推荐的方式二：在一个循环里面根据偏移量发送数据
 2bool SendData(const char* buf , int buf_length)
 3{
 4    //已发送的字节数目
 5    int sent_bytes = 0;
 6    int ret = 0;
 7    while (true)
 8    {
 9        ret = send(m_hSocket, buf + sent_bytes, buf_length - sent_bytes, 0);
10        if (nRet == -1)
11        {
12            if (errno == EWOULDBLOCK)
13            {
14                //严谨的做法，这里如果发不出去，应该缓存尚未发出去的数据，后面介绍
15                break;
16            }
17            else if (errno == EINTR)
18                continue;
19            else
20                return false;
21        }
22        else if (nRet == 0)
23        {
24            return false;
25        }
26
27        sent_bytes += ret;
28        if (sent_bytes == buf_length)
29            break;
30
31        //稍稍降低 CPU 的使用率
32        usleep(1);
33    }
34
35    return true;
36}
```

- **返回值等于 0**

  通常情况下，如果 **send** 或者 **recv** 函数返回 **0**，我们就认为对端关闭了连接，我们这端也关闭连接即可，这是实际开发时最常见的处理逻辑。

  但是，现在还有一种情形就是，假设调用 **send** 函数传递的数据长度就是 0 呢？**send** 函数会是什么行为？对端会 **recv** 到一个 0 字节的数据吗？需要强调的是，**在实际开发中，你不应该让你的程序有任何机会去 send 0 字节的数据，这是一种不好的做法。** 这里仅仅用于实验性讨论，我们来通过一个例子，来看下 **send** 一个长度为 **0** 的数据，**send** 函数的返回值是什么？对端会 **recv** 到 **0** 字节的数据吗？

  **server** 端代码：

```
 1  /**
 2   * 验证recv函数接受0字节的行为，server端，server_recv_zero_bytes.cpp
 3   * zhangyl 2018.12.17
 4   */
 5  #include <sys/types.h>
 6  #include <sys/socket.h>
 7  #include <arpa/inet.h>
 8  #include <unistd.h>
 9  #include <iostream>
10  #include <string.h>
11  #include <vector>
12
13  int main(int argc, char* argv[])
14  {
15      //1.创建一个侦听socket
16      int listenfd = socket(AF_INET, SOCK_STREAM, 0);
17      if (listenfd == -1)
18      {
19          std::cout << "create listen socket error." << std::endl;
20          return -1;
21      }
22
23      //2.初始化服务器地址
24      struct sockaddr_in bindaddr;
25      bindaddr.sin_family = AF_INET;
26      bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
27      bindaddr.sin_port = htons(3000);
28      if (bind(listenfd, (struct sockaddr *)&bindaddr, sizeof(bindaddr)) == -1)
29      {
30          std::cout << "bind listen socket error." << std::endl;
31          close(listenfd);
32          return -1;
33      }
34
35      //3.启动侦听
36      if (listen(listenfd, SOMAXCONN) == -1)
37      {
38          std::cout << "listen error." << std::endl;
39          close(listenfd);
40          return -1;
41      }
42
43      int clientfd;
44
45      struct sockaddr_in clientaddr;
46      socklen_t clientaddrlen = sizeof(clientaddr);
47      //4. 接受客户端连接
48      clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientaddrlen);
49      if (clientfd != -1)
50      {
51          while (true)
52          {
53              char recvBuf[32] = {0};
54              //5. 从客户端接受数据,客户端没有数据来的时候会在recv函数处阻塞
55              int ret = recv(clientfd, recvBuf, 32, 0);
56              if (ret > 0)
57              {
58                  std::cout << "recv data from client, data: " << recvBuf << std::endl;
59              }
60              else if (ret == 0)
61              {
62                  std::cout << "recv 0 byte data." << std::endl;
63                  continue;
64              }
65              else
66              {
67                  //出错
68                  std::cout << "recv data error." << std::endl;
69                  break;
70              }
71          }
72      }
73
74
75      //关闭客户端socket
76      close(clientfd);
77      //7.关闭侦听socket
78      close(listenfd);
79
80      return 0;
81  }
```

上述代码侦听端口号是 **3000**，代码 **55** 行调用了 **recv** 函数，如果客户端一直没有数据，程序会阻塞在这里。

**client** 端代码：

```
 1/**
 2 * 验证非阻塞模式下send函数发送0字节的行为，client端，nonblocking_client_send_zero_bytes.cpp
 3 * zhangyl 2018.12.17
 4 */
 5#include <sys/types.h>
 6#include <sys/socket.h>
 7#include <arpa/inet.h>
 8#include <unistd.h>
 9#include <iostream>
10#include <string.h>
11#include <stdio.h>
12#include <fcntl.h>
13#include <errno.h>
14
15#define SERVER_ADDRESS "127.0.0.1"
16#define SERVER_PORT     3000
17#define SEND_DATA       ""
18
19int main(int argc, char* argv[])
20{
21    //1.创建一个socket
22    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
23    if (clientfd == -1)
24    {
25        std::cout << "create client socket error." << std::endl;
26        return -1;
27    }
28
29    //2.连接服务器
30    struct sockaddr_in serveraddr;
31    serveraddr.sin_family = AF_INET;
32    serveraddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
33    serveraddr.sin_port = htons(SERVER_PORT);
34    if (connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
35    {
36        std::cout << "connect socket error." << std::endl;
37        close(clientfd);
38        return -1;
39    }
40
41    //连接成功以后，我们再将 clientfd 设置成非阻塞模式，
42    //不能在创建时就设置，这样会影响到 connect 函数的行为
43    int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
44    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
45    if (fcntl(clientfd, F_SETFL,  newSocketFlag) == -1)
46    {
47        close(clientfd);
48        std::cout << "set socket to nonblock error." << std::endl;
49        return -1;
50    }
51
52    //3. 不断向服务器发送数据，或者出错退出
53    int count = 0;
54    while (true)
55    {
56        //发送 0 字节的数据
57        int ret = send(clientfd, SEND_DATA, 0, 0);
58        if (ret == -1)
59        {
60            //非阻塞模式下send函数由于TCP窗口太小发不出去数据，错误码是EWOULDBLOCK
61            if (errno == EWOULDBLOCK)
62            {
63                std::cout << "send data error as TCP Window size is too small." << std::endl;
64                continue;
65            }
66            else if (errno == EINTR)
67            {
68                //如果被信号中断，我们继续重试
69                std::cout << "sending data interrupted by signal." << std::endl;
70                continue;
71            }
72            else
73            {
74                std::cout << "send data error." << std::endl;
75                break;
76            }
77        }
78        else if (ret == 0)
79        {
80            //对端关闭了连接，我们也关闭
81            std::cout << "send 0 byte data." << std::endl;
82        }
83        else
84        {
85            count ++;
86            std::cout << "send data successfully, count = " << count << std::endl;
87        }
88
89        //每三秒发一次
90        sleep(3);
91    }
92
93    //5. 关闭socket
94    close(clientfd);
95
96    return 0;
97}
```

**client** 端连接服务器成功以后，每隔 3 秒调用 **send** 一次发送一个 0 字节的数据。除了先启动 **server** 以外，我们使用 tcpdump 抓一下经过端口 **3000** 上的数据包，使用如下命令：

```
1tcpdump -i any 'tcp port 3000'
```

然后启动 **client** ，我们看下结果：

![](../imgs/sendrecvreturnvalue1.png)

客户端确实是每隔 3 秒 **send** 一次数据。此时我们使用 **lsof -i -Pn** 命令查看连接状态，也是正常的：

![img](data:image/gif;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVQImWNgYGBgAAAABQABh6FO1AAAAABJRU5ErkJggg==)

然后，tcpdump 抓包结果输出中，除了连接时的三次握手数据包，再也无其他数据包，也就是说，**send** 函数发送 **0** 字节数据，**client** 的协议栈并不会把这些数据发出去。

```
1[root@localhost ~]# tcpdump -i any 'tcp port 3000'
2tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
3listening on any, link-type LINUX_SLL (Linux cooked), capture size 262144 bytes
417:37:03.028449 IP localhost.48820 > localhost.hbci: Flags [S], seq 1632283330, win 43690, options [mss 65495,sackOK,TS val 201295556 ecr 0,nop,wscale 7], length 0
517:37:03.028479 IP localhost.hbci > localhost.48820: Flags [S.], seq 3669336158, ack 1632283331, win 43690, options [mss 65495,sackOK,TS val 201295556 ecr 201295556,nop,wscale 7], length 0
617:37:03.028488 IP localhost.48820 > localhost.hbci: Flags [.], ack 1, win 342, options [nop,nop,TS val 201295556 ecr 201295556], length 0
```

因此，**server** 端也会一直没有输出，如果你用的是 gdb 启动 **server**，此时中断下来会发现，**server** 端由于没有数据会一直阻塞在 **recv** 函数调用处（**55** 行）。

![](../imgs/sendrecvreturnvalue2.png)

上述示例再次验证了，**send** 一个 0 字节的数据没有任何意思，希望读者在实际开发时，避免写出这样的代码。

## 服务器开发通信协议设计介绍

![](../imgs/protocol1.webp)

## 一、选择 TCP 还是 UDP 协议

由于我们的即时通讯软件的用户存在用户状态问题，即用户登录成功以后可以在他的好友列表中看到哪些好友在线，所以客户端和服务器需要保持长连接状态。另外即时通讯软件一般要求信息准确、有序、完整地到达对端，而这也是 TCP 协议的特点之一。综合这两个所以这里我们选择 TCP 协议，而不是 UDP 协议。

## 二、协议的结构

由于 TCP 协议是流式协议，所谓流式协议即通讯的内容是无边界的字节流：如 A 给 B 连续发送了三个数据包，每个包的大小都是 100 个字节，那么 B 可能会一次性收到 300 个字节；也可能先收到 100 个字节，再收到 200 个字节；也可能先收到 100 个字节，再收到 50 个字节，再收到 150 个字节；或者先收到 50 个字节，再收到 50 个字节，再收到 50 个字节，最后收到 150 个字节。也就是说，B 可能以任何组合形式收到这 300 个字节。即像水流一样无明确的边界。为了能让对端知道如何给包分界，目前一般有三种做法：

1. 以固定大小字节数目来分界，上文所说的就是属于这种类型，如每个包 100 个字节，对端每收齐 100 个字节，就当成一个包来解析；
2. 以特定符号来分界，如每个包都以特定的字符来结尾（如\n），当在字节流中读取到该字符时，则表明上一个包到此为止。
3. 固定包头+包体结构，这种结构中一般包头部分是一个固定字节长度的结构，并且包头中会有一个特定的字段指定包体的大小。这是目前各种网络应用用的最多的一种包格式。

上面三种分包方式各有优缺点，方法 1 和方法 2 简单易操作，但是缺点也很明显，就是很不灵活，如方法一当包数据不足指定长度，只能使用占位符如 0 来凑，比较浪费；方法 2 中包中不能有包界定符，否则就会引起歧义，也就是要求包内容中不能有某些特殊符号。而方法 3 虽然解决了方法 1 和方法 2 的缺点，但是操作起来就比较麻烦。我们的即时通讯协议就采用第三种分包方式。所以我们的协议包的包头看起来像这样：

```
struct package_header
{
    int32_t bodysize;
};
```

一个应用中，有许多的应用数据，拿我们这里的即时通讯来说，有注册、登录、获取好友列表、好友消息等各种各样的协议数据包，而每个包因为业务内容不一样可能数据内容也不一样，所以各个包可能看起来像下面这样：

```
struct package_header
{
    int32_t bodysize;
};

//登录数据包
struct register_package
{
    package_header header;
    //命令号
    int32_t cmd;
    //注册用户名
    char username[16];
    //注册密码
    char password[16];
    //注册昵称
    char nickname[16];
    //注册手机号
    char mobileno[16];
};

//登录数据包
struct login_package
{
    package_header header;
    //命令号
    int32_t cmd;
    //登录用户名
    char username[16];
    //密码
    char password[16];
    //客户端类型
    int32_t clienttype;
    //上线类型，如在线、隐身、忙碌、离开等
    int32_t onlinetype;
};

//获取好友列表
struct getfriend_package
{
    package_header header;
    //命令号
    int32_t cmd;
};

//聊天内容
struct chat_package
{
    package_header header;
    //命令号
    int32_t cmd;
    //发送人userid
    int32_t senderid;
    //接收人userid
    int32_t targetid;
    //消息内容
    char chatcontent[8192];
};
```

看到没有？由于每一个业务的内容不一样，定义的结构体也不一样。如果业务比较多的话，我们需要定义各种各样的这种结构体，这简直是一场噩梦。那么有没有什么方法可以避免这个问题呢？有，我受 jdk 中的流对象的 WriteInt32、WriteByte、WriteInt64、WriteString，这样的接口的启发，也发明了一套这样的协议，而且这套协议基本上是通用协议，可用于任何场景。我们的包还是分为包头和包体两部分，包头和上文所说的一样，包体是一个不固定大小的二进制流，其长度由包头中的指定包体长度的字段决定。

```
struct package_protocol
{
    int32_t bodysize;
    //注意：C/C++语法不能这么定义结构体，
    //这里只是为了说明含义的伪代码
    //bodycontent即为一个不固定大小的二进制流
    char    binarystream[bodysize];
};
```

接下来的核心部分就是如何操作这个二进制流，我们将流分为二进制读和二进制写两种流，下面给出接口定义：

```
//写
class BinaryWriteStream
{
public:
    BinaryWriteStream(string* data);
    const char* GetData() const;
    size_t GetSize() const;
    bool WriteCString(const char* str, size_t len);
    bool WriteString(const string& str);
    bool WriteDouble(double value, bool isNULL = false);
    bool WriteInt64(int64_t value, bool isNULL = false);
    bool WriteInt32(int32_t i, bool isNULL = false);
    bool WriteShort(short i, bool isNULL = false);
    bool WriteChar(char c, bool isNULL = false);
    size_t GetCurrentPos() const{ return m_data->length(); }
    void Flush();
    void Clear();
private:
    string* m_data;
};
//读
class BinaryReadStream : public IReadStream
{
private:
    const char* const ptr;
    const size_t      len;
    const char*       cur;
    BinaryReadStream(const BinaryReadStream&);
    BinaryReadStream& operator=(const BinaryReadStream&);
public:
    BinaryReadStream(const char* ptr, size_t len);
    const char* GetData() const;
    size_t GetSize() const;
    bool IsEmpty() const;
    bool ReadString(string* str, size_t maxlen, size_t& outlen);
    bool ReadCString(char* str, size_t strlen, size_t& len);
    bool ReadCCString(const char** str, size_t maxlen, size_t& outlen);
    bool ReadInt32(int32_t& i);
    bool ReadInt64(int64_t& i);
    bool ReadShort(short& i);
    bool ReadChar(char& c);
    size_t ReadAll(char* szBuffer, size_t iLen) const;
    bool IsEnd() const;
    const char* GetCurrent() const{ return cur; }
public:
    bool ReadLength(size_t & len);
    bool ReadLengthWithoutOffset(size_t &headlen, size_t & outlen);
};
```

这样如果是上文的一个登录数据包，我们只要写成如下形式就可以了：

```
std::string outbuf;
BinaryWriteStream stream(&outbuf);
stream.WriteInt32(cmd);
stream.WriteCString(username, 16);
stream.WriteCString(password, 16);
stream.WriteInt32(clienttype);
stream.WriteInt32(onlinetype);
//最终数据就存储到outbuf中去了
stream.Flush();
```

接着我们再对端，解得正确的包体后，我们只要按写入的顺序依次读出来即可：

```
BinaryWriteStream stream(outbuf.c_str(), outbuf.length());
int32_t cmd;
stream.WriteInt32(cmd);
char username[16];
stream.ReadCString(username, 16, NULL);
char password[16];
stream.WriteCString(password, 16, NULL);
int32_t clienttype;
stream.WriteInt32(clienttype);
int32_t onlinetype;
stream.WriteInt32(onlinetype);
```

这里给出 BinaryReadStream 和 BinaryWriteStream 的完整实现：

```
//计算校验和
unsigned short checksum(const unsigned short *buffer, int size)
{
    unsigned int cksum = 0;
    while (size > 1)
    {
        cksum += *buffer++;
        size -= sizeof(unsigned short);
    }
    if (size)
    {
        cksum += *(unsigned char*)buffer;
    }
    //将32位数转换成16
    while (cksum >> 16)
        cksum = (cksum >> 16) + (cksum & 0xffff);
    return (unsigned short)(~cksum);
}

bool compress_(unsigned int i, char *buf, size_t &len)
{
    len = 0;
    for (int a = 4; a >= 0; a--)
    {
        char c;
        c = i >> (a * 7) & 0x7f;
        if (c == 0x00 && len == 0)
            continue;
        if (a == 0)
            c &= 0x7f;
        else
            c |= 0x80;
        buf[len] = c;
        len++;
    }
    if (len == 0)
    {
        len++;
        buf[0] = 0;
    }
    //cout << "compress:" << i << endl;
    //cout << "compress len:" << len << endl;
    return true;
}

bool uncompress_(char *buf, size_t len, unsigned int &i)
{
    i = 0;
    for (int index = 0; index < (int)len; index++)
    {
        char c = *(buf + index);
        i = i << 7;
        c &= 0x7f;
        i |= c;
    }
    //cout << "uncompress:" << i << endl;
    return true;
}

BinaryReadStream::BinaryReadStream(const char* ptr_, size_t len_)
    : ptr(ptr_), len(len_), cur(ptr_)
{
    cur += BINARY_PACKLEN_LEN_2 + CHECKSUM_LEN;
}

bool BinaryReadStream::IsEmpty() const
{
    return len <= BINARY_PACKLEN_LEN_2;
}

size_t BinaryReadStream::GetSize() const
{
    return len;
}

bool BinaryReadStream::ReadCString(char* str, size_t strlen, /* out */ size_t& outlen)
{
    size_t fieldlen;
    size_t headlen;
    if (!ReadLengthWithoutOffset(headlen, fieldlen)) {
        return false;
    }
    // user buffer is not enough
    if (fieldlen > strlen) {
        return false;
    }
    // 偏移到数据的位置
    //cur += BINARY_PACKLEN_LEN_2;
    cur += headlen;
    if (cur + fieldlen > ptr + len)
    {
        outlen = 0;
        return false;
    }
    memcpy(str, cur, fieldlen);
    outlen = fieldlen;
    cur += outlen;
    return true;
}

bool BinaryReadStream::ReadString(string* str, size_t maxlen, size_t& outlen)
{
    size_t headlen;
    size_t fieldlen;
    if (!ReadLengthWithoutOffset(headlen, fieldlen)) {
        return false;
    }
    // user buffer is not enough
    if (maxlen != 0 && fieldlen > maxlen) {
        return false;
    }
    // 偏移到数据的位置
    //cur += BINARY_PACKLEN_LEN_2;
    cur += headlen;
    if (cur + fieldlen > ptr + len)
    {
        outlen = 0;
        return false;
    }
    str->assign(cur, fieldlen);
    outlen = fieldlen;
    cur += outlen;
    return true;
}

bool BinaryReadStream::ReadCCString(const char** str, size_t maxlen, size_t& outlen)
{
    size_t headlen;
    size_t fieldlen;
    if (!ReadLengthWithoutOffset(headlen, fieldlen)) {
        return false;
    }
    // user buffer is not enough
    if (maxlen != 0 && fieldlen > maxlen) {
        return false;
    }
    // 偏移到数据的位置
    //cur += BINARY_PACKLEN_LEN_2;
    cur += headlen;
    //memcpy(str, cur, fieldlen);
    if (cur + fieldlen > ptr + len)
    {
        outlen = 0;
        return false;
    }
    *str = cur;
    outlen = fieldlen;
    cur += outlen;
    return true;
}

bool BinaryReadStream::ReadInt32(int32_t& i)
{
    const int VALUE_SIZE = sizeof(int32_t);
    if (cur + VALUE_SIZE > ptr + len)
        return false;
    memcpy(&i, cur, VALUE_SIZE);
    i = ntohl(i);
    cur += VALUE_SIZE;
    return true;
}

bool BinaryReadStream::ReadInt64(int64_t& i)
{
    char int64str[128];
    size_t length;
    if (!ReadCString(int64str, 128, length))
        return false;
    i = atoll(int64str);
    return true;
}

bool BinaryReadStream::ReadShort(short& i)
{
    const int VALUE_SIZE = sizeof(short);
    if (cur + VALUE_SIZE > ptr + len) {
        return false;
    }
    memcpy(&i, cur, VALUE_SIZE);
    i = ntohs(i);
    cur += VALUE_SIZE;
    return true;
}

bool BinaryReadStream::ReadChar(char& c)
{
    const int VALUE_SIZE = sizeof(char);
    if (cur + VALUE_SIZE > ptr + len) {
        return false;
    }
    memcpy(&c, cur, VALUE_SIZE);
    cur += VALUE_SIZE;
    return true;
}

bool BinaryReadStream::ReadLength(size_t & outlen)
{
    size_t headlen;
    if (!ReadLengthWithoutOffset(headlen, outlen)) {
        return false;
    }
    //cur += BINARY_PACKLEN_LEN_2;
    cur += headlen;
    return true;
}

bool BinaryReadStream::ReadLengthWithoutOffset(size_t& headlen, size_t & outlen)
{
    headlen = 0;
    const char *temp = cur;
    char buf[5];
    for (size_t i = 0; i<sizeof(buf); i++)
    {
        memcpy(buf + i, temp, sizeof(char));
        temp++;
        headlen++;
        //if ((buf[i] >> 7 | 0x0) == 0x0)
        if ((buf[i] & 0x80) == 0x00)
            break;
    }
    if (cur + headlen > ptr + len)
        return false;
    unsigned int value;
    uncompress_(buf, headlen, value);
    outlen = value;
    /*if ( cur + BINARY_PACKLEN_LEN_2 > ptr + len ) {
    return false;
    }
    unsigned int tmp;
    memcpy(&tmp, cur, sizeof(tmp));
    outlen = ntohl(tmp);*/
    return true;
}

bool BinaryReadStream::IsEnd() const
{
    assert(cur <= ptr + len);
    return cur == ptr + len;
}

const char* BinaryReadStream::GetData() const
{
    return ptr;
}

size_t BinaryReadStream::ReadAll(char * szBuffer, size_t iLen) const
{
    size_t iRealLen = min(iLen, len);
    memcpy(szBuffer, ptr, iRealLen);
    return iRealLen;
}

//=================class BinaryWriteStream implementation============//
BinaryWriteStream::BinaryWriteStream(string *data) :
    m_data(data)
{
    m_data->clear();
    char str[BINARY_PACKLEN_LEN_2 + CHECKSUM_LEN];
    m_data->append(str, sizeof(str));
}

bool BinaryWriteStream::WriteCString(const char* str, size_t len)
{
    char buf[5];
    size_t buflen;
    compress_(len, buf, buflen);
    m_data->append(buf, sizeof(char)*buflen);
    m_data->append(str, len);
    //unsigned int ulen = htonl(len);
    //m_data->append((char*)&ulen,sizeof(ulen));
    //m_data->append(str,len);
    return true;
}

bool BinaryWriteStream::WriteString(const string& str)
{
    return WriteCString(str.c_str(), str.length());
}

const char* BinaryWriteStream::GetData() const
{
    return m_data->data();
}

size_t BinaryWriteStream::GetSize() const
{
    return m_data->length();
}

bool BinaryWriteStream::WriteInt32(int32_t i, bool isNULL)
{
    int32_t i2 = 999999999;
    if (isNULL == false)
        i2 = htonl(i);
    m_data->append((char*)&i2, sizeof(i2));
    return true;
}

bool BinaryWriteStream::WriteInt64(int64_t value, bool isNULL)
{
    char int64str[128];
    if (isNULL == false)
    {
    #ifndef _WIN32
        sprintf(int64str, "%ld", value);
    #else
        sprintf(int64str, "%lld", value);
    #endif
        WriteCString(int64str, strlen(int64str));
    }
    else
        WriteCString(int64str, 0);
    return true;
}

bool BinaryWriteStream::WriteShort(short i, bool isNULL)
{
    short i2 = 0;
    if (isNULL == false)
        i2 = htons(i);
    m_data->append((char*)&i2, sizeof(i2));
    return true;
}

bool BinaryWriteStream::WriteChar(char c, bool isNULL)
{
    char c2 = 0;
    if (isNULL == false)
        c2 = c;
    (*m_data) += c2;
    return true;
}

bool BinaryWriteStream::WriteDouble(double value, bool isNULL)
{
    char   doublestr[128];
    if (isNULL == false)
    {
        sprintf(doublestr, "%f", value);
        WriteCString(doublestr, strlen(doublestr));
    }
    else
        WriteCString(doublestr, 0);
    return true;
}

void BinaryWriteStream::Flush()
{
    char *ptr = &(*m_data)[0];
    unsigned int ulen = htonl(m_data->length());
    memcpy(ptr, &ulen, sizeof(ulen));
}

void BinaryWriteStream::Clear()
{
    m_data->clear();
    char str[BINARY_PACKLEN_LEN_2 + CHECKSUM_LEN];
    m_data->append(str, sizeof(str));
}
```

这里详细解释一下上面的实现原理，即如何把各种类型的字段写入这种所谓的流中，或者怎么从这种流中读出各种类型的数据。上文的字段在流中的格式如下图：

![](../imgs/protocol2.webp)

这里最简便的方式就是每个字段的长度域都是固定字节数目，如 4 个字节。但是这里我们并没有这么做，而是使用了一个小小技巧去对字段长度进行了一点压缩。对于字符串类型的字段，我们将表示其字段长度域的整型值（int32 类型，4 字节）按照其数值的大小压缩成 1 ～ 5 个字节，对于每一个字节，如果我们只用其低 7 位。最高位为标志位，为 1 时，表示其左边的还有下一个字节，反之到此结束。例如，对于数字 127，我们二进制表示成 01111111，由于最高位是 0，那么如果字段长度是 127 及以下，一个字节就可以存储下了。如果一个字段长度大于 127，如等于 256，对应二进制 100000000，那么我们按照刚才的规则，先填充最低字节（从左往右依次是从低到高），由于最低的 7 位放不下，还有后续高位字节，所以我们在最低字节的最高位上填 1，即 10000000，接着次高位为 00000100，由于次高位后面没有更高位的字节了，所以其最高位为 0，组合起来两个字节就是 10000000 0000100。对于数字 50000，其二进制是 1100001101010000，根据每 7 个一拆的原则是：11 0000110 1010000 再加上标志位就是：10000011 10000110 01010000。采用这样一种策略将原来占 4 个字节的整型值根据数值大小压缩成了 1 ～ 5 个字节（由于我们对数据包最大长度有限制，所以不会出现长度需要占 5 个字节的情形）。反过来，解析每个字段的长度，就是先取出一个字节，看其最高位是否有标志位，如果有继续取下一个字节当字段长度的一部分继续解析，直到遇到某个字节最高位不为 1 为止。

对一个整形压缩和解压缩的部分从上面的代码中摘录如下：

压缩：

```
 1    //将一个四字节的整形数值压缩成1~5个字节
 2    bool compress_(unsigned int i, char *buf, size_t &len)
 3    {
 4        len = 0;
 5        for (int a = 4; a >= 0; a--)
 6        {
 7            char c;
 8            c = i >> (a * 7) & 0x7f;
 9            if (c == 0x00 && len == 0)
10                continue;
11            if (a == 0)
12                c &= 0x7f;
13            else
14                c |= 0x80;
15            buf[len] = c;
16            len++;
17        }
18        if (len == 0)
19        {
20            len++;
21            buf[0] = 0;
22        }
23        //cout << "compress:" << i << endl;
24        //cout << "compress len:" << len << endl;
25        return true;
26    }
```

解压

```
 1    //将一个1~5个字节的值还原成四字节的整形值
 2    bool uncompress_(char *buf, size_t len, unsigned int &i)
 3    {
 4        i = 0;
 5        for (int index = 0; index < (int)len; index++)
 6        {
 7            char c = *(buf + index);
 8            i = i << 7;
 9            c &= 0x7f;
10            i |= c;
11        }
12        //cout << "uncompress:" << i << endl;
13        return true;
14    }
```

## 三、关于跨系统与跨语言之间的网络通信协议解析与识别问题

由于我们的即时通讯同时涉及到 Java 和 C++两种编程语言，且有 windows、linux、安卓三个平台，而我们为了保障学习的质量和效果，所以我们不用第三跨平台库（其实我们也是在学习如何编写这些跨平台库的原理），所以我们需要学习以下如何在 Java 语言中去解析 C++的网络数据包或者反过来。安卓端发送的数据使用 Java 语言编写，pc 与服务器发送的数据使用 C++编写，这里以在 Java 中解析 C++网络数据包为例。 这对于很多人来说是一件很困难的事情，所以只能变着法子使用第三方的库。其实只要你掌握了一定的基础知识，利用一些现成的字节流抓包工具（如 tcpdump、wireshark）很容易解决这个问题。我们这里使用 tcpdump 工具来尝试分析和解决这个问题。
首先，我们需要明确字节序列这样一个概念，即我们说的大端编码(big endian)和小端编码(little endian)，x86 和 x64 系列的 cpu 使用小端编码，而数据在网络上传输，以及 Java 语言中，使用的是大端编码。那么这是什么意思呢？
我们举个例子，看一个 x64 机器上的 32 位数值在内存中的存储方式：

![](../imgs/protocol3.webp)

i 在内存中的地址序列是 0x003CF7C4~0x003CF7C8，值为 40 e2 01 00。

![](../imgs/protocol4.webp)

十六进制 0001e240 正好等于 10 进制 123456，也就是说小端编码中权重高的的字节值存储在内存地址高（地址值较大）的位置，权重值低的字节值存储在内存地址低（地址值较小）的位置，也就是所谓的高高低低。
相反，大端编码的规则应该是高低低高，也就是说权值高字节存储在内存地址低的位置，权值低的字节存储在内存地址高的位置。
所以，如果我们一个 C++程序的 int32 值 123456 不作转换地传给 Java 程序，那么 Java 按照大端编码的形式读出来的值是：十六进制 40E20100 = 十进制 1088553216。
所以，我们要么在发送方将数据转换成网络字节序（大端编码），要么在接收端再进行转换。

下面看一下如果 C++端传送一个如下数据结构，Java 端该如何解析（由于 Java 中是没有指针的，也无法操作内存地址，导致很多人无从下手），下面利用 tcpdump 来解决这个问题的思路。
我们客户端发送的数据包：

![](../imgs/protocol5.webp)

其结构体定义如下：

![](../imgs/protocol6.jpeg)

利用 tcpdump 抓到的包如下：

![](../imgs/protocol7.webp)

放大一点：

![](../imgs/protocol8.webp)

我们白色标识出来就是我们收到的数据包。这里我想说明两点：

- 如果我们知道发送端发送的字节流，再比照接收端收到的字节流，我们就能检测数据包的完整性，或者利用这个来排查一些问题；

- 对于 Java 程序只要按照这个顺序，先利用 java.net.Socket 的输出流 java.io.DataOutputStream 对象 readByte、readInt32、readInt32、readBytes、readBytes 方法依次读出一个 char、int32、int32、16 个字节的字节数组、63 个字节数组即可，为了还原像 int32 这样的整形值，我们需要做一些小端编码向大端编码的转换。

# TCP 协议如何解决粘包、半包问题

## 一 TCP 协议是流式协议

很多读者从接触网络知识以来，应该听说过这句话：TCP 协议是流式协议。那么这句话到底是什么意思呢？所谓流式协议，即协议的内容是像流水一样的字节流，内容与内容之间没有明确的分界标志，需要我们人为地去给这些协议划分边界。

举个例子，A 与 B 进行 TCP 通信，A 先后给 B 发送了一个 100 字节和 200 字节的数据包，那么 B 是如何收到呢？B 可能先收到 100 字节，再收到 200 字节；也可能先收到 50 字节，再收到 250 字节；或者先收到 100 字节，再收到 100 字节，再收到 200 字节；或者先收到 20 字节，再收到 20 字节，再收到 60 字节，再收到 100 字节，再收到 50 字节，再收到 50 字节……

不知道读者看出规律没有？规律就是 A 一共给 B 发送了 300 字节，B 可能以一次或者多次任意形式的总数为 300 字节收到。假设 A 给 B 发送的 100 字节和 200 字节分别都是一个数据包，对于发送端 A 来说，这个是可以区分的，但是对于 B 来说，如果不人为规定多长为一个数据包，B 每次是不知道应该把收到的数据中多少字节作为一个有效的数据包的。而规定每次把多少数据当成一个包就是协议格式规范的内容之一。

经常会有新手写出类似下面这样的代码：

**发送端：**

```
//...省略创建socket，建立连接等部分不相关的逻辑...
char buf[] = "the quick brown fox jumps over a lazy dog.";
int n = send(socket, buf, strlen(buf), 0);
//...省略出错处理逻辑...
```

**接收端：**

```
//省略创建socket，建立连接等部分不相关的逻辑...
char recvBuf[50] = { 0 };
int n = recv(socket, recvBuf, 50, 0);
//省略出错处理逻辑...
printf("recvBuf: %s", recvBuf);
```

为了专注问题本身的讨论，我这里省略掉了建立连接和部分错误处理的逻辑。上述代码中发送端给接收端发送了一串字符”the quick brown fox jumps over a lazy dog.“，接收端收到后将其打印出来。

类似这样的代码在本机一般会工作的很好，接收端也如期打印出来预料的字符串，但是一放到局域网或者公网环境就出问题了，即接收端可能打印出来字符串并不完整；如果发送端连续多次发送字符串，接收端会打印出来的字符串不完整或出现乱码。不完整的原因很好理解，即对端某次收到的数据小于完整字符串的长度，recvBuf 数组开始被清空成 0，收到部分字符串后，该字符串的末尾仍然是 0，printf 函数寻找以 0 为结束标志的字符结束输出；乱码的原因是如果某次收入的数据不仅包含一个完整的字符串，还包含下一个字符串部分内容，那么 recvBuf 数组将会被填满，printf 函数输出时仍然会寻找以 0 为结束标志的字符结束输出，这样读取的内存就越界了，一直找到为止，而越界后的内存可能是一些不可读字符，显示出来后就乱码了。

我举这个例子希望你明白 能对 TCP 协议是流式协议有一个直观的认识。正因为如此，所以我们需要人为地在发送端和接收端规定每一次的字节流边界，以便接收端知道从什么位置取出多少字节来当成一个数据包去解析，这就是我们设计网络通信协议格式的要做的工作之一。

## 二 如何解决粘包问题

网络通信程序实际开发中，或者技术面试时，面试官通常会问的比较多的一个问题是：网络通信时，如何解决粘包？

> 有的面试官可能会这么问：网络通信时，如何解决粘包、丢包或者包乱序问题？这个问题其实是面试官在考察面试者的网络基础知识，如果是 TCP 协议，在大多数场景下，是不存在丢包和包乱序问题的，TCP 通信是可靠通信方式，TCP 协议栈通过序列号和包重传确认机制保证数据包的有序和一定被正确发到目的地；如果是 UDP 协议，如果不能接受少量丢包，那就要自己在 UDP 的基础上实现类似 TCP 这种有序和可靠传输机制了（例如 RTP 协议、RUDP 协议）。所以，问题拆解后，只剩下如何解决粘包的问题。

先来解释一下什么是**粘包**，所谓粘包就是连续给对端发送两个或者两个以上的数据包，对端在一次收取中可能收到的数据包大于 1 个，大于 1 个，可能是几个（包括一个）包加上某个包的部分，或者干脆就是几个完整的包在一起。当然，也可能收到的数据只是一个包的部分，这种情况一般也叫**半包**。

无论是半包还是粘包问题，其根源是上文介绍中 TCP 协议是流式数据格式。解决问题的思路还是想办法从收到的数据中把包与包的边界给区分出来。那么如何区分呢？目前主要有三种方法：

**固定包长的数据包**

顾名思义，即每个协议包的长度都是固定的。举个例子，例如我们可以规定每个协议包的大小是 64 个字节，每次收满 64 个字节，就取出来解析（如果不够，就先存起来）。

这种通信协议的格式简单但灵活性差。如果包内容不足指定的字节数，剩余的空间需要填充特殊的信息，如 \0（如果不填充特殊内容，如何区分包里面的正常内容与填充信息呢？）；如果包内容超过指定字节数，又得分包分片，需要增加额外处理逻辑——在发送端进行分包分片，在接收端重新组装包片（分包和分片内容在接下来会详细介绍）。

**以指定字符（串）为包的结束标志**

这种协议包比较常见，即字节流中遇到特殊的符号值时就认为到一个包的末尾了。例如，我们熟悉的 FTP 协议，发邮件的 SMTP 协议，一个命令或者一段数据后面加上"\r\n"（即所谓的 **CRLF**）表示一个包的结束。对端收到后，每遇到一个”\r\n“就把之前的数据当做一个数据包。

这种协议一般用于一些包含各种命令控制的应用中，其不足之处就是如果协议数据包内容部分需要使用包结束标志字符，就需要对这些字符做转码或者转义操作，以免被接收方错误地当成包结束标志而误解析。

**包头 + 包体格式**

这种格式的包一般分为两部分，即包头和包体，包头是固定大小的，且包头中必须含有一个字段来说明接下来的包体有多大。

例如：

```
struct msg_header
{
  int32_t bodySize;
  int32_t cmd;
};
```

这就是一个典型的包头格式，bodySize 指定了这个包的包体是多大。由于包头大小是固定的（这里是 size(int32_t) + sizeof(int32_t) = 8 字节），对端先收取包头大小字节数目（当然，如果不够还是先缓存起来，直到收够为止），然后解析包头，根据包头中指定的包体大小来收取包体，等包体收够了，就组装成一个完整的包来处理。在有些实现中，包头中的 bodySize 可能被另外一个叫 packageSize 的字段代替，这个字段的含义是整个包的大小，这个时候，我们只要用 packageSize 减去包头大小（这里是 sizeof(msg_header)）就能算出包体的大小，原理同上。

> 在使用大多数网络库时，通常你需要根据协议格式自己给数据包分界和解析，一般的网络库不提供这种功能是出于需要支持不同的协议，由于协议的不确定性，因此没法预先提供具体解包代码。当然，这不是绝对的，也有一些网络库提供了这种功能。在 Java Netty 网络框架中，提供了 FixedLengthFrameDecoder 类去处理长度是定长的协议包，提供了 DelimiterBasedFrameDecoder 类去处理按特殊字符作为结束符的协议包，提供 ByteToMessageDecoder 去处理自定义格式的协议包（可用来处理包头 + 包体 这种格式的数据包），然而在继承 ByteToMessageDecoder 子类中你需要根据你的协议具体格式重写 decode() 方法来对数据包解包。

这三种包格式，希望读者能在理解其原理和优缺点的基础上深入掌握。

## 三 解包与处理

在理解了前面介绍的数据包的三种格式后，我们来介绍一下针对上述三种格式的数据包技术上应该如何处理。其处理流程都是一样的，这里我们以**包头 + 包体** 这种格式的数据包来说明。处理流程如下：

![](../imgs/tcp1.webp)

假设我们的包头格式如下：

```
//强制一字节对齐
#pragma pack(push, 1)
//协议头
struct msg
{
    int32_t  bodysize;         //包体大小
};
#pragma pack(pop)
```

那么上面的流程实现代码如下：

```
//包最大字节数限制为10M
#define MAX_PACKAGE_SIZE    10 * 1024 * 1024

void ChatSession::OnRead(const std::shared_ptr<TcpConnection>& conn, Buffer* pBuffer, Timestamp receivTime)
{
    while (true)
    {
        //不够一个包头大小
        if (pBuffer->readableBytes() < (size_t)sizeof(msg))
        {
            //LOGI << "buffer is not enough for a package header, pBuffer->readableBytes()=" << pBuffer->readableBytes() << ", sizeof(msg)=" << sizeof(msg);
            return;
        }

        //取包头信息
        msg header;
        memcpy(&header, pBuffer->peek(), sizeof(msg));

        //包头有错误，立即关闭连接
        if (header.bodysize <= 0 || header.bodysize > MAX_PACKAGE_SIZE)
        {
            //客户端发非法数据包，服务器主动关闭之
            LOGE("Illegal package, bodysize: %lld, close TcpConnection, client: %s", header.bodysize, conn->peerAddress().toIpPort().c_str());
            conn->forceClose();
            return;
        }

        //收到的数据不够一个完整的包
        if (pBuffer->readableBytes() < (size_t)header.bodysize + sizeof(msg))
            return;

        pBuffer->retrieve(sizeof(msg));
        //inbuf用来存放当前要处理的包
        std::string inbuf;
        inbuf.append(pBuffer->peek(), header.bodysize);
        pBuffer->retrieve(header.bodysize);
        //解包和业务处理
        if (!Process(conn, inbuf.c_str(), inbuf.length()))
        {
            //客户端发非法数据包，服务器主动关闭之
            LOGE("Process package error, close TcpConnection, client: %s", conn->peerAddress().toIpPort().c_str());
            conn->forceClose();
            return;
        }
    }// end while-loop
}
```

上述流程代码的处理过程和流程图中是一致的，pBuffer 这里是一个自定义的接收缓冲区，这里的代码，已经将收到的数据放入了这个缓冲区，所以判断当前已收取的字节数目只需要使用这个对象的相应方法即可。上述代码有些细节我需要强调一下：

- 取包头时，你应该拷贝一份数据包头大小的数据出来，而不是从缓冲区 pBuffer 中直接将数据取出来（即取出来的数据从 pBuffer 中移除），这是因为倘若接下来根据包头中的字段得到包体大小时，如果剩余数据不够一个包体大小，你又得把这个包头数据放回缓冲区。为了避免这种不必要的操作，只有缓冲区数据大小够整个包的大小（代码中：header.bodysize + sizeof(msg)）你才需要把整个包大小的数据从缓冲区移除，这也是这里的 pBuffer->peek() 方法 peek 单词的含义（中文可以翻译成“瞟一眼”或者“偷窥”）。
- 通过包头得到包体大小时，你一定要对 bodysize 的数值进行校验，我这里要求 bodysize 必须大于 0 且不大于 10 _ 1024 _ 1024（即 10 M）。当然，实际开发中，你可以根据你自己的需求要决定 bodysize 的上下限（包体大小是 0 字节的包在某些业务场景下是允许的）。记住，一定要判断这个上下限，因为假设这是一个非法的客户端发来的数据，其 bodysize 设置了一个比较大的数值，例如 1 _ 1024 _ 1024 \* 1024（即 1 G），你的逻辑会让你一直缓存该客户端发来的数据，那么很快你的服务器内存将会被耗尽，操作系统在检测到你的进程占用内存达到一定阈值时会杀死你的进程，导致服务不能再正常对外服务。如果你检测了 bodysize 字段的是否满足你设置的上下限，对于非法的 bodysize，直接关闭这路连接即可。这也是服务的一种自我保护措施，避免因为非法数据包带来的损失。
- 不知道你有没有注意到整个判断包头、包体以及处理包的逻辑放在一个 while 循环里面，这是必要的。如果没有这个 while 循环，当你一次性收到多个包时，你只会处理一个，下次接着处理就需要等到新一批数据来临时再次触发这个逻辑。这样造成的结果就是，对端给你发送了多个请求，你最多只能应答一个，后面的应答得等到对端再次给你发送数据时。这就是对**粘包**逻辑的正确处理。

以上逻辑和代码是最基本的**粘包和半包处理机制**，也就是所谓的**技术上的解包处理逻辑**（业务上的解包处理逻辑后面章节再介绍）。希望读者能理解他们，在理解了他们的基础之上，我们可以给解包拓展很多功能，例如，我们再给我们的协议包增加一个支持压缩的功能，我们的包头变成下面这个样子：

```
#pragma pack(push, 1)
//协议头
struct msg
{
    char     compressflag;     //压缩标志，如果为1，则启用压缩，反之不启用压缩
    int32_t  originsize;       //包体压缩前大小
    int32_t  compresssize;     //包体压缩后大小
    char     reserved[16];       //保留字段，用于将来拓展
};
#pragma pack(pop)
```

修改后的代码如下：

```
void ChatSession::OnRead(const std::shared_ptr<TcpConnection>& conn, Buffer* pBuffer, Timestamp receivTime)
{
    while (true)
    {
        //不够一个包头大小
        if (pBuffer->readableBytes() < (size_t)sizeof(msg))
        {
            //LOGI << "buffer is not enough for a package header, pBuffer->readableBytes()=" << pBuffer->readableBytes() << ", sizeof(msg)=" << sizeof(msg);
            return;
        }

        //取包头信息
        msg header;
        memcpy(&header, pBuffer->peek(), sizeof(msg));

        //数据包压缩过
        if (header.compressflag == PACKAGE_COMPRESSED)
        {
            //包头有错误，立即关闭连接
            if (header.compresssize <= 0 || header.compresssize > MAX_PACKAGE_SIZE ||
                header.originsize <= 0 || header.originsize > MAX_PACKAGE_SIZE)
            {
                //客户端发非法数据包，服务器主动关闭之
                LOGE("Illegal package, compresssize: %lld, originsize: %lld, close TcpConnection, client: %s",  header.compresssize, header.originsize, conn->peerAddress().toIpPort().c_str());
                conn->forceClose();
                return;
            }

            //收到的数据不够一个完整的包
            if (pBuffer->readableBytes() < (size_t)header.compresssize + sizeof(msg))
                return;

            pBuffer->retrieve(sizeof(msg));
            std::string inbuf;
            inbuf.append(pBuffer->peek(), header.compresssize);
            pBuffer->retrieve(header.compresssize);
            std::string destbuf;
            if (!ZlibUtil::UncompressBuf(inbuf, destbuf, header.originsize))
            {
                LOGE("uncompress error, client: %s", conn->peerAddress().toIpPort().c_str());
                conn->forceClose();
                return;
            }

            //业务逻辑处理
            if (!Process(conn, destbuf.c_str(), destbuf.length()))
            {
                //客户端发非法数据包，服务器主动关闭之
                LOGE("Process error, close TcpConnection, client: %s", conn->peerAddress().toIpPort().c_str());
                conn->forceClose();
                return;
            }
        }
        //数据包未压缩
        else
        {
            //包头有错误，立即关闭连接
            if (header.originsize <= 0 || header.originsize > MAX_PACKAGE_SIZE)
            {
                //客户端发非法数据包，服务器主动关闭之
                LOGE("Illegal package, compresssize: %lld, originsize: %lld, close TcpConnection, client: %s", header.compresssize, header.originsize, conn->peerAddress().toIpPort().c_str());
                conn->forceClose();
                return;
            }

            //收到的数据不够一个完整的包
            if (pBuffer->readableBytes() < (size_t)header.originsize + sizeof(msg))
                return;

            pBuffer->retrieve(sizeof(msg));
            std::string inbuf;
            inbuf.append(pBuffer->peek(), header.originsize);
            pBuffer->retrieve(header.originsize);
            //业务逻辑处理
            if (!Process(conn, inbuf.c_str(), inbuf.length()))
            {
                //客户端发非法数据包，服务器主动关闭之
                LOGE("Process error, close TcpConnection, client: %s", conn->peerAddress().toIpPort().c_str());
                conn->forceClose();
                return;
            }
        }// end else

    }// end while-loop
}
```

这段代码先根据包头的压缩标志字段判断包体是否有压缩，如果有压缩，则取出包体大小去解压，解压后的数据才是真正的业务数据。整个程序执行流程图如下：

![](../imgs/tcp2.webp)

###
