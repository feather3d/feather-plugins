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
