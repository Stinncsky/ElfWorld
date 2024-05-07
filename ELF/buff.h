#ifndef BUFF_H
#define BUFF_H
#include <QString>
#include <map>

#define MAX_BUFF_LEVEL 5
#define MAX_BATTLE_TIME 10 * 60

enum class Attribute{
    金,
    木,
    水,
    火,
    土,
    电,
    无
};

enum class AttributeRelationship {
    WEAK,       // 弱
    NEUTRAL,    // 中立
    STRONG      // 强
};
const std::map<std::pair<Attribute, Attribute>, AttributeRelationship> attributeRelations({
    {{Attribute::火, Attribute::木}, AttributeRelationship::STRONG},  // 火克制木
    {{Attribute::木, Attribute::水}, AttributeRelationship::STRONG},  // 木克制水
    {{Attribute::水, Attribute::火}, AttributeRelationship::STRONG},  // 水克制火
    {{Attribute::土, Attribute::火}, AttributeRelationship::STRONG},  // 土克制火
    {{Attribute::土, Attribute::电}, AttributeRelationship::STRONG},  // 土克制电
    {{Attribute::金, Attribute::木}, AttributeRelationship::STRONG},  // 金克制木
    {{Attribute::电, Attribute::水}, AttributeRelationship::STRONG},
    // 添加相反的关系
    {{Attribute::木, Attribute::火}, AttributeRelationship::WEAK},    // 木被火克制
    {{Attribute::水, Attribute::木}, AttributeRelationship::WEAK},    // 水被木克制
    {{Attribute::火, Attribute::水}, AttributeRelationship::WEAK},    // 火被水克制
    {{Attribute::火, Attribute::土}, AttributeRelationship::WEAK},    // 火被土克制
    {{Attribute::电, Attribute::土}, AttributeRelationship::WEAK},    // 电被土克制
    {{Attribute::电, Attribute::金}, AttributeRelationship::WEAK},    // 电被金克制
    {{Attribute::木, Attribute::金}, AttributeRelationship::WEAK}     // 木被金克制
});
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
    //void operator=(const Buff& other);
    bool operator<(const Buff& other) const;
private:
    QString buff_name;
    int level;
    double Dur;
    double HP_pers, ATK, DEF, AGI, ATK_INTERVAL, CRIT_Rate;
    bool up_or_down;
};

enum class BuffType {
    POISON = 0,
    HEAL,
    BURN,
    FREEZE,
    PARALYSIS,
    WIND,
    STONE,
    HORMONE,
    WEAK,
    BLEED,
    BUILDUP,
    SPRING,
    SLOWDOWN,
    FLY,
    STUN
};
extern Buff buffs[];
#endif
