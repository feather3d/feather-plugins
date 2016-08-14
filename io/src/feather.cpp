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
    status p;

    // clear out the scenegraph
    plugin::clear();

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
        << "node count = " << header.nodecount << std::endl;

    // create and add the nodes to the scenegraph
    for(unsigned int i=0; i < header.nodecount; i++){
        node_t node;
        file.read((char*)&node,sizeof(node_t));
        char* cname;
        std::cout << "name length = " << node.namelength << std::endl;
        file.read(cname,node.namelength);
        std::string name(cname); 
        std::cout << "name is " << name << " and to add_node\n";
        //unsigned int uid = plugin::add_node(node.nid,name,p);
        std::cout << "NODE INFO FOR: " << name  << std::endl 
            << "\tuid:" << node.uid << std::endl
            << "\tnid:" << node.nid << std::endl
            << "\tnamelength:" << node.namelength << std::endl
            << "\tlinkcount:" << node.linkcount << std::endl
            << "\tdatacount:" << node.datacount << std::endl;

        // NODE LINKS
        for(unsigned int j=0; j < node.linkcount; j++) {
            link_t link;
            file.read((char*)&link,sizeof(link_t));
            std::cout << "\tLINK INFO" << std::endl 
                << "\tsuid:" << link.suid << std::endl
                << "\tsfid:" << link.sfid << std::endl
                << "\ttuid:" << link.tuid << std::endl
                << "\ttfid:" << link.tfid << std::endl;
        }

        // NODE DATA
        for(unsigned int j=0; j < node.datacount; j++) {
            data_t data;
            file.read((char*)&data,sizeof(data_t));
            std::cout << "\tDATA INFO" << std::endl 
                << "\tfid:" << data.fid << std::endl
                << "\ttype:" << data.type << std::endl
                << "\tlength:" << data.length << std::endl;
            // set the field value
            field::FieldBase* srcfield = plugin::get_node_field_base(node.uid,node.nid,data.fid);
            int ival;
            float fval;
            double dval;
 
            switch(data.type){
                case field::Int:
                    file.read((char*)&ival,sizeof(int));
                    static_cast<field::Field<int>*>(srcfield)->value = ival;
                    break;
                case field::Float:
                    file.read((char*)&fval,sizeof(float));
                    static_cast<field::Field<float>*>(srcfield)->value = fval;
                    break;
                case field::Double:
                    file.read((char*)&dval,sizeof(double));
                    static_cast<field::Field<double>*>(srcfield)->value = dval;
                    break;
                case field::Real:
                    file.read((char*)&dval,sizeof(double));
                    static_cast<field::Field<double>*>(srcfield)->value = dval;
                    break;
                    // field::Node can be skipped since it only for connections
                default:
                    std::cout << "FAILED TO FIND VALUE TYPE!\n";
                    break;
            };

        }
    }

    file.close();

    return true;
}


bool io::feather_format::save(std::string filename) {
    std::fstream file;
    file.open(filename,std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);
    //file.open(filename,std::ios_base::out|std::ios_base::binary);

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
    header.nodecount = uids.size()-1; // we are not going to add the root node from the uid list
    file.write((char*)&header,sizeof(header));

    // remove root from uid list
    uids.erase(uids.begin());

    std::cout << "SAVING FEATHER FORMAT:\n";

    // add all the nodes
    for (unsigned int uid : uids){
        node_t node;
        std::vector<link_t> links; // container for all connections
        std::vector<data_t> fielddata; // container for all connections
        status p;
        node.uid = uid;
        node.nid = plugin::get_node_id(uid,p);
        std::string name;
        plugin::get_node_name(uid,name,p);
        node.namelength = strlen(name.c_str());


        //file.write((char*)&node,sizeof(node));
        //file.write(name.c_str(),name.size());
        // get all the node's out field connections
        std::vector<unsigned int> fids;
        // we are going to use the in connections to get the links since
        // the out fields can have multiple links but the in fields can
        // only have one
        p = plugin::get_in_fields(uid,fids);

        node.linkcount=0;
        node.datacount=0;

        // do a quick scan to get the link and data counts
        for (unsigned int fid : fids) {
            field::FieldBase* srcfield = plugin::get_node_field_base(uid,node.nid,fid);
            if(srcfield->connected)
                node.linkcount++;
            else
                node.datacount++;
        }

        // write the node info
        file.write((char*)&node,sizeof(node));
        file.write(name.c_str(),name.size());

        std::cout << "NODE INFO FOR: " << name  << std::endl 
            << "\tuid:" << node.uid << std::endl
            << "\tnid:" << node.nid << std::endl
            << "\tnamelength:" << node.namelength << std::endl
            << "\tlinkcount:" << node.linkcount << std::endl
            << "\tdatacount:" << node.datacount << std::endl;

        for (unsigned int fid : fids) {
            field::FieldBase* srcfield = plugin::get_node_field_base(uid,node.nid,fid);
            if(srcfield->connected){
                link_t link;
                link.suid = srcfield->puid;
                link.sfid = srcfield->pf;
                link.tuid = uid;
                link.tfid = fid;
                links.push_back(link);
           } else {
                data_t fieldinfo;
                fieldinfo.fid = fid;
                fieldinfo.type = srcfield->type;
                switch(srcfield->type){
                    case field::Int:
                        fieldinfo.length = sizeof(unsigned int);
                        break;
                    case field::Float:
                        fieldinfo.length = sizeof(float);
                        break;
                     case field::Double:
                        fieldinfo.length = sizeof(double);
                        break;
                     case field::Real:
                        fieldinfo.length = sizeof(double);
                        break;
                    default:
                        std::cout << "FAILED TO FIND VALUE TYPE!\n";
                        fieldinfo.length = 0;
                        break;            
                };
                //file.write((char*)&fieldinfo,sizeof(data_t));
                // only add the data if its of these types
                if( fieldinfo.type == field::Int ||
                        fieldinfo.type == field::Float ||
                        fieldinfo.type == field::Double ||
                        fieldinfo.type == field::Real )
                    fielddata.push_back(fieldinfo);
           } 


            for ( link_t link : links ) {
                field::FieldBase* srcfield = plugin::get_node_field_base(uid,node.nid,fid);
                file.write((char*)&link,sizeof(link_t));
                std::cout << "\tLINK INFO" << std::endl 
                    << "\tsuid:" << link.suid << std::endl
                    << "\tsfid:" << link.sfid << std::endl
                    << "\ttuid:" << link.tuid << std::endl
                    << "\ttfid:" << link.tfid << std::endl;
             } 

            for ( data_t fieldinfo : fielddata ) {
                field::FieldBase* srcfield = plugin::get_node_field_base(uid,node.nid,fieldinfo.fid);
                file.write((char*)&fieldinfo,sizeof(data_t));
                unsigned int ival;
                float fval;
                double dval;
                switch(srcfield->type){
                    case field::Int:
                        ival = static_cast<field::Field<unsigned int>*>(srcfield)->value;
                        file.write((char*)&ival,fieldinfo.length);
                        break;
                    case field::Float:
                        fval = static_cast<field::Field<float>*>(srcfield)->value;
                        file.write((char*)&fval,fieldinfo.length);
                        break;
                    case field::Double:
                        dval = static_cast<field::Field<double>*>(srcfield)->value;
                        file.write((char*)&dval,fieldinfo.length);
                        break;
                    case field::Real:
                        dval = static_cast<field::Field<double>*>(srcfield)->value;
                        file.write((char*)&dval,fieldinfo.length);
                        break;
                        // field::Node can be skipped since it only for connections
                    default:
                        std::cout << "FAILED TO FIND VALUE TYPE!\n";
                        break;
                };

                std::cout << "\tDATA INFO" << std::endl 
                    << "\tfid:" << fieldinfo.fid << std::endl
                    << "\ttype:" << fieldinfo.type << std::endl
                    << "\tlength:" << fieldinfo.length << std::endl;

            }
        }
    }

    file.close();

    return true;
}
