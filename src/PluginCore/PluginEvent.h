
#ifndef H_FB_PLUGINEVENT
#define H_FB_PLUGINEVENT

#include <string>
#include <map>

namespace FB {

    /** 
     * PluginEvent
     *
     * This is the base class for all OS events that are used in the plugin;
     * generally these come from the PluginWindow class (or a specialization
     * thereof)
     **/
    class PluginEvent
    {
    public:
        PluginEvent() { };
        virtual ~PluginEvent() { };

    public:

        template<class T>
        T* get()
        {
            T* out(dynamic_cast<T>(this));
            if (out == NULL) {
                throw std::invalid_argument("Invalid message type");
            }
        }

        template<class T>
        bool validType()
        {
            T* out(dynamic_cast<T>(this));
            return out != NULL;
        }
    };
};

#endif