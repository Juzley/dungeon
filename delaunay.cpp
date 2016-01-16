#include <cmath>

namespace delaunay
{
    struct Point
    {
        int x;
        int y;
    };


    struct Vec2f
    {
        Vec2f(float x, float y) : x(x), y(y)
        {
        }

        float Magnitude() const
        {
            return std::sqrt(x * x + y * y);
        }

        bool operator == (const Vec2f &v2)
        {
            return x == v2.x && y == v2.y;
        }

        float x;
        float y;
    };

    Vec2f operator - (const Vec2f &vl, const Vec2f &vr)
    {
        return Vec2f(vl.x - vr.x, vl.y - vr.y);
    }

    Vec2f operator + (const Vec2f &vl, const Vec2f &vr)
    {
        return Vec2f(vl.x + vr.x, vl.y + vr.y);
    }


    struct Edge
    {
        Edge(const Vec2f &p1, const Vec2f &p2) : p1(p1), p2(p2),
        {
        }

        bool operator == (const Edge& e2)
        {
            return ((p1 == e2.p1 && p2 == e2.p2) ||
                    (p1 == e2.p2 && p2 == e2.p1));
        }

        Vec2f p1;
        Vec2f p2;
    };


    class Triangle
    {
        public:
            Triangle(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3)
                : p1(p1), p2(p2), p3(p3)
            {
                edges = {Edge(p1, p2), Edge(p2, p3), Edge(p3, p1)};
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

            bool circumcircleContains(const Vec2f &v)
            {
                // Distance between circumcenter and point 
                float distance = (v - circumcenter).Magnitude();
                return distance <= circumradius;
            }

            bool circumcircleContains(const Point &p)
            {
                return circumCircleContains(Vec2f(p.x, p.y));
            }

            bool hasCommonPoints(const Triangle &t)
            {
                return (p1 == t.p1 || p1 == t.p2 || p1 == t.p3 ||
                        p2 == t.p1 || p2 == t.p2 || p2 == t.p3 ||
                        p3 == t.p1 || p3 == t.p2 || p3 == t.p3);
            }
            
            bool operator == (const Triangle &t2)
            {
                return (p1 == t2.p1 || p1 == t2.p2 || p1 == t2.p3) &&
                       (p2 == t2.p1 || p2 == t2.p2 || p1 == t2.p3) &&
                       (p3 == t2.p1 || p3 == t2.p2 || p3 == t2.p3);
            }

            const std::array<Edge, 3> & GetEdges()
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


    Triangle findSuperTriangle(std::vector<Point> &vertices)
    {
        float minX = vertices[0].x;
        float minY = vertices[0].y;
        float maxX = minX;
        float maxY = minY;

        for (auto&& vert : vertices) {
            float x = static_cast<float>(vert.x);
            float y = static_cast<float>(vert.y);

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


    std::vector<std::pair<Point, Point>>
    generateGraph(std::vector<Point> &vertices)
    {
        std::vector<Triangle> tris;

        // Find the super-triangle and add it to the list of tris.
        Triangle superTri = findSuperTriangle(vertices);
        tris.push_back(superTri);

        // Insert vertices into the mesh one at a time.
        for (auto&& v : vertices) {
            std::unordered_set<Triangle> badTris;

            // Find all the tris that are no longer valid once the current
            // vertex is inserted.
            for (auto &&tri : tris) {
                if (tri->circumcircleContains(v)) {
                    badTris.insert(t);
                }
            }

            std::vector<Edge> poly;
            std::unordered_set<Edge> edges;
            std::unordered_set<Edge> duplicateEdges;

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
                if (duplicateEdges.find() == duplicateEdges.end()) {
                    poly.push_back(edge);
                }
            }

            // Remove the bad triangles from the list of tris.
            tris.erase(tris.removeif(
                tris.begin(), tris.end(), [badTris](Triangle t) {
                     return badTris.find(t) != badTris.end();
                });

            // Create new triangles from the current vertex and the edges of
            // the polygon, and add them to the list of tris.
            for (auto &&edge : poly) {
                tris.push_back(Triangle(v, edge.p1, edge.p2));
            }
        }

        // Remove any triangles that contain a vertex from the super-triangle.
        std::remove_if(std::begin(), std::end(),
            [superTri](tri) { return superTri.hasCommonPoints(tri); });
    }
}
