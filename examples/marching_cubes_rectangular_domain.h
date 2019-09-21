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

#pragma once

#include "array.h"
#include "grid.h"
#include "mat3.h"
#include "scalar_polygonization/marching_cubes.h"
#include "scalar_polygonization/vec3.h"

#include <map>

namespace EXAMPLES
{
enum class ScalarObject : int { CIRCLE, DAM };

class MarchingCubesRectangularDomain
{
  using T = float;

 public:
  MarchingCubesRectangularDomain(int nx, int ny, int nz);

  ~MarchingCubesRectangularDomain();

  void createGrid(T x_min, T x_max, T y_min, T y_max, T z_min, T z_max);

  void createScalarField(ScalarObject object);

  void computeNormals();

  void computeVertexNormalsFromTriangles();

  void polygonize(const T iso_alpha);

  void writeToObj(const std::string file_name);

  Grid<T, 3> m_grid;                                                          //!< 3D grid.
  Array<Grid<T, 3>, T> *m_scalar_field;                                       //!< scalar field at all grid locations.
  Array<Grid<T, 3>, SCALAR_POLYGONIZATION::Vec3<T>> *m_normal_vector_field;  //!< normal vectors at all grid locations.
  std::map<size_t, SCALAR_POLYGONIZATION::Vertex<T>> surface_vertices;       //!< vertices forming polygonized field.
  std::vector<SCALAR_POLYGONIZATION::Triangle<T>> surface_triangles;         //!< surface triangles.
};
}  // namespace EXAMPLES
