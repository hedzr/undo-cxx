// undo_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/14.
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


namespace word_processor {

    template<typename State>
    class FontStyleCmd : public undo_cxx::cmd_t<State> {
    public:
        ~FontStyleCmd() {}
        FontStyleCmd() {}
        explicit FontStyleCmd(std::string const &default_state_info)
            : _info(default_state_info) {}
        UNDO_CXX_DEFINE_DEFAULT_CMD_TYPES(FontStyleCmd, undo_cxx::cmd_t);

    protected:
        void do_execute(CmdSP &sender, ContextT &) override {
            UNUSED(sender);
            // ... do sth to add/remove font style to/from
            // current selection in current editor ...
            std::cout << "<<" << _info << ">>" << '\n';
        }
        MementoPtr save_state_impl(CmdSP &sender) override {
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
        UndoCmd() {}
        explicit UndoCmd(std::string const &default_state_info)
            : _info(default_state_info) {}
        UNDO_CXX_DEFINE_DEFAULT_CMD_TYPES(UndoCmd, undo_cxx::base_undo_cmd_t);

    protected:
        void do_execute(CmdSP &sender, ContextT &ctx) override {
            std::cout << "<<" << _info << ">>" << '\n';
            Base::do_execute(sender, ctx);
        }
        MementoPtr save_state_impl(CmdSP &sender) override {
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
        RedoCmd() {}
        explicit RedoCmd(std::string const &default_state_info)
            : _info(default_state_info) {}
        UNDO_CXX_DEFINE_DEFAULT_CMD_TYPES(RedoCmd, undo_cxx::base_redo_cmd_t);

    protected:
        void do_execute(CmdSP &sender, ContextT &ctx) override {
            std::cout << "<<" << _info << ">>" << '\n';
            Base::do_execute(sender, ctx);
        }
        MementoPtr save_state_impl(CmdSP &sender) override {
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

} // namespace word_processor
namespace word_processor {

    namespace fct = undo_cxx::util::factory;

    class actions_mgr {
    public:
        using State = std::string;
        using M = undo_cxx::undoable_cmd_system_t<State>;
        using UndoCmdT = UndoCmd<State>;
        using RedoCmdT = RedoCmd<State>;
        using FontStyleCmdT = FontStyleCmd<State>;

        using Factory = fct::factory<M::CmdT, UndoCmdT, RedoCmdT, FontStyleCmdT>;

        actions_mgr() {
            // std::cerr << undo_cxx::debug::type_name<FontStyleCmdT>() << '\n';
        }
        ~actions_mgr() {}

        template<typename... Args>
        void invoke(char const *const cmd_id_name, Args &&...args) {
            auto cmd = Factory::make_shared(cmd_id_name, args...);
            _undoable_cmd_system.invoke(cmd);
        }

        void invoke(typename M::CmdSP &cmd) {
            _undoable_cmd_system.invoke(cmd);
        }

    private:
        M _undoable_cmd_system;
    };

} // namespace word_processor

void test_undo_sys() {
    using namespace word_processor;
    actions_mgr mgr;

    // do some stuffs
    UNUSED(mgr);
    // mgr.invoke("word_processor::FontStyleCmd<std::__1::basic_string<char> >", "italic state1");
    mgr.invoke("word_processor::FontStyleCmd", "italic state1");
    mgr.invoke("word_processor::FontStyleCmd", "italic-bold state2");
    mgr.invoke("word_processor::FontStyleCmd", "underline state3");
    mgr.invoke("word_processor::FontStyleCmd", "italic state4");

    // and try to undo or redo

    mgr.invoke("word_processor::UndoCmd", "undo 1");
    mgr.invoke("word_processor::UndoCmd", "undo 2");

    mgr.invoke("word_processor::RedoCmd", "redo 1");

    mgr.invoke("word_processor::UndoCmd", "undo 3");
    mgr.invoke("word_processor::UndoCmd", "undo 4");
}

void test_undo_pre() {
    std::string_view v1{"v1"};
    std::size_t v1_hash = std::hash<std::string_view>{}(v1);
    std::cout << v1 << ", hash = " << v1_hash << '\n';

    using namespace undo_cxx;
    using UndoCmd = word_processor::UndoCmd<std::string>;
    std::cout << "id: " << id_gen<UndoCmd>{}(UndoCmd{}) << '\n';
}

int main() {

    test_undo_pre();
    test_undo_sys();

    return 0;
}