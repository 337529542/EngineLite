#include "ELLog.h"


ELLog* ELLog::m_Instance = NULL;

ELLog::ELLog()
:m_bEnabled(true)
{
}
ELLog::~ELLog()
{
}
bool ELLog::Open(string sFileName)
{
	m_tOLogFile.open(sFileName.c_str(), ios_base::out | ios_base::trunc);
	if( !m_tOLogFile )
		{
			return false;
	}
	return true;
}
void ELLog::Close()
{
	if(m_tOLogFile.is_open())
		{
			m_tOLogFile.close();
	}
}
bool ELLog::CommonLogInit()
{
	//time_t tNowTime;
	//time(&tNowTime);
	//tm* tLocalTime = localtime(&tNowTime);
	////
	//string sDateStr = ValueToStr(tLocalTime->tm_year+1900) + "-" +
	//	ValueToStr(tLocalTime->tm_mon+1) + "-" +
	//	ValueToStr(tLocalTime->tm_mday);
	//return Open("EngineLite_" + sDateStr + ".log");
	return Open("EngineLite.log");
}
void ELLog::Enable()
{
	m_bEnabled = true;
}
void ELLog::Disable()
{
	m_bEnabled = false;
}
//
string ELLog::GetTimeStr()
{
	time_t tNowTime;
	time(&tNowTime);
	tm* tLocalTime = localtime(&tNowTime);
	//
	string strDateTime = ValueToStr(tLocalTime->tm_year+1900) + "-" +
		ValueToStr(tLocalTime->tm_mon+1)     + "-" +
		ValueToStr(tLocalTime->tm_mday)      + " " +
		ValueToStr(tLocalTime->tm_hour)      + ":" +
		ValueToStr(tLocalTime->tm_min)       + ":" +
		ValueToStr(tLocalTime->tm_sec)       + " ";
	return strDateTime;
}

ELLog* ELLog::getInstancePtr()
{
	return m_Instance;
}

ELLog& ELLog::getInstance()
{
	return *m_Instance;
}
void ELLog::createInstance()
{
	m_Instance = new ELLog;
}

void ELLog::deleteInstance()
{
	delete m_Instance;
}