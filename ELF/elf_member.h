#ifndef ELF_MEMBER_H
#define ELF_MEMBER_H

#include "buff.h"
#include "elf_skill.h"
#include <string>
#include <map>
#include <set>
#include <random>
#include <QString>
#include <QList>
#include <math.h>

#define MAX_LEVEL 15
#define MAX_SKILL_NUM 4

#define INIT_HP 2000
#define INIT_ATK 30
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

class elf_member{
public:
    int elf_id;
    //elf_member(QString name, int level, Attribute attr, int experience);
    elf_member(QString name = "", int level = 1, int experience = 0, Attribute attr = Attribute(),
           double hp = INIT_HP, double atk = INIT_ATK, double def = INIT_DEF, double atk_interval = INIT_ATK_INTERVAL,
           double crit_rate = INIT_CRIT_RATE, double agi = INIT_AGI,
           double up_hp = INIT_UP_HP, double up_atk = INIT_UP_ATK, double up_def = INIT_UP_DEF,
           double up_atk_interval = INIT_UP_ATK_INTERVAL, double up_crit_rate = INIT_UP_CRIT_RATE, double up_agi = INIT_UP_AGI);
    elf_member(const elf_member& other);

    elf_member& operator=(const elf_member& other);

    QString Elf_name() const;
    int Elf_level() const;
    Attribute Elf_attr() const;
    int Elf_id() const;
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
    int Up_Exp() const;
    const elf_skill* Elf_skills() const;

    void add_skill(const elf_skill&);
    virtual QString normal_ATK(elf_member &enemy);
    virtual QString release_skill(elf_member &enemy, const int sid);
    virtual QString buff_inspire(const Buff);
    void start_battle();
    void buff_erase(const Buff);
    QStringList buff_to_HP();
    void get_exp(const int);

private:
    QString elf_name;
    int elf_level, exp;
    Attribute elf_attr;
    double HP, ATK, DEF, ATK_INTERVAL, CRIT_Rate, AGI;
    double up_HP, up_ATK, up_DEF, up_ATK_INTERVAL, up_CRIT_Rate, up_AGI;
    double fting_HP = HP, fting_ATK, fting_DEF, fting_ATK_INTERVAL, fting_CRIT_Rate, fting_AGI;
    std::set<Buff> elf_state;
    elf_skill elf_skills[MAX_SKILL_NUM];
    std::vector<std::pair<double, QString>> state_HP;
    const std::map<std::pair<Attribute, Attribute>, AttributeRelationship> attributeRelations = {
        {{Attribute("火"), Attribute("木")}, AttributeRelationship::STRONG},  // 火克制木
        {{Attribute("木"), Attribute("水")}, AttributeRelationship::STRONG},  // 木克制水
        {{Attribute("水"), Attribute("火")}, AttributeRelationship::STRONG},  // 水克制火
        {{Attribute("土"), Attribute("火")}, AttributeRelationship::STRONG},  // 土克制火
        {{Attribute("土"), Attribute("电")}, AttributeRelationship::STRONG},  // 土克制电
        {{Attribute("金"), Attribute("木")}, AttributeRelationship::STRONG},  // 金克制木
        {{Attribute("电"), Attribute("水")}, AttributeRelationship::STRONG},
        // 添加相反的关系
        {{Attribute("木"), Attribute("火")}, AttributeRelationship::WEAK},    // 木被火克制
        {{Attribute("水"), Attribute("木")}, AttributeRelationship::WEAK},    // 水被木克制
        {{Attribute("火"), Attribute("水")}, AttributeRelationship::WEAK},    // 火被水克制
        {{Attribute("火"), Attribute("土")}, AttributeRelationship::WEAK},    // 火被土克制
        {{Attribute("电"), Attribute("土")}, AttributeRelationship::WEAK},    // 电被土克制
        {{Attribute("电"), Attribute("金")}, AttributeRelationship::WEAK},    // 电被金克制
        {{Attribute("木"), Attribute("金")}, AttributeRelationship::WEAK}     // 木被金克制
    };
    bool getProbabilityResult(const double p);
    QString skill_effect(const elf_skill, const double ATK,const bool tgt);
    void level_up();
};

class elf_power : public elf_member
{
public:
    //elf_power(QString name, int level, int experience, Attribute attr);//基础力量型
    /*elf_power(QString name, int level, int experience, Attribute attr,//定制力量型
              double atk, double up_atk, double hp, double up_hp, 
              double atk_interval, double up_atk_interval);*/
    elf_power(QString name = "", int level = 1, int experience = 0, Attribute attr = Attribute(),//
              double hp = INIT_HP * 0.75, double atk = INIT_ATK * 1.5, double def = INIT_DEF,
              double atk_interval = INIT_ATK_INTERVAL, double crit_rate = INIT_CRIT_RATE * 0.5, double agi = INIT_AGI * 0,
              double up_hp = INIT_UP_HP, double up_atk = INIT_UP_ATK * 1.2, double up_def = INIT_UP_DEF,
              double up_atk_interval = INIT_UP_ATK_INTERVAL, double up_crit_rate = INIT_UP_CRIT_RATE, double up_agi = INIT_UP_AGI);
    //~elf_power();
};

class elf_tank : public elf_member
{
public:
    elf_tank(QString name = "", int level = 1, int experience = 0, Attribute attr = Attribute(),//
             double hp = INIT_HP * 1.5, double atk = INIT_ATK * 0.75, double def = INIT_DEF * 1.5,
             double atk_interval = INIT_ATK_INTERVAL, double crit_rate = INIT_CRIT_RATE * 0.5, double agi = INIT_AGI * 0,
             double up_hp = INIT_UP_HP, double up_atk = INIT_UP_ATK, double up_def = INIT_UP_DEF * 1.2,
             double up_atk_interval = INIT_UP_ATK_INTERVAL, double up_crit_rate = INIT_UP_CRIT_RATE, double up_agi = INIT_UP_AGI);
    //~elf_tank();
};

class elf_shield : public elf_member
{
public:
    elf_shield(QString name = "", int level = 1, int experience = 0, Attribute attr = Attribute(),//
               double hp = INIT_HP * 1.2, double atk = INIT_ATK, double def = INIT_DEF * 1.2,
               double atk_interval = INIT_ATK_INTERVAL, double crit_rate = INIT_CRIT_RATE * 0.5, double agi = INIT_AGI * 0,
               double up_hp = INIT_UP_HP, double up_atk = INIT_UP_ATK, double up_def = INIT_UP_DEF * 1.2,
               double up_atk_interval = INIT_UP_ATK_INTERVAL, double up_crit_rate = INIT_UP_CRIT_RATE, double up_agi = INIT_UP_AGI);
    ~elf_shield();
};

class elf_speed : public elf_member
{
public:
    elf_speed(QString name = "", int level = 1, int experience = 0, Attribute attr = Attribute(),//
              double hp = INIT_HP * 0.8, double atk = INIT_ATK * 0.8, double def = INIT_DEF * 0.8,
              double atk_interval = INIT_ATK_INTERVAL * 1.5, double crit_rate = INIT_CRIT_RATE * 0.5, double agi = INIT_AGI * 1.5,
              double up_hp = INIT_UP_HP, double up_atk = INIT_UP_ATK, double up_def = INIT_UP_DEF,
              double up_atk_interval = INIT_UP_ATK_INTERVAL * 1.2, double up_crit_rate = INIT_UP_CRIT_RATE, double up_agi = INIT_UP_AGI * 1.2);
    ~elf_speed();
};

#endif // ELF_MEMBER_H
