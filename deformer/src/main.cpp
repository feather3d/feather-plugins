/**********************************************************************
 *
 * Filename: main.cpp
 *
 * Description: Plugin contains deformer nodes and commands.
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
#include <feather/node.hpp>
#include <feather/parameter.hpp>
#include <feather/command.hpp>
#include <feather/render.hpp>
//#include <feather/scenegraph.hpp>
#include <feather/draw.hpp>
#include <feather/plugin.hpp>
#include <feather/tools.hpp>
#include <QColor>

#ifdef __cplusplus
extern "C" {
#endif
    C_PLUGIN_WRAPPER()
#ifdef __cplusplus
}
#endif

using namespace feather;

#define CLUSTER 440

PLUGIN_INIT("Deformer","Deformer nodes and commands","Richard Layman",CLUSTER,CLUSTER)


/*
 ***************************************
 *            CLUSTER                  *
 ***************************************
*/

// IN
// mesh 
ADD_FIELD_TO_NODE(CLUSTER,FMesh,field::Mesh,field::connection::In,FMesh(),1)
// ids 
ADD_FIELD_TO_NODE(CLUSTER,FIntArray,field::IntArray,field::connection::In,std::vector<FInt>(),2)
// weights 
ADD_FIELD_TO_NODE(CLUSTER,FRealArray,field::RealArray,field::connection::In,std::vector<FReal>(),3)
// OUT
// mesh 
ADD_FIELD_TO_NODE(CLUSTER,FMesh,field::Mesh,field::connection::Out,FMesh(),4)

namespace feather
{

    DO_IT(CLUSTER)
    {
        typedef field::Field<FMesh>*  MeshField;
        typedef field::Field<FIntArray>*  IntArrayField;
        typedef field::Field<FRealArray>*  RealArrayField;
        typedef field::Field<FMatrix4x4>*  MatrixField;
        
        MeshField meshIn=0;
        MeshField meshOut=0;
        IntArrayField idsIn=0;
        RealArrayField weightsIn=0;
        MatrixField localMatrixOut=0;

        for(auto f : fields){
            if(f->id == 1)
                meshIn = static_cast<MeshField>(f);
            if(f->id == 2)
                idsIn = static_cast<IntArrayField>(f);
            if(f->id == 3)
                weightsIn = static_cast<RealArrayField>(f);
            if(f->id == 4)
                meshOut = static_cast<MeshField>(f);
            if(f->id == 212)
                localMatrixOut = static_cast<MatrixField>(f);
         }

        // get mesh in
        if(meshIn->connected()) {
            field::Connection conn = meshIn->connections.at(0);
            meshIn->value = static_cast<MeshField>(plugin::get_field_base(conn.puid,conn.pnid,conn.pfid,0))->value;
        } else {
            // since this is a cluster, we won't bother going any further
            meshIn->update=false;
            idsIn->update=false;
            weightsIn->update=false;
            meshOut->update=false;
            return status();
        }

        // get ids
        if(idsIn->connected()) {
            field::Connection conn = idsIn->connections.at(0);
            idsIn->value = static_cast<IntArrayField>(plugin::get_field_base(conn.puid,conn.pnid,conn.pfid,0))->value;
        }

        // if there are no id's, there's no need to do any calculations; apply mesh and get out
        if(!idsIn->value.size()){
            meshOut->value = meshIn->value;
            meshIn->update=false;
            idsIn->update=false;
            weightsIn->update=false;
            meshOut->update=true;
            return status();
        }

        if(weightsIn->connected()) {
            field::Connection conn = weightsIn->connections.at(0);
            weightsIn->value = static_cast<RealArrayField>(plugin::get_field_base(conn.puid,conn.pnid,conn.pfid,0))->value;
        }

        //if(meshIn->update || idsIn->update || weightsIn->update)
        if(true)
        {
            std::cout << "CLUSTER UPDATE\n";

            std::cout << "mesh update=" << meshIn->update
                << " ids update=" << idsIn->update
                << " weights update=" << weightsIn->update
                ;


            // Weight Rules
            // If the weights array only has one value, we'll use that values for all.
            // If the weights array size doesn't match the id's size, we'll use 1.0
            // If there are no weights, we'll use 1.0

            // our first check will be for no weights
            if(!weightsIn->value.size() || weightsIn->value.size() != idsIn->value.size()) {
                // currently will offset each vertex in the id array by the input tranforms
                for(auto id : idsIn->value) {
                    tools::modify_vertex(1.0,&localMatrixOut->value,meshIn->value.v.at(id));
                } 
            } else {
                unsigned int wid=0;
                for(auto id : idsIn->value) {
                    tools::modify_vertex(weightsIn->value.at(wid),&localMatrixOut->value,meshIn->value.v.at(id));
                    wid++;
                } 
            }
 
            meshOut->value = meshIn->value;
            meshIn->update=false;
            idsIn->update=false;
            weightsIn->update=false;
            meshOut->update=true;
        }
 
        return status();
    };

    /*
    DRAW_IT(ANIMATION_KEY)
    {
        std::cout << "ANIMATION_KEY DRAW IT\n";
        ADD_SHADED_MESH(3)
        return status();    
    };
    */

} // namespace feather

NODE_INIT(CLUSTER,node::Deformer,"cluster.svg")


/*
 ***************************************
 *              COMMANDS               *
 ***************************************
*/

namespace feather
{
    namespace command
    {
        enum Command { N=0, ADD_CLUSTER };

        
        // TODO - need to finish this once component selection is possible
        // add key
        status add_cluster(parameter::ParameterList params) {
            // for now we'll make it so that every vertex is bound to the cluster
            status p;
            unsigned int uid = plugin::add_node(CLUSTER,"cluster0",p);
            // our first tests will be with cubes, so we'll add 3 ids leaving 1 not bound
            typedef field::Field<FIntArray>* intarray;
            intarray ids = static_cast<intarray>(plugin::get_field_base(uid,2));
            ids->value.push_back(0);
            ids->value.push_back(1);
            ids->value.push_back(2);

            // connect to the root for now
            plugin::connect(0,202,uid,201);
        
            // update scenegraph            
            plugin::update();

            return status();
        };

    } // namespace command

} // namespace feather

ADD_COMMAND("add_cluster",ADD_CLUSTER,add_cluster)
//ADD_PARAMETER(command::ADD_KEY,1,parameter::Real,"time")

INIT_COMMAND_CALLS(ADD_CLUSTER)
