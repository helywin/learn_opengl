# 说明

OpenGL学习例子

教程地址：https://learnopengl-cn.github.io

## 环境搭建

### Windows

vcpkg

```
vcpkg install glfw3
vcpkg install glad
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

## 编译

cmake -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake

# 资源

gun model: [Free3D](https://free3d.com/3d-model/45-acp-smith-and-wesson-13999.html)