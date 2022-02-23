#include "GameEntity.h"
#include "BufferStructs.h"
#include "Camera.h"
#include "Material.h"
#include <iostream>
GameEntity::GameEntity(Mesh* mesh, std::shared_ptr<Material> mat)
{
    //make sure we set our material
    SetMaterial(mat);
    entitysMesh = mesh;
}

GameEntity::~GameEntity()
{

}
//going to do option two because option 1 doesnt make sense to me
//passing in our constantbuffer and context so that we draw the idnividual entity we want
void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera)
{
    material->GetVertexShader()->SetShader();
    material->GetPixelShader()->SetShader();

    std::shared_ptr<SimpleVertexShader> vs = material->GetVertexShader();
    vs->SetFloat4("colorTint", material->GetColorTint());    // Strings here MUST  
    vs->SetMatrix4x4("worldMatrix", entitysTransform.BuildMatrix());   // match variable  
    vs->SetMatrix4x4("view", camera->GetViewMatrix());             // names in the  
    vs->SetMatrix4x4("projection", camera->GetProjectionMatrix()); // shader�s cbuffer!

    vs->CopyAllBufferData(); // Adjust �vs� variable name if necessary 
	// Draw the object
	entitysMesh->Draw();
}

Mesh* GameEntity::GetMesh()
{
    return entitysMesh;
}

Transform* GameEntity::getTransform()
{
    return &entitysTransform;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> mat)
{
    material = mat;
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
    return material;
}
