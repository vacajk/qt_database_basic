#include "database_basic.h"
#include <QApplication>
#include <QStyleFactory>

#define DATABASE_SAMPLE_DATA  QString("sampledata")

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QApplication a(argc, argv);
    database_basic w(DATABASE_SAMPLE_DATA);
    w.show();

    return a.exec();
}
