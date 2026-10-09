#include "pch.h"
#include "SendThread.h"
#include <WinSock2.h>
#include <ws2ipdef.h>
#include "calc.h"
#include "VRFLInterface.h"
#include <vector>
#include "VRFLTool.h"

#include "gmath/Vector3.hpp"
#include "gmath/Quaternion.hpp"


namespace VRFL
{
	SendThread::SendThread()
	{
		m_timeOut = 1000;
	}

	SendThread::~SendThread()
	{
	}

	void SendThread::Initialize(unsigned short remoteport)
	{
		m_socket = socket(AF_INET, SOCK_DGRAM, 0);

		m_sendEvent = CreateEvent(NULL, false, false, NULL);

		memset(&m_addr, 0, sizeof(m_addr));
		m_addr.sin_family = AF_INET;
		m_addr.sin_port = htons(remoteport);
		inet_pton(AF_INET, "127.0.0.1", (void*)&m_addr.sin_addr.s_addr);
	}

	void SendThread::Send(ObjDataT** objects, int length)
	{
		std::vector<ObjectData>* datas = new std::vector<ObjectData>();

		for (int i = 0; i < length; i++)
		{
			ObjectData od;
			ObjDataT* parent = objects[i];
			std::vector<ObjDataT*> hierarchy;

			od.id = objects[i]->id;
			od.state = objects[i]->state;
			od.visible = objects[i]->visible ? 1 : 0;
			
			od.col[0] = objects[i]->color.red;
			od.col[1] = objects[i]->color.green;
			od.col[2] = objects[i]->color.blue;
			od.col[3] = objects[i]->color.alpha;

			hierarchy.push_back(parent);
			while (parent->base != NULL)
			{
				parent = parent->base;
				hierarchy.push_back(parent);
			}

			Vector3 pos;
			Quaternion rot;

			std::vector<ObjDataT*>::reverse_iterator ritr = hierarchy.rbegin(), eitr = hierarchy.rend();

			for (; ritr != eitr; ritr++)
			{
				ObjDataT* obj = *ritr;

				Vector3 tpos = Vector3(obj->pos.x, obj->pos.y, obj->pos.z);
				Quaternion trot;
				trot = Quaternion::FromEuler(
					DegToRad(obj->rot.pitch),
					DegToRad(obj->rot.yaw),
					DegToRad(obj->rot.roll));

				pos = pos + rot * tpos;
				rot = rot * trot;
			}

			Quaternion offrot;
			offrot = Quaternion::FromEuler(
				DegToRad(objects[i]->offrot.pitch),
				DegToRad(objects[i]->offrot.yaw),
				DegToRad(objects[i]->offrot.roll));

			rot = rot * offrot;

			od.pos[0] = (float)pos.X;
			od.pos[1] = (float)pos.Y;
			od.pos[2] = (float)pos.Z;
			od.rot[0] = (float)rot.X;
			od.rot[1] = (float)rot.Y;
			od.rot[2] = (float)rot.Z;
			od.rot[3] = (float)rot.W;

			datas->push_back(od);
		}

		addData(datas);

		SetEvent(m_sendEvent); 
	}

	//void SendThread::makeTransData(std::vector<ObjectData>* datas, ObjDataT* src, Vector3 offpos, Quaternion offrot)
	//{
	//	ObjectData dst;

	//	dst.id = src->id;
	//	dst.state = src->state;
	//	dst.visible = src->visible;

	//	Vector3 pos = Vector3(src->pos.x, src->pos.y, src->pos.z);
	//	Quaternion rot = Quaternion::FromEuler(src->rot.pitch, src->rot.yaw, src->rot.roll); // Roll, Pitch, Yaw order rotation
	//	
	//	rot = rot * offrot;
	//	pos = offpos + offrot * pos;

	//	dst.pos[0] = pos.X;
	//	dst.pos[1] = pos.Y;
	//	dst.pos[2] = pos.Z;
	//	dst.rot[0] = rot.X;
	//	dst.rot[1] = rot.Y;
	//	dst.rot[2] = rot.Z;
	//	dst.rot[3] = rot.W;

	//	datas->push_back(dst);
	//}

	void SendThread::addData(std::vector<ObjectData>* data)
	{
		SyncKeeper sk(&m_sendLock);

		sk.Lock();
		{
			m_sendList.push_back(data);
		}
		sk.UnLock();
	}

	std::vector<ObjectData>* SendThread::popData()
	{
		SyncKeeper sk(&m_sendLock);
		std::vector<ObjectData>* res;

		sk.Lock();
		{
			if (m_sendList.size() == 0) res = NULL;
			else
			{
				res = m_sendList.front();
				m_sendList.pop_front();
			}
		}
		sk.UnLock();

		return res;
	}

	bool SendThread::ThreadProc()
	{
		int res;
		//struct sockaddr_in remote;

		if (m_senddata == NULL) m_senddata = popData();

		if (m_senddata == NULL)
		{
			WaitForSingleObject(m_sendEvent, m_timeOut);
		}
		else
		{
			res = sendto(m_socket, (const char*)&(*m_senddata)[0], (int)m_senddata->size() * sizeof(ObjectData), 0, (struct sockaddr *)&m_addr, sizeof(m_addr));

			if (res == SOCKET_ERROR) {
				int err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK && err != WSAENOBUFS)
				{
					free(m_senddata);
					m_senddata = NULL;

					return false;  // fatal error
				}

				Sleep(0); // WSAEWOULDBLOCK, WSAENOBUFS âè¡ë“Çø
			}
			else
			{
				free(m_senddata);
				m_senddata = NULL;
			}
		}

		return true;
	}
}