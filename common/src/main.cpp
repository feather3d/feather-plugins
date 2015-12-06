/***********************************************************************
 *
 * Filename: main.cpp
 *
 * Description: Holds common nodes and commands. 
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


/*
 ***************************************
 *           C PLUGIN WRAPPER          *
 ***************************************
 *
 * C_PLUGIN_WRAPPER()
 * 
 * This macro is needed to make it callable
 * by the core.
 * Every plugin needs it and it's always
 * the same so you should never have to
 * edit the below code.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif
    C_PLUGIN_WRAPPER()
#ifdef __cplusplus
}
#endif

using namespace feather;

// Define all of our nodes
// with id's here.
#define EMPTY 1
#define SHAPE 2
#define TRANSFORM 3

PLUGIN_INIT("Common","Commonly used nodes and commands","Richard Layman",EMPTY,TRANSFORM)

/*
 ***************************************
 *               NONE                  *
 ***************************************
*/

// parent
ADD_FIELD_TO_NODE(EMPTY,FNode,field::Node,field::connection::In,FNode(),1)
// child
ADD_FIELD_TO_NODE(EMPTY,FNode,field::Node,field::connection::Out,FNode(),2)


namespace feather
{
    DO_IT(EMPTY)
    { 
        return status();
    };
} // namespace feather

NODE_INIT(EMPTY,node::Empty,"node_empty.svg")


/*
 ***************************************
 *               SHAPE                 *
 ***************************************
*/

ADD_FIELD_TO_NODE(SHAPE,FMesh,field::Mesh,field::connection::In,FMesh(),200)
ADD_FIELD_TO_NODE(SHAPE,FMesh,field::Mesh,field::connection::Out,FMesh(),201)


namespace feather
{
    DO_IT(SHAPE)
    { 
        //ShapeFields* shape = static_cast<ShapeFields*>(fields);
        //std::cout << "shape: meshIn:" << shape->meshIn << std::endl;

        return status();
    };
} // namespace feather

NODE_INIT(SHAPE,node::Object,"")


/*
 ***************************************
 *              TRANSFORM              *
 ***************************************
*/

// parent
ADD_FIELD_TO_NODE(TRANSFORM,FNode,field::Node,field::connection::In,FNode(),1)
// child
ADD_FIELD_TO_NODE(TRANSFORM,FNode,field::Node,field::connection::Out,FNode(),2)
// LOCAL POSITION IN
// tX 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::In,0,3)
// tY 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::In,0,4)
// tZ 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::In,0,5)
// rX 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::In,0,6)
// rY 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::In,0,7)
// rZ 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::In,0,8)
// sX 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::In,1,9)
// sY 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::In,1,10)
// sZ 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::In,1,11)
// LOCAL POSITION OUT
// tX 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,0,12)
// tY 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,0,13)
// tZ 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,0,14)
// rX
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,0,15)
// rY 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,0,16)
// rZ 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,0,17)
// sX 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,1,18)
// sY 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,1,19)
// sZ 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,1,20)
// WORLD POSITION OUT 
// tX 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,0,21)
// tY 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,0,22)
// tZ 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,0,23)
// rX
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,0,24)
// rY 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,0,25)
// rZ 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,0,26)
// sX 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,1,27)
// sY 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,1,28)
// sZ 
ADD_FIELD_TO_NODE(TRANSFORM,FDouble,field::Double,field::connection::Out,1,29)


namespace feather
{
    DO_IT(TRANSFORM) 
    {
        //TransformFields* xform = static_cast<TransformFields*>(fields);
        //std::cout << "xform: tx:" << xform->tx << std::endl;
        
        // Take the input and generate world coords using the parent node position

        return status();
    };
} // namespace feather

NODE_INIT(TRANSFORM,node::Manipulator,"transform.svg")


/*
 ***************************************
 *              COMMANDS               *
 ***************************************
 */

namespace feather
{

    namespace command
    {
        enum Command { N=0 };
    } // namespace command

} // namespace feather

INIT_COMMAND_CALLS(N)
