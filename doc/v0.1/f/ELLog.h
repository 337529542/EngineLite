#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
using namespace std;


class ELLog
{
public:
	static ELLog* getInstancePtr();
	static ELLog& getInstance();
	static void createInstance();
	static void deleteInstance();
	static ELLog* m_Instance;

private:
	ELLog();


public:
	~ELLog();
	bool Open(string sFileName);
	void Close();
	bool CommonLogInit(); //open default log file
	void Enable();
	void Disable();
	string GetTimeStr();
	template <typename T> void LogOut(const T& value)
	{
		if(m_bEnabled)
		{
			m_tOLogFile<<value;
		}
	}
	template <typename T> void LogOutLn(const T& value)
	{
		if(m_bEnabled)
		{
			m_tOLogFile<<value<<endl;
		}
	}
	void LogOutLn()
	{
		if(m_bEnabled)
		{
			m_tOLogFile<<endl;
		}
	}
	template <typename T> ELLog& operator<<(const T& value)
	{
		if(m_bEnabled)
		{
			//m_tOLogFile<<GetTimeStr()<<value<<endl;
			m_tOLogFile<<value;
		}
		return (*this);
	}
	ELLog& operator<<(ostream& (*_Pfn) (ostream&))
	{
		if(m_bEnabled)
		{
			(*_Pfn)(m_tOLogFile);
		}
		return (*this);
	}

private:
	template <typename T> string ValueToStr(T value)
	{
		ostringstream ost;
		ost<<value;
		return ost.str();
	}

private:
	ofstream m_tOLogFile;
	bool m_bEnabled;
};
