#pragma once

#include "Stopwatch.h"
#include  "vid_t.h"

namespace uniza_fri {

	/*
		Jednoduchá trieda, ktorá obsahuje informácie o nájdenej ceste.
		
		N -> dátový typ ohodnotenia hrán.
	 */
	template<typename N>
	class PathInfo
	{
	private:

		N length;
		long long timeTaken;
		size_t nodesVisited;

	public:

		PathInfo(const N & len, long long pTimeTaken, size_t pNodesVisited);
		N getLength();
		long long getTimeTaken() const;
		size_t getNodesVisited() const;

	};

	/*
		Táto trieda oba¾uje dáta potrebné pre implementáciu Dijkstrovho algoritmu.
		Jej inštancie sa vkladajú do vrcholov grafu.

		N -> dátový typ, v ktorom sú ohodnotenia hrán grafu.
	 */
	template<typename N>
	class DijkstraData
	{
	private:

		N markT;
		QueueEntry<N, Vertex<N, DijkstraData<N>>*> * entry;

	public:

		DijkstraData();
		~DijkstraData() = default;

		N getT();
		N setT(N t);
		QueueEntry<N, Vertex<N, DijkstraData<N>>*> * getEntry();
		void setEntry(QueueEntry<N, Vertex<N, DijkstraData<N>>*> * e);

	};

	/*
		Táto trieda implementuje Dijkstrov algoritmus.
		Poskytuje dve jeho implementácie, a to základnú verziu popísanú
		v knihe Introduction to algorithms a verziu zvanú Label-set algoritmus.
	 */
	template<typename N>
	class Dijkstra
	{
	private:

		typedef Vertex<N, DijkstraData<N>> vertex_t;
		typedef Edge<N, DijkstraData<N>>     edge_t;

		Graph<N, DijkstraData<N>> * graph;

	public:

		explicit Dijkstra(Graph<N, DijkstraData<N>> * pGraph);
		virtual ~Dijkstra() = default;

		/*
			Nájde najkratšiu cestu medzi danými vrcholmi. 
			Implementuje Label-set algoritmus. V prioritnom fronte je teda
			na zaèiatku iba jeden vrchol.
		 */
		template<typename prio_queue_t>
		PathInfo<N> * pointToPointSearch(vid_t idSrc, vid_t idDst);

		/*
			Nájde najkratšie cesty z daného vrcholu do všetkých vrcholov.
			Implementuje Label-set algoritmus. V prioritnom fronte je teda
			na zaèiatku iba jeden vrchol.
		 */
		template<typename prio_queue_t>
		PathInfo<N> * pointToAllLabelSet(vid_t idSrc);

		/*
			Nájde najkratšie cesty z daného vrcholu do všetkých vrcholov.
			Implementuje základnú verziu Dijkstrovho algoritmu. Pred zaèiatkom
			preh¾adávania grafu sa do prioritného frontu vložia všetky vrcholy.
		 */
		template<typename prio_queue_t>
		PathInfo<N> * pointToAllBasic(vid_t idSrc);

	private:

		void init();

	};

	//
	// PathInfo
	//
	template<typename N>
	PathInfo<N>::PathInfo(const N & len, const long long pTimeTaken, const size_t pNodesVisited) :
		length(len),
		timeTaken(pTimeTaken),
		nodesVisited(pNodesVisited)
	{
	}

	template<typename N>
	N PathInfo<N>::getLength()
	{
		return this->length;
	}

	template<typename N>
	long long PathInfo<N>::getTimeTaken() const
	{
		return this->timeTaken;
	}
	
	template<typename N>
	size_t PathInfo<N>::getNodesVisited() const
	{
		return this->nodesVisited;
	}

	//
	// DijkstraData
	//
	template<typename N>
	DijkstraData<N>::DijkstraData() :
		entry(nullptr)
	{
	}

	template<typename N>
	N DijkstraData<N>::getT()
	{
		return this->markT;
	}

	template<typename N>
	N DijkstraData<N>::setT(N t)
	{
		return this->markT = t;
	}

	template<typename N>
	QueueEntry<N, Vertex<N, DijkstraData<N>>*>* DijkstraData<N>::getEntry()
	{
		return this->entry;
	}

	template<typename N>
	void DijkstraData<N>::setEntry(QueueEntry<N, Vertex<N, DijkstraData<N>>*>* e)
	{
		this->entry = e;
	}

	//
	// Dijkstra
	//
	template<typename N>
	Dijkstra<N>::Dijkstra(Graph<N, DijkstraData<N>>* pGraph) :
		graph(pGraph)
	{
	}

	template<typename N>
	void Dijkstra<N>::init()
	{
		for (vertex_t * vertex : *(this->graph))
		{
			vertex->getData()->setT(this->graph->getMaxDist());
			vertex->getData()->setEntry(nullptr);
		}
	}

	template<typename N>
	template<typename prio_queue_t>
	PathInfo<N>* Dijkstra<N>::pointToPointSearch(vid_t idSrc, vid_t idDst)
	{
		vertex_t * src = this->graph->getVertexExcept(idSrc);
		vertex_t * dst = this->graph->getVertexExcept(idDst);

		this->init();

		Stopwatch stopwatch;
		PriorityQueue<N, vertex_t*> * queue = Factory<prio_queue_t>::template makeQueue<N, vertex_t *>();

		src->getData()->setT(this->graph->getZeroDist());
		src->getData()->setEntry(queue->insert(src, this->graph->getZeroDist()));

		size_t visited(0);

		while (!queue->isEmpty())
		{
			vertex_t * poped = queue->deleteMin();

			if (poped == dst) break;

			for (edge_t * edge : poped->edges())
			{
				N newCost = poped->getData()->getT() + edge->cost();
				vertex_t * target = edge->target(poped);

				if (newCost < target->getData()->getT())
				{
					target->getData()->setT(newCost);
					QueueEntry<N, vertex_t*> * entry = target->getData()->getEntry();
					if (entry)
					{
						queue->decreaseKey(*entry, newCost);
					}
					else
					{
						target->getData()->setEntry(queue->insert(target, newCost));
						++visited;
					}
				}
			}
		}

		delete queue;

		long long timeTaken = stopwatch.getTime();
		N pathLenght = dst->getData()->getT();

		if (dst->getData()->getT() != this->graph->getMaxDist())
		{
			return new PathInfo<N>(pathLenght, timeTaken, visited);
		}

		return nullptr;
	}

	template<typename N>
	template<typename prio_queue_t>
	PathInfo<N>* Dijkstra<N>::pointToAllLabelSet(vid_t idSrc)
	{
		vertex_t * src = this->graph->getVertexExcept(idSrc);

		this->init();

		Stopwatch stopwatch;
		PriorityQueue<N, vertex_t*> * queue = Factory<prio_queue_t>::template makeQueue<N, vertex_t *>();

		src->getData()->setT(this->graph->getZeroDist());
		src->getData()->setEntry(queue->insert(src, this->graph->getZeroDist()));

		size_t visited(1);

		while (!queue->isEmpty())
		{
			vertex_t * poped = queue->deleteMin();

			for (edge_t * edge : poped->edges())
			{
				N newCost = poped->getData()->getT() + edge->cost();
				vertex_t * target = edge->target(poped);

				if (newCost < target->getData()->getT())
				{
					target->getData()->setT(newCost);
					QueueEntry<N, vertex_t*> * entry = target->getData()->getEntry();
					if (entry)
					{
						queue->decreaseKey(*entry, newCost);
					}
					else
					{
						target->getData()->setEntry(queue->insert(target, newCost));
						++visited;
					}
				}
			}
		}

		delete queue;
		long long timeTaken = stopwatch.getTime();
		return new PathInfo<N>(this->graph->getZeroDist(), timeTaken, visited);
	}

	template<typename N>
	template<typename prio_queue_t>
	PathInfo<N>* Dijkstra<N>::pointToAllBasic(vid_t idSrc)
	{
		vertex_t * src = this->graph->getVertexExcept(idSrc);

		this->init();

		Stopwatch stopwatch;
		PriorityQueue<N, vertex_t*> * queue = Factory<prio_queue_t>::template makeQueue<N, vertex_t *>();

		for (vertex_t * vertex : *(this->graph))
		{
			vertex->getData()->setEntry(queue->insert(vertex, this->graph->getMaxDist()));
		}

		src->getData()->setT(this->graph->getZeroDist());
		queue->decreaseKey(*src->getData()->getEntry(), this->graph->getZeroDist());

		size_t visited(1);

		while (!queue->isEmpty())
		{
			vertex_t * poped = queue->deleteMin();

			if (visited == this->graph->getVertexCount()) break;

			for (edge_t * edge : poped->edges())
			{
				N newCost = poped->getData()->getT() + edge->cost();
				vertex_t * target = edge->target(poped);

				if (newCost < target->getData()->getT())
				{
					if (target->getData()->getT() == this->graph->getMaxDist())
					{
						++visited;
					}

					target->getData()->setT(newCost);
					queue->decreaseKey(*target->getData()->getEntry(), newCost);
				}
			}
		}

		delete queue;
		long long timeTaken = stopwatch.getTime();
		return new PathInfo<N>(this->graph->getZeroDist(), timeTaken, visited);
	}

}