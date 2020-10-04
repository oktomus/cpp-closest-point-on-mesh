#include "gui.h"

#include <iostream>
#include <string>

int main()
{
    gui::Gui& gui = gui::Gui::get_instance();

    gui.init(800, 600);
    gui.run();
    gui.release();

    return 0;
}
