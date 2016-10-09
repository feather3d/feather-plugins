/**********************************************************************
 *
 * Filename: main.cpp
 *
 * Description: Plugin for importing and exporting 3d formats.
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

#include <feather/deps.hpp>
#include <feather/pluginmanager.hpp>
#include <feather/field.hpp>
#include <feather/draw.hpp>
#include <feather/node.hpp>
#include <feather/parameter.hpp>
#include <feather/command.hpp>
#include <feather/scenegraph.hpp>
#include <feather/tools.hpp>
#include <feather/plugin.hpp>

#include "io.hpp"
#include "feather.hpp"

#ifdef __cplusplus
extern "C" {
#endif
    C_PLUGIN_WRAPPER()
#ifdef __cplusplus
}
#endif

using namespace feather;

PLUGIN_INIT("IO","Import and export different 3d file formats","Richard Layman",0,0)


/*
 ***************************************
 *              COMMANDS               *
 ***************************************
 */

namespace feather
{
    namespace command
    {
        enum Command { N=0, OPEN_FEATHER, SAVE_FEATHER, IMPORT_OBJ, EXPORT_CAMERA_DATA, EXPORT_OBJ, EXPORT_PLY };

        // open feather file
        status open_feather(parameter::ParameterList params) {
            std::string filename;
            bool p = false;
            p = params.getParameterValue<std::string>("filename",filename); 
            std::cout << "open feather filename:" << filename << std::endl; 

            io::feather_format::open(filename);
 
            return status();
        };

        // save feather file 
        status save_feather(parameter::ParameterList params) {
            std::string filename;
            bool p = false;
            p = params.getParameterValue<std::string>("filename",filename); 

            io::feather_format::save(filename);
            
            return status();
        };

        // this is used to tracking down import issues, will delete later
        void print_data(obj_data_t& data) {
            for_each(data.object.begin(), data.object.end(), [] (object_t& objdata) {
                std::cout << "MESH " << objdata.o << " DATA\n";
                std::cout << "V DATA\ncount:" << objdata.mesh.v.size() << std::endl;
                for(auto v : objdata.mesh.v)
                    std::cout << "\tx:" << v.x << " y:" << v.y << " z:" << v.z << std::endl; 
                std::cout << "ST DATA\ncount:" << objdata.mesh.st.size() << std::endl;
                for(auto st: objdata.mesh.st)
                    std::cout << "\ts:" << st.s << " t:" << st.t << std::endl; 
                std::cout << "VN DATA\ncount:" << objdata.mesh.vn.size() << std::endl;
                for(auto vn : objdata.mesh.vn)
                    std::cout << "\tx:" << vn.x << " y:" << vn.y << " z:" << vn.z << std::endl; 
                // FACE DATA
                std::cout << "FACE DATA\n" << objdata.grp.at(0).sg.at(0).f.size() << std::endl;
                for(auto g : objdata.grp){
                    std::cout << "\tGROUP DATA\n";
                    for(auto sg : g.sg){
                        std::cout << "\t\tSMOOTHING GROUP - count:" << sg.f.size() << std::endl;
                        for(auto f : sg.f){
                            std::cout << "INDEX COUNT: " << f.size() << " - ";
                            for(auto fp : f) {
                                std::cout << fp.v << "/" << fp.vt << "/" << fp.vn << " ";
                            }
                            std::cout << std::endl;
                        }
                    }
                } 
            }

            );
        };

        // import obj file
        status import_obj(parameter::ParameterList params) {
            feather::status s;
            std::string filename;
            std::vector<unsigned int> uids;
            bool selection=false;
            bool p=false;
            p = params.getParameterValue<std::string>("filename",filename);
            if(!p)
                return status(FAILED,"filename parameter failed");
            p = params.getParameterValue<bool>("selection",selection);
            if(!p)
                return status(FAILED,"selection parameter failed");
            
            // load the mesh
            //mesh_t mesh;
            //p = obj::io::load_mesh(mesh,filename);
            //if(!p)
            //    return status(FAILED,"obj file failed to parse");

            //return status();
            obj_data_t data;
            s = io::file<io::IMPORT,io::OBJ>(data,filename);


            // for each object in the data file, create a node
            // and connect it to the root for now
            unsigned int meshuid = 0;
            unsigned int shapeuid = 0;
            int vstep = 0;

            //print_data(data);

            for_each(data.object.begin(), data.object.end(), [&meshuid,&shapeuid,&vstep,&s] (object_t& objdata) {
                    // add the nodes to the scenegraph
                    meshuid = feather::scenegraph::add_node(324,objdata.o,s);
                    std::stringstream shapename;
                    shapename << objdata.o << "_shape";
                    shapeuid = feather::scenegraph::add_node(320,shapename.str(),s);
                    std::cout << "mesh uid:" << meshuid << std::endl;
                    // for now I'm just going to connect the root to the node 
                    feather::status p = feather::scenegraph::connect(0,202,meshuid,201);
                    if(p.state==feather::FAILED)
                        std::cout << p.msg << std::endl;

                    feather::scenegraph::update();

                    // get the mesh from the node and fill in it's values
                    typedef field::Field<feather::FMesh>* sourcefield;
                    sourcefield sf=NULL;

                    // NOTE: you can't call feather::sg[uid] from here - you will get a seg fault 
                    sf = static_cast<sourcefield>(feather::scenegraph::get_fieldBase(meshuid,324,1,0));
                    if(sf){
                        // only going to do the first object as a test
                        // fill in the mesh
                        sf->value.assign_v(objdata.mesh.v);
                        sf->value.assign_st(objdata.mesh.st);
                        sf->value.assign_vn(objdata.mesh.vn);
                        // assign all groups
                        int i = 0;
                        for(auto g : objdata.grp) {
                            int j = 0;
                            for(auto sg : g.sg) {
                                //sf->value.assign_f(objdata.grp.at(i).sg.at(j).f);
                                sf->value.f.insert(sf->value.f.end(),sg.f.begin(),sg.f.end());
                                j++;
                            }
                            i++;
                        }
                        vstep += objdata.mesh.v.size();
                    }
                    else
                        std::cout << "NULL SOURCE FIELD\n";

                    // connect the mesh node to the shape node
                    p = feather::scenegraph::connect(meshuid,202,shapeuid,201);
                    p = feather::scenegraph::connect(meshuid,2,shapeuid,1);

                    //std::cout << "added " << uid << " to the uid_update state which is " << feather::cstate.uid_update.size() << std::endl;
                    
                    //feather::scenegraph::update();
            });

            return s;
        };


        // export camera data file
        status export_camera_data(parameter::ParameterList params) {
            std::cout << "running export_camera_data command" << std::endl;

            std::string path;
            unsigned int uid;

            bool p = params.getParameterValue<std::string>("path",path);
            if(!p)
                return status(FAILED,"path parameter failed");

            p = params.getParameterValue<unsigned int>("uid",uid);
            if(!p)
                return status(FAILED,"uid parameter failed");

            io::write_camera_data(path,uid); 
 
            return status();
        };

        // export obj file
        status export_obj(parameter::ParameterList params) {
            std::cout << "running export_obj command" << std::endl;
            return status();
        };

        // export obj file
        status export_ply(parameter::ParameterList params) {
            std::cout << "running export_ply command" << std::endl;

            std::string path;
            bool selection;
            bool animation;
            int sframe;
            int eframe;

            // path
            bool p = params.getParameterValue<std::string>("path",path);
            if(!p)
                return status(FAILED,"path parameter failed");

            // selection 
            p = params.getParameterValue<bool>("selection",selection);
            if(!p)
                return status(FAILED,"selection parameter failed");

            // animation 
            p = params.getParameterValue<bool>("animation",animation);
            if(!p)
                return status(FAILED,"animation parameter failed");

            // sframe
            p = params.getParameterValue<int>("sframe",sframe);
            if(!p)
                return status(FAILED,"sframe parameter failed");

            // eframe
            p = params.getParameterValue<int>("eframe",eframe);
            if(!p)
                return status(FAILED,"eframe parameter failed");

            //std::vector<unsigned int> uids = plugin::get_selected_nodes();
            //std::cout << "There are " << uids.size() << " nodes selected\n";
            status pass = io::export_ply(path,selection,animation,sframe,eframe);
            /*
            for(auto uid : uids){
                std::cout << "uid:" << uid << " type:" << plugin::get_node_id(uid,pass) << std::endl;
                // for now we are only going to export the mesh out from the shape node
                if(plugin::get_node_id(uid,pass)==320){
                    typedef field::Field<FMesh>* MeshType;
                    MeshType mesh = static_cast<MeshType>(plugin::get_field_base(uid,3));
                    std::string name;
                    status p;
                    plugin::get_node_name(uid,name,p);
                    io::write_ply(path,name,&mesh->value);
                }
            }
            */
            return pass;
        };

    } // namespace command

} // namespace feather

// Open Feather Command
ADD_COMMAND("open_feather",OPEN_FEATHER,open_feather)

ADD_PARAMETER(command::OPEN_FEATHER,1,parameter::String,"filename")

// Save Feather Command
ADD_COMMAND("save_feather",SAVE_FEATHER,save_feather)

ADD_PARAMETER(command::SAVE_FEATHER,1,parameter::String,"filename")

// Import Obj Command
ADD_COMMAND("import_obj",IMPORT_OBJ,import_obj)

ADD_PARAMETER(command::IMPORT_OBJ,1,parameter::String,"filename")

ADD_PARAMETER(command::IMPORT_OBJ,2,parameter::Bool,"selection")

// Export Camera Data Command
ADD_COMMAND("export_camera_data",EXPORT_CAMERA_DATA,export_camera_data)

ADD_PARAMETER(command::EXPORT_CAMERA_DATA,1,parameter::String,"path")

ADD_PARAMETER(command::EXPORT_CAMERA_DATA,2,parameter::Int,"uid")

// Export Obj Command
ADD_COMMAND("export_obj",EXPORT_OBJ,export_obj)

// Export Ply Command
ADD_COMMAND("export_ply",EXPORT_PLY,export_ply)

ADD_PARAMETER(command::EXPORT_PLY,1,parameter::String,"path")

ADD_PARAMETER(command::EXPORT_PLY,2,parameter::Bool,"selection")

ADD_PARAMETER(command::EXPORT_PLY,3,parameter::Bool,"animation")

ADD_PARAMETER(command::EXPORT_PLY,4,parameter::Int,"sframe")

ADD_PARAMETER(command::EXPORT_PLY,5,parameter::Int,"eframe")

INIT_COMMAND_CALLS(EXPORT_PLY)

