#include "elf_member.h"
std::map<QString, QString> Elfs_List = {
    {"导弹毛", "1"},
    {"哮天犬", "2"},
    {"暗雷小妖", "3"}
};
elf_member elfs_normal[] = {
    elf_member("导弹毛", 1, 0, Attribute::无),
    elf_power("哮天犬", 1, 0, Attribute::火),
    elf_power("暗雷小妖", 1, 0, Attribute::电)
};
