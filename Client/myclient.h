#ifndef MYCLIENT_H
#define MYCLIENT_H

#define nPort 1337
#define aPort "localhost"

#include <QMainWindow>
#include <QTcpSocket>
#include <QTime>

namespace Ui {
class MyClient;
}

class MyClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyClient(QWidget *parent = 0);
    ~MyClient();

private:
    Ui::MyClient *ui;

    QTcpSocket* m_pTcpSocket;
    quint16     m_nNextBlockSize;

private slots:
    void slotReadyRead   (                            );
    void slotError       (QAbstractSocket::SocketError);
    void slotSendToServer(                            );
    void slotConnected   (                            );
    void on_resizeTableBut_clicked();
    void on_sendBut_clicked();
};

#endif // MYCLIENT_H
