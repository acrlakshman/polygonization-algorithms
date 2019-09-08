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
// 3. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission.
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

#include "surface_polygonization/marching_cubes.h"
#include "surface_polygonization/tables.h"

#include <iostream>

template <typename T>
SURFACE_POLYGONIZATION::MarchingCubes<T>::MarchingCubes()
{
}

template <typename T>
SURFACE_POLYGONIZATION::MarchingCubes<T>::~MarchingCubes()
{
}

template <typename T>
std::vector<size_t> SURFACE_POLYGONIZATION::MarchingCubes<T>::vertexToEdgeIds(const std::size_t offset,
                                                                              const std::vector<size_t>& vertex_ids)
{
  std::vector<size_t> edge_ids(12);

  for (int edge = 0; edge < 12; ++edge)
    edge_ids[edge] = vertex_ids[edge_id_to_vertex_id_base_map[edge]] +
                     static_cast<size_t>(edge_id_to_vertex_id_offset_map[edge]) +
                     offset * static_cast<size_t>(edge_id_to_vertex_id_offset_map[edge]);

  return edge_ids;
}

template <typename T>
T SURFACE_POLYGONIZATION::MarchingCubes<T>::edgeIntersectionWeight(const T alpha1, const T alpha2,
                                                                   const T iso_alpha) const
{
  if (fabs(iso_alpha - alpha1) < 1e-5) {
    return 0;
  }
  if (fabs(iso_alpha - alpha2) < 1e-5) {
    return 1;
  }
  if (fabs(alpha1 - alpha2) < 1e-5) {
    return 0;
  }
  return (iso_alpha - alpha1) / (alpha2 - alpha1);
}

template <typename T>
SURFACE_POLYGONIZATION::TriangleVertexTuple_t<T> SURFACE_POLYGONIZATION::MarchingCubes<T>::marchCube(
    const std::vector<Vec3<T>>& cube_vertices, const std::vector<size_t>& edge_ids, const size_t triangle_start_id,
    const std::vector<T>& scalars, const std::vector<Vec3<T>>& normals, const T iso_alpha)
{
  std::vector<Triangle<T>> triangles;
  std::vector<Vertex<T>> triangle_vertices;

  // Find which cube_vertices are inside of the surface and which are outside.
  int vertex_flag = 0;
  for (int i = 0; i < 8; ++i)
    if (scalars[i] < iso_alpha) vertex_flag |= (1 << i);

  // If the cube is entirely inside or outside of the surface, then there will be no intersections.
  if (edge_table[vertex_flag] == 0) {
    TriangleVertexTuple_t<T> triangle_vertex_tuple = std::make_tuple(triangles, triangle_vertices);
    return triangle_vertex_tuple;
  }

  // Find the point of intersection of the surface with each edge. Then find the normal to the surface at those points.
  Vec3<T> vertex_on_edge[12];
  Vec3<T> normal_at_vertex_on_edge[12];
  auto frac = static_cast<T>(0.);

  for (int edge = 0; edge < 12; ++edge) {
    if (edge_table[vertex_flag] & (1 << edge)) {
      frac =
          this->edgeIntersectionWeight(scalars[edge_connection[edge][0]], scalars[edge_connection[edge][1]], iso_alpha);
      vertex_on_edge[edge] = cube_vertices[edge_connection[edge][0]] * (static_cast<T>(1.) - frac) +
                             cube_vertices[edge_connection[edge][1]] * frac;
      normal_at_vertex_on_edge[edge] =
          normals[edge_connection[edge][0]] * (static_cast<T>(1.) - frac) + normals[edge_connection[edge][1]] * frac;
    }
  }

  // Create triangles.
  auto triangle_id = triangle_start_id;
  for (int i_tri = 0; triangle_table[vertex_flag][i_tri] != -1; i_tri += 3) {
    Triangle<T> triangle;
    for (int i_vert = 0; i_vert < 3; ++i_vert) {
      Vertex<T> vertex;
      auto vertex_idx = triangle_table[vertex_flag][i_tri + i_vert];
      vertex.pos = vertex_on_edge[vertex_idx];
      vertex.normal = normal_at_vertex_on_edge[vertex_idx];
      vertex.id = edge_ids[vertex_idx];  // id of the edge.

      triangle.vertex_ids[i_vert] = vertex.id;
      triangle.normal = triangle.normal + vertex.normal;

      triangle_vertices.push_back(std::move(vertex));
    }
    triangle.normal = triangle.normal * static_cast<T>(SURFACE_POLYGONIZATION::one_third);
    ++triangle_id;

    triangles.push_back(std::move(triangle));
  }

  TriangleVertexTuple_t<T> triangle_vertex_tuple = std::make_tuple(std::move(triangles), std::move(triangle_vertices));

  return triangle_vertex_tuple;
}

template class SURFACE_POLYGONIZATION::MarchingCubes<float>;
