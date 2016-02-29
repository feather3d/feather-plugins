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
#include <feather/draw.hpp>

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
#define CAMERA 2
#define SHAPE 3
#define TIME 4
#define TRANSFORM 5

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

    DRAW_IT(EMPTY)
    {
        std::cout << "EMPTY DRAW IT\n";
        ADD_LINE(FVertex3D(0.0,0.0,0.0),FVertex3D(2.0,2.0,2.0),FColorRGB(1.0,1.0,0.0),draw::Line::Solid)
        return status();
    };

    //DRAW_INIT(EMPTY)

} // namespace feather

NODE_INIT(EMPTY,node::Empty,"node_empty.svg")


/*
 ***************************************
 *               CAMERA                *
 ***************************************
*/

// parent
ADD_FIELD_TO_NODE(CAMERA,FNode,field::Node,field::connection::In,FNode(),1)
// child
ADD_FIELD_TO_NODE(CAMERA,FNode,field::Node,field::connection::Out,FNode(),2)
// type 
ADD_FIELD_TO_NODE(CAMERA,FInt,field::Int,field::connection::In,0,3)
// fov 
ADD_FIELD_TO_NODE(CAMERA,FReal,field::Real,field::connection::In,45.0,4)

namespace feather
{
    DO_IT(CAMERA)
    { 
        return status();
    };

    DRAW_IT(CAMERA)
    {
        std::cout << "CAMERA DRAW IT\n";
        ADD_PERSP_CAMERA(4)
        return status();
    };

    //DRAW_INIT(EMPTY)

} // namespace feather

NODE_INIT(CAMERA,node::Camera,"node_camera.svg")


/*
 ***************************************
 *               SHAPE                 *
 ***************************************
*/
// parent
ADD_FIELD_TO_NODE(SHAPE,FNode,field::Node,field::connection::In,FNode(),1)
// child
ADD_FIELD_TO_NODE(SHAPE,FNode,field::Node,field::connection::Out,FNode(),2)
ADD_FIELD_TO_NODE(SHAPE,FMesh,field::Mesh,field::connection::In,FMesh(),3)
ADD_FIELD_TO_NODE(SHAPE,FMesh,field::Mesh,field::connection::Out,FMesh(),4)


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
 *               TIME                  *
 ***************************************
*/

// parent
ADD_FIELD_TO_NODE(TIME,FNode,field::Node,field::connection::In,FNode(),1)
// child
ADD_FIELD_TO_NODE(TIME,FNode,field::Node,field::connection::Out,FNode(),2)
// IN FIELDS
// start time
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::In,0,3)
// end time
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::In,10,4)
// current time 
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::In,0,5)
// fps
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::In,24,6)
// OUT FIELDS
// start time
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::Out,0,7)
// end time
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::Out,0,8)
// current time 
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::Out,0,9)
// fps
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::Out,0,10)

namespace feather
{
    DO_IT(TIME)
    {
        typedef field::Field<FReal>*  DField;

        DField stimeIn=0;
        DField etimeIn=0;
        DField cposIn=0;
        DField fpsIn=0;
        DField stimeOut=0;
        DField etimeOut=0;
        DField cposOut=0;
        DField fpsOut=0;

        for(auto f : fields){
            if(f->id == 3)
                stimeIn = static_cast<DField>(f);
            if(f->id == 4)
                etimeIn = static_cast<DField>(f);
            if(f->id == 5)
                cposIn = static_cast<DField>(f);
            if(f->id == 6)
                fpsIn = static_cast<DField>(f);
            if(f->id == 7)
                stimeOut = static_cast<DField>(f);
            if(f->id == 8)
                etimeOut = static_cast<DField>(f);
            if(f->id == 9)
                cposOut = static_cast<DField>(f);
            if(f->id == 10)
                fpsOut = static_cast<DField>(f);
        }

        // set the values that changed

        // stime
        if(stimeIn->update) {
            stimeOut->value = stimeIn->value;
            stimeIn->update = false;
        }

        // etime
        if(etimeIn->update) {
            etimeOut->value = etimeIn->value;
            etimeIn->update = false;
        }

        // cpos 
        if(cposIn->update) {
            cposOut->value = cposIn->value;
            cposIn->update = false;
            std::cout << "set cposOut to:" << cposOut->value << std::endl;
        }

        // fps 
        if(fpsIn->update) {
            fpsOut->value = fpsIn->value;
            fpsIn->update = false;
        }
        
        return status();
    };

    /*
    DRAW_IT(TIME)
    {
        std::cout << "TIME DRAW IT\n";
        ADD_LINE(FVertex3D(0.0,0.0,0.0),FVertex3D(2.0,2.0,2.0),FColorRGB(1.0,1.0,0.0),draw::Line::Solid)
        return status();
    };
    */

    //DRAW_INIT(TIME)

} // namespace feather

NODE_INIT(TIME,node::Time,"time.svg")


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
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::In,0,3)
// tY 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::In,0,4)
// tZ 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::In,0,5)
// rX 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::In,0,6)
// rY 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::In,0,7)
// rZ 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::In,0,8)
// sX 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::In,1,9)
// sY 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::In,1,10)
// sZ 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::In,1,11)
// LOCAL POSITION OUT
// tX 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,0,12)
// tY 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,0,13)
// tZ 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,0,14)
// rX
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,0,15)
// rY 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,0,16)
// rZ 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,0,17)
// sX 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,1,18)
// sY 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,1,19)
// sZ 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,1,20)
// WORLD POSITION OUT 
// tX 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,0,21)
// tY 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,0,22)
// tZ 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,0,23)
// rX
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,0,24)
// rY 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,0,25)
// rZ 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,0,26)
// sX 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,1,27)
// sY 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,1,28)
// sZ 
ADD_FIELD_TO_NODE(TRANSFORM,FReal,field::Real,field::connection::Out,1,29)


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
