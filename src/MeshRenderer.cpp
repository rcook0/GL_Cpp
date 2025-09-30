// Assuming: mesh in view space (apply camera.view_matrix() earlier)
//           vnorm = mesh.compute_vertex_normals() (also in view space)

RasterBuffer<uint8_t> rb(800, 800, 1, 0, true);
rb.clear_depth();

View3D cam = params.make_view();
Projection3D proj = params.make_projection();

MeshRenderer2D renderer(rb, cam, proj, Point3D(0.3,0.4,-1.0));

// Flat
renderer.render(mesh, vnorm, RenderMode::Flat, /*wire*/true);

// Gouraud
rb.clear_depth(); rb.data.assign(rb.data.size(),0);
renderer.render(mesh, vnorm, RenderMode::Gouraud, /*wire*/false);

// Phong (per-pixel)
rb.clear_depth(); rb.data.assign(rb.data.size(),0);
renderer.render(mesh, vnorm, RenderMode::Phong, /*wire*/false, /*persp*/true, /*base*/230, /*kd*/0.7, /*ks*/0.4, /*shininess*/32.0);

rb.save_ppm("output.ppm");
