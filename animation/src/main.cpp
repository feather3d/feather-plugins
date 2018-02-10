/**********************************************************************
 *
 * Filename: main.cpp
 *
 * Description: Plugin contains common animation nodes and commands.
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
//#include <feather/scenegraph.hpp>
#include <feather/draw.hpp>
#include <feather/plugin.hpp>
#include <feather/tools.hpp>
#include <feather/curve.hpp>
#include <QColor>

#ifdef __cplusplus
extern "C" {
#endif
    C_PLUGIN_WRAPPER()
#ifdef __cplusplus
}
#endif

using namespace feather;

#define ANIMATION_KEYTRACK 420
#define ANIMATION_BONE 421
#define ANIMATION_MORPH 426


PLUGIN_INIT("Animation","Animation nodes and commands","Richard Layman",ANIMATION_KEYTRACK,ANIMATION_MORPH)


/*
 ***************************************
 *            ANIMATION TRACK          *
 ***************************************
*/

// time 
ADD_FIELD_TO_NODE(ANIMATION_KEYTRACK,FReal,field::Real,field::connection::In,0.0,1)
// keys 
ADD_FIELD_TO_NODE(ANIMATION_KEYTRACK,FKeyArray,field::KeyArray,field::connection::In,FKeyArray(),2)
// type 
ADD_FIELD_TO_NODE(ANIMATION_KEYTRACK,FInt,field::Int,field::connection::In,field::Real,3)
// value 
ADD_FIELD_TO_NODE(ANIMATION_KEYTRACK,FReal,field::Real,field::connection::Out,0.0,4)

namespace feather
{

    DO_IT(ANIMATION_KEYTRACK)
    {
        typedef field::Field<FInt>*  IntField;
        typedef field::Field<FReal>*  RealField;
        typedef field::Field<FKeyArray>*  KeyArrayField;

        RealField time;
        KeyArrayField keys;
        IntField keytype;
        RealField value;

        for(auto f : fields){
            if(f->id == 1)
                time = static_cast<RealField>(f);
            if(f->id == 2)
                keys = static_cast<KeyArrayField>(f);
            if(f->id == 3)
                keytype = static_cast<IntField>(f);
            if(f->id == 4)
                value = static_cast<RealField>(f);
        }

        // this is not needed at the moment but will probably be needed later
        /*
        if(!keys->connections.size()){
            std::cout << "no keys connected\n";
            return status();
        }
        */

        std::cout << keys->connections.size() << " keys connected\n";
        std::cout << keys->value.size() << " keys in track\n";

        if(time->connections.size())
            time->value = static_cast<RealField>(plugin::get_node_field_base(time->connections.at(0).puid,time->connections.at(0).pfid))->value;

        if(keys->connections.size())
            keys->value = static_cast<KeyArrayField>(plugin::get_node_field_base(keys->connections.at(0).puid,keys->connections.at(0).pfid))->value;

        if(keytype->connections.size())
            keytype->value = static_cast<IntField>(plugin::get_node_field_base(keytype->connections.at(0).puid,keytype->connections.at(0).pfid))->value;

        if(value->connections.size())
            value->value = static_cast<RealField>(plugin::get_node_field_base(value->connections.at(0).puid,value->connections.at(0).pfid))->value;
 
        // these are the key values from before and after the input time
        // TODO - currently we're only doing linear calculations

        FKey* minKey = nullptr;
        FKey* maxKey = nullptr;

        FReal minDeltaTime;
        FReal maxDeltaTime; 

        std::cout << "KEYTRACK UPDATE\n";

        // If there are no keys, don't bother doing anymore
        if(!keys->value.size()) {
            value->value=0;
            return status();
        }

        // if there is only one key, return that value
        if(keys->value.size() == 1) {
            value->value=keys->value[0].value;
            return status();
        }
 
        int i = 0;
        for(auto key : keys->value) {
            std::cout << "\tkey value:" << key.value << " time:" << key.time << std::endl;
            if(time->value == key.time){
                if(keytype->value == field::Int){
                    value->value = key.value;
                    std::cout << "ival = " << key.value;
                    std::cout << "1 KEY TRACK OUT VALUE = " << value->value << std::endl;
                 }
                if(keytype->value == field::Real){
                    value->value = key.value;
                    std::cout << "rval = " << key.value;
                    std::cout << "2 KEY TRACK OUT VALUE = " << value->value << std::endl;
                 }
                // we have the value, no need to go any further
                return status();
            }

            else if(key.time < time->value) {
                if(!minKey){
                    minDeltaTime = time->value - key.time;
                    std::cout << "setting minConn 1: time:" << key.time << std::endl;
                    minKey = &keys->value[i];
                } else {
                    if(key.time > (time->value - minDeltaTime)){
                        minKey = &keys->value[i];
                        std::cout << "setting minConn 2: time:" << key.time << std::endl;
                        minDeltaTime = time->value - key.time;
                    }
                }
            }

            else if(key.time > time->value) {
                if(!maxKey){
                    maxDeltaTime = key.time - time->value;
                    std::cout << "setting maxConn 1: time:" << key.time << std::endl;
                    maxKey = &keys->value[i];
                } else {
                    if(key.time < (time->value + maxDeltaTime)){
                        maxKey = &keys->value[i];
                        std::cout << "setting maxConn 2: time:" << key.time << std::endl;
                        maxDeltaTime = key.time - time->value;
                    }
                }
            }
            i++;

            //std::cout << "key for uid:" << conn.puid << ", ival=" << ival->value << ", rval=" << rval->value << ", time=" << time->value << std::endl;
        }

        // if the min or max key was not set, set if to the input key
        if(!minKey)
            minKey = &keys->value[0];

        if(!maxKey)
            maxKey = &keys->value[keys->value.size()-1];

        // If we've made it this far, the value has to be calculated.
        // If there are no keys before the time, we'll use the first key's value
        // If there are no keys after the time, we'll use the last key's value
        // calculate the output value

        if(!minKey) {
            if(time->value == maxKey->time){
                if(keytype->value == field::Int){
                    value->value = maxKey->value;
                    //std::cout << "3 KEY TRACK OUT VALUE = " << value->value << std::endl;
                 }
                if(keytype->value == field::Real){
                    value->value = maxKey->value;
                    //std::cout << "4 KEY TRACK OUT VALUE = " << value->value << std::endl;
                 }
                value->value = maxKey->value;
                // we have the value, no need to go any further
                return status();
            }
        }
        else if(!maxKey) {
            if(time->value == minKey->time){
                if(keytype->value == field::Int){
                    value->value = minKey->value;
                    //std::cout << "5 KEY TRACK OUT VALUE = " << value->value << std::endl;
                 }
                if(keytype->value == field::Real){
                    value->value = minKey->value;
                    //std::cout << "6 KEY TRACK OUT VALUE = " << value->value << std::endl;
                 }
                value->value = minKey->value;
                // we have the value, no need to go any further
                return status();
            }
        }
        else {
            FReal minvalue;
            FReal maxvalue;

            // Yea, I know a lot of this is not needed and I'll fix it
            if(keytype->value == field::Int){
                minvalue = minKey->value;
                maxvalue = maxKey->value;
 
            }
            if(keytype->value == field::Real){
                minvalue = minKey->value;
                maxvalue = maxKey->value;
            }
            
            minvalue = minKey->value;
            maxvalue = maxKey->value;
 
            // do a single rise run calculation to set the out value
            FReal rise = maxvalue - minvalue;
            FReal run = maxKey->time - minKey->time;
            FReal slope = rise/run;
            // linear calculated value
            //value->value = minvalue + ((time->value - minKey->time) * slope);            
            // bezier calculated value
            value->value = feather::curve::get_cubic_bezier_x(FPoint2D(minKey->time,minKey->value),minKey->outcp,maxKey->incp,FPoint2D(maxKey->time,maxKey->value),time->value,24.0);

            /*
            std::cout << "On step 7, "
                << " minVal:" << minvalue
                << " minTime:" << minKey->time 
                << " maxVal:" << maxvalue
                << " maxTime:" << maxKey->time
                << " rise:" << rise
                << " run:" << run
                << " slope:" << slope
                << std::endl
                ;
            std::cout << "7 KEY TRACK OUT VALUE = " << value->value << std::endl;
            */
            value->update = true;
        }

        /* 
        std::cout << "The KeyTrack Node Keys:\n";
        for ( auto key : keys->value )
            std::cout << "\ttime:" << key.time << " value:" << key.value << std::endl;
        */

        return status();
    };

} // namespace feather

NODE_INIT(ANIMATION_KEYTRACK,node::Animation,"track.svg")



/*
 ***************************************
 *           ANIMATION BONE            *
 ***************************************
*/

// FIELDS
// currently bones have no fields but will later when other features are added

namespace feather
{

    DO_IT(ANIMATION_BONE)
    { 
        return status();
    };

    /*
    DRAW_IT(ANIMATION_BONE)
    {
        std::cout << "ANIMATION_BONE DRAW IT\n";
        return status();    
    };
    */

} // namespace feather

NODE_INIT(ANIMATION_BONE,node::Animation,"bone.svg")



/*
 ***************************************
 *           ANIMATION MORPH           *
 ***************************************
*/

// FIELDS

// IN

// base 
ADD_FIELD_TO_NODE(ANIMATION_MORPH,FMesh,field::MeshArray,field::connection::In,FMesh(),1)
// targets 
ADD_FIELD_TO_NODE(ANIMATION_MORPH,FMeshArray,field::MeshArray,field::connection::In,FMeshArray(),2)
// weights
ADD_FIELD_TO_NODE(ANIMATION_MORPH,FRealArray,field::Real,field::connection::In,std::vector<FReal>(),3)

// OUT

// mesh 
ADD_FIELD_TO_NODE(ANIMATION_MORPH,FMesh,field::Mesh,field::connection::Out,FMesh(),4)

namespace feather
{

    DO_IT(ANIMATION_MORPH)
    { 
        // get fields
        GET_FIELD_DATA(1,FMesh,baseIn,field::connection::In)
        GET_FIELD_ARRAY_DATA(2,FMeshArray,targetsIn,field::connection::In)
        GET_FIELD_ARRAY_DATA(3,FRealArray,weightsIn,field::connection::In)
        GET_FIELD_DATA(4,FMesh,meshOut,field::connection::Out)

        if(weightsIn->update)
        {
            // clear the meshOut
            meshOut->value.v.clear();
            meshOut->value.st.clear();
            meshOut->value.vn.clear();
            meshOut->value.f.clear();
            meshOut->update = true;
           
            if(!baseIn->connections.size())
                return status(FAILED,"no target meshes in morph node.");

            // if we've made it this far, we will blend the mesh to the final mesh
            meshOut->value = baseIn->value;
 
            // if there is only the base mesh or there are no weights, just output that
            if(!targetsIn->connections.size())
            {
                return status();
            }
   
            uint id=0;

            for(auto conn : targetsIn->connections)
            {
                FMesh* pMesh = &static_cast<field::Field<FMesh>*>(plugin::get_node_field_base(conn.puid,conn.pfid))->value;
                FReal weight=0.0;
                if(!weightsIn->connections.size() || weightsIn->connections.size() < id){
                    if(weightsIn->value.size() <= id) {
                        weight = 0.0;
                        weightsIn->value.push_back(weight);
                    } else {
                        weight = weightsIn->value[id];
                    }
                } else {
                    // use the connection weight value
                    weight = static_cast<field::Field<FReal>*>(plugin::get_node_field_base(weightsIn->connections.at(id).puid,weightsIn->connections.at(id).pfid))->value;
                }
                for(uint i=0; i < meshOut->value.v.size(); i++){
                    meshOut->value.v.at(i).x = meshOut->value.v.at(i).x + ( ( pMesh->v.at(i).x - baseIn->value.v.at(i).x ) * weight );
                    meshOut->value.v.at(i).y = meshOut->value.v.at(i).y + ( ( pMesh->v.at(i).y - baseIn->value.v.at(i).y ) * weight );
                    meshOut->value.v.at(i).z = meshOut->value.v.at(i).z + ( ( pMesh->v.at(i).z - baseIn->value.v.at(i).z ) * weight );
                }
                weight=0.0;
                id++; 
            } 
        }

        return status();
    };

    /*
    DRAW_IT(ANIMATION_MORPH)
    {
        std::cout << "ANIMATION_MORPH DRAW IT\n";
        ADD_SHADED_MESH(3)
        return status();    
    };
    */

} // namespace feather

NODE_INIT(ANIMATION_MORPH,node::Animation,"morph.svg")



/*
 ***************************************
 *              COMMANDS               *
 ***************************************
*/

namespace feather
{
    namespace command
    {
        enum Command { N=0, ADD_KEY };

        // add key
        status add_key(parameter::ParameterList params) {
            FReal time; 
            bool pass = params.getParameterValue<FReal>("time",time);
            if(!pass)
                return status(FAILED,"time parameter failed");

            FReal value; 
            pass = params.getParameterValue<FReal>("value",value);
            if(!pass)
                return status(FAILED,"value parameter failed");

            unsigned int uid; 
            pass = params.getParameterValue<unsigned int>("uid",uid);
            if(!pass)
                return status(FAILED,"uid parameter failed");

            unsigned int fid; 
            pass = params.getParameterValue<unsigned int>("fid",fid);
            if(!pass)
                return status(FAILED,"fid parameter failed");

            // get the field base and check if it's already connected to a track
            field::FieldBase* field = plugin::get_node_field_base(uid,fid);
            // if nothing is connected
            typedef field::Field<FKeyArray>* KeyArrayField;
 
            status p;
            if(!field->connections.size()) {
                // make a track and add the value at the current time
                unsigned int track = plugin::add_node(ANIMATION_KEYTRACK,"track",p);
                // out value connection
                plugin::connect(track,4,uid,fid);
                // parent child connection
                plugin::connect(uid,202,track,201);
                // in time connection
                plugin::connect(1,7,track,1);
                static_cast<KeyArrayField>(plugin::get_node_field_base(track,2))->value.push_back(FKey(value,time,curve::Bezier,FPoint2D(time-1.0,value),curve::Bezier,FPoint2D(time+1.0,value)));
                static_cast<KeyArrayField>(plugin::get_node_field_base(track,2))->update = true;
            } else {
                field::FieldBase* conn = plugin::get_node_field_base(field->connections.at(0).puid,2); 
                std::cout << "Looking for animation keytrack on connection 0 - uid:" << field->connections.at(0).puid << " fid:" << field->connections.at(0).pfid << " nid:" << conn->id << std::endl;
                // we have a connection, and most likely it's a track but let's check just to be safe
                if(plugin::get_node_id(field->connections.at(0).puid,p) == ANIMATION_KEYTRACK){
                    std::cout << "add_key command is adding key at time:" << time << " value:" << value << std::endl;
                    static_cast<KeyArrayField>(conn)->value.push_back(FKey(value,time,curve::Bezier,FPoint2D(time-1.0,value),curve::Bezier,FPoint2D(time+1.0,value)));
                    static_cast<KeyArrayField>(conn)->update = true;
                } else {
                    // it's not a key track so let's get out of here
                    std::cout << "can't add key because something other then a key track is attached to the field\n";
                    return status(FAILED,"something other then a key track is attached to the field");
                }
            } 

            std::cout << "added key to field\n";

            // update scenegraph            
            plugin::update();
            
            std::cout << "add_key command found\n";
 
            return status();
        };

    } // namespace command

} // namespace feather

ADD_COMMAND("add_key",ADD_KEY,add_key)
ADD_PARAMETER(command::ADD_KEY,1,parameter::Real,"time")
ADD_PARAMETER(command::ADD_KEY,2,parameter::Real,"value")
ADD_PARAMETER(command::ADD_KEY,3,parameter::Int,"fid")
ADD_PARAMETER(command::ADD_KEY,4,parameter::Int,"uid")

INIT_COMMAND_CALLS(ADD_KEY)
