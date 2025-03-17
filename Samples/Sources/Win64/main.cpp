#include "Application.h"
#include "Components/CameraControlComponent.h"
#include "CoreDefines.h"
#include "Resource/MaterialResource.h"
#include "Resource/MeshResource.h"
#include "Resource/ShaderResource.h"
#include "Resource/TextureResource.h"
#include "Scene/CameraComponent.h"
#include "Scene/MeshComponent.h"
#include "Scene/Scene.h"
#include "Scene/SceneObject.h"
#include "Scene/TransformComponent.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

TE::TPtr<TE::Scene> CreateSampleScene()
{
    TE::TPtr<TE::Scene> scene = std::make_shared<TE::Scene>();

    TE::TPtr<TE::SceneObject> meshObject = std::make_shared<TE::SceneObject>();
    TE::TPtr<TE::TransformComponent> meshTransformComponent = std::make_shared<TE::TransformComponent>();
    meshObject->AddComponent(meshTransformComponent);


    TE::TPtr<TE::MeshResource> meshResource =
        std::make_shared<TE::MeshResource>("./Samples/Resources/Meshes/viking_room.obj");
    TE::TPtr<TE::ShaderResource> vertexShaderResource =
        std::make_shared<TE::ShaderResource>(TE::EShaderStage::Vertex, "LocalToClipSpaceVertexShader.glsl");
    TE::TPtr<TE::ShaderResource> fragmentShaderResource =
        std::make_shared<TE::ShaderResource>(TE::EShaderStage::Fragment, "LambertBlinnPhoneFragmentShader.glsl");
    TE::TPtr<TE::TextureResource> texture =
        std::make_shared<TE::TextureResource>("./Samples/Resources/Textures/viking_room.png");

    TE::TPtr<TE::MaterialResource> materialResource = std::make_shared<TE::MaterialResource>();
    materialResource->SetShader(TE::EShaderStage::Vertex, vertexShaderResource);
    materialResource->SetShader(TE::EShaderStage::Fragment, fragmentShaderResource);
    materialResource->SetTexture(TE::EShaderStage::Fragment, 0, texture);

    TE::TPtr<TE::MeshComponent> meshComponent = std::make_shared<TE::MeshComponent>();
    meshComponent->SetMesh(meshResource);
    meshComponent->SetMaterial(0, materialResource);
    meshObject->AddComponent(meshComponent);

    scene->AddObject(meshObject);

    TE::TPtr<TE::SceneObject> cameraObject = std::make_shared<TE::SceneObject>();

    TE::TPtr<TE::TransformComponent> cameraTransformComponent = std::make_shared<TE::TransformComponent>();
    glm::mat4x4 cameraTransform = glm::translate(glm::identity<glm::mat4x4>(), glm::vec3(0.0f, 0.0f, 10.0f));
    cameraTransformComponent->SetTransform(cameraTransform);
    cameraObject->AddComponent(cameraTransformComponent);

    TE::TPtr<TE::CameraComponent> cameraComponent = std::make_shared<TE::CameraComponent>();
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    cameraComponent->SetProjectMatrix(projectionMatrix);
    cameraObject->AddComponent(cameraComponent);

    TE::TPtr<CameraControlComponent> cameraControlComponent = std::make_shared<CameraControlComponent>();
    cameraObject->AddComponent(cameraControlComponent);

    scene->SetCamera(cameraComponent);

    return scene;
}

int main()
{
    TE::Application app;

    TE::TPtr<TE::Scene> scene = CreateSampleScene();
    app.Run(scene);

    return 0;
}