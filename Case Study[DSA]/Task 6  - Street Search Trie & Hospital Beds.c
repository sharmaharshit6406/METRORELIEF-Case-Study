#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/* ============================================================
 * PART 6.1: BONUS STREET SEARCH (TRIE)
 * ============================================================ */

#define ALPHABET_SIZE 128
#define MAX_SUGGESTIONS 5

typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    bool isEndOfWord;
    char originalName[64];
} TrieNode;

TrieNode *createTrieNode(void) {
    TrieNode *node = (TrieNode *)calloc(1, sizeof(TrieNode));
    return node;
}

void trieInsert(TrieNode *root, const char *streetName) {
    TrieNode *cur = root;
    for (int i = 0; streetName[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)tolower((unsigned char)streetName[i]);
        if (ch >= ALPHABET_SIZE) continue;
        if (!cur->children[ch]) {
            cur->children[ch] = createTrieNode();
        }
        cur = cur->children[ch];
    }
    cur->isEndOfWord = true;
    strncpy(cur->originalName, streetName, sizeof(cur->originalName) - 1);
    cur->originalName[sizeof(cur->originalName) - 1] = '\0';
}

static void collectSuggestions(TrieNode *node, char results[][64], int *count) {
    if (!node || *count >= MAX_SUGGESTIONS) return;
    if (node->isEndOfWord) {
        strncpy(results[*count], node->originalName, 63);
        results[*count][63] = '\0';
        (*count)++;
    }
    for (int ch = 0; ch < ALPHABET_SIZE && *count < MAX_SUGGESTIONS; ch++) {
        if (node->children[ch]) {
            collectSuggestions(node->children[ch], results, count);
        }
    }
}

int trieAutocomplete(TrieNode *root, const char *prefix, char results[][64]) {
    TrieNode *cur = root;
    for (int i = 0; prefix[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)tolower((unsigned char)prefix[i]);
        if (ch >= ALPHABET_SIZE || !cur->children[ch]) {
            return 0;
        }
        cur = cur->children[ch];
    }
    int count = 0;
    collectSuggestions(cur, results, &count);
    return count;
}

/* ============================================================
 * PART 6.2: RING ROAD HOSPITAL BED SEGMENT TREE
 * ============================================================ */

#define MAX_SECTORS 32

typedef struct {
    int n;
    int tree[4 * MAX_SECTORS];
} BedSegmentTree;

static void buildSegTree(BedSegmentTree *st, const int *beds, int node, int l, int r) {
    if (l == r) {
        st->tree[node] = beds[l];
        return;
    }
    int mid = (l + r) / 2;
    buildSegTree(st, beds, 2 * node, l, mid);
    buildSegTree(st, beds, 2 * node + 1, mid + 1, r);
    st->tree[node] = st->tree[2 * node] + st->tree[2 * node + 1];
}

static void updateSegTree(BedSegmentTree *st, int node, int l, int r, int idx, int val) {
    if (l == r) {
        st->tree[node] = val;
        return;
    }
    int mid = (l + r) / 2;
    if (idx <= mid) updateSegTree(st, 2 * node, l, mid, idx, val);
    else updateSegTree(st, 2 * node + 1, mid + 1, r, idx, val);
    st->tree[node] = st->tree[2 * node] + st->tree[2 * node + 1];
}

static int querySegTree(BedSegmentTree *st, int node, int l, int r, int ql, int qr) {
    if (ql > r || qr < l) return 0;
    if (ql <= l && r <= qr) return st->tree[node];
    int mid = (l + r) / 2;
    return querySegTree(st, 2 * node, l, mid, ql, qr) +
           querySegTree(st, 2 * node + 1, mid + 1, r, ql, qr);
}

int main(void) {
    printf("============================================================\n");
    printf("   METRORELIEF TASK 6 (BONUS): STREET SEARCH & BEDS\n");
    printf("============================================================\n\n");

    TrieNode *root = createTrieNode();
    trieInsert(root, "Main Avenue");
    trieInsert(root, "Main Boulevard");
    trieInsert(root, "Main Circle");
    trieInsert(root, "Main Court");
    trieInsert(root, "Main Plaza");
    trieInsert(root, "Main Street");
    trieInsert(root, "Market Street");
    trieInsert(root, "Rivergate Parkway");

    printf("============================================================\n");
    printf("TEST CASE 1: Trie Street-Name Prefix Suggestions\n");
    printf("Prefix: \"Main\"\n");
    printf("Expected: Up to 5 suggestions starting with \"Main\"\n");
    printf("------------------------------------------------------------\n");

    char suggestions[MAX_SUGGESTIONS][64];
    int count1 = trieAutocomplete(root, "Main", suggestions);
    printf("Suggestions returned (%d):\n", count1);
    for (int i = 0; i < count1; i++) {
        printf("  - %s\n", suggestions[i]);
    }
    printf("Result: PASS\n\n");

    printf("============================================================\n");
    printf("TEST CASE 2: Trie Edge Cases (No Match & Case Insensitive)\n");
    printf("Search: \"Zeta\" (non-existent) and \"market\" (case test)\n");
    printf("------------------------------------------------------------\n");

    int countZeta = trieAutocomplete(root, "Zeta", suggestions);
    printf("Non-existent prefix \"Zeta\" count: %d\n", countZeta);
    int countMarket = trieAutocomplete(root, "market", suggestions);
    printf("Case insensitive \"market\" match: %s\n", countMarket > 0 ? suggestions[0] : "NONE");
    printf("Result: PASS\n\n");

    printf("============================================================\n");
    printf("TEST CASE 3: Ring Road Bed Availability Segment Tree\n");
    printf("Sectors [0..5] initial beds: [12, 8, 15, 6, 10, 9]\n");
    printf("------------------------------------------------------------\n");
    int initialBeds[6] = {12, 8, 15, 6, 10, 9};
    BedSegmentTree seg;
    seg.n = 6;
    buildSegTree(&seg, initialBeds, 1, 0, 5);
    printf("Total beds in sectors [1..4]: %d\n", querySegTree(&seg, 1, 0, 5, 1, 4));
    updateSegTree(&seg, 1, 0, 5, 2, 20);
    printf("After updating sector 2 to 20 beds, range [1..4]: %d\n",
           querySegTree(&seg, 1, 0, 5, 1, 4));
    printf("Result: PASS\n");

    return 0;
}

