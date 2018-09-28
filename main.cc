#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <FL/Fl.h>
#include <FL/Fl_Double_Window.h>
#include <FL/Fl_Output.h>

static long	parsetime(char *s);
static void	fmtdisptime(char *dst, size_t dstsz, long t);
static long	millisecs(void);
static void	winclosed(Fl_Widget *wg, void *args);

int
main(int argc, char **argv)
{
	if(argc != 2){
		printf("usage: ramen 3h2m1s\n");
		return 1;
	}

	long endtime = millisecs() + parsetime(argv[1]);

	auto window = new Fl_Double_Window(120+1, 32+1, "ramen");
	window->position((Fl::w() - window->w())/2, (Fl::h() - window->h())/2);
	window->callback(winclosed);

	auto cntdn = new Fl_Output(0, 0, 120, 32);
	cntdn->callback(winclosed);
	cntdn->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);

	window->end();
	window->iconize();
	
	long triggered = 0;
	for(;;){
		Fl::check();
		long t = millisecs();
		char timestr[32];
		fmtdisptime(timestr, sizeof timestr, endtime - t);
		cntdn->value(timestr);
		window->label(timestr);
		if(t > endtime && !triggered){
			PlaySound("alarm.wav", NULL, SND_LOOP|SND_ASYNC);
			triggered = t;
			window->iconize();
			window->show();
		} else if(t > triggered + 60000)
			PlaySound(NULL, NULL, 0);
		Sleep(100);
	}
	return 0;
}

static long
parsetime(char *s)
{
	long acc = 0;
	for(char *p = s; *p != '\0'; p++){
		if(*p == ' ' || *p == '\t')
			continue;
		if(*p >= '0' && *p <= '9'){
			char *end;
			long t = strtol(p, &end, 10);
			p = end;
			if(*p == 's' || *p == '\0')
				acc += t*1000;
			else if(*p == 'm')
				acc += t*60*1000;
			else if(*p == 'h')
				acc += t*60*60*1000;
		}
	}
	return acc;
}

static void
fmtdisptime(char *dst, size_t dstsz, long t)
{
	const char *prefix;
	if(t > 0){
		prefix = "T-";
		t += 1000;
	}else{
		t = -t;
		prefix = "T+";
	}
	long ms = t%1000;
	long s = (t/1000) % 60;
	long m = (t/(60*1000)) % 60;
	long h = (t/(60*60*1000));
	snprintf(dst, dstsz, "%s%02ld:%02ld:%02ld", prefix, h, m, s);
}

static long
millisecs(void)
{
	return GetTickCount();
}

static void
winclosed(Fl_Widget *wg, void *args)
{
	exit(0);
}
