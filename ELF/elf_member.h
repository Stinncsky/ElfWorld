#ifndef ELF_MEMBER_H
#define ELF_MEMBER_H

#include "buff.h"
#include "elf_skill.h"
#include <string>
#include <map>
#include <set>
#include <random>
#include <QString>
#include <math.h>

#define MAX_LEVEL 15
#define MAX_SKILL_NUM 4

#define INIT_HP 200
#define INIT_ATK 20
#define INIT_DEF 10
#define INIT_ATK_INTERVAL 1.0
#define INIT_CRIT_RATE 0.02
#define INIT_AGI 0.02
#define INIT_UP_HP 1.2
#define INIT_UP_ATK 1.10
#define INIT_UP_DEF 1.15
#define INIT_UP_ATK_INTERVAL 1
#define INIT_UP_CRIT_RATE 1
#define INIT_UP_AGI 1


enum class Exp {
    Level1 = 0,
    Level2 = 100,
    Level3 = 300,
    Level4 = 600,
    Level5 = 1000,
    Level6 = 1500,
    Level7 = 2100,
    Level8 = 2800,
    Level9 = 3600,
    Level10 = 4500,
    Level11 = 5500,
    Level12 = 6600,
    Level13 = 7800,
    Level14 = 9100
};


class elf_member{
public:
    //elf_member(QString name, int level, Attribute attr, int experience);
    elf_member(QString name = "", int level = 1, int experience = 0, Attribute attr = Attribute::无,
           double hp = INIT_HP, double atk = INIT_ATK, double def = INIT_DEF, double atk_interval = INIT_ATK_INTERVAL,
           double crit_rate = INIT_CRIT_RATE, double agi = INIT_AGI,
           double up_hp = INIT_UP_HP, double up_atk = INIT_UP_ATK, double up_def = INIT_UP_DEF,
           double up_atk_interval = INIT_UP_ATK_INTERVAL, double up_crit_rate = INIT_UP_CRIT_RATE, double up_agi = INIT_UP_AGI);

    QString Elf_name() const;
    int Elf_level() const;
    Attribute Elf_attr() const;
    double Atk() const;
    double Def() const;
    double Hp() const;
    double Atk_INTERVAL() const;
    double Crit_Rate() const;
    double Agi() const;
    double Up_ATK() const;
    double Up_DEF() const;
    double Up_HP() const;
    double Up_ATK_INTERVAL() const;
    double Up_CRIT_Rate() const;
    double Up_AGI() const;
    double Fting_ATK() const;
    double Fting_DEF() const;
    double Fting_HP() const;
    double Fting_ATK_INTERVAL() const;
    double Fting_CRIT_Rate() const;
    double Fting_AGI() const;
    int Exp() const;
    const elf_skill* Elf_skills() const;

    void add_skill(const elf_skill&);
    virtual QString normal_ATK(elf_member &enemy);
    virtual QString release_skill(elf_member &enemy, const int sid);
    virtual QString buff_inspire(const Buff);
    void start_battle();
    void buff_erase(const Buff);
    void buff_to_HP();
    void level_up();

protected:
    QString elf_name;
    int elf_level, exp;
    Attribute elf_attr;
    double HP, ATK, DEF, ATK_INTERVAL, CRIT_Rate, AGI;
    double up_HP, up_ATK, up_DEF, up_ATK_INTERVAL, up_CRIT_Rate, up_AGI;
    double fting_HP, fting_ATK, fting_DEF, fting_ATK_INTERVAL, fting_CRIT_Rate, fting_AGI;
    std::set<Buff> elf_state;
    elf_skill elf_skills[MAX_SKILL_NUM];
    std::vector<double> state_HP;

    bool getProbabilityResult(const double p);
    void skill_effect(const elf_skill, const double ATK);
};

class elf_power : public elf_member
{
public:
    //elf_power(QString name, int level, int experience, Attribute attr);//基础力量型
    /*elf_power(QString name, int level, int experience, Attribute attr,//定制力量型
              double atk, double up_atk, double hp, double up_hp, 
              double atk_interval, double up_atk_interval);*/
    elf_power(QString name = "", int level = 1, int experience = 0, Attribute attr = Attribute::无,//
              double hp = INIT_HP * 0.75, double atk = INIT_ATK * 1.5, double def = INIT_DEF,
              double atk_interval = INIT_ATK_INTERVAL, double crit_rate = INIT_CRIT_RATE * 0.5, double agi = INIT_AGI * 0,
              double up_hp = INIT_UP_HP, double up_atk = INIT_UP_ATK * 1.2, double up_def = INIT_UP_DEF,
              double up_atk_interval = INIT_UP_ATK_INTERVAL, double up_crit_rate = INIT_UP_CRIT_RATE, double up_agi = INIT_UP_AGI);
    //~elf_power();
};

class elf_tank : public elf_member
{
public:
    elf_tank(QString name, int level, int experience, Attribute attr);//基础坦克型
    elf_tank(QString name, int level, int experience, Attribute attr,//定制坦克型
             double atk, double up_atk, double hp, double up_hp, 
             double atk_interval, double up_atk_interval);
    elf_tank(QString name = "", int level = 1, int experience = 0, Attribute attr = Attribute::无,//
             double hp = INIT_HP * 1.5, double atk = INIT_ATK * 0.75, double def = INIT_DEF * 1.5,
             double atk_interval = INIT_ATK_INTERVAL, double crit_rate = INIT_CRIT_RATE * 0.5, double agi = INIT_AGI * 0,
             double up_hp = INIT_UP_HP, double up_atk = INIT_UP_ATK, double up_def = INIT_UP_DEF * 1.2,
             double up_atk_interval = INIT_UP_ATK_INTERVAL, double up_crit_rate = INIT_UP_CRIT_RATE, double up_agi = INIT_UP_AGI);
    ~elf_tank();
};

class elf_shield : public elf_member
{
public:
    elf_shield(QString name, int level, int experience, Attribute attr);//基础护盾型
    elf_shield(QString name, int level, int experience, Attribute attr,//定制护盾型
               double atk, double up_atk, double hp, double up_hp, 
               double atk_interval, double up_atk_interval);
    elf_shield(QString name = "", int level = 1, int experience = 0, Attribute attr = Attribute::无,//
               double hp = INIT_HP * 1.2, double atk = INIT_ATK, double def = INIT_DEF * 1.2,
               double atk_interval = INIT_ATK_INTERVAL, double crit_rate = INIT_CRIT_RATE * 0.5, double agi = INIT_AGI * 0,
               double up_hp = INIT_UP_HP, double up_atk = INIT_UP_ATK, double up_def = INIT_UP_DEF * 1.2,
               double up_atk_interval = INIT_UP_ATK_INTERVAL, double up_crit_rate = INIT_UP_CRIT_RATE, double up_agi = INIT_UP_AGI);
    ~elf_shield();
};

class elf_speed : public elf_member
{
public:
    elf_speed(QString name, int level, int experience, Attribute attr);//基础速度型
    elf_speed(QString name, int level, int experience, Attribute attr,//定制速度型
              double atk, double up_atk, double hp, double up_hp, 
              double atk_interval, double up_atk_interval);
    elf_speed(QString name = "", int level = 1, int experience = 0, Attribute attr = Attribute::无,//
              double hp = INIT_HP * 0.8, double atk = INIT_ATK * 0.8, double def = INIT_DEF * 0.8,
              double atk_interval = INIT_ATK_INTERVAL * 1.5, double crit_rate = INIT_CRIT_RATE * 0.5, double agi = INIT_AGI * 1.5,
              double up_hp = INIT_UP_HP, double up_atk = INIT_UP_ATK, double up_def = INIT_UP_DEF,
              double up_atk_interval = INIT_UP_ATK_INTERVAL * 1.2, double up_crit_rate = INIT_UP_CRIT_RATE, double up_agi = INIT_UP_AGI * 1.2);
    ~elf_speed();
};

enum class Elfs{
    导弹毛
};

extern elf_member elfs_normal[];
extern std::map<QString, QString> Elfs_List;
#endif // ELF_MEMBER_H
