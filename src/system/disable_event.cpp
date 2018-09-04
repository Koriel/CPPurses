#include <cppurses/system/events/disable_event.hpp>

#include <cppurses/painter/detail/screen_descriptor.hpp>
#include <cppurses/painter/detail/screen_state.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/event_handler.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
Disable_event::Disable_event(Event_handler* receiver)
    : Event{Event::Disable, receiver} {}

bool Disable_event::send() const {
    Widget* widg{static_cast<Widget*>(receiver_)};
    widg->screen_state().tiles.clear();
    return receiver_->disable_event();
}

bool Disable_event::filter_send(Event_handler* filter) const {
    return filter->disable_event_filter(receiver_);
}

}  // namespace cppurses
