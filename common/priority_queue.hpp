//
// Created by Daniel Secrieru on 26/06/2023.
//

#ifndef DVORONOI_PRIORITY_QUEUE_HPP
#define DVORONOI_PRIORITY_QUEUE_HPP

#include <memory>
#include <vector>

namespace dvoronoi {

    template<typename T>
    class priority_queue_t {
    private:
        [[nodiscard]] int parent(std::size_t idx) const { return (idx + 1) / 2 - 1; }
        [[nodiscard]] std::size_t left_child(std::size_t idx) const { return 2 * (idx + 1) - 1; }
        [[nodiscard]] std::size_t right_child(std::size_t idx) const { return 2 * (idx + 1); }

        void update(std::size_t idx) {
            auto p = parent(idx);
            if (p >= 0 && *_elements[p] < *_elements[idx])
                sift_up(idx);
            else
                sift_down(idx);
        }

    public:
        priority_queue_t() {}
        explicit priority_queue_t(std::size_t reserve_size) {
            _elements.reserve(reserve_size);
        }

    public:
        [[nodiscard]] bool empty() const { return _elements.empty(); }
        [[nodiscard]] std::size_t size() const { return _elements.size(); }

        template<class... Args>
        T* emplace(Args&&... args) {
            const auto& elem = _elements.emplace_back(std::make_unique<T>(args...));
            T* ret = elem.get();
            elem->index = _elements.size() - 1;
            sift_up(_elements.size() - 1);
            return ret;
        }

        std::unique_ptr<T> pop() {
            swap(0, _elements.size() - 1);
            auto top = std::move(_elements.back());
            _elements.pop_back();
            sift_down(0);
            return top;
        }

        void remove(std::size_t idx) {
            swap(idx, _elements.size() - 1);
            _elements.pop_back();
            if (idx < _elements.size())
                update(idx);
        }

    private:
        void swap(std::size_t i, std::size_t j) {
            std::swap(_elements[i], _elements[j]);
            _elements[i]->index = i;
            _elements[j]->index = j;
        }

        void sift_up(std::size_t idx) {
            int p = parent(idx);
            if (p < 0)
                return;

            if (*_elements[p] < *_elements[idx]) {
                swap(idx, p);
                sift_up(p);
            }
        }

        void sift_down(std::size_t idx) {
            std::size_t left = left_child(idx);
            std::size_t right = right_child(idx);
            std::size_t j = idx;
            if (left < _elements.size() && *_elements[j] < *_elements[left])
                j = left;
            if (right < _elements.size() && *_elements[j] < *_elements[right])
                j = right;
            if (j != idx) {
                swap(idx, j);
                sift_down(j);
            }
        }

    private:
        std::vector<std::unique_ptr<T>> _elements;
    };

} // namespace dvoronoi

#endif //DVORONOI_PRIORITY_QUEUE_HPP
