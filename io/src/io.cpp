/***********************************************************************
 *
 * Filename: io.cpp
 *
 * Description: Code to import and export 3d files.
 *
 * Copyright (C) 2015 Richard Layman, rlayman2000@yahoo.com 
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

#include "io.hpp"
#include <feather/plugin.hpp>

bool io::load_mesh(mesh_t& mesh, std::string path)
{
    return true;
}


bool io::write_mesh(obj_data_t& data)
{
    std::stringstream ss;
    std::fstream file;

    std::cout << "==========================\nExtracting Meshes\n==========================\n";

    for(object_t object : data.object)
    {
        // MetaData
        
        std::string filename = object.o + ".mesh";
        file.open(filename.c_str(),std::ios::out|std::ios::binary);
        ss << "<meta>name[" << object.o << "]";
        ss << "v[" << object.mesh.v.size() << "]";
        ss << "st[" << object.mesh.st.size() << "]";        
        ss << "vn[" << object.mesh.vn.size() << "]";
        ss << "groups[" << object.grp.size() << "]";

        for(group_t fg : object.grp)      
        {
            ss << "usemtl[" << fg.usemtl << "]";
            for(smoothing_group_t sg : fg.sg)
            {
                ss << "s[" << sg.s << "]";
                ss << "f[" << sg.f.size() << "]";
            }
        }

        ss << "</meta><data>";

        file.write(ss.str().c_str(),ss.tellp());

        for(feather::FVertex3D v : object.mesh.v)
            file.write((char*)&v, sizeof(feather::FVertex3D));

        for(feather::FTextureCoord st : object.mesh.st)
            file.write((char*)&st, sizeof(feather::FTextureCoord));

        for(feather::FVertex3D vn : object.mesh.vn)
            file.write((char*)&vn, sizeof(feather::FVertex3D));

        /*
        for(group_t group : object.grp)
            for(std::vector<face_point_t> f : group.f)
                for(face_point_t fp : f)
                    file.write((char*)&fp, sizeof(face_point_t));
        */
        
        for(group_t group : object.grp)
            for(smoothing_group_t sg : group.sg)
                for(feather::FFace f : sg.f)
                    for(feather::FFacePoint fp : f)
                        file.write((char*)&fp, sizeof(feather::FFacePoint));

        ss.seekp(0);
        ss.clear();
        ss << "</data>";
        file.write(ss.str().c_str(),ss.tellp());
        ss.seekp(0);
        file.close();

        std::cout << "created " << object.o << ".mesh\n";
    }    

    std::cout << "\nMesh Extraction Complete!\n\n";
    return true;
}

bool io::write_camera_data(std::string filename, unsigned int uid)
{
    std::fstream file;
    file.open(filename.c_str(),std::ios::out|std::ios::binary);

    std::stringstream ss;

    // get all the fields from the time node
    typedef feather::field::Field<feather::FReal>* pReal;

    // TIME NODE FIELDS
    pReal stimeOut = static_cast<pReal>(feather::plugin::get_node_field_base(1,5));
    pReal etimeOut = static_cast<pReal>(feather::plugin::get_node_field_base(1,6));
    pReal fpsOut = static_cast<pReal>(feather::plugin::get_node_field_base(1,8));
    pReal cposIn = static_cast<pReal>(feather::plugin::get_node_field_base(1,3));
    
    // CAMERA NODE FIELDS
    pReal txOut = static_cast<pReal>(feather::plugin::get_node_field_base(uid,214));
    pReal tyOut = static_cast<pReal>(feather::plugin::get_node_field_base(uid,215));
    pReal tzOut = static_cast<pReal>(feather::plugin::get_node_field_base(uid,216));
    pReal rxOut = static_cast<pReal>(feather::plugin::get_node_field_base(uid,217));
    pReal ryOut = static_cast<pReal>(feather::plugin::get_node_field_base(uid,218));
    pReal rzOut = static_cast<pReal>(feather::plugin::get_node_field_base(uid,219));
    pReal fovIn = static_cast<pReal>(feather::plugin::get_node_field_base(uid,2));
    pReal nearIn = static_cast<pReal>(feather::plugin::get_node_field_base(uid,3));
    pReal farIn = static_cast<pReal>(feather::plugin::get_node_field_base(uid,4));
 
    feather::FReal stime = stimeOut->value;
    feather::FReal etime = etimeOut->value;
    feather::FReal cpos = stime;
    feather::FReal framestep = 1.0/fpsOut->value;

    std::cout
        << "Exporting camera data for stime " << stime
        << " to etime " << etime
        << " cpos "  << cpos
        << " fps=" << fpsOut->value
        << " framestep=" << framestep
        << std::endl
        ;

    // write header
    ss << "1.0\n";
    ss << stime << "," << etime << "," << fpsOut->value << "\n";

    while ( cpos >= stime && cpos <= etime){
        cposIn->value = cpos;
        cposIn->update = true; // sg values will not update without this
        feather::plugin::update();
        std::cout
            << "EXPORTING CAMERA DATA - uid:" << uid
            << " cpos:" << cpos
            << " tx:" << txOut->value
            << " ty:" << tyOut->value
            << " tz:" << tzOut->value
            << std::endl
            ;

        ss << cpos 
            << "," << txOut->value
            << "," << tyOut->value
            << "," << tzOut->value
            << "," << rxOut->value
            << "," << ryOut->value
            << "," << rzOut->value
            << "," << fovIn->value
            << "," << nearIn->value
            << "," << farIn->value
            << "\n"
            ;

        cpos += framestep;   
    } 


    // reset to start frame and write data for each frame


    file.write(ss.str().c_str(),ss.tellp());
    //ss.seekp(0);
    file.close();

    return true;
}

bool io::write_obj(std::string filename, obj_data_t& data)
{
    std::stringstream ss;

    std::fstream file;
    file.open(filename.c_str(),std::ios::out|std::ios::binary);

    ss << "# Exported obj from mesh\n";

    // write the mtllib
    for(std::string mtllib : data.mtllib) 
        ss << "mtllib " << mtllib << std::endl;

    // for each object
    for(object_t obj : data.object)
    {
        // object name
        ss << "o " << obj.o << std::endl;
        
        // node info
        ss << "# " << obj.mesh.v.size() << " vertics\n";

        // print vertexs
        for(feather::FVertex3D v : obj.mesh.v)
            ss << "v " << v.x << " " << v.y << " " << v.z << std::endl;

        // print tex coords
        for(feather::FTextureCoord vt : obj.mesh.st)
            ss << "vt " << vt.s << " " << vt.t << std::endl;

        // print normals
        for(feather::FVertex3D vn : obj.mesh.vn)
            ss << "vn " << vn.x << " " << vn.y << " " << vn.z << std::endl;

    }

    file.write(ss.str().c_str(),ss.tellp());
    ss.seekp(0);
    file.close();

    return true;
}

feather::status io::export_ply(std::string path, bool selected, bool animation, int sframe, int eframe)
{
    feather::status p;
    std::vector<unsigned int> uids;

    if(selected){
        // only export selected shapes
        uids = feather::plugin::get_selected_nodes();
    } else {
        // export all polygon shape nodes
        feather::plugin::get_nodes(uids);
    }

    // get the time node
    typedef feather::field::Field<feather::FReal>* RealType;
    RealType ctime = static_cast<RealType>(feather::plugin::get_node_field_base(1,3));
    RealType fps = static_cast<RealType>(feather::plugin::get_node_field_base(1,4));
     
    // setup the animation if animation is set
    if(animation) {
        std::cout << "EXPORTING ANIMATED PLYS\n";
    
        while ( sframe <= eframe ){
            std::cout << "EXPORTING ANIMATED PLYS FRAME:" << sframe << std::endl;
            ctime->value = ( 1.0 / fps->value ) * sframe;
            ctime->update = true;
            feather::plugin::update();

            for(auto uid : uids){
                std::cout << "uid:" << uid << " type:" << feather::plugin::get_node_id(uid,p) << std::endl;
                // for now we are only going to export the mesh out from the shape node
                if(feather::plugin::get_node_id(uid,p)==320){
                    typedef feather::field::Field<feather::FMesh>* MeshType;
                    MeshType mesh = static_cast<MeshType>(feather::plugin::get_field_base(uid,3));
                    std::string name;
                    feather::plugin::get_node_name(uid,name,p);
                    std::cout << "exporting uid:" << uid << " name:" << name << " to path:" << path << std::endl;
                    std::stringstream filename;
                    filename << name << "." << sframe;
                    bool pass = io::write_ply(path,filename.str(),&mesh->value);
                    if(!pass) {
                        std::stringstream ss;
                        ss << "Failed to export " << name << " to ply format.";
                        std::cout << ss.str() << std::endl;
                        return feather::status(feather::FAILED,ss.str().c_str());
                    }
                }
            }

            sframe++;
        } 
    } else {
        std::cout << "EXPORTING PLYS\n";
 
        for(auto uid : uids){
            std::cout << "uid:" << uid << " type:" << feather::plugin::get_node_id(uid,p) << std::endl;
            // for now we are only going to export the mesh out from the shape node
            if(feather::plugin::get_node_id(uid,p)==320){
                typedef feather::field::Field<feather::FMesh>* MeshType;
                MeshType mesh = static_cast<MeshType>(feather::plugin::get_field_base(uid,3));
                std::string name;
                feather::plugin::get_node_name(uid,name,p);
                std::cout << "exporting uid:" << uid << " name:" << name << " to path:" << path << std::endl;
                bool pass = io::write_ply(path,name,&mesh->value);
                if(!pass) {
                    std::stringstream ss;
                    ss << "Failed to export " << name << " to ply format.";
                    std::cout << ss.str() << std::endl;
                    return feather::status(feather::FAILED,ss.str().c_str());
                }
            }
        }

    }

    return p;
}

bool io::write_ply(std::string path, std::string name, feather::FMesh* mesh)
{
    std::fstream file;
    std::stringstream filepath;
    filepath << path << name << ".ply";
    file.open(filepath.str().c_str(),std::ios::out|std::ios::binary);

    std::stringstream ss;
    std::stringstream sv;
    std::stringstream sf;

    unsigned int cfp = 0;
    for(auto f : mesh->f){
        unsigned int fpcount = 0;
            for(auto fp : f){
                sv << mesh->v.at(fp.v).x << " " 
                    << mesh->v.at(fp.v).y << " " 
                    << mesh->v.at(fp.v).z << " " 
                    ;
                if(mesh->vn.size()){
                    sv << mesh->vn.at(fp.vn).x << " " 
                        << mesh->vn.at(fp.vn).y << " " 
                        << mesh->vn.at(fp.vn).z << " "
                        ; 
                }

                if(mesh->st.size()>fp.vt && mesh->st.size() > 0){
                    sv << mesh->st.at(fp.vt).s << " " 
                        << mesh->st.at(fp.vt).t
                        ;
                } 
                else if(mesh->st.size()<=fp.vt && mesh->st.size() != 0){
                    sv << "0 0";
                }
                sv << std::endl;
                ++fpcount;
            }
        sf << fpcount;
        for(unsigned int i=0; i < fpcount; i++){
            sf << " " << cfp;
            ++cfp;
        }
        sf << std::endl;
    }

    // write file
    ss << "ply\n"
        << "format ascii 1.0\n"
        << "comment Created by Feather3D\n"
        << "element vertex " << cfp << std::endl
        << "property float x\n"
        << "property float y\n"
        << "property float z\n"
        ;

    if(mesh->vn.size()){
        ss << "property float nx\n"
            << "property float ny\n"
            << "property float nz\n"
            ;
    }

    if(mesh->st.size()){
        ss << "property float s\n"
            << "property float t\n"
            ;
    }

    ss << "element face " << mesh->f.size() << std::endl
        << "property list uchar uint vertex_indices\n"
        << "end_header\n"
        << sv.str()
        << sf.str()
        ; 

    file.write(ss.str().c_str(),ss.tellp());
    ss.seekp(0);
    file.close();

    return true;
}

template <>
feather::status io::file<io::IMPORT,io::OBJ>(obj_data_t& data, std::string filename)
{
    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef io::parsing::skipper_grammar<iterator_type> skipper_type;
    typedef io::parsing::obj_parser<iterator_type, skipper_type> obj_parser;

    // add std::ios::binary flag to read binary data
    std::ifstream fs(filename.c_str(), std::ios::in|std::ios::ate);
    long size;
    char* buffer;
    std::string input;

    if(fs.is_open())
    {
        size = fs.tellg();
        buffer = new char[size+1];
        fs.seekg(0, std::ios::beg);
        fs.read(buffer, size);
        buffer[size] = '\0';
        input = buffer;
        fs.close();

        obj_parser g; // our grammar

        std::string::const_iterator iter = input.begin();
        std::string::const_iterator end = input.end();

        bool r = phrase_parse(iter, end, g, skipper_type(), data);

        delete[] buffer;

        if(r && iter == end)
        {
            std::cout << "mesh parsed\n"
                << "\tv size: " << data.object.at(0).mesh.v.size() << std::endl
                << "\tv size: " << data.object.at(0).mesh.st.size() << std::endl
                << "\tv size: " << data.object.at(0).mesh.vn.size() << std::endl
                << "\tf size: " << data.object.at(0).grp.at(0).sg.at(0).f.size() << std::endl;
  
            // we need to fix the vertex indices in the faces since they are numbered different in the file then what we'll be using them in the object class
            std::cout << "fixing face indices\n";

            int v_count = 1;
            int vt_count = 1;
            int vn_count = 1;

            for(uint i=0; i < data.object.size(); i++)
            {
                // for each group
                for(uint j=0; j < data.object[i].grp.size(); j++)
                {
                    for(uint k=0; k < data.object[i].grp[j].sg.size(); k++)
                    {
                        // for eash face
                        for(uint l=0; l < data.object[i].grp[j].sg[k].f.size(); l++)
                        {
                            for(uint m=0; m < data.object[i].grp[j].sg[k].f[l].size(); m++)
                            {
                                data.object[i].grp[j].sg[k].f[l][m].v = data.object[i].grp[j].sg[k].f[l][m].v-v_count;
                                if(data.object[i].mesh.st.size() > 0)
                                    data.object[i].grp[j].sg[k].f[l][m].vt = data.object[i].grp[j].sg[k].f[l][m].vt-vt_count;
                                if(data.object[i].mesh.vn.size() > 0)
                                    data.object[i].grp[j].sg[k].f[l][m].vn = data.object[i].grp[j].sg[k].f[l][m].vn-vn_count;
                            }
                        }
                    }
                }

                v_count = v_count + data.object[i].mesh.v.size();
                vt_count = vt_count + data.object[i].mesh.st.size();
                vn_count = vn_count + data.object[i].mesh.vn.size();
                
                std::cout << "fixed " << data.object[i].o << " face indices\n";
            }
 
            std::cout << "finished fixing face indices\n";

            return feather::status();        
        }
        else
        {
            std::cout << "FAILED TO PARSE OBJ\n";
            return feather::status(feather::FAILED,"failed to parse");
        }
    }
    else
    {
        std::cout << "error loading \"" << filename << "\" obj file\n";
        return feather::status(feather::FAILED,"loading error");
    }

    std::cout << "parsing error in \"" << filename << "\" obj file\n";

    return feather::status(feather::FAILED,"parsing error");
}
