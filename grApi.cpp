#include "grApi.h"


grApi::grApi()
{
	CreateDevice();
	CreatePresentableImage();
	ConstructWindow();
	CreateColorTargetView();
	CreateCommandBuffer();
	WsiWinQueuePresent();
}


grApi::~grApi()
{
	DestroyCommandBuffer();
	DestroyColorTargetView();
	DestroyPresentableImage();
	DestroyDevice();
}


GR_VOID grApi::CreateDevice()
{
	GR_APPLICATION_INFO ApplicationInfo{};
	ApplicationInfo.apiVersion = GR_API_VERSION;
	ApplicationInfo.appVersion = GR_MAKE_VERSION(1, 0, 1);
	ApplicationInfo.pAppName = "grProjectGame";
	ApplicationInfo.engineVersion = GR_MAKE_VERSION(1, 0, 0);
	ApplicationInfo.pEngineName = "GameEngineSDK";

	GR_UINT GpuCount = 0;
	grInitAndEnumerateGpus(&ApplicationInfo, nullptr, &GpuCount, &PhysicalGpu);
	if (!PhysicalGpu) {
		MessageBox(nullptr, TEXT("PhysicalGpu object is not created!"), TEXT("Error"), MB_ICONERROR);
		std::exit(-1);
	}
	PhysicalGpu = GR_NULL_HANDLE;
	std::vector<GR_PHYSICAL_GPU> PhysicalGpus(GpuCount);
	grInitAndEnumerateGpus(&ApplicationInfo, nullptr, &GpuCount, PhysicalGpus.data());
	if (PhysicalGpus[0]) PhysicalGpu = PhysicalGpus[0];
	else PhysicalGpu = PhysicalGpus[1];

	GR_DEVICE_QUEUE_CREATE_INFO DeviceQueueCreateInfo{};
	DeviceQueueCreateInfo.queueCount = 1;
	DeviceQueueCreateInfo.queueType = GR_QUEUE_UNIVERSAL;

	GR_DEVICE_CREATE_INFO DeviceCreateInfo{};
	DeviceExtension.push_back(GR_WSI_WINDOWS_EXTENSION_NAME);
	DeviceCreateInfo.queueRecordCount = 1;
	DeviceCreateInfo.pRequestedQueues = &DeviceQueueCreateInfo;
	DeviceCreateInfo.extensionCount = static_cast<GR_UINT>(DeviceExtension.size());
	DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtension.data();
	DeviceCreateInfo.maxValidationLevel = GR_VALIDATION_LEVEL_0;
	DeviceCreateInfo.flags = GR_DEVICE_CREATE_VALIDATION;

	grCreateDevice(PhysicalGpu, &DeviceCreateInfo, &Device);
	if (!Device) {
		MessageBox(nullptr, TEXT("Device object is not created!"), TEXT("Error"), MB_ICONERROR);
		std::exit(-1);
	}

	grGetDeviceQueue(Device, GR_QUEUE_UNIVERSAL, 0, &Queue);
	if (!Queue) {
		MessageBox(nullptr, TEXT("Queue object is not created!"), TEXT("Error"), MB_ICONERROR);
		std::exit(-1);
	}
}


GR_VOID grApi::DestroyDevice() {
	if (grDestroyDevice(Device) != GR_SUCCESS) {
		MessageBox(nullptr, TEXT("Device object is not destroyed!"), TEXT("Error"), MB_ICONERROR);
		std::exit(-1);
	}
}


GR_VOID grApi::CreatePresentableImage()
{
	GR_WSI_WIN_PRESENTABLE_IMAGE_CREATE_INFO WsiWinPresentableImageCreateInfo{};
	WsiWinPresentableImageCreateInfo.format.channelFormat = GR_CH_FMT_R8G8B8A8;
	WsiWinPresentableImageCreateInfo.format.numericFormat = GR_NUM_FMT_UNORM;
	WsiWinPresentableImageCreateInfo.usage = GR_IMAGE_USAGE_COLOR_TARGET;
	WsiWinPresentableImageCreateInfo.extent.width = 1280;
	WsiWinPresentableImageCreateInfo.extent.height = 720;

	grWsiWinCreatePresentableImage(Device, &WsiWinPresentableImageCreateInfo, &Image, &GpuMemory);
	if (!Image && !GpuMemory) {
		MessageBox(nullptr, TEXT("PresentableImage object is not created!"), TEXT("Error"), MB_ICONERROR);
		std::exit(-1);
	}
}


GR_VOID grApi::DestroyPresentableImage() {
	// The grFreeMemory(GpuMemory) function must not be called on this memory object;
	// it is implicitly destroyed when the presentable image is destroyed.
	if (grDestroyObject(Image) != GR_SUCCESS) {
		MessageBox(nullptr, TEXT("Image object is not destroyed!"), TEXT("Error"), MB_ICONERROR);
		std::exit(-1);
	}
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}


GR_VOID grApi::ConstructWindow()
{
	HINSTANCE hInstance = GetModuleHandle(nullptr);

	WNDCLASS wc{};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = TEXT("MainWindowClass");

	RegisterClass(&wc);

	hWnd = CreateWindow(wc.lpszClassName, TEXT("grProjectGame"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hWnd, SW_SHOW);
}


GR_VOID grApi::CreateColorTargetView()
{
	GR_COLOR_TARGET_VIEW_CREATE_INFO ColorTargetViewCreateInfo{};
	ColorTargetViewCreateInfo.image = Image;
	ColorTargetViewCreateInfo.arraySize = 1;
	ColorTargetViewCreateInfo.baseArraySlice = 0;
	ColorTargetViewCreateInfo.mipLevel = 0;
	ColorTargetViewCreateInfo.format.channelFormat = GR_CH_FMT_R8G8B8A8;
	ColorTargetViewCreateInfo.format.numericFormat = GR_NUM_FMT_UNORM;

	grCreateColorTargetView(Device, &ColorTargetViewCreateInfo, &ColorTargetView);
	if (!ColorTargetView) {
		MessageBox(nullptr, TEXT("ColorTargetView object is not created!"), TEXT("Error"), MB_ICONERROR);
		std::exit(-1);
	}
}


GR_VOID grApi::DestroyColorTargetView() {
	if (grDestroyObject(ColorTargetView) != GR_SUCCESS) {
		MessageBox(nullptr, TEXT("ColorTargetView object is not destroyed!"), TEXT("Error"), MB_ICONERROR);
		std::exit(-1);
	}
}


GR_VOID grApi::CreateCommandBuffer()
{
	GR_CMD_BUFFER_CREATE_INFO CmdBufferCreateInfo{};
	CmdBufferCreateInfo.queueType = GR_QUEUE_UNIVERSAL;

	grCreateCommandBuffer(Device, &CmdBufferCreateInfo, &CmdBuffer);
	if (!CmdBuffer) {
		MessageBox(nullptr, TEXT("CmdBuffer object is not created!"), TEXT("Error"), MB_ICONERROR);
		std::exit(-1);
	}

	grBeginCommandBuffer(CmdBuffer, (GR_CMD_BUFFER_BUILD_FLAGS)0);
	{
		GR_IMAGE_SUBRESOURCE_RANGE ImageSubsourceRange{};
		ImageSubsourceRange.aspect = GR_IMAGE_ASPECT_COLOR;
		ImageSubsourceRange.baseMipLevel = 0;
		ImageSubsourceRange.mipLevels = 1;
		ImageSubsourceRange.baseArraySlice = 0;
		ImageSubsourceRange.arraySize = 1;

		GR_IMAGE_STATE_TRANSITION ImageStateTransition{};
		ImageStateTransition.image = Image;
		ImageStateTransition.oldState = GR_IMAGE_STATE_TARGET_RENDER_ACCESS_OPTIMAL;
		ImageStateTransition.newState = GR_IMAGE_STATE_CLEAR;
		ImageStateTransition.subresourceRange = ImageSubsourceRange;

		grCmdPrepareImages(CmdBuffer, 1, &ImageStateTransition);

		float Color[] = { 0.4f, 0.6f, 0.9f, 1.0f };

		grCmdClearColorImage(CmdBuffer, Image, Color, 1, &ImageSubsourceRange);

		ImageStateTransition.image = Image;
		ImageStateTransition.oldState = GR_IMAGE_STATE_CLEAR;
		ImageStateTransition.newState = GR_IMAGE_STATE_TARGET_RENDER_ACCESS_OPTIMAL;
		ImageStateTransition.subresourceRange = ImageSubsourceRange;

		grCmdPrepareImages(CmdBuffer, 1, &ImageStateTransition);
	}
	grEndCommandBuffer(CmdBuffer);
}


GR_VOID grApi::DestroyCommandBuffer() {
	if (grDestroyObject(CmdBuffer) != GR_SUCCESS) {
		MessageBox(nullptr, TEXT("CmdBuffer object is not destroyed!"), TEXT("Error"), MB_ICONERROR);
		std::exit(-1);
	}
}


GR_VOID grApi::WsiWinQueuePresent()
{
	GR_MEMORY_REF MemoryRef{};
	MemoryRef.mem = GpuMemory;

	grQueueSubmit(Queue, 1, &CmdBuffer, 1, &MemoryRef, GR_NULL_HANDLE);

	GR_WSI_WIN_PRESENT_INFO WsiWinPresentInfo{};
	WsiWinPresentInfo.hWndDest = hWnd;
	WsiWinPresentInfo.srcImage = Image;
	WsiWinPresentInfo.presentMode = GR_WSI_WIN_PRESENT_MODE_WINDOWED;

	grWsiWinQueuePresent(Queue, &WsiWinPresentInfo);
}
