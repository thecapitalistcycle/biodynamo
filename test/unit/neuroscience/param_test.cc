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

#include <gtest/gtest.h>

#include "core/param/param.h"
#include "core/simulation.h"
#include "neuroscience/module.h"
#include "neuroscience/param.h"

#include "unit/test_util/test_util.h"

namespace bdm {
namespace experimental {
namespace neuroscience {

TEST(neuroscience, Param) {
  constexpr const char* kConfigFileName = "bdm.toml";
  constexpr const char* kConfigContent =
      "[neuroscience]\n"
      "neurite_default_actual_length = 2.0\n"
      "neurite_default_density = 3.0\n"
      "neurite_default_diameter = 4.0\n"
      "neurite_default_spring_constant = 5.0\n"
      "neurite_default_adherence = 6.0\n"
      "neurite_default_tension = 7.0\n"
      "neurite_min_length = 8.0\n"
      "neurite_max_length = 9.0\n"
      "neurite_minimial_bifurcation_length = 10.0\n";

  std::ofstream config_file(kConfigFileName);
  config_file << kConfigContent;
  config_file.close();

  neuroscience::InitModule();
  Simulation simulation(TEST_NAME);
  auto* param = simulation.GetParam()->GetModuleParam<Param>();

  ASSERT_TRUE(param != nullptr);
  EXPECT_EQ(2.0, param->neurite_default_actual_length_);
  EXPECT_EQ(3.0, param->neurite_default_density_);
  EXPECT_EQ(4.0, param->neurite_default_diameter_);
  EXPECT_EQ(5.0, param->neurite_default_spring_constant_);
  EXPECT_EQ(6.0, param->neurite_default_adherence_);
  EXPECT_EQ(7.0, param->neurite_default_tension_);
  EXPECT_EQ(8.0, param->neurite_min_length_);
  EXPECT_EQ(9.0, param->neurite_max_length_);
  EXPECT_EQ(10.0, param->neurite_minimial_bifurcation_length_);

  remove(kConfigFileName);
}

}  // namespace neuroscience
}  // namespace experimental
}  // namespace bdm
