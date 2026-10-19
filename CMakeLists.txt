cmake_minimum_required(VERSION 3.20)
project(webui_samples CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default to Release if no build type is specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type" FORCE)
endif()

# Map Debug configuration to Release imported target since webui was built for Release
set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG Release)

# Locate webui package: vcpkg ships "unofficial-webui", a local MinGW build ships "webui"
find_package(unofficial-webui CONFIG QUIET)
if(unofficial-webui_FOUND)
    set(WEBUI_TARGET unofficial::webui::webui)
else()
    find_package(webui CONFIG REQUIRED)
    set(WEBUI_TARGET webui::static)
endif()

# Each sample lives in samples/<name>/main.cpp and builds to target "<name>_sample".
# Add a new sample by adding its folder name here.
set(SAMPLES minimal chart)

function(add_sample_executable target source)
    if(WIN32)
        add_executable(${target} WIN32 ${source} samples/common/windows_entrypoint.cpp)
    else()
        add_executable(${target} ${source})
    endif()
    target_link_libraries(${target} PRIVATE ${WEBUI_TARGET})
endfunction()

foreach(sample IN LISTS SAMPLES)
    add_sample_executable(${sample}_sample samples/${sample}/main.cpp)
endforeach()

# Samples with local web assets (JS libraries bundled so they work offline).
# Assets are copied to <exe dir>/<name>_assets and the path is passed as ASSETS_ROOT.
set(ASSET_SAMPLES richchart plotly widgets webview2)

foreach(sample IN LISTS ASSET_SAMPLES)
    add_sample_executable(${sample}_sample samples/${sample}/main.cpp)
    target_compile_definitions(${sample}_sample PRIVATE
        ASSETS_ROOT="$<TARGET_FILE_DIR:${sample}_sample>/${sample}_assets")
    add_custom_command(TARGET ${sample}_sample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/samples/${sample}/assets
            $<TARGET_FILE_DIR:${sample}_sample>/${sample}_assets)
endforeach()

# webview2: show_wv() needs WebView2Loader.dll (x64, from the Microsoft.Web.WebView2 NuGet package)
# next to the executable. The WebView2 runtime itself ships with Windows 11.
add_custom_command(TARGET webview2_sample POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/samples/webview2/third_party/WebView2Loader.dll
        $<TARGET_FILE_DIR:webview2_sample>)

# Install: `cmake --install <build> --prefix dist` puts exes, assets and DLL side by side.
install(TARGETS minimal_sample chart_sample richchart_sample plotly_sample
                widgets_sample webview2_sample RUNTIME DESTINATION .)
foreach(sample IN LISTS ASSET_SAMPLES)
    install(DIRECTORY samples/${sample}/assets/ DESTINATION ${sample}_assets)
endforeach()
install(FILES samples/webview2/third_party/WebView2Loader.dll
              samples/webview2/third_party/WebView2-LICENSE.txt DESTINATION .)
