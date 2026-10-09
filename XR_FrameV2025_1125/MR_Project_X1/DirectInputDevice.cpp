/*
* Copyright Canon Inc. 2018
* All Rights Reserved
*/
#include "stdafx.h"

#include "DirectInputDevice.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace
{
	const std::string HMD_INPUT_DEVICE_NAME_LIST[] =
	{
		"Canon MREAL X1 Button", // MREAL X1 Buttonのボタンデバイス名
		"Canon MREAL S1 Button", // MREAL S1 Buttonのボタンデバイス名
		"Canon MREAL Display Button MD-20 Series", // MREAL Display MD-20 Buttonのボタンデバイス名
		"Canon MREAL Display Button MD-10 Series", // MREAL Display MD-10 Buttonのボタンデバイス名
	};
}

/**
* @brief デバイス検出メソッドのコールバック用構造体。
*/
struct DIDeviceEnumPrm
{
	char devName[256]; ///< 検索対象のデバイス名
	BOOL bFound;	///< 見つかったかどうかのフラグ
	GUID guid;		///< 見つかったデバイスのGUID
};

DirectInputDevice::DirectInputDevice(HWND hwnd)
	: window_handle(hwnd)
	, joystick(nullptr)
	, direct_input(nullptr)
{
	/// DirectInputオブジェクトの生成
	DirectInput8Create(
		::GetModuleHandle(nullptr),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void **)&direct_input,
		nullptr);
}

DirectInputDevice::~DirectInputDevice(void)
{
	Stop();
	Shutdown();
}

bool DirectInputDevice::Initialize()
{
	if (direct_input == nullptr)
	{
		return false;
	}

	DIDeviceEnumPrm prm;
	prm.bFound = false;

	/// MREAL Displayボタンデバイスを列挙して見つかったらGUIDを取得
	direct_input->EnumDevices(
		DI8DEVCLASS_GAMECTRL,
		EnumJoysticksCallback,
		(LPVOID)&prm,
		DIEDFL_ATTACHEDONLY);

	// MREAL Displaボタンが見つからない場合
	if (!prm.bFound)
	{
		return false;
	}

	Shutdown();

	/// MRAEL Displayボタンのインスタンス作成
	HRESULT hr = direct_input->CreateDevice(prm.guid, &joystick, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool DirectInputDevice::Connect()
{
	//  MREAL Displaボタンの初期化
	if (joystick == nullptr)
	{
		return false;
	}

	//  MREAL Displaボタンのデータ形式を設定
	HRESULT hr = joystick->SetDataFormat(&c_dfDIJoystick);

	//  MREAL Displaボタンの動作の設定
	if (SUCCEEDED(hr))
	{
		/// - フォアグラウンド＆非排他モード
		// アプリがフォアグラウンドの場合に入力を受け付け、入力は独占しない
		hr = joystick->SetCooperativeLevel(window_handle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	}

	// 入力制御開始
	if (SUCCEEDED(hr))
	{
		hr = joystick->Acquire();
	}

	// ここまでの処理で失敗した場合は、後始末をして、FALSEを返す
	if (FAILED(hr))
	{
		Stop();
		Shutdown();
		return false;
	}
	return true;
}

BOOL CALLBACK DirectInputDevice::EnumJoysticksCallback(LPCDIDEVICEINSTANCE ipddi, LPVOID pvRef)
{
	if (ipddi == nullptr || pvRef == nullptr)
	{
		return DIENUM_CONTINUE;
	}

	// MREAL Display ボタンの場合
	DIDeviceEnumPrm *prm = static_cast<DIDeviceEnumPrm *>(pvRef);
	for (const std::string& devName : HMD_INPUT_DEVICE_NAME_LIST)
	{
		if (strcmp(devName.c_str(), ipddi->tszProductName) == 0)
		{
			strcpy_s(prm->devName, devName.c_str());
			prm->guid = ipddi->guidInstance;
			prm->bFound = TRUE;
			return DIENUM_STOP;
		}
	}

	// Direct Inputデバイスの検索を継続
	return DIENUM_CONTINUE;
}

void DirectInputDevice::Stop()
{
	if (joystick)
	{
		joystick->Unacquire();
	}
}

void DirectInputDevice::Shutdown()
{
	if (joystick)
	{
		joystick->Release();
		joystick = nullptr;
	}
}

HRESULT DirectInputDevice::GetJoysticState(DIJOYSTATE * joystick_state)
{
	HRESULT hr = E_FAIL;

	if (joystick == nullptr) {
		return S_FALSE;
	}
	hr = joystick->Poll();
	if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
		if (FAILED(joystick->Acquire())) {
			return E_FAIL;
		}
		hr = joystick->Poll();
	}

	switch (hr) {
	case DIERR_INPUTLOST:
	case DIERR_NOTACQUIRED:
		return S_FALSE;
	case S_OK:
	case DI_NOEFFECT:
		break;
	default:
		Stop();
		Shutdown();
		return hr;
	};

	// MRAEL Displayのボタン状態を取得する。
	hr = joystick->GetDeviceState(sizeof(DIJOYSTATE), joystick_state);
	return hr;
}
