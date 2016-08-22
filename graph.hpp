#ifndef __GRAPH_HPP_
#define __GRAPH_HPP_

#include <array>
#include <vector>
#include <cmath>

namespace graph {
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
     * Edge
     * 
     * An edge in a graph, defined by its endpoints.
     */
    struct Edge
    {
        Edge() {}
        Edge(const Vec2f &p1, const Vec2f &p2) : p1(p1), p2(p2) {}

        float Length() const
        {
            return (p1 - p2).Magnitude();
        }

        Vec2f p1;
        Vec2f p2;
    };

    static inline bool operator == (const Edge &e1, const Edge &e2) {
        return ((e1.p1 == e2.p1 && e1.p2 == e2.p2) ||
                (e1.p1 == e2.p2 && e1.p2 == e2.p1));
    }

    static inline bool operator != (const Edge &e1, const Edge &e2) {
        return !operator==(e1, e2);
    }


    /*
     * Triangle
     *
     * A simple triangle class, specifically taylored for our triangulation
     * calculations.
     */
    class Triangle
    {
        friend struct TriangleHash;
        friend std::ostream& operator << (std::ostream& os, const Triangle& t);

        public:
            Triangle() {}

            Triangle(const Triangle &t)
                : p1(t.p1), p2(t.p2), p3(t.p3),
                  edges{{t.edges[0], t.edges[1], t.edges[2]}},
                  circumcenter(t.circumcenter), circumradius(t.circumradius)
            {
            }

            Triangle(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3)
                : p1(p1), p2(p2), p3(p3),
                  edges{{Edge(p1, p2), Edge(p2, p3), Edge(p3, p1)}}
            {
                float ab = (p1.x * p1.x) + (p1.y * p1.y);
                float cd = (p2.x * p2.x) + (p2.y * p2.y);
                float ef = (p3.x * p3.x) + (p3.y * p3.y);

                circumcenter.x =
                    (ab * (p3.y - p2.y) + cd * (p1.y - p3.y) + ef * (p2.y - p1.y))
                    / (p1.x * (p3.y - p2.y) + p2.x * (p1.y - p3.y) + p3.x * (p2.y - p1.y))
                    / 2.0f;
                circumcenter.y =
                    (ab * (p3.x - p2.x) + cd * (p1.x - p3.x) + ef * (p2.x - p1.x))
                    / (p1.y * (p3.x - p2.x) + p2.y * (p1.x - p3.x) + p3.y * (p2.x - p1.x))
                    / 2.0f;

                circumradius = (p1 - circumcenter).Magnitude();
            }

            bool circumcircleContains(const Vec2f &v) const
            {
                // Distance between circumcenter and point 
                float distance = (v - circumcenter).Magnitude();
                return distance <= circumradius;
            }

            bool hasCommonPoints(const Triangle &t) const
            {
                return (p1 == t.p1 || p1 == t.p2 || p1 == t.p3 ||
                        p2 == t.p1 || p2 == t.p2 || p2 == t.p3 ||
                        p3 == t.p1 || p3 == t.p2 || p3 == t.p3);
            }
            
            bool operator == (const Triangle &t2) const
            {
                return (p1 == t2.p1 || p1 == t2.p2 || p1 == t2.p3) &&
                       (p2 == t2.p1 || p2 == t2.p2 || p1 == t2.p3) &&
                       (p3 == t2.p1 || p3 == t2.p2 || p3 == t2.p3);
            }

            const std::array<Edge, 3> & GetEdges() const
            {
                return edges;
            }

        private:
            Vec2f p1;
            Vec2f p2;
            Vec2f p3;
            std::array<Edge, 3> edges;
            Vec2f circumcenter;
            float circumradius;
    };


    /*
     * generateDelaunay
     *
     * Generate a Delaunay triangulation from a given set of points.
     */
    std::vector<Triangle> generateDelaunay(std::vector<Vec2f> &vertices);


    /*
     * generateUrquhart
     *
     * Generate an Urquhart graph for a given set of points.
     */
    std::vector<Edge> generateUrquhart(std::vector<Vec2f> &vertices);
}

#endif /* __GRAPH_HPP__ */
