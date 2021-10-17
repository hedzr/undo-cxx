# undo-cxx

![CMake Build Matrix](https://github.com/hedzr/undo-cxx/workflows/CMake%20Build%20Matrix/badge.svg) <!-- 
![CMake Build Matrix](https://github.com/hedzr/undo-cxx/workflows/CMake%20Build%20Matrix/badge.svg?event=release) 
--> [![GitHub tag (latest SemVer)](https://img.shields.io/github/tag/hedzr/undo-cxx.svg?label=release)](https://github.com/hedzr/undo-cxx/releases)

`undo-cxx` C++17 library provides a undo/redo subsystem (linear or restricted non-linear).

**WIP**

## Features

- Highly configurable/customizable
- Undo/Redo subsystem
  - restricted non-linear undo (batch undo+erase+redo)
  - todo: full-functional non-linear
- Bundled with Command subsystem
  - undoable/redoable
  - Composite command (`undo_cxx::composite_cmd_t<>`): composite multi-commands as one (groupable)
  - Composite memento (`undo_cxx::state_t<>`) for composite-command

---

## Examples

The following code is a part of a word-processor:

```cpp
#include <undo_cxx.hh>

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
    test_undo_sys();
    return 0;
}
```




## Build Options

### Build with CMake

> 1. gcc 10+: passed
> 2. clang 12+: passed
> 3. msvc build tool 16.7.2, 16.8.5 (VS2019 or Build Tool) passed

ninja is optional for faster building.

```bash
# configure
cmake -S . -B build/ -G Ninja
# build
cmake --build build/
# install
cmake --install build/
# Or:cmake --build build/ --target install
#
# Sometimes sudo it:
#   sudo cmake --build build/ --target install
# Or manually:
#   cmake --install build/ --prefix ./install --strip
#   sudo cp -R ./install/include/* /usr/local/include/
#   sudo cp -R ./install/lib/cmake/undo_cxx /usr/local/lib/cmake/
```


### Other CMake Options

1. `UNDO_CXX_BUILD_TESTS_EXAMPLES`=OFF
2. `UNDO_CXX_BUILD_DOCS`=OFF
3. ...


## Thanks to JODL

Thanks to [JetBrains](https://www.jetbrains.com/?from=undo-cxx) for donating product licenses to help develop **undo-cxx** [![jetbrains](https://gist.githubusercontent.com/hedzr/447849cb44138885e75fe46f1e35b4a0/raw/bedfe6923510405ade4c034c5c5085487532dee4/jetbrains-variant-4.svg)](https://www.jetbrains.com/?from=hedzr/undo-cxx)


## LICENSE

MIT

