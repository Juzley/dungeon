#include "generator.hpp"

namespace dungeon
{
    class Room
    {
        public:
            Room(int          left,
                 int          top,
                 unsigned int width,
                 unsigned int height)
                : left(left), top(top), width(width), height(height)
            {
            }

            bool Intersects (Room &other,
                             int  *adjust_x = NULL,
                             int  *adjust_y = NULL)
            {
                if (Left() < other.Right() && Right() > other.Left() &&
                    Top() < other.Bottom() && Bottom() > other.Top()) {
                    // Intersects, find the intersection distance if required.

                    if (adjust_x != NULL) {
                        // Work out if moving left or right is shorter.
                        if (std::abs(Right() - other.Left()) >
                            std::abs(other.Right() - Left())) {
                            *adjust_x = other.Left() - Right();
                        } else {
                            *adjust_x = other.Right() - Left();
                        }
                    }
                    
                    if (adjust_y != NULL) {
                        // Work out if moving up or down is shorter.
                        if (std::abs(Bottom() - other.Top()) >
                            std::abs(other.Bottom() - Top())) {
                            *adjust_y = other.Top() - Bottom();
                        } else {
                            *adjust_y = other.Bottom() - Top();
                        }
                    }

                    return true;
                }

                return false;
            }

            void Move(int x, int y)
            {
                left += x;
                top += y;
            }

            int Left() const
            {
                return left;
            }

            int Right() const
            {
                return left + static_cast<int>(width);
            }

            int Top() const
            {
                return top;
            }

            int Bottom() const
            {
                return top + static_cast<int>(height);
            }

            unsigned int Width() const
            {
                return width;
            }
            
            unsigned int Height() const
            {
                return height;
            }

            int CenterX() const
            {
                return static_cast<int>(static_cast<float>(left) +
                                        static_cast<float>(width) / 2.0f);
            }

            int CenterY() const
            {
                return static_cast<int>(static_cast<float>(top) +
                                        static_cast<float>(height) / 2.0f);
            }

        private:
            int          left;
            int          top;
            unsigned int width;
            unsigned int height;
    };

    std::ostream& operator << (std::ostream& os, const Room& r)
    {
        return os << "Room [L " << r.Left() << ", R " << r.Right() <<
                    ", T " << r.Top() << ", B " << r.Bottom() << "]";
    }

    void Generator::Draw(SDL_Renderer *renderer) const
    {
    }
}
