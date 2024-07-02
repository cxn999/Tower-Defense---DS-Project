#pragma once
#include "Components.h"
#include <string>
#include <tuple>

typedef std::tuple <
	CState,
	CAnimation,
	CBoundingBox,
	CInput,
	CLifeSpan,
	CTransform,
	CLevel,
	CHealth,
	CAttack,
	CRange,
	CType,
	CDelay,
	CFocus
	> ComponentTuple;

class Entity { 
	ComponentTuple m_components;
	std::string m_tag = "default";
	bool m_active = true;
	size_t m_id = 0;

	Entity(const size_t& id, const std::string& tag) : m_id(id), m_tag(tag) {}

	friend class EntityManager;
public:

	size_t id();
	bool isActive();
	const std::string& tag();

	void destroy();
	template <typename T, typename... TArgs>
	T & addComponent(TArgs&&... mArgs);
	template <class T>
	bool hasComponent();
	template <typename T>
	T & getComponent();
	template <typename T>
	const T& getComponent() const;
	template <class T>
	void removeComponent();
};

template <typename T, typename... TArgs>
T& Entity::addComponent(TArgs&&... mArgs)
{
	auto& component = getComponent<T>();
	component = T(std::forward<TArgs>(mArgs)...);
	component.has = true;
	return component;
}


template <typename T>
T& Entity::getComponent() {
	return std::get<T>(m_components);
}

template <typename T>
const T& Entity::getComponent() const {
	return std::get<T>(m_components);
}

template <class T>
bool Entity::hasComponent() {
	return getComponent<T>().has;
}

template <class T>
void Entity::removeComponent() {
	getComponent<T>() = T();
}