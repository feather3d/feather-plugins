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
        enum Command { N=0, OPEN_FEATHER, SAVE_FEATHER, IMPORT_OBJ, EXPORT_OBJ };

        // open feather file
        status open_feather(parameter::ParameterList params) {
            std::string filename;
            bool p = false;
            p = params.getParameterValue<std::string>("filename",filename); 
            std::cout << "open feather filename:" << filename << std::endl; 
            return status();
        };

        // save feather file 
        status save_feather(parameter::ParameterList params) {
            std::string filename;
            bool p = false;
            p = params.getParameterValue<std::string>("filename",filename); 
            
            std::fstream file;
            file.open(filename,std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);

            std::stringstream data;

            data << "<header>";
            data << 0 << 1; // version number
            // get time node
            unsigned int uid;
            feather::scenegraph::get_node_by_name("time",uid);            
            io::save_feather(filename);
 
            // we already know the fid's for the values
            double stime = static_cast<feather::field::Field<double>*>(feather::scenegraph::get_fieldBase(uid,7))->value;
            double etime = static_cast<feather::field::Field<double>*>(feather::scenegraph::get_fieldBase(uid,8))->value;
            double ctime = static_cast<feather::field::Field<double>*>(feather::scenegraph::get_fieldBase(uid,9))->value;
            double fps = static_cast<feather::field::Field<double>*>(feather::scenegraph::get_fieldBase(uid,10))->value;
            data << stime << etime << ctime << fps;
            // header data
            data << "<plugins>";
            // plugin names
            data << "</plugins>";
            data << "<nodes>";
            // plugin names
            data << "</nodes>";
            data << "</header>";
            data << "<data>";
            // data
            data << "</data>";
            data << "<conn>";
            // connections
            data << "</conn>";

            std::istreambuf_iterator<char> src(data.rdbuf());
            std::istreambuf_iterator<char> end;
            std::ostreambuf_iterator<char> dest(file);
            std::copy(src,end,dest);

            file.close();

            return status();
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

            return s;
        };

        // export obj file
        status export_obj(parameter::ParameterList params) {
            std::cout << "running export_obj command" << std::endl;
            return status();
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

// Export Obj Command
ADD_COMMAND("export_obj",EXPORT_OBJ,export_obj)

INIT_COMMAND_CALLS(EXPORT_OBJ)
