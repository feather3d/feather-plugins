/**********************************************************************
 *
 * Filename: main.cpp
 *
 * Description: Plugin for importing and exporting obj files.
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

#include <feather/deps.hpp>
#include <feather/pluginmanager.hpp>
#include <feather/field.hpp>
#include <feather/draw.hpp>
#include <feather/node.hpp>
#include <feather/parameter.hpp>
#include <feather/command.hpp>
#include <feather/scenegraph.hpp>

//#include "obj.hpp"

#ifdef __cplusplus
extern "C" {
#endif
    C_PLUGIN_WRAPPER()
#ifdef __cplusplus
}
#endif

using namespace feather;

PLUGIN_INIT("Ply","Import and export ply format file data.","Richard Layman",0,0)


/*
 ***************************************
 *              COMMANDS               *
 ***************************************
 */

namespace feather
{
    namespace command
    {
        enum Command { N=0, EXPORT_PLY };

        status export_ply(parameter::ParameterList params) {
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
          
            std::cout << "Export Ply Mesh\n";
 
            /* 
            // load the mesh
            //mesh_t mesh;
            //p = obj::io::load_mesh(mesh,filename);
            //if(!p)
            //    return status(FAILED,"obj file failed to parse");

            //return status();
            obj_data_t data;
            s = obj::io::file<obj::io::IMPORT,obj::io::OBJ>(data,filename);


            // for each object in the data file, create a node
            // and connect it to the root for now
            int uid = 0;
            int vstep = 0;

            for_each(data.object.begin(), data.object.end(), [&uid,&vstep,&s] (object_t& objdata) {
                    // add the nodes to the scenegraph
                    uid = feather::scenegraph::add_node(320,objdata.o,s);
                    std::cout << "mesh uid:" << uid << std::endl;
                    // for now I'm just going to connect the root to the node 
                    feather::status p = feather::scenegraph::connect(0,2,uid,1);
                    if(p.state==feather::FAILED)
                        std::cout << p.msg << std::endl;

                    feather::scenegraph::update();

                    // get the mesh from the node and fill in it's values
                    typedef field::Field<feather::FMesh>* sourcefield;
                    sourcefield sf=NULL;

                    // NOTE: you can't call feather::sg[uid] from here - you will get a seg fault 
                    sf = static_cast<sourcefield>(feather::scenegraph::get_fieldBase(uid,320,3));
                    if(sf){
                        // only going to do the first object as a test
                        // fill in the mesh
                        sf->value.assign_v(objdata.mesh.v);
                        sf->value.assign_st(objdata.mesh.st);
                        sf->value.assign_vn(objdata.mesh.vn);
                        // obj v indexes are 1 based and need to be converted to 0 based
                        for_each(objdata.grp.at(0).sg.at(0).f.begin(), objdata.grp.at(0).sg.at(0).f.end(), [&vstep](feather::FFace& f){
                            //std::cout << "face with vstep:" << vstep << std::endl;
                            for_each(f.begin(), f.end(),[&vstep](feather::FFacePoint& fp){
                                fp.v=fp.v-(1+vstep);
                                fp.vn=fp.vn-(1+vstep);
                                //std::cout << "assigning vp:" << fp.v << " ";
                            });
                            //std::cout << std::endl;
                        });
                        sf->value.assign_f(objdata.grp.at(0).sg.at(0).f);
                        vstep += objdata.mesh.v.size();
                    }
                    else
                        std::cout << "NULL SOURCE FIELD\n";

                    //std::cout << "added " << uid << " to the uid_update state which is " << feather::cstate.uid_update.size() << std::endl;
                    
                    //feather::scenegraph::update();
            });

            */

            return s;
        };

    } // namespace command

} // namespace feather

// Export Ply Command
ADD_COMMAND("export_ply",EXPORT_PLY,export_ply)

ADD_PARAMETER(command::EXPORT_PLY,1,parameter::String,"filename")

ADD_PARAMETER(command::EXPORT_PLY,2,parameter::Bool,"selection")

INIT_COMMAND_CALLS(EXPORT_PLY)
