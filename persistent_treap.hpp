//
// Created by venikman1 on 8/28/19.
//

#pragma once

#include "node.hpp"
#include "persistent_treap_iterator.hpp"

namespace EC {

    template<typename Data, typename ... Modifiers>
    class PersistentTreap {
    public:
        using NodeType = Node<Data, Modifiers ...>;
        using const_iterator = PersistentTreapInputIterator<Data, Modifiers ...>;

        PersistentTreap() {
            versions.push_back(nullptr);
        };

        const_iterator begin(size_t version) const {
            const_iterator res(versions[version]);
            res.go_left();
            return res;
        }

        const_iterator end(size_t version) const {
            return const_iterator();
        }

        size_t size(size_t version) const {
            return NodeType::node_size(versions[version]);
        }

        size_t last_version() const {
            return versions.size() - 1;
        };

        template<typename Modifier>
        const Modifier &get_modifier(size_t version) const {
            return versions[version]->template get_modifier<Modifier>();
        }

        size_t append(Data data, size_t version) {
            NodeType *new_node = new NodeType(std::move(data));
            allocated_nodes.push_back(std::unique_ptr<NodeType>(new_node));
            versions.push_back(merge(versions[version], new_node));
            return versions.size() - 1;
        }

        size_t insert(size_t key, Data data, size_t version) {
            ++key; // Array type keys
            NodeType *new_node = new NodeType(std::move(data));
            allocated_nodes.push_back(std::unique_ptr<NodeType>(new_node));
            auto splitted = split(versions[version], key);
            NodeType * res = merge(splitted.first, new_node);
            res = merge(res, splitted.second);
            versions.push_back(res);
            return versions.size() - 1;
        }

        size_t remove(size_t key, size_t version) {
            ++key; // Array type keys
            auto splitted = split(versions[version], key);
            auto splitted2 = split(splitted.second, 2);
            versions.push_back(merge(splitted.first, splitted2.second));
            return versions.size() - 1;
        }

        size_t slice(size_t start, size_t end, size_t version) {
            ++start; ++end; // Array type keys
            auto splitted2 = split(versions[version], end);
            auto splitted1 = split(splitted2.first, start);
            versions.push_back(splitted1.second);
            return last_version();
        }

        const Data &get_item(size_t key, size_t version) const {
            ++key; // Array type keys
            const NodeType *res = get_node(versions[version], key);
            if (res) {
                return res->get_data();
            }
            throw TreapException("Wrong key");
        }

        size_t set_item(size_t key, Data data, size_t version) {
            ++key; // Array type keys
            versions.push_back(set_node_data(versions[version], key, std::move(data)));
            return last_version();
        }

        std::pair<size_t, size_t> split_into_two_versions(size_t key, size_t version) {
            ++key; // Array type keys
            auto splitted = split(versions[version], key);
            versions.push_back(splitted.first);
            versions.push_back(splitted.second);
            return {versions.size() - 2, versions.size() - 1};
        }

        size_t merge_two_versions(size_t version1, size_t version2) {
            versions.push_back(merge(versions[version1], versions[version1]));
            return versions.size() - 1;
        }
        /*
         * Merge/Split logic
         */

    private:
        static int select_priority(const NodeType *left, const NodeType *right) {
            size_t left_size = NodeType::node_size(left);
            size_t right_size = NodeType::node_size(right);
            if (rnd() % (left_size + right_size) < left_size)
                return -1;
            return 1;
        }

        NodeType *make_node_copy(const NodeType* node) {
            NodeType *res = new NodeType(*node);
            allocated_nodes.push_back(std::unique_ptr<NodeType>(res));
            return res;
        }

        NodeType *merge(const NodeType *left, const NodeType *right) {
            if (left == nullptr && right == nullptr)
                return nullptr;
            if (left == nullptr) {
                NodeType* res = make_node_copy(right);
                res->update_all();
                return res;
            }
            if (right == nullptr) {
                NodeType* res = make_node_copy(left);
                res->update_all();
                return res;
            }
            int priority = select_priority(left, right);
            if (priority < 0) {
                NodeType* res = make_node_copy(left);
                res->right = merge(res->right, right);
                res->update_all();
                return res;
            } else {
                NodeType* res = make_node_copy(right);
                res->left = merge(left, res->left);
                res->update_all();
                return res;
            }
        }

        std::pair<NodeType *, NodeType *> split(const NodeType *node, size_t split_key, size_t prev_key = 0) {
            if (node == nullptr)
                return {nullptr, nullptr};
            size_t key = node->get_key(prev_key);
            if (key < split_key) {
                auto splitted = split(node->right, split_key, key);
                NodeType *res = make_node_copy(node);
                res->right = splitted.first;
                res->update_all();
                return {res, splitted.second};
            } else {
                auto splitted = split(node->left, split_key, prev_key);
                NodeType *res = make_node_copy(node);
                res->left = splitted.second;
                res->update_all();
                return {splitted.first, res};
            }
        }

        static const NodeType *get_node(const NodeType *node, size_t get_key, size_t prev_key = 0) {
            if (node == nullptr)
                return nullptr;
            size_t key = node->get_key(prev_key);
            if (key == get_key) {
                return node;
            } else if (key < get_key) {
                return get_node(node->right, get_key, key);
            } else {
                return get_node(node->left, get_key, prev_key);
            }
        }

        NodeType *set_node_data(const NodeType *node, size_t set_key, Data data, size_t prev_key = 0) {
            if (node == nullptr)
                throw TreapException("Can't set node, because nullptr was found");
            size_t key = node->get_key(prev_key);
            if (key == set_key) {
                NodeType *new_node = new NodeType(std::move(data));
                allocated_nodes.push_back(std::unique_ptr<NodeType>(new_node));
                new_node->left = node->left;
                new_node->right = node->right;
                new_node->update_all();
                return new_node;
            } else if (key < set_key) {
                NodeType *res = make_node_copy(node);
                res->right = set_node_data(node->right, set_key, std::move(data), key);
                res->update_all();
                return res;
            } else {
                NodeType *res = make_node_copy(node);
                res->left = set_node_data(node->left, set_key, std::move(data), prev_key);
                res->update_all();
                return res;
            }
        }

        std::vector<const NodeType*> versions;
        std::vector<std::unique_ptr<NodeType>> allocated_nodes;
    };
}