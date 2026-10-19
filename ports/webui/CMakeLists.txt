cmake_minimum_required(VERSION 3.18)
project(WebUILibrary C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SOURCE_FILES src/civetweb/civetweb.c src/webui.c)
if(WIN32)
    list(APPEND SOURCE_FILES src/webview/win32_wv2.cpp)
endif()

add_library(webui ${SOURCE_FILES})
target_include_directories(webui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_compile_definitions(webui PUBLIC NO_CACHING NO_CGI USE_WEBSOCKET NO_SSL
    "$<$<CONFIG:Debug>:WEBUI_LOG>" "$<$<NOT:$<CONFIG:Debug>>:NDEBUG>")

if(WIN32)
    target_link_libraries(webui PRIVATE ws2_32 user32 shell32 ole32)
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(webui PRIVATE CIVETWEB_DLL_EXPORTS PUBLIC CIVETWEB_DLL_IMPORTS)
    endif()
endif()

set_target_properties(webui PROPERTIES OUTPUT_NAME webui-2 PREFIX "")

install(FILES include/webui.h include/webui.hpp DESTINATION include)
install(TARGETS webui
    EXPORT unofficial-webui
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)
install(EXPORT unofficial-webui
    FILE unofficial-webui-config.cmake
    NAMESPACE unofficial::webui::
    DESTINATION share/unofficial-webui)
