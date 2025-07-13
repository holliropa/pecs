#pragma once

namespace bw::pecs {
    class EntityManager;

    class Entity {
    public:
        struct Id {
            Id() : id_(0) {}
            explicit Id(const uint64_t id) : id_(id) {}

            Id(const uint32_t index, const uint32_t version)
                : id_(static_cast<uint64_t>(index) | (static_cast<uint64_t>(version) << 32)) {}

            ~Id() = default;

            [[nodiscard]] uint32_t index() const { return id_ & 0xFFFFFFFFUL; }
            [[nodiscard]] uint32_t version() const { return id_ >> 32; }

            [[nodiscard]] bool operator==(const Id& other) const { return id_ == other.id_; }
            [[nodiscard]] bool operator!=(const Id& other) const { return id_ != other.id_; }

        private:
            uint64_t id_;
        };

        inline static const Id INVALID;

        Entity() = default;
        explicit Entity(const Id id) : id_(id) {}

        [[nodiscard]] Id id() const { return id_; }

    private:
        Id id_ = INVALID;
    };
}
