/*
* Copyright Canon Inc. 2018
* All Rights Reserved
*/
#pragma once

#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h>
#include <Windows.h>
#include <string>

/**
 * @brief MREAL Displayボタン用デバイスインプットクラス
 */
class DirectInputDevice
{
private:
	HWND window_handle;

	//DirectInputオブジェクト
	LPDIRECTINPUT8			direct_input;
	LPDIRECTINPUTDEVICE8	joystick;

	// ボタン監視スレッド
	HANDLE update_event;

public:
	/**
	 * @brief コンストラクタ。
	 * @param hWnd ウィンドウハンドル
	 */
	explicit DirectInputDevice(HWND hWnd);

	/**
	 * @brief デストラクタ。
	 */
	~DirectInputDevice(void);

	/**
	 * @brief デバイスを初期化する。
	 * @retval true 成功。
	 * @retval false 失敗。
	 */
	bool Initialize();

	/**
	 * @brief デバイスへの接続を実行する。
	 * @retval true 成功。
	 * @retval false 失敗。
	 */
	bool Connect();

	/**
	* @brief MREAL Displayのボタン状態を取得する。
	* @param[out] joystick_state ボタン状態
	* @return 取得の成否。
	*/
	HRESULT GetJoysticState(DIJOYSTATE * joystick_state);
private:
	static BOOL CALLBACK EnumJoysticksCallback(LPCDIDEVICEINSTANCE ipddi, LPVOID pvRef);

	void Shutdown();
	void Stop();
};
