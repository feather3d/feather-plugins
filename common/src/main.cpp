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
#include <feather/render.hpp>
#include <feather/attribute.hpp>
#include <feather/parameter.hpp>
#include <feather/command.hpp>
#include <feather/draw.hpp>
#include <feather/tools.hpp>
#include <feather/plugin.hpp>

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

#define COMMON_PLUGIN_ID 1

// Define all of our nodes
// with id's here.
#define EMPTY 1
#define CAMERA 2
#define SHAPE 3
#define TIME 4
#define MULTIPLY 5
#define DIVIDE 6
#define RENDEROBJECT 7

PLUGIN_INIT(COMMON_PLUGIN_ID,"Common","Commonly used nodes and commands","Richard Layman",EMPTY,RENDEROBJECT)

/*
 ***************************************
 *               NONE                  *
 ***************************************
*/

// No Fields

namespace feather
{
    DO_IT(EMPTY)
    { 
        return status();
    };

    DRAW_IT(EMPTY)
    {
        std::cout << "EMPTY DRAW IT\n";
        //ADD_LINE(FVertex3D(0.0,0.0,0.0),FVertex3D(2.0,2.0,2.0),FColorRGB(1.0,1.0,0.0),draw::Line::Solid)
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

// type
// 0 = Perspective
// 1 = Orthographic 
ADD_FIELD_TO_NODE(CAMERA,FInt,field::Int,field::connection::In,0,1)
// fov 
ADD_FIELD_TO_NODE(CAMERA,FReal,field::Real,field::connection::In,45.0,2)
// near plane 
ADD_FIELD_TO_NODE(CAMERA,FReal,field::Real,field::connection::In,0.1,3)
// far plane 
ADD_FIELD_TO_NODE(CAMERA,FReal,field::Real,field::connection::In,100.0,4)

namespace feather
{
    DO_IT(CAMERA)
    { 
        return status();
    };

    DRAW_IT(CAMERA)
    {
        std::cout << "CAMERA DRAW IT\n";
        ADD_PERSP_CAMERA(1,2,3,4)
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

ADD_FIELD_TO_NODE(SHAPE,FMesh,field::Mesh,field::connection::In,FMesh(),1)
ADD_FIELD_TO_NODE(SHAPE,FNode,field::Node,field::connection::In,FNode(),2)
ADD_FIELD_TO_NODE(SHAPE,FMesh,field::Mesh,field::connection::Out,FMesh(),3)

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

// IN FIELDS
// start time 
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::In,0,1)
// end time 
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::In,10,2)
// current time 
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::In,0,3)
// fps
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::In,24,4)
// OUT FIELDS
// start time 
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::Out,0,5)
// end time 
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::Out,0,6)
// current time 
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::Out,0,7)
// fps
ADD_FIELD_TO_NODE(TIME,FReal,field::Real,field::connection::Out,0,8)

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
            if(f->id == 1)
                stimeIn = static_cast<DField>(f);
            if(f->id == 2)
                etimeIn = static_cast<DField>(f);
            if(f->id == 3)
                cposIn = static_cast<DField>(f);
            if(f->id == 4)
                fpsIn = static_cast<DField>(f);
            if(f->id == 5)
                stimeOut = static_cast<DField>(f);
            if(f->id == 6)
                etimeOut = static_cast<DField>(f);
            if(f->id == 7)
                cposOut = static_cast<DField>(f);
            if(f->id == 8)
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

        std::cout << "LOOKING AT CPOSIN\n\tupdate:" << cposIn->update << std::endl;
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
 *               MULTIPLY              *
 ***************************************
*/

// IN FIELDS
// value 
ADD_FIELD_TO_NODE(MULTIPLY,FReal,field::Real,field::connection::In,0,1)
// OUT FIELDS
// multiplier 
ADD_FIELD_TO_NODE(MULTIPLY,FReal,field::Real,field::connection::In,1,2)
// OUT FIELDS
// value 
ADD_FIELD_TO_NODE(MULTIPLY,FReal,field::Real,field::connection::Out,0,3)

namespace feather
{
    DO_IT(MULTIPLY)
    {
        typedef field::Field<FReal>*  DField;

        DField valueIn=0;
        DField multiplierIn=0;
        DField valueOut=0;

        for(auto f : fields){
            if(f->id == 1)
                valueIn = static_cast<DField>(f);
            if(f->id == 2)
                multiplierIn = static_cast<DField>(f);
            if(f->id == 3)
                valueOut = static_cast<DField>(f);
        }

        // set the values that changed

        // value
        if(valueIn->update) {
            valueOut->value = valueIn->value * multiplierIn->value;
            valueIn->update = false;
        }

        return status();
    };

    /*
    DRAW_IT(MULTIPLY)
    {
    };
    */

    //DRAW_INIT(MULTIPLY)

} // namespace feather

NODE_INIT(MULTIPLY,node::Tool,"multiply.svg")


/*
 ***************************************
 *               DIVIDE                *
 ***************************************
*/

// IN FIELDS
// value 
ADD_FIELD_TO_NODE(DIVIDE,FReal,field::Real,field::connection::In,0,1)
// divider 
ADD_FIELD_TO_NODE(DIVIDE,FReal,field::Real,field::connection::In,1,2)
// OUT FIELDS
// value 
ADD_FIELD_TO_NODE(DIVIDE,FReal,field::Real,field::connection::Out,0,3)

namespace feather
{
    DO_IT(DIVIDE)
    {
        typedef field::Field<FReal>*  DField;

        DField valueIn=0;
        DField dividerIn=0;
        DField valueOut=0;

        for(auto f : fields){
            if(f->id == 1)
                valueIn = static_cast<DField>(f);
            if(f->id == 2)
                dividerIn = static_cast<DField>(f);
            if(f->id == 3)
                valueOut = static_cast<DField>(f);
        }

        // set the values that changed

        // value
        if(valueIn->update) {
            valueOut->value = (dividerIn->value != 0) ? 0 : valueIn->value / dividerIn->value;
            valueIn->update = false;
        }

        return status();
    };

    /*
    DRAW_IT(DIVIDE)
    {
    };
    */

    //DRAW_INIT(DIVIDE)

} // namespace feather

NODE_INIT(DIVIDE,node::Tool,"divide.svg")


/*
 ***************************************
 *           RENDEROBJECT              *
 ***************************************
*/

ADD_FIELD_TO_NODE(RENDEROBJECT,FMesh,field::Mesh,field::connection::In,FMesh(),1)
ADD_FIELD_TO_NODE(RENDEROBJECT,FNode,field::Node,field::connection::In,FNode(),2)


namespace feather
{
    DO_IT(RENDEROBJECT)
    { 
        //ShapeFields* shape = static_cast<ShapeFields*>(fields);
        //std::cout << "shape: meshIn:" << shape->meshIn << std::endl;

        return status();
    };
} // namespace feather

NODE_INIT(RENDEROBJECT,node::Object,"renderobject.svg")



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
