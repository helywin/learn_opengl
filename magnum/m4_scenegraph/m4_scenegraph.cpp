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
#include <Magnum/Platform/Sdl2Application.h>
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

class CustomSceneGraph : public Platform::Application
{
public:
    explicit CustomSceneGraph(const Arguments &arguments);

private:
    void drawEvent() override;
    void viewportEvent(ViewportEvent &event) override;
    void keyPressEvent(KeyEvent &event) override;
    void mousePressEvent(MouseEvent &event) override;
    void mouseReleaseEvent(MouseEvent &event) override;
    void mouseMoveEvent(MouseMoveEvent &event) override;
    void mouseScrollEvent(MouseScrollEvent &event) override;
    void addObject(Trade::AbstractImporter &importer, Object3D &parent, UnsignedInt i);
    Vector3 positionOnSphere(const Vector2i &position) const;

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

CustomSceneGraph::CustomSceneGraph(const Arguments &arguments) :
        Platform::Application{arguments,
                              Configuration{}
                                      .setTitle("SceneGraph")
                                      .setSize({800, 800})
                                      .setWindowFlags(Configuration::WindowFlag::Resizable)
        }
{
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

void CustomSceneGraph::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color |
                                 GL::FramebufferClear::Depth);

    using namespace Math::Literals;
    mCamera->draw(mDrawables);
    swapBuffers();
}

void CustomSceneGraph::viewportEvent(Platform::Sdl2Application::ViewportEvent &event)
{
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    mCamera->setViewport(event.windowSize());
}

void CustomSceneGraph::keyPressEvent(Platform::Sdl2Application::KeyEvent &event)
{

}

void CustomSceneGraph::mousePressEvent(Platform::Sdl2Application::MouseEvent &event)
{
    if (event.button() == MouseEvent::Button::Left)
        mPreviousPosition = positionOnSphere(event.position());
}

void CustomSceneGraph::mouseReleaseEvent(Platform::Sdl2Application::MouseEvent &event)
{
    if (event.button() == MouseEvent::Button::Left)
        mPreviousPosition = Vector3();
}

void CustomSceneGraph::mouseMoveEvent(Platform::Sdl2Application::MouseMoveEvent &event)
{
    if (!(event.buttons() & MouseMoveEvent::Button::Left)) return;

    const Vector3 currentPosition = positionOnSphere(event.position());
    const Vector3 axis = Math::cross(mPreviousPosition, currentPosition);

    if (mPreviousPosition.length() < 0.001f || axis.length() < 0.001f) return;

    mManipulator.rotate(Math::angle(mPreviousPosition, currentPosition), axis.normalized());
    mPreviousPosition = currentPosition;

    redraw();
}

void CustomSceneGraph::mouseScrollEvent(Platform::Sdl2Application::MouseScrollEvent &event)
{
    if (!event.offset().y()) return;

    /* Distance to origin */
    const Float distance = mCameraObject.transformation().translation().z();

    /* Move 15% of the distance back or forward */
    mCameraObject.translate(Vector3::zAxis(
            distance * (1.0f - (event.offset().y() > 0 ? 1 / 0.85f : 0.85f))));

    redraw();
}

void CustomSceneGraph::addObject(Trade::AbstractImporter &importer, Object3D &parent, UnsignedInt i)
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

Vector3 CustomSceneGraph::positionOnSphere(const Vector2i &position) const
{
    const Vector2 positionNormalized =
            Vector2{position} / Vector2{mCamera->viewport()} - Vector2{0.5f};
    const Float length = positionNormalized.length();
    const Vector3 result(
            length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized,
                                                                        1.0f - length));
    return (result * Vector3::yScale(-1.0f)).normalized();
}

MAGNUM_APPLICATION_MAIN(CustomSceneGraph)