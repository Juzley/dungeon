#ifndef __GAMESTATE_HPP__
#define __GAMESTATE_HPP__


#include <SDL2/SDL.h>
#include <stack>
#include <memory>


namespace dungeon
{
    class GameState
    {
        public:
            virtual void Draw(SDL_Renderer *renderer) const = 0;
            virtual void Run() = 0;
    };

    class GameStateManager {
        public:
            using GameStatePtr = std::shared_ptr<GameState>;

            void Push(GameStatePtr state)
            {
                m_states.push(state);
            }

            void Replace(GameStatePtr state)
            {
                Pop();
                Push(state);
            }

            void Pop()
            {
                if (!m_states.empty()) {
                    m_states.pop();
                }
            }

            void Draw(SDL_Renderer *renderer) const
            {
                if (!m_states.empty()) {
                    m_states.top()->Draw(renderer);
                }
            }

            void Run()
            {
                if (!m_states.empty()) {
                    m_states.top()->Run();
                }

            }
        private:
            std::stack<GameStatePtr>  m_states;
    };
}


#endif