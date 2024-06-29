#pragma once
#include <vector>
#include <memory>
#include <map>
#include "Entity.h"
#include "Quadtree.h" // Include your Quadtree class

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;

class EntityManager {
    EntityVec m_entities;
    EntityVec m_toAdd;
    EntityMap m_entityMap;
    size_t m_totalEntities = 0;
    Quadtree quadtree; // Add the quadtree member

    void removeDeadEntities(EntityVec& vec);
public:
    EntityManager() {}
    EntityManager(sf::FloatRect bounds) : quadtree(bounds) {}; // Initialize quadtree with bounds
    void init();
    void update();
    std::shared_ptr<Entity> addEntity(const std::string& tag);
    EntityVec& getEntities();
    EntityVec& getEntities(const std::string& tag);
    EntityVec queryRange(sf::FloatRect range); // Method to query entities within a range
};
