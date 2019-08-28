#include <iostream>
#include <algorithm>

#include "treap.hpp"
#include "persistent_treap.hpp"
#include "proxy_treap.hpp"

using namespace std;
using namespace EC;

template <typename Data>
struct SumModifier {
    void update(const Data& data, const SumModifier* left, const SumModifier* right) {
        sum = data;
        if (left)
            sum += left->sum;
        if (right)
            sum += right->sum;
    }

    Data sum = 0;
};

template <typename Data>
struct MinModifier {
    void update(const Data& data, const MinModifier* left, const MinModifier* right) {
        min_res = data;
        if (left)
            min_res = min(min_res, left->min_res);
        if (right)
            min_res = min(min_res, right->min_res);
    }

    Data min_res = 0;
};

int main() {
    using Sum = SumModifier<long long>;
    using Min = MinModifier<long long>;
//    Treap<int, Sum, Min> arr;
//    for (size_t i = 0; i < 10; ++i) {
//        arr.append(i);
//    }
//
//    cout << arr.size() << "\n";
//    cout << arr.get_modifier<Sum>().sum << "\n";
//    cout << arr.get_modifier<Min>().min_res << "\n\n";
//    arr.remove(4);
//    arr.insert(4, -41);
//    cout << arr.size() << "\n";
//    cout << arr.get_modifier<Sum>().sum << "\n";
//    cout << arr.get_modifier<Min>().min_res << "\n";
//    for (size_t i = 0; i < arr.size(); ++i) {
//        cout << arr[i] << "\n";
//    }

//    PersistentTreap<long long, Sum, Min> arr;
//    size_t cur_ver = arr.last_version();
//    for (size_t i = 0; i < 10; ++i) {
//        cur_ver = arr.append(i, cur_ver);
//    }
//    cout << "Size is " << arr.size(cur_ver) << "\n";
//    cout << "Sum is " << arr.get_modifier<Sum>(cur_ver).sum << "\n";
//    for (size_t i = 0; i < 2; ++i) {
//        cur_ver = arr.merge_two_versions(cur_ver, cur_ver);
//    }
//    cout << "\nCopied 2 times\n";
//    cout << "Size is " << arr.size(cur_ver) << "\n";
//    cout << "Sum is " << arr.get_modifier<Sum>(cur_ver).sum << "\n";
//    cur_ver = arr.set_item(5, -175, cur_ver);
//    for (size_t i = 0; i < arr.size(cur_ver); ++i) {
//        size_t idx = i;
//        cout << "arr[" << idx << "] = " << arr.get_item(idx, cur_ver) << "\n";
//    }
//    cout << "Size is " << arr.size(cur_ver) << "\n";
//    cout << "Sum is " << arr.get_modifier<Sum>(cur_ver).sum << "\n";

    ProxyTreap<long long, Sum> arr;

    arr.append(0);
    arr.append(5);

    for (int i = 0; i < 10; ++i)
        arr = ProxyTreap<long long, Sum> ::merge(arr, arr);
    for (int i = 0; i < arr.size(); ++i)
        cout << "arr[" << i << "] = " << arr[i] << "\n";
    cout << "Size is " << arr.size() << "\n";
    cout << "Sum is " << arr.get_modifier<Sum>().sum << "\n\n";

    arr[2] = 10000;
    for (int i = 0; i < arr.size(); ++i)
        cout << "arr[" << i << "] = " << arr[i] << "\n";
    cout << "Size is " << arr.size() << "\n";
    cout << "Sum is " << arr.get_modifier<Sum>().sum << "\n\n";

    auto slice = arr.slice(1, 9);
    for (int i = 0; i < slice.size(); ++i)
        cout << "arr[" << i << "] = " << slice[i] << "\n";
    cout << "Size is " << slice.size() << "\n";
    cout << "Sum is " << slice.get_modifier<Sum>().sum << "\n\n";
    return 0;
}