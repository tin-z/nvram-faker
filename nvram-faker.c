#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nvram-faker.h"
//include before ini.h to override ini.h defaults
#include "nvram-faker-internal.h"
#include "ini.h"

#define RED_ON "\033[22;31m"
#define RED_OFF "\033[22;00m"
#define DEFAULT_KV_PAIR_LEN 1024
#define Size_Int 10

static int kv_count=0;
static int key_value_pair_len=DEFAULT_KV_PAIR_LEN;
static char **key_value_pairs=NULL;
static int allready_parsed=0;

//unsigned int output_mutex = 0;

static int ini_handler(void *user, const char *section, const char *name,const char *value)
{
    printf("ini_handler: key:%s value:%s\n",name,value);

    int old_kv_len;
    char **kv;
    char **new_kv;
    int i;
    
    if(NULL == user || NULL == section || NULL == name || NULL == value)
    {
        DEBUG_PRINTF("bad parameter to ini_handler\n");
        return 0;
    }
    kv = *((char ***)user);
    if(NULL == kv)
    {
        LOG_PRINTF("kv is NULL\n");
        return 0;
    }
    
    DEBUG_PRINTF("kv_count: %d, key_value_pair_len: %d\n", kv_count,key_value_pair_len);
    if(kv_count >= key_value_pair_len)
    {
        old_kv_len=key_value_pair_len;
        key_value_pair_len=(key_value_pair_len * 2);
        new_kv=(char **)malloc(key_value_pair_len * sizeof(char **));
        if(NULL == kv)
        {
            LOG_PRINTF("Failed to reallocate key value array.\n");
            return 0;
        }
        for(i=0;i<old_kv_len;i++)
        {
            new_kv[i]=kv[i];
        }
        free(*(char ***)user);
        kv=new_kv;
        *(char ***)user=kv;
    }
    DEBUG_PRINTF("Got %s:%s\n",name,value);
    kv[kv_count++]=strdup(name);
    kv[kv_count++]=strdup(value);
    
    return 1;
}


void initialize_ini(void)
{
    int ret;
    DEBUG_PRINTF("Initializing.\n");

    if (NULL == key_value_pairs)
    {
        key_value_pairs=malloc(key_value_pair_len * sizeof(char **));
    }
    if(NULL == key_value_pairs)
    {
        LOG_PRINTF("Failed to allocate memory for key value array. Terminating.\n");
        exit(1);
    }
    
    ret = ini_parse(INI_FILE_PATH,ini_handler,(void *)&key_value_pairs);
    if (0 != ret)
    {
        LOG_PRINTF("ret from ini_parse was: %d\n",ret);
        LOG_PRINTF("INI parse failed. Terminating\n");
        free(key_value_pairs);
        key_value_pairs=NULL;
        exit(1);
    }else
    {
        DEBUG_PRINTF("ret from ini_parse was: %d\n",ret);
    }
    
    return;
    
}


void end(void)
{
    int i;
    for (i=0;i<kv_count;i++)
    {
        free(key_value_pairs[i]);
    }
    free(key_value_pairs);
    key_value_pairs=NULL;
    
    return;
}


int nvram_set(const char *key, const char *value)
{
    printf("nvram_set: key:%s value:%s\n",key,value);
    int i;
    int found=0, result=0;

    //lock_mutex(&output_mutex);
    if( ! allready_parsed ){
      initialize_ini();
      allready_parsed = 1;
    }
    //unlock_mutex(&output_mutex);
    
    for(i=0;i<kv_count;i+=2)
    {
        if(strcmp(key,key_value_pairs[i]) == 0)
        {
            //LOG_PRINTF("%s=%s\n",key,key_value_pairs[i+1]);
            found = 1;
            key_value_pairs[i+1] = strdup(value);
            break;
        }
    }

    char **kv;
    char **new_kv;

    if(!found)
    {
      kv = key_value_pairs;
      if(NULL == kv)
      {
          LOG_PRINTF("kv is NULL\n");
          return 0;
      }
      
      DEBUG_PRINTF("kv_count: %d, key_value_pair_len: %d\n", kv_count,key_value_pair_len);
      if(kv_count >= key_value_pair_len)
      {
          //old_kv_len=key_value_pair_len;
          key_value_pair_len=(key_value_pair_len * 2);
          new_kv=(char **)realloc(key_value_pairs, key_value_pair_len * sizeof(char **));
          if(NULL == new_kv)
          {
              LOG_PRINTF("Failed to reallocate key value array.\n");
              return 0;
          }
          kv=new_kv;
          key_value_pairs=kv;
      }
      DEBUG_PRINTF("Got %s:%s\n",key,value);
      kv[kv_count++]=strdup(key);
      kv[kv_count++]=strdup(value);
      result=0;
    }
    return result;
}


int nvram_unset(const char *key)
{
  return nvram_set_int(key, 0);
}


char *nvram_get(const char *key)
{
    int i;
    int found=0;
    char *value;
    char *ret;

    //lock_mutex(&output_mutex);
    if( ! allready_parsed ){
      initialize_ini();
      allready_parsed = 1;
    }
    //unlock_mutex(&output_mutex);
    
    for(i=0;i<kv_count;i+=2)
    {
        if(strcmp(key,key_value_pairs[i]) == 0)
        {
            LOG_PRINTF("%s=%s\n",key,key_value_pairs[i+1]);
            found = 1;
            value=key_value_pairs[i+1];
            break;
        }
    }

    ret = NULL;
    if(!found)
    {
            LOG_PRINTF( RED_ON"%s=Unknown\n"RED_OFF,key);
    }else
    {

            ret=strdup(value);
    }
    return ret;
}


// from here: libshared.so
int nvram_get_int(const char *key)
{
  char *ret=nvram_get(key);
  int ret2=atoi(ret);
  return ret2;
}

int nvram_set_int(const char *key, int value)
{
  char buff[Size_Int+2];
  memset(buff, 0, Size_Int+2);
  snprintf(buff, Size_Int+1,"%d",value);
  return nvram_set(key, buff);
}

