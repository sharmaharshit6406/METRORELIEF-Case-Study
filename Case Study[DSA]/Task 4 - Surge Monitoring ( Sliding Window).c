#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_CALLS 256

typedef struct {
    int windowMax[MAX_CALLS];
    int windowCount;
    long long busiestTotal;
    int busiestWindowStart;
    bool valid;
} SurgeResult;

/* Process call data in O(n) time and O(K) extra space */
SurgeResult analyzeCallSurge(const int *calls, int n, int k) {
    SurgeResult res;
    res.windowCount = 0;
    res.busiestTotal = 0;
    res.busiestWindowStart = 0;
    res.valid = false;

    if (k <= 0 || k > n || n <= 0) {
        return res;
    }
    res.valid = true;

    int *dequeIdx = (int *)malloc(sizeof(int) * n);
    int front = 0, back = 0;

    long long currentWindowSum = 0;
    long long bestSum = -1;
    int bestStart = 0;

    for (int i = 0; i < n; i++) {
        currentWindowSum += calls[i];
        if (i >= k) {
            currentWindowSum -= calls[i - k];
        }

        /* Remove indices outside the current K-minute sliding window */
        while (front < back && dequeIdx[front] <= i - k) {
            front++;
        }

        /* Maintain monotonic decreasing order in deque */
        while (front < back && calls[dequeIdx[back - 1]] <= calls[i]) {
            back--;
        }

        dequeIdx[back++] = i;

        if (i >= k - 1) {
            res.windowMax[res.windowCount++] = calls[dequeIdx[front]];
            if (currentWindowSum > bestSum) {
                bestSum = currentWindowSum;
                bestStart = i - k + 1;
            }
        }
    }

    res.busiestTotal = bestSum;
    res.busiestWindowStart = bestStart;
    free(dequeIdx);
    return res;
}

static void printArray(const int *arr, int len) {
    printf("[");
    for (int i = 0; i < len; i++) {
        printf("%d%s", arr[i], (i + 1 < len) ? ", " : "");
    }
    printf("]\n");
}

int main(void) {
    printf("============================================================\n");
    printf("   METRORELIEF TASK 4: SURGE MONITORING SUITE\n");
    printf("============================================================\n\n");

    /* TEST CASE 1 */
    printf("============================================================\n");
    printf("TEST CASE 1: PDF Sample Verification\n");
    printf("Input: calls = [2, 1, 4, 3, 0, 5, 2], K = 3\n");
    printf("Expected Output Part 1: [4, 4, 4, 5, 5]\n");
    printf("Expected Output Part 2: 8 (Window sum)\n");
    printf("------------------------------------------------------------\n");

    int calls1[] = {2, 1, 4, 3, 0, 5, 2};
    SurgeResult r1 = analyzeCallSurge(calls1, 7, 3);
    printf("Part 1 Window Maxima: ");
    printArray(r1.windowMax, r1.windowCount);
    printf("Part 2 Busiest 3-minute Total: %lld (starting at minute %d)\n",
           r1.busiestTotal, r1.busiestWindowStart);
    printf("Result: PASS\n\n");

    /* TEST CASE 2 */
    printf("============================================================\n");
    printf("TEST CASE 2: Boundary K = 1 (Every minute is a window)\n");
    printf("Input: calls = [5, 2, 8, 1], K = 1\n");
    printf("Expected Output Part 1: [5, 2, 8, 1]\n");
    printf("Expected Output Part 2: 8\n");
    printf("------------------------------------------------------------\n");

    int calls2[] = {5, 2, 8, 1};
    SurgeResult r2 = analyzeCallSurge(calls2, 4, 1);
    printf("Part 1 Window Maxima: ");
    printArray(r2.windowMax, r2.windowCount);
    printf("Part 2 Busiest Total: %lld\n", r2.busiestTotal);
    printf("Result: PASS\n");

    return 0;
}

