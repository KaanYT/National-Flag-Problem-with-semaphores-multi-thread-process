#include <time.h>
#include <sys/time.h>

void timeCalculation(void (*f)())
{
	struct timeval time;
	struct timezone timez;
	int rc;
	long x;
	struct timeval tv;
	struct timezone tz;
	struct tm *tm=malloc(sizeof(time));
	long start,stop;
	gettimeofday(&tv, &tz);
	tm = localtime(&tv.tv_sec);
	printf("TIMESTAMP-START\t %d:%02d:%02d:%d (~%d ms)\n",tm->tm_hour,tm->tm_min,tm->tm_sec,(unsigned int)tv.tv_usec, tm->tm_hour*3600*1000 + tm->tm_min*60*1000 + tm->tm_sec*1000 + (unsigned int)tv.tv_usec/1000);
	start = tm->tm_hour * 3600 * 1000 + tm->tm_min * 60* 1000 + tm->tm_sec * 1000+ tv.tv_usec / 1000;

	//////////THE CODE:

		(*f)();

	//////////THE CODE ENDS:

	gettimeofday(&tv, &tz);
	tm = localtime(&tv.tv_sec);
	stop = tm->tm_hour * 3600 * 1000 + tm->tm_min*60*1000+tm->tm_sec * 1000 + tv.tv_usec / 1000;
	printf("TIMESTAMP-END\t %d:%02d:%02d:%d (~%d ms)\n", tm->tm_hour,tm->tm_min, tm->tm_sec,(unsigned int)tv.tv_usec,tm->tm_hour*3600*1000+tm->tm_min*60*1000+tm->tm_sec*1000+(unsigned int)tv.tv_usec/1000);
	printf("ELAPSED\t %ld ms\n", stop - start);	
}