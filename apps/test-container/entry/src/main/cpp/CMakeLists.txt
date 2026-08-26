# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(entry)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Absolute paths into the SDK source tree (workspace-relative, portable enough for the
# dev container). Expo modules live outside entry/src/main/cpp.
set(EXPOMODULESCORE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../expo-modules-core/ohos)
set(EXPODEVICE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../expo-device/ohos)
set(EXPOFILESYSTEM_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../expo-file-system/ohos)
set(EXPOCONSTANTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../expo-constants/ohos)

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

# HarmonyOS NEXT / RNOH integration (Track 5).
# When the HarmonyOS NEXT SDK and RNOH are installed, the real Hermes / ReactCommon
# headers and libraries are used and the real JSI path is activated (USE_JSI=1).
#
# Search order:
#   1. $HOME/.harmonyos-next-stubs or $USERPROFILE/.harmonyos-next-stubs (local stub overlay)
#   2. $ENV{HARMONYOS_NEXT_SDK} (explicit NEXT SDK root)
#   3. /opt/harmonyos-next (Linux/macOS fallback)
find_path(HERMES_INCLUDE_DIR hermes/hermes.h
    PATHS
        "$ENV{HOME}/.harmonyos-next-stubs/native/js/hermes/include"
        "$ENV{USERPROFILE}/.harmonyos-next-stubs/native/js/hermes/include"
        $ENV{HARMONYOS_NEXT_SDK}/native/js/hermes/include
        /opt/harmonyos-next/native/js/hermes/include
)
find_path(REACTCOMMON_INCLUDE_DIR react/ReactCommon/TurboModule.h
    PATHS
        "$ENV{HOME}/.harmonyos-next-stubs/native/js/react-native/ReactCommon"
        "$ENV{USERPROFILE}/.harmonyos-next-stubs/native/js/react-native/ReactCommon"
        $ENV{HARMONYOS_NEXT_SDK}/native/js/react-native/ReactCommon
        /opt/harmonyos-next/native/js/react-native/ReactCommon
)
find_library(HERMES_LIB hermes
    PATHS
        "$ENV{HOME}/.harmonyos-next-stubs/native/js/hermes/lib"
        "$ENV{USERPROFILE}/.harmonyos-next-stubs/native/js/hermes/lib"
        $ENV{HARMONYOS_NEXT_SDK}/native/js/hermes/lib
        /opt/harmonyos-next/native/js/hermes/lib
)
find_library(REACT_NATIVE_LIB react_native
    PATHS
        "$ENV{HOME}/.harmonyos-next-stubs/native/js/react-native/lib"
        "$ENV{USERPROFILE}/.harmonyos-next-stubs/native/js/react-native/lib"
        $ENV{HARMONYOS_NEXT_SDK}/native/js/react-native/lib
        /opt/harmonyos-next/native/js/react-native/lib
)

if(HERMES_INCLUDE_DIR AND REACTCOMMON_INCLUDE_DIR)
    add_compile_definitions(USE_JSI=1)
    include_directories(
        ${EXPOMODULESCORE_ROOT}/include
        ${EXPODEVICE_ROOT}/include
        ${EXPOFILESYSTEM_ROOT}/include
        ${EXPOCONSTANTS_ROOT}/include
        ${NATIVERENDER_ROOT_PATH}/include
        ${HERMES_INCLUDE_DIR}
        ${REACTCOMMON_INCLUDE_DIR}
    )
    message(STATUS "Using real JSI path with Hermes/ReactCommon")
else()
    message(STATUS "HarmonyOS NEXT SDK Hermes/ReactCommon headers not found; using stub JSI path")
    add_compile_definitions(USE_JSI=1)
    include_directories(
        ${EXPOMODULESCORE_ROOT}/include
        ${EXPODEVICE_ROOT}/include
        ${EXPOFILESYSTEM_ROOT}/include
        ${EXPOCONSTANTS_ROOT}/include
        ${NATIVERENDER_ROOT_PATH}/include
    )
endif()

add_library(entry SHARED
    napi_init.cpp
    ${EXPOMODULESCORE_ROOT}/src/JSValue.cpp
    ${EXPOMODULESCORE_ROOT}/src/ExpoModulesRegistry.cpp
    ${EXPOMODULESCORE_ROOT}/src/Promise.cpp
    ${EXPOMODULESCORE_ROOT}/src/ExpoHostObject.cpp
    ${EXPOMODULESCORE_ROOT}/src/ExpoRuntime.cpp
    ${EXPOMODULESCORE_ROOT}/src/ExpoJsiBridge.cpp
    ${EXPOMODULESCORE_ROOT}/src/JsiTurboModuleFacade.cpp
    ${EXPOMODULESCORE_ROOT}/src/FabricViewManagerFacade.cpp
    ${EXPOMODULESCORE_ROOT}/src/hermes.cpp
    ${EXPODEVICE_ROOT}/src/ExpoDevice.cpp
    ${EXPOFILESYSTEM_ROOT}/src/ExpoFileSystem.cpp
    ${EXPOCONSTANTS_ROOT}/src/ExpoConstants.cpp
)

target_link_libraries(entry PUBLIC
    libace_napi.z.so
    libdeviceinfo_ndk.z.so
    libhilog_ndk.z.so
)

if(HERMES_LIB AND REACT_NATIVE_LIB)
    target_link_libraries(entry PUBLIC
        ${HERMES_LIB}
        ${REACT_NATIVE_LIB}
    )
endif()
