#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_FIELDS 64
#define MAX_VALUE_LEN 512

typedef struct {
    char username[32];
    char name[MAX_VALUE_LEN];
    char email[MAX_VALUE_LEN];
    char bio[MAX_VALUE_LEN];
    int followers;
    int following;
    int repositories;
    int location[64];
    int locations_count;
} GitHubProfile;

typedef struct {
    char key[256];
    char value[MAX_VALUE_LEN];
} ConfigField;

static void trim_whitespace(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;
    
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

static int parse_yaml_value(const char *line, const char *prefix, 
                           ConfigField *field, size_t field_idx) {
    if (strncmp(line, prefix, strlen(prefix)) != 0) return 0;
    
    line += strlen(prefix);
    trim_whitespace(line);
    
    if (*line == ':') {
        line++;
        trim_whitespace(line);
        
        if (strlen(field->key) > 254) field->key[255] = '\0';
        else strcpy(field->key, prefix + strlen(prefix));
        
        if (strlen(line) > MAX_VALUE_LEN - 1) line[MAX_VALUE_LEN - 1] = '\0';
        else strcpy(field->value, line);
        
        return 1;
    }
    return 0;
}

static int parse_yaml_integer(const char *line, const char *prefix, 
                             int *value) {
    if (strncmp(line, prefix, strlen(prefix)) != 0) return 0;
    
    line += strlen(prefix);
    trim_whitespace(line);
    
    if (*line == ':') {
        line++;
        trim_whitespace(line);
        
        char end = '\0';
        while (isdigit((unsigned char)*line) || *line == '-' || 
               *line == '+' || *line == 'e' || *line == 'E' || 
               *line == '.' || *line == end) {
            if (*line == end) break;
            line++;
        }
        
        char buf[64];
        size_t len = 0;
        while (len < sizeof(buf) - 1 && isspace((unsigned char)*line)) line++;
        while (isdigit((unsigned char)*line) || *line == '-' || 
               *line == '+' || *line == 'e' || *line == 'E' || 
               *line == '.' || len < sizeof(buf) - 1) {
            if (*line == end) break;
            buf[len++] = *line++;
        }
        
        char *p = (char*)buf;
        while (isspace((unsigned char)*p)) p++;
        
        *value = atoi(p);
        return 1;
    }
    return 0;
}

static int parse_yaml_location(const char *line, const char *prefix, 
                              int *location) {
    if (strncmp(line, prefix, strlen(prefix)) != 0) return 0;
    
    line += strlen(prefix);
    trim_whitespace(line);
    
    if (*line == ':') {
        line++;
        trim_whitespace(line);
        
        char buf[64];
        size_t len = 0;
        while (len < sizeof(buf) - 1 && isalnum((unsigned char)*line)) {
            buf[len++] = *line++;
        }
        buf[len] = '\0';
        
        if (strlen(buf) > 0) {
            location[location[locations_count]] = atoi(buf);
            return 1;
        }
    }
    return 0;
}

static int parse_yaml_array(const char *line, const char *prefix, 
                           ConfigField *field, size_t field_idx) {
    if (strncmp(line, prefix, strlen(prefix)) != 0) return 0;
    
    line += strlen(prefix);
    trim_whitespace(line);
    
    if (*line == ':') {
        line++;
        trim_whitespace(line);
        
        char *start = line;
        size_t len = 0;
        
        while (len < sizeof(field->value) - 1 && 
               !isspace((unsigned char)*line)) {
            field->value[len++] = *line++;
        }
        field->value[len] = '\0';
        
        return 1;
    }
    return 0;
}

static int parse_yaml(const char *filename, GitHubProfile *profile) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return -1;
    }
    
    memset(profile, 0, sizeof(*profile));
    profile->locations_count = 0;
    
    char line[MAX_LINE];
    int in_nested = 0;
    ConfigField field;
    size_t field_idx = 0;
    
    while (fgets(line, MAX_LINE, fp)) {
        trim_whitespace(line);
        
        if (strlen(line) == 0 || line[0] == '#') continue;
        
        if (in_nested && strncmp(line, "  ", 2) != 0) {
            in_nested = 0;
            field_idx = 0;
        }
        
        if (strncmp(line, "- ", 2) == 0 || 
            strncmp(line, "-", 1) == 0) {
            line += 2;
            trim_whitespace(line);
            
            if (strlen(field.key) > 0 && field_idx < MAX_FIELDS - 1) {
                int parsed = parse_yaml_value(line, "    ", &field, field_idx++);
                if (!parsed) continue;
                
                if (strcmp(field.key, "username") == 0) {
                    strncpy(profile->username, field.value, sizeof(profile->username) - 1);
                } else if (strcmp(field.key, "name") == 0) {
                    strncpy(profile->name, field.value, sizeof(profile->name) - 1);
                } else if (strcmp(field.key, "email") == 0) {
                    strncpy(profile->email, field.value, sizeof(profile->email) - 1);
                } else if (strcmp(field.key, "bio") == 0) {
                    strncpy(profile->bio, field.value, sizeof(profile->bio) - 1);
                } else if (strcmp(field.key, "followers") == 0) {
                    profile->followers = atoi(field.value);
                } else if (strcmp(field.key, "following") == 0) {
                    profile->following = atoi(field.value);
                } else if (strcmp(field.key, "repositories") == 0) {
                    profile->repositories = atoi(field.value);
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value); field.value++) {}
                    while (*field.value && isalnum((unsigned char)*field.value)) {
                        loc[profile->locations_count] = atoi(field.value);
                        profile->locations_count++;
                        field.value++;
                    }
                } else if (strcmp(field.key, "location") == 0) {
                    int loc[MAX_LINE];
                    memset(loc, 0, sizeof(loc));
                    for (int i = 0; i < MAX_LINE && isspace((unsigned char)*field.value);