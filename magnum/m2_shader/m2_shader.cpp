//
// Created by jiang on 2021/9/27.
//

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/ImageView.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Context.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Math/Color.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/Shaders/VertexColor.h>

using namespace Magnum;

class CubeShader : public GL::AbstractShaderProgram
{
public:
    typedef GL::Attribute<0, Vector2> Position;
    typedef GL::Attribute<1, Vector3> Color;

    explicit CubeShader();

};

CubeShader::CubeShader()
{
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

    GL::Shader vert{GL::Version::GL330, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL330, GL::Shader::Type::Fragment};

    vert.addSource(R"(
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
out vec4 textureColor;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    textureColor = vec4(aColor, 1.0);
}
)");
    frag.addSource(R"(
out vec4 FragColor;
in vec4 textureColor;

void main()
{
    FragColor = textureColor;
}
)");

    vert.compile();
    frag.compile();

    attachShader(vert);
    attachShader(frag);

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

class CustomCamera : public Platform::Application
{
public:
    explicit CustomCamera(const Arguments &arguments);

private:
    void drawEvent() override;

    GL::Mesh mMesh;
    CubeShader mShader;
};

CustomCamera::CustomCamera(const Arguments &arguments) :
        Platform::Application{arguments, Configuration{}.setTitle("Shader")}
{
    using namespace Math::Literals;

    struct TriangleVertex
    {
        Vector2 position;
        Color3 color;
    };

    const TriangleVertex data[] = {
            {{-0.5f, -0.5f}, 0xff0000_rgbf},    /* Left vertex, red color */
            {{0.5f,  -0.5f}, 0x00ff00_rgbf},    /* Right vertex, green color */
            {{0.0f,  0.5f},  0x0000ff_rgbf}     /* Top vertex, blue color */
    };

    GL::Buffer buffer;
    buffer.setData(data);

    mMesh.setCount(3);
    mMesh.addVertexBuffer(std::move(buffer), 0,
                          CubeShader::Position{},
                          CubeShader::Color{});
}

void CustomCamera::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    using namespace Math::Literals;
    mShader.draw(mMesh);

    swapBuffers();
}

MAGNUM_APPLICATION_MAIN(CustomCamera)