#ifndef BUFF_H
#define BUFF_H
#include <QString>
#include <map>

#define MAX_BUFF_LEVEL 5
#define MAX_BATTLE_TIME 10 * 60


class Buff{
public:
    Buff(QString Name = "", double Hp = 0, double Atk = 0, double Def = 0, double Agi = 0, double A_i = 0, double Crit = 0, double Dur = 0, int Level = 1);
    //~Buff();

    QString Name() const;
    double Hp() const;
    double Atk() const;
    double Def() const;
    double Agi() const;
    double A_i() const;
    double Crit() const;
    int Level() const;
    bool Up_or_down() const;

    bool is_levelup() const;
    void levelup();
    bool operator==(const Buff& other) const;
    bool operator<(const Buff& other) const;
private:
    QString buff_name;
    int level;
    double Dur;
    double HP_pers, ATK, DEF, AGI, ATK_INTERVAL, CRIT_Rate;
    bool up_or_down;
};


#endif
