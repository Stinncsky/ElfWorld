#include "elf.h"
#include "ui_elf.h"

Elf::Elf(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Elf)
{
    ui->setupUi(this);
    init();
    Exp_model = new QStandardItemModel(BATTLE_LIST_ELF_NUM, 3, this);
    Battle_model = new QStandardItemModel(BATTLE_LIST_ELF_NUM, 3, this);
    build_elf_list(Exp_model, ui->Exp_elf_list, Exp_elfs);
    build_elf_list(Battle_model, ui->Battle_elf_list, Battle_elfs);

    for (int i = 0; i < 6; i++) {
        My_Bag[i] = i;
    }
    socket = new QTcpSocket(this);
    IP = "127.0.0.1";
    port = 6677;

    ui->re_login->hide();
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


void Elf::init(){

    buffs = {
        Buff("中毒", -4, 0, 0, 0, 0, 0, MAX_BATTLE_TIME, 1),
        Buff("治疗", 20, 0, 0, 0, 0, 0, 10, 1),
        Buff("燃烧", -6, 0, 0, 0, 0, 0, 20, 1),
        Buff("冰冻", -2, 0, -1, 0, 0.1, 0, 30, 1),
        Buff("触电", -3, 0, 0, 0.1, 0, 0, 30, 1),
        Buff("疾风", 0, 0, 0, 0.06, -0.2, 0.05, MAX_BATTLE_TIME, 1),
        Buff("硬化", 0, 0, 10, 0, 0, 0, MAX_BATTLE_TIME, 1),
        Buff("激素", 0, 30, 0, 0, 0, 0.05, 20, 1),
        Buff("虚弱", 0, -5, 0, 0, 0, 0, MAX_BATTLE_TIME, 1),
        Buff("流血", -20, 0, -5, 0, 0, 0, 10, 1),
        Buff("强健", 0, 5, 0, 0, 0, 0, MAX_BATTLE_TIME, 1),
        Buff("生机", 5, 0, 0, 0, 0, 0, MAX_BATTLE_TIME, 0),
        Buff("缓慢", 0, 0, 0, -0.1, -0.1, 0, MAX_BATTLE_TIME, 1),
        Buff("凌空", 0, 0, 0, 0.1, 0, 0.1, MAX_BATTLE_TIME, 1),
        Buff("眩晕", 0, 0, 0, 0, 100, 0, 10, 1)
    };
    buffsMap = {
        {"中毒", 0},
        {"治疗", 1},
        {"燃烧", 2},
        {"冰冻", 3},
        {"触电", 4},
        {"疾风", 5},
        {"硬化", 6},
        {"激素", 7},
        {"虚弱", 8},
        {"流血", 9},
        {"强健", 10},
        {"生机", 11},
        {"缓慢", 12},
        {"凌空", 13},
        {"眩晕", 14}
    };
    skills = {
        elf_skill(),
        elf_skill("生机", 0, 60, Attribute("木"), buffs[buffsMap["治疗"]], 0, 1.0, "使用生机技能，恢复目标20点生命值，持续10秒。"),
        elf_skill("毒药", 0, 30, Attribute("木"), buffs[buffsMap["中毒"]], 1, 1.0, "释放毒药攻击，对目标造成30点威力值，并有100%的概率施加中毒状态，每秒造成4点伤害，持续10秒。"),
        elf_skill("打火机", 0, 60, Attribute("火"), buffs[buffsMap["燃烧"]], 1, 1.0, "使用打火机点燃目标，造成40点火焰威力值，并有100%的概率施加燃烧状态，每秒造成6点火焰伤害，持续10秒。"),
        elf_skill("冰箱", 0, 60, Attribute("水"), buffs[buffsMap["冰冻"]], 1, 1.0, "释放冰箱技能，对目标造成30点威力值，并有100%的概率使其陷入冰冻状态，行动速度减慢50%，持续10秒。"),
        elf_skill("埋土里", 0, 60, Attribute("土"), buffs[buffsMap["眩晕"]], 1, 1.0, "使用埋土里技能，对目标造成50点威力值，并有100%的概率使其陷入眩晕状态，持续3秒。"),
        elf_skill("麻痹", 0, 60, Attribute("电"), buffs[buffsMap["触电"]], 1, 1.0, "释放麻痹技能，对目标造成40点威力值，并有100%的概率施加麻痹状态，行动速度降低20%，持续10秒。"),
        elf_skill("手术刀", 0, 60, Attribute("金"), buffs[buffsMap["流血"]], 1, 1.0, "使用手术刀进行攻击，对目标造成60点威力值，并有100%的概率使其流血，每秒损失10点生命值，持续5秒。"),
        elf_skill("枯萎", 0, 60, Attribute("木"), buffs[buffsMap["虚弱"]], 1, 1.0, "释放枯萎技能，对目标造成20点威力值，并有100%的概率使其虚弱，攻击力降低30%，持续10秒。"),
        elf_skill("激素", 0, 60, Attribute(), buffs[buffsMap["激素"]], 0, 1.0, "使用激素注射，提升自身30%的攻击力，持续20秒。"),
        elf_skill("强健", 0, 60, Attribute(), buffs[buffsMap["强健"]], 0, 1.0, "使用强健技能，提升自身10%的防御力，持续20秒。"),
        elf_skill("风之庇佑", 0, 60, Attribute("木"), buffs[buffsMap["疾风"]], 0, 1.0, "获得风之庇佑，提升自身6%的敏捷度，持续20秒。"),
        elf_skill("金钟罩", 0, 60, Attribute("金"), buffs[buffsMap["硬化"]], 0, 1.0, "获得金钟罩的保护，提升自身10%的防御力，持续20秒。"),
        elf_skill("石化", 0, 60, Attribute("土"), buffs[buffsMap["硬化"]], 0, 1.0, "使用石化技能，使自身变得坚硬如石，提升自身10%的防御力，持续20秒。"),
        elf_skill("森林", 0, 60, Attribute("木"), buffs[buffsMap["硬化"]], 0, 1.0, "融入森林之力，提升自身10%的防御力和6%的敏捷度，持续20秒。"),
        elf_skill("水泡", 0, 60, Attribute("水"), buffs[buffsMap["硬化"]], 0, 1.0, "使用水泡技能，包裹自身水泡，提升自身10%的防御力和6%的敏捷度，持续20秒。"),
        elf_skill("光合作用", 0, 60, Attribute("木"), buffs[buffsMap["生机"]], 0, 1.0, "通过光合作用恢复自身生命，每秒恢复5点生命值，持续10秒。"),
        elf_skill("川流不息", 0, 60, Attribute("水"), buffs[buffsMap["生机"]], 0, 1.0, "吸收水流能量，每秒恢复5点生命值，持续10秒。"),
        elf_skill("生生不息", 0, 60, Attribute("火"), buffs[buffsMap["生机"]], 0, 1.0, "拥有生命之火，每秒恢复5点生命值，持续10秒。"),
        elf_skill("大地之母的庇佑", 0, 60, Attribute("土"), buffs[buffsMap["生机"]], 0, 1.0, "获得大地之母的庇佑，每秒恢复5点生命值，持续10秒。"),
        elf_skill("金汤肥牛", 0, 60, Attribute("金"), buffs[buffsMap["生机"]], 0, 1.0, "享用金汤肥牛，每秒恢复5点生命值，持续10秒。"),
        elf_skill("雷池", 0, 60, Attribute("电"), buffs[buffsMap["生机"]], 0, 1.0, "融入雷池能量，每秒恢复5点生命值，持续10秒。"),
        elf_skill("沼泽", 0, 60, Attribute("土"), buffs[buffsMap["缓慢"]], 1, 1.0, "使用沼泽技能，使目标陷入泥沼，行动速度降低50%，持续10秒。"),
        elf_skill("漩涡", 0, 60, Attribute("水"), buffs[buffsMap["缓慢"]], 1, 1.0, "使用漩涡技能，使目标陷入漩涡，行动速度降低50%，持续10秒。"),
        elf_skill("蓝银缠绕", 0, 60, Attribute("木"), buffs[buffsMap["缓慢"]], 1, 1.0, "使用蓝银缠绕技能，使目标被蓝银之力缠绕，行动速度降低50%，持续10秒。"),
        elf_skill("火之翼", 0, 60, Attribute("火"), buffs[buffsMap["凌空"]], 0, 1.0, "展开火之翼，飞向天空，躲避地面攻击。"),
        elf_skill("水之翼", 0, 60, Attribute("水"), buffs[buffsMap["凌空"]], 0, 1.0, "展开水之翼，飞向天空，躲避地面攻击。"),
        elf_skill("金之翼", 0, 60, Attribute("金"), buffs[buffsMap["凌空"]], 0, 1.0, "展开金之翼，飞向天空，躲避地面攻击。"),
        elf_skill("电之翼", 0, 60, Attribute("电"), buffs[buffsMap["凌空"]], 0, 1.0, "展开电之翼，飞向天空，躲避地面攻击。"),
        elf_skill("风之翼", 0, 60, Attribute("木"), buffs[buffsMap["凌空"]], 0, 1.0, "展开风之翼，飞向天空，躲避地面攻击。"),
        elf_skill("爆炸", 120, 60, Attribute("火"), buffs[buffsMap["燃烧"]], 1, 0.2, "引爆自身周围环境，对敌人造成120点火焰威力值，并有20%的概率施加燃烧状态，每秒造成24点火焰伤害，持续10秒。"),
        elf_skill("冰冻", 120, 60, Attribute("水"), buffs[buffsMap["冰冻"]], 1, 0.2, "对目标造成120点威力值，并有20%的概率使其陷入冰冻状态，行动速度减慢50%，持续10秒。"),
        elf_skill("千钧", 120, 60, Attribute("金"), buffs[buffsMap["眩晕"]], 1, 0.2, "对目标造成120点威力值，并有20%的概率使其陷入眩晕状态，失去行动能力，持续10秒。"),
        elf_skill("雷击", 120, 60, Attribute("电"), buffs[buffsMap["麻痹"]], 1, 0.2, "对目标造成120点电击威力值，并有20%的概率施加麻痹状态，行动速度降低20%，持续10秒。"),
        elf_skill("陨石", 120, 60, Attribute("土"), buffs[buffsMap["眩晕"]], 1, 0.2, "召唤陨石坠落，对目标造成120点威力值，并有20%的概率使其陷入眩晕状态，失去行动能力，持续10秒。"),
        elf_skill("中毒", 120, 60, Attribute("木"), buffs[buffsMap["中毒"]], 1, 0.2, "释放毒素攻击，对目标造成120点威力值，并有20%的概率施加中毒状态，每秒造成16点伤害，持续10秒。"),
        elf_skill("虚弱", 120, 60, Attribute("木"), buffs[buffsMap["虚弱"]], 1, 0.2, "释放虚弱技能，对目标造成120点威力值，并有20%的概率使其虚弱，攻击力降低30%，持续10秒。"),
        elf_skill("流血", 120, 60, Attribute("金"), buffs[buffsMap["流血"]], 1, 0.2, "使用流血技能，对目标造成120点威力值，并有20%的概率使其流血，每秒损失20点生命值，持续5秒。"),
        elf_skill("风刃", 120, 60, Attribute("木"), buffs[buffsMap["疾风"]], 0, 0.2, "释放尖锐的风刃，对目标造成120点威力值，并有20%的概率减缓其行动，持续10秒。"),
        elf_skill("蓄意轰拳", 150, 60, Attribute(), Buff(), 1, 0, "聚集力量，对目标造成150点威力值的打击。")


    };

    skillNameMap = {
        {"生机", 1},
        {"毒药", 2},
        {"打火机", 3},
        {"冰箱", 4},
        {"埋土里", 5},
        {"麻痹", 6},
        {"手术刀", 7},
        {"枯萎", 8},
        {"激素", 9},
        {"强健", 10},
        {"风之庇佑", 11},
        {"金钟罩", 12},
        {"石化", 13},
        {"森林", 14},
        {"水泡", 15},
        {"光合作用", 16},
        {"川流不息", 17},
        {"生生不息", 18},
        {"大地之母的庇佑", 19},
        {"金汤肥牛", 20},
        {"雷池", 21},
        {"沼泽", 22},
        {"漩涡", 23},
        {"蓝银缠绕", 24},
        {"火之翼", 25},
        {"水之翼", 26},
        {"金之翼", 27},
        {"电之翼", 28},
        {"风之翼", 29},
        {"爆炸", 30},
        {"冰冻", 31},
        {"千钧", 32},
        {"雷击", 33},
        {"陨石", 34},
        {"中毒", 35},
        {"虚弱", 36},
        {"流血", 37},
        {"风刃", 38},
        {"蓄意轰拳", 39}
    };

    Elfs_List = {
        {"导弹毛", "1"},
        {"哮天犬", "2"},
        {"暗雷小妖", "3"},
        {"水玄龟", "4"}
    };
    elfs_normal = {
        elf_member("导弹毛", 1, 0, Attribute()),
        elf_power("哮天犬", 1, 0, Attribute("火")),
        elf_power("暗雷小妖", 1, 0, Attribute("电")),
        elf_tank("水玄龟", 1, 0, Attribute("水"))
    };

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
                ui->re_login->show();
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
                ui->re_login->show();
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
    ui->re_login->hide();
}

elf_member Elf::random_init_elf(){
    std::random_device rd;  // 用于获取随机数的种子
    std::mt19937 gen(rd()); // 使用Mersenne Twister算法生成随机数
    std::uniform_int_distribution<> dis(0, ALL_ELF_NUM - 1); // 定义随机数的范围

    int random_index = dis(gen); // 生成随机数
    return elfs_normal[random_index];
}

void Elf::new_user(){
    user_elf_id = 1;
    elf_member elf;
    for (int i = 0; i < NEW_USER_ELF_NUM; i++){
        elf = random_init_elf();
        elf.elf_id = user_elf_id++;
        select_Skill(elf);
        All_Elfs.push_back(elf);
        user_elf_add(elf);
    }
}

void Elf::user_elf_add(const elf_member& user_elf){
    QString data = (QChar)NEWELF + '&' + cur_user + '&';
    data += elf_info_to_QString(user_elf);
    SendData(data);
}

void Elf::select_Skill(elf_member& elf){
    QList<elf_skill> selectedSkills;
    QSet<QString> selectedSkillNames;

    while (selectedSkills.size() < 4) {
        int randomIndex = QRandomGenerator::global()->bounded(skills.size());
        elf_skill skill = skills[randomIndex];

        // 检查技能属性是否为空，并且与精灵属性不同，且技能名称不在已选的技能列表中
        if (skill.SkillName() != "" && (skill.Attr() == Attribute() || skill.Attr() == elf.Elf_attr() || elf.Elf_attr() == Attribute()) && !selectedSkillNames.contains(skill.SkillName())) {
            selectedSkills.append(skill);
            selectedSkillNames.insert(skill.SkillName());
        }
    }

    // 创建对话框
    QDialog dialog;
    QVBoxLayout layout(&dialog);
    QLabel label("请选择一个技能:");
    layout.addWidget(&label);

    // 显示四个技能按钮
    QList<QPushButton*> buttons;
    for (const auto& skill : selectedSkills) {
        QPushButton* button = new QPushButton(skill.SkillName());
        buttons.append(button);
        layout.addWidget(button);

        QObject::connect(button, &QPushButton::clicked, [&elf, &dialog, &skill]() {
            // 在这里处理用户点击了哪个按钮
            elf.add_skill(skill);
            qDebug() << "用户选择了技能：" << skill.SkillName();
            dialog.accept(); // 关闭对话框
        });
    }

    dialog.setLayout(&layout);
    dialog.exec(); // 显示对话框
}

QString Elf::elf_info_to_QString(const elf_member& elf){
    QString data;
    data += elf.Elf_name() + ',' + QString::number(elf.Elf_level()) + ',';
    data += QString::number(elf.Exp()) + ',' + elf.Elf_attr().Attr_name() + ',';
    data += QString::number(elf.Hp()) + ',' + QString::number(elf.Atk()) + ',';
    data += QString::number(elf.Def()) + ',' + QString::number(elf.Atk_INTERVAL()) + ',';
    data += QString::number(elf.Crit_Rate()) + ',' + QString::number(elf.Agi()) + ',';
    data += QString::number(elf.Up_HP()) + ',' + QString::number(elf.Up_ATK()) + ',';
    data += QString::number(elf.Up_DEF()) + ',' + QString::number(elf.Up_ATK_INTERVAL()) + ',';
    data += QString::number(elf.Up_CRIT_Rate()) + ',' + QString::number(elf.Up_AGI());
    const elf_skill* skills = elf.Elf_skills();
    for (int i = 0; i < 4; ++i) {
        data += ',' + QString::number(skillNameMap[skills[i].SkillName()]);
    }
    data += '&';
    return data;
}

void Elf::load_user_info(){
    QString data = (QChar)GET_USER_INFO + '&' + cur_user + '&';
    SendData(data);
}

void Elf::load_user_elf(const QString& elf_data){
    QStringList parts = elf_data.split(',');
    if (parts.size() != 21) {
        qDebug() << "Error: Invalid elf data format";
        return;
    }

    QString name = parts[0];
    int level = parts[1].toInt();
    int experience = parts[2].toInt();
    Attribute attr = Attribute(parts[3]);
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
    elf.elf_id = parts[20].toInt();
    All_Elfs.push_back(elf);
}

void Elf::update_user_elf(const elf_member& elf,const int user_elf_id){
    QString data = (QChar)UPDATE_USER_ELF + '&' + cur_user + '&' + QString::number(user_elf_id) + '&';
    data += elf_info_to_QString(elf);
    SendData(data);
}

void Elf::delete_user_elf(const int user_elf_id){
    All_Elfs.erase(All_Elfs.begin() + user_elf_id - 1);
    QString data = (QChar)DELETE_USER_ELF + '&' + cur_user + '&' + QString::number(user_elf_id) + '&';
    SendData(data);
}

void Elf::display_ALLELF() {
    QWidget* widget = new QWidget;
    QGridLayout* layout = new QGridLayout(widget);
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // 设置元素之间的间距为零
    layout->setSpacing(18);
    // 设置元素的对齐方式为左上角
    layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    int columnCount = 4; // 每行显示的列数
    int rowCount = 0; // 当前行数
    int columnIndex = 0; // 当前列数
    int imageSize = 120; // 图片大小，包括边框

    for (int i = 0; i < (int)All_Elfs.size(); ++i) {
        ClickableLabel* label = new ClickableLabel;
        label->setFixedSize(imageSize, imageSize); // 设置固定大小
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QPixmap pixmap(":/pic/" + Elfs_List[All_Elfs[i].Elf_name()] +".png");
        pixmap.setDevicePixelRatio(devicePixelRatio()); // 可选，用于支持高DPI屏幕
        pixmap = pixmap.scaled(QSize(pixmap.width() * 4, pixmap.height() * 4), Qt::IgnoreAspectRatio, Qt::FastTransformation);
        connect(label, &ClickableLabel::clicked, [this, i, pixmap]() {
            // 根据索引 i 获取相应的 elf
            const elf_member& elf = All_Elfs[i];
            // 展示 elf 信息
            selected_elf = i;
            ui->SH_showelf->setPixmap(pixmap.scaled(ui->SH_showelf->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));;
            ui->SH_elf_name->setText(elf.Elf_name());
            ui->SH_elf_attr->setText(elf.Elf_attr().Attr_name());
            ui->SH_elf_level->setText(QString::number(elf.Elf_level()));
            ui->SH_elf_hp->setText(QString::number(elf.Hp()));
            ui->SH_elf_atk->setText(QString::number(elf.Atk()));
            ui->SH_elf_ai->setText(QString::number(elf.Atk_INTERVAL()));
            ui->SH_elf_def->setText(QString::number(elf.Def()));
            ui->SH_elf_cr->setText(QString::number(elf.Crit_Rate()));
            ui->SH_elf_agi->setText(QString::number(elf.Agi()));

            ui->SH_elf_exp->setMaximum(elf.Up_Exp());
            ui->SH_elf_exp->setValue(elf.Exp());
            const elf_skill *skills = elf.Elf_skills();
            //第一个技能信息
            if (skills[0].SkillName() != ""){
                ui->Skill_1->setTitle(skills[0].SkillName());
                ui->Skill_1_cd->setText(QString::number(skills[0].CD()));
                ui->Skill_1->setToolTip(skills[0].Desc());
                if (skills[0].DMG() != 0)
                    ui->Skill_1_dmg->setText(QString::number(skills[0].DMG()));
                else
                    ui->Skill_1_dmg->setText("--");

                if (skills[0].Attr() != Attribute(""))
                    ui->Skill_1_attr->setText(QString(skills[0].Attr().Attr_name()));
                else
                    ui->Skill_1_attr->setText("--");
            } else {
                ui->Skill_1->setTitle("--");
                ui->Skill_1_attr->setText("--");
                ui->Skill_1_cd->setText("--");
                ui->Skill_1_dmg->setText("--");
            }
            //第二个技能信息
            if (skills[1].SkillName() != ""){
                ui->Skill_2->setTitle(skills[1].SkillName());
                ui->Skill_2_cd->setText(QString::number(skills[1].CD()));
                ui->Skill_2->setToolTip(skills[1].Desc());
                if (skills[1].DMG() != 0)
                    ui->Skill_2_dmg->setText(QString::number(skills[1].DMG()));
                else
                    ui->Skill_2_dmg->setText("--");

                if (skills[1].Attr() != Attribute(""))
                    ui->Skill_2_attr->setText(QString(skills[1].Attr().Attr_name()));
                else
                    ui->Skill_2_attr->setText("--");
            } else {
                ui->Skill_2->setTitle("--");
                ui->Skill_2_attr->setText("--");
                ui->Skill_2_cd->setText("--");
                ui->Skill_2_dmg->setText("--");
            }
            //第三个技能信息
            if (skills[2].SkillName() != ""){
                ui->Skill_3->setTitle(skills[2].SkillName());
                ui->Skill_3_cd->setText(QString::number(skills[2].CD()));
                ui->Skill_3->setToolTip(skills[2].Desc());
                if (skills[2].DMG() != 0)
                    ui->Skill_3_dmg->setText(QString::number(skills[2].DMG()));
                else
                    ui->Skill_3_dmg->setText("--");

                if (skills[2].Attr() != Attribute(""))
                    ui->Skill_3_attr->setText(QString(skills[2].Attr().Attr_name()));
                else
                    ui->Skill_3_attr->setText("--");
            } else {
                ui->Skill_3->setTitle("--");
                ui->Skill_3_attr->setText("--");
                ui->Skill_3_cd->setText("--");
                ui->Skill_3_dmg->setText("--");
            }
            //第四个技能信息
            if (skills[3].SkillName() != ""){
                ui->Skill_4->setTitle(skills[3].SkillName());
                ui->Skill_4_cd->setText(QString::number(skills[3].CD()));
                ui->Skill_4->setToolTip(skills[3].Desc());
                if (skills[3].DMG() != 0)
                    ui->Skill_4_dmg->setText(QString::number(skills[3].DMG()));
                else
                    ui->Skill_4_dmg->setText("--");

                if (skills[3].Attr() != Attribute(""))
                    ui->Skill_4_attr->setText(QString(skills[3].Attr().Attr_name()));
                else
                    ui->Skill_4_attr->setText("--");
            } else {
                ui->Skill_4->setTitle("--");
                ui->Skill_4_attr->setText("--");
                ui->Skill_4_cd->setText("--");
                ui->Skill_4_dmg->setText("--");
            }
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
    QGridLayout* storehouseLayout = new QGridLayout(ui->Storehouse_ELF);
    storehouseLayout->setContentsMargins(0, 0, 0, 0);

    // 在布局中添加 widget
    storehouseLayout->addWidget(widget);

    // 设置 widget 的伸展策略，使其占据所有可用空间
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 将布局设置为 widget 的布局
    ui->Storehouse_ELF->setLayout(storehouseLayout);
    ui->Storehouse_ELF->setWidget(widget);
}

void Elf::Solo(){
    //首先初始化界面
    ui->Battle_info->clear();
    QPixmap pixmap(":/pic/" + Elfs_List[first->Elf_name()] +".png");
    pixmap = pixmap.scaled(QSize(100, 100), Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // 缩放图片为 50x50
    ui->F_image->setPixmap(pixmap);
    ui->F_elf_HP->setMaximum(first->Hp());
    ui->F_elf_HP->setValue(first->Hp());
    pixmap = QPixmap(":/pic/" + Elfs_List[second->Elf_name()] +".png");
    pixmap = pixmap.scaled(QSize(100, 100), Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // 缩放图片为 50x50
    ui->S_image->setPixmap(pixmap);
    ui->S_elf_HP->setMaximum(second->Hp());
    ui->S_elf_HP->setValue(second->Hp());
    //初始化倒计时时钟
    remainingTime = MAX_BATTLE_TIME;
    cur_min = remainingTime / 60; // 获取分钟数
    cur_sec = remainingTime % 60; // 获取剩余的秒数
    QString timeStr = QString("%1:%2").arg(cur_min, 2, 10, QChar('0')).arg(cur_sec, 2, 10, QChar('0')); // 格式化时间字符串
    ui->Time_clock->display(timeStr); // 更新 QLCDNumber 显示的时间
    //确认战斗速度
    round_time /= ui->battle_speed_rate->value();
    //倒计时计时器
    QTimer *timer = new QTimer;
    connect(timer, &QTimer::timeout, this, [&](){//倒计时
        if (remainingTime > 0) {
            (remainingTime)--; // 每秒减少1秒
            cur_min = remainingTime / 60; // 获取分钟数
            cur_sec = remainingTime % 60; // 获取剩余的秒数
            QString timeStr = QString("%1:%2").arg(cur_min, 2, 10, QChar('0')).arg(cur_sec, 2, 10, QChar('0')); // 格式化时间字符串
            ui->Time_clock->display(timeStr); // 更新 QLCDNumber 显示的时间
        } else {
            Solo_over(); // 时间到，停止计时器
        }
    });
    timer->setInterval(round_time);
    //初始化战斗属性
    first->start_battle();
    second->start_battle();
    //获取技能列表
    const elf_skill *skills_1 = first->Elf_skills(), *skills_2 = second->Elf_skills();

    QTimer *buffTimer = new QTimer, *firstTimer = new QTimer, *secondTimer = new QTimer;
    Timers.append(timer);
    Timers.append(buffTimer);
    Timers.append(firstTimer);
    Timers.append(secondTimer);
    //技能计时器
    for (int i = 0; i < MAX_SKILL_NUM; i++){
        if (skills_1[i].SkillName() != ""){
            QTimer *skill_1 = new QTimer;
            skill_1->setInterval(skills_1[i].CD() * round_time);
            connect(skill_1, &QTimer::timeout, this, [&, i, skill_1, skills_1](){
                // 处理技能的超时的代码
                ui->Battle_info->append("<font color=\"#30F6AD\">[" + QString::number(cur_min) + ':' + QString::number(cur_sec) + ']' + "</font>" + first->release_skill(*second, i));
                skill_1->setInterval(skills_1[i].CD() * round_time);
                ui->F_elf_HP->setValue(first->Fting_HP());
                ui->S_elf_HP->setValue(second->Fting_HP());
                if (first->Fting_HP() <= 0 || second->Fting_HP() <= 0){
                    Solo_over();
                }
            });
            Timers.append(skill_1);
        }

        if (skills_2[i].SkillName() != ""){
            QTimer *skill_2 = new QTimer;
            skill_2->setInterval(skills_2[i].CD() * round_time);
            connect(skill_2, &QTimer::timeout, this, [&, i, skill_2, skills_2](){
                // 处理技能的超时的代码
                ui->Battle_info->append("<font color=\"#FC0750\">[" + QString::number(cur_min) + ':' + QString::number(cur_sec) + ']' + "</font>" + second->release_skill(*first, i));
                skill_2->setInterval(skills_2[i].CD() * round_time);
                ui->F_elf_HP->setValue(first->Fting_HP());
                ui->S_elf_HP->setValue(second->Fting_HP());
                if (first->Fting_HP() <= 0 || second->Fting_HP() <= 0){
                    Solo_over();
                }
            });
            Timers.append(skill_2);
        }
    }
    //buff对生命值效果的计时器
    buffTimer->setInterval(round_time);
    connect(buffTimer, &QTimer::timeout, this, [&, buffTimer](){
        QString info;
        info = first->buff_to_HP();
        if (info != ""){
            info = "<font color=\"#30F6AD\">[" + QString::number(cur_min) + ':' + QString::number(cur_sec) + ']' + "</font>" + first->Elf_name() + info;
            ui->Battle_info->append(info);
        }
        info = second->buff_to_HP();
        if (info != ""){
            info = "<font color=\"#30F6AD\">[" + QString::number(cur_min) + ':' + QString::number(cur_sec) + ']' + "</font>" + second->Elf_name() + info;
            ui->Battle_info->append(info);
        }
        buffTimer->setInterval(round_time);
        ui->F_elf_HP->setValue(first->Fting_HP());
        ui->S_elf_HP->setValue(second->Fting_HP());
        if (first->Fting_HP() <= 0 || second->Fting_HP() <= 0){
            Solo_over();
        }
    });
    //普攻计时器
    firstTimer->setInterval(first->Fting_ATK_INTERVAL() * round_time);
    connect(firstTimer, &QTimer::timeout, this, [&, firstTimer]() {
        QString NATK_info = first->normal_ATK(*second);
        QString num = "-" + extractNumbers(NATK_info);
        show_HPC(ui->F_HPC, num);
        ui->Battle_info->append("<font color=\"#30F6AD\">[" + QString::number(cur_min) + ':' + QString::number(cur_sec) + ']' + "</font>" + NATK_info);
        firstTimer->setInterval(first->Fting_ATK_INTERVAL() * round_time); // 更新定时器的间隔
        ui->F_elf_HP->setValue(first->Fting_HP());
        ui->S_elf_HP->setValue(second->Fting_HP());
        if (first->Fting_HP() <= 0 || second->Fting_HP() <= 0){
            Solo_over();
        }
    });

    secondTimer->setInterval(second->Fting_ATK_INTERVAL() * round_time);
    connect(secondTimer, &QTimer::timeout, this, [&, secondTimer]() {
        QString NATK_info = second->normal_ATK(*first);
        QString num = "-" + extractNumbers(NATK_info);
        show_HPC(ui->S_HPC, num);
        ui->Battle_info->append("<font color=\"#FC0750\">[" + QString::number(cur_min) + ':' + QString::number(cur_sec) + ']' + "</font>" + NATK_info);
        secondTimer->setInterval(second->Fting_ATK_INTERVAL() * round_time); // 更新定时器的间隔
        ui->F_elf_HP->setValue(first->Fting_HP());
        ui->S_elf_HP->setValue(second->Fting_HP());
        if (first->Fting_HP() <= 0 || second->Fting_HP() <= 0){
            Solo_over();
        }
    });
    /*interval_update.setInterval(1);
    connect(&interval_update, &QTimer::timeout, this, [&, firstTimer, secondTimer] {
        firstTimer->setInterval(first->Atk_INTERVAL());
        secondTimer->setInterval(second->Atk_INTERVAL());
    });*/
    interval_update.start();
    //计时器启动，开始战斗
    for (QTimer* timer : Timers) {
            QMetaObject::invokeMethod(timer, "timeout", Qt::DirectConnection);
            timer->start();
        }
}

void Elf::show_HPC(QVBoxLayout *layout, QString &C_info){
    QLabel *label = new QLabel(C_info);

    // 根据信息内容设置标签颜色，可以扩展为更复杂的逻辑
    if (C_info.contains('-')) {
        label->setStyleSheet("color: red;");
    } else {
        label->setStyleSheet("color: green;");
    }

    // 设置初始字体大小
    QFont initialFont = label->font();
    initialFont.setPointSize(12); // 初始字体大小
    label->setFont(initialFont);

    // 将 QLabel 添加到垂直布局中
    layout->addWidget(label);

    // 获取布局中心的位置
    int layoutWidth = layout->geometry().width();
    int layoutHeight = layout->geometry().height();
    int centerX = layout->geometry().center().x();
    int centerY = layout->geometry().center().y();

    // 设置 QLabel 的初始位置和大小
    label->resize(100, 40);
    label->move(centerX - 50, centerY - 20);

    // 创建一个透明度效果
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(label);
    label->setGraphicsEffect(opacityEffect);

    // 创建一个动画对象，设置属性为透明度变化
    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    opacityAnimation->setDuration(1000); // 动画持续时间为 1 秒
    opacityAnimation->setStartValue(0.0);
    opacityAnimation->setEndValue(1.0);

    // 创建一个动画对象，设置属性为几何变化
    QPropertyAnimation *sizeAnimation = new QPropertyAnimation(label, "geometry");
    QRect startRect(centerX - 50, centerY - 20, 100, 40); // 从初始矩形开始
    QRect endRect(centerX - 50, centerY - 20, 150, 60); // 扩展到一个较大的矩形
    sizeAnimation->setDuration(1000); // 动画持续时间为 1 秒
    sizeAnimation->setStartValue(startRect);
    sizeAnimation->setEndValue(endRect);

    // 动画字体大小变化
    QPropertyAnimation *fontAnimation = new QPropertyAnimation(label, "fontSize");
    fontAnimation->setDuration(1000); // 动画持续时间为 1 秒
    fontAnimation->setStartValue(12); // 初始字体大小
    fontAnimation->setEndValue(20); // 最终字体大小

    // 将动画绑定到 QLabel 的样式表变化上
    connect(fontAnimation, &QPropertyAnimation::valueChanged, [label](const QVariant &value) {
        int fontSize = value.toInt();
        QFont font = label->font();
        font.setPointSize(fontSize);
        label->setFont(font);
    });

    // 设置动画完成后自动删除
    connect(opacityAnimation, &QPropertyAnimation::finished, label, &QLabel::deleteLater);
    connect(sizeAnimation, &QPropertyAnimation::finished, sizeAnimation, &QPropertyAnimation::deleteLater);
    connect(fontAnimation, &QPropertyAnimation::finished, fontAnimation, &QPropertyAnimation::deleteLater);

    // 启动动画
    opacityAnimation->start();
    sizeAnimation->start();
    fontAnimation->start();

    // 在动画结束后几秒后移除标签
    QTimer::singleShot(1500, label, &QLabel::deleteLater);
}
QString Elf::extractNumbers(const QString &str) {
    QRegularExpression re("-?\\d+"); // 匹配可选的负号和一个或多个数字
    QRegularExpressionMatchIterator i = re.globalMatch(str);

    QString result;
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        result = match.captured(0); // 获取匹配的数字
    }
    return result;
}

void Elf::Solo_over(){
    for (QTimer* timer : Timers) {
        timer->stop();
        timer->disconnect();
        timer->deleteLater();
    }
    Timers.clear();
    interval_update.stop();
    round_time = 1000;
    ui->Battle_exit->show();
    ui->Battle_escape->hide();
    if (second->Fting_HP() <= 0){
        user_win();
    } else {
        user_lose();
    }
}

void Elf::on_ALL_func_page_tabBarClicked(int index)
{
    switch (index){
        case 0:
            break;
        case 1:
            display_ALLELF();
            break;
    }
}

void Elf::on_open_bag_clicked()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("My Bag");

    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
    QHBoxLayout *imagesLayout = new QHBoxLayout;
    QVBoxLayout *buttonsLayout = new QVBoxLayout;

    QLabel *bigImageLabel = new QLabel;
    bigImageLabel->setFixedSize(300, 300);
    bigImageLabel->setAlignment(Qt::AlignCenter);
    imagesLayout->addWidget(bigImageLabel, 0, Qt::AlignTop);

    for (int i = 0; i < 6; ++i) {
        ClickableLabel *elfLabel = new ClickableLabel;
        QPixmap pixmap(":/pic/" + Elfs_List[All_Elfs[My_Bag[i]].Elf_name()] +".png"); // 你需要根据实际情况修改
        pixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio);
        elfLabel->setPixmap(pixmap);
        connect(elfLabel, &ClickableLabel::clicked, [this, bigImageLabel, i]() {
            QPixmap pixmap(":/pic/" + Elfs_List[All_Elfs[My_Bag[i]].Elf_name()] +".png"); // 你需要根据实际情况修改
            pixmap = pixmap.scaled(300, 300, Qt::KeepAspectRatio);
            bigImageLabel->setPixmap(pixmap);
        });
        imagesLayout->addWidget(elfLabel);
    }

    QPushButton *closeButton = new QPushButton("Close");
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::close);
    buttonsLayout->addWidget(closeButton);

    mainLayout->addLayout(imagesLayout);
    mainLayout->addLayout(buttonsLayout);

    dialog->exec();
}

void Elf::build_elf_list(QStandardItemModel *model, QTableView *list, std::vector<elf_member>& elf_list){
    model->clear();
    model->setHorizontalHeaderLabels({"头像", "名字", "等级"});

    elf_list.clear();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, MAX_LEVEL - 1);
    elf_member elf;
    for (int i = 0; i < BATTLE_LIST_ELF_NUM; i++){
        elf = random_init_elf();
        int n = dist(gen);
        elf.get_exp(n * (n + 1) * (n + 2) / 6 * 100);
        elf_list.emplace_back(elf);

        // 获取 elf 的头像地址
        QString avatarPath = ":/pic/" + Elfs_List[elf.Elf_name()] + ".png";

        // 创建头像的圆形裁剪
        QPixmap avatarPixmap(avatarPath);
        QPixmap roundedPixmap(avatarPixmap.size());
        roundedPixmap.fill(Qt::transparent);
        QPainter painter(&roundedPixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);
        QPainterPath path;
        path.addEllipse(0, 0, avatarPixmap.width(), avatarPixmap.height());
        painter.setClipPath(path);
        painter.drawPixmap(0, 0, avatarPixmap.width(), avatarPixmap.height(), avatarPixmap);
        painter.end();

        // 调整头像大小
        int avatarSize = 50; // 头像大小
        QPixmap scaledPixmap = roundedPixmap.scaled(avatarSize, avatarSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // 创建 QStandardItem，并设置头像、名字和等级
        QStandardItem *avatarItem = new QStandardItem(); // 设置头像
        avatarItem->setData(QVariant(scaledPixmap), Qt::DecorationRole);

        QStandardItem *nameItem = new QStandardItem(elf.Elf_name()); // 设置名字
        QStandardItem *levelItem = new QStandardItem(QString::number(elf.Elf_level())); // 设置等级

        // 添加 QStandardItem 到 model 中
        model->setItem(i, 0, avatarItem);
        model->setItem(i, 1, nameItem);
        model->setItem(i, 2, levelItem);
    }
    list->setModel(model);
}

void Elf::on_Exp_refresh_clicked()
{
    build_elf_list(Exp_model, ui->Exp_elf_list, Exp_elfs);
}

void Elf::on_Battle_refresh_clicked()
{
    build_elf_list(Battle_model, ui->Battle_elf_list, Battle_elfs);
}

void Elf::user_win(){
    if (is_elf_solo){
        second->elf_id = user_elf_id++;
        All_Elfs.push_back(*second);
        user_elf_add(*second);
    }
    int exp = 0, fl = first->Elf_level(), sl = second->Elf_level();
    if (fl <= sl){
        exp = fl * 100 + (sl - fl) * (sl - fl + 1) * 100;
    } else {
        exp = sl * 100;
    }
    first->get_exp(exp);
    update_user_elf(*first, first->elf_id);
}

void Elf::user_lose(){
    if (is_elf_solo){
        delete_user_elf(All_Elfs[My_Bag[0]].elf_id);
    }
}

void Elf::on_Exp_battle_start_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->fight);
    ui->Battle_exit->hide();
    ui->Battle_escape->show();
    int enemy_id = ui->Exp_elf_list->currentIndex().row();
    first = &All_Elfs[My_Bag[0]];
    second = &Exp_elfs[enemy_id];
    is_elf_solo = 0;
    Solo();
}

void Elf::on_Battle_start_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->fight);
    ui->Battle_exit->hide();
    ui->Battle_escape->show();
    int enemy_id = ui->Battle_elf_list->currentIndex().row();
    first = &All_Elfs[My_Bag[0]];
    second = &Battle_elfs[enemy_id];
    is_elf_solo = 1;
    Solo();
}

void Elf::on_Battle_exit_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->func);
}

void Elf::on_Battle_escape_clicked()
{
    Solo_over();
}

void Elf::on_Delete_elf_clicked()
{
    delete_user_elf(All_Elfs[selected_elf].elf_id);
    qDebug() << selected_elf;
    display_ALLELF();
}

