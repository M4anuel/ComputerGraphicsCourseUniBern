//=============================================================================
//
//   Exercise code for the lecture
//   "Introduction to Computer Graphics"
//   by Prof. Dr. Mario Botsch, Bielefeld University
//
//   Copyright (C) Computer Graphics Group, Bielefeld University.
//
//=============================================================================

//== INCLUDES =================================================================

#include "Cylinder.h"
#include "SolveQuadratic.h"

#include <array>
#include <cmath>

//== IMPLEMENTATION =========================================================

bool
Cylinder::
intersect(const Ray& _ray,
          vec3& _intersection_point,
          vec3& _intersection_normal,
          double& _intersection_t) const
{
    vec3 oc = _ray.origin - center;
    vec3 d = _ray.direction;
    vec3 a = axis;

    double A = dot(d - dot(d, a) * a, d - dot(d, a) * a);
    double B = 2.0 * dot(d - dot(d, a) * a, oc - dot(oc, a) * a);
    double C = dot(oc - dot(oc, a) * a, oc - dot(oc, a) * a) - radius * radius;


    std::array<double, 2> t;
    size_t nsol = solveQuadratic(A, B, C, t);

    _intersection_t = NO_INTERSECTION;

    // Find the closest valid solution (in front of the viewer)
    for (size_t i = 0; i < nsol; ++i) {
        if (t[i] > 0) {
            vec3 intersection_point_temp = _ray.origin + t[i] * _ray.direction;
            vec3 v = intersection_point_temp - center;
            double height_projection = dot(v, a);
            if (height_projection >= 0 && height_projection <= height) {
                _intersection_t = std::min(_intersection_t, t[i]);
                _intersection_point = intersection_point_temp;
            }
        }
    }

    if (_intersection_t == NO_INTERSECTION) return false;

    // Calculate the normal
    vec3 v = _intersection_point - center;
    vec3 axisPoint = center + (dot(v, a) / dot(a, a)) * a;
    _intersection_normal = normalize(_intersection_point - axisPoint);
    // Flip the normal if necessary
    if (dot(_ray.direction, _intersection_normal) > 0)
    {
        _intersection_normal = -_intersection_normal;
    }

    return true;
}
