#include <bits/stdc++.h>

using namespace std;

class BNode;

class BTreeNode
{
public:
    int data;
    BNode *child;
};

class BNode
{
public:
    BNode *leftChild;
    bool isIndex;
    vector<BTreeNode *> keys;
};

class BPlusTree
{
public:
    BPlusTree();
    virtual ~BPlusTree();
    BNode *getRoot();
    void rearrange(BNode *);
    BTreeNode *splitDataNode(BNode *, int);
    void splitIndexNode(BNode *, int);
    void insertNode(int, int, int);
    int countIndexNodes(BNode *);
    int countDataNodes(BNode *);
    void print(BNode *);
    void display();

protected:
private:
    BNode *root;
};

int main()
{
    int d, t;
    cin >> d >> t;

    BPlusTree Tree;

    int n;
    while (cin >> n)
    {
        if (n == 1)
        {
            // insert
            int val;
            cin >> val;

            Tree.insertNode(val, d, t);
        }
        else if (n == 2)
        {
            // print
            BNode *r = Tree.getRoot();
            Tree.display();
            // Tree.print(r);
        }
        else if (n == 3)
            break;
    }

    return 0;
}

BPlusTree::BPlusTree()
{
    root = NULL;
}

BNode *BPlusTree::getRoot()
{
    return root;
}

void BPlusTree::rearrange(BNode *node)
{
    for (int i = node->keys.size() - 1; i > 0; i--)
    {
        if (node->keys[i]->data < node->keys[i - 1]->data)
        {
            // swap
            BTreeNode *p = node->keys[i];
            node->keys[i] = node->keys[i - 1];
            node->keys[i - 1] = p;
        }
    }

    return;
}

BTreeNode *BPlusTree::splitDataNode(BNode *node, int d)
{
    BNode *rightNode = new BNode();
    rightNode->isIndex = false;

    // d keys in leftNode and d+1 keys in rightNode
    BNode *leftNode = node;
    for (int i = d; i < leftNode->keys.size(); i++)
    {
        rightNode->keys.push_back(leftNode->keys[i]);
    }

    for (int i = 0; i < d + 1; i++)
        leftNode->keys.pop_back();

    int d1 = rightNode->keys[0]->data;
    BTreeNode *nodeptr1 = new BTreeNode();
    nodeptr1->data = d1;
    nodeptr1->child = rightNode;

    return nodeptr1;
}

void BPlusTree::splitIndexNode(BNode *node, int t)
{
    BTreeNode *mid = node->keys[t];

    // Finding parent of current node
    BNode *p = root;
    BNode *par = NULL;
    while (p != node)
    {
        par = p;
        int s = p->keys.size();
        for (int i = 0; i < s; i++)
        {
            if (p->keys[i]->data >= mid->data)
            {
                if (i > 0)
                    p = p->keys[i - 1]->child;
                else
                    p = p->leftChild;

                break;
            }

            if (i == s - 1)
            {
                p = p->keys[i]->child;
            }
        }
    }

    BNode *rightNode = new BNode();
    rightNode->leftChild = mid->child;
    rightNode->isIndex = true;

    // t keys in leftNode and t+1 keys in rightNode
    BNode *leftNode = node;
    for (int i = t + 1; i < leftNode->keys.size(); i++)
    {
        rightNode->keys.push_back(leftNode->keys[i]);
    }

    for (int i = 0; i < t + 2; i++)
        leftNode->keys.pop_back();

    mid->child = rightNode;

    // creating new root node
    if (par == NULL)
    {
        par = new BNode();
        par->leftChild = leftNode;
        par->isIndex = true;
        par->keys.push_back(mid);
        root = par;

        return;
    }

    // inserting into parent node
    par->keys.push_back(mid);
    rearrange(par);

    // parent node is not full
    if (par->keys.size() <= (2 * t + 1))
        return;

    // parent node is full
    splitIndexNode(par, t);

    return;
}

void BPlusTree::insertNode(int val, int d, int t)
{
    // Creating new tree node
    BTreeNode *nodeptr = new BTreeNode();
    nodeptr->data = val;
    nodeptr->child = NULL;

    if (root == NULL)
    {
        // Creating first data node
        BNode *node = new BNode();
        node->leftChild = NULL;
        node->isIndex = false;
        node->keys.push_back(nodeptr);

        rearrange(node);
        root = node;

        return;
    }

    if (root->isIndex == false)
    {
        // node is not full
        if (root->keys.size() < (2 * d))
        {
            root->keys.push_back(nodeptr);
            rearrange(root);

            return;
        }

        // node is full
        root->keys.push_back(nodeptr);
        rearrange(root);

        BTreeNode *nodeptr1 = splitDataNode(root, d);

        BNode *indexNode = new BNode();
        indexNode->leftChild = root;
        indexNode->isIndex = true;

        indexNode->keys.push_back(nodeptr1);
        root = indexNode;

        return;
    }

    if (root->isIndex == true)
    {
        BNode *p = root;
        BNode *index = NULL;

        // Descending to data node
        while (p->isIndex != false)
        {
            index = p;
            int s = p->keys.size();
            for (int i = 0; i < s; i++)
            {
                if (p->keys[i]->data >= val)
                {
                    if (i > 0)
                    {
                        p = p->keys[i - 1]->child;
                    }
                    else
                    {
                        p = p->leftChild;
                    }
                    break;
                }

                if (i == s - 1)
                {
                    p = p->keys[i]->child;
                }
            }
        }

        // data node is not full
        if (p->keys.size() < (2 * d))
        {
            p->keys.push_back(nodeptr);
            rearrange(p);

            return;
        }

        // data node is full
        p->keys.push_back(nodeptr);
        rearrange(p);

        BTreeNode *nodeptr1 = splitDataNode(p, d);

        // inserting into index node
        index->keys.push_back(nodeptr1);
        rearrange(index);

        // index node is not full
        if (index->keys.size() <= (2 * t + 1))
            return;

        // index node is full
        splitIndexNode(index, t);
    }

    return;
}

int BPlusTree::countIndexNodes(BNode *node)
{
    if (node == NULL)
        return 0;

    if (node->isIndex == false)
        return 0;

    int k = 1;
    k += countIndexNodes(node->leftChild);
    for (int i = 0; i < node->keys.size(); i++)
    {
        k += countIndexNodes(node->keys[i]->child);
    }

    return k;
}

int BPlusTree::countDataNodes(BNode *node)
{
    if (node == NULL)
        return 0;

    if (node->isIndex == false)
        return 1;

    int k = 0;
    k += countDataNodes(node->leftChild);
    for (int i = 0; i < node->keys.size(); i++)
    {
        k += countDataNodes(node->keys[i]->child);
    }

    return k;
}

void BPlusTree::display()
{
    if (root == NULL)
        return;

    cout << countIndexNodes(root) << " " << countDataNodes(root) << " ";
    for (int i = 0; i < root->keys.size(); i++)
    {
        cout << root->keys[i]->data << " ";
    }
    cout << endl;

    return;
}

void BPlusTree::print(BNode *node)
{
    if (node == NULL)
        return;

    cout << "isIndexNode: " << node->isIndex << " Keys: ";
    for (int i = 0; i < node->keys.size(); i++)
    {
        cout << node->keys[i]->data << " ";
    }
    cout << endl;

    print(node->leftChild);
    for (int i = 0; i < node->keys.size(); i++)
    {
        print(node->keys[i]->child);
    }

    return;
}

BPlusTree::~BPlusTree()
{
}
