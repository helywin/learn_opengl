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

class MeshShader : public GL::AbstractShaderProgram
{
public:
    typedef GL::Attribute<0, Vector3> Position;
    typedef GL::Attribute<1, Vector2> TextureCoord;

    explicit MeshShader();
    MeshShader &bindTexture(GL::Texture2D &texture);

    enum : Int
    {
        TextureUnit = 0
    };

    Int mColorUniform;
};

MeshShader::MeshShader()
{
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL330);

    GL::Shader vert{GL::Version::GL330, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL330, GL::Shader::Type::Fragment};

    vert.addSource(R"(
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    TexCoord = aTexCoord;
}
)");
    frag.addSource(R"(
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
)");

    vert.compile();
    frag.compile();

    attachShader(vert);
    attachShader(frag);

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    setUniform(uniformLocation("texture1"), TextureUnit);
}

MeshShader &MeshShader::bindTexture(GL::Texture2D &texture)
{
    texture.bind(TextureUnit);
    return *this;
}

class CustomTexure : public Platform::Application
{
public:
    explicit CustomTexure(const Arguments &arguments);

private:
    void drawEvent() override;

    GL::Mesh mMesh;
    MeshShader mShader;
    GL::Texture2D mTexture;
};

CustomTexure::CustomTexure(const Arguments &arguments) :
        Platform::Application{arguments, Configuration{}.setTitle("Texture").setSize({800, 800})}
{
    using namespace Math::Literals;

    struct TriangleVertex
    {
        Vector3 position;
        Vector2 texCoord;
    };

    const TriangleVertex data[] = {
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
            {{0.5f,  -0.5f, -0.5f}, {1.0f, 0.0f}},
            {{0.5f,  0.5f,  -0.5f}, {1.0f, 1.0f}},
            {{0.5f,  0.5f,  -0.5f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f,  -0.5f}, {0.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},

            {{-0.5f, -0.5f, 0.5f},  {0.0f, 0.0f}},
            {{0.5f,  -0.5f, 0.5f},  {1.0f, 0.0f}},
            {{0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}},
            {{0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}},
            {{-0.5f, 0.5f,  0.5f},  {0.0f, 1.0f}},
            {{-0.5f, -0.5f, 0.5f},  {0.0f, 0.0f}},

            {{-0.5f, 0.5f,  0.5f},  {1.0f, 0.0f}},
            {{-0.5f, 0.5f,  -0.5f}, {1.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
            {{-0.5f, -0.5f, 0.5f},  {0.0f, 0.0f}},
            {{-0.5f, 0.5f,  0.5f},  {1.0f, 0.0f}},

            {{0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
            {{0.5f,  0.5f,  -0.5f}, {1.0f, 1.0f}},
            {{0.5f,  -0.5f, -0.5f}, {0.0f, 1.0f}},
            {{0.5f,  -0.5f, -0.5f}, {0.0f, 1.0f}},
            {{0.5f,  -0.5f, 0.5f},  {0.0f, 0.0f}},
            {{0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},

            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
            {{0.5f,  -0.5f, -0.5f}, {1.0f, 1.0f}},
            {{0.5f,  -0.5f, 0.5f},  {1.0f, 0.0f}},
            {{0.5f,  -0.5f, 0.5f},  {1.0f, 0.0f}},
            {{-0.5f, -0.5f, 0.5f},  {0.0f, 0.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},

            {{-0.5f, 0.5f,  -0.5f}, {0.0f, 1.0f}},
            {{0.5f,  0.5f,  -0.5f}, {1.0f, 1.0f}},
            {{0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
            {{0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
            {{-0.5f, 0.5f,  0.5f},  {0.0f, 0.0f}},
            {{-0.5f, 0.5f,  -0.5f}, {0.0f, 1.0f}},
    };

    GL::Buffer buffer;
    buffer.setData(data);

    mMesh.setCount(36);
    mMesh.addVertexBuffer(std::move(buffer), 0,
                          MeshShader::Position{},
                          MeshShader::TextureCoord{});

    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> importer =
            manager.loadAndInstantiate("StbImageImporter");
    if (!importer) {
        Utility::Debug{} << "load plugin failed";
        exit(-1);
    }
    if (!importer->openFile(RES_DIR PATH_SEP "container2.png")) {
        Utility::Debug{} << "load resource failed";
        exit(-1);
    }
    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_INTERNAL_ASSERT(image);
    mTexture.setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setMinificationFilter(GL::SamplerFilter::Linear)
            .setStorage(1, GL::textureFormat(image->format()), image->size())
            .setSubImage(0, {}, *image);
    mShader.bindTexture(mTexture);
}

void CustomTexure::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    using namespace Math::Literals;
    mShader.draw(mMesh);

    swapBuffers();
}

MAGNUM_APPLICATION_MAIN(CustomTexure)