Mesh3D grid = make_cube_grid_custom(
    8, 8,
    1.0, 1.0, 1.0, 0.1,
    nullptr,
    [&](int i,int j) {
        bool even = ((i+j)%2==0);
        return even ? Point3D(1,0,0) : Point3D(0,0,1); // red/blue
    }
);
