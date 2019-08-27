#include <iostream>
#include <algorithm>

#include "treap.hpp"

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
    using Sum = SumModifier<int>;
    using Min = MinModifier<int>;
    Treap<int, Sum, Min> arr;
    for (size_t i = 0; i < 10; ++i) {
        arr.append(i);
    }

    cout << arr.size() << "\n";
    cout << arr.get_modifier<Sum>().sum << "\n";
    cout << arr.get_modifier<Min>().min_res << "\n\n";
    arr.remove(4);
    arr.insert(4, -41);
    cout << arr.size() << "\n";
    cout << arr.get_modifier<Sum>().sum << "\n";
    cout << arr.get_modifier<Min>().min_res << "\n";
    for (size_t i = 0; i < arr.size(); ++i) {
        cout << arr[i] << "\n";
    }
    return 0;
}