/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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
#include "ignition/gazebo/detail/View.hh"
#include "ignition/gazebo/EntityComponentManager.hh"

#define BOOST_STACKTRACE_USE_ADDR2LINE
#include <boost/stacktrace.hpp>

using namespace ignition;
using namespace gazebo;
using namespace detail;

static Entity to_lookat = kNullEntity;
//////////////////////////////////////////////////
void View::AddEntity(const Entity _entity, const bool _new)
{
  // std::cout << "AddEntity current thread: " << std::this_thread::get_id() << std::endl;
  if (to_lookat == _entity)
  {
    printf("View::AddEntity entity added: %lu\n", _entity);
    std::cout << "thread: " << std::this_thread::get_id() << std::endl;
  }
  this->entities.insert(_entity);
  if (_new)
  {
    this->newEntities.insert(_entity);
  }
}

//////////////////////////////////////////////////
void View::AddComponent(const Entity _entity,
    const ComponentTypeId _typeId,
    const ComponentId _componentId)
{
  // std::cout << "AddComponent current thread: " << std::this_thread::get_id() << std::endl;
  if (to_lookat == _entity)
  {
    printf("View::AddComponent entity: %lu\n", _entity);
    std::cout << "thread: " << std::this_thread::get_id() << std::endl;
    std::cout << boost::stacktrace::stacktrace() << std::endl;
  }
  this->components.insert(
      std::make_pair(std::make_pair(_entity, _typeId), _componentId));
}

//////////////////////////////////////////////////
bool View::RemoveEntity(const Entity _entity, const ComponentTypeKey &_key)
{
  if (this->entities.find(_entity) == this->entities.end())
    return false;
  printf("ent removed: %lu\n", _entity);
  // Otherwise, remove the entity from the view
  this->entities.erase(_entity);
  this->newEntities.erase(_entity);
  this->toRemoveEntities.erase(_entity);

  // Remove the entity from the components map
  for (const ComponentTypeId &compTypeId : _key)
    this->components.erase(std::make_pair(_entity, compTypeId));

  return true;
}

/////////////////////////////////////////////////
const components::BaseComponent *View::ComponentImplementation(
    const Entity _entity,
    ComponentTypeId _typeId,
    const EntityComponentManager *_ecm) const
{
#if 0
  try
  {
    this->components.at({_entity, _typeId});
  }
  catch (...)
  {
    printf("qweqw\n");
    printf("entity: %llu typeid %lu\n", _entity, _typeId);
    std::cout << "current thread: " << std::this_thread::get_id() << std::endl;
    /*for (auto it : components)
    {
      printf("ent: %lu cmpid: %lu\n", it.first.first, it.first.second);
    }*/
  }
#else
  if (this->components.find({_entity, _typeId}) == this->components.end())
  {
    printf("sleeping in View::ComponentImplementation(), entity: %lu\n", _entity);
    std::cout << "thread: " << std::this_thread::get_id() << std::endl;
    to_lookat = _entity;
    sleep(1);
  }
#endif
  return _ecm->ComponentImplementation(
      {_typeId, this->components.at({_entity, _typeId})});
}

//////////////////////////////////////////////////
void View::ClearNewEntities()
{
  this->newEntities.clear();
}

//////////////////////////////////////////////////
bool View::AddEntityToRemoved(const Entity _entity)
{
  if (this->entities.find(_entity) == this->entities.end())
    return false;
  this->toRemoveEntities.insert(_entity);
  return true;
}
