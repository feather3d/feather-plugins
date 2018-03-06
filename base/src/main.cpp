/***********************************************************************
 *
 * Filename: main.cpp
 *
 * Description: Base plugin which a new plugin is created from.
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
#include <feather/plugin.hpp>
#include <feather/tools.hpp>
#include <feather/field.hpp>
#include <feather/node.hpp>
#include <feather/parameter.hpp>
#include <feather/command.hpp>
#include <feather/render.hpp>


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
#define STARTNODE 5
#define ENDNODE 6

/*
 ***************************************
 *              PLUGIN INIT            *
 ***************************************
 * PLUGIN_INIT(startnode,endnode)
 * startnode = id of the first node
 * endnode = id of the last node
 * This macro handles some misc bindings
 * between the plugin and the pluginmanager.
 */ 
PLUGIN_INIT("name","description","author",STARTNODE,ENDNODE)

/*
 ***************************************
 *                 NODE                *
 ***************************************
*/


/*
 ***************************************
 *          ADD_FIELD_TO_NODE          *
 ***************************************
 * ADD_FIELD_TO_NODE(node,type,connection,key)
 * node = Enum of the node that the field
 *      will be put into.
 * type = datatype 
 * connection = connection direction 
 * key = Starting from 1, the number of
 *      the field in the attribute.
 *
 * The macro will tie the field to the node
 * so that it's value can be used by the
 * node's do_it command.
 */
ADD_FIELD_TO_NODE(STARTNODE,FMesh,field::Mesh,field::connection::In,FMesh(),200)


namespace feather
{
    /*
     ***************************************
     *              DO_IT                  *
     ***************************************
     * DO_IT(node)
     * node = Enum of the node that the field
     *      will be put into.
     *
     * This function will be call everytime
     * the node updates in the scenegraph.
     * A node will get updated when it's
     * input field changes. This can be from
     * the user or another node's field that's
     * connected to the node's input field. 
     */

    DO_IT(STARTNODE)
    {
        /*
         * put your code that needs to run
         * whenever a field is changed
         */ 
        return status();
    };

} // namespace feather

/*
 ***************************************
 *              NODE_INIT              *
 ***************************************
 * NODE_INIT(start_node,node_enum,node_icon_file)
 * node = Enum of the node that the field
 *      will be put into.
 *
 * This macro will setup the node so that
 * it can be called by the scenegraph.
 */
NODE_INIT(STARTNODE,node::Object,"")



/*
 ***************************************
 *              END NODE               *
 ***************************************
*/

/*      SAME AS START NODE             */



/*
 ***************************************
 *              COMMANDS               *
 ***************************************
 * Put all your commands here.
 * Commands are functions that can
 * be called from the interface and
 * are only called once. An example
 * of this would be something like
 * render() or import_obj().
 * You will still need to edit the
 * qml interface to call an commands
 * that you've created.
 */

namespace feather
{
    namespace command
    {
        /*
         * Here you will add enums for every
         * command that you need to call.
         * The first enum must always be N.
         * example:
         * enum Command { N=0, IMPORT_OBJ, EXPORT_OBJ };
         */
        enum Command { N=0 };

        /*
         * Here you will add all the commands that
         * will be linked the the above enums.
         * You need to have a function for each
         * enum above and they need to be formated
         * this way:
         *      status [name](parameter::ParameterList params)
         *      {
         *              ...
         *      };
         * example:
         *      status import_obj(parameter::ParameterList params)
         *      {
         *              // code to import obj file
         *      };
         */
        /*
        status import_obj(parameter::ParameterList params) {
            std::string filename = params.getParameterValue<std::string>("filename");
            bool selection = params.getParameterValue<bool>("selection");

            std::cout << "running import_obj command\nfilename: " << filename << "\n\tselection: " << selection << std::endl;
            return status();
        };

        status export_obj(parameter::ParameterList params) {
            std::cout << "running export_obj command" << std::endl;
            return status();
        };
        */
    } // namespace command

} // namespace feather

/*
 * ADD_COMMAND([name],[enum],[function])
 * Add all the commands that can be called from
 * the qml interface.
 *
 * example:
 * ADD_COMMAND("import_obj",IMPORT_OBJ,import_obj)
 */
//ADD_COMMAND("import_obj",IMPORT_OBJ,import_obj)
//ADD_COMMAND("export_obj",EXPORT_OBJ,export_obj)


/*
 * ADD_PARAMETER([cmd_enum],[param_key],[param_name])
 * Add all the commands that can be called from
 * the qml interface.
 *
 * example:
 * ADD_PARAMETER(command::IMPORT_OBJ,1,parameter::String,"filename")
 */
//ADD_PARAMETER(command::IMPORT_OBJ,1,parameter::String,"filename")
//ADD_PARAMETER(command::IMPORT_OBJ,2,parameter::Bool,"selection")


/*
 * INIT_COMMAND_CALLS([enum])
 * Make the command callable by the core.
 * This make takes the last enum of the
 * feather::command::Command enum.
 *
 * example:
 * INIT_COMMAND_CALLS(EXPORT_OBJ)
 */
INIT_COMMAND_CALLS(N)
