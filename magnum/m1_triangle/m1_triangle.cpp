//
// Created by jiang on 2021/9/27.
//

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Shaders/VertexColor.h>

using namespace Magnum;

class CustomTexture : public Platform::Application
{
public:
    explicit CustomTexture(const Arguments &arguments);

private:
    void drawEvent() override;

    GL::Mesh mMesh;
    Shaders::VertexColor2D mShader;
};

CustomTexture::CustomTexture(const Arguments &arguments) :
        Platform::Application{arguments, Configuration{}.setTitle("Triangle")}
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
                          Shaders::VertexColor2D::Position{},
                          Shaders::VertexColor2D::Color3{});
}

void CustomTexture::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    using namespace Math::Literals;
    mShader.draw(mMesh);

    swapBuffers();
}

MAGNUM_APPLICATION_MAIN(CustomTexture)