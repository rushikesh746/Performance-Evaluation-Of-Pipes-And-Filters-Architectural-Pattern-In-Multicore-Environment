/**
 * pipesandfilters.cpp
 */

#include "pipesandfilters.h"
#include <iostream>
using namespace std;
/**
 * Default constructor.
 */

Packet::Packet()
{
};

/**
 * Default constructor.
 */
#define BUFFER_SIZE 100
Pipe::Pipe()
{
	// printf("Pipe constructor called\n");
	sem_init(&mutex, 0, 1);
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, BUFFER_SIZE);
	// int value; 
	// sem_getvalue(&full, &value); 
	// printf("The value of the full is %d ", value);
	// sem_getvalue(&empty, &value); 
	// printf(" and empty is %d\n", value);
};

int Pipe::size()
{
	return (int) packets.size();
};

//CONSUMER
Packet& Pipe::read()
{
	sem_wait(&full);
	sem_wait(&mutex);
	if( size() == 0 ) 
	{
		// int value; 
		// sem_getvalue(&full, &value); 
		// printf("The value of the full is %d ", value);
		// sem_getvalue(&empty, &value); 
		// printf(" and empty is %d\n", value);
		
		throw new EmptyPipeException();
	}

	Packet packet = packets.back();
	packets.pop_back();
	
	sem_post(&mutex);
	sem_post(&empty);
	
	return packet;
};

//PRODUCER
void Pipe::write( Packet& packet )
{
	//printf("Write called\n");
	sem_wait(&empty);
	sem_wait(&mutex);
	packets.push_front( packet );
	sem_post(&mutex);
	sem_post(&full);
	//observer -> notify();
};

void Pipe::setObserver( DataSink* observer )
{
	this -> observer = observer;
};

DataSink* Pipe::getObserver()
{
	return observer;
};

/**
 * Default constructor.
 */

DataSource::DataSource()
{
	output = (Pipe *) 0;
};

void DataSource::add( Packet& packet )
{
	if( output != (Pipe *) 0 )
	{
		output -> write( packet );
	}
	else
	{
		throw new NoOutputPipeException();
	}
};

void DataSource::setOutput( Pipe* output )
{
	this -> output = output;
};

Pipe* DataSource::getOutput()
{
	return output;
};

DataSink::DataSink()
{
	input = (Pipe *) 0;
	observer = (Filter *) 0;
};

int DataSink::size()
{
	return received.size();
};

Packet& DataSink::nextPacket()
{
	Packet packet = received.back();
	received.pop_back();
	return packet;
};

int DataSink::notify()
{	
	Packet pkt;
	int ret = -1;
	
//	if( input -> size() > 0 )
	{
		pkt = input -> read();
		received.push_front( pkt );
		ret = pkt.getSeqNo();
	}
	if( observer )
	{
		observer -> notify();
	}
	return ret;
};

void DataSink::setInput( Pipe* newinput )
{
	input = newinput;
};

Pipe* DataSink::getInput()
{
	return input;
};

void DataSink::setObserver( Filter* filter )
{
	this -> observer = filter;
}

Filter* DataSink::getObserver()
{
	return observer;
}

/**
 * Default constructor.
 */

Filter::Filter()
{
};

void Filter::transform()
{
	while( input -> size() > 0 )
	{
		output -> add( input -> nextPacket() );
	}
}

void Filter::notify()
{
	transform();
}

void Filter::setDataSource( DataSource* ds )
{
	output = ds;
};

DataSource* Filter::getDataSource()
{
	return output;
};

void Filter::setDataSink( DataSink* ds )
{
	input = ds;
};

DataSink* Filter::getDataSink()
{
	return input;
};