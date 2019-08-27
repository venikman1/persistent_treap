//
// Created by venikman1 on 8/27/19.
//

#pragma once

#include <vector>
#include <memory>
#include <random>
#include <exception>

#include "node.hpp"

namespace EC {
    class TreapException : public std::exception {
    public:
        TreapException(const char *msg) : msg(msg) {}

        virtual const char *what() const throw() {
            return msg;
        }

    private:
        const char *msg;
    };

    std::mt19937 rnd;

    template<typename Data, typename ... Modifiers>
    class Treap {
    public:
        using NodeType = Node<Data, Modifiers ...>;

        Treap() = default;

        size_t size() const {
            return NodeType::node_size(root);
        }

        template<typename Modifier>
        const Modifier &get_modifier() const {
            return root->template get_modifier<Modifier>();
        }

        void append(Data data) {
            NodeType *new_node = new NodeType(std::move(data));
            allocated_nodes.push_back(std::unique_ptr<NodeType>(new_node));
            root = merge(root, new_node);
        }

        void insert(size_t key, Data data) {
            ++key; // Array type keys
            NodeType *new_node = new NodeType(std::move(data));
            allocated_nodes.push_back(std::unique_ptr<NodeType>(new_node));
            auto splitted = split(root, key);
            root = merge(splitted.first, new_node);
            root = merge(root, splitted.second);
        }

        void remove(size_t key) {
            ++key; // Array type keys
            auto splitted = split(root, key);
            auto splitted2 = split(splitted.second, 2);
            root = merge(splitted.first, splitted2.second);
        }

        const Data &operator[](size_t key) const {
            ++key; // Array type keys
            const NodeType *res = get_node(root, key);
            if (res) {
                return res->get_data();
            }
            throw TreapException("Wrong key");
        }
        /*
         * Merge/Split logic
         */

    private:
        static int select_priority(NodeType *left, NodeType *right) {
            size_t left_size = NodeType::node_size(left);
            size_t right_size = NodeType::node_size(right);
            if (rnd() % (left_size + right_size) < left_size)
                return -1;
            return 1;
        }

        static NodeType *merge(NodeType *left, NodeType *right) {
            if (left == nullptr && right == nullptr)
                return nullptr;
            if (left == nullptr) {
                right->update_all();
                return right;
            }
            if (right == nullptr) {
                left->update_all();
                return left;
            }
            int priority = select_priority(left, right);
            if (priority < 0) {
                left->right = merge(left->right, right);
                left->update_all();
                return left;
            } else {
                right->left = merge(left, right->left);
                right->update_all();
                return right;
            }
        }

        static std::pair<NodeType *, NodeType *> split(NodeType *node, size_t split_key, size_t prev_key = 0) {
            if (node == nullptr)
                return {nullptr, nullptr};
            size_t key = node->get_key(prev_key);
            if (key < split_key) {
                auto splitted = split(node->right, split_key, key);
                node->right = splitted.first;
                node->update_all();
                return {node, splitted.second};
            } else {
                auto splitted = split(node->left, split_key, prev_key);
                node->left = splitted.second;
                node->update_all();
                return {splitted.first, node};
            }
        }

        static const NodeType *get_node(const NodeType *node, size_t get_key, size_t prev_key = 0) {
            if (node == nullptr)
                nullptr;
            size_t key = node->get_key(prev_key);
            if (key == get_key) {
                return node;
            } else if (key < get_key) {
                return get_node(node->right, get_key, key);
            } else {
                return get_node(node->left, get_key, prev_key);
            }
        }

        NodeType *root = nullptr;
        std::vector<std::unique_ptr<NodeType>> allocated_nodes;
    };

}