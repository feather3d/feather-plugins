/**********************************************************************
 *
 * Filename: main.cpp
 *
 * Description: Plugin for creating and working with polygon objects.
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
#include <feather/scenegraph.hpp>
#include <feather/draw.hpp>
#include <QColor>

#ifdef __cplusplus
extern "C" {
#endif
    C_PLUGIN_WRAPPER()
#ifdef __cplusplus
}
#endif

using namespace feather;

#define POLYGON_SHAPE 320
#define POLYGON_PLANE 321
#define POLYGON_CUBE 322

PLUGIN_INIT("Polygon","Polygon objects and tools","Richard Layman",POLYGON_SHAPE,POLYGON_CUBE)

/*
 ***************************************
 *            POLYGON SHAPE            *
 ***************************************
*/

// parent
ADD_FIELD_TO_NODE(POLYGON_SHAPE,FNode,field::Node,field::connection::In,FNode(),1)
// child
ADD_FIELD_TO_NODE(POLYGON_SHAPE,FNode,field::Node,field::connection::Out,FNode(),2)

// meshIn
ADD_FIELD_TO_NODE(POLYGON_SHAPE,FMesh,field::Mesh,field::connection::In,FMesh(),3)
// testIn
ADD_FIELD_TO_NODE(POLYGON_SHAPE,FReal,field::Real,field::connection::In,1,4)

namespace feather
{

    DO_IT(POLYGON_SHAPE)
    { 
        typedef field::Field<FMesh>*  MeshField;
        typedef field::Field<FReal>*  RealField;

        MeshField meshIn=0;
        RealField testIn=0;

        for(auto f : fields){
            if(f->id == 3)
                meshIn = static_cast<MeshField>(f);
            if(f->id == 4)
                testIn = static_cast<RealField>(f);
        }

        if(testIn->update)
        {
            std::cout << "SHAPE testIn ATTRIBUTES:\n"
                << "fid:" << testIn->id << std::endl
                << "update:" << testIn->update << std::endl
                //<< "puid:" << testIn->puid << std::endl
                //<< "pnid:" << testIn->pn << std::endl
                //<< "pfid:" << testIn->pf << std::endl
                << "connected:" << testIn->connected() << std::endl
                << "conn type:" << testIn->conn_type << std::endl
                << "type:" << testIn->type << std::endl;

            if(testIn->connected()) {
                field::Connection conn = testIn->connections.at(0);
                testIn->value = static_cast<RealField>(scenegraph::get_fieldBase(conn.puid,conn.pnid,conn.pfid))->value;
            }
            // this is for testing purposes
            // normally you would never modify the node's input
            for(int i=0; i < meshIn->value.v.size(); i++){
                meshIn->value.v.at(i).y += testIn->value;
            }
            /*
            for(auto v : meshIn->value.v){
                std::cout << "modifying y to:" << testIn->value << std::endl;
                v.y = testIn->value;
            }*/

            // TESTING
            /*
            for(auto v : meshIn->value.v){
                std::cout << "verifying that y is set to:" << v.y << std::endl;
            }
            */
        } else {
            std::cout << "test not set to update\n";
        }

        return status();
    };

    DRAW_IT(POLYGON_SHAPE)
    {
        std::cout << "POLYGON_SHAPE DRAW IT\n";
        ADD_MESH(3)
        return status();    
    };

} // namespace feather

NODE_INIT(POLYGON_SHAPE,node::Shape,"polyshape.svg")


/*
 ***************************************
 *            POLYGON PLANE            *
 ***************************************
*/
// parent
ADD_FIELD_TO_NODE(POLYGON_PLANE,FNode,field::Node,field::connection::In,FNode(),1)
// child
ADD_FIELD_TO_NODE(POLYGON_PLANE,FNode,field::Node,field::connection::Out,FNode(),2)
// subX
ADD_FIELD_TO_NODE(POLYGON_PLANE,FInt,field::Int,field::connection::In,2,3)
// subY
ADD_FIELD_TO_NODE(POLYGON_PLANE,FInt,field::Int,field::connection::In,2,4)
// meshOut
ADD_FIELD_TO_NODE(POLYGON_PLANE,FMesh,field::Mesh,field::connection::Out,FMesh(),5)


namespace feather
{

    DO_IT(POLYGON_PLANE)
    { 
        //field::FieldBase* f = feather::scenegraph::get_fieldBase(node.uid,320,3);

        //std::cout << "Polygon Plane - DO_IT()\n";
        typedef field::Field<FMesh>* MeshOut;

        MeshOut meshOut=nullptr;
        for(auto f : fields){
            if(f->id == 5)
                meshOut = static_cast<MeshOut>(f);
        }
        if(!meshOut) {
            //std::cout << "could not find meshOut\n";
            return status();
        }
        //MeshOut meshOut = static_cast<MeshOut>(fields.at(4));

        meshOut->value.v.clear();
        meshOut->value.vn.clear();

        meshOut->value.v.push_back(FVertex3D(1.0,1.0,1.0));
        meshOut->value.v.push_back(FVertex3D(1.0,-1.0,1.0));
        meshOut->value.v.push_back(FVertex3D(-1.0,-1.0,1.0));
        meshOut->value.v.push_back(FVertex3D(-1.0,1.0,1.0));
 
        meshOut->value.vn.push_back(FVertex3D(0.33,0.33,0.33));
        meshOut->value.vn.push_back(FVertex3D(0.33,-0.33,0.33));
        meshOut->value.vn.push_back(FVertex3D(-0.33,-0.33,0.33));
        meshOut->value.vn.push_back(FVertex3D(-0.33,0.33,0.33));
 
        FFace f;           
        // front face 
        f.push_back(FFacePoint(0,0,0));
        f.push_back(FFacePoint(1,0,1));
        f.push_back(FFacePoint(2,0,2));
        f.push_back(FFacePoint(3,0,3));
        meshOut->value.add_face(f);
        f.clear();

        return status();
    };

} // namespace feather

NODE_INIT(POLYGON_PLANE,node::Polygon,"polyplane.svg")


/*
 ***************************************
 *            POLYGON CUBE             *
 ***************************************
*/
// parent
ADD_FIELD_TO_NODE(POLYGON_CUBE,FNode,field::Node,field::connection::In,FNode(),1)
// child
ADD_FIELD_TO_NODE(POLYGON_CUBE,FNode,field::Node,field::connection::Out,FNode(),2)
ADD_FIELD_TO_NODE(POLYGON_CUBE,FInt,field::Int,field::connection::In,0,3)
ADD_FIELD_TO_NODE(POLYGON_CUBE,FInt,field::Int,field::connection::In,0,4)
ADD_FIELD_TO_NODE(POLYGON_CUBE,FInt,field::Int,field::connection::In,0,5)
ADD_FIELD_TO_NODE(POLYGON_CUBE,FMesh,field::Mesh,field::connection::Out,FMesh(),6)

namespace feather
{

    DO_IT(POLYGON_CUBE) 
    {
        typedef field::Field<FMesh>* MeshOut;
        typedef field::Field<int>* SubIn;

        MeshOut meshOut=nullptr;
        SubIn subX=0;
        SubIn subY=0;
        SubIn subZ=0;
        for(auto f : fields){
            if(f->id == 3)
                subX = static_cast<SubIn>(f);
            if(f->id == 4)
                subY = static_cast<SubIn>(f);
            if(f->id == 5)
                subZ = static_cast<SubIn>(f);
            if(f->id == 6)
                meshOut = static_cast<MeshOut>(f);
        }
        if(!meshOut) {
            //std::cout << "could not find meshOut\n";
            return status();
        }
        //typedef field::Field<FMesh>* MeshOut;
        //typedef field::Field<int>* SubIn;

        //MeshOut meshOut = static_cast<MeshOut>(fields.at(0));

        //SubIn subX = static_cast<SubIn>(fields.at(1));
        //SubIn subY = static_cast<SubIn>(fields.at(2));
        //SubIn subZ = static_cast<SubIn>(fields.at(3));

        //int fcount = (subX->value+1)*2 + (subY->value+1)*2 + (subZ->value+1)*2;
        //int vcount = fcount + 2;

        if(subX->update || subY->update || subZ->update)
        {
            // clear the mesh
            meshOut->value.v.clear();
            meshOut->value.vn.clear();

            // COMMENTED THIS OUT FOR THE TIME BEING TO GET RID OF WARNINGS
            // there are 3 side, each will be subdiv by it's sub axis value
            // the front and back of each axis are created
            /*
            int side = 0;
            float maxV = 1.0;
            float minV = -1.0;
            float spanX = 2.0;
            float spanY = 2.0;
            */

            /* 
            while(side < 3) {
                // front
                // create each face
                meshOut->value.v.push_back(FVertex3D(1.0,1.0,1.0);    
            }
            */

            if(!meshOut->value.v.size())
            {
                // VERTICS
                // Front 
                meshOut->value.v.push_back(FVertex3D(1.0,1.0,1.0));
                meshOut->value.v.push_back(FVertex3D(1.0,-1.0,1.0));
                meshOut->value.v.push_back(FVertex3D(-1.0,-1.0,1.0));
                meshOut->value.v.push_back(FVertex3D(-1.0,1.0,1.0));
                // Back 
                meshOut->value.v.push_back(FVertex3D(1.0,1.0,-1.0));
                meshOut->value.v.push_back(FVertex3D(-1.0,1.0,-1.0));
                meshOut->value.v.push_back(FVertex3D(-1.0,-1.0,-1.0));
                meshOut->value.v.push_back(FVertex3D(1.0,-1.0,-1.0));

                // NORMALS
                // Front
                /*
                meshOut->value.vn.push_back(FVertex3D(1.0,1.0,1.0));
                meshOut->value.vn.push_back(FVertex3D(1.0,-1.0,1.0));
                meshOut->value.vn.push_back(FVertex3D(-1.0,-1.0,1.0));
                meshOut->value.vn.push_back(FVertex3D(-1.0,1.0,1.0));
                // Back 
                meshOut->value.vn.push_back(FVertex3D(1.0,1.0,-1.0));
                meshOut->value.vn.push_back(FVertex3D(-1.0,1.0,-1.0));
                meshOut->value.vn.push_back(FVertex3D(-1.0,-1.0,-1.0));
                meshOut->value.vn.push_back(FVertex3D(1.0,-1.0,-1.0));
                */
                // Front
                meshOut->value.vn.push_back(FVertex3D(0.33,0.33,0.33));
                meshOut->value.vn.push_back(FVertex3D(0.33,-0.33,0.33));
                meshOut->value.vn.push_back(FVertex3D(-0.33,-0.33,0.33));
                meshOut->value.vn.push_back(FVertex3D(-0.33,0.33,0.33));
                // Back 
                meshOut->value.vn.push_back(FVertex3D(0.33,0.33,-0.33));
                meshOut->value.vn.push_back(FVertex3D(-0.33,0.33,-0.33));
                meshOut->value.vn.push_back(FVertex3D(-0.33,-0.33,-0.33));
                meshOut->value.vn.push_back(FVertex3D(0.33,-0.33,-0.33));
 

                FFace f;           
                // front face 
                f.push_back(FFacePoint(0,0,0));
                f.push_back(FFacePoint(1,0,1));
                f.push_back(FFacePoint(2,0,2));
                f.push_back(FFacePoint(3,0,3));
                meshOut->value.add_face(f);
                f.clear();
                // back face 
                f.push_back(FFacePoint(4,0,4));
                f.push_back(FFacePoint(5,0,5));
                f.push_back(FFacePoint(6,0,6));
                f.push_back(FFacePoint(7,0,7));
                meshOut->value.add_face(f);
                f.clear();
                // top face 
                f.push_back(FFacePoint(0,0,0));
                f.push_back(FFacePoint(3,0,3));
                f.push_back(FFacePoint(5,0,5));
                f.push_back(FFacePoint(4,0,4));
                meshOut->value.add_face(f);
                f.clear();
                // bottom face 
                f.push_back(FFacePoint(1,0,1));
                f.push_back(FFacePoint(7,0,7));
                f.push_back(FFacePoint(6,0,6));
                f.push_back(FFacePoint(2,0,2));
                meshOut->value.add_face(f);
                f.clear();
                // left face 
                f.push_back(FFacePoint(6,0,6));
                f.push_back(FFacePoint(5,0,5));
                f.push_back(FFacePoint(3,0,3));
                f.push_back(FFacePoint(2,0,2));
                meshOut->value.add_face(f);
                f.clear();
                // right face 
                f.push_back(FFacePoint(1,0,1));
                f.push_back(FFacePoint(0,0,0));
                f.push_back(FFacePoint(4,0,4));
                f.push_back(FFacePoint(7,0,7));
                meshOut->value.add_face(f);
                f.clear();
            }

            subX->update = false;
            subY->update = false;
            subZ->update = false;
        }

        // testing split
        //meshOut->value.split_face(2,5,7);

        return status();
    };

} // namespace feather

NODE_INIT(POLYGON_CUBE,node::Polygon,"polycube.svg")


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
