#pragma once
#include <string>
#include <tuple>
#include <sstream>
#include <limits>
#include <fstream>
#include <iostream>

#include "Graph.h"

namespace uniza_fri {

	typedef unsigned long long ull;

	/*
		Táto trieda je zodpovedná za naèítavanie grafov zo zdrojovıch súborov.	 
		
		vertex_data_t -> typ dát, ktoré sa vkladajú do vrcholov.
	 */
	class Roads
	{
	public:

		/*
			Naèítanı graf vráti ako inštanciu triedy Graph.
			Táto inštancia je alokovaná operátorom new,
			take sa treba postara o jej zmazanie.
		 */
		template<typename vertex_data_t>
		static Graph<ull, vertex_data_t> * load(const std::string & name);
	
	private:
		
		template<typename t1, typename t2, typename t3, typename t4>
		static void split(std::string & line, std::tuple<t1, t2, t3, t4> & tpl);

	};

	template<typename t1, typename t2, typename t3, typename t4>
	void Roads::split(std::string & line, std::tuple<t1, t2, t3, t4>& tpl)
	{
		std::istringstream istr(line);

		istr >> std::get<0>(tpl);
		istr >> std::get<1>(tpl);
		istr >> std::get<2>(tpl);
		istr >> std::get<3>(tpl);
	}

	template<typename vertex_data_t>
	Graph<ull, vertex_data_t>* Roads::load(const std::string & name)
	{
		const std::string fileName("roads/" + name + ".gr");
		std::ifstream ifs(fileName);

		if (!ifs.is_open()) return nullptr;

		std::string line;
		for (size_t i = 0; i < 4; i++) std::getline(ifs, line); // just skip lines

		std::getline(ifs, line);
		std::tuple<std::string, std::string, size_t, size_t> head;
		split(line, head);

		const size_t vertexCount(std::get<2>(head));
		const size_t edgeCount(std::get<3>(head));

		auto * graph = new Graph<ull, vertex_data_t>(name, 0, std::numeric_limits<ull>::max() / 2, vertexCount + 1);

		for (size_t i = 0; i < 2; i++) std::getline(ifs, line); // just skip lines

		for (vid_t i = 1; i <= vertexCount; i++)
		{
			graph->addVertex(i);
		}

		for (size_t i = 0; i < edgeCount; i++)
		{
			std::getline(ifs, line);
			std::tuple<std::string, vid_t, vid_t, ull> tpl;
			split(line, tpl);
			graph->addEdgeBi(std::get<1>(tpl), std::get<2>(tpl), std::get<3>(tpl));
		}

		std::cout << "** RoadLoader -> road graph loaded : " << name << std::endl;

		return graph;
	}
	
}