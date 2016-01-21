#include <cmath>
#include <array>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <iostream>

#include "graph.hpp"
#include "util.hpp"


namespace graph
{
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


    std::ostream& operator << (std::ostream& os, const Edge& e)
    {
        return os << "Edge [(" << e.p1.x << ", " << e.p1.y << "), ("
            << e.p2.x << ", " << e.p2.y << ")]";
    }

    struct EdgeHash
    {
        std::size_t operator()(const Edge &e) const
        {
            std::size_t seed = 0;

            // Sort the points so that if there are two edges with the same
            // points but different order, then hash to the same value.
            std::array<const Vec2f*, 2> pts = { &e.p1, &e.p2 };
            std::sort(pts.begin(), pts.end(), vecLess);

            util::hash_combine(seed, pts[0]->x);
            util::hash_combine(seed, pts[0]->y);
            util::hash_combine(seed, pts[1]->x);
            util::hash_combine(seed, pts[1]->y);

            return seed;
        }
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

            util::hash_combine(seed, pts[0]->x);
            util::hash_combine(seed, pts[0]->y);
            util::hash_combine(seed, pts[1]->x);
            util::hash_combine(seed, pts[1]->y);
            util::hash_combine(seed, pts[2]->x);
            util::hash_combine(seed, pts[2]->y);

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
     * generateDelaunay
     *
     * See function declaration for more detail.
     */
    std::vector<Triangle>
    generateDelaunay(std::vector<Vec2f> &vertices)
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


    /*
     * generateUrquhart
     *
     * See function declaration for more detail.
     */
    std::vector<Edge>
    generateUrquhart(std::vector<Vec2f> &vertices)
    {
        std::vector<Triangle> delaunayTris = generateDelaunay(vertices);

        // Generate the set of edges of the delaunay triangles.
        std::unordered_set<Edge, EdgeHash> edges;
        for (auto &&tri : delaunayTris) {
            auto triEdges = tri.GetEdges();
            edges.insert(triEdges.begin(), triEdges.end());
        }

        // Remove the longest edge from each triangle from the set to give the
        // Urquhart graph.
        for (auto &&tri : delaunayTris) {
            auto triEdges = tri.GetEdges();
            auto &longestEdge =
                *std::max_element(triEdges.begin(), triEdges.end(),
                                [](const Edge &e1, const Edge &e2) {
                                    return e1.Length() < e2.Length();
                                });

            edges.erase(longestEdge);
        }

        std::vector<Edge> result;
        result.assign(edges.begin(), edges.end());
        return result;
    }
}
