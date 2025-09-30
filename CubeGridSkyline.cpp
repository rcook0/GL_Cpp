// Make a 10x10 grid of cubes, each with random height
Mesh3D skyline = make_cube_grid_custom(
    10, 10,
    1.0, 1.0, 1.0, 0.1,
    [&](int i, int j) {
        double h = 0.5 + (std::sin(i*0.5) + std::cos(j*0.3))*0.5 + 1.0;
        Transformation3D scaleH = Transformation3D::scale(1.0, h, 1.0);
        return scaleH;
    }
);

// Move and rotate the whole grid
Transformation3D model = Transformation3D::translation(-5,0,-5) *
                         Transformation3D::rotation_y(0.5);
skyline = transform_mesh(skyline, model);

// To view space
Transformation3D V = cam.view_matrix();
for (auto& p : skyline.vertices) p = V.apply(p);
auto vnorm = skyline.compute_vertex_normals();

renderer.render(skyline, vnorm, RenderMode::Phong);
rb.save_ppm("skyline.ppm");
