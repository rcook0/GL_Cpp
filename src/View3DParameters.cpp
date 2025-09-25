#include "View3DParameters.hpp"

View3DParameters::View3DParameters(const Point3D& eye,
                                   const Point3D& target,
                                   const Point3D& up,
                                   double fovRadians,
                                   double aspect,
                                   double nearZ,
                                   double farZ)
    : eye(eye), target(target), up(up),
      fov(fovRadians), aspect(aspect), nearZ(nearZ), farZ(farZ) {}

View3D View3DParameters::make_view() const {
    return View3D(eye, target, up);
}
 
Projection3D View3DParameters::make_projection() const {
    return Projection3D(ProjectionType::PERSPECTIVE, fov, nearZ, farZ);
}

std::ostream& operator<<(std::ostream& os, const View3DParameters& v) {
    os << "View3DParameters("
       << "eye=" << v.eye
       << ", target=" << v.target
       << ", up=" << v.up
       << ", fov=" << v.fov
       << ", aspect=" << v.aspect
       << ", near=" << v.nearZ
       << ", far=" << v.farZ
       << ")";
    return os;
}
