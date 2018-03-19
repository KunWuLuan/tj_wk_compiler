#define _CRT_SECURE_NO_WARNINGS
#include "TJ_Log.h"
void logfile::writelog(fstream &log,void* msg,int len)
{
	for (int i = 0, j = 0; j < 2;)
	{
		log << setw(3) << i;
		if (i++==7) {
			log << '\t';
			j++;
			i = 0;
		}
	}
	log << setfill('0') << endl;
	char* stream = (char*)msg;
	for (int i = 0,j=0; i < len; i++,++j%=16) {
		if(j<8)
			log << ' ' << setw(2) << hex << (((unsigned int)(stream[i]))&0xff);
		else
			if(stream[i]<=126&&stream[i]>=33)
				log << setw(3) << stream[i];
			else
				log << setw(3) << '.';
		if (j == 15) {
			log << setfill('0') << endl;
		}
		else if (j == 7) {
			log << setfill(' ') << '\t';
			i -= 8;
		}
	}
	int last = len % 8;
	int start = len - last;
	log << setfill(' ');
	log << setw((8 - last) * 3)<<' ';
	log << '\t';
	for(int i=0;i<last;i++,start++)
		if (stream[start] <= 126 && stream[start] >= 33)
			log << setw(3) << stream[start];
		else
			log << setw(3) << '.';
	log << setw((8 - last) * 3) << ' ';
	log << endl;
}
int logfile::create_log(fstream &log, bool tty)
{
	time_t timer;
	tm* tblock;
	char filename[40]={0};
	timer = time(0);
	tblock = localtime(&timer);
	sprintf(filename, "%02d%02d%02d%02d%02d.log"
		, tblock->tm_mon + 1, tblock->tm_mday
		, tblock->tm_hour, tblock->tm_min, tblock->tm_sec);
	log.open(filename,ios::out);

	if(log.is_open()==0)
			return -1;
	return 0;
}
