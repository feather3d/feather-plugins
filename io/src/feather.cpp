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

    std::cout << "OPEN FEATHER FILE\n";

    // clear out the scenegraph
    plugin::clear();

    std::fstream file;
    file.open(filename.c_str(),std::ios_base::in|std::ios_base::binary);

    file.seekg(0);
    header_t header;
    file.read((char*)&header,sizeof(header_t));
    std::cout << "read header\n;";

    std::cout << "version = " << header.major << "." << header.minor << std::endl
        << "sframe = " << header.stime << std::endl
        << "eframe = " << header.etime << std::endl
        << "cframe = " << header.ctime << std::endl
        << "fps = " << header.fps << std::endl;

    std::vector<node_t> nodes;
    std::vector<link_t> links;
    std::vector<field_t> fields;

    // READ NODES
    uint32_t node_count; 
    file.read((char*)&node_count,sizeof(uint32_t));

    for(unsigned int i=0; i < node_count; i++){
        node_t node;
        std::cout << "read node\n";
        file.read((char*)&node,sizeof(node_t));
        char cname[node.namelength];
        std::cout << "read node name\n";
        file.read((char*)&cname,node.namelength);
        std::string name;
        name.insert(0,cname,node.namelength); 
        std::cout << "name is " << name << ", length = " << name.size() << std::endl;

        node.uid = plugin::add_node(node.nid,name,p);

        std::cout << "NODE INFO FOR: " << name  << std::endl 
            << "\tuid:" << node.uid << std::endl
            << "\tnid:" << node.nid << std::endl
            << "\tnamelength:" << node.namelength << std::endl;

    }

    // READ LINKS
    uint32_t link_count; 
    file.read((char*)&link_count,sizeof(uint32_t));

    for(unsigned int j=0; j < link_count; j++) {
        link_t link;
        std::cout << "read link\n";
        file.read((char*)&link,sizeof(link_t));

        std::cout << "\tLINK INFO" << std::endl 
            << "\tsuid:" << link.suid << std::endl
            << "\tsfid:" << link.sfid << std::endl
            << "\ttuid:" << link.tuid << std::endl
            << "\ttfid:" << link.tfid << std::endl;
        plugin::connect(link.suid,link.sfid,link.tuid,link.tfid);
    }


    // READ FIELDS 
    uint32_t field_count; 
    file.read((char*)&field_count,sizeof(uint32_t));

    for(uint32_t i=0; i < field_count; i++){

        std::cout << "step 1\n";

        field_t field;
        file.read((char*)&field,sizeof(field_t));
        std::cout << "\tDATA INFO" << std::endl 
            << "\tuid:" << field.uid << std::endl
            << "\tnid:" << field.nid << std::endl
            << "\tfid:" << field.fid << std::endl
            << "\ttype:" << field.type << std::endl
            << "\tlength:" << field.length << std::endl;
        // set the field value
        field::FieldBase* srcfield = plugin::get_node_field_base(field.uid,field.nid,field.fid);

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

        uint32_t length;

        switch(field.type){
            case field::Bool:
                std::cout << "read Bool value\n";
                file.read((char*)&bval,sizeof(bool));
                static_cast<field::Field<bool>*>(srcfield)->value = bval;
                break;
            case field::Int:
                std::cout << "read Int value\n";
                file.read((char*)&ival,sizeof(int));
                static_cast<field::Field<int>*>(srcfield)->value = ival;
                break;
            case field::Float:
                std::cout << "read Float value\n";
                file.read((char*)&fval,sizeof(float));
                static_cast<field::Field<float>*>(srcfield)->value = fval;
                break;
            case field::Double:
                std::cout << "read Double value\n";
                file.read((char*)&dval,sizeof(double));
                static_cast<field::Field<double>*>(srcfield)->value = dval;
                break;
            case field::Real:
                std::cout << "read Real value\n";
                file.read((char*)&dval,sizeof(double));
                static_cast<field::Field<double>*>(srcfield)->value = dval;
                break;
            case field::Vertex:
                std::cout << "read Vertex value\n";
                file.read((char*)&vertexval,sizeof(FVertex3D));
                static_cast<field::Field<FVertex3D>*>(srcfield)->value = vertexval;
                break;
            case field::Vector:
                std::cout << "read Vector value\n";
                file.read((char*)&vectorval,sizeof(FVector3D));
                static_cast<field::Field<FVector3D>*>(srcfield)->value = vectorval;
                break;
            case field::Mesh:
                std::cout << "read Mesh value\n";
                // v
                file.read((char*)&length,sizeof(uint32_t));
                for(uint32_t i=0; i < length; i++){
                    FVertex3D v;
                    file.read((char*)&v,sizeof(FVertex3D));
                    meshval.v.push_back(v);
                }
                // st
                file.read((char*)&length,sizeof(uint32_t));
                for(uint32_t i=0; i < length; i++){
                    FTextureCoord st;
                    file.read((char*)&st,sizeof(FTextureCoord));
                    meshval.st.push_back(st);
                }
                // vn
                file.read((char*)&length,sizeof(uint32_t));
                for(uint32_t i=0; i < length; i++){
                    FVertex3D vn;
                    file.read((char*)&vn,sizeof(FVertex3D));
                    meshval.vn.push_back(vn);
                }
                // f
                uint32_t fcount;
                uint32_t fpcount;
                file.read((char*)&fcount,sizeof(uint32_t));
                for(uint32_t i=0; i < fcount; i++){
                    file.read((char*)&fpcount,sizeof(uint32_t));
                    FFacePoint fp;
                    FFace face;
                    for(uint32_t j=0; j < fpcount; j++){
                        file.read((char*)&fp,sizeof(FFacePoint));
                        face.push_back(fp);
                    }
                    meshval.f.push_back(face);
                }
                static_cast<field::Field<FMesh>*>(srcfield)->value = meshval;
                break;
            case field::RGB:
                std::cout << "read RGB value\n";
                file.read((char*)&rgbval,sizeof(FColorRGB));
                static_cast<field::Field<FColorRGB>*>(srcfield)->value = rgbval;
                break;
            case field::RGBA:
                std::cout << "read RGBA value\n";
                file.read((char*)&rgbaval,sizeof(FColorRGBA));
                static_cast<field::Field<FColorRGBA>*>(srcfield)->value = rgbaval;
                break;
            case field::Matrix3x3:
                std::cout << "read Matrix3x3 value\n";
                file.read((char*)&matrix3x3val,sizeof(FMatrix3x3));
                static_cast<field::Field<FMatrix3x3>*>(srcfield)->value = matrix3x3val;
                break;
            case field::Matrix4x4:
                std::cout << "read Matrix4x4 value\n";
                file.read((char*)&matrix4x4val,sizeof(FMatrix4x4));
                static_cast<field::Field<FMatrix4x4>*>(srcfield)->value = matrix4x4val;
                break;
            case field::VertexIndiceWeight:
                std::cout << "read VertexIndiceWeight value\n";
                file.read((char*)&vertexindiceweightval,sizeof(FVertexIndiceWeight));
                static_cast<field::Field<FVertexIndiceWeight>*>(srcfield)->value = vertexindiceweightval;
                break;
            case field::VertexIndiceGroupWeight:
                std::cout << "read VertexIndiceGroupWeight value\n";
                file.read((char*)&vertexindicegroupweightval,sizeof(FVertexIndiceGroupWeight));
                static_cast<field::Field<FVertexIndiceGroupWeight>*>(srcfield)->value = vertexindicegroupweightval;
                break;
            case field::Key:
                std::cout << "read Key value\n";
                file.read((char*)&keyval,sizeof(FKey));
                static_cast<field::Field<FKey>*>(srcfield)->value = keyval;
                break;
            case field::CurvePoint2D:
                std::cout << "read CurvePoint2D value\n";
                file.read((char*)&curvepoint2dval,sizeof(FCurvePoint2D));
                static_cast<field::Field<FCurvePoint2D>*>(srcfield)->value = curvepoint2dval;
                break;
            case field::CurvePoint3D:
                std::cout << "read CurvePoint3D value\n";
                file.read((char*)&curvepoint3dval,sizeof(FCurvePoint3D));
                static_cast<field::Field<FCurvePoint3D>*>(srcfield)->value = curvepoint3dval;
                break;
            default:
                std::cout << "FAILED TO FIND VALUE TYPE!\n";
                break;
        };
    }

    file.close();

    return true;
}


bool io::feather_format::save(std::string filename) {
    std::cout << "SAVE FILE\n";
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
    header.stime = static_cast<feather::field::Field<double>*>(plugin::get_field_base(time_uid,5))->value;
    header.etime = static_cast<feather::field::Field<double>*>(plugin::get_field_base(time_uid,6))->value;
    header.ctime = static_cast<feather::field::Field<double>*>(plugin::get_field_base(time_uid,7))->value;
    header.fps = static_cast<feather::field::Field<double>*>(plugin::get_field_base(time_uid,8))->value;

    std::cout << "write header\n"
        << "major:" << header.major << std::endl
        << "minor:" << header.minor << std::endl
        << "stime:" << header.stime << std::endl
        << "etime:" << header.etime << std::endl
        << "fps:" << header.fps << std::endl;

    file.write((char*)&header,sizeof(header));

    // remove root from uid list
    uids.erase(uids.begin());

    std::vector<node_t> nodes;
    std::vector<link_t> links;
    std::vector<field_t> fields;
    status p;

    // FILL IN NODE, LINK and FIELD vectors 
    for (unsigned int uid : uids){
        node_t node;
        node.uid = uid;
        node.nid = plugin::get_node_id(uid,p);
        std::string name;
        plugin::get_node_name(uid,name,p);
        node.namelength = strlen(name.c_str());
        
        nodes.push_back(node);
        
        //file.write((char*)&node,sizeof(node));
        //file.write(name.c_str(),name.size());
        // get all the node's out field connections
        std::vector<unsigned int> fids;
        // we are going to use the in connections to get the links since
        // the out fields can have multiple links but the in fields can
        // only have one
        p = plugin::get_in_fields(uid,fids);

        for (unsigned int fid : fids) {
            field::FieldBase* srcfield = plugin::get_node_field_base(uid,node.nid,fid);
            if(srcfield->connected()){
                // ADD LINK
                std::cout << "field " << fid << " connected.\n";
                for(auto conn : srcfield->connections){
                    link_t link;
                    link.suid = conn.puid;
                    link.sfid = conn.pfid;
                    link.tuid = uid;
                    link.tfid = fid;
                    std::cout << "\tADD LINK" 
                        << " suid:" << link.suid
                        << " sfid:" << link.sfid
                        << " tuid:" << link.tuid
                        << " tfid:" << link.tfid
                        << std::endl;
                    links.push_back(link);
                };
            } else {
                // ADD FIELD
                field_t field;
                field.uid = uid;
                field.nid = node.nid;
                field.fid = fid;
                fields.push_back(field); 
            }

        }

    }

    // WRITE NODES
    uint32_t node_count = nodes.size();
    file.write((char*)&node_count,sizeof(uint32_t));
    for(auto node : nodes) {
        std::string name;
        plugin::get_node_name(node.uid,name,p);
        file.write((char*)&node,sizeof(node_t));
        file.write((char*)name.c_str(),node.namelength);
        std::cout << "NODE NAME = [" << name << "], namelength=" << node.namelength << ", c_str() length=" << sizeof(name.c_str()) << std::endl;
        std::cout << "NODE INFO FOR: " << name  << std::endl 
            << "\tuid:" << node.uid << std::endl
            << "\tnid:" << node.nid << std::endl
            << "\tnamelength:" << node.namelength << std::endl;
    }

    // WRITE LINKS
    uint32_t link_count = links.size();
    std::cout << "link count:" << link_count << std::endl;
    file.write((char*)&link_count,sizeof(uint32_t));
    file.write((char*)&links[0],link_count*sizeof(link_t));

    // GET FIELD TYPE AND LENGTH
    for(uint32_t i=0; i < fields.size(); i++) {
        field::FieldBase* srcfield = plugin::get_node_field_base(fields[i].uid,fields[i].nid,fields[i].fid);
        fields[i].type = srcfield->type;
        std::cout << "WRITTING FIELD " << fields[i].type << " " << srcfield->type << std::endl;

        switch(srcfield->type){
            case field::Bool:
                fields[i].length = sizeof(bool);
                break;
            case field::Int:
                fields[i].length = sizeof(int);
                break;
            case field::Float:
                fields[i].length = sizeof(float);
                break;
            case field::Double:
                fields[i].length = sizeof(double);
                break;
            case field::Real:
                fields[i].length = sizeof(double);
                break;
            case field::Vertex:
                fields[i].length = sizeof(FVertex3D);
                break;
            case field::Vector:
                fields[i].length = sizeof(FVector3D);
                break;
            case field::Mesh:
                break;
            case field::RGB:
                fields[i].length = sizeof(FColorRGB);
                break;
            case field::RGBA:
                fields[i].length = sizeof(FColorRGBA);
                break;
            case field::Matrix3x3:
                fields[i].length = sizeof(FMatrix3x3);
                break;
            case field::Matrix4x4:
                fields[i].length = sizeof(FMatrix4x4);
                break;
            case field::VertexIndiceWeight:
                fields[i].length = sizeof(FVertexIndiceWeight);
                break;
            case field::VertexIndiceGroupWeight:
                fields[i].length = sizeof(FVertexIndiceGroupWeight);
                break;
            case field::Key:
                fields[i].length = sizeof(FKey);
                break;
            case field::CurvePoint2D:
                fields[i].length = sizeof(FCurvePoint2D);
                break;
            case field::CurvePoint3D:
                fields[i].length = sizeof(FCurvePoint3D);
                break;
            default:
                std::cout << "FAILED TO FIND VALUE TYPE!\n";
                fields[i].length = 0;
                break;            
        };

    } 


    // WRITE FIELD DATA

    uint32_t field_count = fields.size();
    file.write((char*)&field_count,sizeof(uint32_t));

    for(auto field : fields) {
        field::FieldBase* srcfield = plugin::get_node_field_base(field.uid,field.nid,field.fid);
        file.write((char*)&field,sizeof(field_t));
        std::cout << "WRITTING TYPE " << field.type << " " << srcfield->type << std::endl;
        std::cout << "WRITTING LENGTH " << field.length << std::endl;

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
        uint32_t length;

        switch(srcfield->type){
            case field::Bool:
                std::cout << "write Bool field\n";
                bval = static_cast<field::Field<bool>*>(srcfield)->value;
                file.write((char*)&bval,field.length);
                break;
            case field::Int:
                std::cout << "write Int field\n";
                ival = static_cast<field::Field<int>*>(srcfield)->value;
                file.write((char*)&ival,field.length);
                break;
            case field::Float:
                std::cout << "write Float field\n";
                fval = static_cast<field::Field<float>*>(srcfield)->value;
                file.write((char*)&fval,field.length);
                break;
            case field::Double:
                std::cout << "write Double field\n";
                dval = static_cast<field::Field<double>*>(srcfield)->value;
                file.write((char*)&dval,field.length);
                break;
            case field::Real:
                std::cout << "write Real field\n";
                dval = static_cast<field::Field<double>*>(srcfield)->value;
                file.write((char*)&dval,field.length);
                break;
            case field::Vertex:
                std::cout << "write Vertex field\n";
                vertexval = static_cast<field::Field<FVertex3D>*>(srcfield)->value;
                file.write((char*)&vertexval,field.length);
                break;
            case field::Vector:
                std::cout << "write Vector field\n";
                vectorval = static_cast<field::Field<FVector3D>*>(srcfield)->value;
                file.write((char*)&vectorval,field.length);
                break;
            case field::Mesh:
                std::cout << "write Mesh field\n";
                meshval = static_cast<field::Field<FMesh>*>(srcfield)->value;
                // v
                length = meshval.v.size();
                file.write((char*)&length,sizeof(uint32_t));
                file.write((char*)&meshval.v[0],length*sizeof(FVertex3D));
                // st
                length = meshval.st.size();
                file.write((char*)&length,sizeof(uint32_t));
                file.write((char*)&static_cast<field::Field<FMesh>*>(srcfield)->value.st[0],length*sizeof(FTextureCoord));
                // vn
                length = meshval.vn.size();
                file.write((char*)&length,sizeof(uint32_t));
                file.write((char*)&static_cast<field::Field<FMesh>*>(srcfield)->value.vn[0],length*sizeof(FVertex3D));
                // f
                length = meshval.f.size();
                file.write((char*)&length,sizeof(uint32_t));
                for(uint32_t i=0; i < meshval.f.size(); i++){
                    length = meshval.f[i].size();
                    file.write((char*)&length,sizeof(uint32_t));
                    // write each face point
                    for(uint32_t j=0; j < length; j++){
                        file.write((char*)&static_cast<field::Field<FMesh>*>(srcfield)->value.f[i][j],sizeof(FFacePoint));
                    }
                }
                break;
            case field::RGB:
                std::cout << "write RGB field\n";
                rgbval = static_cast<field::Field<FColorRGB>*>(srcfield)->value;
                file.write((char*)&rgbval,field.length);
                break;
            case field::RGBA:
                std::cout << "write RGBA field\n";
                rgbaval = static_cast<field::Field<FColorRGBA>*>(srcfield)->value;
                file.write((char*)&rgbaval,field.length);
                break;
            case field::Matrix3x3:
                std::cout << "write Matrix3x3 field\n";
                matrix3x3val = static_cast<field::Field<FMatrix3x3>*>(srcfield)->value;
                file.write((char*)&matrix3x3val,field.length);
                break;
            case field::Matrix4x4:
                std::cout << "write Matrix4x4 field\n";
                matrix4x4val = static_cast<field::Field<FMatrix4x4>*>(srcfield)->value;
                file.write((char*)&matrix4x4val,field.length);
                break;
            case field::VertexIndiceWeight:
                std::cout << "write VertexIndiceWeight field\n";
                vertexindiceweightval = static_cast<field::Field<FVertexIndiceWeight>*>(srcfield)->value;
                file.write((char*)&vertexindiceweightval,field.length);
                break;
            case field::VertexIndiceGroupWeight:
                std::cout << "write VertexIndiceGroupWeight field\n";
                vertexindicegroupweightval = static_cast<field::Field<FVertexIndiceGroupWeight>*>(srcfield)->value;
                file.write((char*)&vertexindicegroupweightval,field.length);
                break;
            case field::Key:
                std::cout << "write Key field\n";
                keyval = static_cast<field::Field<FKey>*>(srcfield)->value;
                file.write((char*)&keyval,field.length);
                break;
            case field::CurvePoint2D:
                std::cout << "write CurvePoint2D field\n";
                curvepoint2dval = static_cast<field::Field<FCurvePoint2D>*>(srcfield)->value;
                file.write((char*)&curvepoint2dval,field.length);
                break;
            case field::CurvePoint3D:
                std::cout << "write CurvePoint3D field\n";
                curvepoint3dval = static_cast<field::Field<FCurvePoint3D>*>(srcfield)->value;
                file.write((char*)&curvepoint3dval,field.length);
                break;
            default:
                std::cout << "FAILED TO FIND VALUE TYPE!\n";
                break;
        };

        std::cout << "\tDATA INFO" << std::endl 
            << "\tfid:" << field.fid << std::endl
            << "\ttype:" << field.type << std::endl
            << "\tlength:" << field.length << std::endl;

    }

    std::cout << "write file end\n";

    // terminating string to verify the file saved correctly
    file.write("<eof>",6);
 
    file.close();

    return true;
}
