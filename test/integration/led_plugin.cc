/*
 * Copyright (C) 2017 Open Source Robotics Foundation
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
#include <mutex>
#include <functional>

#include <ignition/math/Rand.hh>

#include "test_config.h"

#include "gazebo/physics/physics.hh"
#include "gazebo/sensors/sensors.hh"
#include "gazebo/common/Timer.hh"
#include "gazebo/rendering/Camera.hh"
#include "gazebo/sensors/CameraSensor.hh"

#include "gazebo/test/ServerFixture.hh"

using namespace gazebo;
class LedPluginTest : public ServerFixture
{
};

std::mutex mutex;

unsigned char* img = nullptr;
unsigned char* img2 = nullptr;
int imageCount = 0;
int imageCount2 = 0;

/////////////////////////////////////////////////
void OnNewCameraFrame(int* _imageCounter, unsigned char* _imageDest,
                  const unsigned char *_image,
                  unsigned int _width, unsigned int _height,
                  unsigned int _depth,
                  const std::string &/*_format*/)
{
  std::lock_guard<std::mutex> lock(mutex);
  memcpy(_imageDest, _image, _width * _height * _depth);
  *_imageCounter += 1;
}

/////////////////////////////////////////////////
TEST_F(LedPluginTest, Blinking)
{
  // Load a world
  // There are a camera and a blinking object controlled by the led plugin, and
  // a black wall behind it.
  Load("worlds/led_plugin_test.world");

  // Make sure the render engine is available.
  if (rendering::RenderEngine::Instance()->GetRenderPathType() ==
      rendering::RenderEngine::NONE)
  {
    gzerr << "No rendering engine, unable to run camera test"
          << std::endl;
    return;
  }
  gazebo::rendering::ScenePtr scene = gazebo::rendering::get_scene();
  ASSERT_NE(nullptr, scene);

  // 1 camera in scene
  rendering::CameraPtr cam = scene->GetCamera(0);
  ASSERT_NE(nullptr, cam);

  imageCount = 0;
  unsigned int width = cam->ImageWidth();
  unsigned int height = cam->ImageHeight();
  img = new unsigned char[width * height * 3];

  event::ConnectionPtr c =
      cam->ConnectNewImageFrame(
      std::bind(&::OnNewCameraFrame, &imageCount, img,
      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
      std::placeholders::_4, std::placeholders::_5));

  common::Time::MSleep(1500);

  EXPECT_GE(imageCount, 1);
  c.reset();

  // after 1.5 sec, it is supposed to be dimmed, i.e., the color is dark.
  for (unsigned int y = 0; y < height; ++y)
  {
    for (unsigned int x = 0; x < width*3; x+=3)
    {
      unsigned int r = img[(y*width*3) + x];
      unsigned int g = img[(y*width*3) + x + 1];
      unsigned int b = img[(y*width*3) + x + 2];
      EXPECT_LT(r, 50u) << "R value is not low enough." << std::endl;
      EXPECT_LT(g, 50u) << "G value is not low enough." << std::endl;
      EXPECT_LT(b, 50u) << "B value is not low enough." << std::endl;
    }
  }

  // get more images
  imageCount = 0;
  c = cam->ConnectNewImageFrame(
      std::bind(&::OnNewCameraFrame, &imageCount, img,
      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
      std::placeholders::_4, std::placeholders::_5));

  common::Time::MSleep(1000);

  EXPECT_GE(imageCount, 1);
  c.reset();

  // after 1 sec, it should flash, i.e., the color is bright red.
  for (unsigned int y = 0; y < height; ++y)
  {
    for (unsigned int x = 0; x < width*3; x+=3)
    {
      unsigned int r = img[(y*width*3) + x];
      unsigned int g = img[(y*width*3) + x + 1];
      unsigned int b = img[(y*width*3) + x + 2];
      EXPECT_GT(r, 250u) << "R value is not high enough." << std::endl;
      EXPECT_GT(g, 127u) << "G value is not high enough." << std::endl;
      EXPECT_GT(b, 127u) << "B value is not high enough." << std::endl;
    }
  }

  delete [] img;
}

int main(int argc, char **argv)
{
  // Set a specific seed to avoid occasional test failures due to
  // statistically unlikely, but possible results.
  ignition::math::Rand::Seed(42);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
