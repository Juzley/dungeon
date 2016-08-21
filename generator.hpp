#include <SDL2/SDL.h>
#include <vector>


namespace dungeon
{
    class Room;

    class Generator
    {
        public:
            void Draw(SDL_Renderer *renderer) const;

        private:
            enum GeneratorStage {
                START,
                GENERATE_ROOMS,
                FIT_ROOMS,
                CONNECT_ROOMS,
                FIND_PATHS
            };
            std::vector<Room> m_rooms;
            GeneratorStage    m_stage;
    };
}
