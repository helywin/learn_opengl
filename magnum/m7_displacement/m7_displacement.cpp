//
// Created by jiang on 2021/9/29.
//
#define STB_IMAGE_IMPLEMENTATION

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Magnum/ImageView.h>
#include <Magnum/Mesh.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/BufferImage.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/TextureData.h>
#include <Magnum/Math/Color.h>
#include <Magnum/GL/Shader.h>
#include <stb_image.h>

using namespace Magnum;
using Object3D = SceneGraph::Object<SceneGraph::MatrixTransformation3D>;
using Scene3D = SceneGraph::Scene<SceneGraph::MatrixTransformation3D>;

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
    typedef GL::Attribute<0, Vector3> Position;

    explicit MeshShader();
    void setViewMatrix(const Matrix4 &mat);
    void setProjectionMatrix(const Matrix4 &mat);
    MeshShader &bindTexture(GL::Texture2D &texture);
};

MeshShader::MeshShader()
{
    GL::Shader vs{GL::Version::GL420, GL::Shader::Type::Vertex};
    GL::Shader cs{GL::Version::GL420, GL::Shader::Type::TessellationControl};
    GL::Shader es{GL::Version::GL420, GL::Shader::Type::TessellationEvaluation};
//    GL::Shader gs{GL::Version::GL420, GL::Shader::Type::Geometry};
    GL::Shader fs{GL::Version::GL420, GL::Shader::Type::Fragment};

    vs.addFile(ROOT_PATH "/magnum/m7_displacement/shader.vs.glsl");
    cs.addFile(ROOT_PATH "/magnum/m7_displacement/shader.cs.glsl");
    es.addFile(ROOT_PATH "/magnum/m7_displacement/shader.es.glsl");
//    gs.addFile(ROOT_PATH "/magnum/m7_displacement/shader.gs.glsl");
    fs.addFile(ROOT_PATH "/magnum/m7_displacement/shader.fs.glsl");

    ;
    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile(
            {vs, cs, es, fs}
    ));

    attachShaders({vs, cs, es, fs});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    Matrix4 mat{Math::IdentityInit};

    setUniform(Uniform_model, mat);
    setUniform(Uniform_translation, mat);
    setUniform(Uniform_view, mat);
    setUniform(Uniform_projection, mat);
    setUniform(Uniform_displacement, TextureUnit);
}

void MeshShader::setViewMatrix(const Matrix4 &mat)
{
    setUniform(Uniform_view, mat);
}

void MeshShader::setProjectionMatrix(const Matrix4 &mat)
{
    setUniform(Uniform_projection, mat);
}

MeshShader &MeshShader::bindTexture(GL::Texture2D &texture)
{
    texture.bind(TextureUnit);
    return *this;
}

class MeshGrid : public SceneGraph::Drawable3D
{
public:
    void draw(const Matrix4 &transformationMatrix, SceneGraph::Camera<3, Float> &camera) override;
    void setData();

    MeshGrid(Object3D &object, MeshShader &shader, SceneGraph::DrawableGroup3D &group) :
            SceneGraph::Drawable3D{object, &group},
            mShader(shader)
    {}

    GL::Mesh mMesh;

private:
    MeshShader &mShader;
};

void MeshGrid::draw(const Matrix4 &transformationMatrix, SceneGraph::Camera<3, Float> &camera)
{
    mShader.setViewMatrix(camera.cameraMatrix());
    mShader.setProjectionMatrix(camera.projectionMatrix());
    mShader.draw(mMesh);
}

void MeshGrid::setData()
{
    GL::Buffer buffer;
//    Debug{} << "begin set data" << d[size.x() * size.y() - 12431];

    Vector3 vertexes[] = {
            {0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f},
    };
    buffer.setData(vertexes, GL::BufferUsage::StaticDraw);

    mMesh.setPrimitive(GL::MeshPrimitive::Patches)
            .setCount(6)
            .addVertexBuffer(std::move(buffer), 0,
                             MeshShader::Position{});
}

class CustomMesh : public Platform::Application
{
public:
    explicit CustomMesh(const Arguments &arguments);
    void mainLoop();

private:
    void drawEvent() override;
    void viewportEvent(ViewportEvent &event) override;
    void mouseMoveEvent(MouseMoveEvent &event) override;
    void forward();
    void backward();
    void left();
    void right();
    void up();
    void down();
    void processKey();

    Scene3D mScene;
    Object3D mManipulator;
    Object3D mCameraObject;
    SceneGraph::Camera3D *mCamera;
    SceneGraph::DrawableGroup3D mDrawables;
    MeshShader mShader;
    MeshGrid *mMeshGrid;
    GL::Texture2D mTexture;
};

CustomMesh::CustomMesh(const Arguments &arguments) :
        Platform::Application{arguments, Configuration{}
                .setTitle("Mesh")
                .setSize({800, 800})
                .setWindowFlags(Configuration::WindowFlag::Resizable)
        }
{
    glfwSetInputMode(window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    using namespace Math::Literals;
    mCameraObject.setParent(&mScene)
            .translate(Vector3::zAxis(2.0f));
    mCamera = new SceneGraph::Camera3D{mCameraObject};
    (*mCamera).setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
            .setProjectionMatrix(Matrix4::perspectiveProjection(45.0_radf, 1.0f, 0.1f, 100.0f))
            .setViewport(GL::defaultFramebuffer.viewport().size());
    mManipulator.setParent(&mScene);

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::setPatchVertexCount(3);
//    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
//    GL::Renderer::setPolygonMode(GL::Renderer::PolygonMode::Line);
    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> importer =
            manager.loadAndInstantiate("StbImageImporter");
    if (!importer) {
        Error{} << "load importer failed";
        exit(-1);
    }
//    importer->openFile(RES_DIR "/height_short_mat_5.png");
    importer->openFile(RES_DIR "/displacement.png");
    Debug{} << importer->image2D(0)->format();

    Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);

    int width;
    int height;
    int nrComponents;
    unsigned short *data = stbi_load_16(RES_DIR "/height_short_mat_srgb.png", &width, &height,
                                        &nrComponents, 0);
//    GL::BufferImage<2> image(PixelFormat::R16Unorm, GL::PixelType::UnsignedShort, )
//mTexture.setImage()
    mTexture.setWrapping(GL::SamplerWrapping::ClampToEdge)
            .setMagnificationFilter(GL::SamplerFilter::Linear)
            .setMinificationFilter(GL::SamplerFilter::Linear)
            .setStorage(1, GL::textureFormat(image->format()), image->size())
            .setSubImage(0, {}, *image);
    mShader.bindTexture(mTexture);
    auto *object = new Object3D{&mManipulator};
    mMeshGrid = new MeshGrid{*object, mShader, mDrawables};
    mMeshGrid->setData();
}

void CustomMesh::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

    using namespace Math::Literals;
    mShader.draw(mMeshGrid->mMesh);
    Debug{} << "error:" << Magnum::GL::Renderer::error();
    swapBuffers();
}

void CustomMesh::viewportEvent(ViewportEvent &event)
{
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    mCamera->setViewport(event.windowSize());
}

float CAMERA_SPEED = 0.1f;

void CustomMesh::forward()
{
    mCameraObject.translateLocal(Vector3::zAxis(-CAMERA_SPEED));
    redraw();
}

void CustomMesh::backward()
{
    mCameraObject.translateLocal(Vector3::zAxis(CAMERA_SPEED));
    redraw();
}

void CustomMesh::left()
{
    mCameraObject.translateLocal(Vector3::xAxis(-CAMERA_SPEED));
    redraw();
}

void CustomMesh::right()
{
    mCameraObject.translateLocal(Vector3::xAxis(CAMERA_SPEED));
    redraw();
}

void CustomMesh::up()
{
    mCameraObject.translateLocal(Vector3::yAxis(CAMERA_SPEED));
    redraw();
}

void CustomMesh::down()
{
    mCameraObject.translateLocal(Vector3::yAxis(-CAMERA_SPEED));
    redraw();
}

void CustomMesh::processKey()
{
    auto win = window();
    auto keyPressed = [win](int key) { return glfwGetKey(win, key) == GLFW_PRESS; };
    if (keyPressed(GLFW_KEY_W)) {
        forward();
    }
    if (keyPressed(GLFW_KEY_S)) {
        backward();
    }
    if (keyPressed(GLFW_KEY_A)) {
        left();
    }
    if (keyPressed(GLFW_KEY_D)) {
        right();
    }
    if (keyPressed(GLFW_KEY_SPACE)) {
        up();
    }
    if (keyPressed(GLFW_KEY_C)) {
        down();
    }
    if (keyPressed(GLFW_KEY_ESCAPE)) {
        CustomMesh::exit(0);
    }
}

bool posInit = false;
Vector2i lastPos;
float sensitivity = 0.05f;

float yaw = -90.0f;
float pitch = 0;

void CustomMesh::mouseMoveEvent(MouseMoveEvent &event)
{
    if (!posInit) {
        lastPos = event.position();
        posInit = true;
        return;
    }

    auto offset = lastPos - event.position();
    lastPos = event.position();
    auto mat = mCameraObject.transformationMatrix().inverted();
    auto vec = mat * Vector4{0, 1, 0, 0};
    mCameraObject.rotateLocal(Deg(sensitivity * offset.x()), Vector3{vec.x(), vec.y(), vec.z()});
    mCameraObject.rotateXLocal(Deg(sensitivity * offset.y()));

    redraw();
}

void CustomMesh::mainLoop()
{
    while (mainLoopIteration()) {
        processKey();
    }
}

int main(int argc, char **argv)
{
    CustomMesh app({argc, argv});
    app.mainLoop();
    return 0;
}