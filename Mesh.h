#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"

class Mesh
{
public:
	//our neccessary member variables
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>	context;
	int numberOfIndices;
	//createBudder(&verts[0],vertCounter,&indices[0],vertCounter, device);
	

	//our neccessary methods
	Mesh(Vertex vertexArray[], int numberOfVerticesInArray, unsigned int indicesArray[], int numberOfIndicesInArray, Microsoft::WRL::ComPtr<ID3D11Device> deviceObject, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	Mesh(Microsoft::WRL::ComPtr<ID3D11Device> deviceObject, const char* filename);
	void createBuffer(Vertex vertices[], int numOfVerts, unsigned int indices[], int numberOfIndices, Microsoft::WRL::ComPtr<ID3D11Device> deviceObject);
	~Mesh();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();//return the pointer to the vertex buffer object
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();//returns the number of indices this mesh contains.
	void Draw();
};

