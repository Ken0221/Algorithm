#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum Color { RED, BLACK };

struct Node {
    long data;
    Node *left, *right, *parent;
    Color color;
};

class RBtree {
   private:
    Node *root;
    Node *Tnull; // null node, BLACK

    void left_rotate(Node *x) {
        Node *y = x->right;
        // let y's left child be x's right child
        x->right = y->left;
        if (y->left != Tnull) {
            y->left->parent = x;
        }
        // let y's parent be x's parent
        y->parent = x->parent;
        if (x->parent == Tnull) {
            root = y;
            y->parent = Tnull;
        } else if (x->parent->left == x) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        // x be y's left child
        y->left = x;
        x->parent = y;
    }

    void right_rotate(Node *x) {
        Node *y = x->left;
        // let y's right child be x's left child
        x->left = y->right;
        if (y->right != Tnull) {
            y->right->parent = x;
        }
        // let y's parent be x's parent
        y->parent = x->parent;
        if (x->parent == Tnull) { // if x is root, let y be root
            root = y;
            y->parent = Tnull;
        } else if (x->parent->right == x) {
            x->parent->right = y;
        } else {
            x->parent->left = y;
        }
        // x be y's right child
        y->right = x;
        x->parent = y;
    }

    void insert_fix(Node *z) {
        while (z->parent->color == RED) {
            // if z's parent is RED (do not allow two RED in a row)
            if (z->parent->parent->left == z->parent) {
                // z's parent is the left child of its grandparent
                Node *y = z->parent->parent->right; // y is z's uncle
                if (y->color == RED) {
                    // case 1, z's uncle is RED
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent; // continue to check z's grandparent
                } else {
                    if (z == z->parent->right) {
                        // case 2, y is BLACK and z is the right child
                        z = z->parent;
                        left_rotate(z); // make case 2 to case 3
                    }
                    // case 3, y is BLACK and z is the left child
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                    right_rotate(z);
                }
            } else { // if z's parent is the right child of its grandparent
                Node *y = z->parent->parent->left; // y is z's uncle
                if (y->color == RED) {
                    // case 1, z's uncle is RED
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent; // continue to check z's grandparent
                } else {
                    if (z == z->parent->left) {
                        // case 2, y is BLACK and z is the left child
                        z = z->parent;
                        right_rotate(z); // make case 2 to case 3
                    }
                    // case 3, y is BLACK and z is the right child
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                    left_rotate(z);
                }
            }
        }
        root->color = BLACK; // set root to BLACK, this is proerty 2 of RB tree
    }

   public:
    RBtree() {
        Tnull = new Node;
        Tnull->color = BLACK;
        root = Tnull;
        root->color = BLACK;
        root->left = Tnull;
        root->right = Tnull;
        root->parent = Tnull;
    }

    void insert(long n) { // equal: right
        Node *z = new Node;
        z->data = n;
        Node *y = Tnull;
        Node *x = root;
        while (x != Tnull) {
            y = x;
            if (z->data < x->data) {
                x = x->left;
            } else {
                x = x->right;
            }
        }
        // now, x is the node for z, y is the parent of z
        z->parent = y;

        if (y == Tnull) { // if root is null
            root = z;
        } else if (z->data < y->data) { // if z is smaller than y
            y->left = z;                // left node
        } else {                        // if z is bigger than y
            y->right = z;               // right node
        }
        // remenber to set the left and right child of z to null
        z->left = Tnull;
        z->right = Tnull;
        z->color = RED; // set all new node to RED

        insert_fix(z);
    }

    Node *lowerbound(long n) { // find the smallest Node b&e than n
        Node *x = root;
        Node *y = x;
        while (x != Tnull) {
            if (x->data < n) {
                y = x;
                x = x->right;
            } else {
                x = x->left;
            }
        }
        return y;
    }

    Node *upperbound(long n) { // find the biggest Node s&e than n
        Node *x = root;
        Node *y = x;
        // // cout << "root: " << root->data << endl;
        // // cout << "n: " << n << endl;
        while (x != Tnull) {
            if (x->data > n) {
                y = x;
                x = x->left;
            } else {
                x = x->right;
            }
        }
        return y;
    }

    Node *getRoot() { return root; }

    long getInoder(Node *now, long small, long big) {
        long n = 0;
        if (now != Tnull) {
            if (now->data >= small) n += getInoder(now->left, small, big);
            if (now->data >= small && now->data <= big) {
                n++;
            }
            if (now->data <= big) n += getInoder(now->right, small, big);
        }
        return n;
    }
};

long count_sub(vector<long> z, long lower, long upper) {
    // count the number of subarray that the sum is between lower and upper
    vector<long> prefix(0);
    prefix.push_back(0);
    // // cout << "prefix: " << prefix[0] << " ";
    for (long i = 0; i < z.size(); i++) {
        prefix.push_back(prefix[i] + z[i]);
        // // cout << prefix[i + 1] << " ";
    }
    // // cout << endl;

    long num = 0;
    RBtree t;
    t.insert(prefix[0]);

    for (long i = 0; i < z.size(); i++) { // right bound
        // // cout << "i: " << i << endl;
        long cur_sum = prefix[i + 1];
        // // cout << "cur_sum: " << cur_sum << endl;
        // cur_sum - prefix[j] <= upper, prefix[j] >= cur_sum - upper
        // find the smallest prefix[j] that let cur_sum - prefix[j] >= upper
        // // cout << "prefix[j] >= " << cur_sum - upper << endl;
        // Node *lowerbound = t.lowerbound(cur_sum - upper);
        // cur_sum - prefix[j] >= lower, prefix[j] <= cur_sum - lower
        // find the biggest prefix[j] that let cur_sum - prefix[j] >= lower
        // // cout << "prefix[j] <= " << cur_sum - lower << endl;
        // Node *upperbound = t.upperbound(cur_sum - lower);

        // // cout << "lowerbound: " << lowerbound->data << endl;
        // // cout << "upperbound: " << upperbound->data << endl;

        num += t.getInoder(t.getRoot(), cur_sum - upper, cur_sum - lower);
        // // cout << "      num: " << num << endl;
        // add the number of subarray that the sum is between lower and upper
        t.insert(cur_sum);
    }

    return num;
}

int main() {
    // long arr[100000];

    // int n = 0;

    // while (cin >> arr[n]) {
    //     n++;
    //     if (cin.get() == '\n') break;
    // }

    string n;
    getline(cin, n);
    vector<long> z;
    size_t pos = 0;
    while (pos < n.length()) {
        size_t next = n.find(' ', pos);
        if (next == string::npos) next = n.length();
        z.push_back(stoi(n.substr(pos, next - pos)));
        pos = next + 1;
    }

    long lower, upper;
    cin >> lower >> upper;

    cout << count_sub(z, lower, upper) << endl;
    // // cout << "finished." << endl;

    return 0;
}
