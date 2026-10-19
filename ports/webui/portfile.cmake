vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO webui-dev/webui
    REF f1b28eeeecfc2d63bdb4d4857c6f137468c6d6f6
    SHA512 894955f15109e0e1ee2cb512bb8f898f4952a774abc711fc9609d99151a8eac32154d759d7fa502a701a704716f73b02723eb667bd686f8441b51a3141e94176
    HEAD_REF main
)

# Use our own CMakeLists.txt: it installs the unofficial::webui::webui target
file(COPY "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt" DESTINATION "${SOURCE_PATH}")

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(PACKAGE_NAME unofficial-webui)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
