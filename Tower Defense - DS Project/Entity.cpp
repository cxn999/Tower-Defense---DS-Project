#include "Entity.h"

size_t Entity::id() {
	return m_id;
}
bool Entity::isActive() {
	return m_active;
}

const std::string& Entity::tag() {
	return m_tag;
}


void Entity::destroy() {
	m_active = false;
}