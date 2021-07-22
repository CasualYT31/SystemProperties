# SystemProperties
Cross-platform C++ library used to retrieve hardware and software information.

# Goals
The goal of this project is to program an interface via which a program may query the computer for various hardware and software details, such as CPU, memory, storage, GPU, etc. The library must be cross-platform (at least supporting Windows, Linux, and macOS) and must be easily integratable into other projects via CMake.

# How to Use
If you are using CMake to build your project, you can include the following into your `CMakeLists.txt` file in order to add this library:
```
# minimum version required is 3.14 becuase of FetchContent_MakeAvailable
cmake_minimum_required (VERSION 3.14)
include (FetchContent)

# etc.

FetchContent_Declare(systemproperties
	GIT_REPOSITORY https://github.com/CasualYT31/SystemProperties.git
	GIT_TAG 0.4 # alternatively, you can use main to fetch the latest version (which may be unstable)
)
FetchContent_MakeAvailable(systemproperties)

# etc.

target_link_libraries(YourTargetNameHere SystemProperties)
```

Alternatively, you can simply download the code manually and add both `SystemProperties.hpp` and `SystemProperties.cpp` to your project directly as accompanying source files in case you don't wish to use CMake.

# Progress
This library can currently obtain all information that [this library](https://github.com/dabbertorres/systemInfo) can, plus some storage information, but for **Windows and Linux only**. I can very easily extend Windows and Linux features, but as I don't have access to a macOS device, no code has been written for that platform yet.
