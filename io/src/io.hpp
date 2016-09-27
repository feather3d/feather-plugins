/***********************************************************************
 *
 * Filename: io.hpp
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

#ifndef IO_HPP
#define IO_HPP

#include <feather/types.hpp>
#include <feather/deps.hpp>
#include <feather/status.hpp>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Mesh Components

typedef struct {
    int s;
    std::vector<feather::FFace> f;
} smoothing_group_t;

typedef struct {
    std::string usemtl;
    std::vector<smoothing_group_t> sg; 
} group_t;

typedef struct{
    feather::FVertex3DArray v;
    feather::FTextureCoordArray st;
    feather::FVertex3DArray vn;
} mesh_t;

typedef struct {
    std::string o;
    mesh_t mesh;
    std::string g;
    std::vector<group_t> grp;
} object_t;

typedef struct {
    std::vector<std::string> mtllib;
    std::vector<object_t> object;
} obj_data_t;



    namespace io
    {

        enum FileType { Mesh, Shader, Group, Texture, Light, Camera, Global };
        enum Action { IMPORT, EXPORT };
        enum Format { OBJ, PLY };

        bool load_mesh(mesh_t& mesh, std::string path);
        bool write_mesh(obj_data_t& data);
        bool write_camera_data(std::string filename, unsigned int uid);
        bool write_obj(std::string filename, obj_data_t& data);
        bool write_ply(std::string filename, std::string name, feather::FMesh* meshes);

        template <int Action, int Format>
        feather::status file(obj_data_t& data, std::string filename="") { return feather::status(feather::FAILED,"unknown action or format"); };

        // specialization
        template <> feather::status file<IMPORT,OBJ>(obj_data_t& data, std::string filename);

    } // namespace io


#endif
