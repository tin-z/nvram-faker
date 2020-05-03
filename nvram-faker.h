#ifndef __NVRAM_FAKER_H__
#define __NVRAM_FAKER_H__

//linvram.so
char *nvram_get(const char *key);
int nvram_set(const char *key, const char *value);
int nvram_unset(const char *key);


//libshared.so
int nvram_get_int(const char *key);
int nvram_set_int(const char *key, int value);


#endif /* __NVRAM_FAKER_H__ */
