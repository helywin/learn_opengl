//
// Created by jiang on 2021/9/29.
//
#define STB_IMAGE_IMPLEMENTATION

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
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
    typedef GL::Attribute<0, Vector3> Position;

    explicit MeshShader();
    void setViewMatrix(const Matrix4 &mat);
    void setProjectionMatrix(const Matrix4 &mat);
};

MeshShader::MeshShader()
{
    GL::Shader vert{GL::Version::GL330, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GL330, GL::Shader::Type::Fragment};

    vert.addFile(ROOT_PATH "/magnum/m6_height_map/shader.vert");
    frag.addFile(ROOT_PATH "/magnum/m6_height_map/shader.frag");

    vert.compile();
    frag.compile();

    attachShader(vert);
    attachShader(frag);

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    Matrix4 mat{Math::IdentityInit};
    setUniform(0, mat);
    setUniform(1, mat);
    setUniform(2, mat);
    setUniform(3, mat);
}

void MeshShader::setViewMatrix(const Matrix4 &mat)
{
    setUniform(2, mat);
}

void MeshShader::setProjectionMatrix(const Matrix4 &mat)
{
    setUniform(3, mat);
}

class MeshGrid : public SceneGraph::Drawable3D
{

public:
    void draw(const Matrix4 &transformationMatrix, SceneGraph::Camera<3, Float> &camera) override;
    void setData(const unsigned short *data, const Vector2i &size);

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

void MeshGrid::setData(const unsigned short *data, const Vector2i &size)
{
    GL::Buffer buffer;
//    Debug{} << "begin set data" << d[size.x() * size.y() - 12431];
    int realRow = size.y() - size.y() % 2;
    auto vertexData = new Containers::Array<Vector3>(size.x() * realRow);
    int count = 0;
    bool inverse = false;
    for (int row = 0; row < realRow - 1; row += 1) {
        int colStart = 0;
        int colEnd = size.x();
        int colStep = 1;
        if (inverse) {
            std::swap(colStart, colEnd);
            colStep = -colStep;
        }
        for (int col = colStart; col != colEnd; col += colStep) {
            (*vertexData)[count++] = {col / 600.0f, ((float) row) / 760.0f,
                                      data[row * size.x() + col] / 3500.0f};
            (*vertexData)[count++] = {col / 600.0f, ((float) row + 1) / 760.0f,
                                      data[(row + 1) * size.x() + col] / 3500.0f};
        }
        inverse = !inverse;
    }
    buffer.setData(*vertexData, GL::BufferUsage::StaticDraw);

    GL::Buffer index;

    mMesh.setPrimitive(MeshPrimitive::TriangleStrip)
            .setCount(vertexData->size())
            .addVertexBuffer(std::move(buffer), 0, MeshShader::Position{});
    Debug{} << "end set data";
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
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> importer =
            manager.loadAndInstantiate("StbImageImporter");
    if (!importer) {
        Error{} << "load importer failed";
        exit(-1);
    }
    int width;
    int height;
    int nrComponents;
    unsigned short *data = stbi_load_16(RES_DIR "/height_short_mat_5.png", &width, &height,
                                        &nrComponents, 0);

    auto *object = new Object3D{&mManipulator};
    mMeshGrid = new MeshGrid{*object, mShader, mDrawables};
    mMeshGrid->setData(data, Vector2i{width, height});
}

void CustomMesh::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

    using namespace Math::Literals;
    mCamera->draw(mDrawables);
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