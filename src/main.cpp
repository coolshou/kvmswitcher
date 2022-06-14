#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>

#include "kvmswitcher.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "kvmswitcher_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    kvmswitcher *kvms = new kvmswitcher();

    return a.exec();
}
