//
// Created by Hedzr Yeh on 2021/10/9.
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


//


namespace dp { namespace undo { namespace adv {


    template<typename State>
    class FontStyleCmd : public undo_cxx::cmd_t<State> {
    public:
        ~FontStyleCmd() {}
        FontStyleCmd() {}
        FontStyleCmd(std::string const &default_state_info)
            : _info(default_state_info) {}

        using Base = undo_cxx::cmd_t<State>;
        using Memento = typename Base::Memento;
        using MementoPtr = typename Base::MementoPtr;
        using ContextT = typename Base::ContextT;

    protected:
        void do_execute(ContextT &) const override {
            std::cout << "<<" << _info << ">>" << '\n';
        }
        MementoPtr save_state_impl() const override {
            return std::make_unique<Memento>(_info);
        }
        void undo_impl(Memento &memento) const override {
            memento = "make normal (de-italic)";
        }
        void redo_impl(Memento &memento) const override {
            memento = "make italic again";
        }

    private:
        std::string _info{"make italic"};
    };

    template<typename State>
    class UndoCmd : public undo_cxx::UndoCmdBase<State> {
    public:
        ~UndoCmd() {}
        UndoCmd() {}
        UndoCmd(std::string const &default_state_info)
            : _info(default_state_info) {}

        using Base = undo_cxx::UndoCmdBase<State>;
        using Memento = typename Base::Memento;
        using MementoPtr = typename Base::MementoPtr;
        using ContextT = typename Base::ContextT;

        bool can_be_memento() const override { return false; }

    protected:
        void do_execute(ContextT &ctx) const override {
            std::cout << "<<" << _info << ">>" << '\n';
            Base::do_execute(ctx);
        }
        MementoPtr save_state_impl() const override {
            return std::make_unique<Memento>(_info);
        }
        void undo_impl(Memento &memento) const override {
            memento = "do 'undo'";
        }
        void redo_impl(Memento &memento) const override {
            memento = "do 'un-undo' again";
        }

    private:
        std::string _info{"do 'undo'"};
    };

    template<typename State>
    class RedoCmd : public undo_cxx::RedoCmdBase<State> {
    public:
        ~RedoCmd() {}
        RedoCmd() {}
        RedoCmd(std::string const &default_state_info)
            : _info(default_state_info) {}

        using Base = undo_cxx::RedoCmdBase<State>;
        using Memento = typename Base::Memento;
        using MementoPtr = typename Base::MementoPtr;
        using ContextT = typename Base::ContextT;

    protected:
        void do_execute(ContextT &ctx) const override {
            std::cout << "<<" << _info << ">>" << '\n';
            Base::do_execute(ctx);
        }
        MementoPtr save_state_impl() const override {
            return std::make_unique<Memento>(_info);
        }
        void undo_impl(Memento &memento) const override {
            memento = "do 'redo'";
        }
        void redo_impl(Memento &memento) const override {
            memento = "do 'un-redo' again";
        }

    private:
        std::string _info{"do 'redo'"};
    };

}}} // namespace dp::undo::adv
namespace dp { namespace undo { namespace bugs {
}}} // namespace dp::undo::bugs

void test_undo_advanced() {
    using namespace dp::undo::adv;
    using namespace dp::undo::bugs;

    using State = std::string;
    using M = undo_cxx::undo_system_t<State>;
    using UndoCmdT = UndoCmd<State>;
    using RedoCmdT = RedoCmd<State>;
    using FontStyleCmdT = FontStyleCmd<State>;

    M m;

    m.invoke(FontStyleCmdT{"italic state1"});
    m.invoke(FontStyleCmdT{"italic-bold state2"});
    m.invoke(FontStyleCmdT{"underline state3"});
    m.invoke(FontStyleCmdT{"italic state4"});

    m.invoke(UndoCmdT{"undo 1"});
    m.invoke(UndoCmdT{"undo 2"});
    m.invoke(RedoCmdT{"redo 1"});
    m.invoke(UndoCmdT{"undo 3"});
    m.invoke(UndoCmdT{"undo 4"});
}

int main() {

    // test_undo_basic();
    test_undo_advanced();

    return 0;
}