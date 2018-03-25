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

        if(sluxprops.config != nullptr) {
            delete sluxprops.config;
            sluxprops.config = nullptr;
        }

        if(sluxprops.session != nullptr) {
            delete sluxprops.session;
            sluxprops.session = nullptr;
        }

        luxcore::Init();

        // LOAD THE SCENE
        //sluxprops.scene = new luxcore::Scene();

        std::vector<uint32_t> cameras;
        std::vector<uint32_t> shapes;

        scenegraph::get_node_by_type(node::Camera,cameras);
        scenegraph::get_node_by_type(node::Shape,shapes);
        std::cout << "NUMBER OF CAMERAS:" << cameras.size() << std::endl;
        std::cout << "NUMBER OF SHAPES:" << shapes.size() << std::endl;

        
        // TODO - CLEANUP OLD POINTERS HERE

        sluxprops.scene = new luxcore::Scene("/home/richard/luxed_tests/test_set/set.scn");
        //sluxprops.properties = new luxrays::Properties("/home/richard/luxed_tests/test_set/render.cfg");

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

        //sluxprops.properties->Set(luxrays::Property("film.imagepipeline.0.type","TONEMAP_LINEAR"));
        //sluxprops.properties->Set(luxrays::Property("film.imagepipeline.1.type","GAMMA_CORRECTION"));
        //sluxprops.properties->Set(luxrays::Property("film.imagepipeline.1.value",2.2));

        sluxprops.config = new luxcore::RenderConfig(*sluxprops.properties,sluxprops.scene);
        sluxprops.session = new luxcore::RenderSession(sluxprops.config);

        //sluxprops.config->GetProperties().Set(luxrays::Property("frame.width")(frame_width));
        //sluxprops.config->GetProperties().Set(luxrays::Property("frame.height",luxrays::PropertyValue(frame_height)));
        //sluxprops.config->GetProperties().Get("frame.height")=34;
 
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
