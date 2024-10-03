# add as object library as not to compile all of these twice:
add_library(common STATIC Cylinder.cpp Mesh.cpp Plane.cpp Scene.cpp Sphere.cpp vec3.cpp)

add_executable(raytrace raytrace.cpp)
add_executable(debug_aabb debug_aabb.cpp)


find_package(OpenMP)

SET(TARGETS raytrace debug_aabb)

foreach(TARGET common ${TARGETS})
    set_target_properties(${TARGET}
        PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )
    if(WIN32)
        # windows standard library needs some special treatment
        target_compile_definitions(${TARGET} PRIVATE _USE_MATH_DEFINES NOMINMAX)
    endif()

    if(OpenMP_CXX_FOUND)
        target_link_libraries(${TARGET} PUBLIC OpenMP::OpenMP_CXX)
    endif()
endforeach()

foreach(TARGET ${TARGETS})
    target_link_libraries(${TARGET} PRIVATE common)
endforeach()
