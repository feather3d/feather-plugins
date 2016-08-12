/***********************************************************************
 *
 * Filename: feather.cpp
 *
 * Description: Code to open and save feather files.
 *
 * Copyright (C) 2016 Richard Layman, rlayman2000@yahoo.com 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "feather.hpp"
#include <feather/plugin.hpp>
#include <feather/field.hpp>

using namespace feather;

bool io::feather_format::open(std::string filename) {
    unsigned int uid;
    plugin::get_node_by_name("time",uid);

    std::fstream file;
    file.open(filename,std::ios_base::in|std::ios_base::binary);

    file.seekg(0);
    header_t header;
    file.read((char*)&header,sizeof(header));

    std::cout << "version = " << header.major << "." << header.minor << std::endl
        << "sframe = " << header.stime << std::endl
        << "eframe = " << header.etime << std::endl
        << "cframe = " << header.ctime << std::endl
        << "fps = " << header.fps << std::endl
        << "node count = " << header.nodecount << std::endl
        << "link count = " << header.linkcount << std::endl;

    file.close();

    return true;
}


bool io::feather_format::save(std::string filename) {
    std::fstream file;
    file.open(filename,std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);

    std::stringstream data;

    unsigned int time_uid;
    plugin::get_node_by_name("time",time_uid);
    std::vector<unsigned int> uids; // all the scenegraph uids
    plugin::get_nodes(uids);

    header_t header;

    header.major = 0;
    header.minor = 1;
    header.stime = static_cast<feather::field::Field<double>*>(plugin::get_field_base(time_uid,7))->value;
    header.etime = static_cast<feather::field::Field<double>*>(plugin::get_field_base(time_uid,8))->value;
    header.ctime = static_cast<feather::field::Field<double>*>(plugin::get_field_base(time_uid,9))->value;
    header.fps = static_cast<feather::field::Field<double>*>(plugin::get_field_base(time_uid,10))->value;
    header.nodecount = uids.size();
    header.linkcount = 10;
    file.write((char*)&header,sizeof(header));

    // add all the nodes
    for (unsigned int uid : uids){
        node_t node;
        status p;
        node.uid = uid;
        node.nid = plugin::get_node_id(uid,p);
        std::string name;
        plugin::get_node_name(uid,name,p);
        node.namelength = strlen(name.c_str());
        node.name = (char*)name.c_str();
        file.write((char*)&node,sizeof(node));
    }
 
    file.close();

    return true;
}
