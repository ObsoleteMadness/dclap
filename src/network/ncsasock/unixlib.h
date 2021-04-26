/* unixlib.h
  dgg, ansi c header for damn k&r stuff
  */
  
  
char *getwd(char *where) ;
int chdir( char * pathName);
int gettimeofday(struct timeval *tp,struct timezone *tzp); 
int sleep(unsigned seconds) ;
long int getuid(void);
struct passwd *getpwent(void);
struct passwd *getpwuid(void);
struct passwd *getpwnam(void);
int chmod(char *path,int mode);
char *mktemp(char * atemplate);
void abort(void);
void bzero(void * b,long s);
void bfill( void *b,long s,char fill);
void bcopy (void* c1, void* c2, long s);
int bcmp (void* c1,void* c2,long s);
int strcasecmp(char* s1,char* s2);
int strncasecmp(char *s1,char * s2,int n);
