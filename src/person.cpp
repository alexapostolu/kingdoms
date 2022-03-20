#include "person.hpp"
#include "base.hpp"

#include <SDL.h>

#include <random>
#include <vector>
#include <queue>

void Person::generate_path(std::vector<std::vector<Tile>> const& tiles)
{
	int const destx = distrx(eng), desty = distry(eng);

	std::vector<std::vector<int>> costs(tiles.size(), std::vector<int>(tiles[0].size(), 0));
	for (int i = 0; i < costs.size(); ++i)
	{
		for (int j = 0; j < costs[i].size(); ++j)
		{
			if (tiles[i][j].state == TileState::OCCUPIED)
				continue;

			int g_cost = abs(j - x) + abs(i - y);
			int h_cost = abs(destx - x) + abs(destx - y);
			costs[i][j] = g_cost + h_cost;
		}
	}

	auto cmp = [&](SDL_Point const& a, SDL_Point const& b) { return costs[a.y][a.x] < costs[b.y][b.x]; };
	std::priority_queue<SDL_Point, std::vector<SDL_Point>, decltype(cmp)> pq(cmp);

	pq.push(SDL_Point{ (int)x, (int)y });
	while (!pq.empty())
	{
		auto pnt = pq.top();
		pq.pop();

		if (pnt.x < 0 || pnt.x > costs[0].size() || pnt.y < 0 || pnt.y > costs.size())
			continue;
		if (pnt.x)
	}
}

std::random_device Person::dev;
std::mt19937 Person::eng(Person::dev());
std::uniform_int_distribution<std::mt19937::result_type> Person::distrx(0, Base::get().TILES_X - 1),
	Person::distry(0, Base::get().TILES_Y - 1);