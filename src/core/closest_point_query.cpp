#include "closest_point_query.h"

namespace core
{

ClosestPointQuery::ClosestPointQuery(const Mesh& mesh)
  : m_mesh(mesh)
{}

glm::vec3 ClosestPointQuery::get_closest_point(
    const glm::vec3&    query_point, 
    float               max_distance) const
{
    return { 0, 0, 0 };
}

} // namespace core