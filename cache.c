#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

typedef struct Line {
    unsigned char valid;
    unsigned long tag;
    unsigned long hitCount;
    unsigned long freqCount;
}Line;

typedef struct Set {
    Line* lines;
}Set;

int main() {
    int S = 0;
    int E = 0;
    int B = 0;
    int m = 0;
    unsigned long s;
    unsigned long b;
    unsigned long t;
    int LFU = 0;
    int LRU = 0;
    int hitTime = 0;
    int missPenalty = 0;
    unsigned int lookupAddress = 0;
    unsigned int s_mask;
    unsigned int t_mask;
    unsigned int cyclecount = 0;
    int numMisses = 0;
    char *p = (char *) malloc(sizeof(char) * 4);
    unsigned int *addresses = (int *) malloc(sizeof(int) * 100);

    printf("Enter input block\n");
    scanf( "%d\n", &S);
    s = (unsigned long)log2(S);

    scanf("%d%d%d%s%d%d\n", &E, &B, &m, p, &hitTime, &missPenalty);
    b = (unsigned long)log2(B);

    // calculate using the formula
    t = m - (b + s);

    // making masks
    s_mask = ~((~(unsigned long)0) << s);
    t_mask = ~((~(unsigned long)0) << t);

    Set* cache = calloc(S, sizeof(Set)); // allocating array of sets

    int i;
    // allocating lines in sets
    for (i = 0; i < S; i++) {
        Line* line = calloc(E, sizeof(Line));
        cache[i].lines = line;
    }

    if(strcmp(p,"LRU")){
        LRU = 1;
    }
    else if(strcmp(p,"LFU")) {
        LFU = 1;
    }

    // loop to collect till -1
    int count = 0;
    scanf("%x",&lookupAddress);
    while(lookupAddress != 0xffffffff){     // 0xffffffff = -1
        addresses[count] = lookupAddress;
        count++;
        scanf("%x",&lookupAddress);

    }
    int j;
    for(j=0; j< count; j++){
        unsigned long lookupSet = (addresses[j] >> b)& s_mask;
        unsigned long lookupTag = (addresses[j] >> (b + s))& t_mask;
        int hit = 0;
        int line;
        // check if hit
        for (line = 0; line < E; line++) {
            if ((cache[lookupSet].lines[line].valid == 1) &&
                cache[lookupSet].lines[line].tag == lookupTag) {
                //cache hit
                printf("%x H\n", addresses[j]);
                cyclecount += hitTime;
                if (LFU) { cache[lookupSet].lines[line].hitCount += 1; }
                else if (LRU) { cache[lookupSet].lines[line].freqCount = cyclecount; }
                hit = 1;
                break;
            }
        }
        int store = 0;
        // check miss
        if (!hit) {
            printf("%x M\n", addresses[j]);
            numMisses++;
            cyclecount += hitTime + missPenalty;

            for (line = 0; line < E; line++) {
                if (cache[lookupSet].lines[line].valid == 0) {
                    cache[lookupSet].lines[line].valid = 1;
                    cache[lookupSet].lines[line].hitCount = 0;
                    cache[lookupSet].lines[line].freqCount = 0;
                    cache[lookupSet].lines[line].tag = lookupTag;
                    if (LRU) {
                        cache[lookupSet].lines[line].freqCount = cyclecount;
                    }
                    store = 1;
                    break;
                }
            }

            if (!store) {
                // if not check policy
                int lineToEvict = 0;
                for (line = 0; line < E; line++) {
                    if (LRU && cache[lookupSet].lines[line].freqCount < cache[lookupSet].lines[lineToEvict].freqCount
                        || LFU && cache[lookupSet].lines[line].hitCount < cache[lookupSet].lines[lineToEvict].hitCount){
                        lineToEvict = line;
                    }
                }
                cache[lookupSet].lines[lineToEvict].valid = 1;
                cache[lookupSet].lines[lineToEvict].hitCount = 0;
                if (LRU) {
                    cache[lookupSet].lines[lineToEvict].freqCount = cyclecount;
                }
                cache[lookupSet].lines[lineToEvict].tag = lookupTag;
            }
        }
    }

    float missRate = ((float)numMisses / (float)count) * 100;
    printf("%.f %d\n", missRate, cyclecount);

    // freeing memory
    int k;
    for (k = 0; k < S; k++) {
        free(cache[k].lines);
        cache[k].lines = NULL;
    }
    free(p);
    free(addresses);
    free(cache);
    cache = NULL;

    return 0;
}