#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__


#include <algorithm>
#include <iterator>
#include "item.hpp"


namespace dungeon
{
    class Player
    {
        public:
            void AddItems(ItemVector::iterator first,
                          ItemVector::iterator last)
            {
                std::copy(first, last, std::back_inserter(m_items));
            }

            // TODO: Encapsulate these properly
            unsigned int x;
            unsigned int y;

        private:
            ItemVector   m_items;
    };
}


#endif // __PLAYER_HPP__
