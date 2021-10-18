#pragma once


#define GR_USE_PLATFORM_WIN32

#include <mantle/mantleExt.h>
#include <mantle/mantleWsiWinExt.h>
#include <vector>


class grApi
{
public:
	grApi();
	~grApi();

	HWND hWnd = nullptr;

private:

	GR_VOID CreateDevice();
	GR_VOID DestroyDevice();

	GR_VOID CreatePresentableImage();
	GR_VOID DestroyPresentableImage();

	GR_VOID ConstructWindow();

	GR_VOID CreateColorTargetView();
	GR_VOID DestroyColorTargetView();

	GR_VOID CreateCommandBuffer();
	GR_VOID DestroyCommandBuffer();

	GR_VOID WsiWinQueuePresent();


	GR_PHYSICAL_GPU PhysicalGpu = GR_NULL_HANDLE;
	GR_DEVICE Device = GR_NULL_HANDLE;
	GR_QUEUE Queue = GR_NULL_HANDLE;
	GR_GPU_MEMORY GpuMemory = GR_NULL_HANDLE;
	GR_IMAGE Image = GR_NULL_HANDLE;
	GR_COLOR_TARGET_VIEW ColorTargetView = GR_NULL_HANDLE;
	GR_CMD_BUFFER CmdBuffer = GR_NULL_HANDLE;


	std::vector<const GR_CHAR*> DeviceExtension;
};

