#ifndef MYSERVER_H
#define MYSERVER_H

#define nPort 1337

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QTime>

namespace Ui {
class MyServer;
}

class MyServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyServer(QWidget *parent = 0);
    ~MyServer();

private:
    Ui::MyServer *ui;

    QTcpServer* m_ptcpServer;
    quint16     m_nNextBlockSize;

    void sendToClient(QTcpSocket* pSocket, QVector<QVector<int> > outVec, int maxInd);

public slots:
    void slotNewConnection();
    void slotReadClient();
};

#endif // MYSERVER_H
