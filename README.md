# 说明

OpenGL学习例子

教程地址：https://learnopengl-cn.github.io

## 环境搭建

### Windows

vcpkg

```
vcpkg install glfw3
vcpkg install glad
vcpkg install magnum
```

### Linux(Ubuntu)

1. 安装GLAD头文件及动态库

```shell
git clone https://github.com/Dav1dde/glad.git

cd glad
git checkout c
gcc -fpic src/glad.c -c
gcc -shared -Wl,-z,relro,-z,now -o libglad.so glad.o
sudo cp libglad.so /usr/local/lib/
sudo cp include/* -r /usr/local/include
```

或者到 https://glad.dav1d.de/ 下载对应的文件，指定gl版本为3.3或以上，Profile为Core

下载zip包解压后按照上面步骤编译拷贝库文件

2. stb_image

把源码中的`stb_image.h`拷贝到`/usr/local/include`

3. glfw

```shell
sudo apt install libglfw3-dev
```

4. magnum

```shell
sudo apt install magnum
```

## 编译

cmake -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake

# 资源

magnum document: [magnum](https://doc.magnum.graphics/magnum/index.html)
gun model: [Free3D](https://free3d.com/3d-model/45-acp-smith-and-wesson-13999.html)

# 注意事项

1. 使用vcpkg安装的magnum和ubuntu下面编译安装的，在使用GlfwApplication时CMake会出现问题，需要更改第650行附近的代码

```cmake
# FindMagnum.cmake
find_package(glfw3)
set_property(TARGET Magnum::${_component} APPEND PROPERTY
    INTERFACE_LINK_LIBRARIES glfw)
```

2. ubuntu下面还有改相同文件的SDL2配置

`sudo vim /usr/share/cmake/Magnum/FindMagnum.cmake`第647行后用`${SDL2_LIBRARIES}`而非`SDL2::SDL2`

```cmake
find_package(SDL2)
set_property(TARGET Magnum::${_component} APPEND PROPERTY
    INTERFACE_LINK_LIBRARIES ${SDL2_LIBRARIES})
```