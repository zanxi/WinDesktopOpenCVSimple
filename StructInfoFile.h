#pragma once
#include ".\stdafx.h"

class StructInfoFile
{
public: 
	string path;
	string fileName;
	string fileNameFullPath;
	StructInfoFile()
	{

	}

	void SetNameFile(string path, string fileName)		
	{
		this->path=path;
		this->fileName = fileName;

	}

	void SetNameFile(string path, string fileName, string fileNameFullPath)
	{
		this->path = path;
		this->fileName = fileName;
		this->fileNameFullPath = fileNameFullPath;

	}

	/*StructInfoFile& operator=(const StructInfoFile& rhs)
	{
		StructInfoFile sf;		
		sf.path = rhs.path;
		sf.fileName = rhs.fileName;
		return sf;
	}*/
};