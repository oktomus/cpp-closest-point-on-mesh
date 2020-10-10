#pragma once

#include <glm/glm.hpp>

namespace core
{

//
// Math utilities.
//

/**
 * @brief Squared distance between two points.
 */
inline float distance2(const glm::vec3& lhs, const glm::vec3& rhs);

/**
 * @brief Closest point to a given point on a triangle.
 *
 * Reference:
 *  - Distance Between Point and Triangle in 3D
 *    David Eberly, Geometric Tools, Redmond WA 98052
 *    https://www.geometrictools.com/Documentation/DistancePoint3Triangle3.pdf
 */
inline glm::vec3 closest_point_in_triangle(
    const glm::vec3&    p,
    const glm::vec3&    vertex0,
    const glm::vec3&    vertex1,
    const glm::vec3&    vertex2);


//
// Implementaiton.
//

float distance2(const glm::vec3& lhs, const glm::vec3& rhs)
{
    const glm::vec3 diff = lhs - rhs;
    return glm::dot(diff, diff);
}

glm::vec3 closest_point_in_triangle(
    const glm::vec3&    p,
    const glm::vec3&    vertex0,
    const glm::vec3&    vertex1,
    const glm::vec3&    vertex2)
{
    const glm::vec3 vertex0_p = p - vertex0;
    const glm::vec3 edge0 = vertex1 - vertex0;
    const glm::vec3 edge1 = vertex2- vertex0;
    const float a00 = glm::dot(edge0, edge0);
    const float a01 = glm::dot(edge0, edge1);
    const float a11 = glm::dot(edge1, edge1);
    const float b0 = -glm::dot(vertex0_p, edge0);
    const float b1 = -glm::dot(vertex0_p, edge1);

    const float det = a00 * a11 - a01 * a01;
    float t0 = a01 * b1 - a11 * b0;
    float t1 = a01 * b0 - a00 * b1;

    // FIXME: Make this code easy to read.
    // Right now, it's copy-pasted from the paper.
    if (t0 + t1 <= det)
    {
        if (t0 < 0.0f)
        {
            if (t1 < 0.0f)  // region 4
            {
                if (b0 < 0.0f)
                {
                    t1 = 0.0f;
                    if (-b0 >= a00)  // V1
                    {
                        t0 = 1.0f;
                    }
                    else  // E01
                    {
                        t0 = -b0 / a00;
                    }
                }
                else
                {
                    t0 = 0.0f;
                    if (b1 >= 0.0f)  // V0
                    {
                        t1 = 0.0f;
                    }
                    else if (-b1 >= a11)  // V2
                    {
                        t1 = 1.0f;
                    }
                    else  // E20
                    {
                        t1 = -b1 / a11;
                    }
                }
            }
            else  // region 3
            {
                t0 = 0.0f;
                if (b1 >= 0.0f)  // V0
                {
                    t1 = 0.0f;
                }
                else if (-b1 >= a11)  // V2
                {
                    t1 = 1.0f;
                }
                else  // E20
                {
                    t1 = -b1 / a11;
                }
            }
        }
        else if (t1 < 0.0f)  // region 5
        {
            t1 = 0.0f;
            if (b0 >= 0.0f)  // V0
            {
                t0 = 0.0f;
            }
            else if (-b0 >= a00)  // V1
            {
                t0 = 1.0f;
            }
            else  // E01
            {
                t0 = -b0 / a00;
            }
        }
        else  // region 0, interior
        {
            const float invDet = 1.0f / det;
            t0 *= invDet;
            t1 *= invDet;
        }
    }
    else
    {
        float tmp0, tmp1, numer, denom;

        if (t0 < 0.0f)  // region 2
        {
            tmp0 = a01 + b0;
            tmp1 = a11 + b1;
            if (tmp1 > tmp0)
            {
                numer = tmp1 - tmp0;
                denom = a00 - 2.0 * a01 + a11;
                if (numer >= denom)  // V1
                {
                    t0 = 1.0f;
                    t1 = 0.0f;
                }
                else  // E12
                {
                    t0 = numer / denom;
                    t1 = 1.0f - t0;
                }
            }
            else
            {
                t0 = 0.0f;
                if (tmp1 <= 0.0f)  // V2
                {
                    t1 = 1.0f;
                }
                else if (b1 >= 0.0f)  // V0
                {
                    t1 = 0.0f;
                }
                else  // E20
                {
                    t1 = -b1 / a11;
                }
            }
        }
        else if (t1 < 0.0f)  // region 6
        {
            tmp0 = a01 + b1;
            tmp1 = a00 + b0;
            if (tmp1 > tmp0)
            {
                numer = tmp1 - tmp0;
                denom = a00 - 2.0 * a01 + a11;
                if (numer >= denom)  // V2
                {
                    t1 = 1.0f;
                    t0 = 0.0f;
                }
                else  // E12
                {
                    t1 = numer / denom;
                    t0 = 1.0f - t1;
                }
            }
            else
            {
                t1 = 0.0f;
                if (tmp1 <= 0.0f)  // V1
                {
                    t0 = 1.0f;
                }
                else if (b0 >= 0.0f)  // V0
                {
                    t0 = 0.0f;
                }
                else  // E01
                {
                    t0 = -b0 / a00;
                }
            }
        }
        else  // region 1
        {
            numer = a11 + b1 - a01 - b0;
            if (numer <= 0.0f)  // V2
            {
                t0 = 0.0f;
                t1 = 1.0f;
            }
            else
            {
                denom = a00 - 2.0 * a01 + a11;
                if (numer >= denom)  // V1
                {
                    t0 = 1.0f;
                    t1 = 0.0f;
                }
                else  // 12
                {
                    t0 = numer / denom;
                    t1 = 1.0f - t0;
                }
            }
        }
    }

    return vertex0 + t0 * edge0 + t1 * edge1;
}

} // namespace core
