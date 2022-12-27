#pragma once

#include <vector>
#include <algorithm>
#include "PriorityQueue.h"

namespace uniza_fri {

	template<typename N, typename E>
	class JustListPrioQueue : public PriorityQueue<N, E>
	{
	private:

		class ListEntry;

	private:

		std::vector<ListEntry*> data;

	public:

		JustListPrioQueue();
		explicit JustListPrioQueue(size_t initSize);
		virtual ~JustListPrioQueue();

		QueueEntry<N, E>* insert(const E & data, N prio)       override;
		void decreaseKey(QueueEntry<N, E>& entry, N newPrio)   override;
		PriorityQueue<N, E>* meld(PriorityQueue<N, E>* other)  override;
		E deleteMin()                                          override;
		E & findMin()                                          override;
		size_t size()                                          override;
		void clear()                                           override;

	private:

		size_t findMinIndex();

	private:

		class ListEntry : public QueueEntry<N, E>
		{
		public:

			ListEntry(const E & pdata, const N & pprio);
			virtual ~ListEntry();

			void setPrio(const N & prio);

		};

	};

	//
	// JustListPrioQueue
	//
	template<typename N, typename E>
	JustListPrioQueue<N, E>::JustListPrioQueue() :
		JustListPrioQueue<N, E>(4)
	{
	}

	template<typename N, typename E>
	JustListPrioQueue<N, E>::JustListPrioQueue(size_t initSize)
	{
		this->data.reserve(initSize);
	}

	template<typename N, typename E>
	JustListPrioQueue<N, E>::~JustListPrioQueue()
	{
	}

	template<typename N, typename E>
	QueueEntry<N, E>* JustListPrioQueue<N, E>::insert(const E & data, N prio)
	{
		ListEntry * entry(new ListEntry(data, prio));
		this->data.push_back(entry);
		return entry;
	}

	template<typename N, typename E>
	void JustListPrioQueue<N, E>::decreaseKey(QueueEntry<N, E>& entry, N newPrio)
	{
		PriorityQueue<N, E>::decKeyLogicCheck(entry, newPrio);

		ListEntry & listEntry(dynamic_cast<ListEntry&>(entry));
		listEntry.setPrio(newPrio);
	}

	template<typename N, typename E>
	E JustListPrioQueue<N, E>::deleteMin()
	{
		if (this->data.empty())
		{
			throw std::out_of_range("Priority queue is empty.");
		}

		size_t minIndex(this->findMinIndex());
		E ret = this->data[minIndex]->getData();
		delete this->data[minIndex];

		if (this->data.size() > 1)
		{
			std::iter_swap(this->data.begin() + minIndex, this->data.end() - 1);
		}

		this->data.pop_back();

		return ret;
	}

	template<typename N, typename E>
	E & JustListPrioQueue<N, E>::findMin()
	{
		if (this->data.empty())
		{
			throw std::out_of_range("Priority queue is empty.");
		}
		else
		{
			return this->data[this->findMinIndex()]->getData();
		}
	}

	template<typename N, typename E>
	PriorityQueue<N, E>* JustListPrioQueue<N, E>::meld(PriorityQueue<N, E>* other)
	{
		throw std::exception("Not supported yet.");
	}

	template<typename N, typename E>
	size_t JustListPrioQueue<N, E>::size()
	{
		return this->data.size();
	}

	template<typename N, typename E>
	void JustListPrioQueue<N, E>::clear()
	{
		for (ListEntry * entry : this->data)
		{
			delete entry;
		}

		this->data.clear();
	}

	template<typename N, typename E>
	size_t JustListPrioQueue<N, E>::findMinIndex()
	{
		size_t index(0);
		size_t minIndex(0);

		for (ListEntry * entry : this->data)
		{
			if (*entry < *(this->data[minIndex]))
			{
				minIndex = index;
			}

			index++;
		}

		return minIndex;
	}

	//
	// ListEntry
	//
	template<typename N, typename E>
	JustListPrioQueue<N, E>::ListEntry::ListEntry(const E & pdata, const N & pprio) :
		QueueEntry<N, E>(pdata, pprio)
	{
	}

	template<typename N, typename E>
	JustListPrioQueue<N, E>::ListEntry::~ListEntry()
	{
	}

	template<typename N, typename E>
	void JustListPrioQueue<N, E>::ListEntry::setPrio(const N & prio)
	{
		QueueEntry<N, E>::setPrioInternal(prio);
	}

}