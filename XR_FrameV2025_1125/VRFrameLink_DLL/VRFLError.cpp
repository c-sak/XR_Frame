#include "pch.h"
#include "VRFLError.h"

#include <sstream>
#include <iomanip>


namespace VRFL
{
	SyncInterlock VRFLError::_outlock;
	std::shared_ptr<std::ostream> VRFLError::_outstream;

	// ‰B•Á
	VRFLError::VRFLError() {}
	VRFLError::~VRFLError() {}

	void VRFLError::SetOutputStream(std::shared_ptr<std::ostream> outstream)
	{
		SyncKeeper sk(&_outlock);

		sk.Lock();

		_outstream = outstream;
	
		sk.UnLock();
	}

	void VRFLError::ErrorMessage(const char* msg)
	{
		std::string fmsg = formatMsg(msg);
		SyncKeeper sk(&_outlock);

		sk.Lock();

		if (_outstream)
		{
			*_outstream << fmsg << std::endl;
		}
		else
			std::cout << fmsg << std::endl;

		sk.UnLock();
	}

	//void VRFLError::ErrorMessage(std::string msg)
	//{
	//	ErrorMessage(msg.c_str());
	//}

	std::string VRFLError::formatMsg(const char* msg)
	{
		SYSTEMTIME ltime;

		std::stringstream sstr;

		GetLocalTime(&ltime);

		sstr << "[" <<
			std::setw(4) << ltime.wYear << "-" <<
			std::setw(2) << ltime.wMonth << "-" <<
			ltime.wDay << " " <<
			ltime.wHour << ":" <<
			ltime.wMinute << ":" <<
			ltime.wSecond << "]";

		std::string fmsg(sstr.str().c_str());
		
		return fmsg;
	}
}