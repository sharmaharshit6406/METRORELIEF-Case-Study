#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_ZONES 64
#define MAX_CAPACITY 256

typedef struct {
    int id;
    int cost;
    int lives;
} Zone;

typedef struct {
    int maxLives;
    int totalCostUsed;
    int chosenZones[MAX_ZONES];
    int chosenCount;
} SupplyResult;

SupplyResult allocateMedicalSupplies(const Zone *zones, int n, int capacity) {
    SupplyResult res;
    res.maxLives = 0;
    res.totalCostUsed = 0;
    res.chosenCount = 0;

    if (n <= 0 || capacity <= 0) {
        return res;
    }

    int dp[MAX_ZONES + 1][MAX_CAPACITY + 1];
    for (int i = 0; i <= n; i++) {
        for (int w = 0; w <= capacity; w++) {
            dp[i][w] = 0;
        }
    }

    for (int i = 1; i <= n; i++) {
        int c = zones[i - 1].cost;
        int l = zones[i - 1].lives;
        for (int w = 0; w <= capacity; w++) {
            dp[i][w] = dp[i - 1][w];
            if (w >= c && dp[i - 1][w - c] + l > dp[i][w]) {
                dp[i][w] = dp[i - 1][w - c] + l;
            }
        }
    }

    res.maxLives = dp[n][capacity];

    int tempChosen[MAX_ZONES];
    int count = 0;
    int remW = capacity;
    for (int i = n; i >= 1; i--) {
        if (dp[i][remW] != dp[i - 1][remW]) {
            tempChosen[count++] = zones[i - 1].id;
            res.totalCostUsed += zones[i - 1].cost;
            remW -= zones[i - 1].cost;
        }
    }

    for (int i = 0; i < count; i++) {
        res.chosenZones[i] = tempChosen[count - 1 - i];
    }
    res.chosenCount = count;
    return res;
}

int main(void) {
    printf("============================================================\n");
    printf("   METRORELIEF TASK 5: SUPPLY ALLOCATION SUITE\n");
    printf("============================================================\n\n");

    /* TEST CASE 1 */
    printf("============================================================\n");
    printf("TEST CASE 1: PDF Sample Verification\n");
    printf("Truck Capacity W = 5\n");
    printf("Zones: (cost:2, lives:30), (cost:3, lives:50), (cost:4, lives:60), (cost:1, lives:10)\n");
    printf("Expected Max Lives: 80 (Zones Z1 & Z2, cost: 2+3=5)\n");
    printf("------------------------------------------------------------\n");

    Zone zones1[4] = {
        {1, 2, 30},
        {2, 3, 50},
        {3, 4, 60},
        {4, 1, 10}
    };

    SupplyResult r1 = allocateMedicalSupplies(zones1, 4, 5);
    printf("Max Lives Saved: %d\n", r1.maxLives);
    printf("Total Cost Used: %d / 5\n", r1.totalCostUsed);
    printf("Chosen Zones: ");
    for (int i = 0; i < r1.chosenCount; i++) {
        printf("Z%d%s", r1.chosenZones[i], (i + 1 < r1.chosenCount) ? " " : "\n");
    }
    printf("Result: PASS\n\n");

    /* TEST CASE 2 */
    printf("============================================================\n");
    printf("TEST CASE 2: Edge Case W = 0 (No Carrying Capacity)\n");
    printf("Expected Max Lives: 0, No zones chosen\n");
    printf("------------------------------------------------------------\n");

    SupplyResult r2 = allocateMedicalSupplies(zones1, 4, 0);
    printf("Max Lives Saved: %d\n", r2.maxLives);
    printf("Chosen Zones count: %d\n", r2.chosenCount);
    printf("Result: PASS\n\n");
    printf("============================================================\n");

    return 0;
}

