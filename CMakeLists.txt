cmake_minimum_required(VERSION 3.18)
project(learn_opengl)

set(CMAKE_CXX_STANDARD 17)

# 设置编译参数
if (UNIX)
    set(DEFINES "-Wunused-parameter")
    set(CMAKE_CXX_FLAGS_DEBUG "-pipe -O0 -g -Wall -W -D_REENTRANT -fPIC ${DEFINES}")
    set(CMAKE_C_FLAGS_DEBUG "-pipe -O0 -g -Wall -W -D_REENTRANT -fPIC ${DEFINES}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-fno-pie")

    set(CMAKE_CXX_FLAGS_RELEASE "-pipe -O2 -Wall -W -D_REENTRANT -fPIC ${DEFINES}")
    set(CMAKE_C_FLAGS_RELEASE "-pipe -O2 -Wall -W -D_REENTRANT -fPIC ${DEFINES}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-fno-pie")
elseif (WIN32)
    if (MSVC)
        set(DEFINES_DEBUG "${UNICODE_FLAGS} -D_WINDOWS -DWIN32 -D_ENABLE_EXTENDED_ALIGNED_STORAGE -DWIN64 /wd4100")
        # /analyze
        set(CMAKE_CXX_FLAGS_DEBUG "/utf-8 /sdl /EHsc /D_DEBUG -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zi -MDd -W3 -w44456 -w44457 -w44458 ${DEFINES_DEBUG}")
        set(CMAKE_C_FLAGS_DEBUG "/utf-8 /sdl /D_DEBUG -nologo -Zc:wchar_t -FS -Zc:rvalueCast -Zc:inline -Zc:strictStrings -Zc:throwingNew -Zc:referenceBinding -Zc:__cplusplus /FS -Zi -MDd -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc ${DEFINES_DEBUG}")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/NOLOGO /DYNAMICBASE /NXCOMPAT /INCREMENTAL:NO /DEBUG /IGNORE:4099")
        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/NOLOGO /DYNAMICBASE /NXCOMPAT /DEBUG /DLL /SUBSYSTEM:WINDOWS /IGNORE:4099")

        if (ASAN)
            message(STATUS "build with asan")
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /fsanitize=address")
            set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /fsanitize=address")
            set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /WHOLEARCHIVE:clang_rt.asan_dbg_dynamic-x86_64.lib /WHOLEARCHIVE:clang_rt.asan_dbg_dynamic_runtime_thunk-x86_64.lib /WHOLEARCHIVE:vcasand.lib")
            set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /WHOLEARCHIVE:clang_rt.asan_dbg_dynamic-x86_64.lib /WHOLEARCHIVE:clang_rt.asan_dbg_dynamic_runtime_thunk-x86_64.lib /WHOLEARCHIVE:vcasand.lib")
            #/WHOLEACHIVE:clang_rt.asan_dbg_dll_thunk-x86_64.lib
            #/WHOLEARCHIVE:clang_rt.asan_cxx_dbg-x86_64.lib
            #
        endif ()

        # release模式下生成pdb用于调试
        set(DEFINES_RELEASE "${UNICODE_FLAGS} -D_WINDOWS -DWIN32 -D_ENABLE_EXTENDED_ALIGNED_STORAGE -DWIN64")
        set(CMAKE_C_FLAGS_RELEASE "/utf-8 /sdl -nologo -Zc:wchar_t -FS -Zi -Zc:strictStrings -O2 -MD -W3 -w44456 -w44457 -w44458 ${DEFINES_RELEASE}")
        set(CMAKE_CXX_FLAGS_RELEASE "/utf-8 /sdl -nologo -Zc:wchar_t -FS -Zi -Zc:rvalueCast -Zc:inline -Zc:strictStrings -Zc:throwingNew -Zc:referenceBinding -Zc:__cplusplus -O2 -MD -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc ${DEFINES_RELEASE}")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/NOLOGO /DYNAMICBASE /NXCOMPAT /INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF")# /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBC
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/NOLOGO /DYNAMICBASE /NXCOMPAT /INCREMENTAL:NO /DLL /SUBSYSTEM:WINDOWS /DEBUG /OPT:REF /OPT:ICF")
    else ()
        message(FATAL_ERROR "platform not supported ")
    endif ()
endif ()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("=== Debug Mode ===")
    set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/debug)
    set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/debug)
    add_definitions(-DDEBUG) # 加此宏是为了激活代码里面有的#ifdef DEBUG宏
else (CMAKE_BUILD_TYPE MATCHES Release) #CMAKE_BUILD_TYPE MATCHES Release
    message("=== Release Mode ===")
    set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/release)
    set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/release)
    # 屏蔽Qt警告和调试
    add_definitions(-DQT_NO_DEBUG)
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
    add_definitions(-DQT_DEPRECATED_WARNINGS)
endif ()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

if (WIN32)
    find_package(glad CONFIG REQUIRED)
    set(GLAD glad::glad)
    set(OPENGL opengl32)
else ()
    set(GLAD glad)
    set(OPENGL OpenGL)
endif ()

add_compile_definitions(PATH_SEP="/" ROOT_PATH=".." RES_DIR="../res")

set(WITH_ANYIMAGEIMPORTER ON CACHE BOOL "" FORCE)
set(WITH_ANYSCENEIMPORTER ON CACHE BOOL "" FORCE)
set(WITH_OBJIMPORTER ON CACHE BOOL "" FORCE)

find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
#find_package(freetype CONFIG REQUIRED)
set(Magnum_GLFW_FOUND TRUE)
find_package(Magnum REQUIRED
        GL
        Shaders
        Text
#        Application
        Sdl2Application
        GlfwApplication
        Trade
        SceneGraph
        MeshTools
        AnyImageImporter
        AnySceneImporter
        )

find_package(Corrade CONFIG REQUIRED Main)

include_directories(${SDL2_INCLUDE_DIRS})

if (UNIX)
    set(ASSIMP assimp)
else ()
    set(ASSIMP assimp::assimp)
endif ()

add_subdirectory(src)
add_subdirectory(magnum)
