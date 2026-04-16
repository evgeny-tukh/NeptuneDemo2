cmake_minimum_required(
    VERSION 3.27
)

project(NeptuneDemo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(
    nd
    main.cpp
    chartpanel.cpp
    navpanel.cpp
    tools.cpp
    neptune_mgmt.cpp
    main.rc
)

target_compile_definitions(
    nd
    PRIVATE
    MBCS
    _MBCS
    DEBUG
    _DEBUG
    _WIN32
    WIN32
    _WINDOWS
    WIN64
    _WIN64
    _CRT_SECURE_NO_WARNINGS
)

target_compile_options(
    nd
    PRIVATE
    /MTd
    /RTC1
    /Gd
    /std:c++17
    /EHsc
)

target_link_options(
    nd
    PRIVATE
    /SUBSYSTEM:WINDOWS
)

target_link_libraries(
    nd
    PRIVATE
    wsock32.lib
    shlwapi.lib
    kernel32.lib
    user32.lib
    gdi32.lib
    winspool.lib
    shell32.lib
    ole32.lib
    oleaut32.lib
    uuid.lib
    comdlg32.lib
    advapi32.lib
    shlwapi.lib
)
