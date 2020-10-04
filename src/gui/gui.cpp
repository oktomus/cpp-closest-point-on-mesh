#include "gui.h"

namespace gui
{

// Singleton access.
Gui& Gui::get_instance() 
{
    return Gui::m_instance; 
}

// Constructor.
Gui::Gui() {}

// Singleton instance.
Gui Gui::m_instance;

} // namespace gui