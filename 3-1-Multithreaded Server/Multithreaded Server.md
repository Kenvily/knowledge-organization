<font size = 6>Multithreaded Server</font>

[toc]

# 概念

## 局域网和广域网

- 局域网：局域网将一定区域内的各种计算机、外部设备和数据库连接起来形成计算机通信的私有网络
- 广域网：又称广域网、外网、公网。是连接不同地区局域网或城域网计算机通信的远程公共网络

## IP

IP（Internet Protocol）：本质是一个整形数，用于表示计算机在网络中的地址。IP协议版本有两个

**IPv4**

- 使用一个32位的整形数描述一个IP地址，4个字节，int型，也可以使用一个点分十进制字符串描述这个IP地址，如192.168.247.135
- 分成了4份，每份1字节，8bit（char），最大值为 255
  - 0.0.0.0 是最小的IP地址
  - 255.255.255.255是最大的IP地址
- 按照IPv4协议计算，可以使用的IP地址共有 2^32^ 个

**IPv6**

- 使用一个128位的整形数描述一个IP地址，16个字节，也可以使用一个字符串描述这个IP地址：2001:0db8:3c4d:0015:0000:0000:1a2f:1a2b
- 分成了8份，每份2字节，每一部分以16进制的方式表示
- 按照IPv6协议计算，可以使用的IP地址共有 2^128^ 个

## 端口

端口的作用是定位到主机上的某一个进程，通过这个端口进程就可以接受到对应的网络数据了。

端口也是一个整形数 unsigned short ，一个16位整形数，有效端口的取值范围是：0 ~ 65535(0 ~ 216-1)。

- 通过IP地址可以定位到某一台主机，通过端口就可以定位到主机上的某一个进程
- 通过指定的IP和端口，发送数据的时候对端就能接受到数据了
- 如果这个进程不需要网络通信，那么这个进程就不需要绑定端口的
- 一个端口只能给某一个进程使用，多个进程不能同时使用同一个端口

## OSI/ISO 网络分层模型

OSI（Open System Interconnect），即开放式系统互联。 一般都叫OSI参考模型，是ISO（国际标准化组织组织）在1985年研究的网络互联模型。

<img src=".\img\image-20250222132947921.png" alt="image-20250222132947921" style="zoom:70%;" />

- 物理层：负责最后将信息编码成电流脉冲或其它信号用于网上传输
- 数据链路层
  - 数据链路层通过物理网络链路供数据传输
  - 规定了0和1的分包形式，确定了网络数据包的形式
- 网络层
  - 网络层负责在源和终点之间建立连接
  - 此处需要确定计算机的位置，通过IPv4，IPv6格式的IP地址来找到对应的主机
- 传输层
  - 传输层向高层提供可靠的端到端的网络数据流服务
  - 每一个应用程序都会在网卡注册一个端口号，该层就是端口与端口的通信
- 会话层
  - 会话层建立、管理和终止表示层与实体之间的通信会话
  - 建立一个连接（自动的手机信息、自动的网络寻址）
- 表示层
  - 对应用层数据编码和转化, 确保以一个系统应用层发送的信息 可以被另一个系统应用层识别

## 网络协议

网络协议指的是计算机网络中互相通信的对等实体之间交换信息时所必须遵守的规则的集合。一般系统网络协议包括五个部分：通信环境，传输服务，词汇表，信息的编码格式，时序、规则和过程。

## 数据的封装

在网络通信的时候, 程序猿需要负责的应用层数据的处理(最上层)。

<img src=".\img\image-20250222123757248.png" alt="image-20250222123757248" style="zoom: 70%;" />

- 应用层的数据可以使用某些协议进行封装, 也可以不封装
- 程序猿需要调用发送数据的接口函数，将数据发送出去
- 程序猿调用的API做底层数据处理
  - 传输层使用传输层协议打包数据
  - 网络层使用网络层协议打包数据
  - 网络接口层使用网络接口层协议打包数据
  - 数据被发送到internet
- 接收端接收到发送端的数据
- 程序猿调用接收数据的函数接收数据
- 调用的API做相关的底层处理
  - 网络接口层拆包 ==> 网络层的包
  - 网络层拆包 ==> 网络层的包
  - 传输层拆包 ==> 传输层数据
- 如果应用层也使用了协议对数据进行了封装，数据的包的解析需要程序猿做

# socket编程

Socket套接字其目的是将TCP/IP协议相关软件移植到UNIX类系统中。设计者开发了一个接口，以便应用程序能简单地调用该接口通信。这个接口不断完善，最终形成了Socket套接字。Linux系统采用了Socket套接字，因此，Socket接口就被广泛使用，到现在已经成为事实上的标准。与套接字相关的函数被包含在头文件sys/socket.h中。

通过上面的描述可以得知，套接字对应程序猿来说就是一套网络通信的接口，使用这套接口就可以完成网络通信。

与实现语言无关，可跨平台通信。

## 字节序

在各种计算机体系结构中，对于字节、字等的存储机制有所不同，因而引发了计算机通信领域中一个很重要的问题，即通信双方交流的信息单元（比特、字节、字、双字等等）应该以什么样的顺序进行传送。如果不达成一致的规则，通信双方将无法进行正确的编/译码从而导致通信失败。

字节序，顾名思义字节的顺序，就是大于一个字节类型的数据在内存中的存放顺序，也就是说对于单字符来说是没有字节序问题的，字符串是单字符的集合，因此字符串也没有字节序问题。

目前在各种体系的计算机中通常采用的字节存储机制主要有两种：Big-Endian 和 Little-Endian。

**Little-Endian** 

主机字节序 (小端)，数据的低位字节存储到内存的低地址位， 数据的高位字节存储到内存的高地址位。我们使用的PC机，数据的存储默认使用的是小端。

**Big-Endian**

网络字节序 (大端)，数据的低位字节存储到内存的高地址位，数据的高位字节存储到内存的低地址位。套接字通信过程中操作的数据都是大端存储的，包括接收/发送的数据、IP地址、端口。

字节序举例

```c
// 有一个16进制的数, 有32位 (int): 0xab5c01ff
// 字节序, 最小的单位: 字节, int 有4个字节, 需要将其拆分为4份
                 内存低地址位                内存的高地址位
------------------------------------------------------------------------>
小端:         0xff        0x01        0x5c        0xab
大端:         0xab        0x5c        0x01        0xff
```

<img src=".\img\image-20250222134622661.png" alt="image-20250222134622661" style="zoom:50%;" />

<img src=".\img\image-20250222134951257.png" alt="image-20250222134951257" style="zoom:50%;" />

BSD Socket提供了封装好的转换接口，方便程序员使用。包括从主机字节序到网络字节序的转换函数：htons、htonl；从网络字节序到主机字节序的转换函数：ntohs、ntohl。

```c
#include <arpa/inet.h>
// u:unsigned
// 16: 16位, 32:32位
// h: host, 主机字节序
// n: net, 网络字节序
// s: short
// l: int

// 这套api主要用于 网络通信过程中 IP 和 端口 的 转换
// 将一个短整形从主机字节序 -> 网络字节序
uint16_t htons(uint16_t hostshort);	
// 将一个整形从主机字节序 -> 网络字节序
uint32_t htonl(uint32_t hostlong);	

// 将一个短整形从网络字节序 -> 主机字节序
uint16_t ntohs(uint16_t netshort)
// 将一个整形从网络字节序 -> 主机字节序
uint32_t ntohl(uint32_t netlong);
```

## IP地址转换

虽然IP地址本质是一个整形数，但是在使用的过程中都是通过一个字符串来描述。

```c
/*
* 作用：
- 主机字节序的IP地址转换为网络字节序
- 主机字节序的IP地址是字符串, 网络字节序IP地址是整形
* 参数:
- af: 地址族(IP地址的家族包括ipv4和ipv6)协议
	- AF_INET: ipv4格式的ip地址
	- AF_INET6: ipv6格式的ip地址
- src: 传入参数, 对应要转换的点分十进制的ip地址: 192.168.1.100
- dst: 传出参数, 函数调用完成, 转换得到的大端整形IP被写入到这块内存中
* 返回值
- 成功返回1
- 失败返回0或者-1
*/

int inet_pton(int af, const char *src, void *dst); 
```

```c
/*
* 作用：
- 将大端的整形数, 转换为小端的点分十进制的IP地址   
* 参数:
- af: 地址族协议
	- AF_INET: ipv4格式的ip地址
	- AF_INET6: ipv6格式的ip地址
- src: 传入参数, 这个指针指向的内存中存储了大端的整形IP地址
- dst: 传出参数, 存储转换得到的小端的点分十进制的IP地址
- size: 修饰dst参数的, 标记dst指向的内存中最多可以存储多少个字节
* 返回值:
- 成功: 指针指向第三个参数对应的内存地址, 通过返回值也可以直接取出转换得到的IP字符串
- 失败: NULL
*/

#include <arpa/inet.h>

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```

还有一组函数也能处理进程IP地址大小端的转换，但是只能处理ipv4的ip地址

```c
// 点分十进制IP -> 大端整形
in_addr_t inet_addr (const char *cp);

// 大端整形 -> 点分十进制IP
char* inet_ntoa(struct in_addr in);
```

## Sockaddr 数据结构

<img src=".\img\image-20250222140028696.png" alt="image-20250222140028696" style="zoom:90%;" />

```c
struct sockaddr {
	sa_family_t sa_family;       // 地址族协议, ipv4
	char        sa_data[14];     // 端口(2字节) + IP地址(4字节) + 填充(8字节)
}

typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;
typedef unsigned short int sa_family_t;
#define __SOCKADDR_COMMON_SIZE (sizeof (unsigned short int))

struct in_addr
{
    in_addr_t s_addr;
};  

// sizeof(struct sockaddr) == sizeof(struct sockaddr_in)
struct sockaddr_in
{
    sa_family_t sin_family;		/* 地址族协议: AF_INET */
    in_port_t sin_port;         /* 端口, 2字节-> 大端  */
    struct in_addr sin_addr;    /* IP地址, 4字节 -> 大端  */
    /* 填充 8字节 */
    unsigned char sin_zero[sizeof (struct sockaddr) - sizeof(sin_family) - sizeof (in_port_t) - sizeof (struct in_addr)];
};  
```

## 套接字函数

使用套接字通信函数需要包含头文件<arpa/inet.h>，包含了这个头文件<sys/socket.h>就不用在包含了。

### 创建socket

```c
/*
* 作用：
- 创建一个套接字
* 参数:
- domain: 使用的地址族协议
	- AF_INET: 使用IPv4格式的ip地址
	- AF_INET6: 使用IPv6格式的ip地址
- type
	- SOCK_STREAM: 使用流式的传输协议
	- SOCK_DGRAM: 使用报式(报文)的传输协议
- protocol: 一般写0即可, 使用默认的协议
	- SOCK_STREAM: 流式传输默认使用的是tcp
	- SOCK_DGRAM: 报式传输默认使用的udp
* 返回值:
- 函数的返回值是一个文件描述符，通过这个文件描述符可以操作内核中的某一块内存，网络通信是基于这个文件描述符来完成的
- 成功: 可用于套接字通信的文件描述符
- 失败: -1
*/

int socket(int domain, int type, int protocol);
```

### 绑定

```c
/*
* 作用：
- 将文件描述符和本地的IP与端口进行绑定，须为大端
* 参数:
- sockfd: 监听的文件描述符, 通过socket()调用得到的返回值
- addr: 传入参数, 要绑定的IP和端口信息需要初始化到这个结构体中，IP和端口要转换为网络字节序
- addrlen: 参数addr指向的内存大小, sizeof(struct sockaddr)
* 返回值：
- 成功返回0
- 失败返回-1
*/
   
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

### 监听

```c
/*
* 作用：
- 给监听的套接字设置监听
* 参数:
- sockfd: 文件描述符, 可以通过调用socket()得到，在监听之前必须要绑定 bind()
- backlog: 同时能处理的最大连接要求，最大值为128
* 返回值：
- 调用成功返回 0
- 调用失败返回 -1
*/

int listen(int sockfd, int backlog);
```

### 接受连接

这个函数是一个阻塞函数，当没有新的客户端连接请求的时候，该函数阻塞；当检测到有新的客户端连接请求时，阻塞解除，新连接就建立了，得到的返回值也是一个文件描述符，基于这个文件描述符就可以和客户端通信了。

```c
/*
* 作用：
- 等待并接受客户端的连接请求, 建立新的连接, 会得到一个新的文件描述符(通信的)	
* 参数:
- sockfd: 监听的文件描述符
- addr: 传出参数, 里边存储了建立连接的客户端的地址信息
- addrlen: 传入传出参数，用于存储addr指向的内存大小
* 返回值：
- 函数调用成功，得到一个文件描述符, 用于和建立连接的这个客户端通信
- 调用失败返回 -1
*/
	
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

### 接收数据

如果连接没有断开，接收端接收不到数据，接收数据的函数会阻塞等待数据到达，数据到达后函数解除阻塞，开始接收数据。

当发送端断开连接，接收端无法接收到任何数据，但是这时候就不会阻塞了，函数直接返回0。

```c
/*
* 作用：
- 接收数据
* 参数:
- sockfd: 用于通信的文件描述符, accept() 函数的返回值
- buf: 指向一块有效内存, 用于存储接收是数据
- size: 参数buf指向的内存的容量
- flags: 特殊的属性, 一般不使用, 指定为 0
* 返回值:
- 大于0 实际接收的字节数
- 等于0 对方断开了连接
- -1 接收数据失败了
*/

ssize_t read(int sockfd, void *buf, size_t size);
ssize_t recv(int sockfd, void *buf, size_t size, int flags);
```

### 发送数据

```c
/*
* 作用：
- 发送数据的函数
* 参数:
- fd: 通信的文件描述符, accept() 函数的返回值
- buf: 传入参数, 要发送的字符串
- len: 要发送的字符串的长度
- flags: 特殊的属性, 一般不使用, 指定为 0
* 返回值：
- 大于0 实际发送的字节数，和参数len是相等的
- -1 发送数据失败了
*/

ssize_t write(int fd, const void *buf, size_t len);
ssize_t send(int fd, const void *buf, size_t len, int flags);
```

### 连接请求

```c
/*
* 作用
- 成功连接服务器之后, 客户端会自动随机绑定一个端口
- 服务器端调用accept()的函数, 第二个参数存储的就是客户端的IP和端口信息
* 参数:
- sockfd: 通信的文件描述符, 通过调用socket()函数就得到了
- addr: 存储了要连接的服务器端的地址信息: iP和端口，这个IP和端口也需要转换为大端然后再赋值
- addrlen: addr指针指向的内存的大小 sizeof(struct sockaddr)
* 返回值：
- 连接成功返回0
- 连接失败返回-1
*/

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

# TCP通信流程

TCP是一个面向连接的，安全的，流式传输协议，这个协议是一个传输层协议。

- 面向连接：是一个双向连接，通过三次握手完成，断开连接需要通过四次挥手完成。
- 安全：tcp通信过程中，会对发送的每一数据包都会进行校验, 如果发现数据丢失, 会自动重传
- 流式传输：发送端和接收端处理数据的速度，数据的量都可以不一致

<img src=".\img\image-20250222141105412.png" alt="image-20250222141105412" style="zoom:70%;" />

## 服务器端通信流程

1. 创建用于监听的套接字, 这个套接字是一个文件描述符

```c
int lfd = socket();
```

2. 将得到的监听的文件描述符和本地的 IP 端口进行绑定

```C
bind();
```

3. 设置监听(成功之后开始监听, 监听的是客户端的连接)

```c
listen();
```

4. 等待并接受客户端的连接请求, 建立新的连接, 会得到一个新的文件描述符(通信的)，没有新连接请求就阻塞

```c
int cfd = accept();
```

5. 通信，读写操作默认都是阻塞的

```c
// 接收数据
read(); / recv();
// 发送数据
write(); / send();
```

6. 断开连接, 关闭套接字

```c
close();
```

## 客户端的通信流程

在单线程的情况下客户端通信的文件描述符有一个，没有监听的文件描述符

1. 创建一个通信的套接字

```c
int cfd = socket();
```

2. 连接服务器, 需要知道服务器绑定的IP和端口

```c
connect();
```

3. 通信

```c
// 接收数据
read(); / recv();
// 发送数据
write(); / send();
```

4. 断开连接, 关闭文件描述符(套接字)

```c
close();
```

## 文件描述符

在tcp的服务器端,，有两类文件描述符

**监听的文件描述符**

- 只需要有一个
- 不负责和客户端通信,，负责检测客户端的连接请求， 检测到之后调用accept就可以建立新的连接

**通信的文件描述符**

- 负责和建立连接的客户端通信
- 如果有N个客户端和服务器建立了新的连接，通信的文件描述符就有N个，每个客户端和服务器都对应一个通信的文件描述符

<img src=".\img\image-20250222141452500.png" alt="image-20250222141452500" style="zoom:80%;" />

**文件描述符对应的内存结构**

- 一个文件文件描述符对应两块内存,，一块内存是读缓冲区，一块内存是写缓冲区
- 读数据:：通过文件描述符将内存中的数据读出,，这块内存称之为读缓冲区
- 写数据：通过文件描述符将数据写入到某块内存中，这块内存称之为写缓冲区

**监听的文件描述符**

- 客户端的连接请求会发送到服务器端监听的文件描述符的读缓冲区中
- 读缓冲区中有数据,，说明有新的客户端连接
- 调用accept()函数,，这个函数会检测监听文件描述符的读缓冲区
  - 检测不到数据，该函数阻塞
  - 如果检测到数据， 解除阻塞，新的连接建立

**通信的文件描述符**

- 客户端和服务器端都有通信的文件描述符
- 发送数据：调用函数 write() / send()，数据进入到内核中
  - 数据并没有被发送出去，而是将数据写入到了通信的文件描述符对应的写缓冲区中
  - 内核检测到通信的文件描述符写缓冲区中有数据，内核会将数据发送到网络中
  - 当写缓冲区满时，会陷入阻塞
- 接收数据: 调用的函数 read() / recv(), 从内核读数据
  - 数据如何进入到内核程序猿不需要处理，数据进入到通信的文件描述符的读缓冲区中
  - 内核检测到都缓冲区有数据，内核会将数据发送到内存中
  - 当读缓冲区空时，会陷入阻塞

# Windows平台

在Window中也提供了套接字通信的API，这些API函数与Linux平台的API函数几乎相同。

## 初始化套接字环境

使用Windows中的套接字函数需要额外包含对应的头文件以及加载响应的动态库

```c
// 使用包含的头文件 
include <winsock2.h>
// 使用的套接字库 
ws2_32.dll       
```

在Windows中使用套接字需要先加载套接字库（套接字环境），最后需要释放套接字资源。

```c
/*
*作用：
- 初始化Winsock库
* 参数:
- wVersionRequested: 使用的Windows Socket的版本, 一般使用的版本是 2.2
	- 初始化这个 MAKEWORD(2, 2);参数
- lpWSAData：一个WSADATA结构指针, 这是一个传入参数
	- 创建一个 WSADATA 类型的变量, 将地址传递给该函数的第二个参数
* 返回值 
- 成功返回0
- 失败返回SOCKET_ERROR。
*/

WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
```

注销Winsock相关库，函数调用成功返回0，失败返回 SOCKET_ERROR。

```c
int WSACleanup (void);
```

使用举例

```c++
WSAData wsa;
// 初始化套接字库
WSAStartup(MAKEWORD(2, 2), &wsa);

// .......

// 注销Winsock相关库
WSACleanup();
```

## 套接字通信函数

基于Linux的套接字通信流程是最全面的一套通信流程，直接使用window的套接字api进行套接字通信，和Linux平台上的通信流程完全相同。

### 结构体

```c
///////////////////////////////////////////////////////////////////////
/////////////////////////////// Windows ///////////////////////////////
///////////////////////////////////////////////////////////////////////
typedef struct in_addr {
　　union {
　　	struct{ unsigned char s_b1,s_b2, s_b3,s_b4;} S_un_b;
　　	struct{ unsigned short s_w1, s_w2;} S_un_w;
　　	unsigned long S_addr;	// 存储IP地址
　　} S_un;
}IN_ADDR;

struct sockaddr_in {
　　short int sin_family;			 /* Address family */
　　unsigned short int sin_port; 	/* Port number */
　　struct in_addr sin_addr; 		/* Internet address */
　　unsigned char sin_zero[8]; 	/* Same size as struct sockaddr */
};

///////////////////////////////////////////////////////////////////////
//////////////////////////////// Linux ////////////////////////////////
///////////////////////////////////////////////////////////////////////
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;
typedef unsigned short int sa_family_t;

struct in_addr
{
    in_addr_t s_addr;
};  

// sizeof(struct sockaddr) == sizeof(struct sockaddr_in)
struct sockaddr_in
{
    sa_family_t sin_family;     /* 地址族协议: AF_INET */
    in_port_t sin_port;         /* 端口, 2字节-> 大端  */
    struct in_addr sin_addr;    /* IP地址, 4字节 -> 大端  */
    /* 填充 8字节 */
    unsigned char sin_zero[sizeof (struct sockaddr) - sizeof(sin_family) -
                      sizeof (in_port_t) - sizeof (struct in_addr)];
};  
```

### 大小端转换函数

```c
// 主机字节序 -> 网络字节序
u_short htons (u_short hostshort );
u_long htonl ( u_long hostlong);

// 网络字节序 -> 主机字节序
u_short ntohs (u_short netshort );
u_long ntohl ( u_long netlong);

// linux、window上
inet_ntop(); 
inet_pton();

// windows 和 linux 都使用, 只能处理ipv4的ip地址
// 点分十进制IP -> 大端整形
unsigned long inet_addr (const char FAR * cp);		// windows
in_addr_t     inet_addr (const char *cp);			// linux

// 大端整形 -> 点分十进制IP
// window, linux相同
char* inet_ntoa(struct in_addr in);
```

### 套接字函数

window的api中套接字对应的类型是 SOCKET 类型, linux中是 int 类型,，本质是一样的。

```c
// 创建一个套接字
// 返回值: 成功返回套接字, 失败返回INVALID_SOCKET
SOCKET socket(int af,int type,int protocal);
参数:
    - af: 地址族协议
        - ipv4: AF_INET (windows/linux)
        - PF_INET (windows)
        - AF_INET == PF_INET
   - type: 和linux一样
       	- SOCK_STREAM
        - SOCK_DGRAM
   - protocal: 一般写0 即可
       - 在windows上的另一种写法
           - IPPROTO_TCP, 使用指定的流式协议中的tcp协议
           - IPPROTO_UDP, 使用指定的报式协议中的udp协议

// 关键字: FAR NEAR, 这两个关键字在32/64位机上是没有意义的, 指定的内存的寻址方式
// 套接字绑定本地IP和端口
// 返回值: 成功返回0，失败返回SOCKET_ERROR
int bind(SOCKET s,const struct sockaddr FAR* name, int namelen);

// 设置监听
// 返回值: 成功返回0，失败返回SOCKET_ERROR
int listen(SOCKET s,int backlog);

// 等待并接受客户端连接
// 返回值: 成功返回用于的套接字，失败返回INVALID_SOCKET。
SOCKET accept ( SOCKET s, struct sockaddr FAR* addr, int FAR* addrlen );

// 连接服务器
// 返回值: 成功返回0，失败返回SOCKET_ERROR
int connect (SOCKET s,const struct sockaddr FAR* name,int namelen );

// 接收数据
// 返回值: 成功时返回接收的字节数，收到EOF时为0，失败时返回SOCKET_ERROR。
//	==0 代表对方已经断开了连接
int recv (SOCKET s,char FAR* buf,int len,int flags);

// 发送数据
// 返回值: 成功返回传输字节数，失败返回SOCKET_ERROR。
int send (SOCKET s,const char FAR * buf, int len,int flags);

// 关闭套接字
// 返回值: 成功返回0，失败返回SOCKET_ERROR
int closesocket (SOCKET s);		// 在linux中使用的函数是: int close(int fd);

//----------------------- udp 通信函数 -------------------------
// 接收数据
int recvfrom(SOCKET s,char FAR *buf,int len,int flags,
         struct sockaddr FAR *from,int FAR *fromlen);
// 发送数据
int sendto(SOCKET s,const char FAR *buf,int len,int flags,
       const struct sockaddr FAR *to,int tolen);
```

# 线程池

**定义**  

线程池是预先创建并维护一组可复用线程的容器，通过线程复用机制减少频繁创建/销毁线程的开销。线程池中的线程数量通常与CPU核心数相关（如Java中默认核心线程数为CPU数+2）。

管理多个线程（同一进程内的轻量级执行单元），共享进程资源。

**核心原理**

- 由线程池管理器统一调度任务到空闲线程  
- 任务队列缓冲待处理请求  
- 线程从队列获取任务执行，完成后回归池中待命

**适用场景**
I/O密集型任务（如网络请求处理、文件读写），能有效应对短时高并发请求。

# 进程池

**定义** 

进程池是预先创建的一组子进程（通常3-10个），通过复用进程资源避免频繁创建进程的开销。所有子进程运行相同代码且属性一致（如优先级、PGID等），保持资源清洁。

管理多个进程（独立的内存空间和资源），通过操作系统调度。

**核心原理**

- 主进程通过算法（如轮询）或共享队列分配任务  
- 子进程通过IPC（如管道）与主进程通信  
- 每个进程独立内存空间，互不干扰

**适用场景**
CPU密集型任务（如科学计算、图像处理），可突破GIL限制实现多核并行。

**区别**

|   对比维度   |                 线程池                 |                 进程池                 |
| :----------: | :------------------------------------: | :------------------------------------: |
| **并发模型** |           多线程（共享内存）           |           多进程（独立内存）           |
| **资源开销** |       低（线程创建、切换成本低）       |       高（进程创建、切换成本高）       |
| **数据共享** |     直接共享内存（需处理同步问题）     |     需通过 IPC（如队列、管道）通信     |
| **适用场景** | I/O 密集型任务（如网络请求、文件读写） | CPU 密集型任务（如数值计算、图像处理） |
|  **稳定性**  |      线程崩溃可能导致整个进程崩溃      |         进程崩溃不影响其他进程         |
| **并行能力** |              受限于全局锁              |     真正并行（多核 CPU 利用率高）      |

**优势与局限**

优势

- 资源复用：避免频繁创建/销毁开销 
- 可控并发：限制最大线程/进程数防止过载  
- 快速响应：任务到达时直接分配空闲资源  

局限

- 线程池：GIL限制CPU利用率，复杂任务易死锁  
- 进程池：IPC通信成本高，不适合短时小任务  

# 实例

## TCP服务器端-1 to 1

该代码有三处可能会导致服务器阻塞，一旦导致阻塞，服务器便无法继续执行。

```c
/* server.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    // 1. 创建监听的套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1)
    {
        perror("socket");
        exit(0);
    }

    // 2. 将socket()返回值和本地的IP端口绑定到一起
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);   // 大端端口
    // INADDR_ANY代表本机的所有IP, 假设有三个网卡就有三个IP地址
    // 这个宏可以代表本地IP地址
    addr.sin_addr.s_addr = INADDR_ANY;  // 这个宏的值为0 == 0.0.0.0
	//inet_pton(AF_INET, "192.168.237.131", &addr.sin_addr.s_addr);
    int ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("bind");
        exit(0);
    }

    // 3. 设置监听
    ret = listen(lfd, 128);
    if(ret == -1)
    {
        perror("listen");
        exit(0);
    }

    // 4. 阻塞等待并接受客户端连接
    struct sockaddr_in cliaddr;
    int clilen = sizeof(cliaddr);
    int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &clilen);
    if(cfd == -1)
    {
        perror("accept");
        exit(0);
    }
    
    // 打印客户端的地址信息 需将大端转为小端
    char ip[24] = {0};
    printf("客户端的IP地址: %s, 端口: %d\n",
            inet_ntop(AF_INET,&cliaddr.sin_addr.s_addr,ip, sizeof(ip)),
           	ntohs(cliaddr.sin_port));

    // 5. 和客户端通信
    while(1)
    {
        // 接收数据
        char buf[1024];
        memset(buf, 0, sizeof(buf));
   
        int len = read(cfd, buf, sizeof(buf));
        if(len > 0)
        {
            printf("客户端say: %s\n", buf);
            write(cfd, buf, len);
        }
        else if(len  == 0)
        {
            printf("客户端断开了连接...\n");
            break;
        }
        else
        {
            perror("read");
            break;
        }
    }
	
    // 关闭文件描述符
    close(cfd);		//通信文件描述符
    close(lfd);		//监听文件描述符

    return 0;
}
```

## TCP服务器端-1 to N

**实现方案**

- 使用多线程，较多进程更节省资源
- 使用多进程
- 使用I/O多路复用，效率较低
- 使用I/O多路复用+多线程

**使用多线程实现**

- 主线程接收客户端的连接

- 子线程负责与客户端进行通信

注

1. 子线程中栈区资源是不共享的，对于共享区域，如果子线程需要进行(写)操作，此时需要确保线程同步，可以通过信号量来实现

```c
/* server.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

// 信息结构体,用于向子进程传递参数
struct SockInfo
{
    int fd;
    struct sockaddr_in addr;
};

struct SockInfo infos[512];

void* working(void* arg);

int main()
{
    // 1. 创建监听的套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1)
    {
        perror("socket");
        exit(0);
    }

    // 2. 将socket()返回值和本地的IP端口绑定到一起
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);   // 大端端口
    // INADDR_ANY代表本机的所有IP, 假设有三个网卡就有三个IP地址
    // 这个宏可以代表本地IP地址
    addr.sin_addr.s_addr = INADDR_ANY;  // 这个宏的值为0 == 0.0.0.0
    //inet_pton(AF_INET, "192.168.237.131", &addr.sin_addr.s_addr);
    int ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("bind");
        exit(0);
    }

    // 3. 设置监听
    ret = listen(lfd, 128);
    if(ret == -1)
    {
        perror("listen");
        exit(0);
    }

    //初始化结构体数组
    int max = sizeof(infos) / sizeof(infos[0]);
    for(int i = 0; i < max; i++)
    {
        bzero(&infos[i], sizeof(infos[i]));
        infos[i].fd = -1;
    }

    // 4. 阻塞等待并接受客户端连接
    int clilen = sizeof(sockaddr_in);
    while(1)
    {
        struct SockInfo* pinfo;
        for (int i = 0; i < max ; ++i)      //找到当前空闲的结构体
        {
            if(infos[i].fd == -1)
            {
                pinfo = &infos[i];
                break;
            }
        }
        int cfd = accept(lfd, (struct sockaddr*)&pinfo->addr, &clilen);
        if(cfd == -1)
        {
            perror("accept");
            break;
        }
        pinfo->fd = cfd;            //保存通信文件描述符

        // 创建子线程 负责与客户端通信
        pthread_t tid;
        pthread_create(&tid, NULL, working, pinfo);  //执行子线程任务
        // 回收子线程资源
        //pthread_join(tid, NULL);         // 阻塞函数
        pthread_detach(tid);            // 子线程在结束时会自动释放所有资源
    }
    // 关闭监听文件描述符
    close(lfd);

    return 0;
}

void* working(void* arg)
{
    struct SockInfo* pinfo = (struct SockInfo*)arg; //获取传递信息
    // 打印客户端的地址信息 需将大端转为小端
    char ip[24] = {0};
    printf("客户端的IP地址: %s, 端口: %d\n",
           inet_ntop(AF_INET,&pinfo->addr.sin_addr.s_addr,ip, sizeof(ip)),
           ntohs(pinfo->addr.sin_port));

    // 5. 和客户端通信
    while(1)
    {
        // 接收数据
        char buf[1024];
        memset(buf, 0, sizeof(buf));

        int len = read(pinfo->fd, buf, sizeof(buf));
        if(len > 0)
        {
            printf("客户端say: %s\n", buf);
            write(pinfo->fd, buf, len);
        }
        else if(len  == 0)
        {
            printf("客户端断开了连接...\n");
            break;
        }
        else
        {
            perror("read");
            break;
        }
    }

    // 关闭文件描述符
    close(pinfo->fd);				//通信文件描述符
    pinfo->fd = -1;                 // 重置为-1

    return NULL;
}
```

## TCP服务器端(线程池)-1 to N

### 线程池原理

我们使用线程的时候就去创建一个线程，这样实现起来非常简便，但是就会有一个问题：如果并发的线程数量很多，并且每个线程都是执行一个时间很短的任务就结束了，这样频繁创建线程就会大大降低系统的效率，因为频繁创建线程和销毁线程需要时间。

线程池是一种多线程处理形式，处理过程中将任务添加到队列，然后在创建线程后自动启动这些任务。线程池线程都是后台线程。每个线程都使用默认的堆栈大小，以默认的优先级运行，并处于多线程单元中。

线程池的组成主要分为3个部分

- 任务队列，存储需要处理的任务，由工作的线程来处理这些任务
  - 通过线程池提供的API函数，将一个待处理的任务添加到任务队列，或者从任务队列中删除
  - 已处理的任务会被从任务队列中删除
- 工作的线程，N个
  - 线程池中维护了一定数量的工作线程, 他们的作用是是不停的读任务队列, 从里边取出任务并处理
  - 如果任务队列为空, 工作的线程将会被阻塞 (使用条件变量/信号量阻塞)
  - 如果阻塞之后有了新的任务, 由生产者将阻塞解除, 工作线程开始工作
- 管理者线程，1个
  - 它的任务是周期性的对任务队列中的任务数量以及处于忙状态的工作线程个数进行检测
  - 当任务过多的时候, 可以适当的创建一些新的工作线程
  - 当任务过少的时候, 可以适当的销毁一些工作的线程

<img src=".\img\image-20250222210927072.png" alt="image-20250222210927072" style="zoom:70%;" />

### 源代码

**任务队列**

```c
/* c */
// 任务结构体
typedef struct Task
{
    void (*function)(void* arg);
    void* arg;
}Task;
```

```c++
/* c++ 类声明 */

// 定义任务结构体
using callback = void(*)(void*);
struct Task
{
    Task()
    {
        function = nullptr;
        arg = nullptr;
    }
    Task(callback f, void* arg)
    {
        function = f;
        this->arg = arg;
    }
    callback function;
    void* arg;
};

// 任务队列
class TaskQueue
{
public:
    TaskQueue();
    ~TaskQueue();

    // 添加任务
    void addTask(Task& task);
    void addTask(callback func, void* arg);

    // 取出一个任务
    Task takeTask();

    // 获取当前队列中任务个数
    inline int taskNumber()
    {
        return m_queue.size();
    }

private:
    pthread_mutex_t m_mutex;    // 互斥锁
    std::queue<Task> m_queue;   // 任务队列
};
```

```c++
/* c++ 类定义 */

TaskQueue::TaskQueue()
{
    pthread_mutex_init(&m_mutex, NULL);
}

TaskQueue::~TaskQueue()
{
    pthread_mutex_destroy(&m_mutex);
}

void TaskQueue::addTask(Task& task)
{
    pthread_mutex_lock(&m_mutex);
    m_queue.push(task);
    pthread_mutex_unlock(&m_mutex);
}

void TaskQueue::addTask(callback func, void* arg)
{
    pthread_mutex_lock(&m_mutex);
    Task task;
    task.function = func;
    task.arg = arg;
    m_queue.push(task);
    pthread_mutex_unlock(&m_mutex);
}

Task TaskQueue::takeTask()
{
    Task t;
    pthread_mutex_lock(&m_mutex);
    if (m_queue.size() > 0)
    {
        t = m_queue.front();
        m_queue.pop();
    }
    pthread_mutex_unlock(&m_mutex);
    return t;
}
```

**线程池定义**

```c
/* c */

// 线程池结构体
struct ThreadPool
{
    // 任务队列
    Task* taskQ;
    int queueCapacity;  // 容量
    int queueSize;      // 当前任务个数
    int queueFront;     // 队头 -> 取数据
    int queueRear;      // 队尾 -> 放数据

    pthread_t managerID;    // 管理者线程ID
    pthread_t *threadIDs;   // 工作的线程ID
    int minNum;             // 最小线程数量
    int maxNum;             // 最大线程数量
    int busyNum;            // 忙的线程的个数
    int liveNum;            // 存活的线程的个数
    int exitNum;            // 要销毁的线程个数
    pthread_mutex_t mutexPool;  // 锁整个的线程池
    pthread_mutex_t mutexBusy;  // 锁busyNum变量
    pthread_cond_t notFull;     // 任务队列是不是满了
    pthread_cond_t notEmpty;    // 任务队列是不是空了

    int shutdown;           // 是不是要销毁线程池, 销毁为1, 不销毁为0
};
```

```c++
/* c++ 类声明 */

class ThreadPool
{
public:
    ThreadPool(int min, int max);
    ~ThreadPool();

    // 添加任务
    void addTask(Task task);
    // 获取忙线程的个数
    int getBusyNumber();
    // 获取活着的线程个数
    int getAliveNumber();

private:
    // 工作的线程的任务函数
    static void* worker(void* arg);
    // 管理者线程的任务函数
    static void* manager(void* arg);
    void threadExit();

private:
    pthread_mutex_t m_lock;
    pthread_cond_t m_notEmpty;
    pthread_t* m_threadIDs;
    pthread_t m_managerID;
    TaskQueue* m_taskQ;
    int m_minNum;
    int m_maxNum;
    int m_busyNum;
    int m_aliveNum;
    int m_exitNum;
    bool m_shutdown = false;
};
```

```c++
/* c++ 类定义 */

ThreadPool::ThreadPool(int minNum, int maxNum)
{
    // 实例化任务队列
    m_taskQ = new TaskQueue;
    do {
        // 初始化线程池
        m_minNum = minNum;
        m_maxNum = maxNum;
        m_busyNum = 0;
        m_aliveNum = minNum;

        // 根据线程的最大上限给线程数组分配内存
        m_threadIDs = new pthread_t[maxNum];
        if (m_threadIDs == nullptr)
        {
            cout << "malloc thread_t[] 失败...." << endl;;
            break;
        }
        // 初始化
        memset(m_threadIDs, 0, sizeof(pthread_t) * maxNum);
        // 初始化互斥锁,条件变量
        if (pthread_mutex_init(&m_lock, NULL) != 0 ||
            pthread_cond_init(&m_notEmpty, NULL) != 0)
        {
            cout << "init mutex or condition fail..." << endl;
            break;
        }

        /////////////////// 创建线程 //////////////////
        // 根据最小线程个数, 创建线程
        for (int i = 0; i < minNum; ++i)
        {
            pthread_create(&m_threadIDs[i], NULL, worker, this);
            cout << "创建子线程, ID: " << to_string(m_threadIDs[i]) << endl;
        }
        // 创建管理者线程, 1个
        pthread_create(&m_managerID, NULL, manager, this);
    } while (0);
}

ThreadPool::~ThreadPool()
{
    m_shutdown = 1;
    // 销毁管理者线程
    pthread_join(m_managerID, NULL);
    // 唤醒所有消费者线程
    for (int i = 0; i < m_aliveNum; ++i)
    {
        pthread_cond_signal(&m_notEmpty);
    }

    if (m_taskQ) delete m_taskQ;
    if (m_threadIDs) delete[]m_threadIDs;
    pthread_mutex_destroy(&m_lock);
    pthread_cond_destroy(&m_notEmpty);
}

void ThreadPool::addTask(Task task)
{
    if (m_shutdown)
    {
        return;
    }
    // 添加任务，不需要加锁，任务队列中有锁
    m_taskQ->addTask(task);
    // 唤醒工作的线程
    pthread_cond_signal(&m_notEmpty);
}

int ThreadPool::getAliveNumber()
{
    int threadNum = 0;
    pthread_mutex_lock(&m_lock);
    threadNum = m_aliveNum;
    pthread_mutex_unlock(&m_lock);
    return threadNum;
}

int ThreadPool::getBusyNumber()
{
    int busyNum = 0;
    pthread_mutex_lock(&m_lock);
    busyNum = m_busyNum;
    pthread_mutex_unlock(&m_lock);
    return busyNum;
}


// 工作线程任务函数
void* ThreadPool::worker(void* arg)
{
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    // 一直不停的工作
    while (true)
    {
        // 访问任务队列(共享资源)加锁
        pthread_mutex_lock(&pool->m_lock);
        // 判断任务队列是否为空, 如果为空工作线程阻塞
        while (pool->m_taskQ->taskNumber() == 0 && !pool->m_shutdown)
        {
            cout << "thread " << to_string(pthread_self()) << " waiting..." << endl;
            // 阻塞线程
            pthread_cond_wait(&pool->m_notEmpty, &pool->m_lock);

            // 解除阻塞之后, 判断是否要销毁线程
            if (pool->m_exitNum > 0)
            {
                pool->m_exitNum--;
                if (pool->m_aliveNum > pool->m_minNum)
                {
                    pool->m_aliveNum--;
                    pthread_mutex_unlock(&pool->m_lock);
                    pool->threadExit();
                }
            }
        }
        // 判断线程池是否被关闭了
        if (pool->m_shutdown)
        {
            pthread_mutex_unlock(&pool->m_lock);
            pool->threadExit();
        }

        // 从任务队列中取出一个任务
        Task task = pool->m_taskQ->takeTask();
        // 工作的线程+1
        pool->m_busyNum++;
        // 线程池解锁
        pthread_mutex_unlock(&pool->m_lock);
        // 执行任务
        cout << "thread " << to_string(pthread_self()) << " start working..." << endl;
        task.function(task.arg);
        delete task.arg;
        task.arg = nullptr;

        // 任务处理结束
        cout << "thread " << to_string(pthread_self()) << " end working...";
        pthread_mutex_lock(&pool->m_lock);
        pool->m_busyNum--;
        pthread_mutex_unlock(&pool->m_lock);
    }

    return nullptr;
}


// 管理者线程任务函数
void* ThreadPool::manager(void* arg)
{
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    // 如果线程池没有关闭, 就一直检测
    while (!pool->m_shutdown)
    {
        // 每隔5s检测一次
        sleep(5);
        // 取出线程池中的任务数和线程数量
        //  取出工作的线程池数量
        pthread_mutex_lock(&pool->m_lock);
        int queueSize = pool->m_taskQ->taskNumber();
        int liveNum = pool->m_aliveNum;
        int busyNum = pool->m_busyNum;
        pthread_mutex_unlock(&pool->m_lock);

        // 创建线程
        const int NUMBER = 2;
        // 当前任务个数>存活的线程数 && 存活的线程数<最大线程个数
        if (queueSize > liveNum && liveNum < pool->m_maxNum)
        {
            // 线程池加锁
            pthread_mutex_lock(&pool->m_lock);
            int num = 0;
            for (int i = 0; i < pool->m_maxNum && num < NUMBER
                && pool->m_aliveNum < pool->m_maxNum; ++i)
            {
                if (pool->m_threadIDs[i] == 0)
                {
                    pthread_create(&pool->m_threadIDs[i], NULL, worker, pool);
                    num++;
                    pool->m_aliveNum++;
                }
            }
            pthread_mutex_unlock(&pool->m_lock);
        }

        // 销毁多余的线程
        // 忙线程*2 < 存活的线程数目 && 存活的线程数 > 最小线程数量
        if (busyNum * 2 < liveNum && liveNum > pool->m_minNum)
        {
            pthread_mutex_lock(&pool->m_lock);
            pool->m_exitNum = NUMBER;
            pthread_mutex_unlock(&pool->m_lock);
            for (int i = 0; i < NUMBER; ++i)
            {
                pthread_cond_signal(&pool->m_notEmpty);
            }
        }
    }
    return nullptr;
}

// 线程退出
void ThreadPool::threadExit()
{
    pthread_t tid = pthread_self();
    for (int i = 0; i < m_maxNum; ++i)
    {
        if (m_threadIDs[i] == tid)
        {
            cout << "threadExit() function: thread " 
                << to_string(pthread_self()) << " exiting..." << endl;
            m_threadIDs[i] = 0;
            break;
        }
    }
    pthread_exit(NULL);
}
```

**头文件声明**

```c
#ifndef _THREADPOOL_H
#define _THREADPOOL_H

typedef struct ThreadPool ThreadPool;
// 创建线程池并初始化
ThreadPool *threadPoolCreate(int min, int max, int queueSize);

// 销毁线程池
int threadPoolDestroy(ThreadPool* pool);

// 给线程池添加任务
void threadPoolAdd(ThreadPool* pool, void(*func)(void*), void* arg);

// 获取线程池中工作的线程的个数
int threadPoolBusyNum(ThreadPool* pool);

// 获取线程池中活着的线程的个数
int threadPoolAliveNum(ThreadPool* pool);

//////////////////////
// 工作的线程(消费者线程)任务函数
void* worker(void* arg);
// 管理者线程任务函数
void* manager(void* arg);
// 单个线程退出
void threadExit(ThreadPool* pool);
#endif  // _THREADPOOL_H
```

**源文件定义**

```c
ThreadPool* threadPoolCreate(int min, int max, int queueSize)
{
    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    do 
    {
        if (pool == NULL)
        {
            printf("malloc threadpool fail...\n");
            break;
        }

        pool->threadIDs = (pthread_t*)malloc(sizeof(pthread_t) * max);
        if (pool->threadIDs == NULL)
        {
            printf("malloc threadIDs fail...\n");
            break;
        }
        memset(pool->threadIDs, 0, sizeof(pthread_t) * max);
        pool->minNum = min;
        pool->maxNum = max;
        pool->busyNum = 0;
        pool->liveNum = min;    // 和最小个数相等
        pool->exitNum = 0;

        if (pthread_mutex_init(&pool->mutexPool, NULL) != 0 ||
            pthread_mutex_init(&pool->mutexBusy, NULL) != 0 ||
            pthread_cond_init(&pool->notEmpty, NULL) != 0 ||
            pthread_cond_init(&pool->notFull, NULL) != 0)
        {
            printf("mutex or condition init fail...\n");
            break;
        }

        // 任务队列
        pool->taskQ = (Task*)malloc(sizeof(Task) * queueSize);
        pool->queueCapacity = queueSize;
        pool->queueSize = 0;
        pool->queueFront = 0;
        pool->queueRear = 0;

        pool->shutdown = 0;

        // 创建线程
        pthread_create(&pool->managerID, NULL, manager, pool);
        for (int i = 0; i < min; ++i)
        {
            pthread_create(&pool->threadIDs[i], NULL, worker, pool);
        }
        return pool;
    } while (0);

    // 释放资源
    if (pool && pool->threadIDs) free(pool->threadIDs);
    if (pool && pool->taskQ) free(pool->taskQ);
    if (pool) free(pool);

    return NULL;
}

int threadPoolDestroy(ThreadPool* pool)
{
    if (pool == NULL)
    {
        return -1;
    }

    // 关闭线程池
    pool->shutdown = 1;
    // 阻塞回收管理者线程
    pthread_join(pool->managerID, NULL);
    // 唤醒阻塞的消费者线程
    for (int i = 0; i < pool->liveNum; ++i)
    {
        pthread_cond_signal(&pool->notEmpty);
    }
    // 释放堆内存
    if (pool->taskQ)
    {
        free(pool->taskQ);
    }
    if (pool->threadIDs)
    {
        free(pool->threadIDs);
    }

    pthread_mutex_destroy(&pool->mutexPool);
    pthread_mutex_destroy(&pool->mutexBusy);
    pthread_cond_destroy(&pool->notEmpty);
    pthread_cond_destroy(&pool->notFull);

    free(pool);
    pool = NULL;

    return 0;
}


void threadPoolAdd(ThreadPool* pool, void(*func)(void*), void* arg)
{
    pthread_mutex_lock(&pool->mutexPool);
    while (pool->queueSize == pool->queueCapacity && !pool->shutdown)
    {
        // 阻塞生产者线程
        pthread_cond_wait(&pool->notFull, &pool->mutexPool);
    }
    if (pool->shutdown)
    {
        pthread_mutex_unlock(&pool->mutexPool);
        return;
    }
    // 添加任务
    pool->taskQ[pool->queueRear].function = func;
    pool->taskQ[pool->queueRear].arg = arg;
    pool->queueRear = (pool->queueRear + 1) % pool->queueCapacity;
    pool->queueSize++;

    pthread_cond_signal(&pool->notEmpty);
    pthread_mutex_unlock(&pool->mutexPool);
}

int threadPoolBusyNum(ThreadPool* pool)
{
    pthread_mutex_lock(&pool->mutexBusy);
    int busyNum = pool->busyNum;
    pthread_mutex_unlock(&pool->mutexBusy);
    return busyNum;
}

int threadPoolAliveNum(ThreadPool* pool)
{
    pthread_mutex_lock(&pool->mutexPool);
    int aliveNum = pool->liveNum;
    pthread_mutex_unlock(&pool->mutexPool);
    return aliveNum;
}

void* worker(void* arg)
{
    ThreadPool* pool = (ThreadPool*)arg;

    while (1)
    {
        pthread_mutex_lock(&pool->mutexPool);
        // 当前任务队列是否为空
        while (pool->queueSize == 0 && !pool->shutdown)
        {
            // 阻塞工作线程
            pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);

            // 判断是不是要销毁线程
            if (pool->exitNum > 0)
            {
                pool->exitNum--;
                if (pool->liveNum > pool->minNum)
                {
                    pool->liveNum--;
                    pthread_mutex_unlock(&pool->mutexPool);
                    threadExit(pool);
                }
            }
        }

        // 判断线程池是否被关闭了
        if (pool->shutdown)
        {
            pthread_mutex_unlock(&pool->mutexPool);
            threadExit(pool);
        }

        // 从任务队列中取出一个任务
        Task task;
        task.function = pool->taskQ[pool->queueFront].function;
        task.arg = pool->taskQ[pool->queueFront].arg;
        // 移动头结点
        pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
        pool->queueSize--;
        // 解锁
        pthread_cond_signal(&pool->notFull);
        pthread_mutex_unlock(&pool->mutexPool);

        printf("thread %ld start working...\n", pthread_self());
        pthread_mutex_lock(&pool->mutexBusy);
        pool->busyNum++;
        pthread_mutex_unlock(&pool->mutexBusy);
        task.function(task.arg);
        free(task.arg);
        task.arg = NULL;

        printf("thread %ld end working...\n", pthread_self());
        pthread_mutex_lock(&pool->mutexBusy);
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexBusy);
    }
    return NULL;
}

void* manager(void* arg)
{
    ThreadPool* pool = (ThreadPool*)arg;
    while (!pool->shutdown)
    {
        // 每隔3s检测一次
        sleep(3);

        // 取出线程池中任务的数量和当前线程的数量
        pthread_mutex_lock(&pool->mutexPool);
        int queueSize = pool->queueSize;
        int liveNum = pool->liveNum;
        pthread_mutex_unlock(&pool->mutexPool);

        // 取出忙的线程的数量
        pthread_mutex_lock(&pool->mutexBusy);
        int busyNum = pool->busyNum;
        pthread_mutex_unlock(&pool->mutexBusy);

        // 添加线程
        // 任务的个数>存活的线程个数 && 存活的线程数<最大线程数
        if (queueSize > liveNum && liveNum < pool->maxNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            int counter = 0;
            for (int i = 0; i < pool->maxNum && counter < NUMBER
                && pool->liveNum < pool->maxNum; ++i)
            {
                if (pool->threadIDs[i] == 0)
                {
                    pthread_create(&pool->threadIDs[i], NULL, worker, pool);
                    counter++;
                    pool->liveNum++;
                }
            }
            pthread_mutex_unlock(&pool->mutexPool);
        }
        // 销毁线程
        // 忙的线程*2 < 存活的线程数 && 存活的线程>最小线程数
        if (busyNum * 2 < liveNum && liveNum > pool->minNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            pool->exitNum = NUMBER;
            pthread_mutex_unlock(&pool->mutexPool);
            // 让工作的线程自杀
            for (int i = 0; i < NUMBER; ++i)
            {
                pthread_cond_signal(&pool->notEmpty);
            }
        }
    }
    return NULL;
}

void threadExit(ThreadPool* pool)
{
    pthread_t tid = pthread_self();
    for (int i = 0; i < pool->maxNum; ++i)
    {
        if (pool->threadIDs[i] == tid)
        {
            pool->threadIDs[i] = 0;
            printf("threadExit() called, %ld exiting...\n", tid);
            break;
        }
    }
    pthread_exit(NULL);
}
```

### 线程池版代码

```c
/* server.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "threadpool.h"

// 信息结构体,用于向working传递参数
struct SockInfo
{
    int fd;
    struct sockaddr_in addr;
};

typedef struct PoolInfo
{
    ThreadPool* p;
    int fd;
}PoolInfo;

void working(void* arg);
void acceptConn(void* arg);

int main() {
    // 1. 创建监听的套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        exit(0);
    }

    // 2. 将socket()返回值和本地的IP端口绑定到一起
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);   // 大端端口
    // INADDR_ANY代表本机的所有IP, 假设有三个网卡就有三个IP地址
    // 这个宏可以代表本地IP地址
    addr.sin_addr.s_addr = INADDR_ANY;  // 这个宏的值为0 == 0.0.0.0
    //inet_pton(AF_INET, "192.168.237.131", &addr.sin_addr.s_addr);
    int ret = bind(lfd, (struct sockaddr *) &addr, sizeof(addr));
    if (ret == -1) {
        perror("bind");
        exit(0);
    }

    // 3. 设置监听
    ret = listen(lfd, 128);
    if (ret == -1) {
        perror("listen");
        exit(0);
    }

    // 创建线程池
    ThreadPool* pool = threadPoolCreate(3, 8, 100);                 //最小线程数 最大线程数 任务队列大小
    PoolInfo* info = (PoolInfo*)malloc(sizeof(PoolInfo));
    info->p = pool;
    info->fd = lfd;
    // 往线程池中添加任务 检测有无新客户端连接 info 在acceptConn函数执行完后被释放
    threadPoolAdd(pool, acceptConn, info);
    //一些销毁线程池
    pthread_exit(NULL);                                         // 当前线程退出

    return 0;
}

void acceptConn(void* arg)
{
    PoolInfo* poolInfo = (PoolInfo*)arg;
    // 4. 阻塞等待并接受客户端连接
    int clilen = sizeof(sockaddr_in);
    while(1)
    {
        struct SockInfo* pinfo;
        pinfo = (struct SockInfo*) malloc(sizeof(struct SockInfo));
        pinfo->fd = accept(poolInfo->fd, (struct sockaddr*)&pinfo->addr, &clilen);
        if(pinfo->fd == -1)
        {
            perror("accept");
            break;
        }
        //添加通信的任务
        threadPoolAdd(poolInfo->p, working, pinfo);
    }
    // 关闭监听文件描述符
    close(poolInfo->fd);

}

void working(void* arg)
{
    struct SockInfo* pinfo = (struct SockInfo*)arg; //获取传递信息
    // 打印客户端的地址信息 需将大端转为小端
    char ip[24] = {0};
    printf("客户端的IP地址: %s, 端口: %d\n",
           inet_ntop(AF_INET,&pinfo->addr.sin_addr.s_addr,ip, sizeof(ip)),
           ntohs(pinfo->addr.sin_port));

    // 5. 和客户端通信
    while(1)
    {
        // 接收数据
        char buf[1024];
        memset(buf, 0, sizeof(buf));

        int len = read(pinfo->fd, buf, sizeof(buf));
        if(len > 0)
        {
            printf("客户端say: %s\n", buf);
            write(pinfo->fd, buf, len);
        }
        else if(len  == 0)
        {
            printf("客户端断开了连接...\n");
            break;
        }
        else
        {
            perror("read");
            break;
        }
    }

    // 关闭文件描述符
    close(pinfo->fd);		//通信文件描述符    
}
```

## TCP客户端-1 to 1

```c
/* client.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    // 1. 创建通信的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
    {
        perror("socket");
        exit(0);
    }

    // 2. 连接服务器
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);   // 大端端口
    inet_pton(AF_INET, "192.168.237.131", &addr.sin_addr.s_addr);

    int ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("connect");
        exit(0);
    }

    // 3. 和服务器端通信
    int number = 0;
    while(1)
    {
        // 发送数据
        char buf[1024];
        sprintf(buf, "你好, 服务器...%d\n", number++);
        write(fd, buf, strlen(buf)+1);
        
        // 接收数据
        memset(buf, 0, sizeof(buf));
        int len = read(fd, buf, sizeof(buf));
        if(len > 0)
        {
            printf("服务器say: %s\n", buf);
        }
        else if(len  == 0)
        {
            printf("服务器断开了连接...\n");
            break;
        }
        else
        {
            perror("read");
            break;
        }
        sleep(1);   // 每隔1s发送一条数据
    }

    // 关闭通信描述符
    close(fd);

    return 0;
}
```

