#include "core/headers/ObjectContainer.h"

namespace project_playground::core {

ObjectContainer::ObjectContainer(std::vector<std::unique_ptr<interfaces::IObject>> objects)
    : m_objects(std::move(objects))
{
}

void ObjectContainer::AddObject(std::unique_ptr<interfaces::IObject> object)
{
    m_objects.push_back(std::move(object));
}

std::vector<std::unique_ptr<interfaces::IObject>>& ObjectContainer::GetObjects()
{
    return m_objects;
}

const std::vector<std::unique_ptr<interfaces::IObject>>& ObjectContainer::GetObjects() const
{
    return m_objects;
}

}