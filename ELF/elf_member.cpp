#include "elf_member.h"
std::vector<Attribute> attributes = {
    {"金"},
    {"木"},
    {"水"},
    {"火"},
    {"土"},
    {"电"}
};

elf_member::elf_member(QString name, int level, int experience, Attribute attr,
                                             double hp, double atk, double def, double atk_interval, 
                                             double crit_rate, double agi, 
                                             double up_hp, double up_atk, double up_def, 
                                             double up_atk_interval, double up_crit_rate, double up_agi)
    : elf_name(name), elf_level(level), exp(experience), elf_attr(attr),
    HP(hp), ATK(atk), DEF(def), ATK_INTERVAL(atk_interval),
            CRIT_Rate(crit_rate), AGI(agi),
            up_HP(up_hp), up_ATK(up_atk), up_DEF(up_def), 
            up_ATK_INTERVAL(up_atk_interval), up_CRIT_Rate(up_crit_rate), up_AGI(up_agi) 
{
    // 其他初始化代码...
}

elf_member::elf_member(const elf_member& other) :
    elf_id(other.elf_id),
    elf_name(other.elf_name),
    elf_level(other.elf_level),
    exp(other.exp),
    elf_attr(other.elf_attr),
    HP(other.HP), // 使用其他对象的 HP 值
    ATK(other.ATK),
    DEF(other.DEF),
    ATK_INTERVAL(other.ATK_INTERVAL),
    CRIT_Rate(other.CRIT_Rate),
    AGI(other.AGI),
    up_HP(other.up_HP),
    up_ATK(other.up_ATK),
    up_DEF(other.up_DEF),
    up_ATK_INTERVAL(other.up_ATK_INTERVAL),
    up_CRIT_Rate(other.up_CRIT_Rate),
    up_AGI(other.up_AGI)
{
    // 复制技能数组
    for (int i = 0; i < MAX_SKILL_NUM; ++i) {
        elf_skills[i] = other.elf_skills[i];
    }
}

elf_member& elf_member::operator=(const elf_member& other) {
    if (this != &other) { // 检查是否自赋值
        // 从 'other' 复制成员变量到 'this'
        elf_id = other.elf_id;
        elf_name = other.elf_name;
        elf_level = other.elf_level;
        exp = other.exp;
        elf_attr = other.elf_attr;
        HP = other.HP;
        ATK = other.ATK;
        DEF = other.DEF;
        ATK_INTERVAL = other.ATK_INTERVAL;
        CRIT_Rate = other.CRIT_Rate;
        AGI = other.AGI;
        up_HP = other.up_HP;
        up_ATK = other.up_ATK;
        up_DEF = other.up_DEF;
        up_ATK_INTERVAL = other.up_ATK_INTERVAL;
        up_CRIT_Rate = other.up_CRIT_Rate;
        up_AGI = other.up_AGI;
        // 复制 elf_skills 数组
        for (int i = 0; i < MAX_SKILL_NUM; ++i) {
            elf_skills[i] = other.elf_skills[i];
        }
    }
    return *this;
}

QString elf_member::Elf_name() const { return elf_name; }
int elf_member::Elf_level() const { return elf_level; }
Attribute elf_member::Elf_attr() const { return elf_attr; }
int elf_member::Elf_id() const { return elf_id; }
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
int elf_member::Up_Exp() const { return (elf_level + 1) * (elf_level) / 2 * 100; }
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
        enemy.fting_HP = std::max(enemy.fting_HP, 0.0);
        return elf_name + "对" + enemy.Elf_name() + "造成了" + "<font color=\"#800000\">" + QString::number(DMG) + "</font>" + "伤害";
    } else {
        return enemy.elf_name + "用矫健的步伐闪过了这次攻击！";
    }
}

void elf_member::get_exp(const int addition){
    exp += addition;
    level_up();
}

void elf_member::level_up(){
    while (exp >= Up_Exp() && elf_level < MAX_LEVEL){
        exp -= Up_Exp();
        elf_level++;
        HP *= up_HP;
        ATK *= up_ATK;
        DEF *= up_DEF;
        ATK_INTERVAL *= up_ATK_INTERVAL;
        AGI *= up_AGI;
        CRIT_Rate *= up_CRIT_Rate;
    }
}

void elf_member::start_battle(){
    fting_HP = HP;
    fting_ATK = ATK;
    fting_DEF = DEF;
    fting_ATK_INTERVAL = ATK_INTERVAL;
    fting_CRIT_Rate = CRIT_Rate;
    fting_AGI = AGI;
    elf_state.clear();
}

QString elf_member::skill_effect(const elf_skill skill, const double ATK, const bool tgt){
    QString info;
    if (tgt){
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
        fting_HP = std::max(fting_HP, 0.0);
        info += elf_name + "造成了" + "<font color=\"#800000\">" + QString::number(DMG) + "</font>" + "的伤害!";
    }
    if (!skill.Rate_Result())
        return info;
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
        info += "并让" + elf_name + "的<font color=\"#1788C0\">" +skill.buff().Name() + "</font>" + "效果提升!";
    } else {
        // 键不存在于set中
        elf_state.insert(skill.buff());
        buff_inspire(skill.buff());
        info += "对" + elf_name + "施加了" + "<font color=\"#1788C0\">" +skill.buff().Name() + "</font>" + "的效果";
    }
    return info;
}
// 更改对buff函数的调用
QString elf_member::buff_inspire(const Buff buff){
    if (buff.Hp() != 0) {
         state_HP.emplace_back(std::make_pair(buff.Hp(), buff.Name()));
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
        state_HP.erase(std::find(state_HP.begin(), state_HP.end(), std::make_pair(buff.Hp(), buff.Name())));
    }
    fting_AGI = std::max(fting_AGI - buff.Agi(), 0.0);
    fting_ATK = std::max(fting_ATK - buff.Atk(), 0.0);
    fting_DEF = std::max(fting_DEF - buff.Def(), 0.0);
    fting_ATK_INTERVAL = std::max(fting_ATK_INTERVAL - buff.A_i(), 0.0);
    fting_CRIT_Rate = std::max(fting_CRIT_Rate - buff.Crit(), 0.0);
}

QString elf_member::buff_to_HP(){
    QString info;
    for (auto &i : state_HP) {
        fting_HP += i.first;
        if (i.first > 0) {
            info += "<font color=\"#1788C0\">" +i.second + "</font>" + "回复了<font color=\"#17C082\">" + QString::number(i.first) + "</font>" + "生命值。\n";
        } else {
            info += "<font color=\"#1788C0\">" +i.second + "</font>" + "扣除了<font color=\"#800000\">" + QString::number(i.first) + "</font>" + "生命值。\n";
        }
    }
    fting_HP = std::max(fting_HP, 0.0);
    return info;
}

QString elf_member::release_skill(elf_member &enemy, int sid){
    if(elf_skills[sid].TGT()){
        return enemy.skill_effect(elf_skills[sid], ATK, 1);
    } else {
        return skill_effect(elf_skills[sid], ATK, 0);
    }
}

elf_power::elf_power(QString name, int level, int experience, Attribute attr,
                     double hp, double atk, double def, double atk_interval,
                     double crit_rate, double agi,
                     double up_hp, double up_atk, double up_def,
                     double up_atk_interval, double up_crit_rate, double up_agi)
    : elf_member(name, level, experience, attr, hp, atk, def, atk_interval, crit_rate, agi, up_hp, up_atk, up_def, up_atk_interval, up_crit_rate, up_agi)
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

elf_shield::elf_shield(QString name, int level, int experience, Attribute attr,
                       double hp, double atk, double def, double atk_interval,
                       double crit_rate, double agi,
                       double up_hp, double up_atk, double up_def,
                       double up_atk_interval, double up_crit_rate, double up_agi)
    : elf_member(name, level, experience, attr, hp, atk, def, atk_interval, crit_rate, agi, up_hp, up_atk, up_def, up_atk_interval, up_crit_rate, up_agi)
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



