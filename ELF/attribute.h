#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include <QString>
enum class AttributeRelationship {
    WEAK,       // 弱
    NEUTRAL,    // 中立
    STRONG      // 强
};
class Attribute{
public:
    QString Attr_name() const{ return name; }
    Attribute(QString name = ""):name(name){};
    bool operator==(const Attribute& other) const{
        return name == other.name;
    }
    bool operator!=(const Attribute& other) const{
        return name != other.name;
    }
    bool operator<(const Attribute& other) const{
        return name < other.name;
    }
private:
    QString name;
};

#endif // ATTRIBUTE_H
