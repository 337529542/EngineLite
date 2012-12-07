#include "ELRoot.h"


ELRoot::ELRoot()
{
	ELLog::createInstance();
	ELLog::getInstance().CommonLogInit();
	ELLog::getInstance()<<"EngineLite Start"<<endl;
}

ELRoot::~ELRoot()
{
	ELLog::getInstance()<<"EngineLite ShutDown"<<endl;
	ELLog::deleteInstance();
}