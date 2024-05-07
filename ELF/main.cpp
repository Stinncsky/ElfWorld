#include "elf.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Elf w;
    w.show();
    return a.exec();
}
