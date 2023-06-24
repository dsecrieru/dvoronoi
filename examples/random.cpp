#include <random>
#include <chrono>
#include <numeric>
#include <iostream>

#include <dvoronoi/fortune/algorithm.hpp>

constexpr double width = 640;
constexpr double height = 480;
constexpr std::size_t runs = 10;

template<typename T>
struct gen_point2d_t {
    T x = T(0), y = T(0);
};

typedef double scalar_t;
typedef gen_point2d_t<scalar_t> point2d_t;

void populate(std::vector<point2d_t>& sites, std::size_t sites_count) {
    std::random_device rd;
    //std::mt19937 rng(rd());
    std::mt19937 rng(0);
    std::uniform_real_distribution<scalar_t> distrib;

    for(auto i = 0; i < sites_count; ++i) {
        sites.push_back({ distrib(rng) * (width - 1.0f), distrib(rng) * (height - 1.0f) });
    }
}

void bench(std::size_t sites_count) {
    std::vector<std::chrono::milliseconds> durations;

    for (int i = 0; i < runs; ++i) {
        std::vector<point2d_t> sites;
        sites.reserve(sites_count);
        populate(sites, sites_count);

        const auto start = std::chrono::steady_clock::now();
        auto diagram = dvoronoi::fortune::generate(sites, { 0, 0 }, { width, height });
        const auto end = std::chrono::steady_clock::now();

        auto run_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        durations.push_back(run_duration);

        std::cout << "finished run " << i << " in " << run_duration << std::endl;
    }

    const auto avg_duration = std::reduce(durations.begin(), durations.end()) / float(runs);
    std::cout << std::format("generated in ~{:3}\n", avg_duration);
    //std::cout << "generated in ~" << std::chrono::duration_cast<std::chrono::nanoseconds>(avg_duration).count() << std::endl;
}

int main() {
    bench(100000);
}
