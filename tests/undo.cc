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
        UNDO_CXX_DEFINE_DEFAULT_CMD_TYPES(FontStyleCmd, undo_cxx::cmd_t);

    protected:
        void do_execute(CmdSP &sender, ContextT &) override {
            UNUSED(sender);
            // ... do sth to add/remove font style to/from
            // current selection in current editor ...
            std::cout << "<<" << _info << ">>" << '\n';
        }
        MementoPtr save_state_impl(CmdSP &sender, ContextT &) override {
            return std::make_unique<Memento>(sender, _info);
        }
        void undo_impl(CmdSP &sender, ContextT &, Memento &memento) override {
            memento = _info;
            memento.command(sender);
        }
        void redo_impl(CmdSP &sender, ContextT &, Memento &memento) override {
            memento = _info;
            memento.command(sender);
        }

    private:
        std::string _info{"make italic"};
    };

    template<typename State>
    class UndoCmd : public undo_cxx::base_undo_cmd_t<State> {
    public:
        ~UndoCmd() {}
        using undo_cxx::base_undo_cmd_t<State>::base_undo_cmd_t;
        UndoCmd(std::string const &default_state_info)
            : _info(default_state_info) {}
        UNDO_CXX_DEFINE_DEFAULT_CMD_TYPES(UndoCmd, undo_cxx::base_undo_cmd_t);

    protected:
        void do_execute(CmdSP &sender, ContextT &ctx) override {
            std::cout << "<<" << _info << ">>" << '\n';
            Base::do_execute(sender, ctx);
        }
        MementoPtr save_state_impl(CmdSP &sender, ContextT &) override {
            return std::make_unique<Memento>(sender, _info);
        }
        void undo_impl(CmdSP &sender, ContextT &, Memento &memento) override {
            memento = "do 'undo'";
            // auto sp = typename Memento::CmdSP(this);
            memento.command(sender);
        }
        void redo_impl(CmdSP &sender, ContextT &, Memento &memento) override {
            memento = "do 'un-undo' again";
            memento.command(sender);
        }

    private:
        std::string _info{"do 'undo'"};
    };

    template<typename State>
    class RedoCmd : public undo_cxx::base_redo_cmd_t<State> {
    public:
        ~RedoCmd() {}
        using undo_cxx::base_redo_cmd_t<State>::base_redo_cmd_t;
        RedoCmd(std::string const &default_state_info)
            : _info(default_state_info) {}
        UNDO_CXX_DEFINE_DEFAULT_CMD_TYPES(RedoCmd, undo_cxx::base_redo_cmd_t);

    protected:
        void do_execute(CmdSP &sender, ContextT &ctx) override {
            std::cout << "<<" << _info << ">>" << '\n';
            Base::do_execute(sender, ctx);
        }
        MementoPtr save_state_impl(CmdSP &sender, ContextT &) override {
            return std::make_unique<Memento>(sender, _info);
        }
        void undo_impl(CmdSP &sender, ContextT &, Memento &memento) override {
            memento = "do 'redo'";
            memento.command(sender);
        }
        void redo_impl(CmdSP &sender, ContextT &, Memento &memento) override {
            memento = "do 'un-redo' again";
            memento.command(sender);
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

    // do some stuffs

    undoable_cmd_system.invoke<FontStyleCmdT>("italic state1");
    undoable_cmd_system.invoke<FontStyleCmdT>("italic-bold state2");
    undoable_cmd_system.invoke<FontStyleCmdT>("underline state3");
    undoable_cmd_system.invoke<FontStyleCmdT>("italic state4");

    // and try to undo or redo

    undoable_cmd_system.invoke<UndoCmdT>("undo 1");
    undoable_cmd_system.invoke<UndoCmdT>("undo 2");
    undoable_cmd_system.invoke<RedoCmdT>("redo 1");
    undoable_cmd_system.invoke<UndoCmdT>("undo 3");
    undoable_cmd_system.invoke<UndoCmdT>("undo 4");
}

int main() {

    // test_undo_basic();
    test_undo_sys();

    return 0;
}