Mesh3D terrain = make_cube_grid_custom(
    20, 20,
    0.5, 0.5, 0.5, 0.05,
    [&](int i, int j) {
        double y = std::sin(i*0.3) * std::cos(j*0.2) * 2.0;
        return Transformation3D::translation(0, y, 0);
    }
);
