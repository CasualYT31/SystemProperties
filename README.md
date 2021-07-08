# SystemProperties
Cross-platform C++ library used to retrieve hardware and software information.

# Goals
The goal of this project is to program an interface via which a program may query the computer for various hardware and software details, such as CPU, memory, storage, GPU, etc. The library must be cross-platform and must be easily integratable into other projects via CMake.

# How to Use
If you are using CMake to build your project, you can include the following into your `CMakeLists.txt` file:
```

```

# Note on Performance
I intend to use command-line tools in order to carry out reliable queries. This may have an impact on the efficiency of the library, but it should make it robust and extensible. I have employed caching techniques in order to mitigate against the performance impact of relying on command-line utilities, however, if a faster way of performing the queries is identified in the future it will certainly be favoured.

For Windows: so far I have used `Get-CimInstance` via PowerShell to perform *blocking* queries, and I have used temporary text files to output results. Temporary files are given the name "ClassNameObjectName.temp", e.g. "Win32_VideoControllerdriverversion.temp" for the `sys::gpu::driver()` call, and are stored in the current working directory. These files are removed once the query has been completed. These two details make this library pretty slow... However, as previously stated, results are cached, so that subsequent calls do not make PowerShell queries if they returned something other than a blank string when first called.

**Important: I have not made any checks to see if files already exist with the names given to the temporary files.**

# Progress
This library can currently obtain all information that [this library](https://github.com/dabbertorres/systemInfo) can, but for **Windows only**. I can very easily extend Windows-based features, but as I don't have access to a macOS device or easy access to a Linux device, no code has been written for other platforms yet.
