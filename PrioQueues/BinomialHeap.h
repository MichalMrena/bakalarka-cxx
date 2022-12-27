#pragma once
#include <stdexcept>
#include <vector>
#include <cmath>
#include "PriorityQueue.h"

#define ever ;;

namespace uniza_fri {

	template<typename N, typename E>
	class BinomialHeap : public PriorityQueue<N, E>
	{
	private:

		class BinomialTreeNode;
		class BinomialQueueEntry;

	private:

		std::vector<BinomialTreeNode *> roots;
		size_t dataSize;

	public:

		BinomialHeap();
		virtual ~BinomialHeap();

		QueueEntry<N, E> * insert(const E & data, N prio)       override;
		void decreaseKey(QueueEntry<N, E> & entry, N newPrio)   override;
		PriorityQueue<N, E> * meld(PriorityQueue<N, E> * other) override;
		E deleteMin()                                           override;
		E & findMin()                                           override;
		size_t size()                                           override;
		void clear()                                            override;

	private:

		void addItems(BinomialTreeNode * items);
		BinomialTreeNode * findMaxPrioRoot();
		int treesNeeded();
		int treeCount();
		void ensureCapacity();

	private:

		class BinomialTreeNode
		{
		public:

			BinomialQueueEntry * entry;
			int order;

			BinomialTreeNode * parent;
			BinomialTreeNode * next;
			BinomialTreeNode * child;

			BinomialTreeNode(const E & data, N prio);
			~BinomialTreeNode();
			BinomialTreeNode * meld(BinomialTreeNode * other);
			BinomialTreeNode * disconectChildren();
			void addChild(BinomialTreeNode * chld);
			void swapEntries(BinomialTreeNode * other);
			bool operator<(const BinomialTreeNode & other) const;
		};

		class BinomialQueueEntry : public QueueEntry<N, E>
		{
		public:

			BinomialTreeNode * node;

			BinomialQueueEntry(const E & data, N prio, BinomialTreeNode * node);
			virtual ~BinomialQueueEntry();
			void setPrio(N newPrio);

		};

	};

	//
	//  BinomialHeap
	//
	template<typename N, typename E>
	BinomialHeap<N, E>::BinomialHeap() :
		dataSize(0)
	{
		this->roots.resize(4, nullptr);
	}

	template<typename N, typename E>
	BinomialHeap<N, E>::~BinomialHeap()
	{
		this->clear();
	}

	template<typename N, typename E>
	QueueEntry<N, E>* BinomialHeap<N, E>::insert(const E & data, N prio)
	{
		BinomialTreeNode * node = new BinomialTreeNode(data, prio);

		++this->dataSize;
		this->ensureCapacity();
		this->addItems(node);

		return node->entry;
	}

	template<typename N, typename E>
	E BinomialHeap<N, E>::deleteMin()
	{
		if (this->isEmpty())
		{
			throw std::out_of_range("Priority queue is empty.");
		}

		BinomialTreeNode * max = this->findMaxPrioRoot();
		E ret = max->entry->getData();

		this->roots[max->order] = nullptr;
		BinomialTreeNode * children = max->disconectChildren();
		delete max;

		this->addItems(children);

		--this->dataSize;
		return ret;
	}

	template<typename N, typename E>
	E & BinomialHeap<N, E>::findMin()
	{
		if (this->isEmpty())
		{
			throw std::out_of_range("Priority queue is empty.");
		}
		else
		{
			return this->findMaxPrioRoot()->entry->getData();
		}
	}
	
	template<typename N, typename E>
	size_t BinomialHeap<N, E>::size()
	{
		return this->dataSize;
	}

	template<typename N, typename E>
	void BinomialHeap<N, E>::decreaseKey(QueueEntry<N, E> & entry, N newPrio)
	{
		PriorityQueue<N, E>::decKeyLogicCheck(entry, newPrio);

		BinomialQueueEntry * binomialEntry = &(dynamic_cast<BinomialQueueEntry &>(entry));
		BinomialTreeNode * node = binomialEntry->node;
		binomialEntry->setPrio(newPrio);

		while (node->parent && *node < *node->parent)
		{
			node->parent->swapEntries(node);
			node = node->parent;
		}
	}

	template<typename N, typename E>
	void BinomialHeap<N, E>::clear()
	{
		for (BinomialTreeNode * root : this->roots)
		{
			if (root) delete root;
		}

		this->dataSize = 0;
	}

	template<typename N, typename E>
	PriorityQueue<N, E>* BinomialHeap<N, E>::meld(PriorityQueue<N, E>* other)
	{
		BinomialHeap<N, E>* otherBinomial = dynamic_cast<BinomialHeap<N, E>*>(other);
		
		if (!otherBinomial)
		{
			throw std::logic_error("Queues must be of same type.");
		}

		BinomialHeap<N, E> * from;
		BinomialHeap<N, E> * to;

		if (this->treeCount() < otherBinomial->treeCount())
		{
			from = this;
			to = otherBinomial;
		}
		else
		{
			from = otherBinomial;
			to = this;
		}

		to->dataSize += from->dataSize;
		from->dataSize = 0;

		to->ensureCapacity();

		for (size_t i = 0; i < from->roots.size(); i++)
		{
			if (!from->roots[i]) continue;

			from->roots[i]->next = nullptr;
			to->addItems(from->roots[i]);
			from->roots[i] = nullptr;
		}

		return to;
	}

	template<typename N, typename E>
	void BinomialHeap<N, E>::addItems(BinomialTreeNode * items)
	{
		BinomialTreeNode * item = items;
		while (item)
		{
			BinomialTreeNode * nextItem = item->next;

			for (ever)
			{
				if (!this->roots[item->order]) 
				{
					this->roots[item->order] = item;
					break;
				}
				else 
				{
					item = this->roots[item->order]->meld(item);
					this->roots[item->order - 1] = nullptr;
				}
			}

			item = nextItem;
		}
	}

	template<typename N, typename E>
	auto BinomialHeap<N, E>::findMaxPrioRoot() -> BinomialTreeNode *
	{
		BinomialTreeNode * max(nullptr);

		size_t fromI(0);
		const size_t treeNeed = static_cast<size_t>(this->treesNeeded());
		for (size_t i = 0; i <= treeNeed; i++)
		{
			if (this->roots[i])
			{
				max = this->roots[i];
				fromI = i + 1;
				break;
			}
		}

		for (size_t i = fromI; i <= treeNeed; i++)
		{
			if (!this->roots[i]) continue;
			
			if (*this->roots[i] < *max)
			{
				max = this->roots[i];
			}
		}

		return max;
	}

	template<typename N, typename E>
	int BinomialHeap<N, E>::treesNeeded()
	{
		// TODO bitwise
		return static_cast<int>(ceil(log2(this->dataSize)));
	}

	template<typename N, typename E>
	int BinomialHeap<N, E>::treeCount()
	{
		int count(0);
		const int trNeed = this->treesNeeded();
		
		for (int i = 0; i < trNeed; i++)
		{
			if (this->roots[i])
			{
				count++;
			}
		}
		
		return count;
	}

	template<typename N, typename E>
	void BinomialHeap<N, E>::ensureCapacity()
	{
		if (static_cast<size_t>(this->treesNeeded()) >= this->roots.size())
		{
			this->roots.resize(this->dataSize << 1, nullptr);
		}
	}

	//
	//  BinomialTreeNode
	//
	template<typename N, typename E>
	BinomialHeap<N, E>::BinomialTreeNode::BinomialTreeNode(const E & data, N prio) :
		order(0),
		parent(nullptr),
		next(nullptr),
		child(nullptr),
		entry(new BinomialQueueEntry(data, prio, this))
	{
	}

	template<typename N, typename E>
	BinomialHeap<N, E>::BinomialTreeNode::~BinomialTreeNode()
	{
		if (this->child != nullptr)
		{
			BinomialTreeNode * it = this->child->next;
			this->child->next = nullptr;
			while (it != nullptr)
			{
				BinomialTreeNode * nextIt = it->next;
				delete it;
				it = nextIt;
			}
		}

		this->order = -1;
		this->parent = nullptr;
		this->next = nullptr;
		this->child = nullptr;

		delete this->entry;
		this->entry = nullptr;
	}

	template<typename N, typename E>
	auto BinomialHeap<N, E>::BinomialTreeNode::meld(BinomialTreeNode * other) -> BinomialTreeNode *
	{
		if (*this < *other) {
			this->addChild(other);
			return this;
		}
		else 
		{
			other->addChild(this);
			return other;
		}
	}

	template<typename N, typename E>
	void BinomialHeap<N, E>::BinomialTreeNode::addChild(BinomialTreeNode * chld)
	{
		if (!this->child)
		{
			this->child = chld;
			this->child->next = this->child;
		}
		else
		{
			BinomialTreeNode * tmpFirst = this->child->next;
			this->child->next = chld;
			chld->next = tmpFirst;
			this->child = chld;
		}

		chld->parent = this;
		++this->order;
	}

	template<typename N, typename E>
	void BinomialHeap<N, E>::BinomialTreeNode::swapEntries(BinomialTreeNode * other)
	{
		BinomialQueueEntry * tmpEntry = other->entry;
		other->entry = this->entry;
		this->entry = tmpEntry;

		other->entry->node = other;
		this->entry->node = this;
	}

	template<typename N, typename E>
	bool BinomialHeap<N, E>::BinomialTreeNode::operator<(const BinomialTreeNode & other) const
	{
		return *(this->entry) < *(other.entry);
	}

	template<typename N, typename E>
	auto BinomialHeap<N, E>::BinomialTreeNode::disconectChildren() -> BinomialTreeNode *
	{
		if (!this->child)
		{
			return nullptr;
		}

		BinomialTreeNode * ret = this->child->next;
		this->child->next = nullptr;

		BinomialTreeNode * tmpItem = ret;
		while (tmpItem)
		{
			tmpItem->parent = nullptr;
			tmpItem = tmpItem->next;
		}

		this->child = nullptr;
		this->order = 0;

		return ret;
	}

	//
	// BinomialQueueEntry
	//
	template<typename N, typename E>
	BinomialHeap<N, E>::BinomialQueueEntry::BinomialQueueEntry(const E & data, N prio, BinomialTreeNode * pNode) :
		QueueEntry<N, E>(data, prio),
		node(pNode)
	{
	}

	template<typename N, typename E>
	BinomialHeap<N, E>::BinomialQueueEntry::~BinomialQueueEntry()
	{
	}

	template<typename N, typename E>
	void BinomialHeap<N, E>::BinomialQueueEntry::setPrio(N newPrio)
	{
		QueueEntry<N, E>::setPrioInternal(newPrio);
	}
	
}
