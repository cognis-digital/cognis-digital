#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NAME_LEN 256
#define MAX_BIO_LEN 1024
#define MAX_LINKS 32
#define MAX_LINK_LEN 256

typedef struct {
    char name[MAX_NAME_LEN];
    char bio[MAX_BIO_LEN];
    char location[256];
    char website[MAX_LINK_LEN];
    char email[MAX_LINK_LEN];
    char twitter[MAX_LINK_LEN];
    char linkedin[MAX_LINK_LEN];
    char github[MAX_LINK_LEN];
    char portfolio[MAX_LINK_LEN];
    bool has_name;
    bool has_bio;
    bool has_location;
    bool has_website;
    bool has_email;
    bool has_twitter;
    bool has_linkedin;
    bool has_github;
    bool has_portfolio;
} GitHubProfile;

typedef struct {
    char *links[MAX_LINKS];
    int link_count;
} LinksArray;

GitHubProfile* profile_create(void) {
    GitHubProfile *p = malloc(sizeof(GitHubProfile));
    if (!p) return NULL;
    
    memset(p, 0, sizeof(GitHubProfile));
    p->name[0] = '\0';
    p->bio[0] = '\0';
    p->location[0] = '\0';
    p->website[0] = '\0';
    p->email[0] = '\0';
    p->twitter[0] = '\0';
    p->linkedin[0] = '\0';
    p->github[0] = '\0';
    p->portfolio[0] = '\0';
    
    return p;
}

void profile_free(GitHubProfile *p) {
    if (!p) return;
    free(p);
}

GitHubProfile* profile_parse(const char *json, size_t len) {
    GitHubProfile *p = profile_create();
    if (!p) return NULL;
    
    const char *start = json;
    const char *end = start + len;
    
    while (start < end && !p->has_name) {
        if (*start == '"') {
            start++;
            size_t i = 0;
            while (start < end && *start != '"' && i < MAX_NAME_LEN - 1) {
                p->name[i++] = *start++;
            }
            if (i > 0) {
                p->has_name = true;
            }
        } else if (*start == '{') {
            start++;
        } else {
            start++;
        }
    }
    
    while (start < end && !p->has_bio) {
        if (*start == '"') {
            start++;
            size_t i = 0;
            while (start < end && *start != '"' && i < MAX_BIO_LEN - 1) {
                p->bio[i++] = *start++;
            }
            if (i > 0) {
                p->has_bio = true;
            }
        } else if (*start == '{') {
            start++;
        } else {
            start++;
        }
    }
    
    while (start < end && !p->has_location) {
        if (*start == '"') {
            start++;
            size_t i = 0;
            while (start < end && *start != '"' && i < 255) {
                p->location[i++] = *start++;
            }
            if (i > 0) {
                p->has_location = true;
            }
        } else if (*start == '{') {
            start++;
        } else {
            start++;
        }
    }
    
    while (start < end && !p->has_website) {
        if (*start == '"') {
            start++;
            size_t i = 0;
            while (start < end && *start != '"' && i < MAX_LINK_LEN - 1) {
                p->website[i++] = *start++;
            }
            if (i > 0) {
                p->has_website = true;
            }
        } else if (*start == '{') {
            start++;
        } else {
            start++;
        }
    }
    
    while (start < end && !p->has_email) {
        if (*start == '"') {
            start++;
            size_t i = 0;
            while (start < end && *start != '"' && i < MAX_LINK_LEN - 1) {
                p->email[i++] = *start++;
            }
            if (i > 0) {
                p->has_email = true;
            }
        } else if (*start == '{') {
            start++;
        } else {
            start++;
        }
    }
    
    while (start < end && !p->has_twitter) {
        if (*start == '"') {
            start++;
            size_t i = 0;
            while (start < end && *start != '"' && i < MAX_LINK_LEN - 1) {
                p->twitter[i++] = *start++;
            }
            if (i > 0) {
                p->has_twitter = true;
            }
        } else if (*start == '{') {
            start++;
        } else {
            start++;
        }
    }
    
    while (start < end && !p->has_linkedin) {
        if (*start == '"') {
            start++;
            size_t i = 0;
            while (start < end && *start != '"' && i < MAX_LINK_LEN - 1) {
                p->linkedin[i++] = *start++;
            }
            if (i > 0) {
                p->has_linkedin = true;
            }
        } else if (*start == '{') {
            start++;
        } else {
            start++;
        }
    }
    
    while (start < end && !p->has_github) {
        if (*start == '"') {
            start++;
            size_t i = 0;
            while (start < end && *start != '"' && i < MAX_LINK_LEN - 1) {
                p->github[i++] = *start++;
            }
            if (i > 0) {
                p->has_github = true;
            }
        } else if (*start == '{') {
            start++;
        } else {
            start++;
        }
    }
    
    while (start < end && !p->has_portfolio) {
        if (*start == '"') {
            start++;
            size_t i = 0;
            while (start < end && *start != '"' && i < MAX_LINK_LEN - 1) {
                p->portfolio[i++] = *start++;
            }
            if (i > 0) {
                p->has_portfolio = true;
            }
        } else if (*start == '{') {
            start++;
        } else {
            start++;
        }
    }
    
    return p;
}

GitHubProfile* profile_merge(GitHubProfile *base, GitHubProfile *override) {
    if (!base || !override) return NULL;
    
    GitHubProfile *result = profile_create();
    if (!result) return NULL;
    
    // Name: override wins
    if (override->has_name) {
        strncpy(result->name, override->name, MAX_NAME_LEN - 1);
        result->name[MAX_NAME_LEN - 1] = '\0';
        result->has_name = true;
    } else if (base->has_name) {
        strncpy(result->name, base->name, MAX_NAME_LEN - 1);
        result->name[MAX_NAME_LEN - 1] = '\0';
        result->has_name = true;
    }
    
    // Bio: override wins
    if (override->has_bio) {
        strncpy(result->bio, override->bio, MAX_BIO_LEN - 1);
        result->bio[MAX_BIO_LEN - 1] = '\0';
        result->has_bio = true;
    } else if (base->has_bio) {
        strncpy(result->bio, base->bio, MAX_BIO_LEN - 1);
        result->bio[MAX_BIO_LEN - 1] = '\0';
        result->has_bio = true;
    }
    
    // Location: override wins
    if (override->has_location) {
        strncpy(result->location, override->location, 255);
        result->location[255] = '\0';
        result->has_location = true;
    } else if (base->has_location) {
        strncpy(result->location, base->location, 255);
        result->location[255] = '\0';
        result->has_location = true;
    }
    
    // Website: override wins
    if (override->has_website) {
        strncpy(result->website, override->website, MAX_LINK_LEN - 1);
        result->website[MAX_LINK_LEN - 1] = '\0';
        result->has_website = true;
    } else if (base->has_website) {
        strncpy(result->website, base->website, MAX_LINK_LEN - 1);
        result->website[MAX_LINK_LEN - 1] = '\0';
        result->has_website = true;
    }
    
    // Email: override wins
    if (override->has_email) {
        strncpy(result->email, override->email, MAX_LINK_LEN - 1);
        result->email[MAX_LINK_LEN - 1] = '\0';
        result->has_email = true;
    } else if (base->has_email) {
        strncpy(result->email, base->email, MAX_LINK_LEN - 1);
        result->email[MAX_LINK_LEN - 1] = '\0';
        result->has_email = true;
    }
    
    // Twitter: override wins
    if (override->has_twitter) {
        strncpy(result->twitter, override->twitter, MAX_LINK_LEN - 1);
        result->twitter[MAX_LINK_LEN - 1] = '\0';
        result->has_twitter = true;
    } else if (base->has_twitter) {
        strncpy(result->twitter, base->twitter, MAX_LINK_LEN - 1);
        result->twitter[MAX_LINK_LEN - 1] = '\0';
        result->has_twitter = true;
    }
    
    // LinkedIn: override wins
    if (override->has_linkedin) {
        strncpy(result->linkedin, override->linkedin, MAX_LINK_LEN - 1);
        result->linkedin[MAX_LINK_LEN - 1] = '\0';
        result->has_linkedin = true;
    } else if (base->has_linkedin) {
        strncpy(result->linkedin, base->linkedin, MAX_LINK_LEN - 1);
        result->linkedin[MAX_LINK_LEN - 1] = '\0';
        result->has_linkedin = true;
    }
    
    // GitHub: override wins
    if (override->has_github) {
        strncpy(result->github, override->github, MAX_LINK_LEN - 1);
        result->github[MAX_LINK_LEN - 1] = '\0';
        result->has_github = true;
    } else if (base->has_github) {
        strncpy(result->github, base->github, MAX_LINK_LEN - 1);
        result->github[MAX_LINK_LEN - 1] = '\0';
        result->has_github = true;
    }
    
    // Portfolio: override wins
    if (override->has_portfolio) {
        strncpy(result->portfolio, override->portfolio, MAX_LINK_LEN - 1);
        result->portfolio[MAX_LINK_LEN - 1] = '\0';
        result->has_portfolio = true;
    } else if (base->has_portfolio) {
        strncpy(result->portfolio, base->portfolio, MAX_LINK_LEN - 1);
        result->portfolio[MAX_LINK_LEN - 1] = '\0';
        result->has_portfolio = true;
    }
    
    return result;
}

void profile_print(const GitHubProfile *p) {
    printf("=== Merged Profile ===\n");
    printf("Name: %s\n", p->name[0] ? p->name : "(empty)");
    printf("Bio:   %s\n", p->bio[0] ? p->bio : "(empty)");
    printf("Location: %s\n", p->location[0] ? p->location : "(empty)");
    printf("Website: %s\n", p->website[0] ? p->website : "(empty)");
    printf("Email:  %s\n", p->email[0] ? p->email : "(empty)");
    printf("Twitter: %s\n", p->twitter[0] ? p->twitter : "(empty)");
    printf("LinkedIn: %s\n", p->linkedin[0] ? p->linkedin : "(empty)");
    printf("GitHub:  %s\n", p->github[0] ? p->github : "(empty)");
    printf("Portfolio: %s\n", p->portfolio[0] ? p->portfolio : "(empty)");
}

int main(void) {
    // Demo data - simulating two profile configs to merge
    
    GitHubProfile *base = profile_create();
    if (!base) return 1;
    
    base->name[0] = 'T';
    base->name[1] = 'e';
    base->name[2] = '\0';
    base->has_name = true;
    
    base->bio[0] = 'S';
    base->bio[1] = 'o';
    base->bio[2] = 'f';
    base->bio[3] = 't';
    base->bio[4] = 'w';
    base->bio[5] = 'a';
    base->bio[6] = 'r';
    base->bio[7] = 'e';
    base->bio[8] = '\0';
    base->has_bio = true;
    
    base->location[0] = 'S';
    base->location[1] = 'y';
    base->location[2] = 'd';
    base->location[3] = 'n';
    base->location[4] = 'a';
    base->location[5] = 'm';
    base->location[6] = '\0';
    base->has_location = true;
    
    base->website[0] = 'w';
    base->website[1] = 'w';
    base->website[2] = '.';
    base->website[3] = 'e';
    base->website[4] = 'x';
    base->website[5] = 'a';
    base->website[6] = 'm';
    base->website[7] = 'p';
    base->website[8] = 'l';
    base->website[9] = '.';
    base->website[10] = 'c';
    base->website[11] = 'o';
    base->website[12] = 'm';
    base->website[13] = '\0';
    base->has_website = true;
    
    GitHubProfile *override = profile_create();
    if (!override) {
        profile_free(base);
        return 1;
    }
    
    override->name[0] = 'T';
    override->name[1] = 'e';
    override->name[2] = '\0';
    override->has_name = true;
    
    override->bio[0] = 'J';
    override->bio[1] = 'o';
    override->bio[2] = 'b';
    override->bio[3] = 'n';
    override->bio[4] = '\0';
    override->has_bio = true;
    
    // Override only changes bio and name, keeps everything else from base
    
    GitHubProfile *merged = profile_merge(base, override);
    if (!merged) {
        profile_free(base);
        profile_free(override);
        return 1;
    }
    
    printf("Base Profile:\n");
    profile_print(base);
    printf("\nOverride Profile:\n");
    profile_print(override);
    printf("\nMerged Profile:\n");
    profile_print(merged);
    
    // Cleanup
    profile_free(base);
    profile_free(override);
    profile_free(merged);
    
    return 0;
}