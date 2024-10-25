#include <iostream>
using namespace std;

// Colors for Red-Black Tree nodes
enum Color { RED, BLACK };

// Node structure for Red-Black Tree
struct Node {
    long long key;
    int count; // Handle duplicates
    Color color;
    Node *left, *right, *parent;

    Node(long long k)
        : key(k),
          count(1),
          color(RED),
          left(nullptr),
          right(nullptr),
          parent(nullptr) {}
};

class RedBlackTree {
   private:
    Node* root;
    Node* NIL; // Sentinel node

    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;

        if (y->left != NIL) y->left->parent = x;

        y->parent = x->parent;

        if (x->parent == NIL)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;

        y->left = x;
        x->parent = y;
    }

    void rightRotate(Node* y) {
        Node* x = y->left;
        y->left = x->right;

        if (x->right != NIL) x->right->parent = y;

        x->parent = y->parent;

        if (y->parent == NIL)
            root = x;
        else if (y == y->parent->right)
            y->parent->right = x;
        else
            y->parent->left = x;

        x->right = y;
        y->parent = x;
    }

    void fixInsert(Node* k) {
        Node* u;
        while (k->parent->color == RED) {
            if (k->parent == k->parent->parent->right) {
                u = k->parent->parent->left;
                if (u->color == RED) {
                    u->color = BLACK;
                    k->parent->color = BLACK;
                    k->parent->parent->color = RED;
                    k = k->parent->parent;
                } else {
                    if (k == k->parent->left) {
                        k = k->parent;
                        rightRotate(k);
                    }
                    k->parent->color = BLACK;
                    k->parent->parent->color = RED;
                    leftRotate(k->parent->parent);
                }
            } else {
                u = k->parent->parent->right;
                if (u->color == RED) {
                    u->color = BLACK;
                    k->parent->color = BLACK;
                    k->parent->parent->color = RED;
                    k = k->parent->parent;
                } else {
                    if (k == k->parent->right) {
                        k = k->parent;
                        leftRotate(k);
                    }
                    k->parent->color = BLACK;
                    k->parent->parent->color = RED;
                    rightRotate(k->parent->parent);
                }
            }
            if (k == root) break;
        }
        root->color = BLACK;
    }

    int countInRange(Node* node, long long low, long long high) const {
        if (node == NIL) return 0;

        if (node->key < low) return countInRange(node->right, low, high);
        if (node->key > high) return countInRange(node->left, low, high);

        return node->count + countInRange(node->left, low, high) +
               countInRange(node->right, low, high);
    }

   public:
    RedBlackTree() {
        NIL = new Node(0);
        NIL->color = BLACK;
        root = NIL;
    }

    void insert(long long key) {
        Node* newNode = new Node(key);
        newNode->left = NIL;
        newNode->right = NIL;

        Node* y = NIL;
        Node* x = root;

        while (x != NIL) {
            y = x;
            if (key == x->key) {
                x->count++;
                delete newNode;
                return;
            }
            if (key < x->key)
                x = x->left;
            else
                x = x->right;
        }

        newNode->parent = y;
        if (y == NIL)
            root = newNode;
        else if (key < y->key)
            y->left = newNode;
        else
            y->right = newNode;

        fixInsert(newNode);
    }

    int countRangeSum(long long low, long long high) const {
        return countInRange(root, low, high);
    }
};

int countRangeSums(const int arr[], int n, int lower, int upper) {
    // Compute prefix sums
    long long prefixSum = 0;
    RedBlackTree rbtree;
    rbtree.insert(0); // Initialize with 0 for empty subarray
    int count = 0;

    for (int i = 0; i < n; i++) {
        prefixSum += arr[i];
        // Count subarrays ending at index i
        count += rbtree.countRangeSum(prefixSum - upper, prefixSum - lower);
        rbtree.insert(prefixSum);
    }

    return count;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    string line;
    // Read first line containing array
    getline(cin, line);

    // Parse array
    int arr[100000]; // Max size according to constraints
    int n = 0;
    size_t pos = 0;
    while (pos < line.length()) {
        while (pos < line.length() && (line[pos] == ' ' || line[pos] == '\t'))
            pos++;
        if (pos >= line.length()) break;

        bool negative = false;
        if (line[pos] == '-') {
            negative = true;
            pos++;
        }

        int num = 0;
        while (pos < line.length() && line[pos] >= '0' && line[pos] <= '9') {
            num = num * 10 + (line[pos] - '0');
            pos++;
        }
        arr[n++] = negative ? -num : num;
    }

    // Read second line containing lower and upper bounds
    int lower, upper;
    cin >> lower >> upper;

    // Calculate and output result
    cout << countRangeSums(arr, n, lower, upper) << endl;

    return 0;
}