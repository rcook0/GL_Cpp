#include "Scene3D.hpp"
#include "View3DParameters.hpp"
#include "Drawing2D.hpp"
#include "RasterBuffer.hpp"
#include <iostream>
#include <cmath>

int main() {
    // Camera
    View3DParameters params(Point3D(2,2,5), Point3D(0,0,0), Point3D(0,1,0),
                            60.0*M_PI/180.0, 1.0, 0.1, 100.0);
    View3D camera = params.make_view();
    Projection3D projection = params.make_projection();

    // Scene: cube
    Scene3D scene(projection);
    double s = 1.0;
    std::vector<Point3D> verts = {
        {-s,-s,-s},{s,-s,-s},{s,s,-s},{-s,s,-s}, // back face
        {-s,-s,s},{s,-s,s},{s,s,s},{-s,s,s}      // front face
    };
    scene.add_object(Polygon3D({verts[0],verts[1],verts[2],verts[3]}));
    scene.add_object(Polygon3D({verts[4],verts[5],verts[6],verts[7]}));
    scene.add_object(Polygon3D({verts[0],verts[1],verts[5],verts[4]}));
    scene.add_object(Polygon3D({verts[2],verts[3],verts[7],verts[6]}));
    scene.add_object(Polygon3D({verts[1],verts[2],verts[6],verts[5]}));
    scene.add_object(Polygon3D({verts[0],verts[3],verts[7],verts[4]}));

    // Apply view
    scene.apply_transformation(camera.view_matrix());

    // Project
    auto polys2d = scene.project_to_2d();

    // Draw
    RasterBuffer<uint8_t> rb(512,512,1,0);
    Drawing2D draw(rb);
    auto viewport = [&](const Point2D& p) {
        double x = (p.x+1.0)*0.5*rb.width;
        double y = (1.0-(p.y+1.0)*0.5)*rb.height;
        return Point2D(x,y);
    };

    for (const auto& poly : polys2d) {
        std::vector<Point2D> screenPoly;
        for (auto& v : poly) screenPoly.push_back(viewport(v));
        draw.fill_polygon(screenPoly, 200);   // filled
        draw.polygon(screenPoly, 255);       // outline
    }

    rb.save_ppm("cube_filled.ppm");
    std::cout << "Cube rendered to cube_filled.ppm\n";
}
