#pragma once
#include <cmath>
#include <array>
#include <vector>
#include <numbers>
#include <ostream>
#include <limits>
#include <memory>
#include <random>
#include <utility>

using std::make_shared;
using std::shared_ptr;

const double infinity = std::numeric_limits<double>::infinity();
using std::numbers::pi;
using ld = float;

const ld eps = 1e-5;
inline int sgn(ld x) { return (x > -eps) - (x < eps); }

inline double degToRad(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline ld radToDegree(ld rad) {
    return rad * 180 / pi;
}
