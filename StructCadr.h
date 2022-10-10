#pragma once

#include "stdwin.h"

struct shapes_data
{
	std::string label;
	std::string group_id;
	std::string shape_type;
	std::map<string, string> flags;
	std::vector<string> pionts;
};

struct StructCadr
{
public:
	string version;
	vector<shapes_data> shapes;

	string imagePath;
	string imageData;
	string imageWidth;
	string imageHeight;
		
};