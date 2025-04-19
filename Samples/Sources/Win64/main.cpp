#include "Application.h"
#include "Components/CameraControlComponent.h"
#include "CoreDefines.h"
#include "CoreTypes.h"
#include "Resource/MaterialResource.h"
#include "Resource/MeshResource.h"
#include "Resource/ShaderResource.h"
#include "Resource/TextureResource.h"
#include "Resource/ZEMaterialTypes.h"
#include "Render/Mesh.h"
#include "Render/Material.h"
#include "Scene/CameraComponent.h"
#include "Scene/MeshComponent.h"
#include "Scene/Scene.h"
#include "Scene/SceneObject.h"
#include "Scene/TransformComponent.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

ZE::TPtr<ZE::Scene> CreateSampleScene()
{
    ZE::TPtr<ZE::Scene> scene = std::make_shared<ZE::Scene>();

    ZE::TPtr<ZE::SceneObject> meshObject = std::make_shared<ZE::SceneObject>();
    ZE::TPtr<ZE::TransformComponent> meshTransformComponent = std::make_shared<ZE::TransformComponent>();
    meshObject->AddComponent(meshTransformComponent);


    ZE::TPtr<ZE::MeshResource> meshResource =
        std::make_shared<ZE::MeshResource>("./Samples/Resources/Meshes/viking_room.obj");
    ZE::TPtr<ZE::ShaderResource> vertexShaderResource =
        std::make_shared<ZE::ShaderResource>(ZE::EShaderStage::Vertex, "LocalToClipSpaceVertexShader.glsl");
    ZE::TPtr<ZE::ShaderResource> fragmentShaderResource =
        std::make_shared<ZE::ShaderResource>(ZE::EShaderStage::Fragment, "LambertBlinnPhoneFragmentShader.glsl");
    ZE::TPtr<ZE::TextureResource> texture =
        std::make_shared<ZE::TextureResource>("./Samples/Resources/Textures/viking_room.png");

    ZE::TPtr<ZE::PassResource> depthPass = std::make_shared<ZE::PassResource>();
    {
        depthPass->SetShader(ZE::EShaderStage::Vertex, vertexShaderResource);
        depthPass->SetTexture(ZE::EShaderStage::Fragment, 0, texture);

        ZE::DepthStencilState depthStencilState;
        depthStencilState.zTestType = ZE::ECompareOperation::Greater;
        depthStencilState.zWriteType = ZE::EZWriteType::Enable;
        depthPass->SetDepthStencilState(depthStencilState);
    }

    ZE::TPtr<ZE::PassResource> pass = std::make_shared<ZE::PassResource>();
    {
        pass->SetShader(ZE::EShaderStage::Vertex, vertexShaderResource);
        pass->SetShader(ZE::EShaderStage::Fragment, fragmentShaderResource);
        pass->SetTexture(ZE::EShaderStage::Fragment, 0, texture);

        ZE::DepthStencilState depthStencilState;
        depthStencilState.zTestType = ZE::ECompareOperation::Equal;
        depthStencilState.zWriteType = ZE::EZWriteType::Disable;
        pass->SetDepthStencilState(depthStencilState);
    }

    ZE::TPtr<ZE::MaterialResource> materialResource = std::make_shared<ZE::MaterialResource>();
    materialResource->SetPass(ZE::EPassType::DepthPass, depthPass);
    materialResource->SetPass(ZE::EPassType::BasePass, pass);

    ZE::TPtr<ZE::MeshComponent> meshComponent = std::make_shared<ZE::MeshComponent>();
    meshComponent->SetMesh(meshResource);
    meshComponent->SetMaterial(0, materialResource);
    meshObject->AddComponent(meshComponent);

    scene->AddObject(meshObject);

    ZE::TPtr<ZE::SceneObject> cameraObject = std::make_shared<ZE::SceneObject>();

    ZE::TPtr<ZE::TransformComponent> cameraTransformComponent = std::make_shared<ZE::TransformComponent>();
    glm::mat4x4 cameraTransform = glm::translate(glm::identity<glm::mat4x4>(), glm::vec3(0.0f, 0.0f, 10.0f));
    cameraTransformComponent->SetTransform(cameraTransform);
    cameraObject->AddComponent(cameraTransformComponent);

    ZE::TPtr<ZE::CameraComponent> cameraComponent = std::make_shared<ZE::CameraComponent>();
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    cameraComponent->SetProjectMatrix(projectionMatrix);
    cameraObject->AddComponent(cameraComponent);

    ZE::TPtr<CameraControlComponent> cameraControlComponent = std::make_shared<CameraControlComponent>();
    cameraObject->AddComponent(cameraControlComponent);

    scene->SetCamera(cameraComponent);

    return scene;
}

void InitScene(ZE::TPtr<ZE::Scene> scene)
{
    const ZE::TPtrArr<ZE::SceneObject>& objects = scene->GetObjects();

    for (ZE::TPtr<ZE::SceneObject> object : objects)
    {
        ZE::TPtr<ZE::MeshComponent> meshComponent = object->GetComponent<ZE::MeshComponent>();
        if (meshComponent == nullptr)
            continue;

            ZE::TPtr<ZE::MeshResource> meshResource = meshComponent->GetMesh();
        if (meshResource != nullptr)
        {
            ZE::TPtr<ZE::Mesh> mesh = std::make_shared<ZE::Mesh>(meshResource);
            meshResource->SetMesh(mesh);
        }

        ZE::TPtr<ZE::MaterialResource> materialResource = meshComponent->GetMaterial(0);
        if (materialResource != nullptr)
        {
            ZE::TPtr<ZE::Material> material = std::make_shared<ZE::Material>(materialResource);
            materialResource->SetMaterial(material);

            for (int i = 0; i < static_cast<int>(ZE::EPassType::PassCount); i++)
            {
                ZE::EPassType passType = static_cast<ZE::EPassType>(i);

                ZE::TPtr<ZE::PassResource> passResource = materialResource->GetPass(passType);
                if (passResource != nullptr)
                {
                    ZE::TPtr<ZE::Pass> pass = std::make_shared<ZE::Pass>(passResource);
                    material->SetPass(passType, pass);
                }
            }
        }
    }
}

int main()
{
    ZE::Application app;

    ZE::TPtr<ZE::Scene> scene = CreateSampleScene();
    InitScene(scene);
    app.Run(scene);

    return 0;
}