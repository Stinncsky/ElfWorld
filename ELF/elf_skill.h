#ifndef ELF_SKILL_H
#define ELF_SKILL_H

#include "buff.h"
#include <QString>

class elf_skill{
public:
    elf_skill(QString name = "", double dmg = 0, double cd = 0, Attribute attr = Attribute::无, Buff buff = Buff(), bool tgt = 0, double buff_rate = 0);
    //~elf_skill();
    QString SkillName() const; // 获取技能名称
    double DMG() const; // 获取技能伤害
    double CD() const; // 获取技能冷却时间
    Attribute Attr() const; // 获取技能属性
    Buff buff() const; // 获取技能附加状态
    bool TGT() const;
    double buff_rate() const;
    bool Rate_Result() const;
private:
    QString skill_name; // 技能名称
    bool skill_tgt;
    double damage; // 技能伤害
    double cooldown; // 技能冷却时间
    Attribute skill_attribute; // 技能属性
    Buff skill_buff;
    double skill_buff_rate;
};

enum class SkillName {
    生机,
    毒药,
    打火机,
    冰箱,
    埋土里,
    麻痹,
    手术刀,
    枯萎,
    激素,
    强健,
    风之庇佑,
    金钟罩,
    石化,
    森林,
    水泡,
    光合作用,
    川流不息,
    生生不息,
    大地之母的庇佑,
    金汤肥牛,
    雷池,
    沼泽,
    漩涡,
    蓝银缠绕,
    火之翼,
    水之翼,
    金之翼,
    电之翼,
    风之翼,
    爆炸,
    冰冻,
    千钧,
    雷击,
    陨石,
    中毒,
    虚弱,
    流血,
    风刃,
    蓄意轰拳
};

extern elf_skill skills[];
extern std::map<QString, int> skillNameMap;
#endif // ELF_SKILL_H
