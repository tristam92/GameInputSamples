#include <GameInput.h>
#include <wrl.h>
#include <iostream>

using namespace Microsoft::WRL;
using namespace GameInput::v1;

ComPtr<IGameInput> pGameInput;
GameInputCallbackToken callbackToken;

void CALLBACK OnGameInputDeviceStatus(
	GameInputCallbackToken token,
	void* context,
	IGameInputDevice* device,
	unsigned long long /*timestamp*/,
	GameInputDeviceStatus status,
	GameInputDeviceStatus prevStatus
)
{
	const GameInputDeviceInfo* deviceInfo = nullptr;
	device->GetDeviceInfo(&deviceInfo);
	if (!deviceInfo)
	{
		std::cerr << "Failed to retrieve connected device info." << std::endl;
	}


	std::cout << "Device status : ";

	if (status & GameInputDeviceStatus::GameInputDeviceConnected)
	{
		std::cout << "Connected";
	}
	else
	{
		std::cout << "Disconnected";
	}

	std::cout << " VendorId: " << std::hex << deviceInfo->vendorId << std::dec;

	switch (deviceInfo->deviceFamily)
	{
	case GameInputDeviceFamily::GameInputFamilyVirtual:		std::cout << " Device Family: GameInputFamilyVirtual";  break;
	case GameInputDeviceFamily::GameInputFamilyAggregate:	std::cout << " Device Family: GameInputFamilyAggregate";break;  
	case GameInputDeviceFamily::GameInputFamilyXboxOne:		std::cout << " Device Family: GameInputFamilyXboxOne";  break;
	case GameInputDeviceFamily::GameInputFamilyXbox360:		std::cout << " Device Family: GameInputFamilyXbox360";  break;
	case GameInputDeviceFamily::GameInputFamilyHid:			std::cout << " Device Family: GameInputFamilyHid";  	break;
	case GameInputDeviceFamily::GameInputFamilyI8042:		std::cout << " Device Family: GameInputFamilyI8042";	break;
	}

	std::cout << ";" << std::endl;
}

HRESULT InitGameInputInterface()
{
	HRESULT res = GameInputCreate(pGameInput.GetAddressOf());
	if (SUCCEEDED(res))
	{
		res = pGameInput->RegisterDeviceCallback(nullptr,
			GameInputKind::GameInputKindGamepad,
			GameInputDeviceStatus::GameInputDeviceConnected,
			GameInputEnumerationKind::GameInputNoEnumeration,
			nullptr,
			OnGameInputDeviceStatus,
			&callbackToken
		);

		if (FAILED(res))
		{
			std::cerr << "Failed to set DeviceStatus callback for GameInput interface. Result : " << res << std::endl;
		}
	}
	else
	{
		std::cerr << "Failed to create GameInput interface. Result : " << res << std::endl;
	}

	return res;
}

void ReleaseGameInputResources()
{
	if (pGameInput && callbackToken != 0)
	{
		pGameInput->UnregisterCallback(callbackToken);
	}

	pGameInput.Reset();
}

int main()
{
	bool breakLoop = false;
	HRESULT res = InitGameInputInterface();
	while (!breakLoop && SUCCEEDED(res))
	{
		Sleep(1000);
	}
	ReleaseGameInputResources();
	return res;
}