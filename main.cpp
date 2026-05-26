// main.cpp
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include <QApplication>
#include "core/AppController.h"

int main(int argc, char* argv[]) {
    QApplication l_app(argc, argv);

    AppController l_controller;
    l_controller.initialize();

    return l_app.exec();
}