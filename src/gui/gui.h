#pragma once

namespace gui
{

/**
 * @brief GUI Singleton.
 * Control the whole application UI using imgui.
 */
class Gui
{
  public:
    static Gui& get_instance();

  private:
    Gui();

    /**
     * @brief Copy is forbiden.
     */
    Gui(const Gui&) = delete;

    static Gui m_instance;
};

} // namespace gui