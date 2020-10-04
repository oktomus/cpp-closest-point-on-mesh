#pragma once

#include <string>

// Forward declarations.
namespace core { class Scene; }

namespace core
{

/**
 * @brief Load meshes from a file using Assimp and prepare them for render.
 */
Scene* load_scene_from_file(const std::string& file_path);

} // namespace core