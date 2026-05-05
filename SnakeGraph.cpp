#include "SnakeGraph.h"

#include <algorithm>
#include <fstream>

std::string SnakeGraph::RelativePositionToString(RelativePosition position)
{
	switch (position)
	{
	case RelativePosition::START:
		return "START";
	case RelativePosition::EAST:
		return "EAST";
	case RelativePosition::NORTH:
		return "NORTH";
	default:
		return "UNKNOWN";
	}
}

bool SnakeGraph::MatchingContainsEdge(const Matching& matching, int edge_index) const
{
	return std::find(matching.begin(), matching.end(), edge_index) != matching.end();
}

bool SnakeGraph::IsBoundaryMatching(const Matching& matching) const
{
	for (int edge_index : matching)
		if (edges[edge_index].is_internal)
			return false;
	
	return true;
}

std::vector<int> SnakeGraph::SymmetricDifference(const Matching& left, const Matching& right) const
{
	std::set<int> left_set(left.begin(), left.end());
	std::set<int> right_set(right.begin(), right.end());
	std::vector<int> xor_edges;

	for (int edge_index : left)
		if (!right_set.count(edge_index))
			xor_edges.push_back(edge_index);

	for (int edge_index : right)
		if (!left_set.count(edge_index))
			xor_edges.push_back(edge_index);

	return xor_edges;
}

void SnakeGraph::PrintMatchingLabels(const Matching& matching) const
{
	for (int edge_index : matching)
		std::cout << edges[edge_index].label << " ";
}

int SnakeGraph::AddUniqueEdge(int u, int v, std::string label)
{
	edges.push_back({ u, v, std::move(label), false });
	return static_cast<int>(edges.size()) - 1;
}

void SnakeGraph::Backtrack(int v, std::vector<bool>& visited, Matching& current)
{
	if (v == num_vertices)
	{
		matchings.push_back(current);
		return;
	}

	if (visited[v])
	{
		Backtrack(v + 1, visited, current);
		return;
	}

	for (int index = 0; index < static_cast<int>(edges.size()); ++index)
	{
		const auto& edge_value = edges[index];

		int neighbor = -1;
		if (edge_value.u == v)
			neighbor = edge_value.v;
		else if (edge_value.v == v)
			neighbor = edge_value.u;

		if (neighbor != -1 && !visited[neighbor])
		{
			visited[v] = true;
			visited[neighbor] = true;
			current.push_back(index);

			Backtrack(v + 1, visited, current);

			current.pop_back();
			visited[v] = false;
			visited[neighbor] = false;
		}
	}
}

void SnakeGraph::GetMinimalMatching()
{
	if (tiles.empty())
	{
		minimal_perfect_matching.clear();
		return;
	}

	if (matchings.empty())
		FindAllPerfectMatchings();

	const int target_l1 = tiles.front().edge_left;

	for (const auto& matching : matchings)
		if (IsBoundaryMatching(matching) && MatchingContainsEdge(matching, target_l1))
		{
			minimal_perfect_matching = matching;
			return;
		}
	
	minimal_perfect_matching.clear();
}

bool SnakeGraph::IsEnclosedInternal(const Tile& tile, const std::set<int>& xor_set)
{
	const bool vertical_pair = xor_set.count(tile.edge_left) && xor_set.count(tile.edge_right);
	const bool horizontal_pair = xor_set.count(tile.edge_top) && xor_set.count(tile.edge_bottom);
	const bool bottom_right_pair = xor_set.count(tile.edge_bottom) && xor_set.count(tile.edge_right);
	const bool top_left_pair = xor_set.count(tile.edge_top) && xor_set.count(tile.edge_left);

	bool is_internal = false;

	if (tile.pos_relative_to_prev == RelativePosition::EAST)
		is_internal = horizontal_pair;
	else if (tile.pos_relative_to_prev == RelativePosition::NORTH)
		is_internal = vertical_pair;

	if (!is_internal && tile.next_tile_id > 0)
	{
		const Tile& next = tiles[tile.next_tile_id - 1];
		if (next.pos_relative_to_prev == RelativePosition::EAST)
			is_internal = top_left_pair;
		else if (next.pos_relative_to_prev == RelativePosition::NORTH)
			is_internal = bottom_right_pair;
	}

	return is_internal;
}

std::vector<int> SnakeGraph::FindEnclosedTiles(const std::vector<int>& xor_edges)
{
	std::set<int> xor_set(xor_edges.begin(), xor_edges.end());
	std::vector<int> enclosed;

	for (const auto& tile : tiles)
	{
		int boundary_count = 0;
		if (xor_set.count(tile.edge_bottom)) boundary_count++;
		if (xor_set.count(tile.edge_top)) boundary_count++;
		if (xor_set.count(tile.edge_left)) boundary_count++;
		if (xor_set.count(tile.edge_right)) boundary_count++;

		if (boundary_count == 4)
			enclosed.push_back(tile.id);
		else if (boundary_count == 3)
			enclosed.push_back(tile.id);
		else if (boundary_count == 2)
			if (IsEnclosedInternal(tile, xor_set))
				enclosed.push_back(tile.id);
	}

	return enclosed;
}

void SnakeGraph::GlueTileInDirection(Tile& next, const Tile& previous, char direction,
	const std::vector<std::string>& labels)
{
	if (direction == 'E')
	{
		next.pos_relative_to_prev = RelativePosition::EAST;

		int shared_u = edges[previous.edge_right].u;
		int shared_v = edges[previous.edge_right].v;
		int new_bottom_right = num_vertices++;
		int new_top_right = num_vertices++;

		next.edge_left = previous.edge_right;
		edges[next.edge_left].is_internal = true;

		next.edge_bottom = AddUniqueEdge(shared_u, new_bottom_right, labels[0]);
		next.edge_right = AddUniqueEdge(new_bottom_right, new_top_right, labels[2]);
		next.edge_top = AddUniqueEdge(shared_v, new_top_right, labels[3]);
	}
	else if (direction == 'N')
	{
		next.pos_relative_to_prev = RelativePosition::NORTH;

		int shared_l = edges[previous.edge_top].u;
		int shared_r = edges[previous.edge_top].v;
		int new_top_left = num_vertices++;
		int new_top_right = num_vertices++;

		next.edge_bottom = previous.edge_top;
		edges[next.edge_bottom].is_internal = true;

		next.edge_left = AddUniqueEdge(shared_l, new_top_left, labels[1]);
		next.edge_right = AddUniqueEdge(shared_r, new_top_right, labels[2]);
		next.edge_top = AddUniqueEdge(new_top_left, new_top_right, labels[3]);
	}
}

void SnakeGraph::AddTile(char direction, const std::vector<std::string>& labels)
{
	if (labels.size() < 5)
		return;

	Tile next;
	next.id = tiles.size() + 1;
	next.center_label = labels[4];
	next.prev_tile_id = -1;
	next.next_tile_id = -1;

	if (tiles.empty())
	{
		int bottom_left = 0, bottom_right = 1, top_left = 2, top_right = 3;
		num_vertices = 4;
		next.edge_bottom = AddUniqueEdge(bottom_left, bottom_right, labels[0]);
		next.edge_left = AddUniqueEdge(bottom_left, top_left, labels[1]);
		next.edge_right = AddUniqueEdge(bottom_right, top_right, labels[2]);
		next.edge_top = AddUniqueEdge(top_left, top_right, labels[3]);

		next.pos_relative_to_prev = RelativePosition::START;
	}
	else
	{
		Tile& previous = tiles.back();
		next.prev_tile_id = previous.id;
		previous.next_tile_id = next.id;

		GlueTileInDirection(next, previous, direction, labels);
	}
	tiles.push_back(next);
}

void SnakeGraph::FindAllPerfectMatchings()
{
	matchings.clear();
	std::vector<bool> visited(num_vertices, false);
	Matching current_matching;

	Backtrack(0, visited, current_matching);
}

void SnakeGraph::CalculateExpansion()
{
	GetMinimalMatching();
	std::vector<ExpansionTerm> terms;

	for (const auto& matching_value : matchings)
	{
		ExpansionTerm term;
		term.matching = matching_value;
		term.xor_edges = SymmetricDifference(matching_value, minimal_perfect_matching);
		term.enclosed_tiles = FindEnclosedTiles(term.xor_edges);
		terms.push_back(term);
	}
	expansion_terms = terms;
}

void SnakeGraph::Display() const
{
	std::cout << "=== Snake Graph Structure ===" << std::endl;
	for (const auto& tile : tiles)
	{
		std::cout << "Tile " << tile.id << " (Center: " << tile.center_label << ")" << std::endl;
		std::cout << "  Edges -> B:" << edges[tile.edge_bottom].label
			<< " L:" << edges[tile.edge_left].label
			<< " R:" << edges[tile.edge_right].label
			<< " T:" << edges[tile.edge_top].label
			<< " | Position: " << RelativePositionToString(tile.pos_relative_to_prev)
			<< std::endl;
	}
	std::cout << "Snake Graph built with " << num_vertices << " vertices." << std::endl;
	std::cout << "=== End of Structure ===" << std::endl << std::endl;
}

void SnakeGraph::DisplayPerfectMatchings() const
{
	std::cout << "=== Perfect Matchings ===" << std::endl;
	std::cout << "Total Matchings Found: " << matchings.size() << std::endl;
	int index = 1;
	for (const auto& matching_value : matchings)
	{
		std::cout << "Matching " << index++ << ": ";
		PrintMatchingLabels(matching_value);
		std::cout << std::endl;
	}
	std::cout << "=== End of Matchings ===" << std::endl << std::endl;
}

void SnakeGraph::DisplayMinimalPerfectMatching() const
{
	std::cout << "=== Minimal Perfect Matching (P_-) ===" << std::endl;
	std::cout << "P_-: ";
	PrintMatchingLabels(minimal_perfect_matching);
	std::cout << std::endl;
	std::cout << "=== End of Minimal Perfect Matching ===" << std::endl << std::endl;
}

void SnakeGraph::DisplayExpansionTerms() const
{
	std::cout << "=== Snake Graph Expansion Terms ===" << std::endl;
	int term_index = 1;
	for (const auto& term : expansion_terms)
	{
		std::cout << "Term " << term_index++ << ": " << std::string(20, '-') << std::endl;
		std::cout << "  w(P) = ";
		PrintMatchingLabels(term.matching);
		std::cout << std::endl << "  y(P) = ";
		if (term.enclosed_tiles.empty())
		{
			std::cout << "1";
		}
		else
		{
			for (int tile_id : term.enclosed_tiles)
			{
				std::cout << tiles[tile_id - 1].center_label << " ";
			}
		}
		std::cout << std::endl;
	}
	std::cout << "=== End of Expansion Terms ===" << std::endl;
}

void SnakeGraph::ExportExpansionTermsAsLaTex() const
{
	std::ofstream log_file("SnakeGraph.log");
	if (!log_file.is_open())
		return;

	// Create sorted copy of expansion terms
	auto sorted_terms = expansion_terms;
	
	// Sort by: 1) number of enclosed tiles, 2) by the y(P) labels lexicographically
	std::sort(sorted_terms.begin(), sorted_terms.end(),
		[this](const ExpansionTerm& a, const ExpansionTerm& b) {
			if (a.enclosed_tiles.size() != b.enclosed_tiles.size())
				return a.enclosed_tiles.size() < b.enclosed_tiles.size();
			
			// If same number of tiles, sort by y(P) labels
			std::string label_a, label_b;
			if (a.enclosed_tiles.empty())
				label_a = "1";
			else
				for (int tile_id : a.enclosed_tiles)
					label_a += tiles[tile_id - 1].center_label;
			
			if (b.enclosed_tiles.empty())
				label_b = "1";
			else
				for (int tile_id : b.enclosed_tiles)
					label_b += tiles[tile_id - 1].center_label;
			
			return label_a < label_b;
		});

	auto write_monomial = [this, &sorted_terms, &log_file](int index) {
		const auto& term = sorted_terms[index];
		for (int edge_index : term.matching)
			log_file << edges[edge_index].label << " ";
		if (!term.enclosed_tiles.empty())
		{
			for (int tile_id : term.enclosed_tiles)
				log_file << tiles[tile_id - 1].center_label;
		}
	};

	const int term_count = static_cast<int>(sorted_terms.size());

	auto open_block = [&]() {
		log_file << "\\[" << std::endl;
		log_file << "\\begin{array}{cccc}" << std::endl;
	};

	auto close_block = [&]() {
		log_file << "\\end{array}" << std::endl;
		log_file << "\\]" << std::endl;
	};

	if (term_count == 0)
	{
		log_file << "% No expansion terms" << std::endl;
	}
	else
	{
		open_block();

		for (int i = 0; i < term_count; i += 2)
		{
			if (i == 0)
				log_file << "&";
			else
				log_file << "+&";

			write_monomial(i);

			if (i + 1 < term_count)
			{
				log_file << "&+&";
				write_monomial(i + 1);
			}

			if (i + 2 < term_count)
				log_file << "\\\\";
			log_file << std::endl;
		}

		close_block();
	}
}