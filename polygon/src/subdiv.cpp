/**********************************************************************
 *
 * Filename: subdiv.cpp
 *
 * Description: Uses OpenSubdiv API to create smooth meshes from low resolution meshes 
 *
 * Copyright (C) 2016 Richard Layman, rlayman2000@yahoo.com 
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

#include "subdiv.hpp"

using namespace feather;

void subdiv::Shape::loadMesh(
        feather::FMesh* mesh,
        feather::FVertexIndiceGroupWeightArray* vertexWeights,
        feather::FVertexIndiceGroupWeightArray* edgeWeights
        )
{
    for(auto v : mesh->v){
        verts.push_back(v.x);
        verts.push_back(v.y);
        verts.push_back(v.z);
    }

    for(auto vn : mesh->vn){
        normals.push_back(vn.x);
        normals.push_back(vn.y);
        normals.push_back(vn.z);
    }


    nvertsPerFace = mesh->verts_per_face();
    faceverts = mesh->vert_indices_per_face();

    // For now we'll use Chaikin as the default smoothing method but this will be adjustable in the future
    tag *smoothmethod = new tag();
    smoothmethod->name="creasemethod";
    smoothmethod->stringargs.push_back("chaikin");
    tags.push_back(smoothmethod);

    // fill in weighted vertex
    std::for_each(vertexWeights->begin(), vertexWeights->end(), [this] (feather::FVertexIndiceGroupWeight group){
            tag *t = new tag();
            t->name="corner";
            for(auto v : group.v)
                t->intargs.push_back(v);
            t->floatargs.push_back(group.weight);
            tags.push_back(t);
            });

    // fill in weighted vertex
    std::for_each(edgeWeights->begin(), edgeWeights->end(), [this] (feather::FVertexIndiceGroupWeight group){
            tag *t = new tag();
            t->name="crease";
            for(auto v : group.v)
                t->intargs.push_back(v);
            t->floatargs.push_back(group.weight);
            tags.push_back(t);
            });

    // TESTING
    /*
    // crease testing
    // method
    tag *t0 = new tag();
    t0->name="creasemethod";
    t0->stringargs.push_back("chaikin");

    // edge creases
    // the two vertex must connect our there will be a seq fault
    tag *t1 = new tag();
    t1->name="crease";
    t1->intargs.push_back(0);
    t1->intargs.push_back(1);

    t1->intargs.push_back(2);
    t1->intargs.push_back(3);

    t1->intargs.push_back(0);
    t1->intargs.push_back(3);

    t1->intargs.push_back(1);
    t1->intargs.push_back(2);

    t1->floatargs.push_back(weight);

    // vertex crease
    tag *t2 = new tag();
    t2->name="corner";
    t2->intargs.push_back(0);
    t2->intargs.push_back(1);
    t2->intargs.push_back(2);
    t2->intargs.push_back(3);
    t2->floatargs.push_back(weight);

    tags.push_back(t0);
    tags.push_back(t1);
    tags.push_back(t2);
    */
}

std::string subdiv::Shape::tag::genTag() const {
    std::stringstream t;

    t<<"\"t \""<<name<<"\" ";

    t<<intargs.size()<<"/"<<floatargs.size()<<"/"<<stringargs.size()<<" ";

    std::copy(intargs.begin(), intargs.end(), std::ostream_iterator<int>(t));
    t<<" ";

    std::copy(floatargs.begin(), floatargs.end(), std::ostream_iterator<float>(t));
    t<<" ";

    std::copy(stringargs.begin(), stringargs.end(), std::ostream_iterator<std::string>(t));
    t<<"\\n\"\n";

    return t.str();
}


void subdiv::subdiv_mesh(
        unsigned int maxlevel,
        feather::FMesh *meshIn,
        feather::FMesh *meshOut,
        feather::FVertexIndiceGroupWeightArray *vertexWeights,
        feather::FVertexIndiceGroupWeightArray *edgeWeights
        )
{
    Shape *shape = new Shape();

    shape->loadMesh(meshIn,vertexWeights,edgeWeights);

    // create Far mesh (topology)
    OpenSubdiv::Sdc::SchemeType sdctype = GetSdcType(*shape);
    OpenSubdiv::Sdc::Options sdcoptions = GetSdcOptions(*shape);

    typedef OpenSubdiv::Sdc::Options SdcOptions;

    SdcOptions::FVarLinearInterpolation g_fvarInterpolation = SdcOptions::FVAR_LINEAR_ALL;

    sdcoptions.SetFVarLinearInterpolation(g_fvarInterpolation);

    // Instantiate a FarTopologyRefiner from the descriptor
    OpenSubdiv::Far::TopologyRefiner * refiner = OpenSubdiv::Far::TopologyRefinerFactory<Shape>::Create(*shape, OpenSubdiv::Far::TopologyRefinerFactory<Shape>::Options(sdctype, sdcoptions));

    bool g_Adaptive = false;

    if (g_Adaptive) {
        OpenSubdiv::Far::TopologyRefiner::AdaptiveOptions options(maxlevel);
        options.useSingleCreasePatch = false;
        refiner->RefineAdaptive(options);
    } else {
        OpenSubdiv::Far::TopologyRefiner::UniformOptions options(maxlevel);
        options.fullTopologyInLastLevel = true;
        refiner->RefineUniform(options);
    }

    // Allocate a buffer for vertex primvar data. The buffer length is set to
    // be the sum of all children vertices up to the highest level of refinement.
    std::vector<subdiv::Vertex> vbuffer(refiner->GetNumVerticesTotal());
    subdiv::Vertex *verts = &vbuffer[0];


    // Initialize coarse mesh positions
    int nCoarseVerts = meshIn->v.size();
    for (int i=0; i<nCoarseVerts; ++i) {
        verts[i].SetPosition(meshIn->v[i].x, meshIn->v[i].y, meshIn->v[i].z);
        //verts[i].SetPosition(g_verts[i][0], g_verts[i][1], g_verts[i][2]);
    }

    // Interpolate vertex primvar data
    OpenSubdiv::Far::PrimvarRefiner primvarRefiner(*refiner);

    subdiv::Vertex *src = verts;
    for (int level = 1; level <= maxlevel; ++level) {
        subdiv::Vertex *dst = src + refiner->GetLevel(level-1).GetNumVertices();
        primvarRefiner.Interpolate(level, src, dst);
        src = dst;
    }

    // Approximate normals
    OpenSubdiv::Far::TopologyLevel const & refLastLevel = refiner->GetLevel(maxlevel);
    int nverts = refLastLevel.GetNumVertices();
    int nfaces = refLastLevel.GetNumFaces();
    int firstOfLastVerts = refiner->GetNumVerticesTotal() - nverts;

    std::vector<Vertex> normals(nverts);
    enum subdiv::NormalApproximation normalApproximation = subdiv::CrossTriangle;

    // Different ways to approximate smooth normals
    //
    // For details check the description at the beginning of the file
    if (normalApproximation == subdiv::Limit) {

        // Approximation using the normal at the limit with verts that are 
        // not at the limit
        //
        // For details check the description at the beginning of the file

        std::vector<Vertex> fineLimitPos(nverts);
        std::vector<Vertex> fineDu(nverts);
        std::vector<Vertex> fineDv(nverts);

        primvarRefiner.Limit(&verts[firstOfLastVerts], fineLimitPos, fineDu, fineDv);
        
        for (int vert = 0; vert < nverts; ++vert) {
            float const * du = fineDu[vert].GetPosition();
            float const * dv = fineDv[vert].GetPosition();
            
            float norm[3];
            subdiv::cross(du, dv, norm);
            normals[vert].SetPosition(norm[0], norm[1], norm[2]);
        }

    } else if (normalApproximation == subdiv::CrossQuad) {

        // Approximate smooth normals by accumulating normal vectors computed as
        // the cross product of two vectors generated by the 4 verts that 
        // form each quad
        //
        // For details check the description at the beginning of the file

        for (int f = 0; f < nfaces; f++) {
            OpenSubdiv::Far::ConstIndexArray faceVertices = refLastLevel.GetFaceVertices(f);

            // We will use the first three verts to calculate a normal
            const float * v0 = verts[ firstOfLastVerts + faceVertices[0] ].GetPosition();
            const float * v1 = verts[ firstOfLastVerts + faceVertices[1] ].GetPosition();
            const float * v2 = verts[ firstOfLastVerts + faceVertices[2] ].GetPosition();
            const float * v3 = verts[ firstOfLastVerts + faceVertices[3] ].GetPosition();

            // Calculate the cross product between the vectors formed by v1-v0 and
            // v2-v0, and then normalize the result
            float normalCalculated [] = {0.0,0.0,0.0};
            float a[3] = { v2[0]-v0[0], v2[1]-v0[1], v2[2]-v0[2] };
            float b[3] = { v3[0]-v1[0], v3[1]-v1[1], v3[2]-v1[2] };          
            subdiv::cross(a, b, normalCalculated);
            subdiv::normalize(normalCalculated);

            // Accumulate that normal on all verts that are part of that face
            for(int vInFace = 0; vInFace < faceVertices.size() ; vInFace++ ) {

                int vertexIndex = faceVertices[vInFace];
                normals[vertexIndex]._position[0] += normalCalculated[0];
                normals[vertexIndex]._position[1] += normalCalculated[1];
                normals[vertexIndex]._position[2] += normalCalculated[2];
            }
        }

    } else if (normalApproximation == subdiv::CrossTriangle) {

        // Approximate smooth normals by accumulating normal vectors computed as
        // the cross product of two vectors generated by 3 verts of the quad
        //
        // For details check the description at the beginning of the file

        for (int f = 0; f < nfaces; f++) {
            OpenSubdiv::Far::ConstIndexArray faceVertices = refLastLevel.GetFaceVertices(f);

            // We will use the first three verts to calculate a normal
            const float * v0 = verts[ firstOfLastVerts + faceVertices[0] ].GetPosition();
            const float * v1 = verts[ firstOfLastVerts + faceVertices[1] ].GetPosition();
            const float * v2 = verts[ firstOfLastVerts + faceVertices[2] ].GetPosition();

            // Calculate the cross product between the vectors formed by v1-v0 and
            // v2-v0, and then normalize the result
            float normalCalculated [] = {0.0,0.0,0.0};
            float a[3] = { v1[0]-v0[0], v1[1]-v0[1], v1[2]-v0[2] };
            float b[3] = { v2[0]-v0[0], v2[1]-v0[1], v2[2]-v0[2] };
            subdiv::cross(a, b, normalCalculated);
            subdiv::normalize(normalCalculated);

            // Accumulate that normal on all verts that are part of that face
            for(int vInFace = 0; vInFace < faceVertices.size() ; vInFace++ ) {

                int vertexIndex = faceVertices[vInFace];
                normals[vertexIndex]._position[0] += normalCalculated[0];
                normals[vertexIndex]._position[1] += normalCalculated[1];
                normals[vertexIndex]._position[2] += normalCalculated[2];
            }
        }
    }

    // Finally we just need to normalize the accumulated normals
    for (int vert = 0; vert < nverts; ++vert) {
        subdiv::normalize(&normals[vert]._position[0]);
    }
 
    // add vertex's
    for (int vert = 0; vert < nverts; ++vert) {
        float const * pos = verts[firstOfLastVerts + vert].GetPosition();
        meshOut->v.push_back(FVertex3D(pos[0],pos[1],pos[2]));
    }

    // add normals
    for (int vert = 0; vert < nverts; ++vert) {
        float const * pos = normals[vert].GetPosition();
        meshOut->vn.push_back(FVertex3D(pos[0],pos[1],pos[2]));
    }

    // add faces
    for (int face = 0; face < nfaces; ++face) {
        OpenSubdiv::Far::ConstIndexArray fverts = refLastLevel.GetFaceVertices(face);

        // all refined Catmark faces should be quads
        assert(fverts.size()==4);

        FFace _face;
        for (int vert=0; vert<fverts.size(); ++vert) {
            _face.push_back(FFacePoint(fverts[vert],0,fverts[vert]));
        }

        meshOut->f.push_back(_face);
    }

    delete shape;
    shape = 0;

}


