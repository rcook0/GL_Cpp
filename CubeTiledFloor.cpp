/* Multi-colored blocky floor with smooth Phong shading */

Mesh3D grid = make_cube_grid_custom(
    6, 6,
    1.0, 1.0, 1.0, 0.1,
    nullptr,
    [&](int i,int j) {
        return Point3D((i%3)/2.0, (j%3)/2.0, 0.5); // varying colors
    }
);

Transformation3D V = cam.view_matrix();
for (auto& p : grid.vertices) p = V.apply(p);
auto vnorm = grid.compute_vertex_normals();

MeshRenderer2D renderer(rb, cam, proj, Point3D(0.3,0.4,-1.0));
renderer.render(grid, vnorm, RenderMode::Phong);

rb.save_ppm("colored_grid.ppm");
