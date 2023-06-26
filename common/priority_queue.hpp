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
    public:
        bool empty() const { return _elements.empty(); }

        void push(std::unique_ptr<T>&& elem) {
            elem->index = _elements.size();
            _elements.emplace_back(std::move(elem));
            sift_up(_elements.size() - 1);
        }

        template<class... Args>
        void emplace(Args&&... args) {
            auto& elem = _elements.emplace_back(std::make_unique<T>(args...));
            elem->index = _elements.size() - 1;
            sift_up(_elements.size() - 1);
        }

        std::unique_ptr<T> pop() {
            swap(0, _elements.size() - 1);
            auto top = std::move(_elements.back());
            _elements.pop_back();
            sift_down(0);
            return top;
        }

    private:
        [[nodiscard]] int get_parent(std::size_t idx) const { return (idx + 1) / 2 - 1; }
        [[nodiscard]] std::size_t left_child(std::size_t idx) const { return 2 * (idx + 1) - 1; }
        [[nodiscard]] std::size_t right_child(std::size_t idx) const { return 2 * (idx + 1); }

        void swap(std::size_t i, std::size_t j) {
            std::swap(_elements[i], _elements[j]);
            _elements[i]->index = i;
            _elements[j]->index = j;
        }

        void sift_up(std::size_t idx) {
            int parent = get_parent(idx);
            if (parent < 0)
                return;

            if (*_elements[parent] < *_elements[idx]) {
                swap(idx, parent);
                sift_up(parent);
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
