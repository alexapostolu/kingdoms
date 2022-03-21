#include "person.hpp"
#include "base.hpp"

#include <SDL.h>

#include <random>
#include <vector>
#include <queue>
#include <iostream>

void Person::generate_path(std::vector<std::vector<Tile>> const& tiles)
{
	int destx = x, desty = y;
	while (destx == x && desty == y)
	{
		destx = distrx(eng);
		desty = distry(eng);
	}

	std::vector<std::vector<int>> costs(tiles.size(), std::vector<int>(tiles[0].size()));
	std::vector<std::vector<bool>> been(tiles.size(), std::vector<bool>(tiles[0].size(), false));
	for (int i = 0; i < costs.size(); ++i)
	{
		for (int j = 0; j < costs[i].size(); ++j)
		{
			if (tiles[i][j].state == TileState::OCCUPIED)
			{
				been[i][j] = true;
				continue;
			}

			int g_cost = abs(j - (int)x) + abs(i - (int)y);
			int h_cost = abs(destx - (int)x) + abs(destx - (int)y);
			costs[i][j] = g_cost + h_cost;
		}
	}

	auto cmp = [&](std::vector<SDL_Point> const& a, std::vector<SDL_Point> const& b)
		{ return costs[a.back().y][a.back().x] < costs[b.back().y][b.back().x]; };
	std::priority_queue<std::vector<SDL_Point>,
						std::vector<std::vector<SDL_Point>>,
						decltype(cmp)> pq(cmp);

	pq.push({ SDL_Point{ (int)x, (int)y } });
	while (!pq.empty())
	{
		auto points = pq.top();
		auto const cx = points.back().x, cy = points.back().y;
		pq.pop();

		if (been[cy][cx])
			continue;

		been[cy][cx] = true;
		
		if (cx == destx && cy == desty)
		{
			for (auto const& v : points)
				path.push_back({ (float)v.x, (float)v.y });

			return;
		}

		points.push_back(SDL_Point{ cx, cy });
		if (cx < costs[0].size() - 1)
		{
			points.back() = SDL_Point{ cx + 1, cy };
			pq.push(points);
		}
		if (cx > 0)
		{
			points.back() = SDL_Point{ cx - 1, cy };
			pq.push(points);
		}
		if (cy < costs.size() - 1)
		{
			points.back() = SDL_Point{ cx, cy + 1 };
			pq.push(points);
		}
		if (cy > 0)
		{
			points.back() = SDL_Point{ cx, cy - 1 };
			pq.push(points);
		}
	}

	std::cout << "stop\n";
	return;
	generate_path(tiles);
}

std::random_device Person::dev;
std::mt19937 Person::eng(Person::dev());
std::uniform_int_distribution<std::mt19937::result_type> Person::distrx(0, Base::get().TILES_X - 1),
	Person::distry(0, Base::get().TILES_Y - 1);