#ifndef JME_Random_h__
#define JME_Random_h__

/********************************************************************
	created:	2015/06/19
	author:		huangzhi
	
	purpose:	�򵥵��������װ�� ��������ӽ��м򵥴���
	warning:	
*********************************************************************/

#include <assert.h>
#include <time.h>
#include <random>
#include <algorithm>
#include <vector>

#include "boost/random.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

#define Rnd Random::getInstance


class RandSeed
{
public:
	friend class Random;
	RandSeed(){}

	RandSeed(int prob, void* bind):
		_prob(prob),
		_bind(bind),
		_begin(0),
		_end(0)
	{
	}
public:
	int _prob;	//����
	void* _bind;	//�󶨵Ķ���
protected:
	int _begin;
	int _end;
};

class Random
{
public:
public:
	Random(void){ _random_seed = time(NULL); };
	virtual ~Random(void){};

	int randomInt()
	{
		boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration duration( time.time_of_day() );

		boost::mt19937 gen(duration.total_nanoseconds() + _random_seed);
		boost::uniform_int<> dist(0, INT_MAX);

		boost::variate_generator<boost::mt19937, boost::uniform_int<> > die(gen, dist);

		return _random_seed = die();
	}
	int randomInt(int start, int end)
	{
		assert(start <= end);

		boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration duration( time.time_of_day() );

		boost::mt19937 gen(duration.total_nanoseconds() + _random_seed);
		boost::uniform_int<> dist(start, end);

		boost::variate_generator<boost::mt19937, boost::uniform_int<> > die(gen, dist);

		return _random_seed = die();
	}

	bool randomGreater(int perc)
	{
		int ra = randomInt(1, 100);
		return ra >= perc;
	}

	void* randomBySeeds(std::vector<RandSeed>& seeds)
	{
		assert(!seeds.empty());

		for (size_t i = 0; i < seeds.size(); ++i)
		{
			if (0 == i)
				seeds[i]._begin = 1;
			else
				seeds[i]._begin = seeds[i - 1]._end;

			seeds[i]._end = seeds[i]._begin + seeds[i]._prob;
		}

		int rmax = seeds.rbegin()->_end;

		int ra = randomInt(1, rmax - 1);
		for (auto it = seeds.begin(); it != seeds.end(); ++it)
		{
			if (ra >= it->_begin && ra < it->_end)
				return it->_bind;
		}
		return nullptr;
	}
private:
	time_t _random_seed;
};
#endif // JME_Random_h__
