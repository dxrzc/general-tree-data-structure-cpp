# General Tree Data Structure (C++20)

This repository provides a templated general tree data structure implemented in modern C++.  
The design emphasizes **iterator support**, **range compatibility**, **move semantics**, and a **safe public node interface** for structural navigation.

## Features

### Core design
- General (n-ary) tree with no fixed child limit
- Safe public `node` interface for structural navigation
- Preorder and postorder traversal
- Custom iterators compatible with standard algorithms
- Move semantics and deep copy support
- In-place construction (`emplace`)

### Operations overview
- Create root node (create_root, emplace_root)
- Insert children and siblings
- Insert entire subtrees
- Delete subtrees safely
- Compare trees structurally (operator==)
- Clear and reuse tree instances

### Built-in Traversal Modes
The tree supports multiple traversal strategies:
- **Preorder traversal**
- **Postorder traversal**

### Continuous integration
Cross-platform testing:
- Linux (GCC, LLVM/Clang)
- Windows (MSVC)

## Testing
- Unit tests implemented using [doctest](https://github.com/doctest/doctest)

### Requirements
- C++20 compatible compiler
- CMake
- Ninja (recommended)
```bash
git clone https://github.com/dxrzc/general-tree-data-structure-cpp.git
cd general-tree-data-structure-cpp

cmake -S . -B build -G Ninja
cmake --build build

cd build
ctest
```

## Usage example
The following example demonstrates how the general_tree can be used to model a hierarchical file system structure.
- Each directory is represented as a node
- Parent–child and sibling relationships are explicitly constructed
- The tree is traversed using preorder iteration and printed using C++20 ranges
```
/
├── bin
├── etc
├── home
│   └── user
│       ├── Documents
│       └── Projects
└── var
    └── log
```

```c++
#include <algorithm>
#include <general-tree.h>
#include <iostream>
#include <iterator>

int main()
{
    general_tree<std::string> filesystem;
    const auto root_dir = filesystem.create_root("/");

    // level 1
    auto bin = filesystem.insert_left_child(root_dir, "bin");
    auto etc = filesystem.insert_right_sibling(bin, "etc");
    auto home = filesystem.insert_right_sibling(etc, "home");
    auto var = filesystem.insert_right_sibling(home, "var");

    // /home subtree
    auto user = filesystem.insert_left_child(home, "user");
    auto docs = filesystem.insert_left_child(user, "Documents");
    filesystem.insert_right_sibling(docs, "Projects");

    // /var subtree
    filesystem.insert_left_child(var, "log");

    // preorder by default
    std::ranges::copy(filesystem, std::ostream_iterator<std::string>(std::cout, "\n"));

    return 0;
}
```
