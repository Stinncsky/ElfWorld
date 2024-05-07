#ifndef ELF_H
#define ELF_H

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

enum event{
    LOGIN,
    REGISTE,
    NEWELF,
    GET_USER_INFO,
    SEND_USER_INFO,
    NO_EVENT
};

#define NEW_USER_ELF_NUM 6
#define ALL_ELF_NUM 3

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

    void on_tabWidget_tabBarClicked(int index);

private:
    Ui::Elf *ui;
    QString cur_user;

    QPropertyAnimation *animation;
    QStandardItemModel *model;

    QTcpSocket *socket;
    QString IP;
    int port;

    int My_Bag[6];
    std::vector<elf_member> All_Elfs;

    elf_member get_elf(const std::vector<QString>);
    elf_member random_init_elf();
    void Solo(elf_member first, elf_member second);

    void new_user();
    void user_elf_add(const elf_member&);
    void load_user_info();
    void load_user_elf(const QString&);

    void display_ALLELF();

    void readData();
    void SendData(const QString);
    void socket_disconnected();
    void remind(const QString content);
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
