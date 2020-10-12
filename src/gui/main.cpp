#include "mainwindow.h"

#include <iostream>
#include <string>

int main()
{
    gui::MainWindow& window = gui::MainWindow::get_instance();

    window.init(800, 600);

    // Load an asset by default.
    window.load_scene("resources/models/teapot.obj");

    window.run();

    // Close the window.
    window.release();

    return 0;
}
