#include "EntityManager.h"
#include "Entity.h"

void EntityManager::init() {
    // to do
}

void EntityManager::removeDeadEntities(EntityVec& vec) {
    // Using erase-remove idiom
    vec.erase(
        std::remove_if(vec.begin(), vec.end(), [](std::shared_ptr<Entity> entity) {
            return !entity->isActive();
            }),
        vec.end()
    );
}

void EntityManager::update() {
    for (auto e : m_toAdd) {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
    }
    m_toAdd.clear();

    removeDeadEntities(m_entities);


    for (auto& pair : m_entityMap) {
        removeDeadEntities(pair.second);
    }

}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
    auto ent = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

    m_toAdd.push_back(ent);

    return ent;
}

EntityVec& EntityManager::getEntities() {
    return m_entities;
}

EntityVec& EntityManager::getEntities(const std::string& tag) {
    // return that entity vec associated with the tag
    return m_entityMap[tag];
}