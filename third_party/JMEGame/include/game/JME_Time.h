#ifndef JME_Time_h__
#define JME_Time_h__

#include <time.h>
namespace JMEngine
{
	namespace game
	{
		class Time
		{
		public:
			static bool isTimeToday(time_t t);
			static int todayYearday();
			static tm localTime();
			static tm localTime(time_t t);
		};
	}
}
#endif // JME_Time_h__
