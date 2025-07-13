#pragma once
#include <vector>

#include "entity.h"
#include "component.h"

namespace bw::pecs {
    class EntityManager final {
    public:
        template <typename... Components>
        class EntityIterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = Entity;
            using difference_type = std::ptrdiff_t;
            using pointer = Entity*;
            using reference = Entity&;

            EntityIterator(EntityManager* manager, const uint32_t index, const uint32_t max_index)
                : manager_(manager), index_(index), max_index_(max_index) {
                findNextValid();
            }

            EntityIterator& operator++() {
                ++index_;
                findNextValid();
                return *this;
            }

            EntityIterator operator++(int) {
                auto tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const EntityIterator& other) const {
                return index_ == other.index_;
            }

            bool operator!=(const EntityIterator& other) const {
                return index_ != other.index_;
            }

            Entity operator*() const {
                return Entity(Entity::Id(index_, manager_->entity_versions_[index_]));
            }

        private:
            EntityManager* manager_;
            uint32_t index_;
            uint32_t max_index_;

            void findNextValid() {
                while (index_ < max_index_) {
                    if (index_ >= manager_->entity_versions_.size()) {
                        index_ = max_index_;
                        break;
                    }

                    Entity entity(Entity::Id(index_, manager_->entity_versions_[index_]));

                    if (manager_->hasAll<Components...>(entity)) {
                        break;
                    }

                    ++index_;
                }
            }
        };

        template <typename... Components>
        class EntityView {
        public:
            explicit EntityView(EntityManager* manager) : manager_(manager) {}

            EntityIterator<Components...> begin() {
                return EntityIterator<Components...>(manager_, 1, manager_->next_index_);
            }

            EntityIterator<Components...> end() {
                return EntityIterator<Components...>(manager_, manager_->next_index_, manager_->next_index_);
            }

        private:
            EntityManager* manager_;
        };

        EntityManager() = default;

        virtual ~EntityManager() = default;

        Entity create() {
            uint32_t index, version;
            if (free_entities_.empty()) {
                index = next_index_++;
                entity_versions_.resize(next_index_);
                version = entity_versions_[index] = 1;
            }
            else {
                index = free_entities_.back();
                free_entities_.pop_back();
                version = entity_versions_[index];
            }

            const Entity entity(Entity::Id(index, version));
            return entity;
        }

        void destroy(const Entity entity) {
            const auto index = entity.id().index();
            entity_versions_[index]++;
            free_entities_.push_back(index);
        }

        template <typename C, typename... Args>
        C* assign(const Entity entity, Args&&... args) {
            const BaseComponent::Family family = component_family<C>();

            if (components_.size() <= family) {
                components_.resize(family + 1);
            }
            if (components_[family].size() <= entity.id().index()) {
                components_[family].resize(entity.id().index() + 1);
            }
            auto* component = new C(std::forward<Args>(args)...);
            components_[family][entity.id().index()] = component;

            return component;
        }

        template <typename C>
            requires (!std::is_const_v<C>)
        C* component(const Entity entity) {
            const BaseComponent::Family family = component_family<C>();
            if (components_.size() <= family) {
                return nullptr;
            }
            if (components_[family].size() <= entity.id().index()) {
                return nullptr;
            }
            return static_cast<C*>(components_[family][entity.id().index()]);
        }

        template <typename C>
            requires (!std::is_const_v<C>)
        const C* component(const Entity entity) const {
            const BaseComponent::Family family = component_family<C>();
            if (components_.size() <= family) {
                return nullptr;
            }
            if (components_[family].size() <= entity.id().index()) {
                return nullptr;
            }
            return static_cast<const C*>(components_[family][entity.id().index()]);
        }

        template <typename... Components>
        EntityView<Components...> entities() {
            return EntityView<Components...>(this);
        }

        template <typename C>
        [[nodiscard]] bool has(const Entity entity) const {
            return component<C>(entity) != nullptr;
        }

        template <typename... Components>
        [[nodiscard]] bool hasAll(const Entity entity) const {
            return (has<Components>(entity) && ...);
        }

        template <typename... Components, typename Func>
        void foreach(Func&& func) requires std::invocable<Func, Entity, Components&...> {
            for (auto entity : entities<Components...>()) {
                func(entity, *component<Components>(entity)...);
            }
        }

    private:
        uint32_t next_index_ = 1;
        std::vector<uint32_t> free_entities_;
        std::vector<uint32_t> entity_versions_;
        std::vector<std::vector<BaseComponent*>> components_;

        template <typename C>
        static BaseComponent::Family component_family() {
            return Component<std::remove_const_t<C>>::family();
        }
    };
}
