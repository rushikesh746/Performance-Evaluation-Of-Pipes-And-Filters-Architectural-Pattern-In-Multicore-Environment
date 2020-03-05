/**
 * pipesandfilters.h
 * Implemented by Blueprint Technologies, Inc.
 
 *
 * "The pipes and filters architectural pattern provides a
 * structure for systems that process a stream of data. Each
 * processing step is encapsulated in a filter component.
 * Data is passed through pipes between adjacent filters.
 * Recombining filters allows you to build families of
 * related systems." Buschmann, et al. 1996.
 */

#ifndef _pipesandfilters_h
#define _pipesandfilters_h

/**
 * STL used by this module.
 */

#include <deque>

/**
 * Exceptions used by this module.
 */

#include "filterexceptions.h"

#include <semaphore.h> 

/**
 * This class represents an abstract data type that is
 * transferred between the pipes and filters. For purposes
 * of this demonstration, it's just an address.
 */

class Packet
{
private:
	int seqno;
	
public:

	/**
	 * Default constructor.
	 */

	Packet();
	void setSeqNo(int seq) {
		seqno = seq;
	}
	
	int getSeqNo() {
		return seqno;
	}
};

/**
 * Forward declaration because Pipe and DataSink have
 * circular references.
 */

class DataSink;

/**
 * Pipes denote the connections between filters, between the data source
 * and the first filter, and between the last filter and the data sink.
 * If two active components are joined, the pipe synchronizes them.
 */

class Pipe
{

private:
	std::deque< Packet > packets;
	DataSink* observer;
	sem_t mutex;
	sem_t empty;
	sem_t full;

public:

	/**
	 * Default constructor.
	 */

	Pipe();

	/**
	 * Returns the number of queued packets waiting in the pipe.
	 */

	virtual int size();

	/**
	 * Called by the data sink on the far-end of the pipe to get
	 * packets out of the pipe. This method will throw an exception
	 * if no packets are in the queue.
	 */

	virtual Packet& read();

	/**
	 * Called by a data source to queue a packet on the pipe.
	 * This method queues the packet and notifies the data sink
	 * on the other side of the pipe that this pipe's state has changed.
	 */

	virtual void write( Packet& packet );

	/**
	 * Accessors to get and set the observer waiting for
	 * data available on the pipe.
	 */

	void setObserver( DataSink* observer );
	DataSink* getObserver();
};

/**
 * The data source represents the input to the system, and
 * provides a sequence of data values of the same structure
 * or type.
 */

class DataSource
{

private:
	Pipe* output;

public:

	/**
	 * Default constructor.
	 */

	DataSource();

	/**
	 * Typically called internally by the class/subclasses
	 * to add a packet for transmission to the assigned pipe.
	 */

	virtual void add( Packet& packet );

	/**
	 * Accessors to set the pipe this data source
	 * sends packets to.
	 */

	void setOutput( Pipe* output );
	Pipe* getOutput();
};

/**
 * Forward declaration of Filter.
 */

class Filter;

/**
 * The data sink collects the results from the end of the
 * pipeline. Two variants of the data sink are possible. An active
 * data sink pulls results out of the preceeding processing stage,
 * while a passive one allows the preceeding pipe to push or write
 * results onto it.
 */

class DataSink
{

private:
	Pipe* input;
	Filter* observer;

protected:
	std::deque< Packet > received;

public:
	DataSink();

	virtual int size();
	virtual Packet& nextPacket();

	/**
	 * Called by the pipe when new data has arrived
	 * and is available from retrieval by this data sink.
	 */

	virtual int notify();

	/**
	 * Accessors for setting the observer filter
	 * this sink will notify.
	 */

	void setObserver( Filter* filter );
	Filter* getObserver();

	/**
	 * Accessors for setting the pipe this sink talks to.
	 */

	void setInput( Pipe* newinput );
	Pipe* getInput();
};

/**
 * Filter components are the processing units of the pipeline. A filter
 * enriches, refines, or transforms its input data. It enriches data by
 * computing or adding information, refines data by concentrating or
 * extracting information, and transforms data by delivering the data
 * in some other representation.
 */

class Filter
{

private:
	DataSource* output;
	DataSink* input;

protected:
	virtual void transform();

public:
	Filter();
	virtual void notify();

	/**
	 * Accessors for setting up the sink and 
	 * source for this filter.
	 */

	void setDataSource( DataSource* ds );
	DataSource* getDataSource();
	void setDataSink( DataSink* ds );
	DataSink* getDataSink();
};

#endif
