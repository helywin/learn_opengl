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
    enum : Int
    {
        TextureUnit = 0
    };

    enum : Int
    {
        Uniform_model = 0,
        Uniform_translation = 1,
        Uniform_view = 2,
        Uniform_projection = 3,
        Uniform_displacement = 4,
    };
public:
    typedef GL::Attribute<0, Vector4> Position;
    typedef GL::Attribute<1, Vector3> Color;

    explicit MeshShader();
    MeshShader &bindTexture(GL::Texture2D &texture);
};



MeshShader::MeshShader()
{
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL420);
    MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::tessellation_shader);

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


    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
    setUniform(0, TextureUnit);
}

MeshShader &MeshShader::bindTexture(GL::Texture2D &texture)
{
    texture.bind(TextureUnit);
    return *this;
}

class CustomTessellation : public Platform::Application
{
public:
    explicit CustomTessellation(const Arguments &arguments);

private:
    void drawEvent() override;

    GL::Mesh mMesh;
    MeshShader mShader;
    GL::Texture2D mTexture;
};

CustomTessellation::CustomTessellation(const Arguments &arguments) :
        Platform::Application{arguments, Configuration{}.setTitle("tessellation")
                .setSize({800, 800})}
{
//    Platform::Sdl2Application::GLConfiguration::setFlags()
    using namespace Math::Literals;

    struct TriangleVertex
    {
        Vector2 position;
        Color3 color;
    };

    float vertices[] = {
            0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
    };

    GL::Buffer buffer;
    buffer.setData(vertices);

    mMesh.setCount(4);
    mMesh.setPrimitive(GL::MeshPrimitive::Patches);
    mMesh.addVertexBuffer(std::move(buffer), 0,
                          MeshShader::Position{});
    GL::Renderer::setPolygonMode(GL::Renderer::PolygonMode::Line);
    GL::Renderer::setPatchVertexCount(4);

    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> importer =
            manager.loadAndInstantiate("StbImageImporter");
    if (!importer) {
        Error{} << "load importer failed";
        exit(-1);
    }
//    importer->openFile(RES_DIR "/height_short_mat_5.png");
    importer->openFile(RES_DIR "/avator.jpg");
//    Debug{} << importer->image2D(0)->format();

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);

//    GL::BufferImage<2> image(PixelFormat::R16Unorm, GL::PixelType::UnsignedShort, )
//mTexture.setImage()
    mTexture.setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setMinificationFilter(GL::SamplerFilter::Linear)
            .setStorage(1, GL::textureFormat(image->format()), image->size())
            .setSubImage(0, {}, *image);
    mShader.bindTexture(mTexture);
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