# PhysicsFS streambuf
Single-header implementation of `std::basic_streambuf` backed by [PhysicsFS](https://github.com/icculus/physfs) files.


## Features
- Requires C++11 or newer
- Single header, just copy [physfs_streambuf.hpp](physfs_streambuf.hpp) to your project and `#include` it
- Implements the standard `std::basic_streambuf` C++ interface


## Caveats
- Does not implement seeking


## How to use
```cpp
#include <istream>
#include <ostream>
#include <physfs_streambuf.hpp>

// 1. Create physfs_streambuf
// 1.a) From an existing PHYSFS_File*
PHYSFS_File *file = /* ... */;
physfs_streambuf streambuf(file);
// 1.b) From filename and open mode
physfs_streambuf in_streambuf("filename", std::ios::in);
physfs_streambuf out_streambuf("filename", std::ios::out);

// 2. Create istream / ostream and use them as usual
std::istream is(&in_streambuf);
is >> /* ... */;

std::ostream os(&out_streambuf);
os << /* ... */;
```


## How to integrate in CMake projects
```cmake
add_subdirectory(path/to/physfs_streambuf)
target_link_libraries(my_awesome_target physfs_streambuf)
```
