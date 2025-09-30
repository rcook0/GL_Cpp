// Build an icosphere with 2 subdivisions
Mesh3D ico = make_icosphere(2, 1.0);

// Transform to view space
Transformation3D V = cam.view_matrix();
for (auto& p : ico.vertices) p = V.apply(model.apply(p));

// Normals
auto vnorm = ico.compute_vertex_normals();

// Render with Phong
renderer.render(ico, vnorm, RenderMode::Phong);
rb.save_ppm("ico_phong.ppm");
