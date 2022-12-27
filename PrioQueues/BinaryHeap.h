#pragma once
#include <stdexcept>
#include <vector>

#include "PriorityQueue.h"

namespace uniza_fri {

	template<typename N, typename E>
	class BinaryHeap : public PriorityQueue<N, E>
	{
	private:

		class BinaryHeapItem;

	private:     

		std::vector<BinaryHeapItem*> data;

	public:

		explicit BinaryHeap(size_t initCapacity = 4);
		virtual ~BinaryHeap();

		QueueEntry<N, E> * insert(const E & data, N prio)       override;
		void decreaseKey(QueueEntry<N, E> & entry, N prio)      override;
		PriorityQueue<N, E> * meld(PriorityQueue<N, E> * other) override;
		E deleteMin()                                           override;
		E & findMin()                                           override;
		size_t size()                                           override;
		void clear()                                            override;

	private:

		void ensureCapacity();
		void bubleUp(size_t index);
		void bubleDown(size_t index);

	private:

		class BinaryHeapItem : public QueueEntry<N, E>
		{
		public:

			size_t index;

			BinaryHeapItem(const E & pData, N pPrio, size_t pindex);
			virtual ~BinaryHeapItem() = default;
			void setPrio(N newPrio);

		};

	};

	//
	// BinaryHeapItem
	//
	template<typename N, typename E>
	BinaryHeap<N, E>::BinaryHeapItem::BinaryHeapItem(const E & pData, N pPrio, size_t pindex) :
		QueueEntry<N, E>(pData, pPrio),
		index(pindex)
	{
	}

	template<typename N, typename E>
	void BinaryHeap<N, E>::BinaryHeapItem::setPrio(N newPrio)
	{
		QueueEntry<N, E>::setPrioInternal(newPrio);
	}

	//
	// BinaryHeap
	//
	template<typename N, typename E>
	BinaryHeap<N, E>::BinaryHeap(size_t initCapacity)
	{
		initCapacity = std::max<size_t>(4, initCapacity);
		this->data.reserve(initCapacity);
	}

	template<typename N, typename E>
	BinaryHeap<N, E>::~BinaryHeap()
	{
		for (BinaryHeapItem * item : this->data)
		{
			delete item;
		}
	}

	template<typename N, typename E>
	QueueEntry<N, E> * BinaryHeap<N, E>::insert(const E & data, N prio)
	{
		this->ensureCapacity();

		auto * newItem = new BinaryHeapItem(data, prio, this->data.size());
		this->data.push_back(newItem);
		this->bubleUp(this->data.size() - 1);

		return newItem;
	}

	template<typename N, typename E>
	E BinaryHeap<N, E>::deleteMin()
	{
		if (this->isEmpty())
		{
			throw std::out_of_range("Priority queue is empty.");
		}

		BinaryHeapItem * poped = this->data[0];
		BinaryHeapItem * last  = this->data.back();
		this->data.pop_back();

		E popedData = poped->getData();
		delete poped;
		
		if (!this->data.empty())
		{
			this->data[0] = last;
			last->index = 0;
			this->bubleDown(0);
		}

		return popedData;
	}

	template<typename N, typename E>
	E & BinaryHeap<N, E>::findMin()
	{
		if (this->isEmpty())
		{
			throw std::out_of_range("Priority queue is empty.");
		}
		else
		{
			return this->data[0]->getData();
		}
	}

	template<typename N, typename E>
	size_t BinaryHeap<N, E>::size()
	{
		return this->data.size();
	}

	template<typename N, typename E>
	void BinaryHeap<N, E>::decreaseKey(QueueEntry<N, E> & entry, N prio)
	{
		PriorityQueue<N, E>::decKeyLogicCheck(entry, prio);

		auto & item = dynamic_cast<BinaryHeapItem&>(entry);
		item.setPrio(prio);
		this->bubleUp(item.index);
	}

	template<typename N, typename E>
	PriorityQueue<N, E>* BinaryHeap<N, E>::meld(PriorityQueue<N, E>* other)
	{
		// TODO implement O(n)
		auto * otherBin = dynamic_cast<BinaryHeap<N, E> *>(other);
		
		if (!otherBin)
		{
			throw std::logic_error("Queues must be of same type.");
		}

		for (BinaryHeapItem * oe : otherBin->data)
		{
			this->insert(oe->getData(), oe->getPrio());
		}

		otherBin->clear();

		return this;
	}

	template<typename N, typename E>
	void BinaryHeap<N, E>::clear()
	{
		for (BinaryHeapItem * item : this->data)
		{
			delete item;
		}

		this->data.clear();
	}

	template<typename N, typename E>
	void BinaryHeap<N, E>::ensureCapacity()
	{
		if (this->data.capacity() == this->data.size())
		{
			this->data.reserve(this->data.size() << 1);
		}
	}

	template<typename N, typename E>
	void BinaryHeap<N, E>::bubleUp(size_t index)
	{
		BinaryHeapItem * child = this->data[index];

		while (index > 0)
		{
			size_t parentIndex = (index - 1) >> 1;
			BinaryHeapItem * parent = this->data[parentIndex];

			if (!(*child < *parent)) break;

			this->data[index] = parent;
			parent->index = index;
			index = parentIndex;
		}

		this->data[index] = child;
		child->index = index;
	}

	template<typename N, typename E>
	void BinaryHeap<N, E>::bubleDown(size_t index)
	{
		BinaryHeapItem * item = this->data[index];
		const size_t leafBorder = this->data.size() >> 1;

		while (index < leafBorder)
		{
			size_t childIndex = (index << 1) + 1;

			if (childIndex < this->data.size() - 1 && *(this->data[childIndex + 1]) < *(this->data[childIndex]))
				childIndex++;

			if (*item < *(this->data[childIndex])) break;

			this->data[index] = this->data[childIndex];
			this->data[index]->index = index;
			index = childIndex;
		}

		this->data[index] = item;
		item->index = index;
	}

}

