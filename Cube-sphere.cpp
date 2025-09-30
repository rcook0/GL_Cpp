// Cube-sphere with 16 subdivisions per face
Mesh3D cubeSphere = make_cube_sphere(16, 1.0);

// Transform to view space
Transformation3D V = cam.view_matrix();
for (auto& p : cubeSphere.vertices) p = V.apply(model.apply(p));

// Normals
auto vnorm = cubeSphere.compute_vertex_normals();

// Render
renderer.render(cubeSphere, vnorm, RenderMode::Phong);
rb.save_ppm("cube_sphere.ppm");
