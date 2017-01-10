#ifndef __ITEM_HPP__
#define __ITEM_HPP__


#include <memory>
#include <vector>


namespace dungeon
{
    class Item
    {
        public:
            virtual ~Item() {};
    };

    using ItemVector = std::vector<std::shared_ptr<Item>>;
}


#endif // __ITEM_HPP__
