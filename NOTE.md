# 笔记

```mermaid
graph TD;
点--顶点着色器-->
3D坐标--图元装配器-->
图元--几何着色器-->
产生新顶点构造出新图元--光栅化-->
把图元映射成屏幕像素,即片段--片段着色器-->
图像--Alpha测试和混合-->最终图像
```

> OpenGL中的一个片段是OpenGL渲染一个像素所需的所有数据。
>
> 片段着色器的主要目的是计算一个像素的最终颜色
>
> OpenGL扩展:https://www.khronos.org/registry/OpenGL/index_gl.php

## 顶点缓冲对象VBO

一次性发送数据到显卡

```c++
// VBO对象
unsigned int VBO;
// 创建缓冲对象
glGenBuffers(1, &VBO);
// 绑定缓冲对象，绑定到顶点缓冲
glBindBuffer(GL_ARRAY_BUFFER, VBO);
// 复制点到缓冲对象，类型，字节数，指针，绘制类型
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
```

- GL_STATIC_DRAW ：数据不会或几乎不会改变。
- GL_DYNAMIC_DRAW：数据会被改变很多。
- GL_STREAM_DRAW ：数据每次绘制时都会改变。

缓存位置速度不一样

## 顶点着色器

也就是shader

顶点着色器

```glsl
# 指定OpenGL最低版本
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
```

片段着色器

```glsl
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
```



### 编译着色器

```c++
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
// 创建shader对象
unsigned int vertexShader;
vertexShader = glCreateShader(GL_VERTEX_SHADER);
// 附加到对象并编译，1代表字符串数量
glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
glCompileShader(vertexShader);

// 判断是否正确编译
int  success;
char infoLog[512];
glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
// 不成功获取错误信息
if(!success)
{
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
}
```

### 着色器程序

着色器程序对象(Shader Program Object)是多个着色器合并之后并最终链接完成的版本

把上一个着色器的输出给下一个着色器，但是要保证输入输出匹配

```c++
// 创建
unsigned int shaderProgram;
shaderProgram = glCreateProgram();

// 链接
glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);
glLinkProgram(shaderProgram);

// 检测是否链接成功
glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
if(!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    ...
}

// 激活，激活后说着色器的调用渲染都会调用这个
glUseProgram(shaderProgram);

// 链接后可以删除着色器对象
glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);


```

## 链接顶点属性

GL_ARRAY_BUFFER的存储是紧密排列的，使用函数解析顶点数据

```c++
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
// 启用顶点属性
glEnableVertexAttribArray(0);
```

- 第一个参数指定我们要配置的顶点属性。还记得我们在顶点着色器中使用`layout(location = 0)`定义了position顶点属性的位置值(Location)吗？它可以把顶点属性的位置值设置为`0`。因为我们希望把数据传递到这一个顶点属性中，所以这里我们传入`0`。
- 第二个参数指定顶点属性的大小。顶点属性是一个`vec3`，它由3个值组成，所以大小是3。
- 第三个参数指定数据的类型，这里是GL_FLOAT(GLSL中`vec*`都是由浮点数值组成的)。
- 下个参数定义我们是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。我们把它设置为GL_FALSE。
- 第五个参数叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔。由于下个组位置数据在3个`float`之后，我们把步长设置为`3 * sizeof(float)`。要注意的是由于我们知道这个数组是紧密排列的（在两个顶点属性之间没有空隙）我们也可以设置为0来让OpenGL决定具体步长是多少（只有当数值是紧密排列时才可用）。一旦我们有更多的顶点属性，我们就必须更小心地定义每个顶点属性之间的间隔，我们在后面会看到更多的例子（译注: 这个参数的意思简单说就是从这个属性第二次出现的地方到整个数组0位置之间有多少字节）。
- 最后一个参数的类型是`void*`，所以需要我们进行这个奇怪的强制类型转换。它表示位置数据在缓冲中起始位置的偏移量(Offset)。由于位置数据在数组的开头，所以这里是0。我们会在后面详细解释这个参数。

绘制流程

```c++
// 0. 复制顶点数组到缓冲中供OpenGL使用
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// 1. 设置顶点属性指针
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
// 2. 当我们渲染一个物体时要使用着色器程序
glUseProgram(shaderProgram);
// 3. 绘制物体
someOpenGLFunctionThatDrawsOurTriangle();
```

### 顶点数组对象

顶点数组对象(Vertex Array Object, VAO)，只需要执行一次，以后每次绘制只需要绑定VAO就行

![VAO和VBO的关系](https://learnopengl-cn.github.io/img/01/04/vertex_array_objects.png)

```c++
// 创建
unsigned int VAO;
glGenVertexArrays(1, &VAO);

// ..:: 初始化代码（只运行一次 (除非你的物体频繁改变)） :: ..
// 1. 绑定VAO
glBindVertexArray(VAO);
// 2. 把顶点数组复制到缓冲中供OpenGL使用
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// 3. 设置顶点属性指针
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

[...]

// ..:: 绘制代码（渲染循环中） :: ..
// 4. 绘制物体
glUseProgram(shaderProgram);
glBindVertexArray(VAO);
someOpenGLFunctionThatDrawsOurTriangle();
```

## 索引缓冲对象

索引缓冲对象(Element Buffer Object，EBO，也叫Index Buffer Object，IBO)

用来处理共用点的情况

```c++
float vertices[] = {
    0.5f, 0.5f, 0.0f,   // 右上角
    0.5f, -0.5f, 0.0f,  // 右下角
    -0.5f, -0.5f, 0.0f, // 左下角
    -0.5f, 0.5f, 0.0f   // 左上角
};

unsigned int indices[] = { // 注意索引从0开始! 
    0, 1, 3, // 第一个三角形
    1, 2, 3  // 第二个三角形
};
```

创建EBO

```c++
// 创建
unsigned int EBO;
glGenBuffers(1, &EBO);

// 绑定传输数据
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
```

流程

```c++
// ..:: 初始化代码 :: ..
// 1. 绑定顶点数组对象
glBindVertexArray(VAO);
// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
// 4. 设定顶点属性指针
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

[...]

// ..:: 绘制代码（渲染循环中） :: ..
glUseProgram(shaderProgram);
glBindVertexArray(VAO);
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0)
glBindVertexArray(0);
```

# 着色器

结构：

```glsl
#version version_number
in type in_variable_name;
in type in_variable_name;

out type out_variable_name;

uniform type uniform_name;

int main()
{
  // 处理输入并进行一些图形操作
  ...
  // 输出处理过的结果到输出变量
  out_variable_name = weird_stuff_we_processed;
}
```

数据类型：int`、`float`、`double`、`uint`和`bool

## 向量

| 类型    | 含义                            |
| ------- | ------------------------------- |
| `vecn`  | 包含`n`个float分量的默认向量    |
| `bvecn` | 包含`n`个bool分量的向量         |
| `ivecn` | 包含`n`个int分量的向量          |
| `uvecn` | 包含`n`个unsigned int分量的向量 |
| `dvecn` | 包含`n`个double分量的向量       |

向量的重组：

```glsl
vec2 someVec;
vec4 differentVec = someVec.xyxx;
vec3 anotherVec = differentVec.zyw;
vec4 otherVec = someVec.xxxx + anotherVec.yxzy;
```

## 输入输出

`in` `out`关键字

顶点着色器的输入比较特殊，从顶点数据直接接收输入。使用`location`指定输入变量的，`layout (location = 0)`

片段着色器的输出是vec4颜色，需要一个像素颜色，没用则默认为黑色或白色

## Uniform

GPU和CPU变量交互的变量

```c++
angleLocation = glGetUniformLocation(shaderProgram, "ourAngle");
double t = glfwGetTime() / 10;
double angle = t - int(t);
glUniform1f(angleLocation, (float) angle);
```

另外输入多个变量是VAO的设置

输入为：

```c++
// 顶点，颜色
float vertices[] = {
    -0.5f, -0.5f, 0.0f, 1.0f, 0, 0,
    0.5f, -0.5f, 0.0f, 0, 1.0f, 0,
    0.0f, 0.5f, 0.0f, 0, 0, 1.0f,
};
```

顶点shader：

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
in vec3 aColor;
out vec4 textureColor;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    textureColor = vec4(aColor, 1.0);
}
```

设置VAO：

```c++
// 输入变量索引号，0为aPos，1为aColor
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
glEnableVertexAttribArray(0);
glEnableVertexAttribArray(1);
```

# 纹理

纹理(texture1)

纹理坐标范围为x：0到1，y：0到1，做下角为原点，纹理坐标获取颜色成为**采样**。

需要指定每个顶点对应的纹理坐标，其他点的坐标通过插值得到，比如

![texure](https://learnopengl-cn.github.io/img/01/06/tex_coords.png)

纹理环绕方式：

| 环绕方式           | 描述                                                         |
| ------------------ | ------------------------------------------------------------ |
| GL_REPEAT          | 对纹理的默认行为。重复纹理图像。                             |
| GL_MIRRORED_REPEAT | 和GL_REPEAT一样，但每次重复图片是镜像放置的。                |
| GL_CLAMP_TO_EDGE   | 纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。 |
| GL_CLAMP_TO_BORDER | 超出的坐标为用户指定的边缘颜色。                             |

![示例](https://learnopengl-cn.github.io/img/01/06/texture_wrapping.png)

使用`glTexParameter`函数设置

```c++
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
```

如果使用最后一个，还需要设置边框颜色

```c++
float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
```

> 无缝贴图：重复后图片和图片之间没有明显的边线

## 纹理过滤

纹理小物体大拉伸时的插值选项

GL_LINEAR，线性过滤，计算均值，看上去物体会变得模糊

GL_NEAREST，最近的值，会产生像素风格图像

设置方式

```c++
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

## 多级渐远纹理

由于抽样带来性能问题，对与不同距离的物体渲染采用不同大小的贴图

![多级渐远纹理](https://learnopengl-cn.github.io/img/01/06/mipmaps.png)

使用OpenGL函数`glGenerateMipmaps`可以创建

## stb_image

单头文件图像加载库

```bash
vcpkg install stb
```

```c++
// 定义了该宏后只会包含相关的函数
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int width, height, nrChannels;
unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
```

## 生成纹理

```c++
// 创建纹理
GLuint texture1;
glGenTextures(1, &texture1);

// 绑定纹理
glBindTexture(GL_TEXTURE_2D, texture1);

// 载入图片
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGED_BYTE, data);
glGenerateMipmap(GL_TEXTURE_2D);
```

- 第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。
- 第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。
- 第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有`RGB`值，因此我们也把纹理储存为`RGB`值。
- 第四个和第五个参数设置最终的纹理的宽度和高度。我们之前加载图像的时候储存了它们，所以我们使用对应的变量。
- 下个参数应该总是被设为`0`（历史遗留的问题）。
- 第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为`char`(byte)数组，我们将会传入对应值。
- 最后一个参数是真正的图像数据。

## 纹理单元

使用纹理单元可以在shader里面对多个纹理进行叠加运算处理

```c++
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texture1);
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, texture2);
```

```glsl
#version 330 core
...

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}
```

# 变换

## 向量

## GLM

可以通过平移量和旋转量来构建平移旋转变换矩阵

```c++
glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
// 平移变换
glm::mat4 trans;
trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));

// 旋转
trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));

// 缩放
trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

vec = trans * vec;
```

# 坐标系统

- 局部空间(Local Space，或者称为物体空间(Object Space))
- 世界空间(World Space)
- 观察空间(View Space，或者称为视觉空间(Eye Space))
- 裁剪空间(Clip Space)
- 屏幕空间(Screen Space)

![coordination_system](https://learnopengl-cn.github.io/img/01/08/coordinate_systems.png)

1. 局部坐标是对象相对于局部原点的坐标，也是物体起始的坐标。
2. 下一步是将局部坐标变换为世界空间坐标，世界空间坐标是处于一个更大的空间范围的。这些坐标相对于世界的全局原点，它们会和其它物体一起相对于世界的原点进行摆放。
3. 接下来我们将世界坐标变换为观察空间坐标，使得每个坐标都是从摄像机或者说观察者的角度进行观察的。
4. 坐标到达观察空间之后，我们需要将其投影到裁剪坐标。裁剪坐标会被处理至-1.0到1.0的范围内，并判断哪些顶点将会出现在屏幕上。
5. 最后，我们将裁剪坐标变换为屏幕坐标，我们将使用一个叫做视口变换(Viewport Transform)的过程。视口变换将位于-1.0到1.0范围的坐标变换到由glViewport函数所定义的坐标范围内。最后变换出来的坐标将会送到光栅器，将其转化为片段。

## 局部空间

对于模型来说的基准坐标系

## 世界空间

所有模型统一的坐标空间

## 观察空间

摄像机坐标空间

## 裁剪空间

投影后裁剪掉视角之外的点

### 正射投影

平行线仍然是平行线

```c++
glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
```

### 透视投影

远小近大

```c++
glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
```

![透视原理](https://learnopengl-cn.github.io/img/01/08/perspective_frustum.png)

第一个参数决定了角度，第二个参数设置高宽比，第三个近距离平面，第四个参数远距离平面

使用矩阵乘法变换坐标系

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    TexCoord = aTexCoord;
}
```

矩阵乘法从右到左

# 摄像机

## 摄像机位置

## 摄像机方向

从被观测点到相机的向量，并不是朝向的方向

## 右轴

摄像机空间的x轴正方向，获取方式是通过借助一个向上的向量和方向向量叉乘得到和方向向量垂直的右向量

## 上轴

右向量和方向向量叉乘

## Look At

观察矩阵，通过lookAt函数创建，第一个是相机竖直方向，第二个是目标位置，第三个是相机位置

```c++
glm::mat4 view;
view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), 
           glm::vec3(0.0f, 0.0f, 0.0f), 
           glm::vec3(0.0f, 1.0f, 0.0f));
```

z轴方向始终是垂直于视角的向上的，所以固定不变

根据相机方向角计算方向向量（相机不绕着z轴转动，不然要更改lookAt的z轴）

```c++
glm::vec3 front;
front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
front.y = sin(glm::radians(pitch));
front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
```

# 词汇

- **OpenGL**： 一个定义了函数布局和输出的图形API的正式规范。
- **GLAD**： 一个拓展加载库，用来为我们加载并设定所有OpenGL函数指针，从而让我们能够使用所有（现代）OpenGL函数。
- **视口(Viewport)**： 我们需要渲染的窗口。
- **图形管线(Graphics Pipeline)**： 一个顶点在呈现为像素之前经过的全部过程。
- **着色器(Shader)**： 一个运行在显卡上的小型程序。很多阶段的图形管道都可以使用自定义的着色器来代替原有的功能。
- **标准化设备坐标(Normalized Device Coordinates, NDC)**： 顶点在通过在剪裁坐标系中剪裁与透视除法后最终呈现在的坐标系。所有位置在NDC下-1.0到1.0的顶点将不会被丢弃并且可见。
- **顶点缓冲对象(Vertex Buffer Object)**： 一个调用显存并存储所有顶点数据供显卡使用的缓冲对象。
- **顶点数组对象(Vertex Array Object)**： 存储缓冲区和顶点属性状态。
- **索引缓冲对象(Element Buffer Object)**： 一个存储索引供索引化绘制使用的缓冲对象。
- **Uniform**： 一个特殊类型的GLSL变量。它是全局的（在一个着色器程序中每一个着色器都能够访问uniform变量），并且只需要被设定一次。
- **纹理(Texture)**： 一种包裹着物体的特殊类型图像，给物体精细的视觉效果。
- **纹理缠绕(Texture Wrapping)**： 定义了一种当纹理顶点超出范围(0, 1)时指定OpenGL如何采样纹理的模式。
- **纹理过滤(Texture Filtering)**： 定义了一种当有多种纹素选择时指定OpenGL如何采样纹理的模式。这通常在纹理被放大情况下发生。
- **多级渐远纹理(Mipmaps)**： 被存储的材质的一些缩小版本，根据距观察者的距离会使用材质的合适大小。
- **stb_image.h**： 图像加载库。
- **纹理单元(Texture Units)**： 通过绑定纹理到不同纹理单元从而允许多个纹理在同一对象上渲染。
- **向量(Vector)**： 一个定义了在空间中方向和/或位置的数学实体。
- **矩阵(Matrix)**： 一个矩形阵列的数学表达式。
- **GLM**： 一个为OpenGL打造的数学库。
- **局部空间(Local Space)**： 一个物体的初始空间。所有的坐标都是相对于物体的原点的。
- **世界空间(World Space)**： 所有的坐标都相对于全局原点。
- **观察空间(View Space)**： 所有的坐标都是从摄像机的视角观察的。
- **裁剪空间(Clip Space)**： 所有的坐标都是从摄像机视角观察的，但是该空间应用了投影。这个空间应该是一个顶点坐标最终的空间，作为顶点着色器的输出。OpenGL负责处理剩下的事情（裁剪/透视除法）。
- **屏幕空间(Screen Space)**： 所有的坐标都由屏幕视角来观察。坐标的范围是从0到屏幕的宽/高。
- **LookAt矩阵**： 一种特殊类型的观察矩阵，它创建了一个坐标系，其中所有坐标都根据从一个位置正在观察目标的用户旋转或者平移。
- **欧拉角(Euler Angles)**： 被定义为偏航角(Yaw)，俯仰角(Pitch)，和滚转角(Roll)从而允许我们通过这三个值构造任何3D方向。

# 颜色

看到的物体是反射回来的颜色，没被吸收的颜色

光学模型：

```c++
glm::vec3 cubeColor(1.0f, 1.0f, 1.0f);
glm::vec3 terrainColor(1.0f, 0.5f, 0.31f);
glm::vec3 result = cubeColor * terrainColor; // = (1.0f, 0.5f, 0.31f);
```

# 基础光照

冯氏光照模型(Phong Lighting Model)：

- 环境(Ambient)：整体的光亮
- 漫反射(Diffuse)：正对着量，背对着暗，一个平面的亮度是一样的，模拟平行光源
- 镜面(Specular)：高光，模拟点光源

## 环境光照

```glsl
void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * cubeColor;

    vec3 result = ambient * terrainColor;
    FragColor = vec4(result, 1.0);
}
```

## 漫反射光照

平行光照射到表面，入射角不一样单位面积的光照强度不一样，需要表面法向量进行计算

## 法线变换

当物体随着空间坐标平移时，法向量不变，但是物体旋转法向量会变化，物体不等比缩放也会

![不等比缩放](https://learnopengl-cn.github.io/img/02/02/basic_lighting_normal_transformation.png)

法线矩阵：[文章](http://www.lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/)

```glsl
Normal = mat3(transpose(inverse(model))) * aNormal;
```

> 即使是对于着色器来说，逆矩阵也是一个开销比较大的运算，因此，只要可能就应该避免在着色器中进行逆矩阵运算，它们必须为你场景中的每个顶点都进行这样的处理。用作学习目这样做是可以的，但是对于一个对效率有要求的应用来说，在绘制之前你最好用CPU计算出法线矩阵，然后通过uniform把值传递给着色器（像模型矩阵一样）。

## 镜面光照

![原理图](https://learnopengl-cn.github.io/img/02/02/basic_lighting_specular_theory.png)

夹角越小亮度越高

先计算点到相机的向量，然后根据光源到点的向量利用`reflect`函数根据面法向量计算反射光向量，然后取内积

```glsl
vec3 norm = normalize(Normal);
vec3 lightDir = normalize(lightPos - FragPos);
float diff = max(dot(norm, lightDir), 0.0);

vec3 viewDir = normalize(viewPos - FragPos);
// caculate reflection vector by reflect
vec3 reflectDir = reflect(-lightDir, norm);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
float specular = specularStrength * spec;
FragColor = vec4((ambientStrength + diff + specular) * cubeColor * terrainColor, transparent);
```

> 在顶点着色器中实现的冯氏光照模型叫做Gouraud着色(Gouraud Shading)，而不是冯氏着色(Phong Shading)。记住，由于插值，这种光照看起来有点逊色。冯氏着色能产生更平滑的光照效果。

# 材质

```glsl
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
    
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
```

材质的反射系数包含三种颜色，用vector

# 光照贴图

漫反射贴图和镜面贴图

环境光和漫反射

放射光贴图

```glsl
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;

// real position in world axis
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float transparent;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};


struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform sampler2D matrix;

uniform vec3 viewPos;

void main()
{
    // 环境光
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;

    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;

    // 镜面反射
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * texture(material.specular, TexCoord).rgb * spec;

    vec3 matrixColor = texture(matrix, TexCoord).rgb;
    float green = length(matrixColor);

    FragColor = vec4((1 - green) * (ambient + diffuse + specular) + green * matrixColor, transparent);
}
```

# 投光物

## 平行光

定向光源

```glsl
struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
```

可以使用vec4来代表光源方向

```glsl
if(lightVector.w == 0.0) // 注意浮点数据类型的误差
  // 执行定向光照计算
else if(lightVector.w == 1.0)
  // 根据光源的位置做光照计算（与上一节一样）
```

## 点光源

光照强度随着距离衰减
$$
\begin{equation} F_{att} = \frac{1.0}{K_c + K_l * d + K_q * d^2} \end{equation}
$$

- $K_c$通常大于等于1.0，保证值小于等于1
- 一次项线性衰减
- 二次项平方衰减

![衰减](https://learnopengl-cn.github.io/img/02/05/attenuation.png)

经验值[来源](http://www.ogre3d.org/tikiwiki/tiki-index.php?page=-Point+Light+Attenuation)：

| 距离 | 常数项 | 一次项 | 二次项   |
| ---- | ------ | ------ | -------- |
| 7    | 1.0    | 0.7    | 1.8      |
| 13   | 1.0    | 0.35   | 0.44     |
| 20   | 1.0    | 0.22   | 0.20     |
| 32   | 1.0    | 0.14   | 0.07     |
| 50   | 1.0    | 0.09   | 0.032    |
| 65   | 1.0    | 0.07   | 0.017    |
| 100  | 1.0    | 0.045  | 0.0075   |
| 160  | 1.0    | 0.027  | 0.0028   |
| 200  | 1.0    | 0.022  | 0.0019   |
| 325  | 1.0    | 0.014  | 0.0007   |
| 600  | 1.0    | 0.007  | 0.0002   |
| 3250 | 1.0    | 0.0014 | 0.000007 |

## 衰减实现

```glsl
struct Light {
    vec3 position;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
```

## 聚光

Spotlight，

![聚光](https://learnopengl-cn.github.io/img/02/05/light_casters_spotlight_angles.png)

```glsl
struct Light {
    vec3  position;
    vec3  direction;
    float cutOff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
```

比较$\theta$的余弦和cutOff的大小，大于则在里面

## 平滑/软化边缘

内外圆锥，如果在外圆锥外，强度为0，如果在两者之间，为0-1，如果在内则为1
$$
\begin{equation} I = \frac{\theta - \gamma}{\epsilon} \end{equation}
$$
这里$\epsilon$是内（$\phi$）和外圆锥（$\gamma$）之间的余弦值差（$\epsilon=\phi−\gamma$）。最终的*I*值就是在当前片段聚光的强度。

```glsl
struct Light {
    vec3  position;
    vec3  direction;
    float cutOff;
    float outerCutOff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
```

# 多光源

封装多个光源依次渲染

```glsl
out vec4 FragColor;

void main()
{
  // 定义一个输出颜色值
  vec3 output;
  // 将定向光的贡献加到输出中
  output += someFunctionToCalculateDirectionalLight();
  // 对所有的点光源也做相同的事情
  for(int i = 0; i < nr_of_point_lights; i++)
    output += someFunctionToCalculatePointLight();
  // 也加上其它的光源（比如聚光）
  output += someFunctionToCalculateSpotLight();

  FragColor = vec4(output, 1.0);
}
```

# 模型加载

## Assimp

使用Assimp加载模型和材质

## 数据结构

Assimp转化成自己的类用来调用OpenGL接口绘制

```c++
// 顶点，使用了C++的内存对齐，大小为8个float
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

// 纹理
struct Texture {
    unsigned int id;
    string type;
};

// 网格
class Mesh {
    public:
        /*  网格数据  */
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        /*  函数  */
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        void Draw(Shader shader);
    private:
        /*  渲染数据  */
        unsigned int VAO, VBO, EBO;
        /*  函数  */
        void setupMesh();
};  

// 初始化
void setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                 &indices[0], GL_STATIC_DRAW);

    // 顶点位置
    glEnableVertexAttribArray(0);   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // 顶点法线
    glEnableVertexAttribArray(1);   
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // 顶点纹理坐标
    glEnableVertexAttribArray(2);   
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
} 

// 渲染
void Draw(Shader shader) 
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
        // 获取纹理序号（diffuse_textureN 中的 N）
        string number;
        string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);

        shader.setFloat(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // 绘制网格
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
```

# 模型

使用之前的封装，用ASSIMP加载数据

# 深度测试

深度缓冲会存储每个片段的深度信息，当深度测试(Depth Testing)被启用的时候，OpenGL会将一个片段的深度值与深度缓冲的内容进行对比。OpenGL会执行一个深度测试，如果这个测试通过了的话，深度缓冲将会更新为新的深度值。如果深度测试失败了，片段将会被丢弃。

深度缓冲是片段着色器运行后运行，和`glViewPort`定义的窗口密切相关，`gl_FragCoord`从片段着色器之家访问，包含z分类代表深度值

```c++
// 开启
glEnable(GL_DEPTH_TEST);
// 每次绘制清理
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// 设置函数
glDepthMask(GL_LESS);
```

深度测试函数

| 函数        | 描述                                         |
| ----------- | -------------------------------------------- |
| GL_ALWAYS   | 永远通过深度测试                             |
| GL_NEVER    | 永远不通过深度测试                           |
| GL_LESS     | 在片段深度值小于缓冲的深度值时通过测试       |
| GL_EQUAL    | 在片段深度值等于缓冲区的深度值时通过测试     |
| GL_LEQUAL   | 在片段深度值小于等于缓冲区的深度值时通过测试 |
| GL_GREATER  | 在片段深度值大于缓冲区的深度值时通过测试     |
| GL_NOTEQUAL | 在片段深度值不等于缓冲区的深度值时通过测试   |
| GL_GEQUAL   | 在片段深度值大于等于缓冲区的深度值时通过测试 |

GL_ALWAYS，最后绘制的会覆盖掉之前绘制的

## 深度值的计算

$$
\begin{equation} F_{depth} = \frac{z - near}{far - near} \end{equation}
$$

远平面，近平面的约束条件

但通常不会使用线性的方式，因为z从很近到一般近的时候人的分辨能力最高，需要z值更加精确，所以采用曲线方式
$$
\begin{equation} F_{depth} = \frac{1/z - 1/near}{1/far - 1/near} \end{equation}
$$
![deep](https://learnopengl-cn.github.io/img/04/01/depth_non_linear_graph.png)

在z从近到远的时候变换率下降

## 深度缓冲的可视化

```glsl
void main()
{
    // 使用gl_fragCoord获取片元坐标
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}
```

## 深度冲突

两个面重叠，移动视角时会不断地闪烁，呈现奇怪的花纹

解决办法

- 不要靠太近
- 近平面设置远一些
- 增加深度缓冲精度

# 模板测试

模板测试的目的：利用已经本次绘制的物体，产生一个区域，在下次绘制中利用这个区域做一些效果。

片元着色器后执行模板测试，模板测试后执行深度测试。模板缓冲是一个8bit的矩阵

![模板缓冲](https://learnopengl-cn.github.io/img/04/02/stencil_buffer.png)

使用过程

- 启用模板缓冲写入
- 渲染物体，更新模板缓冲
- 禁用模板缓冲写入
- 渲染其他物体

```c++
// 启用
glEnable(GL_STENCIL_TEST);
// 清除
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
// 设置掩码
glStencilMask(0xFF);
```

## 模板函数

`glStencilFunc(GLenum func, GLint ref, GLuint mask)`

- func为函数
- ref为参考值
- mask为和参考值比较之前（AND）运算的掩码

例如

```c++
// 只要片段模板值等于1，片段就会绘制
glStencilFunc(GL_EQUAL, 1, 0xFF)
```

`glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)`

- sfail为模板测试失败时采取的行为
- dpfail为模板测试通过但是深度测试失败采取的行为
- dppass为全部通过时采取的行为

| 行为         | 描述                                               |
| ------------ | -------------------------------------------------- |
| GL_KEEP      | 保持当前储存的模板值                               |
| GL_ZERO      | 将模板值设置为0                                    |
| GL_REPLACE   | 将模板值设置为glStencilFunc函数设置的`ref`值       |
| GL_INCR      | 如果模板值小于最大值则将模板值加1                  |
| GL_INCR_WRAP | 与GL_INCR一样，但如果模板值超过了最大值则归零      |
| GL_DECR      | 如果模板值大于最小值则将模板值减1                  |
| GL_DECR_WRAP | 与GL_DECR一样，但如果模板值小于0则将其设置为最大值 |
| GL_INVERT    | 按位翻转当前的模板缓冲值                           |

默认情况都为`GL_KEEP`

## 物体轮廓

步骤：

1. 在绘制（需要添加轮廓的）物体之前，将模板函数设置为GL_ALWAYS，每当物体的片段被渲染时，将模板缓冲更新为1。
2. 渲染物体。
3. 禁用模板写入以及深度测试。
4. 将每个物体缩放一点点。
5. 使用一个不同的片段着色器，输出一个单独的（边框）颜色。
6. 再次绘制物体，但只在它们片段的模板值不等于1时才绘制。
7. 再次启用模板写入和深度测试。

## 总结

模板测试的目的：利用已经本次绘制的物体，产生一个区域，在下次绘制中利用这个区域做一些效果。

模板测试的有两个要点：
模板测试，用于剔除片段
模板缓冲更新，用于更新出一个模板区域出来，为下次绘制做准备

模板缓冲区(Stencil Buffer)：与颜色缓冲区和深度缓冲区类似，模板缓冲区可以为屏幕上的每个像素点保存一个无符号整数值(8位，最大是256)。
模板掩码函数，这里指定了一个像素点在模板缓冲区中的模板值哪些位是可以被修改的
glStencilMask(0xFF); // 每一位都可以被修改，即启用模板缓冲写入
glStencilMask(0x00); // 每一位都不可以被修改，即禁用模板缓冲写入

模板测试的函数，这里指定是什么情况下通过模板测试
比较流程：掩码值 mask 和参考值 ref 值先做与操作，再把当前模板中的值 stencil 与掩码值 mask 做与操作，然后参考 func  中的方法是否可以通过。这个比较方式使用了第三个参数 mask，例如 GL_LESS 通过，当且仅当 满足: ( stencil &  mask ) ref < ( stencil & mask )。GL_GEQUAL通过，当且仅当( stencil &  mask ) >= ( ref & mask )。

 物体轮廓理解：
 1、开启并设置模板测试条件为：总是通过测试，即本次绘制的所有片段都会通过测试并更新模板值；
 2、绘制物体并更新模板值；
 3、禁用模板缓冲写入；
 4、修改模板测试条件：没有模板值得片段才通过测试，意味着这次绘制会丢弃掉之前绘制的物体区域（并不影响上次的绘制，只会影响接下来的绘制）

>  模板测试还可以用来剔除镜子外面的内容

# 混合

实现透明度，通过alpha通道

## 丢弃透明的片段

```glsl
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{             
    vec4 texColor = texture(texture1, TexCoords);
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
}
```

> 注意，当采样纹理的边缘的时候，OpenGL会对边缘的值和纹理下一个重复的值进行插值（因为我们将它的环绕方式设置为了GL_REPEAT。这通常是没问题的，但是由于我们使用了透明值，纹理图像的顶部将会与底部边缘的纯色值进行插值。这样的结果是一个半透明的有色边框，你可能会看见它环绕着你的纹理四边形。要想避免这个，每当你alpha纹理的时候，请将纹理的环绕方式设置为GL_CLAMP_TO_EDGE：
>
> 

```c++
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
```

## 混合半透明的片段

```c++
// 启用
glEnable(GL_BLEND);
```

混合方程：
$$
\begin{equation}\bar{C}_{result} = \bar{\color{green}C}_{source} * {\color{green}F}_{source} + \bar{\color{red}C}_{destination} * {\color{red}F}_{destination}\end{equation}
$$

- $\bar{\color{green}C}_{source}$：源颜色向量。这是源自纹理的颜色向量。
- $\bar{\color{red}C}_{destination}$：目标颜色向量。这是当前储存在颜色缓冲中的颜色向量。
- ${\color{green}F}_{source}$：源因子值。指定了alpha值对源颜色的影响。
- ${\color{red}F}_{destination}$：目标因子值。指定了alpha值对目标颜色的影响。

$$
\begin{equation}\bar{C}_{result} = \begin{pmatrix} \color{red}{0.0} \\ \color{green}{1.0} \\ \color{blue}{0.0} \\ \color{purple}{0.6} \end{pmatrix} * {\color{green}{0.6}} + \begin{pmatrix} \color{red}{1.0} \\ \color{green}{0.0} \\ \color{blue}{0.0} \\ \color{purple}{1.0} \end{pmatrix} * \color{red}{(1 - 0.6)} \end{equation}
$$

$0.6$为$\bar{\color{green}C}_{source}$

`glBlendFunc(GLenum sfactor, GLenum dfactor)`

因子选项

| 选项                          | 值                                                      |
| ----------------------------- | ------------------------------------------------------- |
| `GL_ZERO`                     | 因子等于0                                               |
| `GL_ONE`                      | 因子等于1                                               |
| `GL_SRC_COLOR`                | 因子等于源颜色向量$\bar{\color{green}C}_{source}$       |
| `GL_ONE_MINUS_SRC_COLOR`      | 因子等于$1-\bar{\color{green}C}_{source}$               |
| `GL_DST_COLOR`                | 因子等于目标颜色向量$\bar{\color{red}C}_{destination}$  |
| `GL_ONE_MINUS_DST_COLOR`      | 因子等于$1-\bar{\color{red}C}_{destination}$            |
| `GL_SRC_ALPHA`                | 因子等于$\bar{\color{green}C}_{source}$的alpha分量      |
| `GL_ONE_MINUS_SRC_ALPHA`      | 因子等于$1-\bar{\color{green}C}_{source}$的alpha分量    |
| `GL_DST_ALPHA`                | 因子等于$\bar{\color{red}C}_{destination}$的alpha分量   |
| `GL_ONE_MINUS_DST_ALPHA`      | 因子等于$1-\bar{\color{red}C}_{destination}$的alpha分量 |
| `GL_CONSTANT_COLOR`           | 因子等于常数颜色向量$\bar{\color{blue}C}_{constant}$    |
| `GL_ONE_MINUS_CONSTANT_COLOR` | 因子等于$1-\bar{\color{blue}C}_{constant}$              |
| `GL_CONSTANT_ALPHA`           | 因子等于$\bar{\color{blue}C}_{constant}$的alpha分量     |
| `GL_ONE_MINUS_CONSTANT_ALPHA` | 因子等于$1-\bar{\color{blue}C}_{constant}$的alpha分量   |

可以使用`glBlendFuncSeparate`为RGB通道和alpha通道设置不同的选项

```c++
glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
```

可以使用`glBlendEquation(GLenum mode)`来设置运算符，修改$\bar{C}_{result}$

- GL_FUNC_ADD：默认选项，将两个分量相加：$\bar{C}_{result}=\color{green}Src+\color{red}Dst$

- GL_FUNC_SUBTRACT：将两个分量相减： $\bar{C}_{result}=\color{green}Src-\color{red}Dst$

- GL_FUNC_REVERSE_SUBTRACT：将两个分量相减，但顺序相反：$\bar{C}_{result}=\color{red}Dst-\color{green}Src$

> 可以结合Photoshop图层叠加方法来找到对应的混合函数

## 渲染半透明纹理

```c++
// 启用混合
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
```

```glsl
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    // 不需要丢弃片段
    FragColor = texture(texture1, TexCoords);
}
```

## 不要打乱顺序

![顺序不对](https://learnopengl-cn.github.io/img/04/03/blending_incorrect_order.png)

由于绘制物体的顺序不对，深度测试时会丢掉被挡住的部分，而不是进行混合

原则：

1. 先绘制所有不透明的物体。
2. 对所有透明的物体排序。
3. 按顺序绘制所有透明的物体。

> 更高级的技术还有次序无关透明度(Order Independent Transparency, OIT)

# 面剔除

OpenGL根据顶点的环绕顺序判断该面是否需要绘制，比如实体内部的面是不需要进行绘制的，被遮挡。

## 环绕顺序

![环绕顺序](https://learnopengl-cn.github.io/img/04/04/faceculling_windingorder.png)

顶点数组里面的顺序决定了环绕顺序

```c++
// 启用
glEnable(GL_CULL_FACE);
```

`glCullFace(GL_FRONT)`

- `GL_BACK`：只剔除背向面。
- `GL_FRONT`：只剔除正向面。
- `GL_FRONT_AND_BACK`：剔除正向面和背向面。

`glFrontFace(GL_CCW)`

- `GL_CCW`：逆时针
- `GL_CW`：顺时针

# 帧缓冲

相当于大的缓冲单元，包含颜色、深度、模板等

## 创建

```c++
// 创建
unsigned int fbo;
glGenFramebuffers(1, &fbo);
// 绑定
glBindFramebuffer(GL_FRAMEBUFFER, fbo);

// 检查是否完整
if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

// 绑定
glBindFramebuffer(GL_FRAMEBUFFER, 0);
// 删除
glDeleteFramebuffers(1, &fbo);
```

## 纹理附件

纹理附加到帧缓冲，渲染会写到纹理中，图像，可以再次利用着色器使用纹理

```c++
unsigned int texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);

glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

纹理的data传入的是空指针，分配内存但是没有填充

```c++
// 把纹理附件附加到帧缓冲
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
```

参数：

- `target`：帧缓冲的目标（绘制、读取或者两者皆有）
- `attachment`：我们想要附加的附件类型。当前我们正在附加一个颜色附件。注意最后的`0`意味着我们可以附加多个颜色附件。我们将在之后的教程中提到。
- `textarget`：你希望附加的纹理类型
- `texture`：要附加的纹理本身
- `level`：多级渐远纹理的级别。我们将它保留为0。

还可以把深度和模板缓冲对象附加进去

- 深度缓冲类型是`GL_DEPTH_ATTACHMENT`，格式是`GL_DEPTH_COMPONENT`
- 模板缓冲类型是`GL_STENCIL_ATTACHMENT`，格式是`GL_STENCIL_INDEX`

也可以把深度缓冲和模板缓冲用一个纹理设置

```c++
// 纹理的32位包含24位深度信息和8位模板信息
glTexImage2D(
  GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, 
  GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
);

glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
```

## 渲染缓冲对象附件

渲染缓冲对象，离屏渲染，可以用`glReadPixels`读取像素，他的数据是原生格式，交换缓冲区速度非常快，每次渲染迭代后使用`glfwSwapBuffers`

```c++
// 创建
unsigned int rbo;
glGenRenderbuffers(1, &rbo);

// 绑定
glBindRenderbuffer(GL_RENDERBUFFER, rbo);

// 创建深度和模板渲染缓冲对象
glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
// 附加对象
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
```

<mark>选择：</mark>

- 不需要对缓冲中采样数据而是直接使用，则使用渲染缓冲对象
- 需要对缓冲中采样数据而是直接使用，则使用纹理附件

## 渲染到纹理

```c++
unsigned int framebuffer;
glGenFramebuffers(1, &framebuffer);
glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

// 生成纹理
unsigned int texColorBuffer;
glGenTextures(1, &texColorBuffer);
glBindTexture(GL_TEXTURE_2D, texColorBuffer);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glBindTexture(GL_TEXTURE_2D, 0);

// 将它附加到当前绑定的帧缓冲对象
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);  

unsigned int rbo;
glGenRenderbuffers(1, &rbo);
glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);  
glBindRenderbuffer(GL_RENDERBUFFER, 0);

glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
glBindFramebuffer(GL_FRAMEBUFFER, 0);


// 绘制
// 第一处理阶段(Pass)
glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 我们现在不使用模板缓冲
glEnable(GL_DEPTH_TEST);
DrawScene();    

// 第二处理阶段
glBindFramebuffer(GL_FRAMEBUFFER, 0); // 返回默认
glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
glClear(GL_COLOR_BUFFER_BIT);

screenShader.use();  
glBindVertexArray(quadVAO);
glDisable(GL_DEPTH_TEST);
glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
glDrawArrays(GL_TRIANGLES, 0, 6);  
```

## 后期处理

可以对片元渲染器添加计算改变颜色

由于渲染的是纹理，还可以做图像处理

```glsl
const float offset = 1.0 / 300.0;  

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );

    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    FragColor = vec4(col, 1.0);
}
```

可以用高斯核，拉普拉斯等做锐化糊化等，sobel算子提取边缘

# 立方体贴图

包含6个2D纹理的纹理，每个2D纹理都组成立方体一个面

![立方体贴图](https://learnopengl-cn.github.io/img/04/06/cubemaps_sampling.png)

根据向量的三个方向的分量来进行采样

```c++
// 创建
unsigned int textureID;
glGenTextures(1, &textureID);
glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
```

| 纹理目标                         | 方位 |
| -------------------------------- | ---- |
| `GL_TEXTURE_CUBE_MAP_POSITIVE_X` | 右   |
| `GL_TEXTURE_CUBE_MAP_NEGATIVE_X` | 左   |
| `GL_TEXTURE_CUBE_MAP_POSITIVE_Y` | 上   |
| `GL_TEXTURE_CUBE_MAP_NEGATIVE_Y` | 下   |
| `GL_TEXTURE_CUBE_MAP_POSITIVE_Z` | 后   |
| `GL_TEXTURE_CUBE_MAP_NEGATIVE_Z` | 前   |



```c++
// 调用6次
glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
```

设置纹理采样

```c++
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
```

shader使用

```glsl
in vec3 textureDir; // 代表3D纹理坐标的方向向量
uniform samplerCube cubemap; // 立方体贴图的纹理采样器

void main()
{             
    FragColor = texture(cubemap, textureDir);
}
```

## 天空盒

![skybox](https://learnopengl-cn.github.io/img/04/06/cubemaps_skybox.png)

