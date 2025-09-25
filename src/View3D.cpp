#include "View3D.hpp"
#include <cmath>

View3D::View3D(const Point3D& eye, const Point3D& target, const Point3D& up)
    : eye(eye), target(target), up(up) {}

Transformation3D View3D::view_matrix() const {
    // Forward (z axis)
    Point3D f = (target - eye).normalized();
    // Right (x axis)
    Point3D r = f.cross(up).normalized();
    // Recomputed up (y axis)
    Point3D u = r.cross(f);

    Transformation3D view;
    view.m = {{
        {{ r.x,  r.y,  r.z, -r.dot(eye) }},
        {{ u.x,  u.y,  u.z, -u.dot(eye) }},
        {{-f.x, -f.y, -f.z,  f.dot(eye) }},
        {{ 0,    0,    0,    1          }}
    }};
    return view;
}

std::ostream& operator<<(std::ostream& os, const View3D& v) {
    os << "View3D(eye=" << v.eye
       << ", target=" << v.target
       << ", up=" << v.up << ")";
    return os;
}
