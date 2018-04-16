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

/*
 * FEATHER FORMAT
 *
 * [header_t]
 * [node_count]
 * NODE DATA
 *      [node_t]
 *      [node_name]
 * [link_count]
 * LINK DATA
 *      [link_t]
 *      ...             // numbers of links based on node_t.link_count
 * [field_count]
 * FIELD DATA
 *      [field_t]
 *      [field data]    // length based on data_t.length
 *      ...
 *
 *
 * When opening Feather files all the nodes are created, links made and then values set.
 */
    namespace feather_format
    {

        struct header_t {
            uint32_t major; // version number
            uint32_t minor; // version number
            double stime;
            double etime;
            double ctime;
            double fps;
            //uint32_t nodecount; // how many nodes in the scenegraph
        };

        struct node_t {
            uint32_t uid;
            uint32_t nid;
            uint32_t namelength;
            //uint8_t linkcount;
            //uint8_t datacount;
        };

        struct link_t {
            uint32_t suid;
            uint32_t sfid;
            uint32_t tuid;
            uint32_t tfid;
        };

        struct field_t {
            uint32_t uid;
            uint32_t nid;
            uint32_t fid;
            uint32_t type;
            uint32_t length;
        };

        bool open(std::string filename);

        bool save(std::string filename);

    } // namespace feather

} // namespace io


#endif
