#include "dx11_render_backend.h"
#include "common_header.h"
#pragma comment(lib, "d3d11.lib")

#ifdef _DEBUG
bool isDebugBuild = true;
#else
bool isDebugBuild = false;
#endif

DX11RenderBackend& DX() { return *DX11RenderBackend::getInstance(); }

DX11RenderBackend* DX11RenderBackend::getInstance()
{
	static DX11RenderBackend* instance = nullptr;
	if (!instance)
	{
		instance = new DX11RenderBackend();
	}
	return instance;
}

DX11RenderBackend::DX11RenderBackend()
{
	DXGI_MODE_DESC backBufferDesc;
	ZeroMemory(&backBufferDesc, sizeof(DXGI_MODE_DESC));

	backBufferDesc.Width = App().GetScreenW();
	backBufferDesc.Height = App().GetScreenH();
	backBufferDesc.RefreshRate.Numerator = 60;
	backBufferDesc.RefreshRate.Denominator = 1;
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	backBufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	backBufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = backBufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = App().appWindowHandle;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //what to do with the old front buffer
	//the flip mode swapchains don't support msaa, so you have to resolve it yourself before presenting
	//but I'm not sure how to do that? TODO

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = App().GetScreenW();
	depthStencilDesc.Height = App().GetScreenH();
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;


	ID3D11Device* tempDevice;
	ID3D11DeviceContext* tempContext;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,  //use default adapter
		D3D_DRIVER_TYPE_HARDWARE, //use the gpu for direct3d
		NULL, //don't use software rasterizing
		isDebugBuild ? D3D11_CREATE_DEVICE_DEBUG : NULL, //no flags
		NULL, //use the highest feature level available
		NULL, //number of elements in the previous feature level array
		D3D11_SDK_VERSION,  //the version of the sdk to use
		&swapChainDesc,
		&swapChain,
		&tempDevice,
		NULL, //pointer to get the highest feature level available
		&tempContext);

	checkf(!FAILED(hr), "Failed to create swapchain and device");

	//we need a ID3D11Device5 interface so we can use fences to synchronize with compute shaders
	hr = tempDevice->QueryInterface(__uuidof(ID3D11Device5), (void**)&device);
	checkf(!FAILED(hr), "Failed to cast device to ID3D11Device5");
	check(device);

	hr = tempContext->QueryInterface(__uuidof(ID3D11DeviceContext4), (void**)&devCon);
	checkf(!FAILED(hr), "Failed to cast device to ID3D11DeviceContext4");
	check(devCon);

	ID3D11Texture2D* backBuffer;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	checkf(!FAILED(hr), "Failed to get back buffer");

	hr = device->CreateRenderTargetView(backBuffer, NULL, &rtView);
	checkf(!FAILED(hr), "Failed to create rtv for back buffer");
	backBuffer->Release();

	hr = device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	checkf(!FAILED(hr), "Failed to create depth buffer");

	hr = device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
	checkf(!FAILED(hr), "Failed to create depth view");


	SetDefaultRenderTargets();
}

DX11RenderBackend::~DX11RenderBackend()
{
	swapChain->Release();
	devCon->Release();
	device->Release();
}

void DX11RenderBackend::ResizeSwapchain(uint32_t width, uint32_t height)
{
	devCon->OMSetRenderTargets(0, 0, 0);
	rtView->Release();

	HRESULT hr;
	// Preserve the existing buffer count and format.
	// Automatically choose the width and height to match the client rect for HWNDs.
	hr = swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	ID3D11Texture2D* backBuffer;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	checkf(!FAILED(hr), "Failed to get back buffer");

	hr = device->CreateRenderTargetView(backBuffer, NULL, &rtView);
	checkf(!FAILED(hr), "Failed to create rtv for back buffer");
	backBuffer->Release();

	devCon->OMSetRenderTargets(1, &rtView, NULL);

	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	devCon->RSSetViewports(1, &vp);

	depthStencilView->Release();
	depthStencilBuffer->Release();

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	checkf(!FAILED(hr), "Failed to create depth buffer");

	hr = device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
	checkf(!FAILED(hr), "Failed to create depth view");

}

void DX11RenderBackend::SetDefaultRenderTargets()
{
	//bind the rtv to the output merger
	ID3D11RenderTargetView* rtvs[1] = { rtView };
	devCon->OMSetRenderTargets(1, rtvs, depthStencilView);
}


ID3D11Buffer* DX11RenderBackend::CreateStructuredBuffer(uint32_t structSize, uint32_t structCount, D3D11_USAGE usage, void* initData)
{
	ID3D11Buffer* outBuffer;

	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(D3D11_BUFFER_DESC));
	cbd.Usage = usage;

	cbd.ByteWidth = structSize * structCount;
	cbd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	cbd.StructureByteStride = structSize;

	if (initData)
	{
		D3D11_SUBRESOURCE_DATA iData;
		iData.pSysMem = initData;
		HRESULT err = device->CreateBuffer(&cbd, &iData, &outBuffer);
		check(err == S_OK);
	}
	else
	{
		HRESULT err = device->CreateBuffer(&cbd, NULL, &outBuffer);
		check(err == S_OK);
	}
	return outBuffer;
}

ID3D11Buffer* DX11RenderBackend::CreateConstantBuffer(uint32_t size, D3D11_USAGE usage, void* initData)
{
	ID3D11Buffer* outBuffer;

	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(D3D11_BUFFER_DESC));
	cbd.Usage = usage;

	cbd.ByteWidth = size;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = usage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;
	cbd.MiscFlags = 0;
	if (initData)
	{
		D3D11_SUBRESOURCE_DATA iData;
		iData.pSysMem = initData;
		HRESULT err = device->CreateBuffer(&cbd, &iData, &outBuffer);
		check(err == S_OK);
	}
	else
	{
		HRESULT err = device->CreateBuffer(&cbd, NULL, &outBuffer);
		check(err == S_OK);
	}

	return outBuffer;
}

ID3D11Buffer* DX11RenderBackend::CreateIndexBuffer(uint32_t numVerts, const uint32_t* indices)
{
	ID3D11Buffer* outBuff;

	D3D11_BUFFER_DESC vertBufferDesc;
	ZeroMemory(&vertBufferDesc, sizeof(vertBufferDesc));
	vertBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertBufferDesc.ByteWidth = numVerts * sizeof(uint32_t);
	vertBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vertBufferDesc.CPUAccessFlags = 0;
	vertBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertBufferData;
	ZeroMemory(&vertBufferData, sizeof(vertBufferData));

	vertBufferData.pSysMem = indices;

	HRESULT err = DX().device->CreateBuffer(&vertBufferDesc, &vertBufferData, &outBuff);
	check(err == S_OK);

	return outBuff;

}

ID3D11Buffer* DX11RenderBackend::CreateVertexBuffer(uint32_t numVerts, uint32_t vertSize, const float* vertData, const D3D11_INPUT_ELEMENT_DESC* inputLayout)
{
	ID3D11Buffer* outBuff;

	D3D11_BUFFER_DESC vertBufferDesc;
	ZeroMemory(&vertBufferDesc, sizeof(vertBufferDesc));
	vertBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertBufferDesc.ByteWidth = vertSize * numVerts;
	vertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBufferDesc.CPUAccessFlags = 0;
	vertBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertBufferData;
	ZeroMemory(&vertBufferData, sizeof(vertBufferData));

	vertBufferData.pSysMem = vertData;

	HRESULT err = DX().device->CreateBuffer(&vertBufferDesc, &vertBufferData, &outBuff);
	check(err == S_OK);

	return outBuff;

}

ID3D11Texture2D* DX11RenderBackend::CreateTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t bindFlags, D3D11_USAGE usage, uint32_t mipLevels, uint32_t arraySize, uint32_t sampleCount, uint32_t cpuAccessFlags, uint32_t miscFlags, D3D11_SUBRESOURCE_DATA* initData)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ID3D11Texture2D* outTexture;

	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = mipLevels;
	textureDesc.ArraySize = arraySize;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = sampleCount;
	textureDesc.Usage = usage;
	textureDesc.BindFlags = bindFlags;
	textureDesc.CPUAccessFlags = cpuAccessFlags;
	textureDesc.MiscFlags = miscFlags;

	// Create the texture
	HRESULT err = device->CreateTexture2D(&textureDesc, initData, &outTexture);
	check(err == S_OK);

	return outTexture;
}

ID3D11RenderTargetView* DX11RenderBackend::CreateRTV(DXGI_FORMAT format, ID3D11Texture2D* texture)
{
	ID3D11RenderTargetView* outRTV;

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	HRESULT err = device->CreateRenderTargetView(texture, &renderTargetViewDesc, &outRTV);
	check(err == S_OK);

	return outRTV;
}

ID3D11ShaderResourceView* DX11RenderBackend::CreateTexture2DSRV(DXGI_FORMAT format, ID3D11Texture2D* texture, uint32_t mostDetailedMip, uint32_t numMips)
{
	ID3D11ShaderResourceView* outSRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	shaderResourceViewDesc.Format = format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = mostDetailedMip;
	shaderResourceViewDesc.Texture2D.MipLevels = numMips;

	// Create the shader resource view.
	HRESULT err = device->CreateShaderResourceView(texture, &shaderResourceViewDesc, &outSRV);
	check(err == S_OK);
	return outSRV;
}

ID3D11UnorderedAccessView* DX11RenderBackend::CreateBufferUAV(ID3D11Buffer* pBuffer)
{
	D3D11_BUFFER_DESC descBuf = {};
	pBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;


	checkf((descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED), "only structured buffer SRVs are supported currently");

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	}

	ID3D11UnorderedAccessView* outUAV;

	HRESULT err = device->CreateUnorderedAccessView(pBuffer, &desc, &outUAV);
	check(err == S_OK);

	return outUAV;
}


ID3D11ShaderResourceView* DX11RenderBackend::CreateBufferSRV(ID3D11Buffer* pBuffer)
{
	D3D11_BUFFER_DESC descBuf = {};
	pBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;


	checkf((descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED), "only structured buffer SRVs are supported currently");

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	}


	ID3D11ShaderResourceView* outSRV;

	HRESULT err = device->CreateShaderResourceView(pBuffer, &desc, &outSRV);
	check(err == S_OK);

	return outSRV;
}


ID3D11UnorderedAccessView* DX11RenderBackend::CreateTexture2DUAV(DXGI_FORMAT format, uint32_t mipSlice, ID3D11Texture2D* texture)
{
	ID3D11UnorderedAccessView* outUAV;
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = mipSlice;

	HRESULT err = device->CreateUnorderedAccessView(texture, &uavDesc, &outUAV);
	check(err == S_OK);

	return outUAV;
}

ID3D11Fence* DX11RenderBackend::CreateFence(uint32_t initialVal, D3D11_FENCE_FLAG flags)
{
	ID3D11Fence* outFence = nullptr;
	HRESULT err = device->CreateFence(initialVal, flags, __uuidof(ID3D11Fence), (void**)&outFence);
	check(err == S_OK);

	return outFence;
}
