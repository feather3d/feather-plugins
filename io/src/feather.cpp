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



    return true;
}


bool io::feather_format::save(std::string filename) {
    std::fstream file;
    file.open(filename,std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);

    std::stringstream data;

    unsigned int uid;
    plugin::get_node_by_name("time",uid);
    std::cout << "saved filename is " << filename << " and the time uid is " << uid << std::endl;

    header_t header;

    data << "<header>";
    //data << 0 << 1; // version number
    header.major = 0;
    header.minor = 1;
    header.stime = static_cast<feather::field::Field<double>*>(plugin::get_field_base(uid,7))->value;
    header.etime = static_cast<feather::field::Field<double>*>(plugin::get_field_base(uid,8))->value;
    header.ctime = static_cast<feather::field::Field<double>*>(plugin::get_field_base(uid,9))->value;
    header.fps = static_cast<feather::field::Field<double>*>(plugin::get_field_base(uid,10))->value;

    data.write((char*)&header,sizeof(header));

    // header data
    data << "<plugins>";
    // plugin names
    data << "</plugins>";
    data << "<nodes>";
    // plugin names
    data << "</nodes>";
    data << "</header>";
    data << "<data>";
    // data
    data << "</data>";
    data << "<conn>";
    // connections
    data << "</conn>";

    std::istreambuf_iterator<char> src(data.rdbuf());
    std::istreambuf_iterator<char> end;
    std::ostreambuf_iterator<char> dest(file);
    std::copy(src,end,dest);

    file.close();

    return true;
}
