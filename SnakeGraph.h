// Snake graph construction and perfect-matching expansion utilities.
#include <iostream>
#include <set>
#include <string>
#include <vector>

/// Relative placement of a tile with respect to the previous tile.
enum class RelativePosition { START, EAST, NORTH };

/// Graph edge with a vertex pair, symbolic label, and internal/boundary flag.
struct Edge
{
	int u, v;
	std::string label;
	bool is_internal = false;
};

/// A single tile in the snake graph.
struct Tile
{
	int id;
	int edge_bottom, edge_left, edge_right, edge_top;
	std::string center_label;

	RelativePosition pos_relative_to_prev;
	int prev_tile_id = -1; // -1 for Tile 1
	int next_tile_id = -1; // -1 for the last tile
};

/**
 * Represents a snake graph together with all perfect matchings and their
 * expansion terms.
 */
class SnakeGraph
{
	using Matching = std::vector<int>;

	std::vector<Edge> edges;
	std::vector<Tile> tiles;
	int num_vertices = 0;
	std::vector<Matching> matchings;
	Matching minimal_perfect_matching; // P_-
	struct ExpansionTerm {
		Matching matching;
		std::vector<int> xor_edges;
		std::vector<int> enclosed_tiles;
	};
	std::vector<ExpansionTerm> expansion_terms;

	int AddUniqueEdge(int u, int v, std::string label);
	static std::string RelativePositionToString(RelativePosition position);
	bool IsBoundaryMatching(const Matching& matching) const;
	bool MatchingContainsEdge(const Matching& matching, int edge_index) const;
	std::vector<int> SymmetricDifference(const Matching& left, const Matching& right) const;
	void PrintMatchingLabels(const Matching& matching) const;

	void Backtrack(int v, std::vector<bool>& visited, Matching& current);

	bool IsEnclosedInternal(const Tile& tile, const std::set<int>& xor_set);

	std::vector<int> FindEnclosedTiles(const std::vector<int>& xor_edges);

	void GlueTileInDirection(Tile& next, const Tile& previous, char direction,
		const std::vector<std::string>& labels);

public:

	/**
	 * @brief Adds a tile and glues it to the previous one based on direction.
	 * Labels must follow: {Bottom, Left, Right, Top, Center}.
	 *
	 * @param direction The placement direction relative to the previous tile.
	 * @param labels Edge and center labels in bottom-left-right-top-center order.
	 */
	void AddTile(char direction, const std::vector<std::string>& labels);

	/** @brief Build all perfect matchings of the current snake graph. */
	void FindAllPerfectMatchings();

	/**
	 * @brief Finds and returns the specific edge indices for P_minus.
	 */
	void GetMinimalMatching();

	/**
	 * @brief Calculates the expansion terms for every perfect matching found.
	 */
	void CalculateExpansion();

	bool HasPerfectMatching() const { return !matchings.empty(); }

	bool HasMinimalPerfectMatching() const { return !minimal_perfect_matching.empty(); }

	bool HasExpansionTerms() const { return !expansion_terms.empty(); }

	void Recalculate() { matchings.clear(); minimal_perfect_matching.clear(); expansion_terms.clear(); }

	/** @brief Print the graph structure to standard output. */
	void Display() const;

	/** @brief Print every perfect matching to standard output. */
	void DisplayPerfectMatchings() const;

	/** @brief Print the minimal perfect matching P_- to standard output. */
	void DisplayMinimalPerfectMatching() const;

	/** @brief Print the computed expansion terms to standard output. */
	void DisplayExpansionTerms() const;

	void ExportExpansionTermsAsLaTex() const;
};