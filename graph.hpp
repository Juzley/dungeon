#ifndef __DELAUNAY_H__
#define __DELAUNAY_H__

namespace delaunay {
    /*
     * Vec2f
     *
     * A very simple 2d float vector class for use in graph calculations.
     */
    struct Vec2f
    {
        Vec2f() : x(0), y(0) {}
        Vec2f(float x, float y) : x(x), y(y) {}

        float Magnitude() const
        {
            return std::sqrt(x * x + y * y);
        }

        bool operator == (const Vec2f &v2) const
        {
            return x == v2.x && y == v2.y;
        }

        float x;
        float y;
    };

    static inline Vec2f operator - (const Vec2f &vl, const Vec2f &vr)
    {
        return Vec2f(vl.x - vr.x, vl.y - vr.y);
    }

    static inline Vec2f operator + (const Vec2f &vl, const Vec2f &vr)
    {
        return Vec2f(vl.x + vr.x, vl.y + vr.y);
    }


    /*
     * delaunayTriangulate
     *
     * Generate a Delaunay triangulation from a given set of points. The edges
     * of the resulting graph are returned as a vector of pairs of points
     * defining the endpoints of the edges.
     */
    std::vector<std::pair<Vec2f, Vec2f>> delaunayTriangulate(
                                                std::vector<Vec2f> &vertices);
}

#endif /* __DELAUNAY_H__ */
