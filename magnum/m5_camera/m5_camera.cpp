//
// Created by jiang on 2021/9/28.
//

/***************************************************************************************
*          例子: https://doc.magnum.graphics/magnum/examples-viewer.html
***************************************************************************************/

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

using namespace Magnum;
using namespace Magnum::Math::Literals;

using Object3D = SceneGraph::Object<SceneGraph::MatrixTransformation3D>;
using Scene3D = SceneGraph::Scene<SceneGraph::MatrixTransformation3D>;

class ColoredDrawable : public SceneGraph::Drawable3D
{
public:
    explicit ColoredDrawable(Object3D &object, Shaders::Phong &shader, GL::Mesh &mesh,
                             const Color4 &color, SceneGraph::DrawableGroup3D &group)
            :
            SceneGraph::Drawable3D{object, &group},
            mShader(shader),
            mMesh(mesh),
            mColor{color}
    {}

private:
    void draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &camera) override;

    Shaders::Phong &mShader;
    GL::Mesh &mMesh;
    Color4 mColor;
};

void ColoredDrawable::draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &camera)
{
    mShader.setLightPosition(camera.cameraMatrix().transformPoint({-3.0f, 10.0f, 10.0f}))
            .setTransformationMatrix(transformationMatrix)
            .setProjectionMatrix(camera.projectionMatrix())
            .setDiffuseColor(mColor)
            .draw(mMesh);
}

class TexturedDrawable : public SceneGraph::Drawable3D
{
public:
    explicit TexturedDrawable(Object3D &object, Shaders::Phong &shader, GL::Mesh &mesh,
                              GL::Texture2D &texture, SceneGraph::DrawableGroup3D &group) :
            SceneGraph::Drawable3D{object, &group}, mShader(shader), mMesh(mesh), mTexture(texture)
    {}

private:
    void draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &camera) override;

    Shaders::Phong &mShader;
    GL::Mesh &mMesh;
    GL::Texture2D &mTexture;
};

void TexturedDrawable::draw(const Matrix4 &transformationMatrix, SceneGraph::Camera3D &camera)
{
    mShader.setLightPosition(camera.cameraMatrix().transformPoint({-3.0f, 10.0f, 10.0f}))
            .setTransformationMatrix(transformationMatrix)
            .setProjectionMatrix(camera.projectionMatrix())
            .bindDiffuseTexture(mTexture)
            .draw(mMesh);
}

class CustomCamera : public Platform::Application
{
public:
    explicit CustomCamera(const Arguments &arguments);
    void mainLoop();

private:
    void drawEvent() override;
    void viewportEvent(ViewportEvent &event) override;
    void mouseMoveEvent(MouseMoveEvent &event) override;
    void addObject(Trade::AbstractImporter &importer, Object3D &parent, UnsignedInt i);
    Vector3 positionOnSphere(const Vector2i &position) const;
    void forward();
    void backward();
    void left();
    void right();
    void up();
    void down();
    void processKey();

    Shaders::Phong mColoredShader;
    Shaders::Phong mTexturedShader{Shaders::Phong::Flag::DiffuseTexture};
    Containers::Array<Containers::Optional<GL::Mesh>> mMeshes;
    Containers::Array<Containers::Optional<GL::Texture2D>> mTextures;
    Containers::Array<Containers::Optional<Trade::PhongMaterialData>> mMaterials;

    Scene3D mScene;
    Object3D mManipulator;
    Object3D mCameraObject;
    SceneGraph::Camera3D *mCamera;
    SceneGraph::DrawableGroup3D mDrawables;
    Vector3 mPreviousPosition;
};

CustomCamera::CustomCamera(const Arguments &arguments) :
        Platform::Application{arguments,
                              Configuration{}
                                      .setTitle("Camera")
                                      .setSize({800, 800})
                                      .setWindowFlags(Configuration::WindowFlag::Resizable)
        }
{
    glfwSetInputMode(window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    mCameraObject.setParent(&mScene)
            .translate(Vector3::zAxis(2.0f));
    mCamera = new SceneGraph::Camera3D{mCameraObject};
    (*mCamera).setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
            .setProjectionMatrix(Matrix4::perspectiveProjection(45.0_radf, 1.0f, 0.1f, 100.0f))
            .setViewport(GL::defaultFramebuffer.viewport().size());
    mManipulator.setParent(&mScene);

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    mColoredShader
            .setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0xffffff_rgbf)
            .setShininess(80.0f);
    mTexturedShader
            .setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0x111111_rgbf)
            .setShininess(80.0f);

    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> importer =
            manager.loadAndInstantiate("AssimpImporter");
    if (!importer) {
        Error{} << "load importer failed";
        exit(-1);
    }

    if (!importer->openFile(RES_DIR PATH_SEP "gun/Handgun_obj.obj")) {
        Error{} << "load model failed";
        exit(-1);
    }

    // 加载纹理
    mTextures = decltype(mTextures){importer->textureCount()};
    for (auto i = 0; i != importer->textureCount(); ++i) {
        Debug{} << "Importing texture" << i << importer->textureName(i);
        auto textureData = importer->texture(i);
        if (!textureData ||
            textureData->type() != Trade::TextureData::Type::Texture2D) {
            Warning{} << "cannot load texture";
            continue;
        }

        Debug{} << "Importing image" << textureData->image()
                << importer->image2DName(textureData->image());

        auto imageData = importer->image2D(textureData->image());
        GL::TextureFormat format;
        if (imageData &&
            imageData->format() == PixelFormat::RGB8Unorm) {
            format = GL::TextureFormat::RGB8;
        } else if (imageData &&
                   imageData->format() == PixelFormat::RGBA8Unorm) {
            format = GL::TextureFormat::RGBA8;
        } else {
            Warning{} << "cannot load image";
            continue;
        }

        GL::Texture2D texture;
        texture.setMagnificationFilter(textureData->magnificationFilter())
                .setMinificationFilter(textureData->minificationFilter())
                .setWrapping(textureData->wrapping().xy())
                .setStorage(Math::log2(imageData->size().max()) + 1, format,
                            imageData->size()) // todo ？
                .setSubImage(0, {}, *imageData)
                .generateMipmap();

        mTextures[i] = std::move(texture);
    }

    // 加载材质
    mMaterials = decltype(mMaterials){importer->materialCount()};
    for (UnsignedInt i = 0; i != importer->materialCount(); ++i) {
        Debug{} << "Importing material" << i << importer->materialName(i);

        auto materialData = importer->material(i);
        if (!materialData || materialData->type() != Trade::MaterialType::Phong) {
            Warning{} << "Cannot load material, skipping";
            continue;
        }

        mMaterials[i] = std::move(dynamic_cast<Trade::PhongMaterialData &>(*materialData));
    }

    // 加载网格
    Utility::Debug{} << "mesh count: " << importer->meshCount();
    mMeshes = decltype(mMeshes){importer->meshCount()};
    for (unsigned int i = 0; i != importer->meshCount(); ++i) {
        Debug{} << "import mesh: " << importer->mesh(i)->indices().size();

        auto meshData = importer->mesh(i);
        if (!meshData ||
            !meshData->hasAttribute(Trade::MeshAttribute::Normal) ||
            meshData->primitive() != MeshPrimitive::Triangles) {
            continue;
        }
        mMeshes[i] = MeshTools::compile(*meshData);
    }

    // 加载场景
    if (importer->defaultScene() != -1) {   // 看文件是否自带scene
        Debug{} << "Adding default scene" << importer->sceneName(importer->defaultScene());

        Containers::Optional<Trade::SceneData> sceneData = importer->scene(
                importer->defaultScene());
        if (!sceneData) {
            Error{} << "Cannot load scene, exiting";
            return;
        }

        /* Recursively add all children */
        for (UnsignedInt objectId: sceneData->children3D())
            addObject(*importer, mManipulator, objectId);

        /* The format has no scene support, display just the first loaded mesh with
           a default material and be done with it */
    } else if (!mMeshes.empty()) {
        Debug{} << "load manually";
    }
}

void CustomCamera::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color |
                                 GL::FramebufferClear::Depth);

    using namespace Math::Literals;
    mCamera->draw(mDrawables);
    swapBuffers();
}

float CAMERA_SPEED = 0.1f;

void CustomCamera::viewportEvent(ViewportEvent &event)
{
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    mCamera->setViewport(event.windowSize());
}

void CustomCamera::addObject(Trade::AbstractImporter &importer, Object3D &parent, UnsignedInt i)
{
    Debug{} << "Importing object: " << i << importer.object3DName(i);
    auto objectData = importer.object3D(i);
    if (!objectData) {
        Error{} << "Cannot import object, skipping";
        return;
    }
    /* Add the object to the scene and set its transformation */
    auto *object = new Object3D{&parent};
    object->setTransformation(objectData->transformation());

    /* Add a drawable if the object has a mesh and the mesh is loaded */
    if (objectData->instanceType() == Trade::ObjectInstanceType3D::Mesh &&
        objectData->instance() != -1 && mMeshes[objectData->instance()]) {
        const Int materialId = dynamic_cast<Trade::MeshObjectData3D *>(objectData.get())->material();

        /* Material not available / not loaded, use a default material */
        if (materialId == -1 || !mMaterials[materialId]) {
            new ColoredDrawable{*object, mColoredShader, *mMeshes[objectData->instance()],
                                0xffffff_rgbf, mDrawables};

            /* Textured material. If the texture failed to load, again just use a
               default colored material. */
        } else if (mMaterials[materialId]->flags() &
                   Trade::PhongMaterialData::Flag::DiffuseTexture) {
            Containers::Optional<GL::Texture2D> &texture = mTextures[mMaterials[materialId]->diffuseTexture()];
            if (texture)
                new TexturedDrawable{*object, mTexturedShader, *mMeshes[objectData->instance()],
                                     *texture, mDrawables};
            else
                new ColoredDrawable{*object, mColoredShader, *mMeshes[objectData->instance()],
                                    0xffffff_rgbf, mDrawables};

            /* Color-only material */
        } else {
            new ColoredDrawable{*object, mColoredShader, *mMeshes[objectData->instance()],
                                mMaterials[materialId]->diffuseColor(), mDrawables};
        }
    }

    /* Recursively add children */
    for (std::size_t id: objectData->children())
        addObject(importer, *object, id);


}

void CustomCamera::forward()
{
    mCameraObject.translateLocal(Vector3::zAxis(-CAMERA_SPEED));
    redraw();
}

void CustomCamera::backward()
{
    mCameraObject.translateLocal(Vector3::zAxis(CAMERA_SPEED));
    redraw();
}

void CustomCamera::left()
{
    mCameraObject.translateLocal(Vector3::xAxis(-CAMERA_SPEED));
    redraw();
}

void CustomCamera::right()
{
    mCameraObject.translateLocal(Vector3::xAxis(CAMERA_SPEED));
    redraw();
}

void CustomCamera::up()
{
    mCameraObject.translateLocal(Vector3::yAxis(CAMERA_SPEED));
    redraw();
}

void CustomCamera::down()
{
    mCameraObject.translateLocal(Vector3::yAxis(-CAMERA_SPEED));
    redraw();
}

void CustomCamera::processKey()
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
        CustomCamera::exit(0);
    }
}

bool posInit = false;
Vector2i lastPos;
float sensitivity = 0.05f;

float yaw = -90.0f;
float pitch = 0;

void CustomCamera::mouseMoveEvent(MouseMoveEvent &event)
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

void CustomCamera::mainLoop()
{
    while (mainLoopIteration()) {
        processKey();
    }
}

int main(int argc, char **argv)
{
    CustomCamera app({argc, argv});
    app.mainLoop();
    return 0;
}