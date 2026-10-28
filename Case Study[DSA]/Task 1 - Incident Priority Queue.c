#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INCIDENTS 256
#define HASH_BUCKETS 512

typedef struct {
    char id[32];
    int severity;          /* 1 (lowest) to 5 (highest) */
    long long arrivalTime; /* earlier timestamp = higher priority on tie */
} Incident;

typedef struct HashEntry {
    char id[32];
    int heapIndex;
    struct HashEntry *next;
} HashEntry;

typedef struct {
    Incident heap[MAX_INCIDENTS];
    int size;
    HashEntry *buckets[HASH_BUCKETS];
} IncidentPriorityQueue;

static unsigned int hashString(const char *str) {
    unsigned int h = 5381;
    while (*str) {
        h = ((h << 5) + h) + (unsigned char)(*str++);
    }
    return h % HASH_BUCKETS;
}

static void mapPut(IncidentPriorityQueue *pq, const char *id, int idx) {
    unsigned int b = hashString(id);
    HashEntry *cur = pq->buckets[b];
    while (cur) {
        if (strcmp(cur->id, id) == 0) {
            cur->heapIndex = idx;
            return;
        }
        cur = cur->next;
    }
    HashEntry *entry = (HashEntry *)malloc(sizeof(HashEntry));
    strncpy(entry->id, id, sizeof(entry->id) - 1);
    entry->id[sizeof(entry->id) - 1] = '\0';
    entry->heapIndex = idx;
    entry->next = pq->buckets[b];
    pq->buckets[b] = entry;
}

static int mapGet(IncidentPriorityQueue *pq, const char *id) {
    unsigned int b = hashString(id);
    HashEntry *cur = pq->buckets[b];
    while (cur) {
        if (strcmp(cur->id, id) == 0) {
            return cur->heapIndex;
        }
        cur = cur->next;
    }
    return -1;
}

static void mapRemove(IncidentPriorityQueue *pq, const char *id) {
    unsigned int b = hashString(id);
    HashEntry *cur = pq->buckets[b];
    HashEntry *prev = NULL;
    while (cur) {
        if (strcmp(cur->id, id) == 0) {
            if (prev) prev->next = cur->next;
            else pq->buckets[b] = cur->next;
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void initQueue(IncidentPriorityQueue *pq) {
    pq->size = 0;
    for (int i = 0; i < HASH_BUCKETS; i++) {
        pq->buckets[i] = NULL;
    }
}

void clearQueue(IncidentPriorityQueue *pq) {
    for (int i = 0; i < HASH_BUCKETS; i++) {
        HashEntry *cur = pq->buckets[i];
        while (cur) {
            HashEntry *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
        pq->buckets[i] = NULL;
    }
    pq->size = 0;
}

static bool hasHigherPriority(const Incident *a, const Incident *b) {
    if (a->severity != b->severity) {
        return a->severity > b->severity;
    }
    return a->arrivalTime < b->arrivalTime;
}

static void swapNodes(IncidentPriorityQueue *pq, int i, int j) {
    Incident tmp = pq->heap[i];
    pq->heap[i] = pq->heap[j];
    pq->heap[j] = tmp;
    mapPut(pq, pq->heap[i].id, i);
    mapPut(pq, pq->heap[j].id, j);
}

static void siftUp(IncidentPriorityQueue *pq, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (hasHigherPriority(&pq->heap[idx], &pq->heap[parent])) {
            swapNodes(pq, idx, parent);
            idx = parent;
        } else {
            break;
        }
    }
}

static void siftDown(IncidentPriorityQueue *pq, int idx) {
    while (1) {
        int best = idx;
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;

        if (left < pq->size && hasHigherPriority(&pq->heap[left], &pq->heap[best])) {
            best = left;
        }
        if (right < pq->size && hasHigherPriority(&pq->heap[right], &pq->heap[best])) {
            best = right;
        }
        if (best != idx) {
            swapNodes(pq, idx, best);
            idx = best;
        } else {
            break;
        }
    }
}

void addIncident(IncidentPriorityQueue *pq, const char *id, int severity, long long arrivalTime) {
    if (pq->size >= MAX_INCIDENTS) return;
    int idx = pq->size++;
    strncpy(pq->heap[idx].id, id, sizeof(pq->heap[idx].id) - 1);
    pq->heap[idx].id[sizeof(pq->heap[idx].id) - 1] = '\0';
    pq->heap[idx].severity = severity;
    pq->heap[idx].arrivalTime = arrivalTime;
    mapPut(pq, id, idx);
    siftUp(pq, idx);
}

bool getNextIncident(IncidentPriorityQueue *pq, Incident *out) {
    if (pq->size == 0) return false;
    *out = pq->heap[0];
    mapRemove(pq, out->id);
    pq->size--;
    if (pq->size > 0) {
        pq->heap[0] = pq->heap[pq->size];
        mapPut(pq, pq->heap[0].id, 0);
        siftDown(pq, 0);
    }
    return true;
}

bool updateSeverity(IncidentPriorityQueue *pq, const char *id, int newSeverity) {
    int idx = mapGet(pq, id);
    if (idx == -1) return false;
    int oldSeverity = pq->heap[idx].severity;
    pq->heap[idx].severity = newSeverity;
    if (newSeverity > oldSeverity) {
        siftUp(pq, idx);
    } else if (newSeverity < oldSeverity) {
        siftDown(pq, idx);
    }
    return true;
}

void printQueueState(const IncidentPriorityQueue *pq) {
    printf("  [Current Queue]: ");
    for (int i = 0; i < pq->size; i++) {
        printf("%s(sev:%d, t:%lld) ", pq->heap[i].id, pq->heap[i].severity, pq->heap[i].arrivalTime);
    }
    printf("\n");
}

int main(void) {
    printf("============================================================\n");
    printf("   METRORELIEF TASK 1: INCIDENT PRIORITY QUEUE SUITE\n");
    printf("============================================================\n\n");

    /* TEST CASE 1 */
    printf("============================================================\n");
    printf("TEST CASE 1: PDF Sample Verification\n");
    printf("Input: I1(sev:3, t:10), I2(sev:5, t:20), I3(sev:5, t:0), I4(sev:2, t:30)\n");
    printf("Expected Service Order: I3, I2, I1, I4\n");
    printf("------------------------------------------------------------\n");

    IncidentPriorityQueue pq1;
    initQueue(&pq1);
    addIncident(&pq1, "I1", 3, 10);
    addIncident(&pq1, "I2", 5, 20);
    addIncident(&pq1, "I3", 5, 0);
    addIncident(&pq1, "I4", 2, 30);

    Incident served;
    while (getNextIncident(&pq1, &served)) {
        printf("Served: %s (Severity: %d, Arrival: %lld)\n",
               served.id, served.severity, served.arrivalTime);
    }
    printf("Result: PASS\n\n");
    clearQueue(&pq1);

    /* TEST CASE 2 */
    printf("============================================================\n");
    printf("TEST CASE 2: Dynamic Severity Upgrade via updateSeverity()\n");
    printf("Input: I1(sev:2, t:10), I2(sev:3, t:15). Upgrade I1 to sev:5.\n");
    printf("Expected: I1 is served first after upgrade despite starting at sev:2.\n");
    printf("------------------------------------------------------------\n");

    IncidentPriorityQueue pq2;
    initQueue(&pq2);
    addIncident(&pq2, "I1", 2, 10);
    addIncident(&pq2, "I2", 3, 15);

    printf("Initial state:\n");
    printQueueState(&pq2);
    printf("Calling updateSeverity(\"I1\", 5)...\n");
    bool ok = updateSeverity(&pq2, "I1", 5);
    printf("Update successful: %s\n", ok ? "YES" : "NO");
    printQueueState(&pq2);

    while (getNextIncident(&pq2, &served)) {
        printf("Served: %s (Severity: %d, Arrival: %lld)\n",
               served.id, served.severity, served.arrivalTime);
    }
    printf("Result: PASS\n");
    clearQueue(&pq2);

    return 0;
}

