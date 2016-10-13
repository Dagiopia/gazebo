/*
 * Copyright (C) 2016 Open Source Robotics Foundation
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
#include "gazebo/common/Console.hh"
#include "gazebo/physics/ShapePrivate.hh"
#include "gazebo/physics/simbody/SimbodyPhysics.hh"
#include "gazebo/physics/simbody/SimbodyBoxShape.hh"

using namespace gazebo;
using namespace physics;

/////////////////////////////////////////////////
SimbodyBoxShape::SimbodyBoxShape(CollisionPtr _parent)
: BoxShape(_parent)
{
}

/////////////////////////////////////////////////
SimbodyBoxShape::~SimbodyBoxShape() {}

/////////////////////////////////////////////////
void SimbodyBoxShape::SetSize(const ignition::math::Vector3d &_size)
{
  if (_size.Min() < 0)
  {
    gzerr << "Box shape does not support negative size\n";
    return;
  }

  ignition::math::Vector3d size = _size;
  if (ignition::math::equal(size.X(), 0.0))
  {
    // Warn user, but still create shape with very small value
    // otherwise later resize operations using setLocalScaling
    // will not be possible
    gzwarn << "Setting box shape's x to zero \n";
    size.X(1e-4);
  }

  if (ignition::math::equal(size.Y(), 0.0))
  {
    gzwarn << "Setting box shape's y to zero \n";
    size.Y(1e-4);
  }

  if (ignition::math::equal(size.Z(), 0.0))
  {
    gzwarn << "Setting box shape's z to zero \n";
    size.Z(1e-4);
  }

  BoxShape::SetSize(size);

  SimbodyCollisionPtr bParent;
  // Nate: put this back in
  /*bParent = std::dynamic_pointer_cast<SimbodyCollision>(
      this->shapeDPtr->collisionParent);
      */
}