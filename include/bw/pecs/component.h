#pragma once

namespace bw::pecs {
    class BaseComponent {
    public:
        typedef size_t Family;

        void operator delete(void* p) = delete;
        void operator delete[](void* p) = delete;

    protected:
        inline static Family family_counter_ = 0;
    };

    template <typename Derived>
    class Component : public BaseComponent {
    public:
        static Family family() {
            static Family family = family_counter_++;

            return family;
        }
    };
}
