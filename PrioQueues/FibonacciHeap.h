#pragma once
#include <cmath>
#include <stdexcept>
#include <vector>
#include "PriorityQueue.h"

#define ever ;;

namespace uniza_fri {

	template<typename N, typename E>
	class FibonacciHeap : public PriorityQueue<N, E>
	{
	private:

		class FibHeapNode;

	private:

		size_t dataSize;
		FibHeapNode * maxPrioItem;

	public:

		FibonacciHeap();
		virtual ~FibonacciHeap();

		QueueEntry<N, E> * insert(const E & data, N prio)       override;
		void decreaseKey(QueueEntry<N, E> & entry, N newPrio)   override;
		PriorityQueue<N, E> * meld(PriorityQueue<N, E> * other) override;
		E deleteMin()                                           override;
		E & findMin()                                           override;
		size_t size()                                           override;
		void clear()                                            override;
		
	private:

		void addOneItem(FibHeapNode * newItem);
		void addMoreItems(FibHeapNode * item);
		void cutChild(FibHeapNode * child);
		void removeMinItem();
		void consolidateRoots();

	private:

		class FibHeapNode : public QueueEntry<N, E>
		{
		public:

			int rank;
			bool marked;

			FibHeapNode * parent;
			FibHeapNode * prev;
			FibHeapNode * next;
			FibHeapNode * child;

			FibHeapNode(const E & data, N prio);
			~FibHeapNode();

			bool isRoot();
			bool isViolating();
			FibHeapNode * merge(FibHeapNode * other);
			FibHeapNode * disconectChildren();
			void cutChild(FibHeapNode * child, FibonacciHeap<N, E> * heap);
			void addChild(FibHeapNode * newChild);
			void removeChild(FibHeapNode * child);
			void removeFromList();
			void setPrio(N newPrio);

		};

	};

	//
	// FibHeapNode
	//
	template<typename N, typename E>
	FibonacciHeap<N, E>::FibHeapNode::FibHeapNode(const E & data, N prio) :
		QueueEntry<N, E>(data, prio),
		rank(0),
		marked(false),
		parent(nullptr),
		prev(nullptr),
		next(nullptr),
		child(nullptr)
	{
	}

	template<typename N, typename E>
	FibonacciHeap<N, E>::FibHeapNode::~FibHeapNode()
	{
		if (this->child != nullptr)
		{
			FibHeapNode * endIt = this->child;
			FibHeapNode * iterated = this->child;

			do {
				FibHeapNode * nextIt = iterated->next;
				delete iterated;
				iterated = nextIt;
			} while (iterated != endIt);
		}

		this->child = nullptr;
		this->prev = nullptr;
		this->next = nullptr;
		this->parent = nullptr;
		this->rank = -1;
	}

	template<typename N, typename E>
	bool FibonacciHeap<N, E>::FibHeapNode::isRoot()
	{
		return this->parent == nullptr;
	}

	template<typename N, typename E>
	bool FibonacciHeap<N, E>::FibHeapNode::isViolating()
	{
		return !this->isRoot() && *this < *this->parent;
	}

	template<typename N, typename E>
	auto FibonacciHeap<N, E>::FibHeapNode::merge(FibHeapNode * other) -> FibHeapNode *
	{
		if (*this < *other)
		{
			other->removeFromList();
			this->addChild(other);
			return this;
		}
		else
		{
			this->removeFromList();
			other->addChild(this);
			return other;
		}
	}

	template<typename N, typename E>
	auto FibonacciHeap<N, E>::FibHeapNode::disconectChildren() -> FibHeapNode *
	{
		FibHeapNode * retChild = this->child;
		if (this->child)
		{
			FibHeapNode * endIt = this->child;
			FibHeapNode * iterated = this->child;
			do {
				iterated->parent = nullptr;
				iterated->marked = false;
				iterated = iterated->next;
			} while (iterated != endIt);

			this->child = nullptr;
			this->rank = 0;
		}

		return retChild;
	}

	template<typename N, typename E>
	void FibonacciHeap<N, E>::FibHeapNode::cutChild(FibHeapNode * cChild, FibonacciHeap<N, E>* heap)
	{
		if (cChild == cChild->next)
		{
			this->child = nullptr;
		}
		else
		{
			if (cChild == this->child)
			{
				this->child = this->child->next;
			}

			cChild->prev->next = cChild->next;
			cChild->next->prev = cChild->prev;
		}

		cChild->parent = nullptr;
		cChild->marked = false;
		--this->rank;
		heap->addOneItem(cChild);

		if (!this->isRoot())
		{
			if (this->marked)
			{
				this->parent->cutChild(this, heap);
			}
			else
			{
				this->marked = true;
			}
		}
	}

	template<typename N, typename E>
	void FibonacciHeap<N, E>::FibHeapNode::addChild(FibHeapNode * newChild)
	{
		if (this->child == nullptr)
		{
			this->child = newChild;
			newChild->next = newChild;
			newChild->prev = newChild;
		}
		else
		{
			newChild->next = this->child->next;
			newChild->prev = this->child;
			this->child->next->prev = newChild;
			this->child->next = newChild;
		}

		newChild->parent = this;
		++this->rank;
	}

	template<typename N, typename E>
	void FibonacciHeap<N, E>::FibHeapNode::removeChild(FibHeapNode * child)
	{
		if (child == child->next)
		{
			this->child = nullptr;
		}
		else
		{
			if (child == this->child)
			{
				this->child = this->child->next;
			}

			child->prev->next = child->next;
			child->next->prev = child->prev;
		}

		child->parent = nullptr;
		--this->rank;

		if (!this->isRoot())
		{
			this->marked = true;
		}
	}

	template<typename N, typename E>
	void FibonacciHeap<N, E>::FibHeapNode::removeFromList()
	{
		this->prev->next = this->next;
		this->next->prev = this->prev;
	}

	template<typename N, typename E>
	void FibonacciHeap<N, E>::FibHeapNode::setPrio(N newPrio)
	{
		QueueEntry<N, E>::setPrioInternal(newPrio);
	}

	//
	// FibonacciIHeap
	//
	template<typename N, typename E>
	FibonacciHeap<N, E>::FibonacciHeap() :
		dataSize(0),
		maxPrioItem(nullptr)
	{
	}

	template<typename N, typename E>
	FibonacciHeap<N, E>::~FibonacciHeap()
	{
		this->clear();
	}

	template<typename N, typename E>
	QueueEntry<N, E>* FibonacciHeap<N, E>::insert(const E & data, N prio)
	{
		FibHeapNode * newItem = new FibHeapNode(data, prio);

		this->addOneItem(newItem);
		++this->dataSize;

		return newItem;
	}

	template<typename N, typename E>
	E FibonacciHeap<N, E>::deleteMin()
	{
		if (this->dataSize == 0)
		{
			throw std::out_of_range("Priority queue is empty.");
		}

		E retData = this->maxPrioItem->getData();
		--this->dataSize;

		this->removeMinItem();

		if (!this->isEmpty())
		{
			this->consolidateRoots();
		}

		return retData;
	}

	template<typename N, typename E>
	E & FibonacciHeap<N, E>::findMin()
	{
		if (this->dataSize == 0)
		{
			throw std::out_of_range("Priority queue is empty.");
		}

		return this->maxPrioItem->getData();
	}

	template<typename N, typename E>
	size_t FibonacciHeap<N, E>::size()
	{
		return this->dataSize;
	}

	template<typename N, typename E>
	void FibonacciHeap<N, E>::decreaseKey(QueueEntry<N, E>& entry, N newPrio)
	{
		PriorityQueue<N, E>::decKeyLogicCheck(entry, newPrio);

		FibHeapNode * node = &dynamic_cast<FibHeapNode&>(entry);
		node->setPrio(newPrio);

		if (node->isViolating()) 
		{
			this->cutChild(node);
		}
		else if (*node < *this->maxPrioItem) 
		{
			this->maxPrioItem = node;
		}
	}
	
	template<typename N, typename E>
	PriorityQueue<N, E>* FibonacciHeap<N, E>::meld(PriorityQueue<N, E>* other)
	{
		FibonacciHeap<N, E> * otherFib = dynamic_cast<FibonacciHeap<N, E> *>(other);

		if (!otherFib)
		{
			throw std::logic_error("Queues must be of same type.");
		}
		else if (this->isEmpty())
		{
			return other;
		}

		this->dataSize += otherFib->dataSize;
		this->addMoreItems(otherFib->maxPrioItem);
		
		otherFib->dataSize = 0;
		otherFib->maxPrioItem = nullptr;

		return this;
	}

	template<typename N, typename E>
	void FibonacciHeap<N, E>::clear()
	{
		if (!this->maxPrioItem)
		{
			return;
		}
		
		this->maxPrioItem->prev->next = nullptr;
		FibHeapNode * it = this->maxPrioItem;

		while (it)
		{
			FibHeapNode * nextIter = it->next;
			delete it;
			it = nextIter;
		}

		this->maxPrioItem = nullptr;
	}

	template<typename N, typename E>
	void FibonacciHeap<N, E>::addOneItem(FibHeapNode * newItem)
	{
		if (!this->maxPrioItem)
		{
			this->maxPrioItem = newItem;
			newItem->next = newItem;
			newItem->prev = newItem;
		}
		else
		{
			newItem->prev = this->maxPrioItem;
			newItem->next = this->maxPrioItem->next;
			this->maxPrioItem->next->prev = newItem;
			this->maxPrioItem->next = newItem;
		}

		if (*newItem < *this->maxPrioItem)
		{
			this->maxPrioItem = newItem;
		}
	}

	template<typename N, typename E>
	void FibonacciHeap<N, E>::addMoreItems(FibHeapNode * item)
	{
		FibHeapNode * leftLeft = this->maxPrioItem;
		FibHeapNode * rightRight = this->maxPrioItem->next;

		FibHeapNode * leftRight = item;
		FibHeapNode * rightLeft = item->prev;

		leftLeft->next = leftRight;
		leftRight->prev = leftLeft;

		rightLeft->next = rightRight;
		rightRight->prev = rightLeft;
	}

	template<typename N, typename E>
	void FibonacciHeap<N, E>::cutChild(FibHeapNode * child)
	{
		FibHeapNode * parent = child->parent;
		const bool goUp = parent->marked;

		parent->removeChild(child);
		child->marked = false;
		this->addOneItem(child);

		if (goUp)
		{
			this->cutChild(parent);
		}
	}

	template<typename N, typename E>
	void FibonacciHeap<N, E>::removeMinItem()
	{
		FibHeapNode * children = this->maxPrioItem->disconectChildren();

		if (this->maxPrioItem == this->maxPrioItem->next)
		{
			delete this->maxPrioItem;
			this->maxPrioItem = children;
		}
		else
		{
			FibHeapNode * oldMaxPrio = this->maxPrioItem;
			
			this->maxPrioItem->removeFromList();
			this->maxPrioItem = this->maxPrioItem->next;
			delete oldMaxPrio;

			if (children)
			{
				this->addMoreItems(children);
			}
		}
	}

	template<typename N, typename E>
	void FibonacciHeap<N, E>::consolidateRoots()
	{
		int maxRank = static_cast<int>(ceil(log(this->dataSize) / log(1.61))) + 1;

		std::vector<FibHeapNode*> aux(maxRank, nullptr);

		FibHeapNode * item = this->maxPrioItem;
		FibHeapNode * last = this->maxPrioItem->prev;
		bool endIt(false);

		for (ever)
		{
			FibHeapNode * nextIt = item->next;

			if (item == last)
			{
				endIt = true;
			}

			for (ever)
			{
				if (!aux[item->rank])
				{
					aux[item->rank] = item;
					break;
				}
				else
				{
					item = aux[item->rank]->merge(item);
					aux[item->rank - 1] = nullptr;
				}
			}

			if (endIt) break;

			item = nextIt;
		}

		this->maxPrioItem = nullptr;

		for (FibHeapNode * itm : aux)
		{
			if (!itm) continue;

			if (!this->maxPrioItem)
			{
				this->maxPrioItem = itm;
			}
			else if (*itm < *this->maxPrioItem)
			{
				this->maxPrioItem = itm;
			}
		}
	}

}
