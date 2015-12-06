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
#include <feather/gl.hpp>
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
ADD_FIELD_TO_NODE(POLYGON_SHAPE,FInt,field::Int,field::connection::In,10,4)

namespace feather
{

    DO_IT(POLYGON_SHAPE)
    { 
        //typedef field::Field<FMesh>* MeshIn;
        //MeshIn meshIn = static_cast<MeshIn>(fields.at(2));
        //meshIn->value.build_gl();

        //std::cout << "value from polycube field:" << f->value << std::endl;
        return status();
    };

    GL_INIT(POLYGON_SHAPE)
    {
        ADD_GL_VERT_SHADER("shaders/vert/mesh.glsl")
        ADD_GL_FRAG_SHADER("shaders/frag/lambert.glsl")
        //ADD_GL_GEOM_SHADER("shaders/geom/wireframe.glsl")
        GL_INIT_FINISH()
    }; 

    GL_DRAW(POLYGON_SHAPE)
    {
        //std::cout << "Polygon Shape - DO_IT()\n";
         // meshIn
        field::FieldBase* f = feather::scenegraph::get_fieldBase(node.uid,320,3);

        typedef field::Field<FMesh>* fielddata;
        fielddata tf = static_cast<fielddata>(f);

        //fielddata tf = static_cast<fielddata>(feather::scenegraph::get_fieldBase(node.uid,320,3));

        // you can check connection using tf->connected
 
        //int glAmbient = info.program->uniformLocation("Ambient");
        //info.program->setUniformValue(glAmbient,QColor(1.0,1.0,1.0));
        //int glLightColor = info.program->uniformLocation("LightColor");
        //info.program->setUniformValue(glLightColor,QColor(1.0,0.0,0.0));
        //int glLightDirection = info.program->uniformLocation("LightDirection");
        //info.program->setUniformValue(glLightDirection,QVertex3D(1.0,0.0,0.0));

        if(tf!=NULL)
        {
            //std::cout << "DRAWING V COUNT " << tf->value.v.size() << std::endl;
            /*
            std::cout << "field specs" 
                << "\n\tconnected:" << tf->connected 
                << "\n\tfid:" << tf->id
                << "\n\tpn:" << tf->pn
                << "\n\tpf:" << tf->pf
                << "\n\ttype:" << tf->type
                << std::endl;
            */

            if(tf->value.v.size() >= 4)
            {
                QVector3D lpos;
                lpos.setX(10);
                lpos.setY(20);
                lpos.setZ(0);

                // future macro that will handle drawing mesh types
                //GL_DRAW_MESH(tf)
                
                info.program->bind();

                FColorRGB lcolor = scenegraph::node_layer(node.uid)->color;
                info.program->setUniformValue(node.glSelected, false);
                info.program->setAttributeValue(node.glLayerColor, QColor(lcolor.int_red(),lcolor.int_green(),lcolor.int_blue()));
                info.program->setAttributeValue(node.glLightPosition, lpos);
                info.program->setAttributeValue(node.glLightAmbient, QColor(0,0,0));
                info.program->setAttributeValue(node.glLightDiffuse, QColor(255,255,255));
                info.program->setAttributeValue(node.glLightSpecular, QColor(0,0,0));
                info.program->setAttributeValue(node.glMaterialAmbient, QColor(0,0,0));
                info.program->setAttributeValue(node.glMaterialDiffuse, QColor(255,255,255));
                info.program->setAttributeValue(node.glMaterialSpecular, QColor(0,0,0));
                info.program->setAttributeValue(node.glMaterialShininess, 0.5);

                info.program->setUniformValue(node.glMatrix, *info.view);

                info.program->enableAttributeArray(node.glVertex);
                info.program->enableAttributeArray(node.glColor);
                info.program->enableAttributeArray(node.glNormal);
                info.program->setAttributeArray(node.glVertex, GL_DOUBLE, &tf->value.v[0], 3);
                info.program->setAttributeArray(node.glColor, GL_FLOAT, &tf->value.glc[0], 4);
                info.program->setAttributeArray(node.glNormal, GL_DOUBLE, &tf->value.vn[0],3);

                /*
                std::cout << "draw count for " << node.uid
                    << "\n\tv=" << tf->value.v.size()
                    << "\n\tglc=" << tf->value.glc.size()
                    << "\n\tvn=" << tf->value.vn.size()
                    << "\n\tgli=" << tf->value.gli.size()
                    << "\n\tf=" << tf->value.f.size()
                    << std::endl;
                */

                /*      
                QColor color;

                color.setRgb(0,0,100);
                info.program->setAttributeValue(node.glShaderDiffuse, color);
                */
                //info.program->setAttributeValue(node.glColor, color);
                uint glView=0;
                info.program->setUniformValue(node.glView, glView);

    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    //glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);

                // SHADED MODEL 
                glPolygonMode(GL_FRONT, GL_FILL);
                //glPolygonMode(GL_BACK, GL_LINE);
                glDrawElements(GL_TRIANGLES, tf->value.gli.size(), GL_UNSIGNED_INT, &tf->value.gli[0]);

                //color.setRgb(0,0,0);
                //info.program->setAttributeValue(node.glShaderDiffuse, color);

                // WIREFRAME MODEL
                glLineWidth(1.5);
                glView=1;
                info.program->setUniformValue(node.glView, glView);

                glPolygonMode(GL_FRONT, GL_LINE);
                glDrawElements(GL_TRIANGLES, tf->value.gli.size(), GL_UNSIGNED_INT, &tf->value.gli[0]);

                /*
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
                // going to try to draw the edges using a shader
                glPolygonMode(GL_FRONT, GL_LINE);
                //glDrawElements(GL_TRIANGLES, tf->value.gli.size(), GL_UNSIGNED_INT, &tf->value.gli[0]);
                // draw face edges
                uint istep=0;
                for_each(tf->value.f.begin(), tf->value.f.end(), [tf,&istep](FFace _f){
                    glDrawElements(GL_LINE_LOOP, _f.size(), GL_UNSIGNED_INT, &tf->value.glei[istep]);
                    istep = istep + _f.size();
                });
                */

                info.program->disableAttributeArray(node.glVertex);
                info.program->disableAttributeArray(node.glColor);
                info.program->disableAttributeArray(node.glNormal);
                info.program->release();

            }
        }

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
        meshOut->value.glv.clear();
        meshOut->value.glvn.clear();
        meshOut->value.gli.clear();

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

        meshOut->value.build_gl();

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
            if(f->id == 5)
                meshOut = static_cast<MeshOut>(f);
        }
        if(!meshOut) {
            //std::cout << "could not find meshOut\n";
            return status();
        }
        //typedef field::Field<FMesh>* MeshOut;
        typedef field::Field<int>* SubIn;

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
            meshOut->value.glv.clear();
            meshOut->value.glvn.clear();
            meshOut->value.gli.clear();


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

        meshOut->value.build_gl();
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
