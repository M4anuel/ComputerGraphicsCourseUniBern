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

#include "Plane.h"
#include <limits>


//== CLASS DEFINITION =========================================================


Plane::Plane(const vec3& _center, const vec3& _normal)
    : center(_center), normal(_normal)
{
}


//-----------------------------------------------------------------------------
// copypasted function from stackoverflow as dotproduct is unlikely to give us back exactly 0, we should check if a number is very close to zero
// https://stackoverflow.com/a/6982672
template <typename T>
bool is_close_to_zero(T x)
{
    return std::abs(x) < std::numeric_limits<T>::epsilon();
}

bool
Plane::
intersect(const Ray& _ray,
          vec3& _intersection_point,
          vec3& _intersection_normal,
          double& _intersection_t) const
/**
{
    const double denom = dot(normal, _ray.direction);
    // if rays are parallel this would be 0, but instead of bool here we can use double for calculation later
    if (is_close_to_zero(denom))
    {
        // _intersection_t = (dot(normal, center) - dot(normal, _ray.origin)) / denom; old way, factor out n
        _intersection_t = dot(normal, center - _ray.origin) / denom;
        _intersection_point = _ray.origin + _intersection_t * _ray.direction;
        _intersection_normal = normal;
    }
    return denom != 0 && _intersection_t > 0;
}
**/
{


    // Get origin and direction of the ray
    vec3 ray_origin = _ray.origin;
    vec3 ray_direction = _ray.direction;

    // Calculate denom (normal dot direction)
    double denom = dot(normal, ray_direction);

    // If denom is 0, they are parallel and there is no intersection, but we used epsilon here so if its really close to 0, it counts as 0. (floating point problem)
    if (std::abs(denom) < std::numeric_limits<double>::epsilon()) {
        return false;
    }

    // Calculate parameter t
    _intersection_t = dot(normal, center - ray_origin) / denom;

    // Intersection is infront of the camera, so it has to be t > 0 or we will not see it in the picture.
    if (_intersection_t < 0) {
        return false;
    }

    // Calculate intersection point
    _intersection_point = ray_origin + _intersection_t * ray_direction;

    //At the intersection, the "intersection normal" has to be the same as the "plane normal"
    _intersection_normal = normal;

    return true;
}

//=============================================================================
