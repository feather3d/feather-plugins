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

#define LUX_RENDER_ID 1

#define LUX_SHADER_MATTE 511
#define LUX_SHADER_EMISSION 512
#define LUX_CAMERA_PERSPECTIVE 561


/*
 ***************************************
 *              PLUGIN INIT            *
 ***************************************
 */ 

PLUGIN_INIT("LuxRender","LuxRender nodes and commands","Richard Layman",LUX_SHADER_MATTE,LUX_CAMERA_PERSPECTIVE)

/*
 ***************************************
 *            MATTE SHADER             *
 ***************************************
 */

// color
ADD_FIELD_TO_NODE(LUX_SHADER_MATTE,FColorRGBA,field::RGBA,field::connection::In,FColorRGBA(),1)


namespace feather
{

    DO_IT(LUX_SHADER_MATTE)
    {
        return status();
    };

    RENDER_START(LUX_RENDER_ID)
    {
        std::cout << "RENDER START\n";
        return status();
    };
 
    RENDER_STOP(LUX_RENDER_ID)
    {
        std::cout << "RENDER STOP\n";
        return status();
    };
   
    RENDER_BUFFER(LUX_RENDER_ID)
    {
        std::cout << "LuxRender Render Buffer Called\n";


        luxcore::Init();

        std::cout << "LuxCore\n"; // << pyluxcore.Version();
        std::cout << "RenderConfig and RenderSession examples (requires scenes directory)...\n";

        // Load the configuration from file
        luxcore::Scene scene("/home/richard/luxed_tests/basic_test/scene.scn");
        luxrays::Properties props("/home/richard/luxed_tests/basic_test/render.cfg");


        //props = pyluxcore.Properties("scenes/luxball/luxball-hdr.cfg")

        // Change the render engine to PATHCPU
        //props.Set(pyluxcore.Property("renderengine.type", ["PATHCPU"]))

        //config = pyluxcore.RenderConfig(props)
        luxcore::RenderConfig config(props,&scene);
        //session = pyluxcore.RenderSession(config)
        luxcore::RenderSession session(&config);
        session.Start();

        bool running=true;
        int count=0;

        while(running) {
            /*
            float* test;
            float data[(400*200)*3];
            char cdata[(400*200)*3];
            test = data;
            buffer.data=cdata;
            */

            if(session.GetFilm().HasOutput(luxcore::Film::OUTPUT_RGB_TONEMAPPED)) {
                std::cout << "FILM OUTPUT FOUND\n";
                /*
                session.GetFilm().GetOutput<float>(luxcore::Film::OUTPUT_RGB_TONEMAPPED,test);
                // film up buffer
                int i=0;
                while(i < ((400*200)*3)) {
                    buffer.data[i++] = test[i] * 255;
                    buffer.data[i++] = test[i] * 255;
                    buffer.data[i++] = test[i] * 255;
                    //std::cout << "[" << test[i++] << "," << test[i++] << "," << test[i++] << "],";
                }
                std::cout << "\n\n\n";
                */

                //session.GetFilm().GetOutput<float>(luxcore::Film::OUTPUT_RGB_TONEMAPPED,reinterpret_cast<float*>(&buffer.data));
                //std::cout << "buffer length " << sizeof(&test) << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));

            count++;
            if(count==20)
                running=false;
        }
        /*
        startTime = time.time()
        while True:
	    time.sleep(1)

	    elapsedTime = time.time() - startTime

	    # Print some information about the rendering progress

	    # Update statistics
	    session.UpdateStats()

	    stats = session.GetStats();
	    print("[Elapsed time: %3d/5sec][Samples %4d][Avg. samples/sec % 3.2fM on %.1fK tris]" % (
		    stats.Get("stats.renderengine.time").GetFloat(),
	            stats.Get("stats.renderengine.pass").GetInt(),
		    (stats.Get("stats.renderengine.total.samplesec").GetFloat()  / 1000000.0),
		    (stats.Get("stats.dataset.trianglecount").GetFloat() / 1000.0)))

	    if elapsedTime > 5.0:
		# Time to stop the rendering
		break
        */


        // the render won't show if the session is still running
        session.Stop();

        float* test;
        float data[(400*200)*3];
        test = data;
        char cdata[(400*200)*3];
        buffer.data = cdata;

        session.GetFilm().GetOutput<float>(luxcore::Film::OUTPUT_RGB_TONEMAPPED,test);
        // film up buffer
        uint32_t buffer_size=400*200*3;
        uint32_t i=0;
        uint32_t offset=199;
        uint32_t linestep=0;
        while(i < buffer_size) {
            // buffer.data[0] = RED
            // buffer.data[1] = GREEN
            // buffer.data[2] = BLUE
            while(linestep < (400*3)) {
                buffer.data[(linestep++)+(400*offset*3)] = test[i++] * 255;
                buffer.data[(linestep++)+(400*offset*3)] = test[i++] * 255;
                buffer.data[(linestep++)+(400*offset*3)] = test[i++] * 255;
                //linestep++;
            }
            offset--;
            linestep=0;
            //std::cout << "[" << test[i++] << "," << test[i++] << "," << test[i++] << "],";
        }
        std::cout << "\n\n\n";

        // Save the rendered image
        //session.GetFilm().Save()
        //session.GetFilm().SaveOutput("/home/richard/lux_test.png",luxcore::Film::OUTPUT_RGB_TONEMAPPED,props);

        std::cout << "Done.\n";

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
