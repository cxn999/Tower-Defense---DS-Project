#include "EntityManager.h"
#include "Entity.h"

void EntityManager::init() {
    // Initialization code, if any
}

void EntityManager::removeDeadEntities(EntityVec& vec) {
    vec.erase(
        std::remove_if(vec.begin(), vec.end(), [](std::shared_ptr<Entity> entity) {
            return !entity->isActive();
            }),
        vec.end()
    );
}

void EntityManager::update() {
    // Add new entities
    for (auto e : m_toAdd) {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
        quadtree.insert(e); // Insert entity into quadtree
    }
    m_toAdd.clear();

    // Remove dead entities from main vector and quadtree
    removeDeadEntities(m_entities);
    quadtree.clear(); // Clear quadtree and reinsert active entities
    for (auto& entity : m_entities) {
        quadtree.insert(entity);
    }

    // Remove dead entities from the entity map
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
    return m_entityMap[tag];
}

EntityVec EntityManager::queryRange(sf::FloatRect range) {
    return quadtree.query(range); // Query entities within a range using quadtree
}
