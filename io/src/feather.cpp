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

// TODO - add support for arrays
// Currently fields that store array values can not be save or loaded.
// This means that KeyTracks can not save keys and needs to be fixed.


bool io::feather_format::open(std::string filename) {
    status p;

    // clear out the scenegraph
    plugin::clear();

    std::fstream file;
    file.open(filename.c_str(),std::ios_base::in|std::ios_base::binary);

    file.seekg(0);
    header_t header;
    file.read((char*)&header,sizeof(header_t));

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
        char cname[node.namelength];
        //std::cout << "name length = " << node.namelength << std::endl;
        file.read((char*)&cname,node.namelength);
        std::string name;
        name.insert(0,cname,node.namelength); 
        std::cout << "name is " << name << ", length = " << name.size() << std::endl;

        unsigned int uid = plugin::add_node(node.nid,name,p);

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

            plugin::connect(link.suid,link.sfid,link.tuid,link.tfid);
        }

        // NODE DATA
        for(unsigned int j=0; j < node.datacount; j++) {
            std::cout << "step 1\n";

            data_t data;
            file.read((char*)&data,sizeof(data_t));
            std::cout << "\tDATA INFO" << std::endl 
                << "\tfid:" << data.fid << std::endl
                << "\ttype:" << data.type << std::endl
                << "\tlength:" << data.length << std::endl;
            // set the field value
            field::FieldBase* srcfield = plugin::get_node_field_base(node.uid,node.nid,data.fid);

            bool bval;
            int ival;
            float fval;
            double dval;
            FVertex3D vertexval;
            FVector3D vectorval;
            FMesh meshval;
            FColorRGB rgbval;
            FColorRGBA rgbaval;
            FMatrix3x3 matrix3x3val;
            FMatrix4x4 matrix4x4val;
            FVertexIndiceWeight vertexindiceweightval;
            FVertexIndiceGroupWeight vertexindicegroupweightval;
            FKey keyval;
            FCurvePoint2D curvepoint2dval;
            FCurvePoint3D curvepoint3dval;

            switch(srcfield->type){
                case field::Bool:
                    file.read((char*)&bval,sizeof(bool));
                    static_cast<field::Field<bool>*>(srcfield)->value = bval;
                    break;
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
                case field::Vertex:
                    file.read((char*)&vertexval,sizeof(FVertex3D));
                    static_cast<field::Field<FVertex3D>*>(srcfield)->value = vertexval;
                    break;
                case field::Vector:
                    file.read((char*)&vectorval,sizeof(FVector3D));
                    static_cast<field::Field<FVector3D>*>(srcfield)->value = vectorval;
                    break;
                case field::Mesh:
                    file.read((char*)&meshval,sizeof(FMesh));
                    static_cast<field::Field<FMesh>*>(srcfield)->value = meshval;
                    break;
                case field::RGB:
                    file.read((char*)&rgbval,sizeof(FColorRGB));
                    static_cast<field::Field<FColorRGB>*>(srcfield)->value = rgbval;
                    break;
                case field::RGBA:
                    file.read((char*)&rgbaval,sizeof(FColorRGBA));
                    static_cast<field::Field<FColorRGBA>*>(srcfield)->value = rgbaval;
                    break;
                case field::Matrix3x3:
                    file.read((char*)&matrix3x3val,sizeof(FMatrix3x3));
                    static_cast<field::Field<FMatrix3x3>*>(srcfield)->value = matrix3x3val;
                    break;
                case field::Matrix4x4:
                    file.read((char*)&matrix4x4val,sizeof(FMatrix4x4));
                    static_cast<field::Field<FMatrix4x4>*>(srcfield)->value = matrix4x4val;
                    break;
                case field::VertexIndiceWeight:
                    file.read((char*)&vertexindiceweightval,sizeof(FVertexIndiceWeight));
                    static_cast<field::Field<FVertexIndiceWeight>*>(srcfield)->value = vertexindiceweightval;
                    break;
                case field::VertexIndiceGroupWeight:
                    file.read((char*)&vertexindicegroupweightval,sizeof(FVertexIndiceGroupWeight));
                    static_cast<field::Field<FVertexIndiceGroupWeight>*>(srcfield)->value = vertexindicegroupweightval;
                    break;
                case field::Key:
                    file.read((char*)&keyval,sizeof(FKey));
                    static_cast<field::Field<FKey>*>(srcfield)->value = keyval;
                    break;
                case field::CurvePoint2D:
                    file.read((char*)&curvepoint2dval,sizeof(FCurvePoint2D));
                    static_cast<field::Field<FCurvePoint2D>*>(srcfield)->value = curvepoint2dval;
                    break;
                case field::CurvePoint3D:
                    file.read((char*)&curvepoint3dval,sizeof(FCurvePoint3D));
                    static_cast<field::Field<FCurvePoint3D>*>(srcfield)->value = curvepoint3dval;
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
    header.stime = static_cast<feather::field::Field<double>*>(plugin::get_field_base(time_uid,5))->value;
    header.etime = static_cast<feather::field::Field<double>*>(plugin::get_field_base(time_uid,6))->value;
    header.ctime = static_cast<feather::field::Field<double>*>(plugin::get_field_base(time_uid,7))->value;
    header.fps = static_cast<feather::field::Field<double>*>(plugin::get_field_base(time_uid,8))->value;
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

        for (unsigned int fid : fids) {
            field::FieldBase* srcfield = plugin::get_node_field_base(uid,node.nid,fid);
            if(srcfield->connected()){
                for(auto conn : srcfield->connections){
                    link_t link;
                    link.suid = conn.puid;
                    link.sfid = conn.pfid;
                    link.tuid = uid;
                    link.tfid = fid;
                    links.push_back(link);
                    node.linkcount++;
                };
            } else {
                node.datacount++;
                data_t fieldinfo;
                fieldinfo.fid = fid;
                fieldinfo.type = srcfield->type;
                switch(srcfield->type){
                    case field::Bool:
                        fieldinfo.length = sizeof(bool);
                        break;
                    case field::Int:
                        fieldinfo.length = sizeof(int);
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
                    case field::Vertex:
                        fieldinfo.length = sizeof(FVertex3D);
                        break;
                    case field::Vector:
                        fieldinfo.length = sizeof(FVector3D);
                        break;
                    case field::Mesh:
                        fieldinfo.length = sizeof(FMesh);
                        break;
                    case field::RGB:
                        fieldinfo.length = sizeof(FColorRGB);
                        break;
                    case field::RGBA:
                        fieldinfo.length = sizeof(FColorRGBA);
                        break;
                    case field::Matrix3x3:
                        fieldinfo.length = sizeof(FMatrix3x3);
                        break;
                    case field::Matrix4x4:
                        fieldinfo.length = sizeof(FMatrix4x4);
                        break;
                    case field::VertexIndiceWeight:
                        fieldinfo.length = sizeof(FVertexIndiceWeight);
                        break;
                    case field::VertexIndiceGroupWeight:
                        fieldinfo.length = sizeof(FVertexIndiceGroupWeight);
                        break;
                    case field::Key:
                        fieldinfo.length = sizeof(FKey);
                        break;
                    case field::CurvePoint2D:
                        fieldinfo.length = sizeof(FCurvePoint2D);
                        break;
                    case field::CurvePoint3D:
                        fieldinfo.length = sizeof(FCurvePoint3D);
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
                        fieldinfo.type == field::Real ||
                         fieldinfo.type == field::Vertex ||
                         fieldinfo.type == field::Vector ||
                        fieldinfo.type == field::Mesh ||
                        fieldinfo.type == field::RGB ||
                        fieldinfo.type == field::RGBA ||
                        fieldinfo.type == field::Matrix3x3 ||
                        fieldinfo.type == field::Matrix4x4 ||
                        fieldinfo.type == field::VertexIndiceWeight ||
                        fieldinfo.type == field::VertexIndiceGroupWeight ||
                        fieldinfo.type == field::Key ||
                        fieldinfo.type == field::CurvePoint2D ||
                        fieldinfo.type == field::CurvePoint3D
                  )
                    fielddata.push_back(fieldinfo);

            } 

        }

        file.write((char*)&node,sizeof(node_t));
        file.write((char*)name.c_str(),node.namelength);

        std::cout << "NODE NAME = [" << name << "], namelength=" << node.namelength << ", c_str() length=" << sizeof(name.c_str()) << std::endl;

        std::cout << "NODE INFO FOR: " << name  << std::endl 
            << "\tuid:" << node.uid << std::endl
            << "\tnid:" << node.nid << std::endl
            << "\tnamelength:" << node.namelength << std::endl
            << "\tlinkcount:" << node.linkcount << std::endl
            << "\tdatacount:" << node.datacount << std::endl;
 
        for ( link_t link : links ) {
            field::FieldBase* srcfield = plugin::get_node_field_base(uid,node.nid,link.tfid);
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

            bool bval;
            int ival;
            float fval;
            double dval;
            FVertex3D vertexval;
            FVector3D vectorval;
            FMesh meshval;
            FColorRGB rgbval;
            FColorRGBA rgbaval;
            FMatrix3x3 matrix3x3val;
            FMatrix4x4 matrix4x4val;
            FVertexIndiceWeight vertexindiceweightval;
            FVertexIndiceGroupWeight vertexindicegroupweightval;
            FKey keyval;
            FCurvePoint2D curvepoint2dval;
            FCurvePoint3D curvepoint3dval;

            switch(srcfield->type){
                case field::Bool:
                    bval = static_cast<field::Field<bool>*>(srcfield)->value;
                    file.write((char*)&bval,fieldinfo.length);
                    break;
                case field::Int:
                    ival = static_cast<field::Field<int>*>(srcfield)->value;
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
                case field::Vertex:
                    vertexval = static_cast<field::Field<FVertex3D>*>(srcfield)->value;
                    file.write((char*)&vertexval,fieldinfo.length);
                    break;
                case field::Vector:
                    vectorval = static_cast<field::Field<FVector3D>*>(srcfield)->value;
                    file.write((char*)&vectorval,fieldinfo.length);
                    break;
                case field::Mesh:
                    meshval = static_cast<field::Field<FMesh>*>(srcfield)->value;
                    file.write((char*)&meshval,fieldinfo.length);
                    break;
                case field::RGB:
                    rgbval = static_cast<field::Field<FColorRGB>*>(srcfield)->value;
                    file.write((char*)&rgbval,fieldinfo.length);
                    break;
                case field::RGBA:
                    rgbaval = static_cast<field::Field<FColorRGBA>*>(srcfield)->value;
                    file.write((char*)&rgbaval,fieldinfo.length);
                    break;
                case field::Matrix3x3:
                    matrix3x3val = static_cast<field::Field<FMatrix3x3>*>(srcfield)->value;
                    file.write((char*)&matrix3x3val,fieldinfo.length);
                    break;
                case field::Matrix4x4:
                    matrix4x4val = static_cast<field::Field<FMatrix4x4>*>(srcfield)->value;
                    file.write((char*)&matrix4x4val,fieldinfo.length);
                    break;
                case field::VertexIndiceWeight:
                    vertexindiceweightval = static_cast<field::Field<FVertexIndiceWeight>*>(srcfield)->value;
                    file.write((char*)&vertexindiceweightval,fieldinfo.length);
                    break;
                case field::VertexIndiceGroupWeight:
                    vertexindicegroupweightval = static_cast<field::Field<FVertexIndiceGroupWeight>*>(srcfield)->value;
                    file.write((char*)&vertexindicegroupweightval,fieldinfo.length);
                    break;
                case field::Key:
                    keyval = static_cast<field::Field<FKey>*>(srcfield)->value;
                    file.write((char*)&keyval,fieldinfo.length);
                    break;
                case field::CurvePoint2D:
                    curvepoint2dval = static_cast<field::Field<FCurvePoint2D>*>(srcfield)->value;
                    file.write((char*)&curvepoint2dval,fieldinfo.length);
                    break;
                case field::CurvePoint3D:
                    curvepoint3dval = static_cast<field::Field<FCurvePoint3D>*>(srcfield)->value;
                    file.write((char*)&curvepoint3dval,fieldinfo.length);
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

    // terminating string to verify the file saved correctly
    file.write("<eof>",6);
 
    file.close();

    return true;
}
