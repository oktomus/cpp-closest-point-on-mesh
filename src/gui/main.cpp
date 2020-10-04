#include "mainwindow.h"

#include <iostream>
#include <string>

int main()
{
    gui::MainWindow& window = gui::MainWindow::get_instance();

    window.init(800, 600);
    window.load_scene("resources/models/cube.obj");
    window.run();
    window.release();

    return 0;
}
