// undo_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/9.
//

#ifndef UNDO_CXX_UNDO_ZCORE_HH
#define UNDO_CXX_UNDO_ZCORE_HH

#include "undo-log.hh"

#include <iterator>
#include <list>
#include <vector>

#include <limits.h> // SIZE_T_MAX
#if defined(_MSC_VER)
#define SIZE_T_MAX ULONG_MAX
#endif

// forward ref --------------------
namespace undo_cxx {

    class base_cmd_t;

    template<typename State, typename Base = base_cmd_t>
    class cmd_t;

    template<typename State>
    struct context_t;

    /**
     * @brief Undo manager and Command system manager.
     * @tparam State  the object that is going to maintain the state of originator. It's just a POJO.
     * @tparam Cmd 
     * @details undoable command system is a bundle of undo/redo and commands manager.
     */
    template<typename State,
             typename Context = context_t<State>,
             typename BaseCmdT = base_cmd_t,
             template<class S, class B> typename RefCmdT = cmd_t,
             typename Cmd = RefCmdT<State, BaseCmdT>>
    class undoable_cmd_system_t;

    template<typename State,
             typename Context = context_t<State>,
             typename BaseCmdT = base_cmd_t,
             template<class S, class B> typename RefCmdT = cmd_t,
             typename Cmd = RefCmdT<State, BaseCmdT>>
    using MgrT = undoable_cmd_system_t<State, Context, BaseCmdT, RefCmdT, Cmd>;

} // namespace undo_cxx

// context_t --------------------
namespace undo_cxx {

    template<typename State>
    struct context_t {
        using BaseCmdT = base_cmd_t;
        template<class S, class B>
        using RefCmdT = cmd_t<S, B>;
        using Cmd = RefCmdT<State, BaseCmdT>;
        using ContextT = context_t<State>;
        using Mgr = undoable_cmd_system_t<State, ContextT, BaseCmdT, cmd_t, Cmd>;

        Mgr &mgr;

        context_t(Mgr &m_)
            : mgr(m_) {}
    };

} // namespace undo_cxx

// state_t --------------------
namespace undo_cxx {
    template<typename State,
             typename BaseCmdT = base_cmd_t,
             template<class S, class B> typename RefCmdT = cmd_t,
             typename Cmd = RefCmdT<State, BaseCmdT>>
    struct state_t {
        using StateT = State;
        using CmdT = Cmd;
        // using CmdPtr = CmdT const *; // std::weak_ptr<const CmdT>;
        // using CmdWP = std::weak_ptr<CmdT const>;
        // using CmdSPC = std::shared_ptr<CmdT const>;
        using CmdSP = std::shared_ptr<CmdT>;
        using Pair = std::pair<CmdSP, StateT>;
        using PairVec = std::vector<Pair>;

        state_t() = default;
        ~state_t() = default;
        state_t(CmdSP &c, StateT const &s) { pairs.push_back(Pair{c, s}); }

        void add_state(CmdSP &c, StateT const &s) { emplace_back(c, s); }
        void emplace_back(CmdSP &c, StateT const &s) { pairs.emplace_back(c, s); }

        template<class _InputIterator, class _Function>
        _Function for_each_children(_Function &&fn) {
            auto it = pairs.begin();
            it++;
            return std::for_each(it, pairs.end(), fn);
        }

        CmdSP &command() { return pairs[0].first; }
        state_t &command(CmdSP &c) {
            if (pairs.empty())
                pairs.emplace_back(c, StateT{});
            else
                pairs[0].first = CmdSP(c);
            return (*this);
        }
        StateT const &operator()() const { return pairs[0].second; }
        StateT &operator()() { return pairs[0].second; }

        state_t &operator=(StateT const &s) {
            if (pairs.empty())
                pairs.emplace_back(CmdSP{}, s);
            else
                pairs[0].second = s;
            return (*this);
        }

        friend std::ostream &operator<<(std::ostream &os, state_t const &o) {
            return os << o();
        }

    private:
        template<class _InputIterator, class _Function>
        _Function for_each(_Function &&fn) {
            return std::for_each(pairs.begin(), pairs.end(), fn);
        }

    private:
        PairVec pairs{};
    };
} // namespace undo_cxx

// cmd_t --------------------
namespace undo_cxx {

    class base_cmd_t {
    public:
        virtual ~base_cmd_t() {}
    };

    template<typename State, typename Base>
    class cmd_t : public Base {
    public:
        virtual ~cmd_t() {}

        using Self = cmd_t<State, Base>;
        using CmdSP = std::shared_ptr<Self>;
        using CmdSPC = std::shared_ptr<Self const>;
        using CmdId = std::string_view;
        CmdId id() const { return debug::type_name<Self>(); }

        using ContextT = context_t<State>;
        void execute(CmdSP &sender, ContextT &ctx) { do_execute(sender, ctx); }

        using StateT = State;
        using StateUniPtr = std::unique_ptr<StateT>;
        using Memento = state_t<StateT>;
        using MementoPtr = typename std::unique_ptr<Memento>;
        MementoPtr save_state(CmdSP &sender, ContextT &ctx) { return save_state_impl(sender, ctx); }
        void undo(CmdSP &sender, ContextT &ctx, Memento &memento) { undo_impl(sender, ctx, memento); }
        void redo(CmdSP &sender, ContextT &ctx, Memento &memento) { redo_impl(sender, ctx, memento); }
        virtual bool can_be_memento() const { return true; }

    protected:
        virtual void do_execute(CmdSP &sender, ContextT &ctx) = 0;
        virtual MementoPtr save_state_impl(CmdSP &sender, ContextT &ctx) = 0;
        virtual void undo_impl(CmdSP &sender, ContextT &ctx, Memento &memento) = 0;
        virtual void redo_impl(CmdSP &sender, ContextT &ctx, Memento &memento) = 0;
    };

} // namespace undo_cxx

#define UNDO_CXX_DEFINE_CMD_TYPES()               \
    using Base = Super;                           \
    using CmdSP = typename Super::CmdSP;          \
    using CmdId = typename Super::CmdId;          \
    using Memento = typename Base::Memento;       \
    using MementoPtr = typename Base::MementoPtr; \
    using ContextT = typename Base::ContextT

#define UNDO_CXX_DEFINE_DEFAULT_CMD_TYPES(SelfType, SuperType) \
    using Super = SuperType<State>;                            \
    using Self = SelfType<State>;                              \
    UNDO_CXX_DEFINE_CMD_TYPES()

// composite_cmd_t --------------------
namespace undo_cxx {

    /**
     * @brief A composite command which groups the multiple commands as one.
     */
    template<typename State, typename BaseCmdT = base_cmd_t,
             template<class S, class B> typename RefCmdT = cmd_t>
    class composite_cmd_t : public RefCmdT<State, BaseCmdT> {
    public:
        virtual ~composite_cmd_t() {}

        using Super = RefCmdT<State, BaseCmdT>;
        using Self = composite_cmd_t<State, BaseCmdT, RefCmdT>;
        UNDO_CXX_DEFINE_CMD_TYPES();
        // using CmdSP = typename Super::CmdSP;
        // using CmdId = typename Super::CmdId;
        // using ContextT = typename CmdT::ContextT;
        // using Memento = typename CmdT::Memento;
        // using MementoPtr = typename CmdT::MementoPtr;
        using StateT = State;
        using CmdT = typename Super::Self; //RefCmdT<StateT, BaseCmdT>;
        using CmdPtr = CmdT const *;
        using ContainerT = std::list<CmdSP>;


    public:
        void add_command(CmdSP &&cmd) {
            _commands.emplace_back(cmd);
        }
        template<class _InputIterator, class _Function>
        _Function for_each(_Function &&fn) {
            return std::for_each(_commands.begin(), _commands.end(), fn);
        }

    protected:
        virtual void do_execute(CmdSP &sender, ContextT &ctx) override {
            UNUSED(sender);
            for_each([&](CmdSP &cmd) {
                cmd->do_execute(ctx);
            });
        }
        virtual MementoPtr save_state_impl(CmdSP &sender, ContextT &) override {
            MementoPtr r = std::make_unique<Memento>(this, StateT{});
            for_each([sender, r, this](CmdSP &cmd) {
                r->emplace_back(cmd, cmd->save_state_impl(sender));
            });
            return r;
        }
        virtual void undo_impl(CmdSP &sender, ContextT &ctx, Memento &memento) override {
            UNUSED(memento);
            if (memento.command().get() == this) {
                // composite memento (state_t):
                memento.for_each_children([ctx](typename Memento::Pair const &item) {
                    item.first.undo_impl(item.first, ctx, item.second);
                });
            } else {
                for_each([sender, ctx, memento, this](CmdSP &cmd) {
                    cmd->undo(sender, ctx, memento);
                });
            }
        }
        virtual void redo_impl(CmdSP &sender, ContextT &ctx, Memento &memento) override {
            UNUSED(memento);
            if (memento.command() == this) {
                // composie memento (state_t):
                memento.for_each_children([ctx](typename Memento::Pair const &item) {
                    item.first.redo_impl(item.first, ctx, item.second);
                });
            } else {
                for_each([sender, ctx, memento, this](CmdSP &cmd) {
                    cmd->redo(sender, ctx, memento);
                });
            }
        }

    private:
        ContainerT _commands;
    };

} // namespace undo_cxx

// base_undo_redo_base_cmd_t --------------------
// base_undo_cmd_t, base_redo_cmd_t
namespace undo_cxx {

    template<typename State, typename BaseCmdT = base_cmd_t,
             template<class S, class B> typename RefCmdT = cmd_t>
    class base_undo_redo_base_cmd_t : public RefCmdT<State, BaseCmdT> {
    public:
        ~base_undo_redo_base_cmd_t() {}
        base_undo_redo_base_cmd_t(int delta_ = 1)
            : _delta(delta_) {}

        /** @brief don't record this command into the undo/redo history */
        virtual bool can_be_memento() const override { return false; }

        int delta() const { return _delta; }
        void delta(int n) { _delta = n; }

    protected:
        int _delta{1};
    };

    /** @brief your UndoCmd should be derived from base_undo_cmd_t */
    template<typename State, typename BaseCmdT = base_cmd_t,
             template<class S, class B> typename RefCmdT = cmd_t>
    class base_undo_cmd_t : public base_undo_redo_base_cmd_t<State, BaseCmdT, RefCmdT> {
    public:
        using base_undo_redo_base_cmd_t<State, BaseCmdT, RefCmdT>::base_undo_redo_base_cmd_t;
        ~base_undo_cmd_t() {}
        UNDO_CXX_DEFINE_DEFAULT_CMD_TYPES(base_undo_cmd_t, base_undo_redo_base_cmd_t);

    protected:
        virtual void do_execute(CmdSP &sender, ContextT &ctx) override;
    };

    /** @brief your RedoCmd should be derived from base_redo_cmd_t */
    template<typename State, typename BaseCmdT = base_cmd_t,
             template<class S, class B> typename RefCmdT = cmd_t>
    class base_redo_cmd_t : public base_undo_redo_base_cmd_t<State, BaseCmdT, RefCmdT> {
    public:
        using base_undo_redo_base_cmd_t<State, BaseCmdT, RefCmdT>::base_undo_redo_base_cmd_t;
        ~base_redo_cmd_t() {}
        UNDO_CXX_DEFINE_DEFAULT_CMD_TYPES(base_redo_cmd_t, base_undo_redo_base_cmd_t);

    protected:
        virtual void do_execute(CmdSP &sender, ContextT &ctx) override;
    };

} // namespace undo_cxx

// undoable_cmd_system_t --------------------
namespace undo_cxx {

    template<typename State,
             typename Context,
             typename BaseCmdT,
             template<class S, class B> typename RefCmdT,
             typename Cmd>
    class undoable_cmd_system_t {
    public:
        ~undoable_cmd_system_t() = default;

        using StateT = State;
        using ContextT = Context;
        using CmdT = Cmd;
        using CmdSP = std::shared_ptr<CmdT>;
        using Memento = typename CmdT::Memento;
        using MementoPtr = typename std::unique_ptr<Memento>;
        // using Container = Stack;
        using Container = std::list<MementoPtr>;
        using Iterator = typename Container::iterator;

        using size_type = typename Container::size_type;

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
        void invoke(CmdSP &cmd) {
            cmd->execute(cmd, _ctx);
            // if constexpr (has_can_be_memento<CmdSP>::value) {
            if (cmd->can_be_memento())
                save(cmd);
            // }
        }
        template<typename ConcreteCmd, typename... Args>
        void invoke(Args &&...args) {
            CmdSP sp = std::make_shared<ConcreteCmd>(args...);
            invoke(sp);
        }
        void undo(CmdSP &undo_cmd) {
            if constexpr (has_undo<CmdT>::value) {
                // needs void undo_cmd::undo(sender, ctx, delta)
                undo_cmd->undo(undo_cmd, _ctx, 1);
                return;
            }

            if (undo_one()) {
                // undo ok
            }
        }
        void redo(CmdSP &redo_cmd) {
            if constexpr (has_redo<CmdT>::value) {
                // needs void redo_cmd::redo(sender, ctx, delta)
                redo_cmd->redo(redo_cmd, _ctx, 1);
                return;
            }

            if (redo_one()) {
                UNUSED(redo_cmd);
            }
        }

        void undo(CmdSP &undo_cmd, int delta) {
            if constexpr (has_undo<CmdT>::value) {
                // needs void undo_cmd::undo(sender, ctx, delta)
                undo_cmd->undo(undo_cmd, _ctx, delta);
                return;
            }

            int pos = (int) position();
            int pos_new = pos - delta;
            if (pos_new > 0 && (size_type) pos_new <= size()) {
                Iterator it = _position;
                std::advance(it, -delta);
                for (int i = 0; i < delta; i++) {
                    undo(undo_cmd);
                }
            }
        }
        void redo(CmdSP redo_cmd, int delta) {
            if constexpr (has_redo<CmdT>::value) {
                // needs void redo_cmd::redo(sender, ctx, delta)
                redo_cmd->redo(redo_cmd, _ctx, delta);
                return;
            }

            int pos = (int) position();
            int pos_new = pos + delta;
            if (pos_new > 0 && (size_type) pos_new <= size()) {
                Iterator it = _position;
                std::advance(it, delta);
                for (int i = 0; i < delta; i++) {
                    redo(redo_cmd);
                }
            }
        }

        void erase(int n = 1) {
            while (n-- && !empty()) {
                Iterator it = _position;
                if (it != _saved_states.end()) {
                    _position--;
                    _saved_states.erase(it);
                    _position++;
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

        size_type max_size() const { return _max_size; }
        void max_size(size_type max_value) {
            _max_size = max_value;
            if constexpr (undo_cxx::traits::has_max_size_set_v<Container>) {
                // if `void stack::max_size(size_t max_size_)` exists:
                _saved_states.max_size(_max_size);
            }
        }

        void clear() {
            _saved_states.clear();
            _position = _saved_states.end();
        }

    private:
        void save(CmdSP &cmd) {
            // std::printf("  . save memento\n");
            // // if constexpr (has_save_state<CmdSP>::value) {
            auto m = cmd->save_state(cmd, _ctx);
            push(std::move(m));
            // // }
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

            if (size() >= _max_size) {
                if constexpr (undo_cxx::traits::has_max_size_set_v<Container>) {
                    // nothing to do here
                } else if constexpr (undo_cxx::traits::has_pop_front_v<Container>) {
                    _saved_states.pop_front();
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

#if !defined(_MSC_VER) // These codes have a purpose for references
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
#endif

    private:
        Container _saved_states{};
        Iterator _position{_saved_states.end()};
        size_type _max_size{SIZE_T_MAX};
        ContextT _ctx{*this};
    };

} // namespace undo_cxx

// inline --------------------
namespace undo_cxx {

    template<typename State, typename BaseCmdT,
             template<class S, class B> typename RefCmdT>
    inline void base_undo_cmd_t<State, BaseCmdT, RefCmdT>::
            do_execute(CmdSP &sender, ContextT &ctx) {
        // auto cmd = ctx.mgr.create(Self::id(), "");
        // ctx.mgr.undo(cmd);
        ctx.mgr.undo(sender, Base::_delta);
    }

    template<typename State, typename BaseCmdT,
             template<class S, class B> typename RefCmdT>
    inline void base_redo_cmd_t<State, BaseCmdT, RefCmdT>::
            do_execute(CmdSP &sender, ContextT &ctx) {
        ctx.mgr.redo(sender, Base::_delta);
    }

} // namespace undo_cxx

#endif //UNDO_CXX_UNDO_ZCORE_HH
