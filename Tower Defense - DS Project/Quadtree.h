#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Entity.h"

struct QuadtreeNode {
    sf::FloatRect bounds;
    std::vector<Entity> entities;
    QuadtreeNode* children[4];

    QuadtreeNode(sf::FloatRect bounds) : bounds(bounds) {
        for (int i = 0; i < 4; ++i) children[i] = nullptr;
    }
};

const int MAX_ENTITIES = 4;

class Quadtree {
public:
    Quadtree() {}
    Quadtree(sf::FloatRect bounds);
    void insert(std::shared_ptr<Entity> entity);
    void clear();
    std::vector<std::shared_ptr<Entity>> query(sf::FloatRect range);

private:
    struct QuadtreeNode {
        sf::FloatRect bounds;
        std::vector<std::shared_ptr<Entity>> entities;
        QuadtreeNode* children[4] = { nullptr, nullptr, nullptr, nullptr };

        QuadtreeNode(sf::FloatRect b) : bounds(b) {}
    };

    QuadtreeNode* root;
    sf::FloatRect bounds;

    void insert(QuadtreeNode* node, std::shared_ptr<Entity> entity);
    void subdivide(QuadtreeNode* node);
    void query(QuadtreeNode* node, sf::FloatRect range, std::vector<std::shared_ptr<Entity>>& found);
};
