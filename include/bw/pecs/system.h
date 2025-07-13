#pragma once

#include "entity_manager.h"

namespace bw::pecs {
    class System {
    public:
        virtual ~System() = default;
        virtual void update(EntityManager& em, float dt) = 0;
    };
}
