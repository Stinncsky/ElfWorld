#ifndef ELF_SERVER_H
#define ELF_SERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtSql>

enum event{
    LOGIN,
    REGISTE,
    NEWELF,
    GET_USER_INFO,
    SEND_USER_INFO,
    UPDATE_USER_ELF,
    DELETE_USER_ELF,
    NO_EVENT
};


QT_BEGIN_NAMESPACE
namespace Ui {
class elf_server;
}
QT_END_NAMESPACE

class elf_server : public QMainWindow
{
    Q_OBJECT

public:
    elf_server(QWidget *parent = nullptr);
    ~elf_server();

private:
    Ui::elf_server *ui;
    QTcpSocket *socket;
    QTcpServer *server;
    QString IP;
    int port;

    void ReadData();
    void ClientConnect();
    void SendData(const QString&);

    void Send_user_info(const QString&);

    void add_user_elf(const QString&, const QString&);
    void update_user_elf(const QString&, const int, const QString&);
    void delete_user_elf(const QString&, const int);

    void login(const QString& username, const QString& password);
    void registerUser(const QString& username, const QString& password);
    void createAccountTable(const QSqlDatabase);
    void create_user_info_table(const QSqlDatabase db,const QString&);
    void insertAccount(const QSqlDatabase db, const QString& username, const QString& password);
};
#endif // ELF_SERVER_H
