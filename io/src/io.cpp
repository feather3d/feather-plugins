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

                if(mesh->st.size()){
                    sv << mesh->st.at(fp.vt).s << " " 
                        << mesh->st.at(fp.vt).t
                        ;
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

    // Had originally tryed using assimp to export the data but it kept seq faulting
    // and didn't work correctly, so I just decided to do it manually since it's
    // easier and probably faster

    // EXPORT FORMAT's
    /*
       Export Format Id's
       [0]: dae - COLLADA - Digital Asset Exchange Schema
       [1]: x - X Files
       [2]: stp - Step Files
       [3]: obj - Wavefront OBJ format
       [4]: stl - Stereolithography
       [5]: stl - Stereolithography (binary)
       [6]: ply - Stanford Polygon Library
       [7]: ply - Stanford Polygon Library (binary)
       [8]: 3ds - Autodesk 3DS (legacy)
       [9]: assbin - Assimp Binary
       [10]: assxml - Assxml Document
    */

    /*
    Assimp::Exporter exporter;
    const aiExportFormatDesc* format = exporter.GetExportFormatDescription(6);

    aiScene mscene;
    mscene.mFlags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;
    mscene.mRootNode = new aiNode(name.c_str());
    aiMesh* nodeMeshes[1];
    mscene.mMeshes = nodeMeshes;
    aiMesh* nodeMesh = new aiMesh();
    mscene.mNumMeshes = 1;
    mscene.mMeshes[0] = nodeMesh;
    mscene.mNumMaterials = 1;
    mscene.mRootNode->mNumMeshes = 1;
    unsigned int meshes[1] = {1};
    mscene.mRootNode->mMeshes = meshes;
    mscene.mMaterials = new aiMaterial*[1];
    mscene.mMaterials[0] = nullptr;
    mscene.mMaterials[0] = new aiMaterial();

    // build Mesh
    nodeMesh->mName = name.c_str();
    nodeMesh->mNumVertices = mesh->v.size();
    aiVector3D vertex[mesh->v.size()];
    nodeMesh->mVertices = vertex;
    int i=0;
    for(auto v : mesh->v){
        std::cout << "vertex count:" << mesh->v.size() << std::endl;
        std::cout << "loading vertex x:" << v.x << " y:" << v.y << " z:" << v.z << std::endl;
        nodeMesh->mVertices[i] = aiVector3t<float>(v.x,v.y,v.z);
        i++;
    }
    std::cout << "applied vertex data\n";
    // go through each face and extract it's vertex, normal and st data
    i=0;
    nodeMesh->mNumFaces = mesh->f.size();
    aiFace faces[mesh->f.size()];
    nodeMesh->mFaces = faces;
    for(auto face : mesh->f){
        nodeMesh->mFaces[i].mNumIndices = face.size();
        unsigned int indices[face.size()];
        nodeMesh->mFaces[i].mIndices = indices;
        int j=0;
        for(auto fp : face){
            nodeMesh->mFaces[i].mIndices[j] = fp.v;
            j++;
        }
        i++;
    }
    std::cout << "faces loaded\n";
    std::cout << "exporting mesh" << std::endl;
    printAssimp(&mscene);

    std::stringstream file;
    file << path << name << ".ply"; //filename << name << ".ply";
    std::cout << "exporting " << file.str() << std::endl;
    exporter.Export(&mscene,format->id,file.str().c_str(),mscene.mFlags);
    */
 
}

void io::printAssimp(aiScene* scene)
{
    std::cout << "ASSIMP SCENE CONTENTS\n=====START=====\n";
    std::cout << "root mesh count:" << scene->mNumMeshes << std::endl;
    for(int i=0; i < scene->mNumMeshes; i++){
        std::cout << "MESH DATA\n";
        std::cout << "v count:" << scene->mMeshes[i]->mNumVertices << std::endl;
        std::cout << "f count:" << scene->mMeshes[i]->mNumFaces << std::endl;
        for(int j=0; j < scene->mMeshes[i]->mNumFaces; j++){
            std::cout << "\ti count:" << scene->mMeshes[i]->mFaces[j].mNumIndices << std::endl;
        }
    }
    std::cout << "=====END=====\n";
}


template <>
feather::status io::file<io::IMPORT,io::OBJ>(obj_data_t& data, std::string filename)
{

    // import obj file
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filename.c_str(),
            aiProcess_CalcTangentSpace          |
            //aiProcess_Triangulate               |
            aiProcess_JoinIdenticalVertices     |
            aiProcess_SortByPType);

    // if import failed
    if(!scene){
        std::cout << "failed to load " << filename << std::endl;
        return feather::status(feather::FAILED,"Error! file failed to load");
    }

    // get number of meshes
    const int mcount = scene->mNumMeshes;
    //std::cout << "MESH COUNT " << mcount << std::endl;
 
    // transfer the data for each mesh into the obj data structure
    for ( int i=0; i < mcount; i++) {
        object_t obj;
        obj.o = scene->mMeshes[i]->mName.C_Str();
        // vertices and normals
        int vcount = scene->mMeshes[i]->mNumVertices;
        //std::cout << "VCOUNT for " << obj.o << " = " << vcount << std::endl;
        for( int j=0; j < vcount; j++){
            //std::cout << "IMPORTING x:" << scene->mMeshes[i]->mVertices[j][0] << " y:" << scene->mMeshes[i]->mVertices[j][1] << " z:" << scene->mMeshes[i]->mVertices[j][2] << std::endl;
            obj.mesh.v.push_back(feather::FVertex3D(scene->mMeshes[i]->mVertices[j][0], scene->mMeshes[i]->mVertices[j][1], scene->mMeshes[i]->mVertices[j][2]));
            obj.mesh.vn.push_back(feather::FVertex3D(scene->mMeshes[i]->mNormals[j][0], scene->mMeshes[i]->mNormals[j][1], scene->mMeshes[i]->mNormals[j][2]));
        }
        // st 
        int stcount = scene->mMeshes[i]->mNumUVComponents[2];
        //std::cout << "STCOUNT for " << obj.o << " = " << stcount << std::endl;

        // TODO
        // I need to add support for UV mapping
        for( int j=0; j < stcount; j++){
            //obj.mesh.st.push_back(feather::FVertex3D(scene->mMeshes[i]->mVertices[j][0], scene->mMeshes[i]->mVertices[j][1], scene->mMeshes[i]->mVertices[j][2]));
        }
        // faces
        int fcount = scene->mMeshes[i]->mNumFaces;
        //std::cout << "FCOUNT for " << obj.o << " = " << fcount << std::endl;
        smoothing_group_t sgroup;
        sgroup.s = 0; // smoothing
        for( int j=0; j < fcount; j++){
            feather::FFace face;
            int icount = scene->mMeshes[i]->mFaces->mNumIndices;
            //std::cout << "face i count=" << icount << "\nface data" << std::endl;
            for( int k=0; k < icount; k++){
                //std::cout << scene->mMeshes[i]->mFaces[j].mIndices[k] << ",";
                feather::FFacePoint fp = feather::FFacePoint(scene->mMeshes[i]->mFaces[j].mIndices[k],0,scene->mMeshes[i]->mFaces[j].mIndices[k]);
                face.push_back(fp); 
            }
            sgroup.f.push_back(face);
            //std::cout << std::endl;
        }
        group_t group;
        group.sg.push_back(sgroup);
        obj.grp.push_back(group); 
        data.object.push_back(obj);
    } 


    std::cout << "parsing error in \"" << filename << "\" obj file\n";

    return feather::status();
}
