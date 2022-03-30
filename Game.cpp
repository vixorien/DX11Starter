#include "Game.h"
#include <iostream>
#include "DXCore.h"
#include "Vertex.h"
#include "Input.h"
#include "d3dcompiler.h"
#include "BufferStructs.h"
#include "GameEntity.h"
#include "Material.h"
#include "WICTextureLoader.h"

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
// For the DirectX Math library
using namespace DirectX;
// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true),			   // Show extra stats (fps) in title bar?
	//call transform constructor
	transform(),
	vsync(false)
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Note: Since we're using smart pointers (ComPtr),
	// we don't need to explicitly clean up those DirectX objects
	// - If we weren't using smart pointers, we'd need
	//   to call Release() on each DirectX object created in Game

	//make sure we offload our entities in our constructor
	for (auto& e : listOfEntitys) { delete e; }


}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Create a sampler state for texture sampling options
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // What happens outside the 0-1 uv range?
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;		// How do we handle sampling "between" pixels?
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, sampler.GetAddressOf());
	// Create a sampler state for texture sampling options
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler2;
	device->CreateSamplerState(&sampDesc, sampler2.GetAddressOf());
	//now that we have sampler state  load our textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rock;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brick;
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/brick.jpg").c_str(), 0, rock.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/rock.jpg").c_str(), 0, brick.GetAddressOf());


	LoadShaders();


	mat1 = std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT3(1, 1, 1), .9f);
	mat2 = std::make_shared<Material>(vertexShader, pixelShader2, XMFLOAT3(1, 1, 1), 1.0f);
	//set the resources for this material
	mat1->AddTextureSRV("SurfaceTexture", rock);
	mat1->AddSampler("BasicSampler", sampler);
	//set the resources for this material
	mat2->AddTextureSRV("SurfaceTexture", brick);
	mat2->AddSampler("BasicSampler", sampler2);


	CreateBasicGeometry();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//create our camera
	camera = std::make_shared<Camera>(0.0f, 0.0f, -5.0f, (float)width / height);

	LoadLights();
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	pixelShader2 = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"CustomPS.cso").c_str());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	shapeOne = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/sphere.obj").c_str(), device,context);
	shapeTwo = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/torus.obj").c_str(), device,context);
	shapeThree = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device,context);
	shapeFour = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/cylinder.obj").c_str(), device, context);
	shapeFive = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/helix.obj").c_str(), device, context);
	shapeSix = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/quad.obj").c_str(), device, context);

	//creating our 5 entitys
	GameEntity* entityOne = new GameEntity(shapeOne.get(),mat1);
	GameEntity* entityTwo = new GameEntity(shapeTwo.get(),mat1);
	GameEntity* entityThree = new  GameEntity(shapeThree.get(),mat2);
	GameEntity* entityFour = new GameEntity(shapeFour.get(),mat2);
	GameEntity* entityFive = new GameEntity(shapeFive.get(),mat2);
	
	//pushing entitys to list
	listOfEntitys.push_back(entityOne);
	listOfEntitys.push_back(entityTwo);
	listOfEntitys.push_back(entityThree);
	listOfEntitys.push_back(entityFour);
	listOfEntitys.push_back(entityFive);
	
	//making sure we put them in a good spot
	listOfEntitys[0]->getTransform()->SetPosition(0, 0, 0);
	listOfEntitys[1]->getTransform()->SetPosition(-2.5, 0, 0);
	listOfEntitys[2]->getTransform()->SetPosition(2.5, 0, 0);
	listOfEntitys[3]->getTransform()->SetPosition(-5.5, 0, 0);
	listOfEntitys[4]->getTransform()->SetPosition(5.5, 0, 0);
}

void Game::LoadLights()
{
	//create our ambient color taht will be sent to  pixel shader
	ambientColor = XMFLOAT3(0.1, 0.1, 0.25);

	Light dirLight1 = {};
	Light dirLight2 = {};
	Light dirLight3 = {};
	Light pointLight1 = {};
	Light pointLight2 = {};
	//setting only the values we need to set for a directional light
	dirLight1.Type = 0;
	dirLight2.Type = 0;
	dirLight3.Type = 0;
	pointLight1.Type = 1;
	pointLight2.Type = 1;
	//pointing right
	dirLight1.Direction = XMFLOAT3(1, 0, 0);
	dirLight2.Direction = XMFLOAT3(-1, 0, 0);
	dirLight3.Direction = XMFLOAT3(0, -1, 0);
	/// /////color////////////////
	dirLight1.Color = XMFLOAT3(.5, .5, .5);
	dirLight2.Color = XMFLOAT3(.5, .5, .5);
	dirLight3.Color = XMFLOAT3(.5, .5, .5);
	pointLight1.Color = XMFLOAT3(.5, .5, .5);
	pointLight2.Color = XMFLOAT3(1, 1, 1);
	/// //////intensity/////////////
	dirLight1.Intensity = .71;
	dirLight2.Intensity = .51;
	dirLight3.Intensity = .41;
	pointLight1.Intensity = .1;
	pointLight2.Intensity = .90;
	/// //////Position(not for directionals)/////////////
	pointLight1.Position = XMFLOAT3(-7, 3, 0);
	pointLight2.Position = XMFLOAT3(0, -1, 0);
	/// //////Range(not for directionals)/////////////
	pointLight1.Range = 10.0f;
	pointLight2.Range = 5.0f;
	lights.push_back(dirLight1);
	lights.push_back(dirLight2);
	lights.push_back(dirLight3);
	lights.push_back(pointLight1);
	lights.push_back(pointLight2);
}

void Game::LoadTexturesSRVsAndSampler()
{
	

	
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	//make sure we update our projection matrix when the screen resizes
	camera->UpdateProjectionMatrix((float)this->width / this->height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

	//make sure we update our camera
	camera->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	offset += .00001f;

	pixelShader->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());
	pixelShader2->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());

	//pass in the UV offset
	pixelShader->SetFloat("scale", offset);

	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
	
	//loop through and draw our entitys
	for (int i = 0; i < listOfEntitys.size(); i++) {
		//going to pass this jawn over to our shader here because for some reason this doesnt belong in entity class but wouldnt it make more sense to pass the ambient color into the entity instead of creating a seperation of tasks that just doesnt make a whole lot of sense, Yeah i get it, this is probably a little less cpu power but im not sure if its worth the loss in coesive code
		listOfEntitys[i]->GetMaterial()->GetPixelShader()->SetFloat3("ambient", ambientColor);
		listOfEntitys[i]->GetMaterial()->BindTexturesAndSamplers();

		listOfEntitys[i]->Draw(context, camera);
	}
	
	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(vsync ? 1 : 0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}