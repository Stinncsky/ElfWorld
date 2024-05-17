#ifndef ELF_H
#define ELF_H

#include "qtableview.h"
#include <QWidget>
#include <QTcpSocket>
#include <QLabel>
#include <QPropertyAnimation>
#include <QStandardItemModel>
#include <QGraphicsItemAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <elf_member.h>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QPainter>
#include <QPainterPath>>

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



#define NEW_USER_ELF_NUM 6
#define ALL_ELF_NUM 4
#define BATTLE_LIST_ELF_NUM 15

QT_BEGIN_NAMESPACE
namespace Ui {
class Elf;
}
QT_END_NAMESPACE

class Elf : public QWidget
{
    Q_OBJECT

public:
    Elf(QWidget *parent = nullptr);
    ~Elf();

private slots:
    void on_exit_clicked();

    void on_rgst_clicked();

    void on_login_clicked();

    void on_re_login_clicked();

    void on_ALL_func_page_tabBarClicked(int index);

    void on_open_bag_clicked();

    void on_Exp_battle_start_clicked();

    void on_Exp_refresh_clicked();

    void on_Battle_refresh_clicked();

    void on_Battle_start_clicked();

    void on_Battle_exit_clicked();

    void on_Battle_escape_clicked();

    void on_Delete_elf_clicked();

private:
    Ui::Elf *ui;
    QString cur_user;
    int user_elf_id;

    QPropertyAnimation *animation;
    QStandardItemModel *Exp_model, *Battle_model, *Online_model;

    QTcpSocket *socket;
    QString IP;
    int port;

    int My_Bag[6];
    int selected_elf;
    std::vector<elf_member> All_Elfs, Exp_elfs, Battle_elfs;

    std::vector<Attribute> attributes;
    //std::map<std::pair<Attribute, Attribute>, AttributeRelationship> attributeRelations;
    std::vector<elf_member> elfs_normal;
    std::map<QString, QString> Elfs_List;
    std::vector<elf_skill> skills;
    std::map<QString, int> skillNameMap;
    std::vector<Buff> buffs;
    std::map<QString, int> buffsMap;
    void init();


    int round_time = 1000;
    elf_member get_elf(const std::vector<QString>);
    elf_member random_init_elf();
    //战斗所用变量
    elf_member *first, *second;
    int remainingTime, cur_min, cur_sec;
    QVector<QTimer*> Timers;
    QTimer interval_update;
    bool is_elf_solo;
    void Solo();
    void Solo_over();
    void select_Skill(elf_member&);
    void user_win();
    void user_lose();

    void new_user();
    QString elf_info_to_QString(const elf_member&);
    void user_elf_add(const elf_member&);
    void load_user_info();
    void load_user_elf(const QString&);
    void update_user_elf(const elf_member &elf, const int);
    void delete_user_elf(const int);

    void display_ALLELF();

    void readData();
    void SendData(const QString);
    void socket_disconnected();
    void remind(const QString content);

    void build_elf_list(QStandardItemModel*, QTableView*,std::vector<elf_member>&);
};


class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = nullptr) : QLabel(parent) {}

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override {
        emit clicked();
    }
};

#endif // ELF_H
