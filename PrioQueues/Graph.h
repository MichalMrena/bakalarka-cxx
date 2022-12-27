#pragma once

#include <vector>
#include <stdexcept>

#include "PrioQueueFactory.h"
#include "vid_t.h"

namespace uniza_fri {
	
	template<typename N, typename vertex_data_t>
	class Edge;

	/*
		Vrchol grafu. VlastnÌ d·ta, ktorÈ sa do neho vloûia pri vytorenÌ.
		M· unik·tny identifik·tor a zoznam hr·n, ktorÈ tvoria jeho dopredn˙ hviezdu.
		Dopredn˙ hviezdu a d·ta vie sprÌstupniù.
	 */
	template<typename N, typename vertex_data_t>
	class Vertex
	{
	private:

		vid_t id;
		std::vector<Edge<N, vertex_data_t> *> forwardStar;
		vertex_data_t data;

	public:

		Vertex(vid_t pId);
		~Vertex();
		void addEdge(Edge<N, vertex_data_t> * e);
		const std::vector<Edge<N, vertex_data_t> *> & edges();
		vid_t getId() const;
		vertex_data_t * getData();

	};

	/*
		Abstraktn˝ predok hrany grafu. M· svoje ohodnotenie.
		NeskÙr je öpecializovan· ako jednosmern· alebo obojsmern·.
	 */
	template<typename N, typename vertex_data_t>
	class Edge {
	private:

		N edgeCost;
		bool canDelete;

	public:

		explicit Edge(const N & pcost);
		virtual ~Edge() = default;
		virtual Vertex<N, vertex_data_t> * target(Vertex<N, vertex_data_t> * src) = 0;
		virtual const N & cost();
		void markAsDeleteble();
		bool canBeDeleted() const;

	};

	/*
		Orientovan· (jednosmern·) hrana. M· ukazovateæ na svoj cieæ, 
		ktor˝ vie sprÌstupniù.
	 */
	template<typename N, typename vertex_data_t>
	class EdgeOriented : public Edge<N, vertex_data_t> {
	private:

		Vertex<N, vertex_data_t> * edgeTarget;

	public:

		EdgeOriented(Vertex<N, vertex_data_t> * ptarget, const N & pcost);
		virtual ~EdgeOriented();
		Vertex<N, vertex_data_t> * target(Vertex<N, vertex_data_t> * src) override;

	};

	/*
		Obojsmern· hrana. M· ukazovateæe na dva vrcholy, ktorÈ sp·ja.
		Vie sprÌstupniù cieæ hrany "podæa toho, z ktorÈho vrcholu sa p˝tame".
	 */
	template<typename N, typename vertex_data_t>
	class EdgeBi : public Edge<N, vertex_data_t> {
	private:

		Vertex<N, vertex_data_t> * v1;
		Vertex<N, vertex_data_t> * v2;

	public:

		EdgeBi(Vertex<N, vertex_data_t> * pv1, Vertex<N, vertex_data_t> * pv2, const N & cost);
		virtual ~EdgeBi() = default;
		Vertex<N, vertex_data_t> * target(Vertex<N, vertex_data_t> * src) override;

	};

	/*
		Trieda, ktor· reprezentuje graf. Vie sprÌstupniù jednotlivÈ vrcholy
		podæa ich identifik·tora.

		N -> d·tov˝ typ ohodnotenia hr·n.
		vertex_data_t -> typ d·t, ktorÈ sa vkladaj˙ do vrcholov.
	 */
	template<typename N, typename vertex_data_t>
	class Graph
	{
	public:

		/*
			Iter·tor grafu, sprÌstupÚuje jeho vrcholy kvÙli inicializ·cii.
		 */
		class GraphIterator
		{
		private:

			std::vector<Vertex<N, vertex_data_t> *> * itVertices;
			size_t pos;

		public:

			GraphIterator(size_t pPos, std::vector<Vertex<N, vertex_data_t> *> * pVertices);
			bool operator!=(const GraphIterator & other);
			Vertex<N, vertex_data_t> * operator*();
			const GraphIterator & operator++();

		};

	private:

		std::string name;
		N zeroDist;
		N maxDist;
		std::vector<Vertex<N, vertex_data_t> *> vertices;
		size_t vertexCount;

	public:

		Graph(std::string pname, const N & zeroDistance, const N & maxDistance, size_t maxVertexCount = 100);
		~Graph();

		const std::string & getName() const;

		N getZeroDist();
		N getMaxDist();
		Vertex<N, vertex_data_t> * getVertexExcept(const vid_t & vertexID);
		size_t getVertexCount() const;

		void addVertex(const vid_t & vertexID);
		void addEdgeBi(const vid_t & vid1, const vid_t & vid2, const N & cost);
		void addEdgeOriented(const vid_t & vidSrc, const vid_t & vidDst, const N & cost);

		const std::vector<Vertex<N, vertex_data_t> *> & getVertices();

		typename Graph<N, vertex_data_t>::GraphIterator begin();
		typename Graph<N, vertex_data_t>::GraphIterator end();

	private:

		bool vertexExists(const vid_t & vertexID);

	};
	
	//
	// Graph
	//
	template<typename N, typename vertex_data_t>
	Graph<N, vertex_data_t>::Graph(std::string pname, const N & zeroDistance, const N & maxDistance, const size_t maxVertexCount) :
		name(std::move(pname)),
		zeroDist(zeroDistance),
		maxDist(maxDistance),
		vertexCount(0)
	{
		this->vertices.resize(maxVertexCount, nullptr);
	}

	template<typename N, typename vertex_data_t>
	Graph<N, vertex_data_t>::~Graph()
	{
		for (Vertex<N, vertex_data_t> * vptr : this->vertices)
		{
			delete vptr;
		}
	}

	template<typename N, typename vertex_data_t>
	const std::string & Graph<N, vertex_data_t>::getName() const
	{
		return this->name;
	}

	template<typename N, typename vertex_data_t>
	N Graph<N, vertex_data_t>::getZeroDist()
	{
		return this->zeroDist;
	}

	template<typename N, typename vertex_data_t>
	N Graph<N, vertex_data_t>::getMaxDist()
	{
		return this->maxDist;
	}

	template<typename N, typename vertex_data_t>
	void Graph<N, vertex_data_t>::addVertex(const vid_t & vertexID)
	{
		if (this->vertexExists(vertexID))
		{
			throw std::logic_error("Vertex alredy in graph.");
		}

		const size_t verticesSize = this->vertices.size();
		if (vertexID >= verticesSize)
		{
			this->vertices.resize(vertexID + 1, nullptr);
		}

		this->vertices[vertexID] = new Vertex<N, vertex_data_t>(vertexID);
		++this->vertexCount;
	}

	template<typename N, typename vertex_data_t>
	void Graph<N, vertex_data_t>::addEdgeBi(const vid_t & vid1, const vid_t & vid2, const N & cost)
	{
		Vertex<N, vertex_data_t> * v1 = this->getVertexExcept(vid1);
		Vertex<N, vertex_data_t> * v2 = this->getVertexExcept(vid2);
		Edge<N, vertex_data_t> * edge = new EdgeBi<N, vertex_data_t>(v1, v2, cost);
		v1->addEdge(edge);
		v2->addEdge(edge);
	}

	template<typename N, typename vertex_data_t>
	void Graph<N, vertex_data_t>::addEdgeOriented(const vid_t & vidSrc, const vid_t & vidDst, const N & cost)
	{
		Vertex<N, vertex_data_t> * src = this->getVertexExcept(vidSrc);
		Vertex<N, vertex_data_t> * dst = this->getVertexExcept(vidDst);
		Edge<N, vertex_data_t> * edge = new EdgeOriented<N, vertex_data_t>(dst, cost);
		src->addEdge(edge);
	}

	template<typename N, typename vertex_data_t>
	const std::vector<Vertex<N, vertex_data_t>*>& Graph<N, vertex_data_t>::getVertices()
	{
		return this->vertices;
	}

	template<typename N, typename vertex_data_t>
	auto Graph<N, vertex_data_t>::begin() -> typename Graph<N, vertex_data_t>::GraphIterator
	{
		size_t initPos(0);

		while (initPos <= this->vertices.size() && !(this->vertices[initPos]))
		{
			++initPos;
		}

		return GraphIterator(initPos, &(this->vertices));
	}

	template<typename N, typename vertex_data_t>
	auto Graph<N, vertex_data_t>::end() -> typename Graph<N, vertex_data_t>::GraphIterator
	{
		return GraphIterator(this->vertices.size(), &(this->vertices));
	}

	template<typename N, typename vertex_data_t>
	bool Graph<N, vertex_data_t>::vertexExists(const vid_t & vertexID)
	{
		return vertexID < this->vertices.size() && this->vertices[vertexID];
	}

	template<typename N, typename vertex_data_t>
	Vertex<N, vertex_data_t> * Graph<N, vertex_data_t>::getVertexExcept(const vid_t & vertexID)
	{
		if (!this->vertexExists(vertexID))
		{
			throw std::logic_error("No such vertex.");
		}

		return this->vertices[vertexID];
	}

	template<typename N, typename vertex_data_t>
	size_t Graph<N, vertex_data_t>::getVertexCount() const
	{
		return this->vertexCount;
	}

	//
	// Edge
	//
	template<typename N, typename vertex_data_t>
	Edge<N, vertex_data_t>::Edge(const N & pcost) :
		edgeCost(pcost),
		canDelete(false)
	{
	}
	
	template<typename N, typename vertex_data_t>
	const N & Edge<N, vertex_data_t>::cost()
	{
		return this->edgeCost;
	}

	template<typename N, typename vertex_data_t>
	void Edge<N, vertex_data_t>::markAsDeleteble()
	{
		this->canDelete = true;
	}

	template<typename N, typename vertex_data_t>
	bool Edge<N, vertex_data_t>::canBeDeleted() const
	{
		return this->canDelete;
	}

	//
	// EdgeOriented
	//
	template<typename N, typename vertex_data_t>
	EdgeOriented<N, vertex_data_t>::EdgeOriented(Vertex<N, vertex_data_t> * ptarget, const N & pcost) :
		Edge(pcost),
		edgeTarget(ptarget)
	{
	}

	template<typename N, typename vertex_data_t>
	EdgeOriented<N, vertex_data_t>::~EdgeOriented()
	{
	}

	template<typename N, typename vertex_data_t>
	Vertex<N, vertex_data_t> * EdgeOriented<N, vertex_data_t>::target(Vertex<N, vertex_data_t> * src)
	{
		return this->target;
	}

	//
	// EdgeBi
	//
	template<typename N, typename vertex_data_t>
	EdgeBi<N, vertex_data_t>::EdgeBi(Vertex<N, vertex_data_t> * pv1, Vertex<N, vertex_data_t> * pv2, const N & cost) :
		Edge<N, vertex_data_t>(cost),
		v1(pv1),
		v2(pv2)
	{
	}

	template<typename N, typename vertex_data_t>
	Vertex<N, vertex_data_t> * EdgeBi<N, vertex_data_t>::target(Vertex<N, vertex_data_t> * src)
	{
		return src == this->v1 ? this->v2 : this->v1;
	}

	//
	// Vertex
	//
	template<typename N, typename vertex_data_t>
	Vertex<N, vertex_data_t>::Vertex(vid_t pId) :
		id(pId)
	{
	}

	template<typename N, typename vertex_data_t>
	Vertex<N, vertex_data_t>::~Vertex()
	{
		for (Edge<N, vertex_data_t> * edge : this->forwardStar)
		{
			if (edge->canBeDeleted())
			{
				delete edge;
			}
			else
			{
				edge->markAsDeleteble();
			}
		}
	}

	template<typename N, typename vertex_data_t>
	void Vertex<N, vertex_data_t>::addEdge(Edge<N, vertex_data_t> * e)
	{
		this->forwardStar.push_back(e);
	}

	template<typename N, typename vertex_data_t>
	const std::vector<Edge<N, vertex_data_t>*>& Vertex<N, vertex_data_t>::edges()
	{
		return this->forwardStar;
	}

	template<typename N, typename vertex_data_t>
	vid_t Vertex<N, vertex_data_t>::getId() const
	{
		return this->id;
	}

	template<typename N, typename vertex_data_t>
	vertex_data_t * Vertex<N, vertex_data_t>::getData()
	{
		return &(this->data);
	}

	//
	// GraphIterator
	//
	template<typename N, typename vertex_data_t>
	Graph<N, vertex_data_t>::GraphIterator::GraphIterator(size_t pPos, std::vector<Vertex<N, vertex_data_t> *> * pVertices) :
		itVertices(pVertices),
		pos(pPos)
	{
	}

	template<typename N, typename vertex_data_t>
	bool Graph<N, vertex_data_t>::GraphIterator::operator!=(const GraphIterator & other)
	{
		return this->pos != other.pos;
	}

	template<typename N, typename vertex_data_t>
	Vertex<N, vertex_data_t>* Graph<N, vertex_data_t>::GraphIterator::operator*()
	{
		return (*this->itVertices)[this->pos];
	}

	template<typename N, typename vertex_data_t>
	auto Graph<N, vertex_data_t>::GraphIterator::operator++() -> const GraphIterator &
	{
		++this->pos;

		if (this->pos < this->itVertices->size() && !((*this->itVertices)[this->pos]))
		{
			while (this->pos <= this->itVertices->size() && !((*this->itVertices)[this->pos]))
			{
				++this->pos;
			}
		}

		return *this;
	}

}