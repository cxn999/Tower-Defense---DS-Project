#include "Quadtree.h"

Quadtree::Quadtree(sf::FloatRect bounds) : root(new QuadtreeNode(bounds)), bounds(bounds) {}

void Quadtree::insert(std::shared_ptr<Entity> entity) {
    insert(root, entity);
}

void Quadtree::clear() {
    delete root;
    root = new QuadtreeNode(bounds);
}

std::vector<std::shared_ptr<Entity>> Quadtree::query(sf::FloatRect range) {
    std::vector<std::shared_ptr<Entity>> found;
    query(root, range, found);
    return found;
}

void Quadtree::insert(QuadtreeNode* node, std::shared_ptr<Entity> entity) {
    if (!node->bounds.intersects(entity->getComponent<CAnimation>().animation.getSprite().getGlobalBounds())) return;

    if (node->entities.size() < MAX_ENTITIES || node->children[0] == nullptr) {
        node->entities.push_back(entity);
    }
    else {
        if (node->children[0] == nullptr) subdivide(node);
        for (int i = 0; i < 4; ++i) insert(node->children[i], entity);
    }
}

void Quadtree::subdivide(QuadtreeNode* node) {
    float x = node->bounds.left;
    float y = node->bounds.top;
    float width = node->bounds.width / 2;
    float height = node->bounds.height / 2;

    node->children[0] = new QuadtreeNode(sf::FloatRect(x, y, width, height));
    node->children[1] = new QuadtreeNode(sf::FloatRect(x + width, y, width, height));
    node->children[2] = new QuadtreeNode(sf::FloatRect(x, y + height, width, height));
    node->children[3] = new QuadtreeNode(sf::FloatRect(x + width, y + height, width, height));
}

void Quadtree::query(QuadtreeNode* node, sf::FloatRect range, std::vector<std::shared_ptr<Entity>>& found) {
    if (!node->bounds.intersects(range)) return;

    for (auto& entity : node->entities) {
        if (range.intersects(entity->getComponent<CAnimation>().animation.getSprite().getGlobalBounds())) {
            found.push_back(entity);
        }
    }

    if (node->children[0] != nullptr) {
        for (int i = 0; i < 4; ++i) query(node->children[i], range, found);
    }
}
