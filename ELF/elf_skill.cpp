#include "elf_skill.h"
#include <random>

elf_skill::elf_skill(QString name, double dmg, double cd, Attribute attr, Buff buff, bool tgt, double buff_rate, QString desc):
    skill_name(name),
    skill_tgt(tgt),
    damage(dmg),
    cooldown(cd),
    skill_attribute(attr),
    skill_buff(buff),
    skill_buff_rate(buff_rate),
    description(desc)
{
    // 如果需要，添加构造函数体
}

QString elf_skill::SkillName() const { return skill_name; }
double elf_skill::DMG() const { return damage; }
double elf_skill::CD() const { return cooldown; }
Attribute elf_skill::Attr() const { return skill_attribute; }
Buff elf_skill::buff() const { return skill_buff; }
bool elf_skill::TGT() const { return skill_tgt; }
double elf_skill::buff_rate() const { return skill_buff_rate; }

bool elf_skill::Rate_Result() const{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    double random_number = dis(gen);
    return random_number < skill_buff_rate;
}

QString elf_skill::Desc() const{
    return description;
}
