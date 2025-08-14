#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtCharts>
#include<QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

#define USART_REC_LEN 100   // 定义最大接收字节数 100

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    void on_checkBox_main_stateChanged(int arg1);
    void on_checkBox_light_stateChanged(int arg1);
    void on_checkBox_window_stateChanged(int arg1);
    void on_checkBox_alarm_stateChanged(int arg1);

    void serialReceiveInfo();
    void UART_SendData(uint8_t deviceId);

private:
    Ui::Widget *ui;
    QChart *chart;
    QSerialPort *serialPort;    //串口类
    QLineSeries *line;
    QValueAxis *axis_y;
    QDateTimeAxis *axis_x;

    void chart_init();

    void startSerial(void);
    void stopSerial(void);
    void show_music(uint8_t data);
    uint8_t calculate_crc(uint8_t *data, uint16_t length);
};
#endif // WIDGET_H
