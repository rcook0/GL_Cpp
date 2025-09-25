#pragma once
#include "Point2D.hpp"
#include <vector>
#include <cmath>

class Bezier2D {
public:
    // Evaluate quadratic Bezier at t in [0,1]
    static Point2D quad(const Point2D& p0, const Point2D& p1, const Point2D& p2, double t) {
        double u = 1.0 - t;
        return p0*(u*u) + p1*(2*u*t) + p2*(t*t);
    }

    // Evaluate cubic Bezier at t in [0,1]
    static Point2D cubic(const Point2D& p0, const Point2D& p1, const Point2D& p2, const Point2D& p3, double t) {
        double u = 1.0 - t;
        double u2 = u*u, t2 = t*t;
        double u3 = u2*u, t3 = t2*t;
        return p0*u3 + p1*(3*u2*t) + p2*(3*u*t2) + p3*t3;
    }

    // Tesselate to polyline with N+1 points (including endpoints)
    static std::vector<Point2D> tesselate_quadratic(const Point2D& p0, const Point2D& p1, const Point2D& p2, int segments) {
        std::vector<Point2D> out;
        out.reserve(segments+1);
        for (int i=0; i<=segments; ++i) {
            double t = (segments==0) ? 0.0 : static_cast<double>(i)/segments;
            out.push_back(quad(p0,p1,p2,t));
        }
        return out;
    }

    static std::vector<Point2D> tesselate_cubic(const Point2D& p0, const Point2D& p1, const Point2D& p2, const Point2D& p3, int segments) {
        std::vector<Point2D> out;
        out.reserve(segments+1);
        for (int i=0; i<=segments; ++i) {
            double t = (segments==0) ? 0.0 : static_cast<double>(i)/segments;
            out.push_back(cubic(p0,p1,p2,p3,t));
        }
        return out;
    }
};
