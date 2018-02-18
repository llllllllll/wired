#include <iostream>

#include <wired/array.h>

template<typename T, std::size_t n>
std::ostream& operator<<(std::ostream& stream, std::array<T, n> arr) {
    auto it = arr.cbegin();
    stream << '[';
    if (it == arr.cend()) {
        return stream << ']';
    }
    else {
        stream << *it++;
    }
    while (it != arr.cend()) {
        stream << ", " << *it++;
    }
    return stream << ']';
}

template<std::size_t... ns>
std::ostream& operator<<(std::ostream& stream, std::index_sequence<ns...>) {
    return stream << std::array<std::size_t, sizeof...(ns)>{ns...};
}

namespace psyche {
// BEGIN GENERATED CODE
// PSYCHE-GENERATED-CODE
// END GENERATED CODE
}

int main() {
    std::cout << psyche::result::materialize() << '\n';
    return 0;
}
