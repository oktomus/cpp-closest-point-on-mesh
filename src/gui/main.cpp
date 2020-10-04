#include "mainwindow.h"

#include "core/mesh.h"

#include <iostream>
#include <string>

int main()
{
    tryCoreMesh();
    gui::MainWindow& window = gui::MainWindow::get_instance();

    window.init(800, 600);
    window.run();
    window.release();

    return 0;
}
