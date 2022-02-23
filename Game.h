#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "GameEntity.h"
#include "DXCore.h"
#include <memory>
#include <vector>
#include "SimpleShader.h"
#include "Material.h"
class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	//creating our 3 meshes for our shapes
	
	

private:
	std::vector<GameEntity*> listOfEntitys;
	//shapes and meshes
	std::shared_ptr<Mesh> shapeOne;
	std::shared_ptr<Mesh> shapeTwo;
	std::shared_ptr<Mesh> shapeThree;
	//transform
	Transform transform;
	//camera
	std::shared_ptr<Camera> camera;
	//shaders
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	//materials
	std::shared_ptr<Material> mat1;
	std::shared_ptr<Material> mat2;
	std::shared_ptr<Material> mat3;
	// Should we use vsync to limit the frame rate?
	bool vsync;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateBasicGeometry();

	

	


};

