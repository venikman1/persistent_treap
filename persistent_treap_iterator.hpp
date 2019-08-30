//
// Created by venikman1 on 8/28/19.
//

#pragma once

#include <iterator>
#include <stack>

#include "node.hpp"

namespace EC {

    template<typename Data, typename ... Modifiers>
    class PersistentTreapInputIterator : public std::iterator<
            std::input_iterator_tag,
            Data,
            long,
            const Data*,
            const Data&> {
    private:
        using NodeType = Node<Data, Modifiers...>;
    public:
        PersistentTreapInputIterator() = default;
        PersistentTreapInputIterator(const NodeType* cur_node) : current_node(cur_node) {} // optimization for it++ logic

        const Data& operator*() const {
            return current_node->get_data();
        }

        const Data* operator->() const {
            return &(current_node->get_data());
        }

        PersistentTreapInputIterator& operator++() {
            if (current_node == nullptr) {
                return *this;
            }
            if (current_node->right != nullptr) {
                go_down(current_node->right);
                go_left();
                return *this;
            }
            while (is_it_right_son()) {
                go_up();
            }
            if (is_it_left_son()) {
                go_up();
                return *this;
            }
            current_node = nullptr;
            return *this;
        }

        PersistentTreapInputIterator operator++(int) {
            PersistentTreapInputIterator res(current_node); // Don't copy stack
            ++*this;
            return res;
        }

        bool operator==(const PersistentTreapInputIterator& it) const {
            return current_node == it.current_node;
        }

        bool operator!=(const PersistentTreapInputIterator& it) const {
            return current_node != it.current_node;
        }

        void go_left() {
            while (current_node->left != nullptr)
                go_down(current_node->left);
        }

    private:
        bool go_up() {
            if (descent_stack.empty()) {
                return false;
            } else {
                current_node = descent_stack.top();
                descent_stack.pop();
                return true;
            }
        }

        void go_down(const NodeType* node) {
            descent_stack.push(current_node);
            current_node = node;
        }

        bool is_it_left_son() {
            return !descent_stack.empty() && descent_stack.top()->left == current_node;
        }

        bool is_it_right_son() {
            return !descent_stack.empty() && descent_stack.top()->right == current_node;
        }

        std::stack<const NodeType*> descent_stack;
        const NodeType* current_node = nullptr;

    };
}