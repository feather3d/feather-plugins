/***********************************************************************
 *
 * Filename: feather.hpp
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

#ifndef FEATHER_HPP
#define FEATHER_HPP

#include <feather/types.hpp>
#include <feather/deps.hpp>
#include <feather/status.hpp>

namespace io 
{

    namespace feather_format
    {

        struct header_t {
            unsigned short major; // version number
            unsigned short minor; // version number
            double stime;
            double etime;
            double ctime;
            double fps;
            unsigned int nodecount; // how many nodes in the scenegraph
            unsigned int linkcount; // how many nodes in the scenegraph
        };

        struct node_t {
            unsigned int uid;
            unsigned int nid;
            unsigned short namelength;
            char* name;
        };

        struct link_t {
            unsigned int suid;
            unsigned int sfid;
            unsigned int tuid;
            unsigned int tfid;
        };

        bool open(std::string filename);

        bool save(std::string filename);

    } // namespace feather

} // namespace io


#endif
