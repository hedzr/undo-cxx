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


namespace dp { namespace undo { namespace test {
    
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
            return std::make_unique<Memento>(this, _info);
        }
        void undo_impl(Memento &memento) const override {
            memento = "make normal (de-italic)";
            memento.cmd(this);
        }
        void redo_impl(Memento &memento) const override {
            memento = "make italic again";
            memento.cmd(this);
        }

    private:
        std::string _info{"make italic"};
    };

    template<typename State>
    class UndoCmd : public undo_cxx::base_undo_cmd_t<State> {
    public:
        ~UndoCmd() {}
        UndoCmd() {}
        UndoCmd(std::string const &default_state_info)
            : _info(default_state_info) {}

        using Base = undo_cxx::base_undo_cmd_t<State>;
        using Memento = typename Base::Memento;
        using MementoPtr = typename Base::MementoPtr;
        using ContextT = typename Base::ContextT;

    protected:
        void do_execute(ContextT &ctx) const override {
            std::cout << "<<" << _info << ">>" << '\n';
            Base::do_execute(ctx);
        }
        MementoPtr save_state_impl() const override {
            return std::make_unique<Memento>(this, _info);
        }
        void undo_impl(Memento &memento) const override {
            memento = "do 'undo'";
            memento.cmd(this);
        }
        void redo_impl(Memento &memento) const override {
            memento = "do 'un-undo' again";
            memento.cmd(this);
        }

    private:
        std::string _info{"do 'undo'"};
    };

    template<typename State>
    class RedoCmd : public undo_cxx::base_redo_cmd_t<State> {
    public:
        ~RedoCmd() {}
        RedoCmd() {}
        RedoCmd(std::string const &default_state_info)
            : _info(default_state_info) {}

        using Base = undo_cxx::base_redo_cmd_t<State>;
        using Memento = typename Base::Memento;
        using MementoPtr = typename Base::MementoPtr;
        using ContextT = typename Base::ContextT;

    protected:
        void do_execute(ContextT &ctx) const override {
            std::cout << "<<" << _info << ">>" << '\n';
            Base::do_execute(ctx);
        }
        MementoPtr save_state_impl() const override {
            return std::make_unique<Memento>(this, _info);
        }
        void undo_impl(Memento &memento) const override {
            memento = "do 'redo'";
            memento.cmd(this);
        }
        void redo_impl(Memento &memento) const override {
            memento = "do 'un-redo' again";
            memento.cmd(this);
        }

    private:
        std::string _info{"do 'redo'"};
    };

}}} // namespace dp::undo::test
namespace dp { namespace undo { namespace bugs {
}}} // namespace dp::undo::bugs

void test_undo_sys() {
    using namespace dp::undo::test;
    using namespace dp::undo::bugs;

    using State = std::string;
    using M = undo_cxx::undoable_cmd_system_t<State>;
    using UndoCmdT = UndoCmd<State>;
    using RedoCmdT = RedoCmd<State>;
    using FontStyleCmdT = FontStyleCmd<State>;

    M undoable_cmd_system;

    undoable_cmd_system.invoke(FontStyleCmdT{"italic state1"});
    undoable_cmd_system.invoke(FontStyleCmdT{"italic-bold state2"});
    undoable_cmd_system.invoke(FontStyleCmdT{"underline state3"});
    undoable_cmd_system.invoke(FontStyleCmdT{"italic state4"});

    undoable_cmd_system.invoke(UndoCmdT{"undo 1"});
    undoable_cmd_system.invoke(UndoCmdT{"undo 2"});
    undoable_cmd_system.invoke(RedoCmdT{"redo 1"});
    undoable_cmd_system.invoke(UndoCmdT{"undo 3"});
    undoable_cmd_system.invoke(UndoCmdT{"undo 4"});
}

int main() {

    // test_undo_basic();
    test_undo_sys();

    return 0;
}