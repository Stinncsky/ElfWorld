#include "elf.h"
#include "ui_elf.h"

Elf::Elf(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Elf)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);
    IP = "127.0.0.1";
    port = 6677;

    QObject::connect(socket, &QTcpSocket::readyRead, this, &Elf::readData);
    QObject::connect(socket, &QTcpSocket::disconnected, this, &Elf::socket_disconnected);

    socket->connectToHost(IP, port);

    ui->reminder->hide();
    QGraphicsOpacityEffect *opacity = new QGraphicsOpacityEffect(this);
    ui->reminder->setGraphicsEffect(opacity);
    animation = new QPropertyAnimation(opacity, "opacity");
}

Elf::~Elf()
{
    delete ui;
}



void Elf::socket_disconnected(){
    socket->close();
}

void Elf::readData()
{
    QByteArray buf = socket->readAll();
    qDebug() << buf;
    int start = 0, next = 1;
    QString data(buf);
    QStringList parts = data.split('&');
    int numParts = parts.size();
    read_res:
    switch (parts[start][0].unicode()){
        case LOGIN:
            if (parts[next] == "1"){
                next++;
                ui->stackedWidget->setCurrentWidget(ui->func);
                cur_user = ui->username->text();
                ui->username->clear();
                ui->password->clear();
                ui->display_username->setText(cur_user);
                load_user_info();
            } else {
                remind("用户名不存在或密码错误");
            }
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case REGISTE:
            if (parts[1] == "1"){
                ui->stackedWidget->setCurrentWidget(ui->func);
                cur_user = ui->username->text();
                ui->username->clear();
                ui->password->clear();
                ui->display_username->setText(cur_user);
                new_user();
            } else {
                remind("该用户已被注册");
            }
            if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                start = next;
                next++;
                goto read_res;
            }
            break;
        case SEND_USER_INFO:
            for (; next < numParts - 1; next++){
                if (parts[next].size() == 1 && parts[next] < (QChar)NO_EVENT){
                    start = next;
                    next++;
                    goto read_res;
                }
                load_user_elf(parts[next]);
            }
            break;
    }
}

void Elf::SendData(const QString data){
    socket->write(data.toUtf8());
}

void Elf::on_exit_clicked()
{
    close();
}


void Elf::on_rgst_clicked()
{
    QString username = ui->username->text(), password = ui->password->text();
    if (username.length() < 4 || password.length() < 4){
        remind("用户名和密码不可以太短欧~");
        return;
    }
    QString data = (QChar)REGISTE+'&'+username+'&'+password + '&';
    SendData(data);
}


void Elf::on_login_clicked()
{
    QString username = ui->username->text(), password = ui->password->text();
    QString data = (QChar)LOGIN+'&'+username+'&'+password + '&';
    SendData(data);
}

void Elf::remind(QString content){
    ui->reminder->setText(content);
    if (animation->state() == QAbstractAnimation::Running)
        return;
    ui->reminder->show(); // 显示标签

    // 第一个动画：从透明到不透明
    animation->setDuration(700);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
    // 第一个定时器：在第一个动画结束后启动第二个动画
    QTimer::singleShot(700, this, [=]() {
        // 第二个动画：从不透明到透明
        animation->setDuration(700);
        animation->setStartValue(1);
        animation->setEndValue(0);
        animation->start();
        QTimer::singleShot(1000, this, [=]() {
            ui->reminder->hide();
        });
    });
}

void Elf::on_re_login_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->init);
    All_Elfs.clear();
}

elf_member Elf::random_init_elf(){
    std::random_device rd;  // 用于获取随机数的种子
    std::mt19937 gen(rd()); // 使用Mersenne Twister算法生成随机数
    std::uniform_int_distribution<> dis(0, ALL_ELF_NUM - 1); // 定义随机数的范围

    int random_index = dis(gen); // 生成随机数
    return elfs_normal[random_index];
}

void Elf::new_user(){
    elf_member elf;
    for (int i = 0; i < NEW_USER_ELF_NUM; i++){
        elf = random_init_elf();
        All_Elfs.push_back(elf);
        user_elf_add(elf);
    }
}

void Elf::user_elf_add(const elf_member& user_elf){
    QString data = (QChar)NEWELF + '&' + cur_user + '&';
    data += user_elf.Elf_name() + ',' + QString::number(user_elf.Elf_level()) + ',';
    data += QString::number(user_elf.Exp()) + ',' + QString::number(static_cast<int>(user_elf.Elf_attr())) + ',';
    data += QString::number(user_elf.Hp()) + ',' + QString::number(user_elf.Atk()) + ',';
    data += QString::number(user_elf.Def()) + ',' + QString::number(user_elf.Atk_INTERVAL()) + ',';
    data += QString::number(user_elf.Crit_Rate()) + ',' + QString::number(user_elf.Agi()) + ',';
    data += QString::number(user_elf.Up_HP()) + ',' + QString::number(user_elf.Up_ATK()) + ',';
    data += QString::number(user_elf.Up_DEF()) + ',' + QString::number(user_elf.Up_ATK_INTERVAL()) + ',';
    data += QString::number(user_elf.Up_CRIT_Rate()) + ',' + QString::number(user_elf.Up_AGI());
    const elf_skill* skills = user_elf.Elf_skills();
    for (int i = 0; i < 4; ++i) {
        data += ',' + QString::number(skillNameMap[skills[i].SkillName()]);
    }
    data += '&';
    SendData(data);
}

void Elf::load_user_info(){
    QString data = (QChar)GET_USER_INFO + '&' + cur_user + '&';
    SendData(data);
}

void Elf::load_user_elf(const QString& elf_data){
    QStringList parts = elf_data.split(',');
    if (parts.size() != 20) {
        qDebug() << "Error: Invalid elf data format";
        return;
    }

    QString name = parts[0];
    int level = parts[1].toInt();
    int experience = parts[2].toInt();
    Attribute attr = static_cast<Attribute>(parts[3].toInt());
    double hp = parts[4].toDouble();
    double atk = parts[5].toDouble();
    double def = parts[6].toDouble();
    double atk_interval = parts[7].toDouble();
    double crit_rate = parts[8].toDouble();
    double agi = parts[9].toDouble();
    double up_hp = parts[10].toDouble();
    double up_atk = parts[11].toDouble();
    double up_def = parts[12].toDouble();
    double up_atk_interval = parts[13].toDouble();
    double up_crit_rate = parts[14].toDouble();
    double up_agi = parts[15].toDouble();

    // 使用解析后的数据创建精灵对象
    elf_member elf(name, level, experience, attr, hp, atk, def, atk_interval,
                   crit_rate, agi, up_hp, up_atk, up_def, up_atk_interval,
                   up_crit_rate, up_agi);
    for (int i = 0; i < MAX_SKILL_NUM; i++){
        if (parts[i + 16] != ""){
            elf.add_skill(skills[parts[i + 16].toInt()]);
        }
    }
    All_Elfs.push_back(elf);
}

void Elf::display_ALLELF() {
    QWidget* widget = new QWidget;
    QGridLayout* layout = new QGridLayout(widget);

    // 设置元素之间的间距为零
    layout->setSpacing(18);

    // 设置元素的对齐方式为左上角
    layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    int columnCount = 8; // 每行显示的列数
    int rowCount = 0; // 当前行数
    int columnIndex = 0; // 当前列数
    int imageSize = 120; // 图片大小，包括边框

    for (int i = 0; i < (int)All_Elfs.size(); ++i) {
        ClickableLabel* label = new ClickableLabel;
        label->setFixedSize(imageSize, imageSize); // 设置固定大小
        QPixmap pixmap(":/pic/" + Elfs_List[All_Elfs[i].Elf_name()] +".png");
        pixmap.setDevicePixelRatio(devicePixelRatio()); // 可选，用于支持高DPI屏幕
        pixmap = pixmap.scaled(QSize(pixmap.width() * 4, pixmap.height() * 4), Qt::IgnoreAspectRatio, Qt::FastTransformation);
        connect(label, &ClickableLabel::clicked, [this, i, pixmap]() {
            // 根据索引 i 获取相应的 elf
            const elf_member& elf = All_Elfs[i];
            // 在这里添加展示 elf 信息的代码，例如弹出对话框

            ui->SH_showelf->setPixmap(pixmap.scaled(ui->SH_showelf->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));;
        });

        pixmap = pixmap.scaled(QSize(110, 110), Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // 缩放图片为 50x50
        label->setPixmap(pixmap);
        // 添加边框样式
        label->setStyleSheet("border: 1px solid black;");

        // 为 ClickableLabel 添加点击事件处理函数


        layout->addWidget(label, rowCount, columnIndex);

        // 更新当前列数和行数
        ++columnIndex;
        if (columnIndex == columnCount) {
            columnIndex = 0;
            ++rowCount;
        }
    }

    widget->setLayout(layout);
    ui->Storehouse_ELF->setWidget(widget);
}

void Elf::Solo(elf_member first, elf_member second){
    first.start_battle();
    second.start_battle();
}

void Elf::on_tabWidget_tabBarClicked(int index)
{
    switch (index){
        case 0:
            break;
        case 1:
            display_ALLELF();
            break;
    }
}

