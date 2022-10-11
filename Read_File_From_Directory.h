#pragma once
#include "stdafx.h"
#include ".\StructInfoFile.h"

wstring string_to_wstring(string& s)
{
	ofstream ofs("temp.txt", ofstream::out);
	if (!ofs) { cerr << "don't open file: 'temp.txt1' " << endl; exit(1); }
	ofs << s;
	ofs.close();

	wstring s1;
	wifstream wifs("temp.txt", wifstream::in);
	if (!wifs) { cerr << "don't open file: 'temp.txt2'" << endl; exit(1); }
	wifs >> s1;
	wifs.close();
	return s1;
}

string wstring_to_string(wstring s)
{
	wofstream wofs("temp.txt", wofstream::out);
	if (!wofs) { cerr << "don't open file: 'temp.txt3'" << endl; exit(1); }
	wofs << s;
	wofs.close();

	string s1;
	ifstream ifs("temp.txt", ifstream::in);
	if (!ifs) { cerr << "don't open file: 'temp.txt3'" << endl; exit(1); }
	ifs >> s1;
	ifs.close();
	return s1;
}

void ResetFile(string pathFile)
{
	ofstream rewrite(pathFile);
	rewrite.write("", 0);
	rewrite.close();
}

//boost::lockfree::spsc_queue<StructInfoFile> scandir(string p)
vector<StructInfoFile> scandir(string p, string addPref)
{
	vector<StructInfoFile> v;
	//string path = p + addPref;
	//wstring Path = string_to_wstring(path);

	//WIN32_FIND_DATA FindFileData;
	//HANDLE hf;
	//hf = FindFirstFile((string_to_wstring(path)).c_str(), &FindFileData);
	//vector<StructInfoFile> v;
	//if (hf != INVALID_HANDLE_VALUE)
	//{
	//	do
	//	{
	//		//wcout << "FindFileData.dwFileAttributes= " << FindFileData.dwFileAttributes << endl;
	//		if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
	//		{				
	//			StructInfoFile sf;
	//			sf.SetNameFile(p, wstring_to_string(FindFileData.cFileName));
	//			v.push_back(sf);
	//		}
	//	} while (FindNextFile(hf, &FindFileData) != 0);
	//}

	return v;
}

