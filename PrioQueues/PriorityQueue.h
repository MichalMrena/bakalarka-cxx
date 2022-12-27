#pragma once

namespace uniza_fri {

	/**
		< Template parameters >

		N -> Priority comparable with operator<(). 
			 Smaller value means higher priority.
		E -> element type
	*/

	template<typename N, typename E>
	class QueueEntry
	{
	private:
		E data;
		N prio;

	public:
		QueueEntry(const E & pData, N pPrio);
		virtual ~QueueEntry() = default;
		E & getData();
		N getPrio() const;
		virtual bool operator<(const QueueEntry<N, E> & other);

	protected:
		void setPrioInternal(N newPrio);

	};

	template<typename N, typename E>
	class PriorityQueue
	{
	public:

		virtual ~PriorityQueue() = default;

		/**
			@return Pointer that can later be used to decrease key (increase priority) of inserted element.
			@param  data Element to be inserted.
			@param  prio Priority of inserted element.
		*/
		virtual QueueEntry<N, E> * insert(const E & data, N prio) = 0;
		
		/**
			@param entry   QueueEntry pointer associated with element whose priority it to be increased.
			@param newPrio New value of priority. Must be lower or equal than current priority.
		*/
		virtual void decreaseKey(QueueEntry<N, E> & entry, N newPrio) = 0;

		/**
			Merges this priority queue with other leaving this and other empty.

			@return New priority queue containing all elements from this and other queue.
					This queue is created via operator new so it must be deleted.
			@param  other Priority queue that should be merged with this one.
		*/
		virtual PriorityQueue<N, E> * meld(PriorityQueue<N, E> * other) = 0;

		/**
			Removes element with highest priority from queue.
			Throws std::out_of_range if queue is empty.

			@return Element with highest priority.
		*/
		virtual E deleteMin() = 0;

		/**
			Throws std::out_of_range if queue is empty.

			@return Reference to an element with highest priority.
		*/
		virtual E & findMin() = 0;

		/**
			@return Number of elements in queue.
		*/
		virtual size_t size() = 0;

		/**
			Removes all elements from the queue leaving it with size 0.
		*/
		virtual void clear() = 0;

		/**
			@return true if size() == 0, otherwise false
		*/
		virtual bool isEmpty();

	protected:

		static void decKeyLogicCheck(QueueEntry<N, E> & entry, N newPrio);

	};

	//
	// QueueEntry
	//
	template<typename N, typename E>
	QueueEntry<N, E>::QueueEntry(const E & pdata, N pprio) :
		data(pdata),
		prio(pprio)
	{
	}

	template<typename N, typename E>
	E & QueueEntry<N, E>::getData()
	{
		return this->data;
	}

	template<typename N, typename E>
	N QueueEntry<N, E>::getPrio() const
	{
		return this->prio;
	}

	template<typename N, typename E>
	bool QueueEntry<N, E>::operator<(const QueueEntry<N, E>& other)
	{
		return this->prio < other.prio;
	}
	
	template<typename N, typename E>
	void QueueEntry<N, E>::setPrioInternal(N newPrio)
	{
		this->prio = newPrio;
	}

	//
	// PriorityQueue
	//
	template<typename N, typename E>
	bool PriorityQueue<N, E>::isEmpty()
	{
		return this->size() == 0;
	}

	template<typename N, typename E>
	void PriorityQueue<N, E>::decKeyLogicCheck(QueueEntry<N, E>& entry, N newPrio)
	{
		if (newPrio > entry.getPrio())
		{
			throw std::invalid_argument("New value of the priority must be lower or equal than current value.");
		}
	}

}