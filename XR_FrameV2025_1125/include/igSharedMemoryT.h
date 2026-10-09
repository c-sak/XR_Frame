/** @class	iglib::igSharedMemoryT
	@brief
	共有メモリを提供します。

	@author Toshimitsu Mukai(toshimi@iltj.jp)

	@version 1.0

	@date    04-June-2007

	@attention
	Copyright (C) 2007, ILTJ Inc.

@verbatim
参考:
	本クラスの提供する共有メモリ領域は、１つのreae/write可能なプロセスと、複数のread onlyのプロセス間で
	利用される事を前提としており、複数のread/write可能なプロセスの存在を許していません。
	また、read/write可能なプロセスが一番初めにマッピングファイルオブジェクトを作成するようにしています。
	本クラス利用者はread/writeを行うプロセスが１つになる様に配慮すれば、どのような順番でプロセスが起動
	しても、read/write可能なプロセスが、一番初めにマッピングファイルオブジェクトを作成するようになっています。
	逆にread onlyのプロセスのみしか起動しないと、いつまでもwait状態となります。
@endverbatim
*/
#ifndef __IG_SHARED_MEMORY_T_H__
#define __IG_SHARED_MEMORY_T_H__

#include <windows.h>
#include <string>
using namespace std;

namespace iglib {	// iglib namespace start here...................................................

template <class T> class igSharedMemoryT {
	string m_mappedFilename;	///< マッピングファイルオブジェクト名(任意に指定が可能。デフォルト値あり)
	string m_mutexName;			///< ミューテック名(固定値)
	HANDLE m_handle;			///< マッピングファイルオブジェクトのハンドル
	bool m_writable;			///< 書込みを許可するか(真で許可)。読み込みは常時可
	T *m_data;					///< 共有領域へのread/write用データ

public:
	igSharedMemoryT();

	/// @brief	仮引数のあるコンストラクタ
	///	@brief	仮引数でiniファイルのパスを指定します。
	///	@param	filename	[in]	共有メモリ識別用のキーワード
	igSharedMemoryT(const string filename);

	~igSharedMemoryT(){};

	///	@brief	共有領域を作成します。または既にある共有領域へのアクセスを可能とします。
	///	@param	writable	[in]	trueでread/writeで共有領域を作成します、falseでread onlyで共有領域を開きます。
	///	@return	共有領域の作成に成功すればtrue、失敗すればfalseが戻ります。
	bool open(const bool writable = false);

	///	@brief	共有領域のデータを読みます。
	///	@param	data	[out]	共有領域内のデータ
	///	@return	readに成功すればtrue、失敗すればfalseが戻ります。
	/// @attention readが可能なのは共有領域をopen(true)で作成した時のみです。
	bool read(T &data)const;

	///	@brief	共有領域へデータを書きます。
	///	@param	data	[in]	共有領域へ書くデータ
	///	@return	writeに成功すればtrue、失敗すればfalseが戻ります。
	bool write(const T &data);

	///	@brief	共有領域を削除します。または利用中の共有領域へのアクセスを閉じます。
	///	@return	共有領域の削除または閉じることにに成功すればtrue、失敗すればfalseが戻ります。
	bool close(void)const;
};

//__________________________________________________________________________________________________
//__constractor_____________________________________________________________________________________
//
template <class T>
igSharedMemoryT<T>::igSharedMemoryT(){
	m_mappedFilename = "ICOM_SHARED_MEMORY_FILE_ID";	// default name
	m_mutexName = "ICOM_SHARED_MEMORY_MUTEX_NAME";
	m_handle = NULL;
	m_writable = false;
}

//__________________________________________________________________________________________________
//__constractor_____________________________________________________________________________________
//
template <class T>
igSharedMemoryT<T>::igSharedMemoryT(const string filename){
	m_mappedFilename = filename;
	m_mutexName = "ICOM_SHARED_MEMORY_MUTEX_NAME";
	m_handle = NULL;
	m_writable = false;
}

//__________________________________________________________________________________________________
//__open____________________________________________________________________________________________
//
template <class T>
bool igSharedMemoryT<T>::open(const bool writable){
	m_writable = writable;
	DWORD fdwProtect = m_writable ? PAGE_READWRITE : PAGE_READONLY;

	// ファイルマップオブジェクトの作成
	for(;;){
		m_handle = CreateFileMappingA(
			HANDLE(-1),
			NULL,
			fdwProtect,
			0,
			sizeof(T),
			m_mappedFilename.c_str()
		);

		// 既にファイルがあるかを確認する
		DWORD fileExists = GetLastError();

		if(m_handle == NULL)return false;

		// 既にread/write可能なプロセスが存在し、再びread/writeが可能な
		// モードでファイルを開こうとした場合にはエラーとなる
		if(m_writable){
			if(fileExists == ERROR_ALREADY_EXISTS){			// 既にread/writeが可能なプロセスがある
				this->close();
				return false;
			}else break;									// 初めてread/writeが可能なプロセスを作成した
		}else{
			if(fileExists == ERROR_ALREADY_EXISTS)break;	// read onlyのファイルを２番目以降に開いた
			else{											// read onlyのファイルを一番初めに作成しようとした
				this->close();
				Sleep(1000);								// 1秒後に再度トライする
				continue;
			}
		}
	}

	// ビューの作成
	DWORD dwDesiredAccess = m_writable ? FILE_MAP_WRITE : FILE_MAP_READ;
	m_data = (T *)MapViewOfFile(m_handle, dwDesiredAccess, 0, 0, sizeof(T));
	if(m_data == NULL)return false;

	return true;
}

//__________________________________________________________________________________________________
//__read____________________________________________________________________________________________
//
template <class T>
bool igSharedMemoryT<T>::read(T &data)const{
	// 排他処理の開始
#ifdef UNICODE
  wchar_t l_mutexName[1024];
  mbstowcs(l_mutexName, m_mutexName.c_str(), 1023);
  HANDLE hMutex = CreateMutex(NULL, FALSE, l_mutexName);
#else
	HANDLE hMutex = CreateMutex(NULL, FALSE, m_mutexName.c_str());
#endif

	WaitForSingleObject(hMutex, INFINITE);

	data = *m_data;

	// 排他処理の解除 
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);

	return true;
}

//__________________________________________________________________________________________________
//__write___________________________________________________________________________________________
//
template <class T>
bool igSharedMemoryT<T>::write(const T &data){
	// 排他処理の開始
#ifdef UNICODE
  wchar_t l_mutexName[1024];
  mbstowcs(l_mutexName, m_mutexName.c_str(), 1023);
  HANDLE hMutex = CreateMutex(NULL, FALSE, l_mutexName);
#else
	HANDLE hMutex = CreateMutex(NULL, FALSE, m_mutexName.c_str());
#endif

	WaitForSingleObject(hMutex, INFINITE);

	*m_data = data;

	// 排他処理の解除
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);

	return true;
}

//__________________________________________________________________________________________________
//__close___________________________________________________________________________________________
//
template <class T>
bool igSharedMemoryT<T>::close(void)const{
	if(m_data != NULL)UnmapViewOfFile(m_data);
	if(m_handle != NULL)CloseHandle(m_handle);

	return true;
}

}	// end of iglib namespace.......................................................................

#endif
