#include "buff.h"

Buff::Buff(QString name,double HP, double atk, double def, double agi, double a_i, double crit, double dur, int level):
    buff_name(name),
    level(level),
    Dur(dur),
    HP_pers(HP * level),
    ATK(atk * level),
    DEF(def * level),
    AGI(agi * level),
    ATK_INTERVAL(a_i * level),
    CRIT_Rate(crit * level)

{
    // 如果需要，添加构造函数体
}

QString Buff::Name() const { return buff_name; }
double Buff::Hp() const { return HP_pers; }
double Buff::Atk() const { return ATK; }
double Buff::Def() const { return DEF; }
double Buff::Agi() const { return AGI; }
double Buff::A_i() const { return ATK_INTERVAL; }
double Buff::Crit() const { return CRIT_Rate; }
int Buff::Level() const { return level; }
bool Buff::Up_or_down() const { return up_or_down; }

bool Buff::operator==(const Buff& other) const {
    return buff_name == other.buff_name; // 比较 buff_name 是否相等
}

/*void Buff::operator=(const Buff& other) {
    buff_name = other.buff_name;
    level = other.level;
    Dur = other.Dur;
    HP_pers = other.HP_pers;
    ATK = other.ATK;
    DEF = other.DEF;
    AGI = other.AGI;
    ATK_INTERVAL = other.ATK_INTERVAL;
    CRIT_Rate = other.CRIT_Rate;
}*/

bool Buff::operator<(const Buff& other) const {
    return buff_name < other.buff_name;
}

bool Buff::is_levelup() const{
    if (level < MAX_BUFF_LEVEL)
        return 1;
    else
        return 0;
}

void Buff::levelup() {
    // 增加等级
    level++;

    // 将所有属性值提升为当前值的 1.5 倍
    HP_pers *= 1.5;
    ATK *= 1.5;
    DEF *= 1.5;
    AGI *= 1.5;
    ATK_INTERVAL *= 1.5;
    CRIT_Rate *= 1.5;
}
