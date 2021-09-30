//
// Created by jiang on 2021/9/27.
//

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/ImageView.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Extensions.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Context.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/Platform/GLContext.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Math/Color.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/GL/Renderer.h>

using namespace Magnum;

class MeshShader : public GL::AbstractShaderProgram
{
public:
    typedef GL::Attribute<0, Vector2> Position;
    typedef GL::Attribute<1, Vector3> Color;

    explicit MeshShader();

};

//#define NORMAL

MeshShader::MeshShader()
{
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL420);
    MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::tessellation_shader);


#ifdef NORMAL
    GL::Shader vs{GL::Version::GL420, GL::Shader::Type::Vertex};
    GL::Shader fs{GL::Version::GL420, GL::Shader::Type::Fragment};
    vs.addFile(ROOT_PATH "/magnum/m8_tessellation/normal.vs.glsl");
    fs.addFile(ROOT_PATH "/magnum/m8_tessellation/normal.fs.glsl");

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile(
            {vs,
             fs}
    ));

    attachShaders({vs,
                   fs});
#else
    GL::Shader vs{GL::Version::GL420, GL::Shader::Type::Vertex};
    GL::Shader cs{GL::Version::GL420, GL::Shader::Type::TessellationControl};
    GL::Shader es{GL::Version::GL420, GL::Shader::Type::TessellationEvaluation};
//    GL::Shader gs{GL::Version::GL420, GL::Shader::Type::Geometry};
    GL::Shader fs{GL::Version::GL420, GL::Shader::Type::Fragment};
    vs.addFile(ROOT_PATH "/magnum/m8_tessellation/vs.glsl");
    cs.addFile(ROOT_PATH "/magnum/m8_tessellation/cs.glsl");
    es.addFile(ROOT_PATH "/magnum/m8_tessellation/es.glsl");
//    gs.addFile(ROOT_PATH "/magnum/m8_tessellation/gs.glsl");
    fs.addFile(ROOT_PATH "/magnum/m8_tessellation/fs.glsl");
    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile(
            {vs,
             cs,
             es,
             fs}
    ));

    attachShaders({vs,
                   cs,
                   es,
                   fs});
#endif


    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

class CustomTessellation : public Platform::Application
{
public:
    explicit CustomTessellation(const Arguments &arguments);

private:
    void drawEvent() override;

    GL::Mesh mMesh;
    MeshShader mShader;
};

CustomTessellation::CustomTessellation(const Arguments &arguments) :
        Platform::Application{arguments, Configuration{}.setTitle("Shader")}
{
//    Platform::Sdl2Application::GLConfiguration::setFlags()
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
    mMesh.setPrimitive(GL::MeshPrimitive::Patches);
    mMesh.addVertexBuffer(std::move(buffer), 0,
                          MeshShader::Position{},
                          MeshShader::Color{});
    GL::Renderer::setPolygonMode(GL::Renderer::PolygonMode::Line);
}

void CustomTessellation::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    using namespace Math::Literals;
    mShader.draw(mMesh);

    swapBuffers();
}

int main(int argc, char **argv)
{

    CustomTessellation app({argc, argv});
    return app.exec();
}