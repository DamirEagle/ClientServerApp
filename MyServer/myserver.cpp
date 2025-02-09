#include "myserver.h"
#include "ui_myserver.h"

MyServer::MyServer(QWidget *parent) :
    QMainWindow(parent),
    m_nNextBlockSize(0),
    ui(new Ui::MyServer)
{
    ui->setupUi(this);

    m_ptcpServer = new QTcpServer(this);
        if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
            QMessageBox::critical(0,
                                  "Server Error",
                                  "Unable to start the server:"
                                  + m_ptcpServer->errorString()
                                 );
            m_ptcpServer->close();
            return;
        }
        connect(m_ptcpServer, SIGNAL(newConnection()),
                this,         SLOT(slotNewConnection())
               );
}

void MyServer::slotNewConnection()
{
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
           );
    connect(pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClient())
           );

//    sendToClient(pClientSocket, "Server Response: Connected!");
}

void MyServer::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_4_2);

    QVector<QVector<int> > inVec;

    for (;;) {
        if (!m_nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        in >> inVec;

        int maxInd = 0;
        int max = 0;

        for (int j = 0; j < inVec[0].length(); j++) {
            int counter = 0;
            int maxIndex = 0;
            for (int i = 0; i < inVec.length(); i++) {
                if (inVec[i][j] == 0) {
                    counter++;
                    maxIndex = j;
                }
            }
            if(max < counter)
            {
                max = counter;
                maxInd = maxIndex;
            }
        }/*
        QVector<QVector <int>>::iterator it;
        for(it = inVec.begin(); it < inVec.end(); it++)
        for (int i = 0; i < inVec.length(); i++) {
             inVec[i][maxInd] = 0;
        }
        for (int j = maxInd; j < inVec[0].length(); j++) {
            for (int i = 0; i < inVec.length(); i++) {
                if ((j+1) != inVec[0].length())
                inVec[i][j] = inVec[i][j+1];
            }
        }*/
        if (!inVec.empty())
        { int n;
          for (QVector<QVector <int>>::iterator it_col=inVec.begin(); it_col!=inVec.end(); it_col++)
          {
            n=0;
            for (QVector <int>::iterator it_row=(*it_col).begin(); it_row!=(*it_col).end(); it_row++,n++)
              if (n==maxInd)
              {
                  (*it_col).erase(it_row); break;
              }
          }
        }// end  if (!m.empty())
        else
          qDebug() << "empty"<<endl;
        qDebug() << inVec.length();

        m_nNextBlockSize = 0;

        sendToClient(pClientSocket, inVec, maxInd);
    }
}

void MyServer::sendToClient(QTcpSocket* pSocket, QVector<QVector<int> > outVec, int maxInd)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << outVec << maxInd;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}

MyServer::~MyServer()
{
    delete ui;
}
