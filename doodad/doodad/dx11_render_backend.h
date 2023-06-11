#pragma once
#include <d3d11_4.h>
#include <d3d10.h>
#include <stdint.h>

class DX11RenderBackend
{
public:
	static DX11RenderBackend* getInstance();
	void ResizeSwapchain(uint32_t width, uint32_t height);


	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device5* device = nullptr;
	ID3D11DeviceContext4* devCon = nullptr;
	ID3D11RenderTargetView* rtView = nullptr;

	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11Texture2D* depthStencilBuffer = nullptr;

	ID3D11Buffer* CreateConstantBuffer(uint32_t bufferSize, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, void* initData = nullptr);
	ID3D11Buffer* CreateStructuredBuffer(uint32_t structSize, uint32_t structCount, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, void* initData = nullptr);

	ID3D11ShaderResourceView* CreateBufferSRV(ID3D11Buffer* pBuffer);
	ID3D11UnorderedAccessView* CreateBufferUAV(ID3D11Buffer* pBuffer);

	ID3D11Texture2D* CreateTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t bindFlags, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, uint32_t mipLevels = 1, uint32_t arraySize = 1, uint32_t sampleCount = 1, uint32_t cpuAccessFlags = 0, uint32_t miscFlags = 0, D3D11_SUBRESOURCE_DATA* initData = nullptr);
	ID3D11RenderTargetView* CreateRTV(DXGI_FORMAT format, ID3D11Texture2D* texture);

	//todo: cleanup the srv creation to use the buffer to get the desc
	ID3D11ShaderResourceView* CreateTexture2DSRV(DXGI_FORMAT format, ID3D11Texture2D* texture, uint32_t mostDetailedMip = 0, uint32_t numMips = 1);
	ID3D11Fence* CreateFence(uint32_t initialVal, D3D11_FENCE_FLAG flags = D3D11_FENCE_FLAG_NONE);

	ID3D11Buffer* CreateVertexBuffer(uint32_t numVerts, uint32_t vertSize, const float* vertData, const D3D11_INPUT_ELEMENT_DESC* inputLayout);
	ID3D11Buffer* CreateIndexBuffer(uint32_t numVerts, const uint32_t* indices);

	ID3D11UnorderedAccessView* CreateTexture2DUAV(DXGI_FORMAT format, uint32_t mipSlice, ID3D11Texture2D* texture);

	void SetDefaultRenderTargets();

private:
	void ConstructObjectIDBufferInternal();
	DX11RenderBackend();
	~DX11RenderBackend();

};

DX11RenderBackend& DX();
