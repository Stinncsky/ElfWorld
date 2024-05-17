#ifndef ELF_SKILL_H
#define ELF_SKILL_H

#include "buff.h"
#include "attribute.h"
#include <QString>

class elf_skill{
public:
    elf_skill(QString name = "", double dmg = 0, double cd = 0, Attribute attr = {""}, Buff buff = Buff(), bool tgt = 0, double buff_rate = 0, QString desc = "");
    //~elf_skill();
    QString SkillName() const; // 获取技能名称
    double DMG() const; // 获取技能伤害
    double CD() const; // 获取技能冷却时间
    Attribute Attr() const; // 获取技能属性
    Buff buff() const; // 获取技能附加状态
    bool TGT() const;
    double buff_rate() const;
    bool Rate_Result() const;
    QString Desc() const;
private:
    QString skill_name; // 技能名称
    bool skill_tgt;
    double damage; // 技能伤害
    double cooldown; // 技能冷却时间
    Attribute skill_attribute; // 技能属性
    Buff skill_buff;
    double skill_buff_rate;
    QString description;
};


#endif // ELF_SKILL_H
