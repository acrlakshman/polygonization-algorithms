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

#include "marching_cubes_rectangular_domain.h"
#include "surface_polygonization/tables.h"

#include <assert.h>
#include <fstream>

using namespace EXAMPLES;

MarchingCubesRectangularDomain::MarchingCubesRectangularDomain(int nx, int ny, int nz)
    : m_grid(nx, ny, nz), m_scalar_field(nullptr), m_normal_vector_field(nullptr)
{
}

MarchingCubesRectangularDomain::~MarchingCubesRectangularDomain()
{
  if (m_scalar_field) delete m_scalar_field;
  if (m_normal_vector_field) delete m_normal_vector_field;
}

void MarchingCubesRectangularDomain::createGrid(T x_min, T x_max, T y_min, T y_max, T z_min, T z_max)
{
  m_grid.generate(x_min, x_max, y_min, y_max, z_min, z_max);

  m_scalar_field = new Array<Grid<T, 3>, T>(m_grid);
  m_normal_vector_field = new Array<Grid<T, 3>, SURFACE_POLYGONIZATION::Vec3<T>>(m_grid);
}

void MarchingCubesRectangularDomain::createScalarField(ScalarObject object)
{
  auto &scalar_field = *m_scalar_field;
  auto &normals = *m_normal_vector_field;

  // accessing grid details
  const auto mask = m_grid.getMask();
  const int pad = m_grid.getPadding();
  const auto num_cells = m_grid.numCells();

  // defining working+ghost domain extent
  int i_min = -pad * mask[0];
  int j_min = -pad * mask[1];
  int k_min = -pad * mask[2];
  int i_max = num_cells[0] + pad * mask[0];
  int j_max = num_cells[1] + pad * mask[1];
  int k_max = num_cells[2] + pad * mask[2];

  switch (object) {
    case ScalarObject::CIRCLE: {
      T radius = 0.25;
      SURFACE_POLYGONIZATION::Vec3<T> center(0.5, 0.5, 0.5);

      for (int i = i_min + 1; i < i_max - 1; ++i)
        for (int j = j_min + 1; j < j_max - 1; ++j)
          for (int k = k_min + 1; k < k_max - 1; ++k) {
            T dist = 0.;
            for (int cmpt = 0; cmpt < 3; ++cmpt)
              dist += (m_grid(i, j, k)[cmpt] - center[cmpt]) * (m_grid(i, j, k)[cmpt] - center[cmpt]);

            scalar_field(i, j, k) = dist - radius * radius;
            // scalar_field(i, j, k) = static_cast<T>(0.);
            // if (dist <= radius * radius) scalar_field(i, j, k) = static_cast<T>(1.);

            // Initialize normals to zero vector.
            normals(i, j, k) = SURFACE_POLYGONIZATION::Vec3<T>(0., 0., 0.);
          }
      break;
    }

    case ScalarObject::DAM: {
      T x_w = 0.2, y_w = 0.25, z_w = 0.2;

      for (int i = i_min + 1; i < i_max - 1; ++i)
        for (int j = j_min + 1; j < j_max - 1; ++j)
          for (int k = k_min + 1; k < k_max - 1; ++k) {
            scalar_field(i, j, k) = static_cast<T>(0.);
            if (m_grid(i, j, k)[0] <= x_w && m_grid(i, j, k)[1] <= y_w && m_grid(i, j, k)[2] <= z_w)
              scalar_field(i, j, k) = static_cast<T>(1.);

            // Initialize normals to zero vector.
            normals(i, j, k) = SURFACE_POLYGONIZATION::Vec3<T>(0., 0., 0.);
          }

      break;
    }

    default:
      break;
  }
}

void MarchingCubesRectangularDomain::computeNormals()
{
  auto &scalar_field = *m_scalar_field;
  auto &normals = *m_normal_vector_field;

  using T_GRID = decltype(m_grid);
  const auto dim = T_GRID::dim;

  const auto num_cells = m_grid.numCells();
  const auto dx = m_grid.dX();
  const auto &axis_vectors = T_GRID::axis_vectors;

  for (int i = 0; i < num_cells[0]; ++i)
    for (int j = 0; j < num_cells[1]; ++j)
      for (int k = 0; k < num_cells[2]; ++k) {
        for (int cmpt = 0; cmpt < dim; ++cmpt) {
          typename T_GRID::value_type one_by_dx = static_cast<typename T_GRID::value_type>(1.) / dx[cmpt];

          normals(i, j, k)[cmpt] =
              (scalar_field(i + axis_vectors(cmpt, 0), j + axis_vectors(cmpt, 1), k + axis_vectors(cmpt, 2)) -
               scalar_field(i - axis_vectors(cmpt, 0), j - axis_vectors(cmpt, 1), k - axis_vectors(cmpt, 2))) *
              one_by_dx * static_cast<T>(0.5);

          // NOTE: This sign change is not needed if Convention-2 is followed and the scalar field enclosing
          // the surface is higher than the iso-surface value.
          normals(i, j, k)[cmpt] *= static_cast<T>(-1.);
        }
      }
}

void MarchingCubesRectangularDomain::computeVertexNormalsFromTriangles()
{
  // Set normals at all surface vertices to zero.
  for (auto &surface_triangle : surface_triangles) {
    for (auto i = 0; i < 3; ++i) {
      const auto &vertex_id = surface_triangle.vertex_ids[i];
      surface_vertices[vertex_id].normal = SURFACE_POLYGONIZATION::Vec3<T>(0., 0., 0.);
      surface_vertices[vertex_id].num_shared_triangles = static_cast<unsigned>(0);
    }
  }

  // Average triangle normals to vertex normals.
  for (auto &surface_triangle : surface_triangles) {
    for (auto i = 0; i < 3; ++i) {
      const auto &vertex_id = surface_triangle.vertex_ids[i];
      surface_vertices[vertex_id].normal = surface_vertices[vertex_id].normal + surface_triangle.normal;
      ++surface_vertices[vertex_id].num_shared_triangles;
    }
  }

  for (auto &surface_vertex : surface_vertices) {
    auto &vertex = surface_vertex.second;
    for (int i = 0; i < 3; ++i) vertex.normal[i] /= vertex.num_shared_triangles;

    vertex.normal.normalize();
  }
}

void MarchingCubesRectangularDomain::polygonize(const T iso_alpha)
{
  auto &scalar_field = *m_scalar_field;
  auto &normal_vector_field = *m_normal_vector_field;

  // accessing grid details
  const auto mask = m_grid.getMask();
  const int pad = m_grid.getPadding();
  const auto num_cells = m_grid.numCells();

  // defining working+ghost domain extent
  int i_min = -pad * mask[0];
  int j_min = -pad * mask[1];
  int k_min = -pad * mask[2];
  int i_max = num_cells[0] + pad * mask[0];
  int j_max = num_cells[1] + pad * mask[1];
  int k_max = num_cells[2] + pad * mask[2];

  SURFACE_POLYGONIZATION::MarchingCubes<T> mc;

  std::vector<SURFACE_POLYGONIZATION::Vec3<int>> vertex_indices(8);
  std::vector<size_t> vertex_ids(8);
  std::vector<SURFACE_POLYGONIZATION::Vec3<T>> cube_vertices(8);
  std::vector<size_t> edge_ids(12);
  std::vector<T> scalars(8);
  std::vector<SURFACE_POLYGONIZATION::Vec3<T>> normals(8);
  size_t triangle_start_id = 0;

  for (int i = i_min; i < i_max - 1; ++i)
    for (int j = j_min; j < j_max - 1; ++j)
      for (int k = k_min; k < k_max - 1; ++k) {
        // ------ Convention-2 (Ref.: http://paulbourke.net/geometry/polygonise/)
        // vertex_indices[0] = SURFACE_POLYGONIZATION::Vec3<int>(i, j, k);
        // vertex_indices[1] = SURFACE_POLYGONIZATION::Vec3<int>(i + 1, j, k);
        // vertex_indices[2] = SURFACE_POLYGONIZATION::Vec3<int>(i + 1, j, k + 1);
        // vertex_indices[3] = SURFACE_POLYGONIZATION::Vec3<int>(i, j, k + 1);
        // vertex_indices[4] = SURFACE_POLYGONIZATION::Vec3<int>(i, j + 1, k);
        // vertex_indices[5] = SURFACE_POLYGONIZATION::Vec3<int>(i + 1, j + 1, k);
        // vertex_indices[6] = SURFACE_POLYGONIZATION::Vec3<int>(i + 1, j + 1, k + 1);
        // vertex_indices[7] = SURFACE_POLYGONIZATION::Vec3<int>(i, j + 1, k + 1);
        //--------------------

        for (int v_idx = 0; v_idx < 8; ++v_idx) {
          // ------ Convention-1
          vertex_indices[v_idx] = SURFACE_POLYGONIZATION::Vec3<int>(i, j, k) +
                                  SURFACE_POLYGONIZATION::Vec3<int>(SURFACE_POLYGONIZATION::vertex_offset[v_idx][0],
                                                                    SURFACE_POLYGONIZATION::vertex_offset[v_idx][1],
                                                                    SURFACE_POLYGONIZATION::vertex_offset[v_idx][2]);
          //--------------------
          vertex_ids[v_idx] = m_grid.index(vertex_indices[v_idx]);
          cube_vertices[v_idx] = m_grid(vertex_indices[v_idx]);
          scalars[v_idx] = scalar_field(vertex_indices[v_idx]);
          normals[v_idx] = normal_vector_field(vertex_indices[v_idx]);
        }

        // Get edge_ids from vertex_ids.
        edge_ids = mc.vertexToEdgeIds(m_grid.size(), vertex_ids);

        // Run marching cubes algorithm.
        const auto triangle_vertex_tuple =
            mc.marchCube(cube_vertices, edge_ids, triangle_start_id, scalars, normals, iso_alpha);

        std::vector<SURFACE_POLYGONIZATION::Triangle<T>> triangles =
            std::get<SURFACE_POLYGONIZATION::TRIANGLES>(triangle_vertex_tuple);
        std::vector<SURFACE_POLYGONIZATION::Vertex<T>> triangle_vertices =
            std::get<SURFACE_POLYGONIZATION::VERTICES>(triangle_vertex_tuple);

        triangle_start_id += triangles.size();

        for (auto &triangle : triangles) surface_triangles.push_back(std::move(triangle));

        for (auto &triangle_vertex : triangle_vertices)
          if (surface_vertices.find(triangle_vertex.id) == surface_vertices.end())
            surface_vertices[triangle_vertex.id] = std::move(triangle_vertex);
      }

  assert(surface_triangles.size() == triangle_start_id);
  std::cout << "Scalar polygonization complete" << std::endl;
  std::cout << "\tNumber of surface vertices: " << surface_vertices.size() << std::endl;
  std::cout << "\tNumber of surface triangles: " << surface_triangles.size() << std::endl;

  // Update obj_id of each surface vertex.
  size_t obj_id = 1;
  for (auto &surface_vertex_pair : surface_vertices) {
    auto &surface_vertex = surface_vertex_pair.second;
    surface_vertex.obj_id = obj_id++;
  }

  // Compute normals at vertices as average of triangle normals.
  this->computeVertexNormalsFromTriangles();
}

void MarchingCubesRectangularDomain::writeToObj(const std::string file_name)
{
  std::ofstream obj_file(file_name);

  // Write vertex locations.
  for (const auto &surface_vertex_pair : surface_vertices) {
    const auto &surface_vertex = surface_vertex_pair.second;
    obj_file << "v " << surface_vertex.pos[0] << " " << surface_vertex.pos[1] << " " << surface_vertex.pos[2]
             << std::endl;
  }

  // Write vertex normals.
  for (const auto &surface_vertex_pair : surface_vertices) {
    const auto &surface_vertex = surface_vertex_pair.second;
    obj_file << "vn " << surface_vertex.normal[0] << " " << surface_vertex.normal[1] << " " << surface_vertex.normal[2]
             << std::endl;
  }

  // Write face data.
  for (const auto &surface_triangle : surface_triangles) {
    auto v_vn_0 = surface_vertices[surface_triangle.vertex_ids[0]].obj_id;
    auto v_vn_1 = surface_vertices[surface_triangle.vertex_ids[1]].obj_id;
    auto v_vn_2 = surface_vertices[surface_triangle.vertex_ids[2]].obj_id;

    obj_file << "f " << v_vn_0 << "//" << v_vn_0 << " " << v_vn_1 << "//" << v_vn_1 << " " << v_vn_2 << "//" << v_vn_2
             << " " << std::endl;
  }

  obj_file.close();
}
