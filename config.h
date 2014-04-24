#pragma once
#include <yaml.h>

void config_init(char *config_file);
char * config_get(char *key);