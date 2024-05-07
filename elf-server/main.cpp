#include "elf_server.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    elf_server w;
    w.show();
    return a.exec();
}
