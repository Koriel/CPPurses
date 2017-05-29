#ifndef SYSTEM_MODULE_EVENTS_HIDE_EVENT_HPP
#define SYSTEM_MODULE_EVENTS_HIDE_EVENT_HPP

#include "system_module/event.hpp"

namespace cppurses {

class Hide_event : public Event {
   public:
    Hide_event() : Event{Event::Hide} {}
};

}  // namespace cppurses
#endif  // SYSTEM_MODULE_EVENTS_HIDE_EVENT_HPP