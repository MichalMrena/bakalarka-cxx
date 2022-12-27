#include <iostream>
#include <fstream>
#include <cstdlib>
//#include <vld.h>

#include "Roads.h"
#include "PrioQueueFactory.h"
#include "Dijkstra.h"
#include "RNG.h"
#include "ValueStabilizer.h"

using namespace uniza_fri;

struct Integer
{
	int val;
	explicit Integer(const int pVal) : val(pVal) {};
};

/*
	Funkcia na overenia spr·vneho fungovania prioritnÈho frontu.
	VloûÌ do frontu TestCaseCount n·hodn˝ch ËÌsel, n·sledne ich vyberie a kontroluje Ëi s˙ usporiadanÈ.
	N·sledne znovu vloûÌ TestCaseCount n·hodn˝ch ËÌsel. Potom pri kaûdom vykon· decreaseKey.
	N·sledne znovu vöetky vyberie a skontroluje usporiadanie.
	Tak˝to postup nemusÌ nutne odaliù vöetky chyby, ale pri veækom TestCaseCount preverÌ veæa rÙznych prÌpadov.
 */
template<typename prio_queue_t, size_t TestCaseCount = 1000000, long Seed = 144>
bool testCorrectness(const std::string & prioQueueName)
{
	try {
		std::cout << "Testing: " << prioQueueName << std::endl;

		std::random_device rd;
		std::mt19937 rng(rd());
		rng.seed(Seed);
		const std::uniform_int_distribution<int> uniPush(0, std::numeric_limits<int>::max());

		std::unique_ptr<PriorityQueue<int, Integer>> queue(Factory<prio_queue_t>::template makeQueue<int, Integer>());

		// Push test
		for (size_t i = 0; i < TestCaseCount; i++)
		{
			const int d = uniPush(rng);
			Integer ig(d);
			queue->insert(ig, d);
		}
		std::cout << " push -> successful" << std::endl;

		// Pop test
		Integer pp = queue->deleteMin();
		int prevpoped = pp.val;
		for (size_t i = 1; i < TestCaseCount; i++)
		{
			pp = queue->deleteMin();
			const int poped = pp.val;
			if (poped < prevpoped)
			{
				std::cout << "# nespravne usporiadanie" << std::endl;
				return false;
			}
			prevpoped = poped;
		}
		std::cout << " pop -> successful" << std::endl;

		// Decrease test
		const std::uniform_int_distribution<int> uniDecrease(0, std::numeric_limits<int>::max());

		std::vector<QueueEntry<int, Integer>*> entries;
		for (size_t i = 0; i < TestCaseCount; i++)
		{
			const int prio = uniDecrease(rng);
			Integer in(prio);
			QueueEntry<int, Integer> * entry = queue->insert(in, prio);
			entries.push_back(entry);
		}

		std::cout << " decrease -> push successful" << std::endl;

		for (size_t i = 0; i < TestCaseCount; i++)
		{
			QueueEntry<int, Integer> * entry = entries[i];
			const int newPrio = uniDecrease(rng) % entry->getPrio();
			queue->decreaseKey(*entry, newPrio);
			entry->getData().val = newPrio;
		}

		std::cout << " decrease -> decrease successful" << std::endl;

		pp = queue->deleteMin();
		prevpoped = pp.val;
		for (size_t i = 1; i < TestCaseCount; i++)
		{
			pp = queue->deleteMin();
			const int poped = pp.val;
			if (poped < prevpoped)
			{
				std::cout << "# nespravne usporiadanie po decrease key" << std::endl;
				return false;
			}
		}

		std::cout << " decrease -> pop after successful" << std::endl;
		std::cout << " ---> test successful <---" << std::endl << std::endl;
	
		return true;
	}
	catch (std::exception & e)
	{
		std::cout << " -> test failed." << std::endl;
		std::cout << e.what()           << std::endl;
		return false;
	}
}

/*
	Experiment, pri ktorom sa hæad· cesta z n·hodnÈho vrcholu do vöetk˝ch vrcholov v grafoch
	rÙznych veækostÌ. Po kaûdom n·jdenÌ cesty sa uloûÌ Ëas, ktor˝ bol potrebn˝ na n·jdenie cesty.
	Z Ëasov sa vypoËÌtava priemer (viz. trieda ValueStabilizer). KeÔ je priemer stabilizovan˝
	v˝sledok sa uloûÌ do s˙boru a prejde sa na ÔalöÌ graf.
	Na hæadanie sa pouûije Label-Set algoritmus (viz. trieda Dijkstra) a prioritn˝ front podæa parametra
	öablÛny (viz. trieda PrioQueueFactory).
 */
template<typename prio_queue_t>
void labelSetExperiment(const std::string & pqname)
{
	std::vector<std::string> graphNames = {
		"USA-road-d.BAY",
		"USA-road-d.COL",
		"USA-road-d.FLA",
		"USA-road-d.NW",
		"USA-road-d.NE",
		"USA-road-d.CAL",
		"USA-road-d.LKS",
		"USA-road-d.E",
		"USA-road-d.W"
	};

	std::ofstream ofstr("results/" + pqname + "_result.csv");
	RNG randomGenerator(144);

	for (std::string & graphname : graphNames)
	{
		Graph<ull, DijkstraData<ull>> * graph = Roads::load<DijkstraData<ull>>(graphname);
		Dijkstra<ull> pathfinder(graph);
		ValueStabilizer<100> stabilizer;
		size_t replicationCount(0);

		while (!stabilizer.isStable())
		{
			vid_t randomSrc(randomGenerator.nextUniqueSizeT(1, graph->getVertexCount()));
			PathInfo<ull> * info(pathfinder.pointToAllLabelSet<prio_queue_t>(randomSrc));
			stabilizer.addValue(info->getTimeTaken());
			delete info;
			++replicationCount;
		}

		std::cout << "replications    : " << replicationCount				  << std::endl;
		std::cout << "avg search time : " << stabilizer.getLastAvg() << " ms" << std::endl;

		ofstr << graph->getVertexCount() << ";" << stabilizer.getLastAvg() << std::endl;

		delete graph;
	}

	std::cout << "## " << pqname << "_result.dat created" << std::endl;
}

/*
	Experiment, pri ktorom sa hæad· cesta z n·hodnÈho vrcholu do vöetk˝ch vrcholov v grafoch
	rÙznych veækostÌ. Po kaûdom n·jdenÌ cesty sa uloûÌ Ëas, ktor˝ bol potrebn˝ na n·jdenie cesty.
	Z Ëasov sa vypoËÌtava priemer (viz. trieda ValueStabilizer). KeÔ je priemer stabilizovan˝
	v˝sledok sa uloûÌ do s˙boru a prejde sa na ÔalöÌ graf.
	Na hæadanie sa pouûije z·kladn˝ Dijkstrov algoritmus (viz. trieda Dijkstra) a prioritn˝ front podæa parametra
	öablÛny (viz. trieda PrioQueueFactory).
 */
template<typename prio_queue_t>
void basicDijkstraExperiment(const std::string & pqname)
{
	std::vector<std::string> graphNames = {
		"USA-road-d.BAY",
		"USA-road-d.COL",
		"USA-road-d.FLA",
		"USA-road-d.NW",
		"USA-road-d.NE",
		"USA-road-d.CAL",
		"USA-road-d.LKS",
		"USA-road-d.E",
		"USA-road-d.W"
	};

	std::ofstream ofstr("results/" + pqname + "_result.csv");
	RNG randomGenerator(144);

	for (std::string & graphname : graphNames)
	{
		Graph<ull, DijkstraData<ull>> * graph = Roads::load<DijkstraData<ull>>(graphname);
		Dijkstra<ull> pathfinder(graph);
		ValueStabilizer<100> stabilizer;
		size_t replicationCount(0);

		while (!stabilizer.isStable())
		{
			vid_t randomSrc(randomGenerator.nextUniqueSizeT(1, graph->getVertexCount()));
			PathInfo<ull> * info(pathfinder.pointToAllBasic<prio_queue_t>(randomSrc));
			stabilizer.addValue(info->getTimeTaken());
			delete info;
			++replicationCount;
		}

		std::cout << "replications    : " << replicationCount				  << std::endl;
		std::cout << "avg search time : " << stabilizer.getLastAvg() << " ms" << std::endl;

		ofstr << graph->getVertexCount() << ";" << stabilizer.getLastAvg() << std::endl;

		delete graph;
	}

	std::cout << "## " << pqname << "_result.dat created" << std::endl;
}

int main()
{
	testCorrectness<binary_heap>("BinaryHeap");
	testCorrectness<binomial_heap>("BinomialHeap");
	testCorrectness<fibonacci_heap>("FibonacciHeap");
	testCorrectness<brodal_queue>("BrodalQueue");
	testCorrectness<strict_fibonacci_heap>("StrictFibonacci");
	testCorrectness<boost_fibonacci_heap>("BoostFibonacciHeap");

	labelSetExperiment<binary_heap>("BinaryHeap");
	labelSetExperiment<fibonacci_heap>("FibonacciHeap");
	labelSetExperiment<brodal_queue>("BrodalQueue");
	labelSetExperiment<strict_fibonacci_heap>("StrictFibonacciHeap");
	labelSetExperiment<boost_fibonacci_heap>("BoostFibonacciHeap");

	basicDijkstraExperiment<binary_heap>("BinaryHeap");
	basicDijkstraExperiment<fibonacci_heap>("FibonacciHeap");
	basicDijkstraExperiment<brodal_queue>("BrodalQueue");
	basicDijkstraExperiment<strict_fibonacci_heap>("StrictFibonacciHeap");
	basicDijkstraExperiment<boost_fibonacci_heap>("BoostFibonacciHeap");

	std::cout << std::endl << " <!#$@		PRESS ENY KEY		@$#!>" << std::endl;
	getchar();
	return 0;
}