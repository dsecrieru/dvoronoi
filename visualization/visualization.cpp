//
// Created by Daniel Secrieru on 18/06/2023.
//

#include <random>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;
#include <array>
#include <iostream>
#include <unordered_set>

#include <SFML/Graphics.hpp>

#include <dvoronoi/fortune/algorithm.hpp>

//#define MYGAL
#ifdef MYGAL
#include <MyGAL/FortuneAlgorithm.h>
#endif

const int width = 1280;
const int height = 1024;
const std::size_t SITES_COUNT = 100;

typedef float scalar_t;
typedef sf::Vector2f point_t;
#ifdef MYGAL
typedef double mygal_t;
using mygal_point_t = mygal::Vector2<mygal_t>;
#endif

#ifdef MYGAL
void populate(std::vector<point_t>& sites, std::vector<mygal_point_t>& mygal_sites, std::vector<sf::Vector2f>& move_vec, std::size_t sites_count) {
#else
void populate(std::vector<point_t>& sites, std::vector<sf::Vector2f>& move_vec, std::size_t sites_count) {
#endif
    std::random_device rd;
    std::mt19937 rng(0/*rd()*/);
    std::uniform_real_distribution<scalar_t> distrib;

    sites.reserve(sites_count);
    move_vec.reserve(sites_count);
#ifdef MYGAL
    mygal_sites.reserve(sites_count);
#endif
    for(auto i = 0; i < sites_count; ++i) {
        sites.emplace_back(distrib(rng) * (width - 1.0f), distrib(rng) * (height - 1.0f));
        move_vec.emplace_back(distrib(rng) * 2.0f - 1.0f, distrib(rng) * 2.0f - 1.0f);
#ifdef MYGAL
        mygal_sites.emplace_back(sites.back().x, sites.back().y);
#endif
    }
}

sf::CircleShape get_shape(scalar_t r, scalar_t x, scalar_t y, const sf::Color fill) {
    sf::CircleShape circle(r);
    circle.setPosition({ x - r, y - r });
    circle.setFillColor(fill);
    return circle;
}

int main() {

    std::vector<point_t> sites;
    std::vector<point_t> move_vec;
#ifdef MYGAL
    std::vector<mygal_point_t> mygal_sites;
    populate(sites, mygal_sites, move_vec, SITES_COUNT);
#else
    populate(sites, move_vec, SITES_COUNT);
#endif

    std::size_t index = 0;

    sf::RenderWindow window(sf::VideoMode(width, height), "dvoronoi");
#ifdef MYGAL
    sf::RenderWindow window_mygal(sf::VideoMode(width, height), "mygal");
#endif

    std::unordered_set<std::size_t> info_printed;

#ifdef MYGAL
    while (window.isOpen() || window_mygal.isOpen()) {
#else
    while (window.isOpen()) {
#endif
        sf::Event event{};
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Q)
                        window.close();
                    if (event.key.code == sf::Keyboard::N)
                        ++index;
                    break;

                default:
                    break;
            }
        }
//        ++index;

#ifdef MYGAL
        while (window_mygal.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window_mygal.close();
                    break;

                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Q)
                        window_mygal.close();
//                    if (event.key.code == sf::Keyboard::N)
//                        ++index;
                    break;

                default:
                    break;
            }
        }
#endif

        window.clear(sf::Color(100, 100, 100));
#ifdef MYGAL
        window_mygal.clear(sf::Color(100, 100, 100));
#endif

        auto diagram = dvoronoi::fortune::generate(sites, { 0, 0 }, { scalar_t(width), scalar_t(height) });

#ifdef MYGAL
        auto algorithm = mygal::FortuneAlgorithm<mygal_t>(mygal_sites);
        algorithm.construct();
        algorithm.bound(mygal::Box<mygal_t>{ 0.0, height - 1.0, width - 1.0, 0.0 });
        auto mygal_diagram = algorithm.getDiagram();
#endif

        for (const auto& p : sites)
            window.draw(get_shape(2, p.x, p.y, sf::Color(146, 255, 206)));

#ifdef MYGAL
        for (const auto&s : mygal_diagram.getSites())
            window_mygal.draw(get_shape(2, static_cast<scalar_t>(s.point.x), static_cast<scalar_t>(s.point.y), sf::Color(146, 255, 206)));
#endif

        for (const auto he : diagram.half_edges) {
            if (!he.orig || !he.dest)
                continue;

            std::array<sf::Vertex, 2> line = {
                    sf::Vertex({ static_cast<scalar_t>(he.orig->point.x), static_cast<scalar_t>(he.orig->point.y) }),
                    sf::Vertex({ static_cast<scalar_t>(he.dest->point.x), static_cast<scalar_t>(he.dest->point.y) })
            };
            window.draw(line.data(), 2, sf::Lines);
        }

#ifdef MYGAL
        for (const auto he : mygal_diagram.getHalfEdges()) {
            if (!he.origin || !he.destination)
                continue;

            std::array<sf::Vertex, 2> line = {
                    sf::Vertex({ static_cast<scalar_t>(he.origin->point.x), static_cast<scalar_t>(he.origin->point.y) }),
                    sf::Vertex({ static_cast<scalar_t>(he.destination->point.x), static_cast<scalar_t>(he.destination->point.y) })
            };
            window_mygal.draw(line.data(), 2, sf::Lines);
        }
#endif

//        for (const auto v: diagram.vertices) {
//            window.draw(get_shape(1, v.point, sf::Color(255, 50, 50)));
//        }

        if (index > diagram.sites.size() - 1) {
            window.display();
            std::this_thread::sleep_for(10ms);
            continue;
        }

        auto site = diagram.sites[index];
        auto face = site.face;
        window.draw(get_shape(2, static_cast<scalar_t>(site.point.x), static_cast<scalar_t>(site.point.y), sf::Color::Red));

        auto first_he = face->half_edge;
        auto he = first_he;
        bool missing_ends = false;
        bool missing_he = false;
        do {
            if (!he->orig || !he->dest) {
                missing_ends = true;
                he = he->next;
                continue;
            }

            std::array<sf::Vertex, 2> line = {
                sf::Vertex({ static_cast<scalar_t>(he->orig->point.x), static_cast<scalar_t>(he->orig->point.y) }, sf::Color::Red),
                sf::Vertex({ static_cast<scalar_t>(he->dest->point.x), static_cast<scalar_t>(he->dest->point.y) }, sf::Color::Red)
            };
            window.draw(line.data(), 2, sf::Lines);
            he = he->next;
            if (!he)
                missing_he = true;
        } while (he && he != first_he);
//
//        if (!info_printed.contains(index)) {
//            std::cout << index;
//            if (missing_ends)
//                std::cout << ", missing ends";
//            if (missing_he)
//                std::cout << ", missing he";
//            std::cout << std::endl;
//            info_printed.insert(index);
//
//            //if (index == 15 || index == 92 || index == 166 || index == 189 || index == 233)
//            if (index == 15/*53*/)
//                std::cout << std::endl;
//        }

        window.display();

        for (auto i = 0; i < sites.size(); ++i) {
            sites[i] += move_vec[i];
#ifdef MYGAL
            mygal_sites[i].x += move_vec[i].x;
            mygal_sites[i].y += move_vec[i].y;
#endif

            if (sites[i].x < 0 || sites[i].x > width) {
                sites[i].x = std::clamp(sites[i].x, 0.0f, width - 1.0f);
#ifdef MYGAL
                mygal_sites[i].x = std::clamp(mygal_sites[i].x, 0.0, width - 1.0);
#endif
                move_vec[i].x = -move_vec[i].x;
            }

            if (sites[i].y < 0 || sites[i].y > height) {
                sites[i].y = std::clamp(sites[i].y, 0.0f, height - 1.0f);
#ifdef MYGAL
                mygal_sites[i].y = std::clamp(mygal_sites[i].y, 0.0, height - 1.0);
#endif
                move_vec[i].y = -move_vec[i].y;
            }
        }

#ifdef MYGAL
        window_mygal.display();
#endif

        std::this_thread::sleep_for(10ms);
    }
}
