//
// Created by Daniel Secrieru on 17/07/2023.
//

#ifndef TRACING_RESOURCE_HPP
#define TRACING_RESOURCE_HPP

#include <memory_resource>
#include <iostream>

namespace dvoronoi::memory_management {

class tracing_resource final : public std::pmr::memory_resource {
public:
    tracing_resource(const std::string& name, std::pmr::memory_resource* next = std::pmr::get_default_resource(), std::ostream& out = std::cout)
        : _name(name), _next(next), _out(out) {}

private:
    void* do_allocate(std::size_t bytes, std::size_t align) override {
        auto* addr = _next->allocate(bytes, align);
        _out << '[' << _name << "] allocate(" << bytes << ", " << align << ") -> " << addr << std::endl;
        return addr;
    }

    void do_deallocate(void* addr, std::size_t bytes, std::size_t align) override {
        _out << '[' << _name << "] deallocate(" << addr << ", " << bytes << ", " << align << ")\n";
        _next->deallocate(addr, bytes, align);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        auto p = dynamic_cast<const tracing_resource*>(&other);
        return p && _next == p->_next;
    }

private:
    std::string _name;
    std::pmr::memory_resource* _next;
    std::ostream& _out;
};

}

#endif //TRACING_RESOURCE_HPP