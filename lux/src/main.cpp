/***********************************************************************
 *
 * Filename: main.cpp
 *
 * Description: Plugin for LuxRender.
 *
 * Copyright (C) 2018 Richard Layman, rlayman2000@yahoo.com 
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
#include <feather/plugin.hpp>
#include <feather/tools.hpp>
#include <feather/render.hpp>
#include <feather/field.hpp>
#include <feather/node.hpp>
#include <feather/parameter.hpp>
#include <feather/command.hpp>
#include <feather/scenegraph.hpp>

#include <luxcore/luxcore.h>
#include <luxrays/utils/properties.h>
#include <slg/slg.h>
#include <chrono>
#include <thread>
#include "lux.hpp"


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

// LUX ID's
// 500-510 - objects
// 511-550 - shaders
// 551-560 - lights
// 561-570 - camera 

#define LUX_PLUGIN_ID 6 

#define LUX_TEST_BOOL_ATTRIBUTE 1
#define LUX_TEST_UINT_ATTRIBUTE 2
#define LUX_TEST_INT_ATTRIBUTE 3
#define LUX_TEST_REAL_ATTRIBUTE 4
#define LUX_TEST_STRING_ATTRIBUTE 5
// TEST ATTRIBUTES
#define LUX_FRAME_WIDTH 6
#define LUX_FRAME_HEIGHT 7

#define LUX_RENDER_ID 1

#define LUX_SHADER_MATTE 511
#define LUX_SHADER_EMISSION 512
#define LUX_CAMERA_PERSPECTIVE 561


/*
 ***************************************
 *              PLUGIN INIT            *
 ***************************************
 */ 

PLUGIN_INIT(LUX_PLUGIN_ID,"LuxRender","LuxRender nodes and commands","Richard Layman",LUX_SHADER_MATTE,LUX_CAMERA_PERSPECTIVE)


/*
 ***************************************
 *            ATTRIBUTES               *
 ***************************************
 */ 

ADD_BOOL_ATTRIBUTE(LUX_TEST_BOOL_ATTRIBUTE,test_bool_attribute,true)
ADD_UINT_ATTRIBUTE(LUX_TEST_UINT_ATTRIBUTE,test_uint_attribute,2332)
ADD_INT_ATTRIBUTE(LUX_TEST_INT_ATTRIBUTE,test_int_attribute,23)
ADD_REAL_ATTRIBUTE(LUX_TEST_REAL_ATTRIBUTE,test_real_attribute,23.23)
ADD_STRING_ATTRIBUTE(LUX_TEST_STRING_ATTRIBUTE,test_string_attribute,"test")
ADD_UINT_ATTRIBUTE(LUX_FRAME_WIDTH,frame_width,400)
ADD_UINT_ATTRIBUTE(LUX_FRAME_HEIGHT,frame_height,200)


/*
 ***************************************
 *            MATTE SHADER             *
 ***************************************
 */

// color
ADD_FIELD_TO_NODE(LUX_SHADER_MATTE,FColorRGBA,field::RGBA,field::connection::In,FColorRGBA(),1)

struct LuxRenderProperties {
    luxcore::Scene* scene;
    luxrays::Properties* properties;
    luxrays::Properties* sceneprops;
    luxcore::RenderConfig* config;
    luxcore::RenderSession* session;
};

static LuxRenderProperties sluxprops = LuxRenderProperties();

namespace feather
{

    DO_IT(LUX_SHADER_MATTE)
    {
        return status();
    };

    RENDER_START(LUX_RENDER_ID)
    {
        std::cout << "RENDER START\n";

        // clear out the old pointers
        if(sluxprops.scene != nullptr) {
            delete sluxprops.scene;
            sluxprops.scene = nullptr;
        }

        if(sluxprops.properties != nullptr) {
            delete sluxprops.properties;
            sluxprops.properties = nullptr;
        }

        if(sluxprops.sceneprops != nullptr) {
            delete sluxprops.sceneprops;
            sluxprops.sceneprops = nullptr;
        }

        if(sluxprops.config != nullptr) {
            delete sluxprops.config;
            sluxprops.config = nullptr;
        }

        if(sluxprops.session != nullptr) {
            delete sluxprops.session;
            sluxprops.session = nullptr;
        }

        luxcore::Init();

 
        //std::cout << "FRAME WIDTH:" << frame_width << " FRAME HEIGHT:" << frame_height << std::endl;

        sluxprops.scene = new luxcore::Scene();

        // RENDER PROPERTIES
        sluxprops.properties = new luxrays::Properties();
        sluxprops.properties->Set(luxrays::Property("renderengine.type",std::string("PATHOCL")));
        sluxprops.properties->Set(luxrays::Property("opencl.platform.index",-1));
        sluxprops.properties->Set(luxrays::Property("opencl.cpu.use",0));
        sluxprops.properties->Set(luxrays::Property("opencl.gpu.use",1));
        sluxprops.properties->Set(luxrays::Property("opencl.gpu.workgroup.size",64));
        sluxprops.properties->Set(luxrays::Property("scene.epsilon.min",9.999));
        sluxprops.properties->Set(luxrays::Property("scene.epsilon.max",0.1));
        sluxprops.properties->Set(luxrays::Property("accelerator.instances.enable",0));
        sluxprops.properties->Set(luxrays::Property("path.maxdepth",12));
        sluxprops.properties->Set(luxrays::Property("film.width",frame_width));
        sluxprops.properties->Set(luxrays::Property("film.height",frame_height));
        sluxprops.properties->Set(luxrays::Property("sampler.type",std::string("RANDOM")));
        sluxprops.properties->Set(luxrays::Property("film.filter.type",std::string("GAUSSIAN")));
        sluxprops.properties->Set(luxrays::Property("film.filter.xwidth",1));
        sluxprops.properties->Set(luxrays::Property("film.filter.ywidth",1));

        // THIS WILL CAUSE ISSUES
        //sluxprops.properties->Set(luxrays::Property("film.imagepipeline.0.type","TONEMAP_LINEAR"));
        //sluxprops.properties->Set(luxrays::Property("film.imagepipeline.1.type","GAMMA_CORRECTION"));
        //sluxprops.properties->Set(luxrays::Property("film.imagepipeline.1.value",2.2));

        // SCENE PROPERTIES
        sluxprops.sceneprops = new luxrays::Properties();
 
        // CAMERA

        sluxprops.sceneprops->Set(luxrays::Property("scene.camera.lookat.orig",luxrays::PropertyValues{0,1,6}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.camera.lookat.target",luxrays::PropertyValues{0.0,1,0.0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.camera.up", luxrays::PropertyValues{0, 1, 0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.camera.cliphither", 0.001));
        sluxprops.sceneprops->Set(luxrays::Property("scene.camera.clipyon", 100.00));
        sluxprops.sceneprops->Set(luxrays::Property("scene.camera.lensradius", 0.00));
        sluxprops.sceneprops->Set(luxrays::Property("scene.camera.focaldistance", 0.28));
        sluxprops.sceneprops->Set(luxrays::Property("scene.camera.fieldofview", 49));

        // VOLUME

        sluxprops.sceneprops->Set(luxrays::Property("scene.volumes.default_volume.type",std::string("clear")));

        // LIGHT - You must have at least one light source that is not an emission
        sluxprops.sceneprops->Set(luxrays::Property("scene.lights.ext_light.gain", luxrays::PropertyValues{0, 0, 0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.lights.ext_light.gain.transformation", luxrays::PropertyValues{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.lights.ext_light.gain.samples", -1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.lights.ext_light.gain.visibility.indirect.diffuse.enable", 1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.lights.ext_light.gain.visibility.indirect.glossy.enable", 1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.lights.ext_light.gain.visibility.indirect.specular.enable", 1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.lights.ext_light.gain.dir", luxrays::PropertyValues{-0.51, -3.38, 0.76}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.lights.ext_light.gain.turbidity", 2.20));
        sluxprops.sceneprops->Set(luxrays::Property("scene.lights.ext_light.gain.relsize", 1));
 
        // LIGHT MATERIAL

        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.type",std::string("matte")));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.kd",luxrays::PropertyValues{1.0,1.0,1.0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.bumptex",luxrays::PropertyValues{0.0,0.0,0.0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.normaltex",luxrays::PropertyValues{0.0,0.0,0.0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.samples",-1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.visibility.indirect.diffuse.enable",1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.visibility.indirect.glossy.enable",1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.visibility.indirect.specular.enable",1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.bumpsamplingdistance",0.001));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.volume.interior",std::string("default_volume")));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.volume.exterior",std::string("default_volume")));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.id",0));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.transparency",luxrays::PropertyValues{1.0,1.0,1.0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.shadowcatcher.enable",0));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.emission",luxrays::PropertyValues{1.0,1.0,1.0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.emission.gain",luxrays::PropertyValues{1.0,1.0,1.0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.emission.power",10.0));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.emission.efficency",90.0));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.emission.mapfile",std::string("")));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.emission.gamma",2.2));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.emission.iesfile",std::string("")));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.emission.flipz",0));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.emission.samples",-1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.emission.map.width",0));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.emission.map.height",0));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_light_mat.emission.id",0));


        // DEFAULT MATERIAL 

        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_mat.type",std::string("roughmatte")));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_mat.kd",luxrays::PropertyValues{0.5,0.5,0.5}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_mat.sigma",luxrays::PropertyValues{0,0,0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_mat.bumptex",luxrays::PropertyValues{0,0,0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_mat.normaltex",luxrays::PropertyValues{0,0,0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_mat.samples",-1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_mat.visibility.indirect.diffuse.enable",1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_mat.visibility.indirect.glossy.enable",1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_mat.visibility.indirect.specular.enable",1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_mat.bumpsamplingdistance",0.001));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_mat.volume.interior",std::string("default_volume")));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_mat.volume.exterior",std::string("default_volume")));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.default_mat.id",1));


        /*
        luxrays::Point* points = luxcore::Scene::AllocVerticesBuffer(3);
        luxrays::Triangle* tris = luxcore::Scene::AllocTrianglesBuffer(1);
        points[0]={0,0,0};
        points[1]={4,0,0};
        points[2]={0,0,4};
        tris[0]={2,1,0};
        luxrays::ExtTriangleMesh mesh(3,1,points,tris);
        sluxprops.scene->DefineMesh("test_mesh",&mesh);
        */

        // LOAD SHAPE NODES
        std::vector<uint32_t> cameras;
        std::vector<uint32_t> shapes;

        scenegraph::get_node_by_type(node::Camera,cameras);
        scenegraph::get_node_by_type(node::Shape,shapes);
        std::cout << "NUMBER OF CAMERAS:" << cameras.size() << std::endl;
        std::cout << "NUMBER OF SHAPES:" << shapes.size() << std::endl;

        // load polygons
        for(auto uid : shapes) {
            status error;
            std::string name;
            scenegraph::get_node_name(uid,name,error);
            // get the mesh in
            field::Field<FMesh>* meshfield = static_cast<field::Field<FMesh>*>(scenegraph::get_fieldBase(uid,1));
            // create lux mesh
            if(meshfield->value.is_tri_mesh())
                std::cout << "TRI ONLY MESH\n";
            else
                std::cout << "NOT A TRI MESH\n";

            // vertex
            luxrays::Point* points = luxcore::Scene::AllocVerticesBuffer(meshfield->value.v.size());
 
            //std::vector<luxrays::Point> points;
            uint32_t i=0;
            for(auto point : meshfield->value.v) {
                points[i++] = {point.x,point.y,point.z};
                //points.push_back(luxrays::Point(point.x,point.y,point.z));
            }

            // triangle face indics

            std::vector<luxrays::Triangle> tris;
            i=0;
            uint32_t fcount=0;
            for(auto face : meshfield->value.f) {
                //tris.push_back(luxrays::Triangle(static_cast<uint32_t>(face[0].v),static_cast<uint32_t>(face[1].v),static_cast<uint32_t>(face[3].v)));
                for(uint32_t j=0; j < face.size(); j++) {
                    if(j < 3) {
                        tris.push_back(luxrays::Triangle(static_cast<uint32_t>(face[0].v),static_cast<uint32_t>(face[1].v),static_cast<uint32_t>(face[2].v)));
                        j+=2;
                    } else {
                        tris.push_back(luxrays::Triangle(static_cast<uint32_t>(face[0].v),static_cast<uint32_t>(face[j-1].v),static_cast<uint32_t>(face[j].v)));
                    } 
                    fcount++;
                }
            }
            luxrays::Triangle* luxtris = luxcore::Scene::AllocTrianglesBuffer(fcount);

            i=0;
            for(auto tri : tris) {
                luxtris[i++] = {tri.v[0],tri.v[1],tri.v[2]};
            }

            //luxrays::ExtTriangleMesh* mesh = new luxrays::ExtTriangleMesh(meshfield->value.v.size(),tris.size(),static_cast<luxrays::Point*>(&points[0]),static_cast<luxrays::Triangle*>(&tris[0]));
            luxrays::ExtTriangleMesh* mesh = new luxrays::ExtTriangleMesh(meshfield->value.v.size(),fcount,points,luxtris);

            std::cout << "DEFINING MESH " << name.c_str() << std::endl;

            sluxprops.scene->DefineMesh(name.c_str(),mesh);
            std::stringstream ss;
            ss << "scene.objects." << name.c_str() << ".shape";
            std::cout << ss.str() << std::endl;
            sluxprops.sceneprops->Set(luxrays::Property(ss.str().c_str(),std::string(name.c_str())));
            ss.str(std::string());
            ss << "scene.objects." << name.c_str() << ".material";
            std::cout << ss.str() << std::endl;
            if(name == "light_shape")
                sluxprops.sceneprops->Set(luxrays::Property(ss.str().c_str(),std::string("default_light_mat")));
            else 
                sluxprops.sceneprops->Set(luxrays::Property(ss.str().c_str(),std::string("default_mat")));
       }


        /*
        // LIGHT SETUP
        sluxprops.sceneprops->Set(luxrays::Property("scene.shapes.light_housing_shp.type",std::string("inlinedmesh")));
        sluxprops.sceneprops->Set(luxrays::Property("scene.shapes.light_housing_shp.vertices",luxrays::PropertyValues{0.0,3.0,0.0, 1.0,3.0,0.0, 0.0,3.0,1.0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.shapes.light_housing_shp.faces",luxrays::PropertyValues{0,1,2}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.objects.light_housing_obj.material",std::string("light_housing_mat")));
        sluxprops.sceneprops->Set(luxrays::Property("scene.objects.light_housing_obj.shape",std::string("light_housing_shp")));
        
        // CUBE

        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.cube_mat.type",std::string("roughmatte")));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.cube_mat.kd",luxrays::PropertyValues{0.0,0.75,0.75}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.cube_mat.sigma",luxrays::PropertyValues{0,0,0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.cube_mat.bumptex",luxrays::PropertyValues{0,0,0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.cube_mat.normaltex",luxrays::PropertyValues{0,0,0}));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.cube_mat.samples",-1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.cube_mat.visibility.indirect.diffuse.enable",1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.cube_mat.visibility.indirect.glossy.enable",1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.cube_mat.visibility.indirect.specular.enable",1));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.cube_mat.bumpsamplingdistance",0.001));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.cube_mat.volume.interior",std::string("default_volume")));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.cube_mat.volume.exterior",std::string("default_volume")));
        sluxprops.sceneprops->Set(luxrays::Property("scene.materials.cube_mat.id",1));
        */

        // CUBE MATERIAL
        //sluxprops.sceneprops->Set(luxrays::Property("scene.objects.test_mesh.shape",std::string("test_mesh")));
        //sluxprops.sceneprops->Set(luxrays::Property("scene.objects.test_mesh.material",std::string("cube_mat")));

        sluxprops.scene->Parse(*sluxprops.sceneprops);
        sluxprops.config = new luxcore::RenderConfig(*sluxprops.properties,sluxprops.scene);
        sluxprops.session = new luxcore::RenderSession(sluxprops.config);

        /* 
        std::vector<std::string> names = sluxprops.properties->GetAllNames();
        for(auto name : names)
            std::cout << "property name:" << name << std::endl;
        */

        sluxprops.session->Start();

        return status();
    };
 
    RENDER_STOP(LUX_RENDER_ID)
    {
        std::cout << "RENDER STOP\n";

        // the render won't show if the session is still running
        sluxprops.session->Stop();

        return status();
    };
   
    RENDER_BUFFER(LUX_RENDER_ID)
    {
        sluxprops.session->UpdateStats();

        std::cout << "frame_width=" << frame_width << " frame_height=" << frame_height << std::endl;
        std::cout << "buffer width=" << buffer.width << " buffer height=" << buffer.height << std::endl;
        std::cout << "value=" << (frame_width*frame_height)*3 << std::endl;

        float* data = new float[(frame_width*frame_height)*3];
        delete [] buffer.data;
        buffer.data = new char[(frame_width*frame_height)*3];

        std::cout << "sample count:" << sluxprops.session->GetFilm().GetTotalSampleCount();

        if(sluxprops.session->GetFilm().HasOutput(luxcore::Film::OUTPUT_RGB_TONEMAPPED)) {
            sluxprops.session->GetFilm().GetOutput<float>(luxcore::Film::OUTPUT_RGB_TONEMAPPED,data);
            // film up buffer
            uint32_t buffer_size=frame_width*frame_height*3;
            uint32_t i=0;
            //uint32_t offset=199;
            uint32_t offset=frame_height-1;
            uint32_t linestep=0;
            while(i < buffer_size) {
                // buffer.data[0] = RED
                // buffer.data[1] = GREEN
                // buffer.data[2] = BLUE
                while(linestep < (frame_width*3)) {
                    buffer.data[(linestep++)+(frame_width*offset*3)] = data[i++] * 255;
                    buffer.data[(linestep++)+(frame_width*offset*3)] = data[i++] * 255;
                    buffer.data[(linestep++)+(frame_width*offset*3)] = data[i++] * 255;
                    //linestep++;
                }
                offset--;
                linestep=0;
                //std::cout << "[" << test[i++] << "," << test[i++] << "," << test[i++] << "],";
            }
        }

        std::cout << "\nTEST BOOL ATTRIBUTE VALUE:" << test_bool_attribute << std::endl;
        std::cout << "\nTEST UINT ATTRIBUTE VALUE:" << test_uint_attribute << std::endl;
        std::cout << "\nTEST INT ATTRIBUTE VALUE:" << test_int_attribute << std::endl;
        std::cout << "\nTEST REAL ATTRIBUTE VALUE:" << test_real_attribute << std::endl;
        //std::cout << "\nTEST STRING ATTRIBUTE VALUE:" << test_bool_attribute << std::endl;

        delete[] data;

        return status();
    };

} // namespace feather


RENDER_INIT(LUX_RENDER_ID,"LuxRender")


NODE_INIT(LUX_SHADER_MATTE,node::Shader,"")


/*
 ***************************************
 *         EMISSION SHADER             *
 ***************************************
 */

// color
ADD_FIELD_TO_NODE(LUX_SHADER_EMISSION,FColorRGBA,field::RGBA,field::connection::In,FColorRGBA(),1)


namespace feather
{

    DO_IT(LUX_SHADER_EMISSION)
    {
        return status();
    };

} // namespace feather


NODE_INIT(LUX_SHADER_EMISSION,node::Shader,"")


/*
 ***************************************
 *         CAMERA PERSPECTIVE          *
 ***************************************
 */

// fov
ADD_FIELD_TO_NODE(LUX_CAMERA_PERSPECTIVE,FReal,field::Real,field::connection::In,55.0,1)


namespace feather
{

    DO_IT(LUX_CAMERA_PERSPECTIVE)
    {
        return status();
    };

} // namespace feather

NODE_INIT(LUX_CAMERA_PERSPECTIVE,node::Camera,"")



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
