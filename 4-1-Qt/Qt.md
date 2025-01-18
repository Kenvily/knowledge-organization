<font size = 6>Qt</font>

[toc]

# Qt介绍

## Introduction

跨平台C++图形用户界面应用程序开发框架，既可以开发GUI程序，也可用于开发非GUI程序，如控制台工具和服务器。

Characteristic

- 面对对象，容易扩展

- 控件间相互通信

## QtCreator

### Introduction

用于Qt开发的轻量级跨平台集成开发环境。

Characteristic

- 逻辑与界面分离

- 在不同系统中的程序可以在多平台上运行

### Qt Creator文件结构

* .pro：工程文件
* .h：头文件
* .cpp：源文件，main和类
* .ui：界面文件
* .user：本地配置文件，换设备时记得删除

## Qt编译过程

1. 编写源代码
2. 修改环境变量
3. 生成工程文件（qmake -project）
4. 生成Makefile（qmake）
5. 编译工程（mingw2-make）
6. 运行生成exe文件

## 控件

* 页面窗口

> QWidget：不带菜单栏
>
> QMainwindow：带有菜单栏

* 布局

> Vertical layout：垂直布局，界面全选垂直布局后，各组件随窗口大小变化而变化
>
> horizontal layout：水平布局
>
> grid layout：网格布局，如计算器
>
> form layout:表单布局，一般用于组合控件

* 按钮

> push button：方框按钮
>
> tool button：与前者差不多
>
> radio button：点框
>
> check box：复选框

* 垫子/弹簧

> 隐形的，用于调整组件间间隔

* 单元视图

>  实现数据的现实和分离，常与数据库联用
>
> table view：表单视图

* 单元控件

> 文件管理系统常用

* 容器

> 容纳控件

* 输入控件

> LineEdit：行编辑框

> TextEdit：文本编辑框

* 显示控件

> Label：标签

# 基础知识

## 界面设置

```c++
//设置窗口名称
this->setWindowTitle("TCP客户端");

//设置应用程序图标
setWindowIcon(QIcon(":/new/prefix1/img/client.svg"));

//设置窗口固定大小
this->setMaximumSize(219,362);
this->setMinimumSize(219,362);

//字体
QFont f("微软雅黑",14);
ui->displayLineEdit->setFont(f);

//改变=按钮背景色
ui->equalButton->setStyleSheet("background-color:rgb(128,128,128)");
```

## QString

类似于C++中的string

## 标签上显示图片

### QPixmap

```c++
//标签显示图片
QPixmap pix("..\\..\\1.jpg");
ui->label->setPixmap(pix);
```

### QImage

```c++
//标签显示图片
QImage img;
img.load("..\\..\\1.jpg");
ui->label->setPixmap(QPixmap::fromImage(img));
```

注

1. 适配图片：QLable -> scaleContents

## qDebug

进行调试

```c++
qDebug() << fileName;	//打印信息
```

## QMessageBox

提示框，所含类别如下

<img src=".\img\message-1.png" alt="messageBox" style="zoom:50%;" />

### 基本用法

显示一个信息提示框

```c++
QMessageBox::warning(this,"连接提示","连接失败");
```

### 进阶用法

显示一个选择框

```c++
int ok = QMessageBox::warning(this,"确定？","删否？",QMessageBox::Yes,QMessageBox::No);
if(ok==QMessageBox::Yes){
  //...
}else{
  //...
}
```

# 信号与槽

信号和槽是两个独立的，需要关联起来，常使用connect进行连接。

```c++
//谁发出信号 发出什么信号 谁处理信号 怎么处理
connect(×，×，×，×)；
```

## signals

类似于java中监听器，用于发送信号。

Characteristic:

- 对象都是可以发出信号的，也是以函数形式存在的

- 信号函数只有声明没有定义，槽函数有声明和定义

### 自定义信号

1. .h头文件中声明自定义信号

```c++
signals:
    void connectionDataRequested(QString IP, QString port,QString state); //带参
    void connectFinish();		//不带参
```

2. .cpp文件中发送信号

```c++
emit connectionDataRequested(IP,port,state);    //发送客户端配置信号
emit connectFinish();                           //发送连接完成信号
```

3. 使用自定义信号

```c++
//无参
connect(myClient,&ClientHandler::connectFinish,[this](){//...});
//带参
connect(myClient,&ClientHandler::connectionDataRequested,this,&MyTcpServer::sendClientLog);
// or
connect(this, SIGNAL(mySignal(int, float)), this, SLOT(mySlot(int)));
```

4. 通过自定义槽函数来使用自定义信号中的参数

```c++
//先声明后定义
void sendClientLog(QString IP, QString port, QString state);
//定义
void MyTcpServer::sendClientLog(QString IP, QString port, QString state){
    //使用自定义信号传递的参数IP、port、state
}
```

注

1. 当信号的参数与槽函数的参数数量不同时，只能是信号的参数数量多于槽函数的参数数量，且前面相同数量的参数类型应一致，信号中多余的参数会被忽略。

## slots

类似于java中事件，用于触发信号后做出响应。

## 添加槽函数方式

1. 自动连接（无需使用connect函数）

在.ui文件中为控件直接添加槽函数（无需connect显式关联）

2. 使connect函数

- 宏定义形式

```c++
//宏的形式
connect(ui->cmdEdit,SIGNAL(returnPressed()),this,SLOT(on_commitButton_clicked()));
```

- 指针重载形式

```c++
//指针的重载形式
connect(ui->cancelButton, &QPushButton::clicked, this, &Widget::on_cancelButton_clicked);
```

- Lambda表达式

```c++
//Lambda表达式
connect(ui->browseButton, &QPushButton::clicked,[this](){
    QMessageBox::information(this,"信息","点击浏览");
})
```

注

1. （自定义）信号与槽函数一定要在头文件下进行声明
2. connect绑定信号与槽函数可以叠加绑定，要区分执行次数，<font color = red>不要重复绑定</font>

# 定时器

## QObject

不同定时器各有一个不同的ID。

### startTimer 

开启定时器

```c++
void Widget::on_startButton_clicked()
{
    //开启定时器，返回定时器编号
    myTimerId = this->startTimer(TIMEOUT);  //单位：毫秒
}
//myTimerId为私有成员变量
```

### killTimer

关闭定时器

```c++
void Widget::on_stopButton_clicked()
{
    //杀死指定Id的定时器
    this->killTimer(myTimerId);
}
```

### timeEvent

不同定时器每次TIMEOUT后都会自动调用该事件

### 示例

1. 需先在头文件进行声明

```c++
public:
	void timevent(QTimerEvent *event)	//重写虚函数
```

2. 重写timeEvent

```c++
void Widget::timerEvent(QTimerEvent *event)
{
    if(event->timerId() != myTimerId)
    {
        return;
    }
    QString dirPath("..\\..\\");
    dirPath += QString::number(picId);
    dirPath += ".jpg";

    QPixmap pix(dirPath);
    ui->label->setPixmap(pix);

    picId ++;
    if(4 == picId)
    {
        picId = 1;
    }
}
```

## QTimer

### start

```c++
void Widget::on_startButton_clicked()
{
    //开启定时器
    timer->start(TIMEOUT);
}
```

### stop

```c++
void Widget::on_stopButton_clicked()
{
    //关闭定时器
    timer->stop();
}
```

### 示例

1. 首先需声明QTimer

```c++
QTimer *timer;	//声明QTimer对象
```

2. 初始化QTimer对象   

```c++
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    timer = new QTimer;		//初始化 	
}
```

3. 利用connect进行定时器事件处理。

```c++
//使用槽函数实现定时事件处理
connect(timer,&QTimer::timeout,this,&Widget::timeoutSlot);
```

4. 也可单次触发定时器

```c++
void Widget::on_pushButton_clicked()
{
    //定时器只做一次
    QTimer::singleShot(TIMEOUT,this,SLOT(timeoutSlot()));	//同时绑定槽函数
}
```

# 文件操作

## 常用控件

>* menuBar:菜单栏
>* menu：菜单
>* action：动作（没有ui界面的转到槽）

注

1. QMenu对象，那么它本身没有setText()方法，因为QMenu是用来包含多个QAction的容器。如要在QMenu中显示带有文字的菜单项，你需要为QMenu添加QAction，然后为这些QAction设置文字和图标。

## 关联动作的信号与槽

```c++
//绑定新建的信号与槽
connect(ui->newAction,&QAction::triggered,this,&MainWindow::newAcionSlot);
//绑定打开的信号与槽
connect(ui->openAction,&QAction::triggered,this,&MainWindow::openAcionSlot);
//绑定另存为的信号与槽
connect(ui->saveAction,&QAction::triggered,this,&MainWindow::saveAcionSlot);
```

### 打开文件夹

```c++
//实现打开的槽函数
//打开是将文件的内容写到TextEdit中
void MainWindow::openAcionSlot()
{
    //打开打开文件对话框
    //初次在当前文件夹中选择
    //仅打开.cpp类型文件
    QString fileName = QFileDialog::getOpenFileName(this,"选择一个文件",
                                 QCoreApplication::applicationFilePath(),"*.cpp");
    //如果为空则提示
    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,"警告","请选择一个文件");
    }
    else
    {
        //测试是否打开成功
        //qDebug() << fileName;
        //将文件的内容写到TextEdit中
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);     //只读方式打开
        QByteArray ba = file.readAll();     //只适合少量数据
        ui->textEdit->setText(QString(ba));
        file.close();                       //关闭文件
    }
}
```

### 另存为文件夹

```c++
//实现另存为的槽函数
//另存为是读取TextEdit的内容写到文件里面去
void MainWindow::saveAcionSlot()
{
    //打开保存文件对话框
    //初次在当前文件夹中选择
    //打开所有类型文件
    QString fileName = QFileDialog::getSaveFileName(this,"选择一个文件",
                                              QCoreApplication::applicationFilePath());
    //如果为空则提示
    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,"警告","请选择一个文件");
    }
    else
    {
        //TextEdit的内容写到文件里面去
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);     //只写方式打开
        QByteArray ba;
        QString text = ui->textEdit->toPlainText(); //获取textEdit中数据并转为QString
        file.write(text.toUtf8());              //写入
        file.close();                           //关闭文件
    }
}
```

### 新建文件

```c++
//实现新建的槽函数
void MainWindow::newAcionSlot()
{
    ui->textEdit->clear();      //清空编辑框
    this->setWindowTitle("新建文本文档.txt");
}
```

注

1. 获取当前路径

```c++
QCoreApplication::applicationFilePath()
```

2. 全部文件：\*.*
3. (&F)：Art+F快捷键；(&N)：Art+N快捷键；...

## TextEdit

### 末尾追加写入

```c++
void MainWindow::connectLog(QString IP, QString port,QString state)
{
    // 获取当前日期和时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    // 格式化日期时间为字符串，格式为 yyyy-MM-dd hh:mm:ss
    QString dateTimeStr = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    // 创建包含IP和Port信息的字符串
    QString logInfo = dateTimeStr + "- 客端端IP: " + IP + ", 端口号Port: " + port + ", 连接状态State: " + state;

    // 将光标移动到文本编辑器的最后
    QTextCursor cursor = ui->mainTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->mainTextEdit->setTextCursor(cursor);

    // 插入新的日志信息
    ui->mainTextEdit->insertPlainText(logInfo + "\n");

    // 再次将光标移动到文本编辑器的最后，确保它始终保持在最后一行
    cursor = ui->mainTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->mainTextEdit->setTextCursor(cursor);
}
```

### 清空

```c++
void MainWindow::on_pushButton_clicked()
{
    confirm = new Confirm("是否清空在线日志？");
    confirm->show();
    //确定
    connect(confirm,&Confirm::yes,[this](){
        ui->mainTextEdit->clear();	//清空
        delete confirm;
    });
    //取消
    connect(confirm,&Confirm::no,[this](){
        delete confirm;
    });
}
```

# 事件

## Introduction

Qt将系统产生的消息转化为Qt事件，Qt事件被封装为对象，所有Qt事件继承抽象类QEvevt（自动触发），用于描述程序内部或外部发生的动作，任意QObject对象都具备处理Qt事件的能力。

注

1. 消息处理函数是虚函数，需要在头文件中声明重写事件函数。

## Qt事件类型

> * 键盘事件
> * 鼠标事件
> * 拖放时间
> * 滚轮事件
> * 绘屏事件
> * 定时事件
> * 移动时间
> * 大小改变事件
> * 显示和隐藏事件

## 示例

1. 首先进行声明

```c++
//虚函数重载
void keyPressEvent(QKeyEvent *k);
void mousePressEvent(QMouseEvent *m);
```

2. 重写事件，注意事件当触发时自动调用

```c++
//重写键盘事件
void MainWindow::keyPressEvent(QKeyEvent *k)
{
    if(k->modifiers() == Qt::ControlModifier && k->key() == Qt::Key_S)
    {
        saveAcionSlot();
    }
}
```

```c++
//重写鼠标事件
void MainWindow::mousePressEvent(QMouseEvent *m)
{
    //获取鼠标位置
    QPoint pt = m->pos();
    qDebug() << pt;

    //鼠标左右键
    if(m->button() == Qt::LeftButton)
    {
         qDebug() << "左键被按下";
    }
    //鼠标左右键
    if(m->button() == Qt::RightButton)
    {
        qDebug() << "右键被按下";
    }
}
```

# TCP

## Introduction

|  服务器   | 客户端  |
| :-------: | :-----: |
|  socket   | socket  |
|   bing    |         |
|  listen   | connect |
|  accept   |         |
| send/recv | connect |

与网络相关的，需先在工程文件中加 network

```c++
#按需添加
QT       += core gui network
```

## TCP客户端

### 示例

1. .h文件中创建对象

```c++
QTcpSocket *socket;     //声明网络对象
```

2. .cpp文件中进行初始化

```c++
socket = new QTcpSocket;    //初始化Socket对象
```

3. 客户端连接服务器

```c++
void Widget::on_connectButton_clicked()
{
    //获取IP地址和端口号
    QString IP = ui->ipLineEdit->text();
    QString port = ui->portLineEdit->text();
    
  	//断开连接 防止连接重复
    socket->disconnectFromHost();       
    //连接服务器
    socket->connectToHost (QHostAddress(IP),port.toShort());
}
```

4. 连接失败

```c++
//连接服务器失败打印错误信息
connect(socket,&QTcpSocket::errorOccurred,[this](){
    // 显示错误提示
    QMessageBox::warning(this, "连接提示", QString("连接失败：%1").arg(socket->errorString()));
        // 连接失败，停止尝试连接
    socket->abort(); // 不触发disconnected信号，或者使用 socket->disconnectFromHost() 触发disconnected信号
    socket->close();
});
```

5. 连接中断

```c++
//连接断开，socket发出信号
connect(socket,&QTcpSocket::disconnected,[this](){
  QMessageBox::warning(this,"连接提示","连接异常，网络断开");
  delete socket
});
```

6. 连接成功

```c++
//连接服务器成功，socket对象会发出信号
connect(socket,&QTcpSocket::connected,[this](){
    qDebug() << 1  ;
    QMessageBox::warning(this,"连接提示","连接服务器成功");
    //打开子界面
    this->hide();   //原界面隐藏
    //若为局部变量，则执行完即释放，子界面无法显示，故不可为局部变量
    Chat *c = new Chat(socket);     //堆空间创建，同时重写构造函数
    c->show();                      //显示窗口
});
```

## TCP服务器（非多线程）

### 示例

1. .h文件中创建对象

```c++
QTcpServer *server;		 //声明TCP服务器对象
```

2. 在.cpp文件中进行初始化

```c++
server = new QTcpServer;    //初始化server对象	
```

3. 服务器监听以及绑定客户端连接信号与槽函数

```c++
//服务器进行监听
server->listen(QHostAddress::AnyIPv4,PORT);
//客户端发起连接，server发出信号
connect(server,&QTcpServer::newConnection,this,&Widget::newClientHandler);
```

4. 服务器新客户端连接对应的槽函数

```c++
void Widget::newClientHandler()
{
    //建立TCP连接
    QTcpSocket *socket = server->nextPendingConnection();

    //获取客户端ip
    ui->ipLineEdit->setText(socket->peerAddress().toString());
    //获取客户端端口号 OS系统分配
    ui->portLineEdit->setText(QString::number(socket->peerPort()));
}
```

## TCP服务器与客户端互传消息

### 单个数据

1. 发送数据

```c++
void Chat::on_sendButton_clicked()
{
    //socket发送数据
    socket->write(ui->lineEdit->text().toUtf8());
}
```

2. 接收数据

```c++
//服务器收到客户端发送到的信息，socket发出readyread信号
connect(socket,&QTcpSocket::readyRead,this,&Widget::clientInfoSlots);

void Widget::clientInfoSlots()
{
    //获取信号的发出者
    //sender：返回由信号激活的槽中指向发送信号的对象的指针
    //获取void Widget::newClientHandler()函数中局部变量socket
    QTcpSocket *s = (QTcpSocket *)sender();
    ui->lineEdit->setText(QString(s->readAll()));
}
```

### 多个数据

实现服务器端客户端发送不同消息方式

- 消息前缀或命令码

- 消息ID

- 固定的消息结构

- JSON格式

1. 发送数据

```c++
void MainWindow::on_searchPushButton_clicked()
{
    // 发送查询消息
    QString message1 = "CMD1:";
    message1.append("search");
    socket->write(message1.toUtf8());
}

void MainWindow::on_updatePushButton_clicked()
{
    // 发送更新消息
    QString message1 = "CMD2:";
    message1.append("update");
    socket->write(message1.toUtf8());
}
```

2. 接收数据

```c++
//处理客户端发送的信息
connect(socket,&QTcpSocket::readyRead,this,&ClientHandler::recInfo);

void ClientHandler::recInfo()
{
    //获取信号的发出者
    QTcpSocket *s = (QTcpSocket *)sender();
    // 读取所有数据并存储
    QByteArray data = s->readAll();

    if(data.startsWith("CMD1:"))
    {
        //处理接收数据 如发送客户端
        socket->write("CMD1:");
    }
    else if(data.startsWith("CMD2:"))
    {
      //处理接收数据 如发送客户端
      socket->write("CMD2:");
    }
    else
    {
      qDebug() << "Receive error";
    }
}
```

# 窗口嵌套

## 步骤

1. 添加新文件(Qt->设计师设计类)
2. 界面隐藏/删除
3. 不能为局部变量，一定要堆空间创建对象，即类指针
4. （若需传参）修改新增页面.h文件中构造函数并在.cpp文件中重写构造函数


```c++
//连接服务器成功，socket对象会发出信号
connect(socket,&QTcpSocket::connected,[this](){
    QMessageBox::warning(this,"连接提示","连接服务器成功");
    //打开子界面
    this->hide();   //原界面隐藏
    //若为局部变量，则执行完即释放，子界面无法显示，故不可为局部变量
    Chat *c = new Chat(socket);     //堆空间创建，同时重写构造函数
    c->show();                      //显示窗口
});
```

# 多线程服务器

## 步骤

1. 主.cpp文件中进行服务器监听

```c++
// ====服务器监听===== //
server = new MyTcpServer;    //初始化server对象
//服务器进行监听
server->listen(QHostAddress::AnyIPv4,PORT);
```

2. TCPServer中重写*incomingConnection*函数

先声明

```c++
protected:
    void incomingConnection(qintptr socketDescriptor);  //待重写的函数
```

后实现

```c++
//重写incomingConnection函数，当有客户端连接会触发该函数
void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    //启动线程
    QThread *subThread = new QThread;                                   	   //线程对象
    ClientHandler *myClient = new ClientHandler(socketDescriptor,subThread); //事件对象
    //进行关联 将事件移动到线程里
     myClient->moveToThread(subThread);     //myClient会在subThread被删除时自动删除。
    //线程启动
    subThread->start();
    //关联信号与槽函数，调用ClientHandler类working槽函数
    connect(this,&MyTcpServer::signal_to_thread,myClient, &ClientHandler::working);
    connect(myClient,&ClientHandler::connectFinish,[this](){
        //防止重复连接
        disconnect(this,&MyTcpServer::signal_to_thread,0, 0);
    });
    //发送信号
    emit signal_to_thread();
}
```

3. 在事件文件中进行编写处理线程的程序

首先通过socket描述符获取socket以及子线程

```c++
ClientHandler::ClientHandler( int sockfd, QThread *subThread,QObject *parent)
    : QObject{parent}
{
    this->socketDescriptor = sockfd;    //获取socket描述符
    this->subThread  = subThread;				//获取子线程
}
```

再对每个线程进行处理

```c++
//当有客户端连接触发该函数 每个子线程单独处理
void ClientHandler::working()
{
    socket = new QTcpSocket;
    //使用线程描述符创建socket对象
    //qDebug() << socketDescriptor;
    socket->setSocketDescriptor(socketDescriptor);
  
    //...
  
  //客户端断开连接
    connect(socket,&QTcpSocket::disconnected,[this](){
        emit disconnectionDataRequested(IP,port,"DisConnected",this->subThread);    //发送客户端断开信号
        delete socket;	//释放资源  
    });
```

4. TCPServer中完成线程资源释放

```c++
//发送客户端断开连接信息 释放资源，注意不需要disconnect
connect(myClient,&ClientHandler::disconnectionDataRequested,this,&MyTcpServer::sendClientLog_and_delete);

void MyTcpServer::sendClientLog_and_delete(QString IP, QString port,QString state,QThread *sub)
{
    emit deleteConnection(IP,port,state);
    // 线程发出退出请求
    sub->quit();
    // 等待线程真正结束 退出则返回true
    sub->wait();
    //退出后释放线程
    connect(sub,SIGNAL(finished()),sub,SLOT(deleteLater()));	//推荐使用deleteLater
}
```

## 注意

1. socket不能跨线程使用

```c++
ClientHandler *myClient = new ClientHandler(socket);    //事件对象，无法传递socket
```

2. 子线程代码必须放在run()函数里面

```c++
//重定义run函数
//子线程任务代码必须要放在run函数里面，否则无法实现多线程
void myThread::run()
{ 
    //服务器收到客户端发送到的信息，socket发出readyread信号
    connect(socket,&QTcpSocket::readyRead,this,&myThread::clientInfoSlots);
}
```

3. QT的主线程负责刷新界面
4. ui只能在对应的类中进行操作ui

```c++
void myThread::clientInfoSlots(){
    //仅能在当前类操作对应ui
    qDebug() << socket->readAll();
}
```

5. &QTcpSocket::connected无法在服务器端使用
6. sender()：在槽函数里面调用可以获取谁发出的信号

# 数据库

## 步骤

1. .pro加入sql

```c++
QT       += core gui sql
```

2. 声明数据库对象

```c++
QSqlDatabase db;    //声明数据库对象
```

3. 连接数据库

```c++
//连接数据库
db = QSqlDatabase::addDatabase("QMYSQL"); //加载数据库驱动

//连接对应数据库
db.setHostName("localhost"); 
db.setPort(3306);
db.setDatabaseName("mydatabase");
db.setUserName("root");
db.setPassword("root");

//判断是否连接成功
if(db.open())
{
    QMessageBox::information(this,"连接提示","连接成功");
}
else
{
    QMessageBox::warning(this,"连接提示","连接失败");
}
```

4. 插入数据

```c++
void Widget::on_insertPushButton_clicked()
{
    //获取数据
    QString id = ui->idLineEdit->text();
    QString name = ui->nameLineEdit->text();
    QString birth = ui->birthLineEdit->text();

    //MyQSL插入语句
    QString sql = QString("insert into student values (%1, '%2', '%3');").arg(id,name,birth);
    //执行插入并检查
    QSqlQuery query;
    if(query.exec(sql))
    {
        QMessageBox::information(this,"插入提示","插入成功");
    }
    else
    {
        QMessageBox::warning(this,"插入提示","插入失败");
    }
}
```

5. 查询数据

```c++
void Widget::on_findPushButton_clicked()
{
    //执行查询语句
    QSqlQuery query;
    query.exec("select * from student");
    //查询所有
    while(query.next())
    {
        qDebug() << query.value(0);
        qDebug() << query.value(1);
        qDebug() << query.value(2);
    }
}
```

## tableView

专门用于对数据库进行增删改查。

### 步骤

1. 声明对象

```c++
  QSqlTableModel *m;  //声明表单对象
```

2. 初始化

```c++
//判断是否连接成功
if(db.open())
{
    QMessageBox::information(this,"连接提示","连接成功");
    m = new QSqlTableModel;		//初始化
    m->setTable("student");		//选择关联的表
    //置数据库修改后保存的方式，默认应该是OnFieldChange，即编辑完就保存修改至数据库，OnManualSubmit是需要手动提交时
    m->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //刷新任务列表
    ui->mainTableView->setModel(m);
    ui->mainTableView->resizeColumnsToContents();		//自动调整列宽
    ui->mainTableView->resizeRowsToContents();			//自动调整行距
    ui->mainTableView->setAlternatingRowColors(true);	//设置行间交叉颜色
    ui->mainTableView->setColumnHidden(0,true);			//设置某一列不显示 默认第一列不显示
}
else
{
     QMessageBox::warning(this,"连接提示","连接失败");
}
```

3. 查询

```c++
if(data.startsWith("CMD1:"))         //查询
{
    m->select();
}
```

4. 更新

```c++
if(data.startsWith("CMD2:"))   //确认更新
{
    m->database().transaction();
    if(m->submitAll()){
      m->database().commit();
      m->select();		//显示更新后视图
    }else{
      //输出错误信息
      m->database().rollback();
      QString str;
      str += m->lastError().text();
      QMessageBox::warning(this,"error",str);
    }
}
```

5. 删除

```c++
if(data.startsWith("CMD3:"))      //删除
{
    int curRow=ui->mainTableView->currentIndex().row();
    m->removeRow(curRow);
    int ok = QMessageBox::warning(this,"确定？","删否？",QMessageBox::Yes,QMessageBox::No);
    if(ok==QMessageBox::Yes){	//确定删除
        if (!m->submitAll()) {
          //打印错误信息
          m->database().rollback();
          QString str;
          str += m->lastError().text();
          QMessageBox::warning(this,"error",str);
        }
      m->select();		//显示更新后视图
    }else{						//取消删除
      m->revertAll();//回退，注意，只有在保存策略是OnManualSubmit时有用
    }
}
```

6. 新增

```c++
if(data.startsWith("CMD4:"))       //新增
{
    QSqlRecord record = m->record();

    //"id"就是列名，后面跟着的是相应的值
    record.setValue("id", ui->lineEdit->text());
    if (!m->insertRecord(0, record)) {  //每次都从第一列插入
        m->database().rollback();
        QString str;
        str += m->lastError().text();
        QMessageBox::warning(this,"Insert failed:",str);
    } else if (!m->submitAll()) {			//提交
        m->database().rollback();
        QString str;
        str += m->lastError().text();
        QMessageBox::warning(this,"Submit failed:",str);
    } else {
        m->select();		//显示新增后视图
    }
}
```

# 其他

## 添加图片资源

1. 新建Qt Resource file，如：res.qrc
2. 添加前缀
3. 添加文件
4. 复制资源路径 即可

## 发布步骤

1. 项目构建中选择 release
2. 在项目文件夹如中 ‘Desktop_Qt_6_5_3_MinGW_64_bit-Release’ 中打开 ’release‘
3. 拷贝 ‘××.exe’ 文件到一空白文件夹中
4. 打开Qt命令行编辑器
5. 转到目标空白文件夹中，如：cd /d C:\Users\lenovo\OneDrive\桌面\GreenSoftwarePack
6. 输入命令 windeployqt ***.exe ，windeployqt工具将把该exe程序所依赖的库文件拷贝到该文件夹下

注

1. 正常情况下，此时的绿色便携版文件夹已经可以拷贝到其他的机器上正常运行，但也有可能出现其他机器上仍然提示缺少xxx.dll的情况，这时，需要手动将该dll文件拷贝到该文件夹中。
