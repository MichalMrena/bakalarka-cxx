#pragma once

#include <queue>
#include <vector>
#include "PriorityQueue.h"

namespace uniza_fri {

	template<typename N, typename E>
	class StlBinaryHeap : public PriorityQueue<N, E>
	{
	private:

		struct Comparator
		{
			bool operator()(const QueueEntry<N, E> * e1, const QueueEntry<N, E> * e2) const;
		};

	private:

		std::priority_queue<QueueEntry<N, E>*, std::vector<QueueEntry<N, E>*>, Comparator> prioQueue;

	public:

		StlBinaryHeap();
		virtual ~StlBinaryHeap();

		QueueEntry<N, E>* insert(const E & data, N prio)      override;
		void decreaseKey(QueueEntry<N, E>& entry, N newPrio)  override;
		PriorityQueue<N, E>* meld(PriorityQueue<N, E>* other) override;
		E deleteMin()                                         override;
		E & findMin()                                         override;
		size_t size()                                         override;
		void clear()                                          override;

	};

	//
	// Comparator
	//
	template<typename N, typename E>
	bool StlBinaryHeap<N, E>::Comparator::operator()(const QueueEntry<N, E> * e1, const QueueEntry<N, E> * e2) const
	{
		return !(e1->getPrio() < e2->getPrio());
	}

	//
	// StlBinaryHeap
	//
	template<typename N, typename E>
	StlBinaryHeap<N, E>::StlBinaryHeap()
	{
	}

	template<typename N, typename E>
	StlBinaryHeap<N, E>::~StlBinaryHeap()
	{
	}

	template<typename N, typename E>
	QueueEntry<N, E>* StlBinaryHeap<N, E>::insert(const E & data, N prio)
	{
		QueueEntry<N, E> * entry = new QueueEntry<N, E>(data, prio);
		this->prioQueue.push(entry);
		return entry;
	}

	template<typename N, typename E>
	void StlBinaryHeap<N, E>::decreaseKey(QueueEntry<N, E>& entry, N newPrio)
	{
		throw new std::exception("Unsupported operation.");
	}

	template<typename N, typename E>
	PriorityQueue<N, E>* StlBinaryHeap<N, E>::meld(PriorityQueue<N, E>* other)
	{
		throw new std::exception("Unsupported operation.");
	}

	template<typename N, typename E>
	E StlBinaryHeap<N, E>::deleteMin()
	{
		if (this->isEmpty())
		{
			throw std::out_of_range("Priority queue is empty.");
		}

		QueueEntry<N, E> * entry = this->prioQueue.top();
		E ret = entry->getData();
		this->prioQueue.pop();
		delete entry;
		return ret;
	}

	template<typename N, typename E>
	E & StlBinaryHeap<N, E>::findMin()
	{
		if (this->isEmpty())
		{
			throw std::out_of_range("Priority queue is empty.");
		}
		else
		{
			return this->prioQueue.top()->getData();
		}
	}

	template<typename N, typename E>
	size_t StlBinaryHeap<N, E>::size()
	{
		return this->prioQueue.size();
	}

	template<typename N, typename E>
	void StlBinaryHeap<N, E>::clear()
	{
		while (!this->prioQueue.empty())
		{
			delete this->prioQueue.top();
			this->prioQueue.pop();
		}
	}

}