#pragma once

#include "JustListPrioQueue.h"
#include "BinaryHeap.h"
#include "BinomialHeap.h"
#include "FibonacciHeap.h"
#include "BrodalQueue.h"
#include "StrictFibonacciHeap.h"
#include "BoostFibHeap.h"
#include "StlBinaryHeap.h"

namespace uniza_fri {

	class binary_heap {};
	class brodal_queue {};
	class binomial_heap {};
	class boost_fibonacci_heap {};
	class fibonacci_heap {};
	class just_list_queue {};
	class stl_binary_heap {};
	class strict_fibonacci_heap {};

	/*
		Tov�re� na v�robu prioritn�ch frontov r�znych druhov.
		Pre konkr�tne hodnoty parametra T (deklarovan� nad touto triedou)
		je t�to �abl�nov� trieda �pecializovan� tak, aby met�da makeQueue
		vracala in�tanciu konkr�tneho prioritn�ho frontu pod�a parametra.
	 */
	template<class T>
	class Factory
	{
	public:
		template<class N, class E>
		static PriorityQueue<N, E> * makeQueue();
	};

	template<> class Factory<just_list_queue>
	{
	public:
		template<class N, class E>
		static PriorityQueue<N, E> * makeQueue()
		{
			return new JustListPrioQueue<N, E>();
		}
	};

	template<> class Factory<binary_heap>
	{
	public:
		template<class N, class E>
		static PriorityQueue<N, E> * makeQueue()
		{
			return new BinaryHeap<N, E>();
		}
	};

	template<> class Factory<binomial_heap>
	{
	public:
		template<class N, class E>
		static PriorityQueue<N, E> * makeQueue()
		{
			return new BinomialHeap<N, E>();
		}
	};

	template<> class Factory<fibonacci_heap>
	{
	public:
		template<class N, class E>
		static PriorityQueue<N, E> * makeQueue()
		{
			return new FibonacciHeap<N, E>();
		}
	};

	template<> class Factory<brodal_queue>
	{
	public:
		template<class N, class E>
		static PriorityQueue<N, E> * makeQueue()
		{
			return new BrodalQueue<N, E>();
		}
	};

	template<> class Factory<strict_fibonacci_heap>
	{
	public:
		template<class N, class E>
		static PriorityQueue<N, E> * makeQueue()
		{
			return new StrictFibonacciHeap<N, E>();
		}
	};

	template<> class Factory<boost_fibonacci_heap>
	{
	public:
		template<class N, class E>
		static PriorityQueue<N, E> * makeQueue()
		{
			return new BoostFibHeap<N, E>();
		}
	};

	template<> class Factory<stl_binary_heap>
	{
	public:
		template<class N, class E>
		static PriorityQueue<N, E> * makeQueue()
		{
			return new StlBinaryHeap<N, E>();
		}
	};

}