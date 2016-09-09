/**********************************************************************
 *
 * Filename: main.cpp
 *
 * Description: Plugin contains common animation nodes and commands.
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
//#include <feather/scenegraph.hpp>
#include <feather/draw.hpp>
#include <feather/plugin.hpp>
#include <QColor>

#ifdef __cplusplus
extern "C" {
#endif
    C_PLUGIN_WRAPPER()
#ifdef __cplusplus
}
#endif

using namespace feather;

#define ANIMATION_INT_KEY 420
#define ANIMATION_REAL_KEY 421
#define ANIMATION_INT_KEYTRACK 425
#define ANIMATION_REAL_KEYTRACK 426

PLUGIN_INIT("Animation","Animation nodes and commands","Richard Layman",ANIMATION_INT_KEY,ANIMATION_REAL_KEYTRACK)


/*
 ***************************************
 *            ANIMATION INT KEY        *
 ***************************************
*/

// parent
ADD_FIELD_TO_NODE(ANIMATION_INT_KEY,FNode,field::Node,field::connection::In,FNode(),1)
// child
ADD_FIELD_TO_NODE(ANIMATION_INT_KEY,FNode,field::Node,field::connection::Out,FNode(),2)
// time 
ADD_FIELD_TO_NODE(ANIMATION_INT_KEY,FReal,field::Real,field::connection::In,0.0,3)
// value
ADD_FIELD_TO_NODE(ANIMATION_INT_KEY,FInt,field::Int,field::connection::In,0,4)
// in curve type 
ADD_FIELD_TO_NODE(ANIMATION_INT_KEY,FInt,field::Int,field::connection::In,0,5)
// out curve type 
ADD_FIELD_TO_NODE(ANIMATION_INT_KEY,FInt,field::Int,field::connection::In,0,6)
// add curve vectors

namespace feather
{

    DO_IT(ANIMATION_INT_KEY)
    { 
        return status();
    };

    /*
    DRAW_IT(ANIMATION_INT_KEY)
    {
        std::cout << "ANIMATION_KEY DRAW IT\n";
        ADD_MESH(3)
        return status();    
    };
    */

} // namespace feather

// TODO add icon path
NODE_INIT(ANIMATION_INT_KEY,node::Animation,"key.svg")


/*
 ***************************************
 *            ANIMATION REAL KEY       *
 ***************************************
*/

// parent
ADD_FIELD_TO_NODE(ANIMATION_REAL_KEY,FNode,field::Node,field::connection::In,FNode(),1)
// child
ADD_FIELD_TO_NODE(ANIMATION_REAL_KEY,FNode,field::Node,field::connection::Out,FNode(),2)
// time 
ADD_FIELD_TO_NODE(ANIMATION_REAL_KEY,FReal,field::Real,field::connection::In,0.0,3)
// value
ADD_FIELD_TO_NODE(ANIMATION_REAL_KEY,FReal,field::Real,field::connection::In,0.0,4)
// in curve type 
ADD_FIELD_TO_NODE(ANIMATION_REAL_KEY,FInt,field::Int,field::connection::In,0,5)
// out curve type 
ADD_FIELD_TO_NODE(ANIMATION_REAL_KEY,FInt,field::Int,field::connection::In,0,6)
// add curve vectors

namespace feather
{

    DO_IT(ANIMATION_REAL_KEY)
    { 
        return status();
    };

    /*
    DRAW_IT(ANIMATION_REAL_KEY)
    {
        std::cout << "ANIMATION_KEY DRAW IT\n";
        ADD_MESH(3)
        return status();    
    };
    */

} // namespace feather

// TODO add icon path
NODE_INIT(ANIMATION_REAL_KEY,node::Animation,"key.svg")



/*
 ***************************************
 *            ANIMATION INT TRACK      *
 ***************************************
*/
// parent
ADD_FIELD_TO_NODE(ANIMATION_INT_KEYTRACK,FNode,field::Node,field::connection::In,FNode(),1)
// child
ADD_FIELD_TO_NODE(ANIMATION_INT_KEYTRACK,FNode,field::Node,field::connection::Out,FNode(),2)
// time 
ADD_FIELD_TO_NODE(ANIMATION_INT_KEYTRACK,FReal,field::Real,field::connection::In,0.0,3)
// keys 
ADD_FIELD_TO_NODE(ANIMATION_INT_KEYTRACK,FNodeArray,field::NodeArray,field::connection::In,FNodeArray(),4)
// value 
ADD_FIELD_TO_NODE(ANIMATION_INT_KEYTRACK,FInt,field::Int,field::connection::Out,0.0,5)

namespace feather
{

    DO_IT(ANIMATION_INT_KEYTRACK)
    { 
        return status();
    };

} // namespace feather

// TODO add icon path
NODE_INIT(ANIMATION_INT_KEYTRACK,node::Animation,"track.svg")


/*
 ***************************************
 *            ANIMATION REAL TRACK     *
 ***************************************
*/
// parent
ADD_FIELD_TO_NODE(ANIMATION_REAL_KEYTRACK,FNode,field::Node,field::connection::In,FNode(),1)
// child
ADD_FIELD_TO_NODE(ANIMATION_REAL_KEYTRACK,FNode,field::Node,field::connection::Out,FNode(),2)
// time 
ADD_FIELD_TO_NODE(ANIMATION_REAL_KEYTRACK,FReal,field::Real,field::connection::In,0.0,3)
// keys 
ADD_FIELD_TO_NODE(ANIMATION_REAL_KEYTRACK,FNodeArray,field::NodeArray,field::connection::In,FNodeArray(),4)
// value 
ADD_FIELD_TO_NODE(ANIMATION_REAL_KEYTRACK,FReal,field::Real,field::connection::Out,0.0,5)

namespace feather
{

    DO_IT(ANIMATION_REAL_KEYTRACK)
    { 
        return status();
    };

} // namespace feather

// TODO add icon path
NODE_INIT(ANIMATION_REAL_KEYTRACK,node::Animation,"track.svg")



/*
 ***************************************
 *              COMMANDS               *
 ***************************************
*/

namespace feather
{
    namespace command
    {
        enum Command { N=0, ADD_KEY };

        // add key
        status add_key(parameter::ParameterList params) {
            double time; 
            bool pass = params.getParameterValue<double>("time",time);
            if(!pass)
                return status(FAILED,"time parameter failed");

            // add key to scenegraph
            status p;
            unsigned int uid = plugin::add_node(ANIMATION_INT_KEY,"key",p);

            // set the time of the key
            typedef field::Field<feather::FReal>* sourcefield;
            sourcefield sf=NULL;
            sf = static_cast<sourcefield>(feather::plugin::get_node_field_base(uid,3));
            sf->value = time;

            // connect to the root for now
            plugin::connect(0,2,uid,1);
        
            // update scenegraph            
            plugin::update();
            
            std::cout << "add_key command found\n";
 
            return status();
        };

    } // namespace command

} // namespace feather

ADD_COMMAND("add_key",ADD_KEY,add_key)
ADD_PARAMETER(command::ADD_KEY,1,parameter::Real,"time")

INIT_COMMAND_CALLS(ADD_KEY)
