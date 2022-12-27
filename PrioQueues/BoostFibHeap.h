#pragma once

#include <boost/heap/fibonacci_heap.hpp>
#include "PriorityQueue.h"

namespace uniza_fri {

	template<typename N, typename E>
	class BoostFibHeap : public PriorityQueue<N, E>
	{
	private:

		class BoostFibEntry;

		struct Comparator
		{
			bool operator()(const BoostFibEntry * e1, const BoostFibEntry * e2) const;
		};

		typedef typename boost::heap::fibonacci_heap<BoostFibEntry*, boost::heap::compare<Comparator>>::handle_type handle_t;

		class BoostFibEntry : public QueueEntry<N, E>
		{
		public:

			handle_t handle;

			BoostFibEntry(const E & data, N prio);
			virtual ~BoostFibEntry();

			void setPrio(N prio);

		};

	private:

		boost::heap::fibonacci_heap<BoostFibEntry*, boost::heap::compare<Comparator>> fibheap;

	public:

		BoostFibHeap();
		virtual ~BoostFibHeap();

		QueueEntry<N, E> * insert(const E & data, N prio)       override;
		void decreaseKey(QueueEntry<N, E> & entry, N prio)      override;
		PriorityQueue<N, E> * meld(PriorityQueue<N, E> * other) override;
		E deleteMin()                                           override;
		E & findMin()                                           override;
		size_t size()                                           override;
		void clear()                                            override;

	};

	//
	// BoostFibHeap
	//
	template<typename N, typename E>
	BoostFibHeap<N, E>::BoostFibHeap()
	{
		
	}

	template<typename N, typename E>
	BoostFibHeap<N, E>::~BoostFibHeap()
	{
		this->clear();
	}

	template<typename N, typename E>
	QueueEntry<N, E>* BoostFibHeap<N, E>::insert(const E & data, N prio)
	{
		BoostFibEntry * entry(new BoostFibEntry(data, prio));
		
		handle_t tmphandle = this->fibheap.push(entry);
		(*tmphandle)->handle = tmphandle;

		return entry;
	}

	template<typename N, typename E>
	E BoostFibHeap<N, E>::deleteMin()
	{
		if (this->fibheap.empty())
		{
			throw std::out_of_range("Priority queue is empty.");
		}

		BoostFibEntry * entry(this->fibheap.top());
		this->fibheap.pop();
		E ret = entry->getData();
		delete entry;
		return ret;
	}

	template<typename N, typename E>
	E & BoostFibHeap<N, E>::findMin()
	{
		if (this->fibheap.empty())
		{
			throw std::out_of_range("Priority queue is empty.");
		}
		else
		{
			return this->fibheap.top()->getData();
		}
	}

	template<typename N, typename E>
	size_t BoostFibHeap<N, E>::size()
	{
		return this->fibheap.size();
	}

	template<typename N, typename E>
	void BoostFibHeap<N, E>::decreaseKey(QueueEntry<N, E>& entry, N prio)
	{
		PriorityQueue<N, E>::decKeyLogicCheck(entry, prio);

		BoostFibEntry & boostEntry(dynamic_cast<BoostFibEntry &>(entry));
		boostEntry.setPrio(prio);
		this->fibheap.increase(boostEntry.handle);
	}

	template<typename N, typename E>
	PriorityQueue<N, E>* BoostFibHeap<N, E>::meld(PriorityQueue<N, E>* other)
	{
		throw std::exception("Not supported yet.");
	}

	template<typename N, typename E>
	void BoostFibHeap<N, E>::clear()
	{
		for (BoostFibEntry * entry : this->fibheap)
		{
			delete entry;
		}
		
		this->fibheap.clear();
	}

	//
	// Comparator
	//
	template<typename N, typename E>
	bool BoostFibHeap<N, E>::Comparator::operator()(const BoostFibEntry * e1, const BoostFibEntry * e2) const
	{
		return !(e1->getPrio() < e2->getPrio());
	}

	//
	// BoostFibEntry
	//
	template<typename N, typename E>
	BoostFibHeap<N, E>::BoostFibEntry::BoostFibEntry(const E & data, N prio) :
		QueueEntry<N, E>(data, prio)
	{
	}

	template<typename N, typename E>
	BoostFibHeap<N, E>::BoostFibEntry::~BoostFibEntry()
	{
	}

	template<typename N, typename E>
	void BoostFibHeap<N, E>::BoostFibEntry::setPrio(N prio)
	{
		QueueEntry<N, E>::setPrioInternal(prio);
	}

}