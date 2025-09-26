#include "Mesh3D.hpp"
#include "MeshRenderer2D.hpp"
#include "Transformation3D.hpp"
#include "View3DParameters.hpp"
#include "RasterBuffer.hpp"
#include <iostream>

int main() {
    // Camera + projection
    View3DParameters params(Point3D(2,2,5), Point3D(0,0,0), Point3D(0,1,0),
                            60.0*M_PI/180.0, 1.0, 0.1, 100.0);
    View3D cam = params.make_view();
    Projection3D proj = params.make_projection();

    // Build a shared-vertex cube
    Mesh3D mesh;
    double s=1.0;
    int v0 = mesh.add_vertex(Point3D(-s,-s,-s));
    int v1 = mesh.add_vertex(Point3D( s,-s,-s));
    int v2 = mesh.add_vertex(Point3D( s, s,-s));
    int v3 = mesh.add_vertex(Point3D(-s, s,-s));
    int v4 = mesh.add_vertex(Point3D(-s,-s, s));
    int v5 = mesh.add_vertex(Point3D( s,-s, s));
    int v6 = mesh.add_vertex(Point3D( s, s, s));
    int v7 = mesh.add_vertex(Point3D(-s, s, s));

    // Faces (quads)
    mesh.add_face({v0,v1,v2,v3}); // back
    mesh.add_face({v4,v5,v6,v7}); // front
    mesh.add_face({v0,v1,v5,v4}); // bottom
    mesh.add_face({v2,v3,v7,v6}); // top
    mesh.add_face({v1,v2,v6,v5}); // right
    mesh.add_face({v0,v3,v7,v4}); // left

    // Transform to VIEW space (apply camera)
    Transformation3D V = cam.view_matrix();
    for (auto& p : mesh.vertices) p = V.apply(p);

    // Compute vertex normals (view space)
    auto vnorm = mesh.compute_vertex_normals();

    // Prepare target
    RasterBuffer<uint8_t> rb(512,512,1,0,true);
    rb.clear_depth();

    MeshRenderer2D renderer(rb, cam, proj, Point3D(0,0,-1));
    renderer.render_gouraud(mesh, vnorm, true);

    rb.save_ppm("mesh_cube_gouraud.ppm");
    std::cout << "Saved mesh_cube_gouraud.ppm\n";
    return 0;
}
