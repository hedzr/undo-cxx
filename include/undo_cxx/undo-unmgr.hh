// undo_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/9.
//

#ifndef UNDO_CXX_UNDO_UNMGR_HH
#define UNDO_CXX_UNDO_UNMGR_HH

#include "undo-log.hh"

#include <iterator>
#include <list>
#include <vector>

// forward ref --------------------
namespace undo_cxx {

    template<typename State>
    class cmd_t;

    template<typename State>
    struct context_t;

    /**
     * @brief Undo Manager
     * @tparam State  the object that is going to maintain the state of originator. It's just a POJO.
     * @tparam Cmd 
     */
    template<typename State, typename Context = context_t<State>, typename Cmd = cmd_t<State>>
    class undo_system_t;

    template<typename State, typename Context = context_t<State>, typename Cmd = cmd_t<State>>
    using MgrT = undo_system_t<State, Context, Cmd>;

} // namespace undo_cxx

// context_t --------------------
namespace undo_cxx {

    template<typename State>
    struct context_t {
        using Cmd = cmd_t<State>;
        using ContextT = context_t<State>;
        using Mgr = MgrT<State, ContextT, Cmd>;
        Mgr &mgr;

        context_t(Mgr &m_)
            : mgr(m_) {}
    };

} // namespace undo_cxx

// cmd_t --------------------
namespace undo_cxx {

    template<typename State>
    class cmd_t {
    public:
        virtual ~cmd_t() {}

        using ContextT = context_t<State>;
        void execute(ContextT &ctx) const { do_execute(ctx); }

        using Memento = State;
        using MementoPtr = typename std::unique_ptr<Memento>;
        MementoPtr save_state() const { return save_state_impl(); }
        void undo(Memento &memento) const { undo_impl(memento); }
        void redo(Memento &memento) const { redo_impl(memento); }
        virtual bool can_be_memento() const { return true; }

    protected:
        virtual void do_execute(ContextT &ctx) const = 0;
        virtual MementoPtr save_state_impl() const = 0;
        virtual void undo_impl(Memento &memento) const = 0;
        virtual void redo_impl(Memento &memento) const = 0;
    };

    template<typename State>
    class UndoRedoCmdBase : public cmd_t<State> {
    public:
        ~UndoRedoCmdBase() {}
        bool can_be_memento() const override { return false; }
    };

    template<typename State>
    class UndoCmdBase : public UndoRedoCmdBase<State> {
    public:
        ~UndoCmdBase() {}

        using Base = UndoRedoCmdBase<State>;
        using Memento = typename Base::Memento;
        using MementoPtr = typename Base::MementoPtr;
        using ContextT = typename Base::ContextT;

    protected:
        void do_execute(ContextT &ctx) const override;
    };

    template<typename State>
    class RedoCmdBase : public UndoRedoCmdBase<State> {
    public:
        ~RedoCmdBase() {}

        using Base = UndoRedoCmdBase<State>;
        using Memento = typename Base::Memento;
        using MementoPtr = typename Base::MementoPtr;
        using ContextT = typename Base::ContextT;

    protected:
        void do_execute(ContextT &ctx) const override;
    };

} // namespace undo_cxx

// undo_system_t --------------------
namespace undo_cxx {

    template<typename State, typename Context, typename Cmd>
    class undo_system_t {
    public:
        ~undo_system_t() = default;

        using Memento = typename Cmd::Memento;
        using MementoPtr = typename std::unique_ptr<Memento>;
        // using Container = Stack;
        using Container = std::list<MementoPtr>;
        using Iterator = typename Container::iterator;
        using size_type = typename Container::size_type;
        using ContextT = Context;

        template<typename T, typename = void>
        struct has_save_state : std::false_type {};
        template<typename T>
        struct has_save_state<T, decltype(void(std::declval<T &>().save_state()))> : std::true_type {};

        template<typename T, typename = void>
        struct has_undo : std::false_type {};
        template<typename T>
        struct has_undo<T, decltype(void(std::declval<T &>().undo()))> : std::true_type {};

        template<typename T, typename = void>
        struct has_redo : std::false_type {};
        template<typename T>
        struct has_redo<T, decltype(void(std::declval<T &>().redo()))> : std::true_type {};

        template<typename T, typename = void>
        struct has_can_be_memento : std::false_type {};
        template<typename T>
        struct has_can_be_memento<T, decltype(void(std::declval<T &>().can_be_memento()))> : std::true_type {};

    public:
        void invoke(Cmd const &cmd) {
            cmd.execute(_ctx);
            if constexpr (has_can_be_memento<Cmd>::value) {
                if (cmd.can_be_memento())
                    save(cmd);
            }
        }
        void undo(Cmd const &undo_cmd) {
            if constexpr (has_undo<Cmd>::value) {
                undo_cmd.undo(_ctx, 1);
                return;
            }

            if (undo_one()) {
                // undo ok
            }
        }
        void redo(Cmd const &redo_cmd) {
            if constexpr (has_redo<Cmd>::value) {
                redo_cmd.redo(_ctx, 1);
                return;
            }

            if (redo_one()) {
                UNUSED(redo_cmd);
            }
        }

        void undo(Cmd const &undo_cmd, int delta) {
            if constexpr (has_undo<Cmd>::value) {
                undo_cmd.undo(_ctx, delta);
                return;
            }

            int pos = (int) position();
            int pos_new = pos - delta;
            if (pos_new > 0 && pos_new <= size()) {
                Iterator it = _position;
                std::advance(it, -delta);
                for (int i = 0; i < delta; i++) {
                    undo(undo_cmd);
                }
            }
        }
        void redo(Cmd const &redo_cmd, int delta) {
            if constexpr (has_redo<Cmd>::value) {
                redo_cmd.redo(_ctx, delta);
                return;
            }

            int pos = (int) position();
            int pos_new = pos + delta;
            if (pos_new > 0 && pos_new <= size()) {
                Iterator it = _position;
                std::advance(it, delta);
                for (int i = 0; i < delta; i++) {
                    redo(redo_cmd);
                }
            }
        }

        MementoPtr &focused_item() { return *_position; }
        MementoPtr const &focused_item() const { return *_position; }
        std::ptrdiff_t position() { return std::distance(_saved_states.begin(), _position); }
        auto size() const { return _saved_states.size(); }
        bool empty() const { return _saved_states.empty(); }
        bool can_restore() const { return !empty() && _position != _saved_states.begin(); }
        bool can_replay() const { return !empty() && _position != _saved_states.end(); }
        bool can_undo() const { return can_restore(); }
        bool can_redo() const { return can_replay(); }

    private:
        void save(Cmd const &cmd) {
            static_assert("expecting member function present: Memento Cmd::save_state()");
            if constexpr (has_save_state<Cmd>::value) {
                auto m = cmd.save_state();
                push(std::move(m));
            }
        }
        // void save(Memento &&s) { push(std::move(s)); }
        bool restore(MementoPtr &s) {
            return pop(s);
        }
        bool replay_one() { return replay_one_impl(); }
        bool undo_one() { return pop(); }
        bool redo_one() { return replay_one_impl(); }

    private:
        void push(MementoPtr &&s) {
            if (!empty()) {
                if (_position != _saved_states.end()) {
                    // erase [_position..)
                    _saved_states.erase(_position, _saved_states.end());
                }
            }

            // insert element at _position
            if constexpr (undo_cxx::traits::has_push_v<Container>) {
                _saved_states.push(std::move(s));
            } else if constexpr (undo_cxx::traits::has_emplace_v<Container>) {
                _saved_states.emplace(std::move(s));
            } else if constexpr (undo_cxx::traits::has_emplace_back_v<Container>) {
                _saved_states.emplace_back(std::move(s));
            } else {
                return;
            }
            _position = _saved_states.end();

            dbg_print("  . save memento state at position #%d: %s, size=%d", position(), undo_cxx::to_string(*_saved_states.back()).c_str(), size());
        }
        bool pop() {
            if (_saved_states.empty()) {
                return false;
            }

            if (_position == _saved_states.begin()) {
                return false;
            }

            Iterator it = _position;
            it--;
            _position = it;

            dbg_print("  . restore memento state at position #%d: %s, size=%d", position(), undo_cxx::to_string(*_position).c_str(), size());
            return true;
        }
        bool pop_old(MementoPtr &s) {
            if (_saved_states.empty()) {
                return false;
            }

            if (_position == _saved_states.begin()) {
                return false;
            }

            Iterator it = _position;
            it--;
            s = std::move(*it);
            _position = it;

            dbg_print("  . restore memento state at position #%d: %s, size=%d", position(), undo_cxx::to_string(*s).c_str(), size());
            return true;
        }
        bool replay_one_impl() {
            if (_saved_states.empty()) {
                return false;
            }

            if (_position == _saved_states.end()) {
                return false;
            }

            Iterator it = _position;
            auto &v = (*it);
            auto &vv = (*v);
            dbg_print("  . replay memento state at position #%d: %s, size=%d", position(), undo_cxx::to_string(vv).c_str(), size());
            if (_position != _saved_states.end()) {
                _position++;
            }

            return true;
        }

        std::optional<State> pop_orig() {
            std::optional<State> ret;
            if (_saved_states.empty()) {
                return ret;
            }
            ret.emplace(_saved_states.back());
            if constexpr (undo_cxx::traits::has_pop_v<Container>) {
                _saved_states.pop();
            } else if constexpr (undo_cxx::traits::has_pop_back_v<Container>) {
                _saved_states.pop_back();
            }
            if constexpr (has_undo<State>::value) {
                ret().undo();
            }
            dbg_print("  . restore memento state : %s", undo_cxx::to_string(*ret).c_str());
            return ret;
        }
        bool can_pop() const { return !empty(); }

    private:
        Container _saved_states{};
        Iterator _position{_saved_states.end()};
        ContextT _ctx{*this};
    };

} // namespace undo_cxx

// inline --------------------
namespace undo_cxx {

    template<typename State>
    inline void UndoCmdBase<State>::do_execute(ContextT &ctx) const {
        ctx.mgr.undo(*this);
    }

    template<typename State>
    inline void RedoCmdBase<State>::do_execute(ContextT &ctx) const {
        ctx.mgr.redo(*this);
    }

} // namespace undo_cxx

#endif //UNDO_CXX_UNDO_UNMGR_HH
