#include "MeshBuilders.hpp"
#include "MeshRenderer2D.hpp"
#include "Transformation3D.hpp"
#include "View3DParameters.hpp"
#include "RasterBuffer.hpp"

int main() {
    View3DParameters params(
        Point3D(0,0,5), Point3D(0,0,0), Point3D(0,1,0),
        60.0*M_PI/180.0, 1.0, 0.1, 100.0);
    View3D cam = params.make_view();
    Projection3D proj = params.make_projection();

    Mesh3D sphere = make_uv_sphere(24, 32, 1.2);

    // Model transform
    Transformation3D model = Transformation3D::rotation_y(0.6) *
                             Transformation3D::rotation_x(-0.4);

    // Push through view transform
    Transformation3D V = cam.view_matrix();
    for (auto& p : sphere.vertices) p = V.apply(model.apply(p));

    auto vnorm = sphere.compute_vertex_normals();

    RasterBuffer<uint8_t> rb(600, 600, 1, 0, true);
    MeshRenderer2D renderer(rb, cam, proj, Point3D(0.3, 0.4, -1.0));

    // Flat shading
    rb.clear_depth(); rb.data.assign(rb.data.size(),0);
    renderer.render(sphere, vnorm, RenderMode::Flat, true);
    rb.save_ppm("sphere_flat.ppm");

    // Gouraud shading
    rb.clear_depth(); rb.data.assign(rb.data.size(),0);
    renderer.render(sphere, vnorm, RenderMode::Gouraud);
    rb.save_ppm("sphere_gouraud.ppm");

    // Phong shading
    rb.clear_depth(); rb.data.assign(rb.data.size(),0);
    renderer.render(sphere, vnorm, RenderMode::Phong, false, true, 230, 0.7, 0.4, 32.0);
    rb.save_ppm("sphere_phong.ppm");

    return 0;
}
