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
#include "Scene.h"

#include "Plane.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Mesh.h"

#include <limits>
#include <map>
#include <functional>
#include <stdexcept>

#if HAVE_OPENMP
#  include <omp.h>
#endif

//-----------------------------------------------------------------------------


Image Scene::render()
{
    // allocate new image.
    Image img(camera.width, camera.height);

    // Function rendering a full column of the image
    auto raytraceColumn = [&img, this](int x) {
        for (int y=0; y<int(camera.height); ++y)
        {
            Ray ray = camera.primary_ray(x,y);

            // compute color by tracing this ray
            vec3 color = trace(ray, 0);

            // avoid over-saturation
            color = min(color, vec3(1, 1, 1));

            // store pixel color
            img(x,y) = color;
        }
    };

    // If possible, raytrace image columns in parallel.

#if HAVE_OPENMP
    std::cout << "Rendering with up to " << omp_get_max_threads() << " threads." << std::endl;
#  pragma omp parallel for
#else
    std::cout << "Rendering singlethreaded (compiled without OpenMP)." << std::endl;
#endif

    for (int x=0; x<int(camera.width); ++x) {
        raytraceColumn(x);
    }

    // Note: compiler will elide copy.
    return img;
}

//-----------------------------------------------------------------------------

vec3 Scene::trace(const Ray& _ray, int _depth)
{
    // stop if recursion depth (=number of reflection) is too large
    if (_depth > max_depth) return vec3(0, 0, 0);

    // Find first intersection with an object. If an intersection is found,
    // it is stored in object, point, normal, and t.
    Object_ptr  object;
    vec3        point;
    vec3        normal;
    double      t;
    if (!intersect(_ray, object, point, normal, t))
    {
        return background;
    }

    // compute local Phong lighting (ambient+diffuse+specular)
    vec3 color = lighting(point, normal, -_ray.direction, object->material);

    // Compute reflections by recursive ray tracing
    if (object->material.mirror && _depth < max_depth)
    {
        vec3 reflectionDir = reflect(_ray.direction, normal);
        Ray reflectionRay(point + normal * 0.001, reflectionDir); // small offset to avoid self-intersection
        vec3 reflectionColor = trace(reflectionRay, _depth + 1);
        // Linear interpolation
        color = (1 - object->material.mirror) * color + object->material.mirror * reflectionColor;
    }

    return color;
}


//-----------------------------------------------------------------------------

bool Scene::intersect(const Ray& _ray, Object_ptr& _object, vec3& _point, vec3& _normal, double& _t)
{
    double  t, tmin(Object::NO_INTERSECTION);
    vec3    p, n;

    for (const auto &o: objects) // for each object
    {
        if (o->intersect(_ray, p, n, t)) // does ray intersect object?
        {
            if (t < tmin) // is intersection point the currently closest one?
            {
                tmin = t;
                _object = o.get();
                _point  = p;
                _normal = n;
                _t      = t;
            }
        }
    }

    return (tmin != Object::NO_INTERSECTION);
}

vec3 Scene::lighting(const vec3& _point, const vec3& _normal, const vec3& _view, const Material& _material)
{
    vec3 ambient_contribution  = _material.ambient*ambience;
    vec3 diff_spec_shadows = vec3(0, 0, 0);

    for (const Light& lightsource : lights)
    {
        vec3 l = normalize(lightsource.position - _point);
        Ray shadowRay(_point + _normal * 0.001, l); // small offset to avoid self-intersection
        Object_ptr shadowObject;
        vec3 shadowIntersectionPoint, shadowIntersectionNormal;
        double shadowIntersectionT;

        bool inShadow = intersect(shadowRay, shadowObject, shadowIntersectionPoint, shadowIntersectionNormal, shadowIntersectionT);

        if (!inShadow)
        {
            double diffuse_part = std::max(dot(_normal, l), 0.0);
            vec3 diffuse_reflection_part = _material.diffuse * diffuse_part;
            if (diffuse_part)
            {
                vec3 specular_reflection_part = _material.specular * pow(std::max(dot(mirror(l, _normal), _view), 0.0), _material.shininess);
                diff_spec_shadows += lightsource.color * (diffuse_reflection_part + specular_reflection_part);
            }
        }
    }

    vec3 color = ambient_contribution+diff_spec_shadows;

    return color;
}

//-----------------------------------------------------------------------------

void Scene::read(const std::string &_filename)
{
    std::ifstream ifs(_filename);
    if (!ifs)
        throw std::runtime_error("Cannot open file " + _filename);

    const std::map<std::string, std::function<void(void)>> entityParser = {
        {"depth",      [&]() { ifs >> max_depth; }},
        {"camera",     [&]() { ifs >> camera; }},
        {"background", [&]() { ifs >> background; }},
        {"ambience",   [&]() { ifs >> ambience; }},
        {"light",      [&]() { lights .emplace_back(ifs); }},
        {"plane",      [&]() { objects.emplace_back(new    Plane(ifs)); }},
        {"sphere",     [&]() { objects.emplace_back(new   Sphere(ifs)); }},
        {"cylinder",   [&]() { objects.emplace_back(new Cylinder(ifs)); }},
        {"mesh",       [&]() { objects.emplace_back(new     Mesh(ifs, _filename)); }}
    };

    // parse file
    std::string token;
    while (ifs && (ifs >> token) && (!ifs.eof())) {
        if (token[0] == '#') {
            ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (entityParser.count(token) == 0)
            throw std::runtime_error("Invalid token encountered: " + token);
        entityParser.at(token)();
    }
}


//=============================================================================
