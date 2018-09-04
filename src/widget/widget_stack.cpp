#include <cppurses/widget/widgets/widget_stack.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include <signals/slot.hpp>

#include <cppurses/system/focus.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

void Widget_stack::set_active_page(std::size_t index) {
    if (index > this->size()) {
        return;
    }
    active_page_ = this->children.get()[index].get();
    this->enable(this->enabled(), false);
    if (sets_focus_) {
        Focus::set_focus_to(active_page_);
    }
    this->page_changed(index);
}

void Widget_stack::sets_focus_on_change(bool sets_focus) {
    sets_focus_ = sets_focus;
}

void Widget_stack::add_page(std::unique_ptr<Widget> widget) {
    widget->disable();
    this->children.add(std::move(widget));
}

void Widget_stack::insert_page(std::size_t index,
                               std::unique_ptr<Widget> widget) {
    widget->disable();
    this->children.insert(std::move(widget), index);
}

void Widget_stack::remove_page(std::size_t index) {
    if (index >= this->size()) {
        return;
    }
    Widget* at_index{this->children.get()[index].get()};
    if (at_index == this->active_page()) {
        active_page_ = nullptr;
    }
    at_index->close();
}

void Widget_stack::clear() {
    active_page_ = nullptr;
    while (!this->children.get().empty()) {
        this->children.get().front()->close();
    }
}

std::size_t Widget_stack::size() const {
    return this->children.get().size();
}

Widget* Widget_stack::active_page() const {
    return active_page_;
}

std::size_t Widget_stack::active_page_index() const {
    if (active_page_ == nullptr) {
        return 0;  // TODO need better value here.
    }
    const auto& child_vec = this->children.get();

    auto at = std::find_if(
        std::begin(child_vec), std::end(child_vec),
        [this](const auto& w_ptr) { return w_ptr.get() == active_page_; });
    return std::distance(std::begin(child_vec), at);
}

void Widget_stack::enable(bool enable, bool post_child_polished_event) {
    this->enable_and_post_events(enable, post_child_polished_event);
    for (const std::unique_ptr<Widget>& child : this->children.get()) {
        if (child.get() == active_page_) {
            child->enable(enable, post_child_polished_event);
        } else {
            child->disable();
        }
    }
}

namespace slot {

sig::Slot<void(std::size_t)> set_active_page(Widget_stack& stack) {
    sig::Slot<void(std::size_t)> slot{
        [&stack](auto index) { stack.set_active_page(index); }};
    slot.track(stack.destroyed);
    return slot;
}

sig::Slot<void()> set_active_page(Widget_stack& stack, std::size_t index) {
    sig::Slot<void()> slot{[&stack, index] { stack.set_active_page(index); }};
    slot.track(stack.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> remove_page(Widget_stack& stack) {
    sig::Slot<void(std::size_t)> slot{
        [&stack](auto index) { stack.remove_page(index); }};
    slot.track(stack.destroyed);
    return slot;
}

sig::Slot<void()> remove_page(Widget_stack& stack, std::size_t index) {
    sig::Slot<void()> slot{[&stack, index] { stack.remove_page(index); }};
    slot.track(stack.destroyed);
    return slot;
}

sig::Slot<void(std::size_t, std::unique_ptr<Widget>)> insert_page(
    Widget_stack& stack) {
    sig::Slot<void(std::size_t, std::unique_ptr<Widget>)> slot{
        [&stack](auto index, auto widget) {
            stack.insert_page(index, std::move(widget));
        }};
    slot.track(stack.destroyed);
    return slot;
}

sig::Slot<void(std::unique_ptr<Widget>)> insert_page(Widget_stack& stack,
                                                     std::size_t index) {
    sig::Slot<void(std::unique_ptr<Widget>)> slot{[&stack, index](auto widget) {
        stack.insert_page(index, std::move(widget));
    }};
    slot.track(stack.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
