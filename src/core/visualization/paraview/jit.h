// -----------------------------------------------------------------------------
//
// Copyright (C) The BioDynaMo Project.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// See the LICENSE file distributed with this work for details.
// See the NOTICE file distributed with this work for additional information
// regarding copyright ownership.
//
// -----------------------------------------------------------------------------

#ifndef CORE_VISUALIZATION_PARAVIEW_JIT_H_
#define CORE_VISUALIZATION_PARAVIEW_JIT_H_

#include "core/container/math_array.h"
#include "core/functor.h"
#include "core/visualization/paraview/helper.h"
#include "core/simulation.h"
#include "core/param/param.h"
#include "core/sim_object/sim_object.h"
#include "core/resource_manager.h"
#include "core/sim_object/cell.h" // FIXME remove

#ifndef __ROOTCLING__

#include <vtkIntArray.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>

namespace bdm {

template <typename T>
struct GetVtkArrayType {};

template <>
struct GetVtkArrayType<double> {
  using type = vtkDoubleArray;
};

template <>
struct GetVtkArrayType<int> {
  using type = vtkIntArray;
};

template <>
struct GetVtkArrayType<uint64_t> {
  using type = vtkIntArray;
};

template <typename T, std::size_t N>
struct GetVtkArrayType<MathArray<T, N>> {
  using type = typename GetVtkArrayType<T>::type;
};

template <typename T>
struct GetNumberOfComponents {
  static const int value = 1;
};

template <typename T, std::size_t N>
struct GetNumberOfComponents<MathArray<T, N>> {
  static const int value = N;
};

template <typename T, std::size_t N>
struct GetNumberOfComponents<std::array<T, N>> {
  static const int value = N;
};

template<typename T>
struct IsArray : std::false_type {};

template<typename T, size_t N>
struct IsArray<std::array<T, N>> : std::true_type {};

template<typename T, std::size_t N>
struct IsArray<MathArray<T, N>> : std::true_type {};

template <typename T>
inline int CreateVtkDataArray(const std::string& dm_name, VtkSoGrid* so_grid) {
  using VtkArrayType = typename GetVtkArrayType<T>::type;
  unsigned components = GetNumberOfComponents<T>::value;
  vtkNew<VtkArrayType> new_vtk_array;
  new_vtk_array->SetName(dm_name.c_str());
  auto* vtk_array = new_vtk_array.GetPointer();
  vtk_array->SetNumberOfComponents(components);
  auto* point_data = so_grid->data_->GetPointData();
  return point_data->AddArray(vtk_array);
}

template <>
inline int CreateVtkDataArray<Double3>(const std::string& dm_name, VtkSoGrid* so_grid) {
  vtkNew<vtkDoubleArray> new_vtk_array;
  new_vtk_array->SetName(dm_name.c_str());
  auto* vtk_array = new_vtk_array.GetPointer();
  vtk_array->SetNumberOfComponents(3);
  if (dm_name == "position_") {
    vtkNew<vtkPoints> points;
    points->SetData(vtk_array);
    so_grid->data_->SetPoints(points.GetPointer());
    return -1;
  } else if (dm_name == "mass_location_") {
    // create points with position {0, 0, 0}
    // BDMGlyph will rotate and translate based on the attribute data
    vtkNew<vtkPoints> points;
    points->SetData(vtk_array);
    so_grid->data_->SetPoints(points.GetPointer());
    return so_grid->data_->GetPointData()->AddArray(vtk_array);
  } else {
    return so_grid->data_->GetPointData()->AddArray(vtk_array);
  }
  return -1;
}

inline void InitializeVtkSoGrid(VtkSoGrid* so_grid) {
  Cell so;
  so_grid->shape_ = so.GetShape();
  so_grid->vis_data_members_ = so.GetRequiredVisDataMembers();
  auto* param = Simulation::GetActive()->GetParam();
  for (auto& dm : param->visualize_sim_objects_.at(so_grid->name_)) {
    so_grid->vis_data_members_.insert(dm);
  }

  so_grid->data_members_.push_back({"position_", "Double3", "Cell", 104});
  so_grid->data_members_.push_back({"diameter_", "double", "Cell", 152});

  so_grid->data_members_[0].array_idx = CreateVtkDataArray<Double3>("position_", so_grid);
  so_grid->data_members_[1].array_idx = CreateVtkDataArray<double>("diameter_", so_grid);
}

struct PopulateDataArraysFunctor : public Functor<void, SimObject*, SoHandle> {
  vtkUnstructuredGrid* grid_;
  vtkPointData* point_data_;

  PopulateDataArraysFunctor(VtkSoGrid* so_grid) : grid_(so_grid->data_), point_data_(so_grid->data_->GetPointData()) {}

  template <typename TClass, typename TDataMember>
  typename std::enable_if<IsArray<TDataMember>::value>::type
  SetTuple(SimObject* so, uint64_t so_idx, int array_idx, uint64_t dm_offset) {
    auto* casted_so = static_cast<TClass*>(so);
    if (array_idx == -1) {
      auto* data = reinterpret_cast<TDataMember*>(reinterpret_cast<char*>(casted_so)+dm_offset)->data();
      grid_->GetPoints()->GetData()->SetTuple(so_idx, data);
    } else {
      auto* data = reinterpret_cast<TDataMember*>(reinterpret_cast<char*>(casted_so)+dm_offset)->data();
      point_data_->GetArray(array_idx)->SetTuple(so_idx, data);
    }
  }

  template <typename TClass, typename TDataMember>
  typename std::enable_if<!IsArray<TDataMember>::value>::type
  SetTuple(SimObject* so, uint64_t so_idx, int array_idx, uint64_t dm_offset) {
    auto* casted_so = static_cast<TClass*>(so);
    if (array_idx != -1) {
      auto* data = reinterpret_cast<TDataMember*>(reinterpret_cast<char*>(casted_so)+dm_offset);
      point_data_->GetArray(array_idx)->SetTuple(so_idx, data);
    }
  }

  void operator()(SimObject* so, SoHandle soh) {
    auto idx = soh.GetElementIdx();
    SetTuple<Cell, Double3>(so, idx, -1, 104);
    SetTuple<Cell, double>(so, idx, 0, 152);
  }
};


}  // namespace bdm

#endif  // __ROOTCLING__
#endif  // CORE_VISUALIZATION_PARAVIEW_JIT_H_
