/*
 * Cluster algebra formula and perfect matching
 * Based on Musiker-Schiffler (2009)
 *
 * Copyright (c) 2026 WONG, Chi Ping
 * All rights reserved.
 */

#include "SnakeGraph.h"

#include <limits>

/// Sample Graph from Musiker-Schiffler (2009)
void BuildSampleGraph(SnakeGraph& snake_graph)
{
	snake_graph = SnakeGraph();
	snake_graph.AddTile(' ', { "x4", "x5", "x2", "x6", "y1" });
	snake_graph.AddTile('N', { "IGNORE", "x1", "x3", "x8", "y2" });
	snake_graph.AddTile('N', { "IGNORE", "x2", "x7", "x4", "y3" });
	snake_graph.AddTile('E', { "x3", "IGNORE", "x5", "x1", "y4" });
	snake_graph.AddTile('E', { "x4", "IGNORE", "x2", "x6", "y1" });
	snake_graph.AddTile('N', { "IGNORE", "x1", "x3", "x8", "y2" });
	std::cout << "Sample graph loaded." << std::endl;
}

/// Custom graph by the author
void BuildMyGraph(SnakeGraph& snake_graph)
{
	snake_graph = SnakeGraph();
	snake_graph.AddTile(' ', {"x_{10}", "x_{11}", "x_8", "x_2", "y_1"});    // Tile 1
	snake_graph.AddTile('E', {"x_1", "x_8", "x_3", "x_9", "y_2"});          // Tile 2
	snake_graph.AddTile('N', {"x_9", "x_2", "x_4", "x_{10}", "y_3"});       // Tile 3
	snake_graph.AddTile('N', {"x_{10}", "x_3", "x_5", "x_{12}", "y_4"});    // Tile 4
	snake_graph.AddTile('N', {"x_{12}", "x_4", "x_9", "x_6", "y_5"});       // Tile 5
	snake_graph.AddTile('E', {"x_5", "x_9", "x_7", "x_{14}", "y_6"});       // Tile 6
	snake_graph.AddTile('N', {"x_{14}", "x_6", "x_8", "x_{13}", "y_7"});    // Tile 7
	snake_graph.AddTile('N', {"x_{13}", "x_7", "x_1", "x_2", "y_8"});       // Tile 8
	snake_graph.AddTile('E', {"x_8", "x_1", "x_9", "x_3", "y_2"});          // Tile 9
	snake_graph.AddTile('N', {"x_3", "x_2", "x_5", "x_6", "y_9"});          // Tile 10
	snake_graph.AddTile('E', {"x_9", "x_5", "x_{14}", "x_7", "y_6"});       // Tile 11
	std::cout << "Custom graph loaded." << std::endl;
}

void BuildCustomGraph(SnakeGraph& snake_graph)
{
	snake_graph = SnakeGraph();

	int tile_count = 0;
	std::cout << "Enter number of tiles: ";
	if (!(std::cin >> tile_count) || tile_count <= 0)
	{
		std::cout << "Invalid tile count.\n";
		return;
	}

	for (int tile_index = 0; tile_index < tile_count; ++tile_index)
	{
		char direction = ' ';
		if (tile_index > 0)
		{
			std::cout << "Tile " << tile_index + 1 << " direction (E/N): ";
			std::cin >> direction;
		}

		std::vector<std::string> labels(5);
		std::cout << "Enter labels for tile " << tile_index + 1
			<< " in Bottom Left Right Top Center order:\n";
		for (std::string& label : labels)
		{
			std::cin >> label;
		}

		snake_graph.AddTile(direction, labels);
	}
}

void PrintMenu()
{
	std::cout << std::endl << "=== Snake Graph Menu ===" << std::endl
		<< "1. Build graph" << std::endl
		<< "2. Recalculate" << std::endl
		<< "3. Display graph structure" << std::endl
		<< "4. Display perfect matchings" << std::endl
		<< "5. Display minimal perfect matching" << std::endl
		<< "6. Display expansion terms" << std::endl
		<< "7. Export expansion terms as LaTeX (SnakeGraph.log)" << std::endl
		<< "0. Exit" << std::endl
		<< "Select an option: ";
}

void BuildGraph(SnakeGraph& snake_graph)
{
	std::cout << "1. Load sample graph" << std::endl;
	std::cout << "2. Load custom graph" << std::endl;
	std::cout << "3. Build graph interactively" << std::endl;
	std::cout << "Select a graph source: ";
	int graph_choice = 0;
	if (!(std::cin >> graph_choice))
	{
		std::cout << "Invalid input.\n";
		return;
	}

	switch (graph_choice)
	{
	case 1:
		BuildSampleGraph(snake_graph);
		break;
	case 2:
		BuildMyGraph(snake_graph);
		break;
	case 3:
		BuildCustomGraph(snake_graph);
		break;
	default:
		std::cout << "Invalid graph source." << std::endl;
	}
}

int main()
{
	SnakeGraph snake_graph;
	int choice{ -1 };

	while (true)
	{
		PrintMenu();
		if (!(std::cin >> choice))
			break;

		if (choice == 0)
			break;

		switch (choice)
		{
		case 1:
			BuildGraph(snake_graph);
			break;
		case 2:
			snake_graph.Recalculate();
			break;
		case 3:
			snake_graph.Display();
			break;
		case 4:
			if (!snake_graph.HasPerfectMatching())
				snake_graph.FindAllPerfectMatchings();
			snake_graph.DisplayPerfectMatchings();
			break;
		case 5:
			if (!snake_graph.HasPerfectMatching())
				snake_graph.FindAllPerfectMatchings();
			if (!snake_graph.HasMinimalPerfectMatching())
				snake_graph.GetMinimalMatching();
			snake_graph.DisplayMinimalPerfectMatching();
			break;
		case 6:
			if (!snake_graph.HasPerfectMatching())
				snake_graph.FindAllPerfectMatchings();
			if (!snake_graph.HasExpansionTerms())
				snake_graph.CalculateExpansion();
			snake_graph.DisplayExpansionTerms();
			break;
		case 7:
			if (!snake_graph.HasPerfectMatching())
				snake_graph.FindAllPerfectMatchings();
			if (!snake_graph.HasExpansionTerms())
				snake_graph.CalculateExpansion();
			snake_graph.ExportExpansionTermsAsLaTex();
			break;
		default:
			std::cout << "Invalid option." << std::endl;
			break;
		}

		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	return 0;
}