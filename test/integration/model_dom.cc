/*
 * Copyright 2018 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <string>
#include <gtest/gtest.h>
#include <ignition/math/Pose3.hh>
#include "sdf/Element.hh"
#include "sdf/Error.hh"
#include "sdf/Filesystem.hh"
#include "sdf/Joint.hh"
#include "sdf/Link.hh"
#include "sdf/Model.hh"
#include "sdf/Root.hh"
#include "sdf/Types.hh"
#include "sdf/World.hh"
#include "test_config.h"

using namespace ignition::math;

//////////////////////////////////////////////////
TEST(DOMModel, NotAModel)
{
  // Create an Element that is not a model
  sdf::ElementPtr element(new sdf::Element);
  element->SetName("world");
  sdf::Model model;
  sdf::Errors errors = model.Load(element);
  ASSERT_FALSE(errors.empty());
  EXPECT_EQ(sdf::ErrorCode::ELEMENT_INCORRECT_TYPE, errors[0].Code());
  EXPECT_TRUE(errors[0].Message().find("Attempting to load a Model") !=
               std::string::npos);
}

//////////////////////////////////////////////////
TEST(DOMModel, NoName)
{
  // Create a "model" with no name
  sdf::ElementPtr element(new sdf::Element);
  element->SetName("model");

  sdf::Model model;
  sdf::Errors errors = model.Load(element);
  ASSERT_FALSE(errors.empty());
  EXPECT_EQ(sdf::ErrorCode::ATTRIBUTE_MISSING, errors[0].Code());
  EXPECT_TRUE(errors[0].Message().find("model name is required") !=
               std::string::npos);
}

/////////////////////////////////////////////////
TEST(DOMRoot, LoadLinkCheck)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "empty.sdf");

  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  // Get the first world
  const sdf::World *world = root.WorldByIndex(0);
  ASSERT_NE(nullptr, world);
  EXPECT_EQ("default", world->Name());

  // Get the first model
  const sdf::Model *model = world->ModelByIndex(0);
  ASSERT_NE(nullptr, model);
  EXPECT_EQ("ground_plane", model->Name());
  EXPECT_EQ(1u, model->LinkCount());
  ASSERT_FALSE(nullptr == model->LinkByIndex(0));
  ASSERT_FALSE(nullptr == model->LinkByName("link"));
  EXPECT_EQ(model->LinkByName("link")->Name(), model->LinkByIndex(0)->Name());
  EXPECT_TRUE(nullptr == model->LinkByIndex(1));
  EXPECT_TRUE(model->LinkNameExists("link"));
  EXPECT_FALSE(model->LinkNameExists("links"));
}

/////////////////////////////////////////////////
TEST(DOMRoot, LoadDoublePendulum)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "double_pendulum.sdf");

  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  // Get the first model
  const sdf::Model *model = root.ModelByIndex(0);
  ASSERT_NE(nullptr, model);
  EXPECT_EQ("double_pendulum_with_base", model->Name());
  EXPECT_EQ(3u, model->LinkCount());
  EXPECT_FALSE(nullptr == model->LinkByIndex(0));
  EXPECT_FALSE(nullptr == model->LinkByIndex(1));
  EXPECT_FALSE(nullptr == model->LinkByIndex(2));
  EXPECT_TRUE(nullptr == model->LinkByIndex(3));
  EXPECT_EQ(ignition::math::Pose3d(1, 0, 0, 0, 0, 0), model->Pose());
  EXPECT_EQ("", model->PoseFrame());

  EXPECT_TRUE(model->LinkNameExists("base"));
  EXPECT_TRUE(model->LinkNameExists("upper_link"));
  EXPECT_TRUE(model->LinkNameExists("lower_link"));

  EXPECT_EQ(2u, model->JointCount());
  EXPECT_FALSE(nullptr == model->JointByIndex(0));
  EXPECT_FALSE(nullptr == model->JointByIndex(1));
  EXPECT_TRUE(nullptr == model->JointByIndex(2));

  EXPECT_TRUE(model->JointNameExists("upper_joint"));
  EXPECT_TRUE(model->JointNameExists("lower_joint"));
}

//////////////////////////////////////////////////
TEST(DOMModel, FourBar)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "four_bar.sdf");
  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  const sdf::Model *model = root.ModelByIndex(0);
  ASSERT_TRUE(model != nullptr);

  const sdf::Link *linkOne = model->LinkByName("link1");
  ASSERT_TRUE(linkOne != nullptr);
  const sdf::Link *linkTwo = model->LinkByName("link2");
  ASSERT_TRUE(linkTwo != nullptr);
  const sdf::Link *linkThree = model->LinkByName("link3");
  ASSERT_TRUE(linkThree != nullptr);
  const sdf::Link *linkFour = model->LinkByName("link4");
  ASSERT_TRUE(linkFour != nullptr);

  const sdf::Joint *jointOne = model->JointByName("joint1");
  ASSERT_TRUE(jointOne != nullptr);
  const sdf::Joint *jointTwo = model->JointByName("joint2");
  ASSERT_TRUE(jointTwo != nullptr);
  const sdf::Joint *jointThree = model->JointByName("joint3");
  ASSERT_TRUE(jointThree != nullptr);
  const sdf::Joint *jointFour = model->JointByName("joint4");
  ASSERT_TRUE(jointFour != nullptr);

  // Link 1
  EXPECT_EQ(Pose3d(0, 0.2, 0.05, 0, 0, 0), linkOne->Pose());
  EXPECT_EQ(Pose3d(0, 0, 0, 0, 0, 0), linkOne->Pose("link1"));
  EXPECT_EQ(Pose3d(-0.2, 0.2, 0, 0, 0, 0), linkOne->Pose("link2"));
  EXPECT_EQ(Pose3d(0, 0.4, 0, 0, 0, 0), linkOne->Pose("link3"));
  EXPECT_EQ(Pose3d(0.2, 0.2, 0, 0, 0, 0), linkOne->Pose("link4"));

  EXPECT_EQ(Pose3d(-0.2, 0, 0, 0, 0, 0), linkOne->Pose("joint1"));
  EXPECT_EQ(Pose3d(-0.2, 0.4, 0, 0, 0, 0), linkOne->Pose("joint2"));
  EXPECT_EQ(Pose3d(0.2, 0.4, 0, 0, 0, 0), linkOne->Pose("joint3"));
  EXPECT_EQ(Pose3d(0.2, 0.2, 0.05, 0, 0, 0), linkOne->Pose("joint4"));

  // Link 2
  EXPECT_EQ(Pose3d(0.2, 0, 0.05, 0, 0, 0), linkTwo->Pose());
  EXPECT_EQ(Pose3d(0.2, -0.2, 0, 0, 0, 0), linkTwo->Pose("link1"));
  EXPECT_EQ(Pose3d(0, 0, 0, 0, 0, 0), linkTwo->Pose("link2"));
  EXPECT_EQ(Pose3d(0.2, 0.2, 0, 0, 0, 0), linkTwo->Pose("link3"));
  EXPECT_EQ(Pose3d(0.4, 0, 0, 0, 0, 0), linkTwo->Pose("link4"));

  EXPECT_EQ(Pose3d(0.2, -0.2, 0.05, 0, 0, 0), linkTwo->Pose("joint1"));
  EXPECT_EQ(Pose3d(0, 0.2, 0, 0, 0, 0), linkTwo->Pose("joint2"));
  EXPECT_EQ(Pose3d(0.4, 0.2, 0, 0, 0, 0), linkTwo->Pose("joint3"));
  EXPECT_EQ(Pose3d(0.4, -0.2, 0, 0, 0, 0), linkTwo->Pose("joint4"));

  // Link 3
  EXPECT_EQ(Pose3d(0, -0.2, 0.05, 0, 0, 0), linkThree->Pose());
  EXPECT_EQ(Pose3d(0, -0.4, 0, 0, 0, 0), linkThree->Pose("link1"));
  EXPECT_EQ(Pose3d(-0.2, -0.2, 0, 0, 0, 0), linkThree->Pose("link2"));
  EXPECT_EQ(Pose3d(0, 0, 0, 0, 0, 0), linkThree->Pose("link3"));
  EXPECT_EQ(Pose3d(0.2, -0.2, 0, 0, 0, 0), linkThree->Pose("link4"));

  EXPECT_EQ(Pose3d(-0.2, -0.4, 0, 0, 0, 0), linkThree->Pose("joint1"));
  EXPECT_EQ(Pose3d(-0.2, -0.2, 0.05, 0, 0, 0), linkThree->Pose("joint2"));
  EXPECT_EQ(Pose3d(0.2, 0, 0, 0, 0, 0), linkThree->Pose("joint3"));
  EXPECT_EQ(Pose3d(0.2, -0.4, 0, 0, 0, 0), linkThree->Pose("joint4"));

  // Link 4
  EXPECT_EQ(Pose3d(-0.2, 0, 0.05, 0, 0, 0), linkFour->Pose());
  EXPECT_EQ(Pose3d(-0.2, -0.2, 0, 0, 0, 0), linkFour->Pose("link1"));
  EXPECT_EQ(Pose3d(-0.4, 0, 0, 0, 0, 0), linkFour->Pose("link2"));
  EXPECT_EQ(Pose3d(-0.2, 0.2, 0, 0, 0, 0), linkFour->Pose("link3"));
  EXPECT_EQ(Pose3d(0, 0, 0, 0, 0, 0), linkFour->Pose("link4"));

  EXPECT_EQ(Pose3d(-0.4, -0.2, 0, 0, 0, 0), linkFour->Pose("joint1"));
  EXPECT_EQ(Pose3d(-0.4, 0.2, 0, 0, 0, 0), linkFour->Pose("joint2"));
  EXPECT_EQ(Pose3d(-0.2, 0.2, 0.05, 0, 0, 0), linkFour->Pose("joint3"));
  EXPECT_EQ(Pose3d(0, -0.2, 0, 0, 0, 0), linkFour->Pose("joint4"));

  // Joint 1
  EXPECT_EQ(Pose3d(0, 0.2, 0, 0, 0, 0), jointOne->Pose());
  EXPECT_EQ(Pose3d(0.2, 0, 0, 0, 0, 0), jointOne->Pose("link1"));
  EXPECT_EQ(Pose3d(0, 0.2, 0, 0, 0, 0), jointOne->Pose("link2"));
  EXPECT_EQ(Pose3d(0.2, 0.4, 0, 0, 0, 0), jointOne->Pose("link3"));
  EXPECT_EQ(Pose3d(0.4, 0.2, 0, 0, 0, 0), jointOne->Pose("link4"));

  EXPECT_EQ(Pose3d(0, 0, 0, 0, 0, 0), jointOne->Pose("joint1"));
  EXPECT_EQ(Pose3d(0, 0.4, 0, 0, 0, 0), jointOne->Pose("joint2"));
  EXPECT_EQ(Pose3d(0.4, 0.4, 0, 0, 0, 0), jointOne->Pose("joint3"));
  EXPECT_EQ(Pose3d(0.4, 0, 0, 0, 0, 0), jointOne->Pose("joint4"));

  // Joint 2
  EXPECT_EQ(Pose3d(0.2, 0, 0, 0, 0, 0), jointTwo->Pose());
  EXPECT_EQ(Pose3d(0.2, -0.4, 0, 0, 0, 0), jointTwo->Pose("link1"));
  EXPECT_EQ(Pose3d(0, -0.2, 0, 0, 0, 0), jointTwo->Pose("link2"));
  EXPECT_EQ(Pose3d(0.2, 0, 0, 0, 0, 0), jointTwo->Pose("link3"));
  EXPECT_EQ(Pose3d(0.4, -0.2, 0, 0, 0, 0), jointTwo->Pose("link4"));

  EXPECT_EQ(Pose3d(0, -0.4, 0, 0, 0, 0), jointTwo->Pose("joint1"));
  EXPECT_EQ(Pose3d(0, 0, 0, 0, 0, 0), jointTwo->Pose("joint2"));
  EXPECT_EQ(Pose3d(0.4, 0, 0, 0, 0, 0), jointTwo->Pose("joint3"));
  EXPECT_EQ(Pose3d(0.4, -0.4, 0, 0, 0, 0), jointTwo->Pose("joint4"));

  // Joint 3
  EXPECT_EQ(Pose3d(0, -0.2, 0, 0, 0, 0), jointThree->Pose());
  EXPECT_EQ(Pose3d(-0.2, -0.4, 0, 0, 0, 0), jointThree->Pose("link1"));
  EXPECT_EQ(Pose3d(-0.4, -0.2, 0, 0, 0, 0), jointThree->Pose("link2"));
  EXPECT_EQ(Pose3d(-0.2, 0, 0, 0, 0, 0), jointThree->Pose("link3"));
  EXPECT_EQ(Pose3d(0, -0.2, 0, 0, 0, 0), jointThree->Pose("link4"));

  EXPECT_EQ(Pose3d(-0.4, -0.4, 0, 0, 0, 0), jointThree->Pose("joint1"));
  EXPECT_EQ(Pose3d(-0.4, 0, 0, 0, 0, 0), jointThree->Pose("joint2"));
  EXPECT_EQ(Pose3d(0, 0, 0, 0, 0, 0), jointThree->Pose("joint3"));
  EXPECT_EQ(Pose3d(0, -0.4, 0, 0, 0, 0), jointThree->Pose("joint4"));

  // Joint 4
  EXPECT_EQ(Pose3d(-0.2, 0, 0, 0, 0, 0), jointFour->Pose());
  EXPECT_EQ(Pose3d(-0.2, 0, 0, 0, 0, 0), jointFour->Pose("link1"));
  EXPECT_EQ(Pose3d(-0.4, 0.2, 0, 0, 0, 0), jointFour->Pose("link2"));
  EXPECT_EQ(Pose3d(-0.2, 0.4, 0, 0, 0, 0), jointFour->Pose("link3"));
  EXPECT_EQ(Pose3d(0, 0.2, 0, 0, 0, 0), jointFour->Pose("link4"));

  EXPECT_EQ(Pose3d(-0.4, 0, 0, 0, 0, 0), jointFour->Pose("joint1"));
  EXPECT_EQ(Pose3d(-0.4, 0.4, 0, 0, 0, 0), jointFour->Pose("joint2"));
  EXPECT_EQ(Pose3d(0, 0.4, 0, 0, 0, 0), jointFour->Pose("joint3"));
  EXPECT_EQ(Pose3d(0, 0, 0, 0, 0, 0), jointFour->Pose("joint4"));
}