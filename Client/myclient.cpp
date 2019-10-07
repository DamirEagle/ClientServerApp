#include "myclient.h"
#include "ui_myclient.h"

MyClient::MyClient(QWidget *parent) :
    QMainWindow(parent),
    m_nNextBlockSize(0),
    ui(new Ui::MyClient)
{
    ui->setupUi(this);

    m_pTcpSocket = new QTcpSocket(this);

    m_pTcpSocket->connectToHost(aPort, nPort);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError)));
}

void MyClient::slotReadyRead()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_4_2);
    QVector<QVector<int> > resVec;
    int resMiddle;

    for (;;) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }

        in >> resVec >> resMiddle;

        ui->tableResult->setColumnCount(resVec[0].length());
        ui->tableResult->setRowCount(resVec.length());

        for (int i = 0; i < resVec.length(); i++) {
            for (int j = 0; j < resVec[i].length(); j++) {
                ui->tableResult->setItem(i, j, new QTableWidgetItem(QString::number(resVec[i][j])));
            }
        }

        ui->labelResult->setText(QString::number(resMiddle));

        m_nNextBlockSize = 0;
    }
}

void MyClient::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                     "The host was not found." :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     "The remote host is closed." :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     "The connection was refused." :
                     QString(m_pTcpSocket->errorString())
                    );
    qDebug() << strError;
}

void MyClient::slotSendToServer()
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << QTime::currentTime() << "m_ptxtInput->text()";

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
}

void MyClient::slotConnected()
{
    qDebug() << "Received the connected() signal";
}

MyClient::~MyClient()
{
    delete ui;
}

void MyClient::on_resizeTableBut_clicked()
{
    ui->tableMatrix->setColumnCount(ui->spinBox_width->value());
    ui->tableMatrix->setRowCount(ui->spinBox_height->value());
}

void MyClient::on_sendBut_clicked()
{
    QVector<QVector<int> > cells;
    for (int i = 0; i < ui->tableMatrix->rowCount(); i++) {
        QVector<int> curRow;
        for (int j = 0; j < ui->tableMatrix->columnCount(); j++) {
            curRow.push_back(ui->tableMatrix->item(i, j)->text().toInt());
        }
        cells.push_back(curRow);
    }

    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << cells;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
}
