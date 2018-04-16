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
#include <feather/render.hpp>
//#include <feather/scenegraph.hpp>
#include <feather/draw.hpp>
#include <feather/tools.hpp>
#include <feather/plugin.hpp>

#include "subdiv.hpp"

#ifdef __cplusplus
extern "C" {
#endif
    C_PLUGIN_WRAPPER()
#ifdef __cplusplus
}
#endif

using namespace feather;

#define POLYGON_PLUGIN_ID 3

#define POLYGON_SHAPE 320
#define POLYGON_PLANE 321
#define POLYGON_CUBE 322
#define POLYGON_SUBDIV 323
#define POLYGON_MESH 324


PLUGIN_INIT(POLYGON_PLUGIN_ID,"Polygon","Polygon objects and tools","Richard Layman",POLYGON_SHAPE,POLYGON_MESH)

/*
 ***************************************
 *            POLYGON SHAPE            *
 ***************************************
*/
// meshIn
ADD_FIELD_TO_NODE(POLYGON_SHAPE,FMesh,field::Mesh,field::connection::In,FMesh(),1)
// xformIn 
ADD_FIELD_TO_NODE(POLYGON_SHAPE,FMatrix4x4,field::Matrix4x4,field::connection::In,FMatrix4x4(),2)
// shader 
ADD_FIELD_TO_NODE(POLYGON_SHAPE,FNode,field::Node,field::connection::In,FNode(),3)
// meshOut 
ADD_FIELD_TO_NODE(POLYGON_SHAPE,FMesh,field::Mesh,field::connection::Out,FMesh(),4)

namespace feather
{

    DO_IT(POLYGON_SHAPE)
    { 
        typedef field::Field<FMesh>*  MeshField;
        typedef field::Field<FMatrix4x4>*  MatrixField;
        typedef field::Field<FNode>*  NodeField;

        MeshField meshIn=0;
        MeshField meshOut=0;
        MatrixField xformIn=0;
        MatrixField worldMatrixOut=0;
        NodeField shaderIn=0;

        for(auto f : fields){
            if(f->id == 1)
                meshIn = static_cast<MeshField>(f);
            if(f->id == 2)
                xformIn = static_cast<MatrixField>(f);
            if(f->id == 3)
                shaderIn = static_cast<NodeField>(f);
            if(f->id == 4)
                meshOut = static_cast<MeshField>(f);
            if(f->id == 214)
                worldMatrixOut = static_cast<MatrixField>(f);
         }

        if(meshIn->connected()) {
            field::Connection conn = meshIn->connections.at(0);
            meshIn->value = static_cast<MeshField>(plugin::get_field_base(conn.puid,conn.pnid,conn.pfid,0))->value;
            meshIn->update = static_cast<MeshField>(plugin::get_field_base(conn.puid,conn.pnid,conn.pfid,0))->update;
        }


        if(meshIn->update || worldMatrixOut->update)
        {
            std::cout << "POLYGON SHAPE UPDATE\n";


            /* 
            std::cout << "SHAPE testIn ATTRIBUTES:\n"
                << "fid:" << testIn->id << std::endl
                << "update:" << testIn->update << std::endl
                //<< "puid:" << testIn->puid << std::endl
                //<< "pnid:" << testIn->pn << std::endl
                //<< "pfid:" << testIn->pf << std::endl
                << "connected:" << testIn->connected() << std::endl
                << "conn type:" << testIn->conn_type << std::endl
                << "type:" << testIn->type << std::endl;
            */

            /*
            if(xformIn->connected()) {
                field::Connection conn = xformIn->connections.at(0);
                xformIn->value = static_cast<MatrixField>(plugin::get_field_base(conn.puid,conn.pnid,conn.pfid,0))->value;
            }
            */

            /*
            if(meshIn->connected()) {
                field::Connection conn = meshIn->connections.at(0);
                meshIn->value = static_cast<MeshField>(plugin::get_field_base(conn.puid,conn.pnid,conn.pfid,0))->value;
            }
            */

            meshOut->value = meshIn->value;
            // modify the mesh location based on the xform input
            //std::cout << "BEFORE\n";
            //for(auto v : meshOut->value.v)
            //    std::cout << "meshOut.x = " << v.x << " meshOut.y = " << v.y << " meshOut.z = " << v.z << std::endl;
            //tools::apply_matrix_to_mesh(&xformIn->value,meshOut->value);
            //std::cout << "AFTER\n";
            //for(auto v : meshOut->value.v)
            //    std::cout << "meshOut.x = " << v.x << " meshOut.y = " << v.y << " meshOut.z = " << v.z << std::endl;
            meshOut->update = true;
            std::cout << "SHAPE MATRIX\n";
            //worldMatrixOut->value.print();
            //std::cout << "mesh before\n";
            //meshOut->value.print();
            meshOut->value.apply_matrix(worldMatrixOut->value);
            //std::cout << "mesh after\n";
            //meshOut->value.print();
         }

        return status();
    };

    DRAW_IT(POLYGON_SHAPE)
    {
        std::cout << "POLYGON_SHAPE DRAW IT\n";
        ADD_SHADED_MESH(3)
        return status();    
    };

} // namespace feather

NODE_INIT(POLYGON_SHAPE,node::Shape,"polyshape.svg")


/*
 ***************************************
 *            POLYGON PLANE            *
 ***************************************
*/
// subX
ADD_FIELD_TO_NODE(POLYGON_PLANE,FInt,field::Int,field::connection::In,2,1)
// subY
ADD_FIELD_TO_NODE(POLYGON_PLANE,FInt,field::Int,field::connection::In,2,2)
// meshOut
ADD_FIELD_TO_NODE(POLYGON_PLANE,FMesh,field::Mesh,field::connection::Out,FMesh(),3)


namespace feather
{

    DO_IT(POLYGON_PLANE)
    { 
        //field::FieldBase* f = feather::plugin::get_field_base(node.uid,320,3);

        //std::cout << "Polygon Plane - DO_IT()\n";
        typedef field::Field<FMesh>* MeshOut;

        MeshOut meshOut=nullptr;
        for(auto f : fields){
            if(f->id == 3)
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
// sub x
ADD_FIELD_TO_NODE(POLYGON_CUBE,FInt,field::Int,field::connection::In,0,1)
// sub y
ADD_FIELD_TO_NODE(POLYGON_CUBE,FInt,field::Int,field::connection::In,0,2)
// sub z
ADD_FIELD_TO_NODE(POLYGON_CUBE,FInt,field::Int,field::connection::In,0,3)
// mesh out
ADD_FIELD_TO_NODE(POLYGON_CUBE,FMesh,field::Mesh,field::connection::Out,FMesh(),4)

namespace feather
{

    DO_IT(POLYGON_CUBE) 
    {
        typedef field::Field<FMesh>* MeshOut;
        typedef field::Field<int>* SubIn;
        typedef field::Field<FMatrix4x4>* MatrixField;

        MeshOut meshOut=nullptr;
        SubIn subX=0;
        SubIn subY=0;
        SubIn subZ=0;
        MatrixField worldMatrixOut;

        for(auto f : fields){
            if(f->id == 1)
                subX = static_cast<SubIn>(f);
            if(f->id == 2)
                subY = static_cast<SubIn>(f);
            if(f->id == 3)
                subZ = static_cast<SubIn>(f);
            if(f->id == 4)
                meshOut = static_cast<MeshOut>(f);
            if(f->id == 214)
                worldMatrixOut = static_cast<MatrixField>(f);
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

        if(subX->update || subY->update || subZ->update || worldMatrixOut->update)
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

            std::cout << "CUBE MATRIX\n";
            worldMatrixOut->value.print();
            std::cout << "mesh before\n";
            meshOut->value.print();
            meshOut->value.apply_matrix(worldMatrixOut->value);
            std::cout << "mesh after\n";
            meshOut->value.print();
            subX->update = false;
            subY->update = false;
            subZ->update = false;
            meshOut->update=true;
        }

        // testing split
        //meshOut->value.split_face(2,5,7);
 
        return status();
    };

} // namespace feather

NODE_INIT(POLYGON_CUBE,node::Polygon,"polycube.svg")


/*
 ***************************************
 *            POLYGON SUBDIV           *
 ***************************************
*/
// IN
// mesh in 
ADD_FIELD_TO_NODE(POLYGON_SUBDIV,FMesh,field::Mesh,field::connection::In,FMesh(),1)
// level 
ADD_FIELD_TO_NODE(POLYGON_SUBDIV,FInt,field::Int,field::connection::In,2,2)
// vertex weights
ADD_FIELD_TO_NODE(POLYGON_SUBDIV,FVertexIndiceGroupWeightArray,field::VertexIndiceGroupWeightArray,field::connection::In,std::vector<FVertexIndiceGroupWeight>(),3)
// edge weights
ADD_FIELD_TO_NODE(POLYGON_SUBDIV,FVertexIndiceGroupWeightArray,field::VertexIndiceGroupWeightArray,field::connection::In,std::vector<FVertexIndiceGroupWeight>(),4)
// OUT
// mesh
ADD_FIELD_TO_NODE(POLYGON_SUBDIV,FMesh,field::Mesh,field::connection::Out,FMesh(),5)

namespace feather
{

    DO_IT(POLYGON_SUBDIV) 
    {
        GET_FIELD_DATA(1,FMesh,meshIn,field::connection::In)
        GET_FIELD_DATA(2,FInt,levelIn,field::connection::In)
        GET_FIELD_DATA(3,FVertexIndiceGroupWeightArray,vertexWeightsIn,field::connection::In)
        GET_FIELD_DATA(4,FVertexIndiceGroupWeightArray,edgeWeightsIn,field::connection::In)
        GET_FIELD_DATA(5,FMesh,meshOut,field::connection::Out)

        if(meshIn->update || levelIn->update || vertexWeightsIn->update || edgeWeightsIn->update)
        {
            std::cout << "SUBDIV DO_IT() UPDATE\n";
            // if there is no input mesh, get out of here
            if(!meshIn->value.v.size())
                return status();

            // clear the mesh
            meshOut->value.v.clear();
            meshOut->value.st.clear();
            meshOut->value.vn.clear();
            meshOut->value.f.clear();

            subdiv::subdiv_mesh(
                    levelIn->value,
                    &meshIn->value,
                    &meshOut->value,
                    &vertexWeightsIn->value,
                    &edgeWeightsIn->value
                    );

            meshOut->update = true;
        }

        return status();
    };

    DRAW_IT(POLYGON_SUBDIV)
    {
        std::cout << "POLYGON_SUBDIV DRAW IT\n";
        ADD_COMPONENT_MESH(1)
        return status();    
    };

} // namespace feather

NODE_INIT(POLYGON_SUBDIV,node::Polygon,"polysubdiv.svg")


/*
 ***************************************
 *            POLYGON MESH             *
 ***************************************
*/
// IN
// mesh in 
ADD_FIELD_TO_NODE(POLYGON_MESH,FMesh,field::Mesh,field::connection::In,FMesh(),1)
// OUT
// mesh
ADD_FIELD_TO_NODE(POLYGON_MESH,FMesh,field::Mesh,field::connection::Out,FMesh(),2)

namespace feather
{

    DO_IT(POLYGON_MESH) 
    {
        typedef field::Field<FMesh>* MeshField;

        MeshField  meshIn;
        MeshField meshOut;

        for(auto f : fields){
            if(f->id == 1)
                meshIn = static_cast<MeshField>(f);
            if(f->id == 2)
                meshOut = static_cast<MeshField>(f);
        }

        if(meshIn->connected()) {
            field::Connection conn = meshIn->connections.at(0);
            meshIn->value = static_cast<MeshField>(plugin::get_field_base(conn.puid,conn.pnid,conn.pfid,0))->value;
            meshIn->update = static_cast<MeshField>(plugin::get_field_base(conn.puid,conn.pnid,conn.pfid,0))->update;
        }

        if(meshIn->update)
        {
            // if there is no input mesh, get out of here
            if(!meshIn->value.v.size())
                return status();

            // clear the mesh
            meshOut->value.v.clear();
            meshOut->value.st.clear();
            meshOut->value.vn.clear();
            meshOut->value.f.clear();

            meshOut->value = meshIn->value;

            meshOut->update = true;
        }

        return status();
    };

} // namespace feather

NODE_INIT(POLYGON_MESH,node::Polygon,"polymesh.svg")



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
