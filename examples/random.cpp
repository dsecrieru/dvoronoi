#include <random>
#include <chrono>
#include <numeric>
#include <iostream>

#include <dvoronoi/fortune/algorithm.hpp>

typedef double scalar_t;
constexpr scalar_t width = 3840;
constexpr scalar_t height = 2160;
constexpr std::size_t runs = 100;

template<typename T>
struct gen_point2d_t {
    T x = T(0), y = T(0);
};

typedef gen_point2d_t<scalar_t> point2d_t;

void bench(std::size_t sites_count) {
    std::vector<std::chrono::milliseconds> durations;

    for (int r = 0; r < runs; ++r) {
        std::vector<point2d_t> sites;
        sites.reserve(sites_count);

        //std::random_device rd;
        //std::mt19937 rng(rd());
        std::mt19937 rng(r);
        std::uniform_real_distribution<scalar_t> distrib;

        for(auto i = 0; i < sites_count; ++i) {
            sites.push_back({ distrib(rng) * (width - 1.0), distrib(rng) * (height - 1.0) });
        }

        const auto start = std::chrono::steady_clock::now();
        auto diagram = dvoronoi::fortune::generate(sites, { 0, 0 }, { width, height });
        const auto end = std::chrono::steady_clock::now();

        auto run_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        durations.push_back(run_duration);

        std::cout << "finished run " << r << " in " << run_duration << std::endl;
    }

    const auto avg_duration = std::reduce(durations.begin(), durations.end()) / float(runs);
    std::cout << std::format("generated in ~{:3}\n", avg_duration);
    //std::cout << "generated in ~" << std::chrono::duration_cast<std::chrono::nanoseconds>(avg_duration).count() << std::endl;
}

int main() {
    bench(100000);
}
