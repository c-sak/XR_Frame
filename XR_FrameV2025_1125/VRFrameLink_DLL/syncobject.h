#pragma once

#include "framework.h"

/// 同期用オブジェクト
class SyncObject
{
public:
	SyncObject() {}
	virtual ~SyncObject() {}
	
	virtual void Lock() = 0;	///< ロック取得
	virtual bool TryLock() = 0; ///< ロック取得のトライ
	virtual void UnLock() = 0;  ///< 取得したロックのリリース
};

/// Interlock関数で作成した同期用オブジェクト
class SyncInterlock : public SyncObject
{
protected:
	long m_lock;

public:
	SyncInterlock() : m_lock(0) {}
	virtual ~SyncInterlock() {}

	virtual void Lock(){
		while (InterlockedExchange(&m_lock, 1)) { Sleep(0); }
	}
	virtual bool TryLock() {
		return InterlockedExchange(&m_lock, 1) ? false : true;
	}
	virtual void UnLock() {
		InterlockedExchange(&m_lock, 0);
	}
};

/// 同期用オブジェクト(SyncObject)のLockをスコープ内に限定する為のクラス
class SyncKeeper
{
private:
	bool	m_hasLock;
	SyncObject* m_pSyncObject;

public:
	SyncKeeper() : m_pSyncObject(NULL), m_hasLock(false) {}
	SyncKeeper(SyncObject* sync) : m_pSyncObject(sync), m_hasLock(false) {}
	~SyncKeeper() {
		if (m_pSyncObject && m_hasLock)
		{
			m_pSyncObject->UnLock();
		}
		m_pSyncObject = NULL;
		m_hasLock = false;
	}

	void SetSyncObject(SyncObject* sync) { UnLock(); m_pSyncObject = sync; }
	void Lock() {
		if (m_pSyncObject && !m_hasLock) {
			m_pSyncObject->Lock();
			m_hasLock = true;
		}
	}
	bool TryLock() {
		if (m_pSyncObject && !m_hasLock) {
			m_hasLock = m_pSyncObject->TryLock();
			return m_hasLock;
		}
		return false;
	}
	void UnLock() {
		if (m_pSyncObject && m_hasLock) m_pSyncObject->UnLock();
		m_hasLock = false;
	}
};

