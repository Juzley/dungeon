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
            virtual ~GameState() {}

            virtual void Draw() const = 0;
            virtual void Run() = 0;

            virtual void Start()
            {
            }
    };

    class GameStateManager {
        public:
            using GameStatePtr = std::shared_ptr<GameState>;

            void Push(GameStatePtr state)
            {
                m_states.push(state);
                state->Start();
            }

            void Replace(GameStatePtr state)
            {
                Pop();
                Push(state);
                state->Start();
            }

            void Pop(unsigned int count=1)
            {
                while (count-- > 0) {
                    if (!m_states.empty()) {
                        m_states.pop();
                    }
                }
            }

            void Draw() const
            {
                if (!m_states.empty()) {
                    m_states.top()->Draw();
                }
            }

            void Run()
            {
                if (!m_states.empty()) {
                    m_states.top()->Run();
                }

            }

            bool IsEmpty()
            {
                return m_states.empty();
            }

        private:
            std::stack<GameStatePtr>  m_states;
    };
}


#endif
