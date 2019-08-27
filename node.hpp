//
// Created by venikman1 on 8/27/19.
//

#pragma once

#include <utility>
#include <type_traits>

namespace EC {
/*
 * What is Modifier:
 * Modifier is a class which store some user-defines information for some segment of treap
 * Modifier can contain any information
 *
 * Modifier must provide method void update(const Data& data, const Modifier* left, const Modifier* right)
 * which updates current Modifier with child Modifiers and current node data
 *
 *
 */

    template<typename Data, typename ... Modifiers>
    class Node : private Modifiers ... {
    public:
        Node() = default;

        Node(Data data) : data(std::move(data)) {
            update_all();
        }


        static size_t node_size(const Node *node) {
            return node ? node->get_size() : 0;
        }

        size_t get_size() const {
            return subtree_size;
        }

        const Data &get_data() const {
            return data;
        }

        size_t update_size() {
            return subtree_size = 1 + node_size(left) + node_size(right);
        }

        size_t get_key(size_t prev_key) const {
            return node_size(left) + prev_key + 1;
        }

        template<typename Modifier>
        const Modifier &get_modifier() const {
            return *this;
        }

        template<typename UpdateFirstModifier, typename UpdateSecondModifier, typename ... UpdateModifiers>
        void update_modifiers() {
            const UpdateFirstModifier *left_mod = (left ? &(left->get_modifier<UpdateFirstModifier>()) : nullptr);
            const UpdateFirstModifier *right_mod = (right ? &(right->get_modifier<UpdateFirstModifier>()) : nullptr);
            UpdateFirstModifier::update(this->data, left_mod, right_mod);
            update_modifiers<UpdateSecondModifier, UpdateModifiers ...>();
        }

        template<typename UpdateFirstModifier>
        void update_modifiers() {
            const UpdateFirstModifier *left_mod = (left ? &(left->get_modifier<UpdateFirstModifier>()) : nullptr);
            const UpdateFirstModifier *right_mod = (right ? &(right->get_modifier<UpdateFirstModifier>()) : nullptr);
            UpdateFirstModifier::update(this->data, left_mod, right_mod);
        }

        void update_all() {
            update_size();
            update_modifiers<Modifiers ...>();
        }


        Node *left = nullptr;
        Node *right = nullptr;

    private:
        size_t subtree_size = 1;
        Data data = Data();
    };
}