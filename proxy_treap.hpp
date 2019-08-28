//
// Created by venikman1 on 8/28/19.
//

#pragma once

#include <memory>

#include "persistent_treap.hpp"

namespace EC {

    template<typename Data, typename ... Modifiers>
    class ProxyTreap {
    private:
        class ProxySetter {
        public:
            ProxySetter(ProxyTreap& self, size_t key) : self(self), setter_key(key) {}

            ProxySetter& operator=(const Data& data) {
                self.current_version = self.main_treap->set_item(setter_key, data, self.current_version);
            }

            ProxySetter& operator=(Data&& data) {
                self.current_version = self.main_treap->set_item(setter_key, data, self.current_version);
            }

            operator Data() {
                return self.main_treap->get_item(setter_key, self.current_version);
            }

        private:
            size_t setter_key;
            ProxyTreap& self;
        };
    public:
        using NodeType = Node<Data, Modifiers ...>;
        using PersistentTreapType = PersistentTreap<Data, Modifiers ...>;

        ProxyTreap() : main_treap(new PersistentTreapType)  {current_version = main_treap->last_version();}
        ProxyTreap(std::shared_ptr<PersistentTreapType> pers_treap, size_t version)
        :main_treap(std::move(pers_treap)), current_version(version) {}

        size_t size() const {
            return main_treap->size(current_version);
        }

        template<typename Modifier>
        const Modifier &get_modifier() const {
            return main_treap->template get_modifier<Modifier>(current_version);
        }

        void append(Data data) {
            current_version = main_treap->append(std::move(data), current_version);
        }

        void insert(size_t key, Data data) {
            current_version = main_treap->insert(key, std::move(data), current_version);
        }

        void remove(size_t key) {
            current_version = main_treap->remove(key, current_version);
        }

        ProxyTreap slice(size_t start, size_t end) {
            return ProxyTreap(main_treap, main_treap->slice(start, end, current_version));
        }

        const Data &operator[](size_t key) const {
            return main_treap->get_item(key, current_version);
        }

        ProxySetter operator[](size_t key) {
            return ProxySetter(*this, key);
        }

        std::pair<ProxyTreap, ProxyTreap> split(size_t key) {
            auto splitted = main_treap->split_into_two_versions(key, current_version);
            return {ProxyTreap(main_treap, splitted.first), ProxyTreap(main_treap, splitted.second)};
        }

        static ProxyTreap merge(const ProxyTreap& left, const ProxyTreap& right) {
            if (left.main_treap != right.main_treap)
                throw TreapException("Cannot merge two proxy treaps from different persistent treap");
            size_t new_ver = left.main_treap->merge_two_versions(left.current_version, right.current_version);
            return ProxyTreap(left.main_treap, new_ver);
        }
        /*
         * Merge/Split logic
         */

    private:
        size_t current_version;
        std::shared_ptr<PersistentTreapType> main_treap;
    };
}