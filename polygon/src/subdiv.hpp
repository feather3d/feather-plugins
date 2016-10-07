/**********************************************************************
 *
 * Filename: subdiv.hpp
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
#ifndef SUBDIV_HPP
#define SUBDIV_HPP

#include <feather/types.hpp>
#include <opensubdiv/far/topologyDescriptor.h>
#include <opensubdiv/far/topologyRefinerFactory.h>
#include <opensubdiv/far/primvarRefiner.h>

namespace subdiv
{
    // Approximation methods for smooth normal computations
    enum NormalApproximation
    {
        CrossTriangle,
        CrossQuad,
        Limit
    };

    // Returns the normalized version of the input vector
    inline void
        normalize(float *n) {
            float rn = 1.0f/sqrtf(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
            n[0] *= rn;
            n[1] *= rn;
            n[2] *= rn;
        }

    // Returns the cross product of \p v1 and \p v2.                                
    inline void cross(float const *v1, float const *v2, float* vOut)
    {                                                                                
        vOut[0] = v1[1] * v2[2] - v1[2] * v2[1];
        vOut[1] = v1[2] * v2[0] - v1[0] * v2[2];
        vOut[2] = v1[0] * v2[1] - v1[1] * v2[0];
    }


    struct Vertex {

        // Minimal required interface ----------------------
        Vertex() { }

        Vertex(Vertex const & src) {
            _position[0] = src._position[0];
            _position[1] = src._position[1];
            _position[2] = src._position[2];
        }

        void Clear( void * =0 ) {
            _position[0]=_position[1]=_position[2]=0.0f;
        }

        void AddWithWeight(Vertex const & src, float weight) {
            _position[0]+=weight*src._position[0];
            _position[1]+=weight*src._position[1];
            _position[2]+=weight*src._position[2];
        }

        // Public interface ------------------------------------
        void SetPosition(float x, float y, float z) {
            _position[0]=x;
            _position[1]=y;
            _position[2]=z;
        }

        const float * GetPosition() const {
            return _position;
        }

        float _position[3];
    };


    enum Scheme {
        kBilinear=0,
        kCatmark,
        kLoop
    };


    struct Shape {
        Shape() : scheme(kCatmark), isLeftHanded(false) { }
        ~Shape() { tags.clear(); }

        struct tag {

            static tag * parseTag(char const * stream);

            std::string genTag() const;

            std::string              name;
            std::vector<int>         intargs;
            std::vector<float>       floatargs;
            std::vector<std::string> stringargs;
        };


        void loadMesh(
                feather::FMesh* mesh,
                feather::FVertexIndiceGroupWeightArray *vertexWeights,
                feather::FVertexIndiceGroupWeightArray *edgeWeights
                );

        int GetNumVertices() const { return (int)verts.size()/3; }

        int GetNumFaces() const { return (int)nvertsPerFace.size(); }

        bool HasUV() const { return not (uvs.empty() or faceuvs.empty()); }

        int GetFVarWidth() const { return HasUV() ? 2 : 0; }

        std::vector<float>      verts;
        std::vector<float>      uvs;
        std::vector<float>      normals;
        std::vector<int>        nvertsPerFace;
        std::vector<int>        faceverts;
        std::vector<int>        faceuvs;
        std::vector<int>        facenormals;
        std::vector<tag *>      tags;
        Scheme                  scheme;
        bool                    isLeftHanded;
    };


    inline OpenSubdiv::Sdc::SchemeType
        GetSdcType(Shape const & shape) {

            OpenSubdiv::Sdc::SchemeType type=OpenSubdiv::Sdc::SCHEME_CATMARK;

            switch (shape.scheme) {
                case kBilinear: type = OpenSubdiv::Sdc::SCHEME_BILINEAR; break;
                case kCatmark : type = OpenSubdiv::Sdc::SCHEME_CATMARK; break;
                case kLoop    : type = OpenSubdiv::Sdc::SCHEME_LOOP; break;
            }
            return type;
        }

    inline OpenSubdiv::Sdc::Options
        GetSdcOptions(Shape const & shape) {

            typedef OpenSubdiv::Sdc::Options Options;

            Options result;

            result.SetVtxBoundaryInterpolation(Options::VTX_BOUNDARY_EDGE_ONLY);
            result.SetCreasingMethod(Options::CREASE_UNIFORM);
            result.SetTriangleSubdivision(Options::TRI_SUB_CATMARK);

            for (int i=0; i<(int)shape.tags.size(); ++i) {

                Shape::tag * t = shape.tags[i];

                if (t->name=="interpolateboundary") {
                    if ((int)t->intargs.size()!=1) {
                        printf("expecting 1 integer for \"interpolateboundary\" tag n. %d\n", i);
                        continue;
                    }
                    switch( t->intargs[0] ) {
                        case 0 : result.SetVtxBoundaryInterpolation(Options::VTX_BOUNDARY_NONE); break;
                        case 1 : result.SetVtxBoundaryInterpolation(Options::VTX_BOUNDARY_EDGE_AND_CORNER); break;
                        case 2 : result.SetVtxBoundaryInterpolation(Options::VTX_BOUNDARY_EDGE_ONLY); break;
                        default: printf("unknown interpolate boundary : %d\n", t->intargs[0] ); break;
                    }
                } else if (t->name=="facevaryinginterpolateboundary") {
                    if ((int)t->intargs.size()!=1) {
                        printf("expecting 1 integer for \"facevaryinginterpolateboundary\" tag n. %d\n", i);
                        continue;
                    }
                    switch( t->intargs[0] ) {
                        case 0 : result.SetFVarLinearInterpolation(Options::FVAR_LINEAR_NONE); break;
                        case 1 : result.SetFVarLinearInterpolation(Options::FVAR_LINEAR_CORNERS_ONLY); break;
                        case 2 : result.SetFVarLinearInterpolation(Options::FVAR_LINEAR_CORNERS_PLUS1); break;
                        case 3 : result.SetFVarLinearInterpolation(Options::FVAR_LINEAR_CORNERS_PLUS2); break;
                        case 4 : result.SetFVarLinearInterpolation(Options::FVAR_LINEAR_BOUNDARIES); break;
                        case 5 : result.SetFVarLinearInterpolation(Options::FVAR_LINEAR_ALL); break;
                        default: printf("unknown interpolate boundary : %d\n", t->intargs[0] ); break;
                    }
                } else if (t->name=="facevaryingpropagatecorners") {
                    if ((int)t->intargs.size()==1) {
                        // XXXX no propagate corners in Options
                        assert(0);
                    } else
                        printf( "expecting single int argument for \"facevaryingpropagatecorners\"\n" );
                } else if (t->name=="creasemethod") {

                    if ((int)t->stringargs.size()==0) {
                        printf("the \"creasemethod\" tag expects a string argument\n");
                        continue;
                    }

                    if (t->stringargs[0]=="normal") {
                        result.SetCreasingMethod(Options::CREASE_UNIFORM);
                    } else if (t->stringargs[0]=="chaikin") {
                        result.SetCreasingMethod(Options::CREASE_CHAIKIN);
                    } else {
                        printf("the \"creasemethod\" tag only accepts \"normal\" or \"chaikin\" as value (%s)\n", t->stringargs[0].c_str());
                    }
                } else if (t->name=="smoothtriangles") {

                    if (shape.scheme!=kCatmark) {
                        printf("the \"smoothtriangles\" tag can only be applied to Catmark meshes\n");
                        continue;
                    }
                    if (t->stringargs[0]=="catmark") {
                        result.SetTriangleSubdivision(Options::TRI_SUB_CATMARK);
                    } else if (t->stringargs[0]=="smooth") {
                        result.SetTriangleSubdivision(Options::TRI_SUB_SMOOTH);
                    } else {
                        printf("the \"smoothtriangles\" tag only accepts \"catmark\" or \"smooth\" as value (%s)\n", t->stringargs[0].c_str());
                    }
                }
            }

            return result;
        }


    void subdiv_mesh(
            unsigned int maxlevel,
            feather::FMesh *meshIn,
            feather::FMesh *meshOut,
            feather::FVertexIndiceGroupWeightArray *vertexWeights,
            feather::FVertexIndiceGroupWeightArray *edgeWeights
            );

} // namespace subdiv

namespace OpenSubdiv {
    namespace OPENSUBDIV_VERSION {

        namespace Far {

            template <>
                inline bool
                TopologyRefinerFactory<subdiv::Shape>::resizeComponentTopology(
                        Far::TopologyRefiner & refiner, subdiv::Shape const & shape) {

                    int nfaces = shape.GetNumFaces(),
                        nverts = shape.GetNumVertices();

                    setNumBaseFaces(refiner, nfaces);
                    for (int i=0; i<nfaces; ++i) {

                        int nv = shape.nvertsPerFace[i];
                        setNumBaseFaceVertices(refiner, i, nv);
                    }

                    // Vertices and vert-faces and vert-edges
                    setNumBaseVertices(refiner, nverts);

                    return true;
                }

            //----------------------------------------------------------
            template <>
                inline bool
                TopologyRefinerFactory<subdiv::Shape>::assignComponentTopology(
                        Far::TopologyRefiner & refiner, subdiv::Shape const & shape) {

                    { // Face relations:
                        int nfaces = getNumBaseFaces(refiner);

                        for (int i=0, ofs=0; i < nfaces; ++i) {

                            Far::IndexArray dstFaceVerts = getBaseFaceVertices(refiner, i);

                            if (shape.isLeftHanded) {
                                dstFaceVerts[0] = shape.faceverts[ofs++];
                                for (int j=dstFaceVerts.size()-1; j>0; --j) {
                                    dstFaceVerts[j] = shape.faceverts[ofs++];
                                }
                            } else {
                                for (int j=0; j<dstFaceVerts.size(); ++j) {
                                    dstFaceVerts[j] = shape.faceverts[ofs++];
                                }
                            }
                        }
                    }
                    return true;
                }

            //----------------------------------------------------------
            template <>
                inline bool
                TopologyRefinerFactory<subdiv::Shape>::assignFaceVaryingTopology(
                        Far::TopologyRefiner & refiner, subdiv::Shape const & shape) {

                    // UV layyout (we only parse 1 channel)
                    if (not shape.faceuvs.empty()) {

                        int nfaces = getNumBaseFaces(refiner),
                            channel = createBaseFVarChannel(refiner, (int)shape.uvs.size()/2 );

                        for (int i=0, ofs=0; i < nfaces; ++i) {

                            Far::IndexArray dstFaceUVs = getBaseFaceFVarValues(refiner, i, channel);

                            if (shape.isLeftHanded) {
                                dstFaceUVs[0] = shape.faceuvs[ofs++];
                                for (int j=dstFaceUVs.size()-1; j > 0; --j) {
                                    dstFaceUVs[j] = shape.faceuvs[ofs++];
                                }
                            } else {
                                for (int j=0; j<dstFaceUVs.size(); ++j) {
                                    dstFaceUVs[j] = shape.faceuvs[ofs++];
                                }
                            }
                        }
                    }
                    return true;
                }

            //----------------------------------------------------------
            template <>
                inline bool
                TopologyRefinerFactory<subdiv::Shape>::assignComponentTags(
                        Far::TopologyRefiner & refiner, subdiv::Shape const & shape) {

                    for (int i=0; i<(int)shape.tags.size(); ++i) {

                        subdiv::Shape::tag * t = shape.tags[i];

                        if (t->name=="crease") {

                            for (int j=0; j<(int)t->intargs.size()-1; j += 2) {

                                OpenSubdiv::Far::Index edge = findBaseEdge(refiner, t->intargs[j], t->intargs[j+1]);
                                if (edge==OpenSubdiv::Far::INDEX_INVALID) {
                                    printf("cannot find edge for crease tag (%d,%d)\n", t->intargs[j], t->intargs[j+1] );
                                    return false;
                                } else {
                                    int nfloat = (int) t->floatargs.size();
                                    setBaseEdgeSharpness(refiner, edge,
                                            std::max(0.0f, ((nfloat > 1) ? t->floatargs[j] : t->floatargs[0])));
                                }
                            }
                        } else if (t->name=="corner") {

                            for (int j=0; j<(int)t->intargs.size(); ++j) {
                                int vertex = t->intargs[j];
                                if (vertex<0 or vertex>=getNumBaseVertices(refiner)) {
                                    printf("cannot find vertex for corner tag (%d)\n", vertex );
                                    return false;
                                } else {
                                    int nfloat = (int) t->floatargs.size();
                                    setBaseVertexSharpness(refiner, vertex,
                                            std::max(0.0f, ((nfloat > 1) ? t->floatargs[j] : t->floatargs[0])));
                                }
                            }
                        }
                    }
                    { // Hole tags
                        for (int i=0; i<(int)shape.tags.size(); ++i) {
                            subdiv::Shape::tag * t = shape.tags[i];
                            if (t->name=="hole") {
                                for (int j=0; j<(int)t->intargs.size(); ++j) {
                                    setBaseFaceHole(refiner, t->intargs[j], true);
                                }
                            }
                        }
                    }
                    return true;
                }

            template <>
                inline void
                TopologyRefinerFactory<subdiv::Shape>::reportInvalidTopology(
                        TopologyRefinerFactory::TopologyError /* errCode */, char const * msg, subdiv::Shape const & /* shape */) {
                    Warning(msg);
                }

        } // namespace Far

    } // namespace OPENSUBDIV_VERSION
} // namespace OpenSubdiv


#endif
