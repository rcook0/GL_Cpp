#include "Renderer2D.hpp"
#include "View3DParameters.hpp"
#include <iostream>

int main() {
    View3DParameters params(Point3D(2,2,5), Point3D(0,0,0), Point3D(0,1,0),
                            60.0*M_PI/180.0, 1.0, 0.1, 100.0);
    View3D camera = params.make_view();
    Projection3D proj = params.make_projection();

    Scene3D scene(proj);
    double s=1.0;
    std::vector<Point3D> v = {
        {-s,-s,-s},{s,-s,-s},{s,s,-s},{-s,s,-s},
        {-s,-s,s},{s,-s,s},{s,s,s},{-s,s,s}
    };
    scene.add_object(Polygon3D({v[0],v[1],v[2],v[3]}));
    scene.add_object(Polygon3D({v[4],v[5],v[6],v[7]}));
    scene.add_object(Polygon3D({v[0],v[1],v[5],v[4]}));
    scene.add_object(Polygon3D({v[2],v[3],v[7],v[6]}));
    scene.add_object(Polygon3D({v[1],v[2],v[6],v[5]}));
    scene.add_object(Polygon3D({v[0],v[3],v[7],v[4]}));

    scene.apply_transformation(camera.view_matrix());

    RasterBuffer<uint8_t> rb(512,512,1,0,true);
    rb.clear_depth();
    Renderer2D renderer(rb, camera, proj);
    renderer.render(scene);

    rb.save_ppm("cube_z.ppm");
    std::cout << "Cube rendered with Z-buffer to cube_z.ppm\n";
}
