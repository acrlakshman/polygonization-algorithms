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

#include "array.h"
#include "mat3.h"

template <typename T_GRID, typename T_ARRAY>
EXAMPLES::Array<T_GRID, T_ARRAY>::Array(const EXAMPLES::Grid<typename T_GRID::value_type, T_GRID::dim>& grid)
    : m_grid(grid),
      m_nx(grid.numCells()[0]),
      m_ny(grid.numCells()[1]),
      m_nz(grid.numCells()[2]),
      m_pad(grid.getPadding())
{
  m_data.resize(m_grid.size());

  for (int i = 0; i < m_data.size(); ++i) {
    m_data[i] = T_ARRAY();
  }
}

template <typename T_GRID, typename T_ARRAY>
EXAMPLES::Array<T_GRID, T_ARRAY>::~Array()
{
  m_data.clear();
  m_data.shrink_to_fit();
}

template <typename T_GRID, typename T_ARRAY>
const std::size_t EXAMPLES::Array<T_GRID, T_ARRAY>::size() const
{
  return m_data.size();
}

template <typename T_GRID, typename T_ARRAY>
const T_GRID& EXAMPLES::Array<T_GRID, T_ARRAY>::grid() const
{
  return m_grid;
}

template <typename T_GRID, typename T_ARRAY>
const SURFACE_POLYGONIZATION::Vec3<int> EXAMPLES::Array<T_GRID, T_ARRAY>::numCells() const
{
  return SURFACE_POLYGONIZATION::Vec3<int>(m_nx, m_ny, m_nz);
}

template <typename T_GRID, typename T_ARRAY>
const T_ARRAY& EXAMPLES::Array<T_GRID, T_ARRAY>::operator[](const std::size_t idx) const
{
  return m_data[idx];
}

template <typename T_GRID, typename T_ARRAY>
T_ARRAY& EXAMPLES::Array<T_GRID, T_ARRAY>::operator[](const std::size_t idx)
{
  return m_data[idx];
}

template <typename T_GRID, typename T_ARRAY>
const T_ARRAY& EXAMPLES::Array<T_GRID, T_ARRAY>::operator()(const int i, const int j, const int k) const
{
  const std::size_t idx = m_grid.index(i, j, k);
  return m_data[idx];
}

template <typename T_GRID, typename T_ARRAY>
T_ARRAY& EXAMPLES::Array<T_GRID, T_ARRAY>::operator()(const int i, const int j, const int k)
{
  const std::size_t idx = m_grid.index(i, j, k);
  return m_data[idx];
}

template <typename T_GRID, typename T_ARRAY>
const T_ARRAY& EXAMPLES::Array<T_GRID, T_ARRAY>::operator()(const SURFACE_POLYGONIZATION::Vec3<int> node_id) const
{
  return m_data[m_grid.index(node_id)];
}

template <typename T_GRID, typename T_ARRAY>
T_ARRAY& EXAMPLES::Array<T_GRID, T_ARRAY>::operator()(const SURFACE_POLYGONIZATION::Vec3<int> node_id)
{
  return m_data[m_grid.index(node_id)];
}

template <typename T_GRID, typename T_ARRAY>
void EXAMPLES::Array<T_GRID, T_ARRAY>::operator=(const EXAMPLES::Array<T_GRID, T_ARRAY>& array)
{
  m_data = array.data();
}

template class EXAMPLES::Array<EXAMPLES::Grid<float, 3>, float>;
template class EXAMPLES::Array<EXAMPLES::Grid<double, 3>, double>;
template class EXAMPLES::Array<EXAMPLES::Grid<float, 3>, SURFACE_POLYGONIZATION::Vec3<float>>;
template class EXAMPLES::Array<EXAMPLES::Grid<double, 3>, SURFACE_POLYGONIZATION::Vec3<double>>;
