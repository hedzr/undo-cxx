//
// Created by Hedzr Yeh on 2021/9/26.
//


#include "undo_cxx.hh"

#include <iomanip>
#include <iostream>
#include <math.h>
#include <string>

#include <functional>
#include <memory>
#include <random>

#include <deque>
#include <list>
#include <optional>
#include <queue>
#include <stack>
#include <vector>

namespace dp { namespace undo { namespace basic {

    /**
     * @brief memento POJO container
     * @tparam State the object that is going to maintain the state of originator. It's just a POJO.
     */
    template<typename State>
    class memento_t {
    public:
        ~memento_t() = default;

        void push(State &&s) {
            _saved_states.emplace_back(s);
            dbg_print("  . save memento state : %s", undo_cxx::to_string(s).c_str());
        }
        std::optional<State> pop() {
            std::optional<State> ret;
            if (_saved_states.empty()) {
                return ret;
            }
            ret.emplace(_saved_states.back());
            _saved_states.pop_back();
            dbg_print("  . restore memento state : %s", undo_cxx::to_string(*ret).c_str());
            return ret;
        }
        auto size() const { return _saved_states.size(); }
        bool empty() const { return _saved_states.empty(); }
        bool can_pop() const { return !empty(); }

    private:
        std::list<State> _saved_states;
    };

    template<typename State, typename Memento = memento_t<State>>
    class originator_t {
    public:
        originator_t() = default;
        ~originator_t() = default;

        void set(State &&s) {
            _state = std::move(s);
            dbg_print("originator_t: set state (%s)", undo_cxx::to_string(_state).c_str());
        }
        void save_to_memento() {
            dbg_print("originator_t: save state (%s) to memento", undo_cxx::to_string(_state).c_str());
            _history.push(std::move(_state));
        }
        void restore_from_memento() {
            _state = *_history.pop();
            dbg_print("originator_t: restore state (%s) from memento", undo_cxx::to_string(_state).c_str());
        }

    private:
        State _state;
        Memento _history;
    };

    template<typename State>
    class caretaker {
    public:
        caretaker() = default;
        ~caretaker() = default;
        void run() {
            originator_t<State> o;
            o.set("state1");
            o.set("state2");
            o.save_to_memento();
            o.set("state3");
            o.save_to_memento();
            o.set("state4");

            o.restore_from_memento();
        }
    };

}}} // namespace dp::undo::basic
namespace dp { namespace undo { namespace bugs {
    int v_int = 0;
}}} // namespace dp::undo::bugs

void test_undo_basic() {
    using namespace dp::undo::basic;
    caretaker<std::string> c;
    c.run();
}

int main() {

    test_undo_basic();

    return 0;
}