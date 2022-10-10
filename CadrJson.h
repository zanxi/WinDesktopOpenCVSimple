#pragma once

#include "stdafx.h"
#include "StructInfoFile.h"
#include "Read_File_From_Directory.h"

void print_(boost::property_tree::ptree const& pt)
{
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        std::cout << it->first << ": " << it->second.get_value<std::string>() << std::endl;
        print_(it->second);
    }
}

void print(boost::property_tree::ptree const& pt)
{
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
        string first = it->first;
        string second = it->second.get_value<std::string>();

        //std::cout << "first: " << first << ": --- second " << second << std::endl;

        if (first == "version")
        {
            string value = it->second.get_value<std::string>();
            std::cout << "version value: " << value << std::endl;
            //string service_list.push_back(it->second.get_value<std::string>());
        }

        if (first == "flags")
        {
            string value = it->second.get_value<std::string>();
            std::cout << "flags value: " << value << std::endl;
            //string service_list.push_back(it->second.get_value<std::string>());
        }

        if (first == "shapes")
        {
            string value = it->second.get_value<std::string>();
            std::cout << "shapes value: " << value << std::endl;
            //string service_list.push_back(it->second.get_value<std::string>());
        }

        /*int id = strTree.get<int>("shapes");
        ptree names = strTree.get_child("t");


        if (first == "shapes")
        {
            boost::property_tree::ptree pt2 = it->second;
            ptree::const_iterator end = pt2.end();
            for (ptree::const_iterator it2 = pt2.begin(); it2 != end; ++it2) {
                string first2 = it2->first;
                string second2 = it2->second.get_value<std::string>();
                std::cout << "\n\nfirst2: " << first2 << ": --- second2 " << second2 << std::endl;
            }

        }*/



        //print(it->second);
    }
}



using boost::property_tree::ptree;

struct service {
    std::string version;
    std::string flags;
    //std::string label;

    struct shapes_t {
        std::vector<std::vector<string>> cvPoints;
        std::string label;
        std::string group_id;
        std::string shape_type;
        std::string flags;
    };
    std::vector<shapes_t> shapes;

    std::string imagePath;
    std::string imageData;
    std::string imageHeight;
    std::string imageWidth;
};

namespace bmi = boost::multi_index;

using Table = bmi::multi_index_container<
    service,
    bmi::indexed_by<
    bmi::ordered_unique<bmi::tag<struct by_name>,
    bmi::member<service, std::string, &service::version>
    >
    >
>;

#include <iostream>

Table read_json(std::string const& fname) {
    Table services;
    boost::property_tree::ptree pt;
    std::ifstream file(fname);
    boost::property_tree::read_json(file, pt);

    service svc;

    svc.version = pt.get("version", "");
    svc.imageData = pt.get("imageData", "");
    svc.imagePath = pt.get("imagePath", "");
    svc.imageHeight = pt.get("imageHeight", "");
    svc.imageWidth = pt.get("imageWidth", "");

    for (auto& v : pt.get_child("shapes"))
    {
        auto& node = v.second;

        //string type_ = v.second.get_value<string>();

        service::shapes_t shape;
        shape.label = node.get("label", "");
        shape.group_id = node.get("group_id", "");
        shape.shape_type = node.get("shape_type", "");

        //svc.points.para1 = node.get_child("points");

       /* for (auto& array3 : pt.get("points", "")) {
            int first, second;
            int* const elements[2] = { &first, &second};
            auto element = begin(elements);

            for (auto& i : array3) {
                **element++ = i.second.get_value<int>();

                if (element == end(elements)) break;
            }

            std::cout << "first:" << first << " second:" << second << "\n";
        }*/

        BOOST_FOREACH(boost::property_tree::ptree::value_type & rowPair, node.get_child("points")) {
            // rowPair.first == ""
            std::vector<string> cvPoints;
            BOOST_FOREACH(boost::property_tree::ptree::value_type & itemPair, rowPair.second) {
                //string coord = itemPair.second.get_value<std::string>();
                string coord = itemPair.second.get_value<string>();
                cout << coord << "  ******* ";
                cvPoints.push_back(coord);
            }

            cout << "  ******* \n ";
            shape.cvPoints.push_back(cvPoints);
            cout << endl;
        }

        cout << "  \n ";

        boost::property_tree::ptree node_flags = v.second.get_child("flags");
        boost::property_tree::ptree::iterator pos = node_flags.begin();
        for (; pos != node_flags.end(); ++pos)
        {
            //double value = pos->second.get_value<double>();
            cout << pos->first << "  [flags]: " << pos->second.get_value<string>() << "  ******* \n ";
        }




        svc.shapes.push_back(shape);

        //svc.points.para1 = node.get("points.para1", "");
        //svc.points.para2 = node.get("points.para2", "");
        //svc.points.para3 = node.get("points.para3", "");

        //if (!services.insert(svc).second)
        //    std::cout << "skipped duplicate service\n";
    }

    return services;
}

bool InsertJson(string newobj_fileName)
{
    string str = "{\"ID\":0,\"Student\":[{\"Name\":\"April\"},{\"Name\":\"Harris\"}]}";
    //stringstream stream(newobj_fileName);
    stringstream stream(str);
    ptree strTree;
    try {
        read_json(stream, strTree);
    }
    catch (boost::property_tree::ptree_error& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    ptree subject_info;
    ptree array1, array2, array3;
    array1.put("course", "Java");
    array2.put("course", "C++");
    array3.put("course", "MySql");
    subject_info.push_back(make_pair("", array1));
    subject_info.push_back(make_pair("", array2));
    subject_info.push_back(make_pair("", array3));

    strTree.put_child("Points", subject_info);
    stringstream s;
    write_json(s, strTree);
    string outstr = s.str();
    return true;
}



static inline std::ostream& operator<<(std::ostream& os, service const& s) {
    return os << "'" << s.version << "' params { "
        << "'" << "s.shapes.para1" << "' "
        << "'" << "s.shapes.para2" << "' "
        << "'" << "s.shapes.para3" << "' }";
}


