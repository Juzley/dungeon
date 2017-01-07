#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__


#include <SDL2/SDL.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <stdexcept>


namespace dungeon
{
    class Settings
    {
        public:
            enum Control {
                CONTROL_UP,
                CONTROL_LEFT,
                CONTROL_DOWN,
                CONTROL_RIGHT
            };

            void Load()
            {
                try {
                    read_json("settings.json", m_pt);
                } catch (const boost::property_tree::json_parser_error &e) {
                    // We couldn't open the settings file (possibly because
                    // this is the first time running, and it doesn't exist
                    // yet) - set the defaults.
                    m_pt.put("controls.up", SDLK_UP);
                    m_pt.put("controls.down", SDLK_DOWN);
                    m_pt.put("controls.left", SDLK_LEFT);
                    m_pt.put("controls.right", SDLK_RIGHT);
                }
            }

            void Save()
            {
                write_json("settings.json", m_pt);
            }

            void SetControl(Control c, SDL_Keycode k)
            {
                m_pt.put(ControlToProperty(c), k);
            }

            SDL_Keycode GetControl(Control c) const
            {
                // Use zero as the default arg to get() - the existence of the
                // property should be guaranteed as we have loaded the defaults
                // in Load(), so the default should never be used here.
                return m_pt.get(ControlToProperty(c), 0);
            }

        private:
            boost::property_tree::ptree m_pt;

            static const char *ControlToProperty(Control c)
            {
                switch (c)
                {
                case CONTROL_UP:
                    return "controls.up";
                case CONTROL_DOWN:
                    return "controls.down";
                case CONTROL_LEFT:
                    return "controls.left";
                case CONTROL_RIGHT:
                    return "controls.right";
                default:
                    throw std::runtime_error("Failure to look up property for control");
                }

            }
    };

    extern Settings g_settings;
}


#endif // __SETTINGS_HPP__

