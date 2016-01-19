#include <cmath>
#include <array>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <iostream>


#include "graph.hpp"


namespace graph
{
    /*
     * hash_combine
     *
     * Combine multiple std::hash values.
     */
    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }


   struct {
       bool operator()(const Vec2f* v1, const Vec2f* v2)
       {
           if (v1->x < v2->x) {
               return true;
           } else if (v1->x > v2->x) {
               return false;
           } else if (v1->y < v2->y) {
               return true;
           } else {
               return false;
           }
       }
   } vecLess;


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

        bool operator == (const Edge& e2) const
        {
            return ((p1 == e2.p1 && p2 == e2.p2) ||
                    (p1 == e2.p2 && p2 == e2.p1));
        }

        Vec2f p1;
        Vec2f p2;
    };

    struct EdgeHash
    {
        std::size_t operator()(const Edge &e) const
        {
            std::size_t seed = 0;

            // Sort the points so that if there are two edges with the same
            // points but different order, then hash to the same value.
            std::array<const Vec2f*, 2> pts = { &e.p1, &e.p2 };
            std::sort(pts.begin(), pts.end(), vecLess);

            hash_combine(seed, pts[0]->x);
            hash_combine(seed, pts[0]->y);
            hash_combine(seed, pts[1]->x);
            hash_combine(seed, pts[1]->y);

            return seed;
        }
    };


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

    std::ostream& operator << (std::ostream& os, const Triangle& t)
    {
        return os << "Triangle ["
            << "(" << t.p1.x << ", " << t.p1.y << "), "
            << "(" << t.p2.x << ", " << t.p2.y << "), "
            << "(" << t.p3.x << ", " << t.p3.y << ")] ";
    }

    struct TriangleHash
    {
        std::size_t operator()(const Triangle &t) const
        {
            std::size_t seed = 0;

            // Sort the points so that if there are two tris with the same
            // points but different order, then hash to the same value.
            std::array<const Vec2f*, 3> pts = { &t.p1, &t.p2, &t.p3 };
            std::sort(pts.begin(), pts.end(), vecLess);

            hash_combine(seed, pts[0]->x);
            hash_combine(seed, pts[0]->y);
            hash_combine(seed, pts[1]->x);
            hash_combine(seed, pts[1]->y);
            hash_combine(seed, pts[2]->x);
            hash_combine(seed, pts[2]->y);

            return seed;
        }
    };


    /*
     * findSuperTriangle
     *
     * Finds the super-triangle which encloses a given set of points in the 2D
     * plane. The points of the triangle will be in clockwise order.
     */
    static Triangle findSuperTriangle(std::vector<Vec2f> &vertices)
    {
        float minX = vertices[0].x;
        float minY = vertices[0].y;
        float maxX = minX;
        float maxY = minY;

        for (auto&& vert : vertices) {
            float x = vert.x;
            float y = vert.y;

            if (x < minX) { minX = x; }
            if (y < minY) { minY = y; }
            if (x > maxX) { maxX = x; }
            if (y > minY) { maxY = y; }
        }

        float dx = maxX - minX;
        float dy = maxY - minY;
        float maxD = std::max(dx, dy);
        float midX = (minX + maxX) / 2.0f;
        float midY = (minY + maxY) / 2.0f;

        return Triangle(Vec2f(midX - 20 * maxD, midY - maxD),
                        Vec2f(midX, midY + 20 * maxD),
                        Vec2f(midX + 20 * maxD, midY - maxD));
    }


    /*
     * delaunayTriangulate
     *
     * See function declaration for more detail.
     */
    std::vector<Triangle>
    delaunayTriangulate(std::vector<Vec2f> &vertices)
    {
        std::vector<Triangle> tris;

        // Find the super-triangle and add it to the list of tris.
        Triangle superTri = findSuperTriangle(vertices);
        tris.push_back(superTri);

        // Insert vertices into the mesh one at a time.
        for (auto&& v : vertices) {
            std::unordered_set<Triangle, TriangleHash> badTris;

            // Find all the tris that are no longer valid once the current
            // vertex is inserted.
            for (auto &&tri : tris) {
                if (tri.circumcircleContains(v)) {
                    badTris.insert(tri);
                }
            }

            std::vector<Edge> poly;

            std::unordered_set<Edge, EdgeHash> edges;
            std::unordered_set<Edge, EdgeHash> duplicateEdges;

            // Find all unique edges in the bad tris and add them to the poly.
            for (auto &&tri : badTris) {
                for (auto &&edge : tri.GetEdges()) {
                    if (edges.find(edge) != edges.end()) {
                        duplicateEdges.insert(edge);
                    } else {
                        edges.insert(edge);
                    }
                }
            }

            for (auto &&edge : edges) {
                if (duplicateEdges.find(edge) == duplicateEdges.end()) {
                    poly.push_back(edge);
                }
            }

            // Remove the bad triangles from the list of tris.
            tris.erase(std::remove_if(
                tris.begin(), tris.end(), [badTris](Triangle t) {
                    return badTris.find(t) != badTris.end();
                }), tris.end());

            // Create new triangles from the current vertex and the edges of
            // the polygon, and add them to the list of tris.
            for (auto &&edge : poly) {
                tris.push_back(Triangle(v, edge.p1, edge.p2));
            }
        }

        // Remove any triangles that contain a vertex from the super-triangle.
        tris.erase(std::remove_if(tris.begin(), tris.end(),
            [superTri](const Triangle &tri) {
                return superTri.hasCommonPoints(tri);
            }), tris.end());
        
        return tris;
    }


    std::vector<Edge>
    generateUrquhart(std::vector<Vec2f> &vertices)
    {
        std::vector<Triangle> delaunayTris = delaunayTriangulate(vertices);

        std::unordered_set<Edge, EdgeHash> edges;
        for (auto &&tri : delaunayTris) {
            auto triEdges = tri.GetEdges();
            auto &longestEdge =
                *std::maxElement(triEdges.begin(), triEdges.end(),
                                [](const Edge &e1, const Edge &e2) {
                                    e1.Length() < e2.Length();
                                });
            std::copy_if(triEdges.begin() triEdges.end(), edges.begin(),
                         [](const Edge &e) { return e != longestEdge; });
        }

        return edges;
    }
}
