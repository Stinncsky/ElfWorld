#include "elf_member.h"

/*elf_member::elf_member(QString name, int level, Attribute attr, int experience)
    : elf_name(name), elf_level(level), exp(experience), elf_attr(attr),
    HP(INIT_HP * std::pow(INIT_UP_HP,level-1)), ATK(INIT_ATK * std::pow(INIT_UP_ATK,level-1)), DEF(INIT_DEF * std::pow(INIT_UP_DEF,level-1)), ATK_INTERVAL(INIT_ATK_INTERVAL * std::pow(INIT_UP_ATK_INTERVAL,level-1)),
    CRIT_Rate(INIT_CRIT_RATE * std::pow(INIT_UP_CRIT_RATE,level-1)), AGI(INIT_AGI * std::pow(INIT_UP_AGI,level-1)),
    up_HP(INIT_UP_HP), up_ATK(INIT_UP_ATK), up_DEF(INIT_UP_DEF), 
    up_ATK_INTERVAL(INIT_UP_ATK_INTERVAL), up_CRIT_Rate(INIT_UP_CRIT_RATE), up_AGI(INIT_UP_AGI) 
{
    // 其他初始化代码...
}*/

elf_member::elf_member(QString name, int level, int experience, Attribute attr,
                                             double hp, double atk, double def, double atk_interval, 
                                             double crit_rate, double agi, 
                                             double up_hp, double up_atk, double up_def, 
                                             double up_atk_interval, double up_crit_rate, double up_agi)
        : elf_name(name), elf_level(level), exp(experience), elf_attr(attr),
    HP(hp * std::pow(up_hp,level-1)), ATK(atk * std::pow(up_atk,level-1)), DEF(def * std::pow(up_def,level-1)), ATK_INTERVAL(atk_interval * std::pow(up_atk_interval,level-1)),
            CRIT_Rate(crit_rate * std::pow(up_crit_rate,level-1)), AGI(agi * std::pow(up_agi,level-1)),
            up_HP(up_hp), up_ATK(up_atk), up_DEF(up_def), 
            up_ATK_INTERVAL(up_atk_interval), up_CRIT_Rate(up_crit_rate), up_AGI(up_agi) 
{
    // 其他初始化代码...
}



QString elf_member::Elf_name() const { return elf_name; }
int elf_member::Elf_level() const { return elf_level; }
Attribute elf_member::Elf_attr() const { return elf_attr; }
double elf_member::Atk() const { return ATK; }
double elf_member::Def() const { return DEF; }
double elf_member::Hp() const { return HP; }
double elf_member::Atk_INTERVAL() const { return ATK_INTERVAL; }
double elf_member::Crit_Rate() const  { return CRIT_Rate; }
double elf_member::Agi() const { return AGI; }
double elf_member::Up_ATK() const { return up_ATK; }
double elf_member::Up_DEF() const { return up_DEF; }
double elf_member::Up_HP() const { return up_HP; }
double elf_member::Up_ATK_INTERVAL() const { return up_ATK_INTERVAL; }
double elf_member::Up_CRIT_Rate() const { return up_CRIT_Rate; }
double elf_member::Up_AGI() const { return up_AGI; }
double elf_member::Fting_ATK() const { return fting_ATK; }
double elf_member::Fting_DEF() const { return fting_DEF; }
double elf_member::Fting_HP() const { return fting_HP; }
double elf_member::Fting_ATK_INTERVAL() const { return fting_ATK_INTERVAL; }
double elf_member::Fting_CRIT_Rate() const { return fting_CRIT_Rate; }
double elf_member::Fting_AGI() const { return fting_AGI; }
int elf_member::Exp() const { return exp; }
const elf_skill* elf_member::Elf_skills() const { return elf_skills; }



bool elf_member::getProbabilityResult(const double p){
    std::random_device rd; // 使用随机设备作为种子
    std::mt19937 gen(rd()); // 选择 Mersenne Twister 引擎
    std::uniform_real_distribution<> dis(0, 1); // 均匀分布，范围为[0,1)

    if (dis(gen) < p) {
        return 1;
    } else {
        return 0;
    }
}

void elf_member::add_skill(const elf_skill& skill){
    for (auto &s:elf_skills){
        if(s.SkillName() == ""){
            s = skill;
            return;
        }
    }
}

QString elf_member::normal_ATK(elf_member &enemy){
    double ATK = fting_ATK, E_DEF = enemy.fting_DEF;
    auto attr = attributeRelations.find({elf_attr, enemy.elf_attr});
    if (attr != attributeRelations.end()) {
        if (attr->second == AttributeRelationship::STRONG) {
            ATK *= 1.3;
        } else if (attr->second == AttributeRelationship::WEAK) {
            ATK *= 0.85;
        }
    }
    double DMG = ATK-E_DEF;
    DMG = std::max(0.0, DMG);
    if (getProbabilityResult(fting_CRIT_Rate)){
        DMG *= 2.0;
    }
    if (!getProbabilityResult(enemy.fting_AGI)){
        enemy.fting_HP -= DMG;
        return elf_name + "造成了成吨的伤害！";
    } else {
        return enemy.elf_name + "用矫健的步伐闪过了这次攻击！";
    }
}

void elf_member::level_up(){
    elf_level++;
    HP *= up_HP;
    ATK *= up_ATK;
    DEF *= up_DEF;
    ATK_INTERVAL *= up_ATK_INTERVAL;
    AGI *= up_AGI;
    CRIT_Rate *= up_CRIT_Rate;
}

void elf_member::start_battle(){
    fting_HP = HP;
    fting_ATK = ATK;
    fting_DEF = DEF;
    fting_ATK_INTERVAL = ATK_INTERVAL;
    fting_CRIT_Rate = CRIT_Rate;
    fting_AGI = AGI;
    for (auto &i : elf_state) {
        buff_erase(i);
    }
}

void elf_member::skill_effect(const elf_skill skill, const double ATK){
    double DMG = skill.DMG() * ATK / 100.0;
    auto attr = attributeRelations.find({skill.Attr(), elf_attr});
    if (attr != attributeRelations.end()) {
        if (attr->second == AttributeRelationship::STRONG) {
            DMG *= 1.6;
        } else if (attr->second == AttributeRelationship::WEAK) {
            DMG *= 0.67;
        }
    }
    fting_HP -= DMG;
    if (!skill.Rate_Result())
        return;
    auto it = elf_state.find(skill.buff());
    if (it != elf_state.end()) {
        // 键存在于set中
        // 这里你可以使用it来访问键所关联的值
        Buff oldBuff = *it; // 获取旧的 Buff 对象
        elf_state.erase(it); // 从set中删除旧的元素
        buff_erase(oldBuff);
        // 对旧的 Buff 对象进行修改
        oldBuff.levelup();

        // 将修改后的 Buff 对象插入回set中
        elf_state.insert(oldBuff);
        buff_inspire(oldBuff);
    } else {
        // 键不存在于set中
        elf_state.insert(skill.buff());
        buff_inspire(skill.buff());
    }
}

// 更改对buff函数的调用
QString elf_member::buff_inspire(const Buff buff){
    if (buff.Hp() != 0) {
        state_HP.emplace_back(buff.Hp());
    }
    fting_AGI = std::max(fting_AGI + buff.Agi(), 0.0);
    fting_ATK = std::max(fting_ATK + buff.Atk(), 0.0);
    fting_DEF = std::max(fting_DEF + buff.Def(), 0.0);
    fting_ATK_INTERVAL = std::max(fting_ATK_INTERVAL + buff.A_i(), 0.0);
    fting_CRIT_Rate = std::max(fting_CRIT_Rate + buff.Crit(), 0.0);
    return elf_name + "获得了" + buff.Name();
}

void elf_member::buff_erase(const Buff buff){
    if (buff.Hp() != 0) {
        state_HP.erase(std::find(state_HP.begin(), state_HP.end(), buff.Hp()));
    }
    fting_AGI = std::max(fting_AGI - buff.Agi(), 0.0);
    fting_ATK = std::max(fting_ATK - buff.Atk(), 0.0);
    fting_DEF = std::max(fting_DEF - buff.Def(), 0.0);
    fting_ATK_INTERVAL = std::max(fting_ATK_INTERVAL - buff.A_i(), 0.0);
    fting_CRIT_Rate = std::max(fting_CRIT_Rate - buff.Crit(), 0.0);
}

void elf_member::buff_to_HP(){
    for (auto &i : state_HP) {
        fting_HP += i;
    }
}

QString elf_member::release_skill(elf_member &enemy, int sid){
    if(elf_skills[sid].TGT()){
        enemy.skill_effect(elf_skills[sid], ATK);
    } else {
        skill_effect(elf_skills[sid], ATK);
    }
}

/*elf_power::elf_power(QString name, int level, int experience, Attribute attr)
    : elf_member(name, level, experience, attr)
{
    HP -= 50;
    ATK *= (pow(1.2, level - 1) / pow(1.1, level - 1));
    up_ATK = 1.2;
    ATK_INTERVAL *= 2.0;
}

elf_power::elf_power(QString name, int level, int experience, Attribute attr,
                     double atk, double up_atk, double hp, double up_hp, double atk_interval, double up_atk_interval)
    : elf_member(name, level, experience, attr,
                 hp, atk, INIT_DEF, atk_interval, INIT_CRIT_RATE * 0.5, INIT_AGI * 0, 
                 up_hp, up_atk, INIT_UP_DEF, up_atk_interval, INIT_UP_CRIT_RATE, INIT_UP_AGI)
{

}*/

elf_power::elf_power(QString name, int level, int experience, Attribute attr,
                     double hp, double atk, double def, double atk_interval,
                     double crit_rate, double agi,
                     double up_hp, double up_atk, double up_def,
                     double up_atk_interval, double up_crit_rate, double up_agi)
    : elf_member(name, level, experience, attr, hp, atk, def, atk_interval, crit_rate, agi, up_hp, up_atk, up_def, up_atk_interval, up_crit_rate, up_agi)
{

}

elf_tank::elf_tank(QString name, int level, int experience, Attribute attr)
    : elf_member(name, level, experience, attr)
{
    HP *= 1.5;
    DEF *= 1.5;
    up_DEF = 1.5;
    ATK_INTERVAL *= 0.5;
}

elf_tank::elf_tank(QString name, int level, int experience, Attribute attr,
                   double atk, double up_atk, double hp, double up_hp, double atk_interval, double up_atk_interval)
    : elf_member(name, level, experience, attr,
                 hp, atk, INIT_DEF, atk_interval, INIT_CRIT_RATE * 0.5, INIT_AGI * 0, 
                 up_hp, up_atk, INIT_UP_DEF, up_atk_interval, INIT_UP_CRIT_RATE, INIT_UP_AGI)
{

}

elf_tank::elf_tank(QString name, int level, int experience, Attribute attr,
                   double hp, double atk, double def, double atk_interval,
                   double crit_rate, double agi,
                   double up_hp, double up_atk, double up_def,
                   double up_atk_interval, double up_crit_rate, double up_agi)
    : elf_member(name, level, experience, attr, hp, atk, def, atk_interval, crit_rate, agi, up_hp, up_atk, up_def, up_atk_interval, up_crit_rate, up_agi)
{

}

elf_shield::elf_shield(QString name, int level, int experience, Attribute attr)
    : elf_member(name, level, experience, attr)
{
    HP *= 1.2;
    DEF *= 1.2;
    up_DEF = 1.2;
    ATK_INTERVAL *= 0.5;
}

elf_shield::elf_shield(QString name, int level, int experience, Attribute attr,
                       double atk, double up_atk, double hp, double up_hp, double atk_interval, double up_atk_interval)
    : elf_member(name, level, experience, attr,
                 hp, atk, INIT_DEF, atk_interval, INIT_CRIT_RATE * 0.5, INIT_AGI * 0, 
                 up_hp, up_atk, INIT_UP_DEF, up_atk_interval, INIT_UP_CRIT_RATE, INIT_UP_AGI)
{

}

elf_shield::elf_shield(QString name, int level, int experience, Attribute attr,
                       double hp, double atk, double def, double atk_interval,
                       double crit_rate, double agi,
                       double up_hp, double up_atk, double up_def,
                       double up_atk_interval, double up_crit_rate, double up_agi)
    : elf_member(name, level, experience, attr, hp, atk, def, atk_interval, crit_rate, agi, up_hp, up_atk, up_def, up_atk_interval, up_crit_rate, up_agi)
{

}

elf_speed::elf_speed(QString name, int level, int experience, Attribute attr)
    : elf_member(name, level, experience, attr)
{
    HP *= 0.8;
    DEF *= 0.8;
    AGI *= 1.5;
    up_AGI = 1.5;
    ATK_INTERVAL *= 1.5;
}

elf_speed::elf_speed(QString name, int level, int experience, Attribute attr,
                     double atk, double up_atk, double hp, double up_hp, double atk_interval, double up_atk_interval)
    : elf_member(name, level, experience, attr,
                 hp, atk, INIT_DEF, atk_interval, INIT_CRIT_RATE * 0.5, INIT_AGI * 0, 
                 up_hp, up_atk, INIT_UP_DEF, up_atk_interval, INIT_UP_CRIT_RATE, INIT_UP_AGI)
{

}

elf_speed::elf_speed(QString name, int level, int experience, Attribute attr,
                     double hp, double atk, double def, double atk_interval,
                     double crit_rate, double agi,
                     double up_hp, double up_atk, double up_def,
                     double up_atk_interval, double up_crit_rate, double up_agi)
    : elf_member(name, level, experience, attr, hp, atk, def, atk_interval, crit_rate, agi, up_hp, up_atk, up_def, up_atk_interval, up_crit_rate, up_agi)
{

}
