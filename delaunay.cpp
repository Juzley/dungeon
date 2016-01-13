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

        float x;
        float y;
    };


    struct Triangle
    {
        Triangle(Vec2f &p1, Vec2f &p2, Vec2f &p3)
            : p1(p1), p2(p2), p3(p3)
        {
        }

        Vec2f p1;
        Vec2f p2;
        Vec2f p3;
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

        // Find the super triangle and add it to the list of tris.
        tris.push_back(findSuperTriangle(vertices));
    }
}
