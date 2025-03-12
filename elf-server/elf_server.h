#ifndef ELF_SERVER_H
#define ELF_SERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtSql>

enum event{
    LOGIN, // 登录事件
    REGISTE, // 注册事件
    UPDATE_CLINET_USER, // 更新客户端用户事件
    NEWELF, // 新精灵事件
    GET_USER_INFO, // 获取用户信息事件
    SEND_USER_INFO, // 发送用户信息事件
    UPDATE_USER_INFO, // 更新用户信息事件
    UPDATE_USER_ELF, // 更新用户精灵事件
    DELETE_USER_ELF, // 删除用户精灵事件
    ONLINE_USER_TABLE, // 在线用户表事件
    GET_OTHER_USER_INFO, // 获取其他用户信息事件
    SEND_OTHER_USER_INFO, // 发送其他用户信息事件
    NO_EVENT // 无事件
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
    Ui::elf_server *ui; // 用户界面对象
    QMap<QTcpSocket*, QString> clients; // 客户端列表，键是socket，值是用户名
    QTcpServer *server; // 服务器对象
    QString IP; // 服务器IP地址
    int port; // 服务器端口号

    void ReadData(); // 读取数据的函数
    void ClientConnect(); // 处理客户端连接的函数
    void ClientDisconnected(); // 处理客户端断开连接的函数
    void SendData(QTcpSocket * const,const QString&); // 发送数据的函数

    void Send_user_info(QTcpSocket * const, const QString&); // 发送用户信息的函数

    void update_user_info(const QString&, const QString&); // 更新用户信息的函数
    void add_user_elf(const QString&, const QString&); // 添加用户精灵的函数
    void update_user_elf(const QString&, const int, const QString&); // 更新用户精灵的函数
    void delete_user_elf(const QString&, const int); // 删除用户精灵的函数

    void update_client_user(QTcpSocket * const); // 更新客户端用户的函数
    void Send_online_user_table(); // 发送在线用户表的函数
    void Send_other_user_info(QTcpSocket * const, const QString&); // 发送其他用户信息的函数

    void login(QTcpSocket * const, const QString& username, const QString& password); // 登录的函数
    void registerUser(QTcpSocket * const, const QString& username, const QString& password); // 注册用户的函数
    void createAccountTable(const QSqlDatabase); // 创建账户表的函数
    void create_user_info_table(const QSqlDatabase db,const QString&); // 创建用户信息表的函数
    void insertAccount(const QSqlDatabase db, const QString& username, const QString& password); // 插入账户的函数
};
#endif // ELF_SERVER_H
