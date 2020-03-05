/**
 * testpipes.cpp
 * Implemented by Blueprint Technologies, Inc.
 */

#include "pipesandfilters.h"
#include <iostream>
#include <pthread.h>
#include<unistd.h>
using namespace std;

void *  activateSource(void* _source) {
	DataSource* source = (DataSource*)_source;
	Packet packet1, packet2;
	int i = 0;
	while(1) {
		usleep(1);
		packet1.setSeqNo(i++);
		source -> add( packet1 );
		//source -> add( packet2 );
	}
}

void *  activateFilter(void* _filter) {
	Filter* filter = (Filter*)_filter;
	DataSink* filterSink;
	while(1) {
		//sleep(1);
		filterSink = filter->getDataSink();
		filterSink->notify();
	}
}

void *  activateSink(void* _sink) {
	DataSink* sink = (DataSink*)_sink;
	while(1) {
		//usleep(1);
		int seq = sink->notify();
		cout << "Sink: Size " << sink -> size() << " seqno " << seq << endl;
	}
}

int main( void )
{

	/**
	 * Instantiate our pattern blueprint classes.
	 */

	DataSink* sink = new DataSink();
	DataSource* source = new DataSource();
	Pipe* pipe = new Pipe();

	/**
	 * Configure them.
	 */

	source -> setOutput( pipe );
	//pipe -> setObserver( sink );
	//sink -> setInput( pipe );


	/**
	 * Plug a couple of packets into the source, and they
	 * will migrate across to the sink.
	 */

	// Packet packet1, packet2;
	// source -> add( packet1 );
	// source -> add( packet2 );

	/**
	 * Print out the number of packets in the sink.
	 */

	//cout << "Sink: " << sink -> size() << endl;

	/**
	 * Add a filter with a source and sink.
	 */

	DataSink* filterSink = new DataSink();
	DataSource* filterSource = new DataSource();
	Filter* filter = new Filter();
	Pipe* pipe2 = new Pipe();

	/**
	 * Reconfigure all the objects for the pipeline.
	 * Our object graph should look like this:
	 *		source -> pipe -> filterSink -> filter -> filterSource -> pipe2 -> sink
	 * 
	 * NOTE: filterSink and filterSource and encapsulated inside filter.
	 */

	pipe -> setObserver( filterSink );
	filterSink -> setInput( pipe );
	filterSink -> setObserver( filter );
	filter -> setDataSink( filterSink );
	filter -> setDataSource( filterSource );
	filterSource -> setOutput( pipe2 );
	pipe2 -> setObserver( sink );
	sink -> setInput( pipe2 );

	pthread_t sourceThread;
  	pthread_attr_t thread_attr1;
  	pthread_attr_init(&thread_attr1);
  	pthread_create(&sourceThread, &thread_attr1, activateSource, (void*)source);

	pthread_t filterThread;
	pthread_attr_t thread_attr3;
	pthread_attr_init(&thread_attr3);
	pthread_create(&filterThread, &thread_attr3, activateFilter, (void*)filter);

	pthread_t sinkThread;
  	pthread_attr_t thread_attr2;
  	pthread_attr_init(&thread_attr2);
  	pthread_create(&sinkThread, &thread_attr2, activateSink, (void*)sink);

	/**
	 * Add two more packets.
	 */

	//source -> add( packet1 );
	//source -> add( packet2 );
	

	/**
	 * Print out the number of packets in the sink.
	 */
	sleep(1000);
	cout << "Sink: " << sink -> size() << endl;

	return 0;
};
