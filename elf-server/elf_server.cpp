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

// 客户端连接处理函数
void elf_server::ClientConnect(){
    // 当服务器有待处理的连接时，进入循环
    while (server->hasPendingConnections()){
        // 获取下一个待处理的连接，并创建一个对应的socket
        QTcpSocket *clientSocket = server->nextPendingConnection();
        // 将新的客户端socket添加到客户端列表中
        clients[clientSocket] = "";
        // 当socket有数据可读时，调用ReadData函数
        connect(clientSocket, &QTcpSocket::readyRead, this, &elf_server::ReadData);
        // 当socket断开连接时，调用ClientDisconnected函数
        connect(clientSocket, &QTcpSocket::disconnected, this, &elf_server::ClientDisconnected);
        // 在控制台打印新客户端连接的信息
        qDebug() << "New client connected from" << clientSocket->peerAddress().toString();
    }
}

void elf_server::update_client_user(QTcpSocket *const socket) {
    clients[socket] = "";
    Send_online_user_table();
}

void elf_server::ClientDisconnected() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        clients.remove(clientSocket);
        clientSocket->deleteLater();
        Send_online_user_table();
        qDebug() << "Client disconnected from" << clientSocket->peerAddress().toString();
    }
}

// 读取数据的函数
void elf_server::ReadData(){
    // 获取发送信号的对象，并将其转换为QTcpSocket类型
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    // 如果转换失败，则返回
    if (!clientSocket) return;

    // 读取所有数据
    QByteArray buf = clientSocket->readAll();
    // 在控制台打印读取的数据
    qDebug() << buf;
    // 初始化开始和下一个位置的索引
    int start = 0, next = 1;
    // 将读取的数据转换为QString类型
    QString data(buf);
    // 将数据按'&'分割
    QStringList parts = data.split('&');
    // 标签，用于跳转
    read_res:
    // 根据开始位置的字符的Unicode值，判断要执行的操作
    switch (parts[start][0].unicode()){
        case LOGIN: // 登录操作
            login(clientSocket, parts[next], parts[next+1]);
            next += 2;
            // 如果下一个位置的字符的Unicode值小于NO_EVENT，则跳转到read_res
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case REGISTE: // 注册操作
            registerUser(clientSocket, parts[next], parts[next+1]);
            next += 2;
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case UPDATE_CLINET_USER: // 更新客户端用户操作
            update_client_user(clientSocket);
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case NEWELF: // 添加新精灵操作
            add_user_elf(parts[next],parts[next+1]);
            next += 2;
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case GET_USER_INFO: // 获取用户信息操作
            Send_user_info(clientSocket,parts[next]);
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case UPDATE_USER_INFO: // 更新用户信息操作
            update_user_info(parts[next], parts[next+1]);
            next += 2;
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case UPDATE_USER_ELF: // 更新用户精灵操作
            update_user_elf(parts[next], parts[next+1].toInt(), parts[next+2]);
            next += 3;
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case DELETE_USER_ELF: // 删除用户精灵操作
            delete_user_elf(parts[next], parts[next+1].toInt());
            next += 2;
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case GET_OTHER_USER_INFO: // 获取其他用户信息操作
            Send_other_user_info(clientSocket, parts[next]);
            next++;
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
    }
}

void elf_server::login(QTcpSocket *const socket, const QString& username, const QString& password) {
    QList<QString> clientUsernames = clients.values();
    QSet<QString> onlineUsers(clientUsernames.begin(), clientUsernames.end());
    if (onlineUsers.contains(username)){
        QString data = (QChar)LOGIN + '&' + "2" + '&';
        SendData(socket, data);
        return;
    }
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
        clients[socket] = username;
        QString data = (QChar)LOGIN + '&' + "1" + '&';
        SendData(socket,data);
        Send_online_user_table();
    } else {
        qDebug() << "Login failed. Invalid username or password.";
        QString data = (QChar)LOGIN + '&' + "0" + '&';
        SendData(socket,data);
    }
}

void elf_server::registerUser(QTcpSocket *const socket, const QString& username, const QString& password) {
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
        SendData(socket,data); // 发送注册失败的消息
        return;
    } else {
        clients[socket] = username;
        insertAccount(db, username, password);
        QString data = (QChar)REGISTE + '&' + "1" + '&';
        qDebug() << data;
        SendData(socket,data); // 发送注册成功的消息
        //Send_online_user_table();
        create_user_info_table(db, username);
    }
}

// 创建 "Accounts" 数据库表的函数，参数是数据库连接对象
void elf_server::createAccountTable(const QSqlDatabase db) {
    QSqlQuery query(db); // 创建一个查询对象

    // 检查 "Accounts" 表是否存在
    if (!query.exec("SELECT 1 FROM Accounts LIMIT 1")) {
        // 如果不存在，则创建表
        query.exec("CREATE TABLE Accounts ("
                   "user_id INTEGER PRIMARY KEY AUTOINCREMENT," // 用户ID，主键，自增
                   "username TEXT NOT NULL," // 用户名，不能为空
                   "password TEXT NOT NULL," // 密码，不能为空
                   "total_wins INTEGER DEFAULT 0," // 总胜场数，默认为0
                   "total_battles INTEGER DEFAULT 0," // 总战斗场数，默认为0
                   "exp_wins INTEGER DEFAULT 0," // 经验胜场数，默认为0
                   "exp_battles INTEGER DEFAULT 0," // 经验战斗场数，默认为0
                   "battle_wins INTEGER DEFAULT 0," // 战斗胜场数，默认为0
                   "battle_battles INTEGER DEFAULT 0," // 战斗战斗场数，默认为0
                   "total_elf INTEGER DEFAULT 0," // 总精灵数量，默认为0
                   "metal_elf INTEGER DEFAULT 0," // 金属精灵数量，默认为0
                   "wood_elf INTEGER DEFAULT 0," // 木精灵数量，默认为0
                   "water_elf INTEGER DEFAULT 0," // 水精灵数量，默认为0
                   "fire_elf INTEGER DEFAULT 0," // 火精灵数量，默认为0
                   "soil_elf INTEGER DEFAULT 0," // 土精灵数量，默认为0
                   "electric_elf INTEGER DEFAULT 0," // 电精灵数量，默认为0
                   "null_elf INTEGER DEFAULT 0," // 空精灵数量，默认为0
                   "max_level_elf INTEGER DEFAULT 0," // 最高等级精灵数量，默认为0
                   "user_elf_id INTEGER DEFAULT 0" // 用户精灵ID，默认为0
                   ");");
    }
}

void elf_server::update_user_info(const QString& username, const QString& data) {
    // 将数据字符串拆分为各个字段
    QStringList parts = data.split(',');

    // 检查字段数量是否正确
    if (parts.size() != 16) { // 确保有16个字段
        qDebug() << "Error: Incorrect number of data fields.";
        return;
    }

    // 将字段转换为相应的数据类型
    int total_wins = parts[0].toInt();
    int total_battles = parts[1].toInt();
    int exp_wins = parts[2].toInt();
    int exp_battles = parts[3].toInt();
    int battle_wins = parts[4].toInt();
    int battle_battles = parts[5].toInt();
    int total_elf = parts[6].toInt();
    int metal_elf = parts[7].toInt();
    int wood_elf = parts[8].toInt();
    int water_elf = parts[9].toInt();
    int fire_elf = parts[10].toInt();
    int soil_elf = parts[11].toInt();
    int electric_elf = parts[12].toInt();
    int null_elf = parts[13].toInt();
    int max_level_elf = parts[14].toInt();
    int user_elf_id = parts[15].toInt();

    // 获取数据库连接
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isOpen()) {
        qDebug() << "Error: Database is not open";
        return;
    }

    // 更新数据库中的用户信息
    QSqlQuery query(db);
    query.prepare("UPDATE Accounts SET "
                  "total_wins = ?, "
                  "total_battles = ?, "
                  "exp_wins = ?, "
                  "exp_battles = ?, "
                  "battle_wins = ?, "
                  "battle_battles = ?, "
                  "total_elf = ?, "
                  "metal_elf = ?, "
                  "wood_elf = ?, "
                  "water_elf = ?, "
                  "fire_elf = ?, "
                  "soil_elf = ?, "
                  "electric_elf = ?, "
                  "null_elf = ?, "
                  "max_level_elf = ?, "
                  "user_elf_id = ? "
                  "WHERE username = ?");
    query.addBindValue(total_wins);
    query.addBindValue(total_battles);
    query.addBindValue(exp_wins);
    query.addBindValue(exp_battles);
    query.addBindValue(battle_wins);
    query.addBindValue(battle_battles);
    query.addBindValue(total_elf);
    query.addBindValue(metal_elf);
    query.addBindValue(wood_elf);
    query.addBindValue(water_elf);
    query.addBindValue(fire_elf);
    query.addBindValue(soil_elf);
    query.addBindValue(electric_elf);
    query.addBindValue(null_elf);
    query.addBindValue(max_level_elf);
    query.addBindValue(user_elf_id);
    query.addBindValue(username);

    // 执行查询
    if (!query.exec()) {
        qDebug() << "Error updating user info:" << query.lastError().text();
    } else {
        qDebug() << "User info updated successfully!";
    }
}

void elf_server::insertAccount(const QSqlDatabase db, const QString& username, const QString& password) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO Accounts (username, password, "
                  "total_wins, total_battles, exp_wins, exp_battles, battle_wins, battle_battles, "
                  "total_elf, metal_elf, wood_elf, water_elf, fire_elf, soil_elf, electric_elf, null_elf, max_level_elf, user_elf_id) "
                  "VALUES (?, ?, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)");
    query.addBindValue(username);
    query.addBindValue(password);
    if (!query.exec()) {
        qDebug() << "Error inserting account:" << query.lastError().text();
        return;
    }
    Send_online_user_table();
}

void elf_server::SendData(QTcpSocket *const socket,const QString& data){
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
                                                              "elf_skills_4 INTEGER,"
                                                              "elf_type INTERGET"
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
    query.prepare("INSERT INTO " + tableName + " (elf_name, elf_level, exp, elf_attr, HP, ATK, DEF, ATK_INTERVAL, CRIT_Rate, AGI, up_HP, up_ATK, up_DEF, up_ATK_INTERVAL, up_CRIT_Rate, up_AGI, elf_skills_1, elf_skills_2, elf_skills_3, elf_skills_4, elf_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
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
    query.addBindValue(parts[20].toInt()); // elf_type
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
    query.prepare("UPDATE " + tableName + " SET elf_name = ?, elf_level = ?, exp = ?, elf_attr = ?, HP = ?, ATK = ?, DEF = ?, ATK_INTERVAL = ?, CRIT_Rate = ?, AGI = ?, up_HP = ?, up_ATK = ?, up_DEF = ?, up_ATK_INTERVAL = ?, up_CRIT_Rate = ?, up_AGI = ?, elf_skills_1 = ?, elf_skills_2 = ?, elf_skills_3 = ?, elf_skills_4 = ?, elf_type = ? WHERE elf_id = ?");
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
    query.addBindValue(parts[20].toInt()); // elf_type
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

void elf_server::Send_user_info(QTcpSocket *const socket, const QString& username){
    QSqlDatabase db = QSqlDatabase::database();
    // 查询用户信息
    QString data = (QChar)SEND_USER_INFO + '&';
    QSqlQuery query;
    query.prepare("SELECT * FROM Accounts WHERE username = ?");
    query.bindValue(0, username);

    if (!query.exec()) {
        // 查询出错，处理错误
        qDebug() << "Error executing query:" << query.lastError().text();
        return;
    }

    // 如果找到了用户信息，发送回客户端
    if (query.next()) {
        data += QString::number(query.value("total_wins").toInt()) + "," +
                           QString::number(query.value("total_battles").toInt()) + "," +
                           QString::number(query.value("exp_wins").toInt()) + "," +
                           QString::number(query.value("exp_battles").toInt()) + "," +
                           QString::number(query.value("battle_wins").toInt()) + "," +
                           QString::number(query.value("battle_battles").toInt()) + "," +
                           QString::number(query.value("total_elf").toInt()) + "," +
                           QString::number(query.value("metal_elf").toInt()) + "," +
                           QString::number(query.value("wood_elf").toInt()) + "," +
                           QString::number(query.value("water_elf").toInt()) + "," +
                           QString::number(query.value("fire_elf").toInt()) + "," +
                           QString::number(query.value("soil_elf").toInt()) + "," +
                           QString::number(query.value("electric_elf").toInt()) + "," +
                           QString::number(query.value("null_elf").toInt()) + "," +
                           QString::number(query.value("max_level_elf").toInt()) + "," +
                           QString::number(query.value("user_elf_id").toInt()) + '&';
    }
    QString tableName = '_' + username + "_user_info";
    //QSqlQuery query;
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
        int elf_type = query.value(21).toInt();
        data += elf_name + "," + QString::number(elf_level) + "," + QString::number(experience) + "," +
                elf_attr + "," +
                QString::number(HP) + "," + QString::number(ATK) + "," + QString::number(DEF) + "," +
                QString::number(ATK_INTERVAL) + "," + QString::number(CRIT_Rate) + "," + QString::number(AGI) + "," +
                QString::number(up_HP) + "," + QString::number(up_ATK) + "," + QString::number(up_DEF) + "," +
                QString::number(up_ATK_INTERVAL) + "," + QString::number(up_CRIT_Rate) + "," + QString::number(up_AGI) + "," +
                QString::number(elf_skills_1) + "," + QString::number(elf_skills_2) + "," + QString::number(elf_skills_3) + "," + QString::number(elf_skills_4) + "," + QString::number(elf_id) + ','+ QString::number(elf_type) +"&";
    }
    SendData(socket, data);
}

void elf_server::Send_online_user_table(){
    QString onlineUserTable = (QChar)ONLINE_USER_TABLE + '&';
    QString offlineUserTable = "";

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Error: Database is not open";
        return;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT username FROM Accounts")) {
        qDebug() << "Error: Unable to execute query" << query.lastError();
        return;
    }

    QList<QString> clientUsernames = clients.values();
    QSet<QString> onlineUsers(clientUsernames.begin(), clientUsernames.end());

    while (query.next()) {
        QString username = query.value(0).toString();
        if (onlineUsers.contains(username)) {
            onlineUserTable += username + ',';
        } else {
            offlineUserTable += username + ',';
        }
    }
    onlineUserTable += '&';
    offlineUserTable += '&';
    QString userTable = onlineUserTable + offlineUserTable;
    for (QTcpSocket* clientSocket : clients.keys()) {
        SendData(clientSocket, userTable);
    }
}

void elf_server::Send_other_user_info(QTcpSocket * const socket, const QString& username){
    QString data = (QChar)SEND_OTHER_USER_INFO + '&';
    QSqlDatabase db = QSqlDatabase::database();
    // 查询用户信息
    QSqlQuery query;
    query.prepare("SELECT * FROM Accounts WHERE username = ?");
    query.bindValue(0, username);

    if (!query.exec()) {
        // 查询出错，处理错误
        qDebug() << "Error executing query:" << query.lastError().text();
        return;
    }

    // 如果找到了用户信息，发送回客户端
    if (query.next()) {
        data += QString::number(query.value("total_wins").toInt()) + "," +
                QString::number(query.value("total_battles").toInt()) + "," +
                QString::number(query.value("exp_wins").toInt()) + "," +
                QString::number(query.value("exp_battles").toInt()) + "," +
                QString::number(query.value("battle_wins").toInt()) + "," +
                QString::number(query.value("battle_battles").toInt()) + "," +
                QString::number(query.value("total_elf").toInt()) + "," +
                QString::number(query.value("metal_elf").toInt()) + "," +
                QString::number(query.value("wood_elf").toInt()) + "," +
                QString::number(query.value("water_elf").toInt()) + "," +
                QString::number(query.value("fire_elf").toInt()) + "," +
                QString::number(query.value("soil_elf").toInt()) + "," +
                QString::number(query.value("electric_elf").toInt()) + "," +
                QString::number(query.value("null_elf").toInt()) + "," +
                QString::number(query.value("max_level_elf").toInt()) + "," +
                QString::number(query.value("user_elf_id").toInt()) + '&';
    }
    QString tableName = '_' + username + "_user_info";
    //QSqlQuery query;
    // 执行查询
    query.prepare("SELECT * FROM " + tableName);
    if (!query.exec()) {
        qDebug() << "Error: Unable to execute query" << query.lastError();
        return;
    }
    while (query.next()) {
        QString elf_name = query.value(1).toString();
        int elf_level = query.value(2).toInt();
        QString elf_attr = query.value(4).toString();
        data += elf_name + "," + QString::number(elf_level) + "," + elf_attr + ".";
    }
    data += '&';
    SendData(socket, data);
}
