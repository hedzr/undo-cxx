# undo-cxx

![CMake Build Matrix](https://github.com/hedzr/undo-cxx/workflows/CMake%20Build%20Matrix/badge.svg) <!-- 
![CMake Build Matrix](https://github.com/hedzr/undo-cxx/workflows/CMake%20Build%20Matrix/badge.svg?event=release) 
--> [![GitHub tag (latest SemVer)](https://img.shields.io/github/tag/hedzr/undo-cxx.svg?label=release)](https://github.com/hedzr/undo-cxx/releases)

`undo-cxx` C++17 library provides a non-linear undo/redo subsystem.

**WIP**

## Features

- Highly configurable/customizable
- Undo/Redo subsystem (linear or non-linear)
- Bundled with Command subsystem
  - Composite command: composite multi-commands as one (groupable)
  - 

---

## Examples

The following is part of a word-processor:

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
            Base::do_execute(ctx); // = ctx.mgr.undo(*this)
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
            Base::do_execute(ctx); // = ctx.mgr.redo(*this)
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

}}} // namespace dp::undo::test

void test_undo_sys() {
    using namespace dp::undo::test;

    using State = std::string;
    using M = undo_cxx::undoable_cmd_system_t<State>;
    using UndoCmdT = UndoCmd<State>;
    using RedoCmdT = RedoCmd<State>;
    using FontStyleCmdT = FontStyleCmd<State>;

    M undoable_cmd_system;

    // do some stuffs

    undoable_cmd_system.invoke(FontStyleCmdT{"italic state1"});
    undoable_cmd_system.invoke(FontStyleCmdT{"italic-bold state2"});
    undoable_cmd_system.invoke(FontStyleCmdT{"underline state3"});
    undoable_cmd_system.invoke(FontStyleCmdT{"italic state4"});

    // and try to undo or redo
    
    undoable_cmd_system.invoke(UndoCmdT{"undo 1"});
    undoable_cmd_system.invoke(UndoCmdT{"undo 2"});

    undoable_cmd_system.invoke(RedoCmdT{"redo 1"});

    undoable_cmd_system.invoke(UndoCmdT{"undo 3"});
    undoable_cmd_system.invoke(UndoCmdT{"undo 4"});
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

