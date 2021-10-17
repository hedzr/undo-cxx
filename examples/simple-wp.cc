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
        explicit UndoCmd(std::string const &default_state_info)
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
        explicit RedoCmd(std::string const &default_state_info)
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

} // namespace word_processor
namespace word_processor {

    namespace fct = undo_cxx::util::factory;

    class actions_controller {
    public:
        using State = std::string;
        using M = undo_cxx::undoable_cmd_system_t<State>;

        using UndoCmdT = UndoCmd<State>;
        using RedoCmdT = RedoCmd<State>;
        using FontStyleCmdT = FontStyleCmd<State>;

        using Factory = fct::factory<M::CmdT, UndoCmdT, RedoCmdT, FontStyleCmdT>;

        actions_controller() {}
        ~actions_controller() {}

        template<typename Cmd, typename... Args>
        void invoke(Args &&...args) {
            auto cmd = Factory::make_shared(undo_cxx::id_name<Cmd>(), args...);
            _undoable_cmd_system.invoke(cmd);
        }

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
    actions_controller controller;

    using FontStyleCmd = actions_controller::FontStyleCmdT;
    using UndoCmd = actions_controller::UndoCmdT;
    using RedoCmd = actions_controller::RedoCmdT;
    
    // do some stuffs

    // controller.invoke("word_processor::FontStyleCmd<std::__1::basic_string<char> >", "italic state1");
    controller.invoke<FontStyleCmd>("italic state1");
    controller.invoke<FontStyleCmd>("italic-bold state2");
    controller.invoke<FontStyleCmd>("underline state3");
    controller.invoke<FontStyleCmd>("italic state4");

    // and try to undo or redo

    controller.invoke<UndoCmd>("undo 1");
    controller.invoke<UndoCmd>("undo 2");

    controller.invoke<RedoCmd>("redo 1");

    controller.invoke<UndoCmd>("undo 3");
    controller.invoke<UndoCmd>("undo 4");

    controller.invoke("word_processor::RedoCmd", "redo 2 redo");
}

void test_undo_pre() {
    using namespace std::string_view_literals;
    std::string_view v1{"v1"};
    std::size_t v1_hash = std::hash<std::string_view>{}(v1);
    std::cout << v1 << ", hash = "sv << v1_hash << '\n';
    using namespace undo_cxx;
    using State = std::string;
    // using FontStyleCmd = word_processor::FontStyleCmd<State>;
    using UndoCmd = word_processor::UndoCmd<State>;
    using RedoCmd = word_processor::RedoCmd<State>;
#if !defined(_MSC_VER)
    std::cout << "id: " << id_gen<UndoCmd>{}(UndoCmd{}) << '\n';
    std::cout << "id: " << id_name<UndoCmd>() << '\n';
#else
    std::cout << "detail::type_name: " << debug::detail::type_name<UndoCmd>() << '\n';
    std::cout << "type_name        : " << debug::type_name<UndoCmd>() << '\n';
    std::cout << "id_name          : " << id_name<UndoCmd>() << '\n';
#endif
    dbg_debug("OK, UndoCmd's id = %s", std::string(id_name<UndoCmd>()).c_str());
    dbg_debug("OK, RedoCmd's id = %s", std::string(id_name<RedoCmd>()).c_str());
}

int main() {

    test_undo_pre();
    test_undo_sys();

    return 0;
}