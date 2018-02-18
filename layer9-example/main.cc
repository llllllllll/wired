#include <array>
#include <iostream>
#include <utility>

#include <layer9/predict.h>

// load our training data
#include "samples.p7"
#include "observations.p7"

/** Allow printing `std::array`.
 */
template<typename T, std::size_t n>
std::ostream& operator<<(std::ostream& stream, std::array<T, n> arr) {
    auto it = arr.cbegin();
    stream << '{';
    if (it == arr.cend()) {
        return stream << '}';
    }
    else {
        stream << *it++;
    }
    while (it != arr.cend()) {
        stream << ", " << *it++;
    }
    return stream << '}';
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "usage: " << argv[0] << " DOUBLE DOUBLE DOUBLE\n";
        return -1;
    }

    // read the 3 inputs from stdin
    std::array<double, 3> data {std::stod(argv[1]),
                                std::stod(argv[2]),
                                std::stod(argv[3])};

    // make a prediction for the input data based on the compile-time training
    // data
    auto prediction = layer9::predict<protocol7::samples,
                                      protocol7::observations>(data);
    std::cout << prediction << '\n';
    return 0;
}
