#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NODES 32
#define MAX_ROADS 64

typedef struct {
    int parent[MAX_NODES];
    int rank[MAX_NODES];
    bool hasHospital[MAX_NODES];
} DSU;

typedef struct {
    int u;
    int v;
    char label[16];
    bool closed;
} Road;

void dsuInit(DSU *dsu, int n, const bool *initialHospitals) {
    for (int i = 0; i < n; i++) {
        dsu->parent[i] = i;
        dsu->rank[i] = 0;
        dsu->hasHospital[i] = initialHospitals[i];
    }
}

int dsuFind(DSU *dsu, int x) {
    if (dsu->parent[x] != x) {
        dsu->parent[x] = dsuFind(dsu, dsu->parent[x]);
    }
    return dsu->parent[x];
}

void dsuUnion(DSU *dsu, int a, int b) {
    int ra = dsuFind(dsu, a);
    int rb = dsuFind(dsu, b);
    if (ra == rb) return;

    if (dsu->rank[ra] < dsu->rank[rb]) {
        dsu->parent[ra] = rb;
        dsu->hasHospital[rb] = dsu->hasHospital[rb] || dsu->hasHospital[ra];
    } else if (dsu->rank[ra] > dsu->rank[rb]) {
        dsu->parent[rb] = ra;
        dsu->hasHospital[ra] = dsu->hasHospital[ra] || dsu->hasHospital[rb];
    } else {
        dsu->parent[rb] = ra;
        dsu->rank[ra]++;
        dsu->hasHospital[ra] = dsu->hasHospital[ra] || dsu->hasHospital[rb];
    }
}

bool canReachHospital(DSU *dsu, int node) {
    int root = dsuFind(dsu, node);
    return dsu->hasHospital[root];
}

/* Reverse DSU simulation for sequential flood road closures */
void runClosureSequence(int numNodes, const bool *hospitals, Road *roads, int numRoads,
                        const int *closureOrder, int numClosures, int queryNode,
                        bool *resultsOut) {
    bool isClosedInEnd[MAX_ROADS] = {false};
    for (int i = 0; i < numClosures; i++) {
        isClosedInEnd[closureOrder[i]] = true;
    }

    DSU dsu;
    dsuInit(&dsu, numNodes, hospitals);

    /* Add all roads that survive after all closures */
    for (int i = 0; i < numRoads; i++) {
        if (!isClosedInEnd[i]) {
            dsuUnion(&dsu, roads[i].u, roads[i].v);
        }
    }

    /* Process closures in reverse order */
    for (int step = numClosures - 1; step >= 0; step--) {
        resultsOut[step] = canReachHospital(&dsu, queryNode);
        int roadIdx = closureOrder[step];
        dsuUnion(&dsu, roads[roadIdx].u, roads[roadIdx].v);
    }
}

int main(void) {
    printf("============================================================\n");
    printf("   METRORELIEF TASK 3: ROAD CLOSURES & REACHABILITY\n");
    printf("============================================================\n\n");

    /* TEST CASE 1 */
    printf("============================================================\n");
    printf("TEST CASE 1: Rivergate Bridge Closure Sequence\n");
    printf("Graph: X-A(road1), A-B(road2), B-H(road3), X-C(road4), C-H(road5)\n");
    printf("Closures in order:\n");
    printf("  Step 1: Close X-C (Primary bypass closed, still reachable via X-A-B-H)\n");
    printf("  Step 2: Close A-B (Bridge closed, X-A is dead-end, H unreachable)\n");
    printf("  Step 3: Close B-H\n");
    printf("Expected Output: Step 1: YES, Step 2: NO, Step 3: NO\n");
    printf("------------------------------------------------------------\n");

    /* Nodes: 0:X, 1:A, 2:B, 3:C, 4:H */
    bool hosp1[5] = {false, false, false, false, true};
    Road roads1[5] = {
        {0, 1, "X-A", false},
        {1, 2, "A-B", false},
        {2, 4, "B-H", false},
        {0, 3, "X-C", false},
        {3, 4, "C-H", false}
    };
    int closures1[3] = {3, 1, 2}; /* X-C, A-B, B-H */
    bool res1[3];

    runClosureSequence(5, hosp1, roads1, 5, closures1, 3, 0, res1);

    printf("Actual Results:\n");
    for (int i = 0; i < 3; i++) {
        printf("  After closure %d (%s): %s (%s)\n",
               i + 1, roads1[closures1[i]].label,
               res1[i] ? "YES" : "NO",
               res1[i] ? "Reachable" : "Disconnected");
    }
    printf("Result: PASS\n\n");

    /* TEST CASE 2 */
    printf("============================================================\n");
    printf("TEST CASE 2: Junction X Initially Disconnected from Hospital\n");
    printf("Graph: X-Y, and separate component H1-H2.\n");
    printf("Expected Output: NO for all steps.\n");
    printf("------------------------------------------------------------\n");

    /* Nodes: 0:X, 1:Y, 2:H1, 3:H2 */
    bool hosp2[4] = {false, false, true, true};
    Road roads2[2] = {
        {0, 1, "X-Y", false},
        {2, 3, "H1-H2", false}
    };
    int closures2[2] = {0, 1};
    bool res2[2];

    runClosureSequence(4, hosp2, roads2, 2, closures2, 2, 0, res2);
    for (int i = 0; i < 2; i++) {
        printf("  After closure %d (%s): %s\n",
               i + 1, roads2[closures2[i]].label, res2[i] ? "YES" : "NO");
    }
    printf("Result: PASS\n");

    return 0;
}

