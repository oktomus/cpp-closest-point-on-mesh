#pragma once

#include <glm/glm.hpp>

namespace core
{

inline float distance2(const glm::vec3& lhs, const glm::vec3& rhs);

float distance2(const glm::vec3& lhs, const glm::vec3& rhs)
{
    const glm::vec3 diff = lhs - rhs;
    return glm::dot(diff, diff);
}

} // namespace core
