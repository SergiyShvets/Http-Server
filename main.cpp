#include "dialog.h"
#include "test.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

   // Test test;
   // test.doTest();
    Dialog dialog;
    dialog.show();

    return app.exec();
}
