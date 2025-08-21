#include "widget.h"
#include "ui_widget.h"
#include <QTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //设置窗口名称
    this->setWindowTitle("David录音室");

    //设置应用程序图标
    setWindowIcon(QIcon(":/new/prefix1/res/icon.svg"));

    chart_init();

}

Widget::~Widget()
{
    delete ui;
}

uint8_t USART_RX_BUF[USART_REC_LEN];

void Widget::chart_init(){
    //第一步设置标题和坐标轴
    chart = new QChart;
    ui->chartView->setChart(chart);
    chart->legend()->setVisible(false);              //图例不显示
    chart->setContentsMargins(0, 0, 0, 0);  //设置外边界全部为0

    //设置x轴
    axis_x = new QDateTimeAxis();
    axis_x->setFormat("HH:mm:ss");
    axis_x->setTitleText("时间");
    axis_x->setTickCount(10);
    chart->addAxis(axis_x, Qt::AlignBottom);

    //设置y轴
    axis_y =new QValueAxis;
    axis_y->setTitleText("分贝(dB)");
    axis_y->setRange(0,160);//设置值域
    chart->addAxis(axis_y,Qt::AlignLeft);//沿底边显示

    //第二步增加数据
    line =new QLineSeries;
    chart->addSeries(line);
    line->attachAxis(axis_x);//绑定x轴
    line->attachAxis(axis_y);//绑定y轴
}


void Widget::on_checkBox_main_stateChanged(int arg1)
{
    //2表开启 0表示关闭
    if(arg1 == 2)
    {
        startSerial();
    }
    else if(arg1 == 0)
    {
        stopSerial();
    }
}


void Widget::on_checkBox_light_stateChanged(int arg1)
{
    UART_SendData(0x20);
    //2表示开启 0表示关闭
}


void Widget::on_checkBox_window_stateChanged(int arg1)
{
    UART_SendData(0x40);
    //2表示开启 0表示关闭
}


void Widget::on_checkBox_alarm_stateChanged(int arg1)
{
    UART_SendData(0x80);
    //2表示开启 0表示关闭
}

void Widget::startSerial(void)
{
    serialPort = new QSerialPort();
    if(serialPort->isOpen())                    //如果串口已经打开了 先给他关闭了

    {
        serialPort->clear();
        serialPort->close();
    }

    //设置串口名字 假设我们上面已经成功获取到了 并且使用第一个
    serialPort->setPortName(ui->comboBox_serialPoat->currentText());

    // 打开串口
    if (serialPort->open(QIODevice::ReadWrite)) {
        QMessageBox::information(this, "串口打开成功", "串口成功打开");
    } else {
        QMessageBox::critical(this, "串口打开失败", "无法打开串口");
    }

    // 获取当前选择的串口参数
    int baudRate = ui->comboBox_serialBaudrate->currentText().toInt();
    QSerialPort::DataBits dataBits = QSerialPort::DataBits(ui->comboBox_serialData->currentText().toInt());
    QSerialPort::Parity parity = QSerialPort::Parity(ui->comboBox_serialCheck->currentText().toInt());
    QSerialPort::StopBits stopBits = QSerialPort::StopBits(ui->comboBox_serialPause->currentText().toInt());

    // 设置串口参数
    serialPort->setBaudRate(baudRate, QSerialPort::AllDirections);
    serialPort->setDataBits(dataBits);                          //数据位
    serialPort->setParity(parity);                              //校验位
    serialPort->setStopBits(stopBits);                          //停止位
    serialPort->setFlowControl(QSerialPort::NoFlowControl);     //无流控制

    //连接信号槽 当下位机发送数据QSerialPortInfo 会发送 readyRead 信号,我们定义个槽void receiveInfo()解析数据
    connect(serialPort,SIGNAL(readyRead()),this,SLOT(serialReceiveInfo()));

    // qDebug() << serialPort->portName();
    // qDebug() << serialPort->dataBits();
    // qDebug() << serialPort->parity();
    // qDebug() << serialPort->stopBits();
    // qDebug() << serialPort->baudRate();
}

void Widget::stopSerial(void)
{
    if (serialPort->isOpen())
    {
        serialPort->close();
    }
    delete serialPort;
    QMessageBox::information(this, "串口关闭成功", "串口成功关闭");
}

/* Data数据标识符对应表 */
// uint8_t music -> 0x1								 	// 分贝
// uint8_t temperature -> 0x2					 	// 温度
// uint8_t humidity -> 0x4						 		// 湿度
// uint8_t smoke -> 0x8								 	// 烟雾浓度
// uint8_t alcohol -> 0x10						 		// 酒精浓度
// uint8_t led_white_mode -> 0x20 		 		// 照明灯工作模式
// uint8_t servo_mode -> 0x40			 		 	// 舵机工作模式
// uint8_t buzzer_mode -> 0x80		 		 	// 蜂鸣器工作模式
void Widget::serialReceiveInfo(void)
{
    static uint8_t RxState = 0;	// 定义表示当前状态机状态的静态变量
    static uint8_t dataNum = 0;	// 定义表示当前接收数据个数的静态变量
    static uint8_t dataId = 0;		// 定义表示当前接收数据标识符的静态变量
    static uint8_t RxLocate = 0; // 定义表示当前接收数据标识符的静态变量

    QByteArray buffer = serialPort->readAll(); //获取数据
    uint8_t *uint8Array = reinterpret_cast<uint8_t *>(buffer.data());
    // qDebug() << buffer.toHex().toUpper();

    for (int i = 0; i < buffer.size(); i++) {
        // 接受来自下位机数据
        /*使用状态机的思路，依次处理数据包的不同部分*/
        /*当前状态为0，接收数据包包头*/
        if (RxState == 0)
        {
            if (uint8Array[i] == 0xFF) // 如果数据确实是包头，并且上一个数据包已处理完毕
            {
                USART_RX_BUF[RxLocate] = uint8Array[i];
                RxLocate++;
                RxState = 1; // 置下一个状态
            }
            else if (uint8Array[i] == 0x01) // 如果数据确实是包头，并且上一个数据包已处理完毕
            {
                QMessageBox::information(this, "设备控制", "设备状态已改变");
            }
        }
        /*当前状态为1，接收数据包数据个数*/
        else if (RxState == 1)
        {
            USART_RX_BUF[RxLocate] = uint8Array[i];
            RxLocate++;
            dataNum = uint8Array[i]; // 记录待接收的数据个数
            RxState = 2;
        }
        /* 当前状态为2，获取数据标识符 */
        else if (RxState == 2)
        {
            USART_RX_BUF[RxLocate] = uint8Array[i];
            RxLocate++;
            dataId = uint8Array[i]; // 数据标识符
            RxState = 3;
        }
        /* 当前状态为3，根据数据标识符读取数据 */
        else if (RxState == 3)
        {
            USART_RX_BUF[RxLocate] = uint8Array[i];
            RxLocate++;
            QString str = QString::number(uint8Array[i]);          // 将 uint8_t 数据转换为十进制字符串
            switch (dataId)
            {
            case 0x1:
                show_music(uint8Array[i]);
                // qDebug() << str;
                // qDebug()<< QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
                break;
            case 0x02:
                ui->lineEdit_temp->setText(str);
                break;
            case 0x04:
                ui->lineEdit_humi->setText(str);
                break;
            case 0x08:
                 ui->lineEdit_smoke->setText(str);
                break;
            case 0x10:
                 ui->lineEdit_alcohol->setText(str);
                break;
            default:
                break;
            }
            dataNum--;
            if (dataNum == 0)
            {
                RxState = 4;
            }
            else
            {
                RxState = 2;
            }
        }
        /* 当前状态为4，获取应答位 */
        else if (RxState == 4)
        {
            if (uint8Array[i] == 0x0)
            {
                USART_RX_BUF[RxLocate] = uint8Array[i];
                RxLocate++;
                RxState = 5;
            }
        }
        /* 当前状态为5，CRC校验 */
        else if (RxState == 5)
        {
            USART_RX_BUF[RxLocate] = uint8Array[i];
            RxLocate++;
            uint8_t crc = calculate_crc(USART_RX_BUF, RxLocate - 1); // 计算CRC
            if (USART_RX_BUF[RxLocate-1] == crc)
            {
                RxState = 6;
            }
            else{									// 出错丢弃重发
                RxState = 0;
                RxLocate = 0;
                dataId = 0;
            }
        }
        else if (uint8Array[i] == 0x55)
        {
            RxState = 0;
            RxLocate = 0;
            dataId = 0;
        }
    }
}

void Widget::UART_SendData(uint8_t deviceId)
{
    uint8_t num[7] = {0xFF,0x1,deviceId,0x1,0xaa,0x0,0x55};
    num[5] = calculate_crc(num,5);	//CRC数据位
    // 将 uint8_t 数组转换为 QByteArray
    QByteArray byteArray(reinterpret_cast<const char*>(num), sizeof(num));
    //发送数据
    serialPort->write(byteArray);
}

// 计算CRC
uint8_t Widget::calculate_crc(uint8_t *data, uint16_t length)
{
    uint8_t crc = 0xFF;				 // 初始值
    uint8_t polynomial = 0x07; // CRC-8 多项式

    for (uint16_t i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ polynomial;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}

// 计算CRC
void Widget::show_music(uint8_t data)
{
    // 获取当前时间
    QDateTime currentTime = QDateTime::currentDateTime();
    // 纵轴值
    qreal value = data;
    // 添加新数据点
    line->append(currentTime.toMSecsSinceEpoch(), value);
    // 动态更新X轴范围
    axis_x->setRange(currentTime.addSecs(-60), currentTime);
}



