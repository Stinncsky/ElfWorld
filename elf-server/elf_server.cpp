#include "elf_server.h"
#include "ui_elf_server.h"

elf_server::elf_server(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::elf_server)
{
    ui->setupUi(this);
    server = new QTcpServer(this);
    server->listen(QHostAddress::Any, 6677);
    connect(server, &QTcpServer::newConnection, this, &elf_server::ClientConnect);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("accounts.db");

    if (!db.open()) {
        qDebug() << "Error: Unable to open database";
        return;
    }
    createAccountTable(db);
}

elf_server::~elf_server()
{
    delete ui;
}

void elf_server::ClientConnect(){
    while (server->hasPendingConnections()){
        socket = server->nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this, &elf_server::ReadData);
    }
}

void elf_server::ReadData(){
    QByteArray buf = socket->readAll();
    qDebug() << buf;
    int start = 0, next = 1;
    QString data(buf);
    QStringList parts = data.split('&');
    read_res:
    switch (parts[start][0].unicode()){
        case LOGIN:
            login(parts[next], parts[next+1]);
            next += 2;
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case REGISTE:
            registerUser(parts[next], parts[next+1]);
            next += 2;
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case NEWELF:
            add_user_elf(parts[next],parts[next+1]);
            next += 2;
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case GET_USER_INFO:
            Send_user_info(parts[next]);
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case UPDATE_USER_ELF:
            update_user_elf(parts[next], parts[next+1].toInt(), parts[next+2]);
            next += 3;
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case DELETE_USER_ELF:
            delete_user_elf(parts[next], parts[next+1].toInt());
            break;
    }
}

void elf_server::login(const QString& username, const QString& password) {
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isValid()) {
        qDebug() << "Error: Database connection invalid";
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT * FROM accounts WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec()) {
        qDebug() << "Error: Unable to execute query";
        return;
    }

    if (query.next()) {
        qDebug() << "Login successful!";
        QString data = (QChar)LOGIN + '&' + "1" + '&';
        SendData(data);
    } else {
        qDebug() << "Login failed. Invalid username or password.";
        QString data = (QChar)LOGIN + '&' + "0" + '&';
        SendData(data);
    }
}

void elf_server::registerUser(const QString& username, const QString& password) {
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isValid()) {
        qDebug() << "Error: Database connection invalid";
        return;
    }

    // 检查用户名是否已存在
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT * FROM accounts WHERE username = :username");
    checkQuery.bindValue(":username", username);
    if (!checkQuery.exec()) {
        qDebug() << "Error: Unable to execute query" << checkQuery.lastError().text();
        return;
    }

    if (checkQuery.next()) {
        qDebug() << "Username already exists. Please choose a different username.";
        QString data = (QChar)REGISTE + '&' + "0" + '&';
        SendData(data); // 发送注册失败的消息
        return;
    } else {
        insertAccount(db, username, password);
        QString data = (QChar)REGISTE + '&' + "1" + '&';
        qDebug() << data;
        SendData(data); // 发送注册成功的消息
        create_user_info_table(db, username);
    }
}

void elf_server::createAccountTable(const QSqlDatabase db) {
    QSqlQuery query(db);

    // 检查 "Accounts" 表是否存在
    if (!query.exec("SELECT 1 FROM Accounts LIMIT 1")) {
        // 如果不存在，则创建表
        query.exec("CREATE TABLE Accounts ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "username TEXT NOT NULL,"
                   "password TEXT NOT NULL"
                   ");");
    }
}

void elf_server::insertAccount(const QSqlDatabase db, const QString& username, const QString& password) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO Accounts (username, password) VALUES (?, ?)");
    query.addBindValue(username);
    query.addBindValue(password);
    if (!query.exec()) {
        qDebug() << "Error inserting account:" << query.lastError().text();
        return;
    }
}

void elf_server::SendData(const QString& data){
    //qDebug() << data;
    socket->write(data.toUtf8());
    //qDebug() << data.toUtf8();
}

void elf_server::create_user_info_table(const QSqlDatabase db, const QString& username){
    //QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
    qDebug() << "Error: Database is not open";
    return;
    }

    QString tableName = '_' + username + "_user_info";
    bool tableExists = false;
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=:tableName");
    checkQuery.bindValue(":tableName", tableName);
    if(checkQuery.exec() && checkQuery.next()){
        tableExists = true;
    }
    if(tableExists){
        qDebug() << tableName << " already exists.";
        return;
    }
    QSqlQuery query(db);
    query.prepare("CREATE TABLE " + tableName + " ("
                                                              "elf_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                                              "elf_name TEXT,"
                                                              "elf_level INTEGER,"
                                                              "exp INTEGER,"
                                                              "elf_attr TEXT,"
                                                              "HP REAL,"
                                                              "ATK REAL,"
                                                              "DEF REAL,"
                                                              "ATK_INTERVAL REAL,"
                                                              "CRIT_Rate REAL,"
                                                              "AGI REAL,"
                                                              "up_HP REAL,"
                                                              "up_ATK REAL,"
                                                              "up_DEF REAL,"
                                                              "up_ATK_INTERVAL REAL,"
                                                              "up_CRIT_Rate REAL,"
                                                              "up_AGI REAL,"
                                                              "elf_skills_1 INTEGER,"
                                                              "elf_skills_2 INTEGER,"
                                                              "elf_skills_3 INTEGER,"
                                                              "elf_skills_4 INTEGER"
                                                              ");");

    if (!query.exec()) {
        qDebug() << "Error: Unable to create table" << query.lastError();
    } else {
        qDebug() << "Table created successfully";
    }
}

void elf_server::add_user_elf(const QString& username, const QString& elf_data) {
    QSqlDatabase db = QSqlDatabase::database();
    QStringList parts = elf_data.split(',');
    if (!db.isOpen()) {
    qDebug() << "Error: Database is not open";
    return;
    }
    QSqlQuery query(db);
    QString tableName = '_' + username + "_user_info";
    qDebug() << "Number of data items: " << parts.size();
    query.prepare("INSERT INTO " + tableName + " (elf_name, elf_level, exp, elf_attr, HP, ATK, DEF, ATK_INTERVAL, CRIT_Rate, AGI, up_HP, up_ATK, up_DEF, up_ATK_INTERVAL, up_CRIT_Rate, up_AGI, elf_skills_1, elf_skills_2, elf_skills_3, elf_skills_4) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(parts[0]); // elf_name
    query.addBindValue(parts[1].toInt()); // elf_level
    query.addBindValue(parts[2].toInt()); // exp
    query.addBindValue(parts[3]); // elf_attr
    query.addBindValue(parts[4].toDouble()); // HP
    query.addBindValue(parts[5].toDouble()); // ATK
    query.addBindValue(parts[6].toDouble()); // DEF
    query.addBindValue(parts[7].toDouble()); // ATK_INTERVAL
    query.addBindValue(parts[8].toDouble()); // CRIT_Rate
    query.addBindValue(parts[9].toDouble()); // AGI
    query.addBindValue(parts[10].toDouble()); // up_HP
    query.addBindValue(parts[11].toDouble()); // up_ATK
    query.addBindValue(parts[12].toDouble()); // up_DEF
    query.addBindValue(parts[13].toDouble()); // up_ATK_INTERVAL
    query.addBindValue(parts[14].toDouble()); // up_CRIT_Rate
    query.addBindValue(parts[15].toDouble()); // up_AGI
    query.addBindValue(parts[16].toInt()); // elf_skills_1
    query.addBindValue(parts[17].toInt()); // elf_skills_2
    query.addBindValue(parts[18].toInt()); // elf_skills_3
    query.addBindValue(parts[19].toInt()); // elf_skills_4

    qDebug() << "SQL query: " << query.lastQuery();
    if (!query.exec()) {
        qDebug() << "Error executing query: " << query.lastError().text();
    }
}

void elf_server::update_user_elf(const QString& username, const int user_elf_id, const QString& elf_data){
    QSqlDatabase db = QSqlDatabase::database();
    QStringList parts = elf_data.split(',');
    if (!db.isOpen()) {
        qDebug() << "Error: Database is not open";
        return;
    }
    QSqlQuery query(db);
    QString tableName = '_' + username + "_user_info";
    qDebug() << "Number of data items: " << parts.size();
    query.prepare("UPDATE " + tableName + " SET elf_name = ?, elf_level = ?, exp = ?, elf_attr = ?, HP = ?, ATK = ?, DEF = ?, ATK_INTERVAL = ?, CRIT_Rate = ?, AGI = ?, up_HP = ?, up_ATK = ?, up_DEF = ?, up_ATK_INTERVAL = ?, up_CRIT_Rate = ?, up_AGI = ?, elf_skills_1 = ?, elf_skills_2 = ?, elf_skills_3 = ?, elf_skills_4 = ? WHERE elf_id = ?");
    query.addBindValue(parts[0]); // elf_name
    query.addBindValue(parts[1].toInt()); // elf_level
    query.addBindValue(parts[2].toInt()); // exp
    query.addBindValue(parts[3]); // elf_attr
    query.addBindValue(parts[4].toDouble()); // HP
    query.addBindValue(parts[5].toDouble()); // ATK
    query.addBindValue(parts[6].toDouble()); // DEF
    query.addBindValue(parts[7].toDouble()); // ATK_INTERVAL
    query.addBindValue(parts[8].toDouble()); // CRIT_Rate
    query.addBindValue(parts[9].toDouble()); // AGI
    query.addBindValue(parts[10].toDouble()); // up_HP
    query.addBindValue(parts[11].toDouble()); // up_ATK
    query.addBindValue(parts[12].toDouble()); // up_DEF
    query.addBindValue(parts[13].toDouble()); // up_ATK_INTERVAL
    query.addBindValue(parts[14].toDouble()); // up_CRIT_Rate
    query.addBindValue(parts[15].toDouble()); // up_AGI
    query.addBindValue(parts[16].toInt()); // elf_skills_1
    query.addBindValue(parts[17].toInt()); // elf_skills_2
    query.addBindValue(parts[18].toInt()); // elf_skills_3
    query.addBindValue(parts[19].toInt()); // elf_skills_4
    query.addBindValue(user_elf_id); // elf_id

    qDebug() << "SQL query: " << query.lastQuery();
    if (!query.exec()) {
        qDebug() << "Error update query: " << query.lastError().text();
    }
}

void elf_server::delete_user_elf(const QString& username, const int user_elf_id){
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Error: Database is not open";
        return;
    }

    QSqlQuery query(db);
    QString tableName = '_' + username + "_user_info";
    query.prepare("DELETE FROM " + tableName + " WHERE elf_id = ?");
    query.addBindValue(user_elf_id);

    if (!query.exec()) {
        qDebug() << "Error deleting user elf:" << query.lastError().text();
    } else {
        qDebug() << "User elf deleted successfully!";
    }
}

void elf_server::Send_user_info(const QString& username){
    QSqlDatabase db = QSqlDatabase::database();
    QString tableName = '_' + username + "_user_info";
    QSqlQuery query;
    QString data = (QChar)SEND_USER_INFO + '&';

    // 执行查询
    query.prepare("SELECT * FROM " + tableName);
    if (!query.exec()) {
        qDebug() << "Error: Unable to execute query" << query.lastError();
        return;
    }

    // 构建数据字符串
    while (query.next()) {
        QString elf_name = query.value(1).toString();
        int elf_level = query.value(2).toInt();
        int experience = query.value(3).toInt();
        QString elf_attr = query.value(4).toString();
        double HP = query.value(5).toDouble();
        double ATK = query.value(6).toDouble();
        double DEF = query.value(7).toDouble();
        double ATK_INTERVAL = query.value(8).toDouble();
        double CRIT_Rate = query.value(9).toDouble();
        double AGI = query.value(10).toDouble();
        double up_HP = query.value(11).toDouble();
        double up_ATK = query.value(12).toDouble();
        double up_DEF = query.value(13).toDouble();
        double up_ATK_INTERVAL = query.value(14).toDouble();
        double up_CRIT_Rate = query.value(15).toDouble();
        double up_AGI = query.value(16).toDouble();
        int elf_skills_1 = query.value(17).toInt();
        int elf_skills_2 = query.value(18).toInt();
        int elf_skills_3 = query.value(19).toInt();
        int elf_skills_4 = query.value(20).toInt();
        int elf_id = query.value(0).toInt();
        data += elf_name + "," + QString::number(elf_level) + "," + QString::number(experience) + "," +
                elf_attr + "," +
                QString::number(HP) + "," + QString::number(ATK) + "," + QString::number(DEF) + "," +
                QString::number(ATK_INTERVAL) + "," + QString::number(CRIT_Rate) + "," + QString::number(AGI) + "," +
                QString::number(up_HP) + "," + QString::number(up_ATK) + "," + QString::number(up_DEF) + "," +
                QString::number(up_ATK_INTERVAL) + "," + QString::number(up_CRIT_Rate) + "," + QString::number(up_AGI) + "," +
                QString::number(elf_skills_1) + "," + QString::number(elf_skills_2) + "," + QString::number(elf_skills_3) + "," + QString::number(elf_skills_4) + "," + QString::number(elf_id) + "&";
    }
    SendData(data);
}
