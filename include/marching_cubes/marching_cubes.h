///////////////////////////////////////////////////////////////////////////////
// Copyright 2019 Lakshman Anumolu.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////
//
// This algorithm is based on the source code available at http://paulbourke.net/geometry/polygonise/.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "marching_cubes/utilities.h"
#include "marching_cubes/vec3.h"

#include <limits.h>
#include <tuple>
#include <vector>

namespace MARCHING_CUBES
{
enum Mesh : unsigned int { TRIANGLES, VERTICES };

/*!
 * \class Vertex.
 */
template <typename T>
class Vertex
{
 public:
  Vertex() : id(ULONG_MAX), obj_id(ULONG_MAX), pos(Vec3<T>{}), normal(Vec3<T>{}), num_shared_triangles(0) {}
  ~Vertex() {}

  std::size_t id;                 //!< Unique id of a vertex.
  std::size_t obj_id;             //!< Reindexed id to write to obj. Index starts with 1.
  Vec3<T> pos;                    //!< Position of a vertex.
  Vec3<T> normal;                 //!< Normal at vertex.
  unsigned num_shared_triangles;  //!< Number of triangles that share this vertex.
};

/*!
 * \class Triangle.
 */
template <typename T>
class Triangle
{
 public:
  Triangle()
      : id(ULONG_MAX),
        vertex_ids(ULONG_MAX, ULONG_MAX, ULONG_MAX),
        normal(static_cast<T>(0.), static_cast<T>(0.), static_cast<T>(0.))
  {
  }

  ~Triangle() {}

  std::size_t id;
  Vec3<size_t> vertex_ids;
  Vec3<T> normal;
};

template <typename T>
using TriangleVertexTuple_t = std::tuple<std::vector<Triangle<T>>, std::vector<Vertex<T>>>;

/*!
 * \class MarchingCubes
 *
 * Convention-1:
 * -------------
 *
 * V: Vertices.
 * E: Edges.
 *
 *                               ^ Y
 *                               |
 *                               |
 *                               |
 *
 *                               3          2          2
 *                               V----------E----------V
 *                              /|                    /|
 *                             / |                   / |
 *                            /  |                  /  |
 *                        11 E   |              10 E   |
 *                          /    E 3              /    E 1
 *                         /     |               /     |
 *                        /      |   6          /      |
 *                     7 V-------|--E----------V 6     |
 *                       |       |             |       |
 *                       |     0 V----------E--|-------V   -----> X
 *                       |      /           0  |      /1
 *                       |     /               |     /
 *                     7 E    /              5 E    /
 *                       |   E 8               |   E 9
 *                       |  /                  |  /
 *                       | /                   | /
 *                       |/                    |/
 *                     4 V----------V----------V
 *                                  4          5
 *
 *                    /
 *                   /
 *                  /
 *                 v
 *
 *               Z
 *
 *
 * Edge ids from vertex ids:
 *   * Let Vi = id of `i`th vertex.
 *   *
 *     Edge number    |            Id
 *    -----------------------------------------
 *          0              V0
 *          1              V1 + 1 * offset + 1
 *          2              V3
 *          3              V0 + 1 * offset + 1
 *          4              V4
 *          5              V5 + 1 * offset + 1
 *          6              V7
 *          7              V4 + 1 * offset + 1
 *          8              V0 + 2 * offset + 2
 *          9              V1 + 2 * offset + 2
 *          10             V2 + 2 * offset + 2
 *          11             V3 + 2 * offset + 2
 *
 *
 * Convention-2: (Ref.: http://paulbourke.net/geometry/polygonise/)
 * ----------------------------------------------------------------
 *
 *                               ^ Y
 *                               |
 *                               |
 *                               |
 *
 *                               4          4          5
 *                               V----------E----------V
 *                              /|                    /|
 *                             / |                   / |
 *                            /  |                  /  |
 *                         7 E   |               5 E   |
 *                          /    E 8              /    E 9
 *                         /     |               /     |
 *                        /      |   6          /      |
 *                     7 V-------|--E----------V 6     |
 *                       |       |             |       |
 *                       |     0 V----------E--|-------V   -----> X
 *                       |      /           0  |      /1
 *                       |     /               |     /
 *                    11 E    /             10 E    /
 *                       |   E 3               |   E 1
 *                       |  /                  |  /
 *                       | /                   | /
 *                       |/                    |/
 *                     3 V----------V----------V
 *                                  2          2
 *
 *                    /
 *                   /
 *                  /
 *                 v
 *
 *               Z
 *
 *
 * Edge ids from vertex ids:
 *   * Let Vi = id of `i`th vertex.
 *   *
 *     Edge number    |            Id
 *    -----------------------------------------
 *          0              V0
 *          1              V1 + 2 * offset + 2
 *          2              V3
 *          3              V0 + 2 * offset + 2
 *          4              V4
 *          5              V5 + 2 * offset + 2
 *          6              V7
 *          7              V4 + 2 * offset + 2
 *          8              V0 + 1 * offset + 1
 *          9              V1 + 1 * offset + 1
 *          10             V2 + 1 * offset + 1
 *          11             V3 + 1 * offset + 1
 */
template <typename T = float>
class MarchingCubes
{
 public:
  /*! Default constructor.
   */
  MarchingCubes();

  /*! Default destructor.
   */
  ~MarchingCubes();

  /*! Compute edge ids using vertex ids in a cube.
   *
   * Currently I consider total number of cells as `offset`.
   *
   * \param offset added to avoid collision between two edges in the same cell.
   * \param vertex_ids ids of 8 vertices of a cube.
   *
   * \return ids of 12 edges of a cube.
   */
  std::vector<size_t> vertexToEdgeIds(const std::size_t offset, const std::vector<size_t>& vertex_ids);

  /*! Returns normalized distance of the iso-surface intersection from vertex-1.
   *
   * Usage:
   * ------
   *
   *         <--(f)--> <--(1 - f)-->
   *  (v1) X ---------O------------- X (v2)
   *
   *  alpha_{O} = (1 - f) * alpha_{v1} + f * alpha_{v2}.
   *
   * \param alpha1 value of a scalar at vertex-1.
   * \param alpha2 value of a scalar at vertex-2.
   * \param iso_alpha value for which iso-surface needs to be extracted.
   *
   * \return normalized distance of iso_alpha from vertex-1.
   */
  T edgeIntersectionWeight(const T alpha1, const T alpha2, const T iso_alpha) const;

  /*! Marching cubes algorithm on a single cube.
   *
   * Normals at surface vertices are computed via interpolation of provided normals.
   * Triangle normals are computed as an average of vertex normals.
   *
   * \param cube_vertices position vectors of 8 vertices of a cube.
   * \param edge_ids ids of 12 edges of a cube.
   * \param triangle_start_id id assigned to first triangle created in this cube.
   * \param scalars vector of size 8 with scalar values at all vertices of a cube.
   * \param normals vector of size 8 with normal vectors at all vertices of a cube.
   * \param iso_alpha value for which iso-surface needs to be extracted.
   *
   * \return tuple of triangles and triangle corners(/vertices).
   */
  TriangleVertexTuple_t<T> marchCube(const std::vector<Vec3<T>>& cube_vertices, const std::vector<size_t>& edge_ids,
                                     const size_t triangle_start_id, const std::vector<T>& scalars,
                                     const std::vector<Vec3<T>>& normals, const T iso_alpha);
};
}  // namespace MARCHING_CUBES
