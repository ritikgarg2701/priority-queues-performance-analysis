#include <bits/stdc++.h>

using namespace std;

struct binomialNode
{
    int distance;
    int vertex;
    int degree;
    struct binomialNode *child, *sibling, *parent;
};

struct fiboNode
{
    int distance;
    int vertex;
    int rank;
    bool isMarked;
    struct fiboNode *parent;
    list<struct fiboNode *> children;
};

/*Extract minimum from heap implemented using array. The function has time complexity of O(N), because
 * the index of minValue is found by traversing the entire array*/

int extractMinArray(vector<int> &Q, int N)
{
    int minIndex;
    bool isFirst = true;
    for (int i = 1; i <= N; i++)
    {
        if (Q[i] != -1)
        { // Elements already extracted from the heap correspond to '-1'.
            if (isFirst)
            {
                minIndex = i;
                isFirst = false;
            }
            else
            {
                if (Q[i] < Q[minIndex])
                    minIndex = i;
            }
        }
    }
    return minIndex;
}

/*percolateUpBinary() is a recursive function, which compares the value of a node with
 * the value of it's parent and swap the values, in case min-heap property is not satisfied.
 * The function is then recursively called. Complexity : O(log N)
 */

void percolateUpBinary(vector<pair<int, int>> &Q, int indexOfKeys[], int k)
{
    if (k > 1)
    {
        int parent = k / 2;
        if (Q[parent].first > Q[k].first)
        {
            indexOfKeys[Q[parent].second] = k;
            indexOfKeys[Q[k].second] = parent;
            pair<int, int> temp = Q[k];
            Q[k] = Q[parent];
            Q[parent] = temp;
            percolateUpBinary(Q, indexOfKeys, parent);
        }
    }
}

/*percolateDownBinary() is a recursive function, which compares the value of a node
 * with value of it's children, and swap the values of the node with its smaller child,
 *  in case min-heap property is not satisfied. The function is then recursively called.
 * Complexity : O(log N)
 */

void percolateDownBinary(vector<pair<int, int>> &Q, int indexOfKeys[], int Qsize, int k)
{
    int left = 2 * k, right = 2 * k + 1; // indices of child vertices in a binary heap following '1'-based indexing
    int smallerChild;
    if (left <= Qsize && right <= Qsize)
    { // To check whether the calculated indices are within the bounds of binary heap
        if (Q[left].first <= Q[right].first)
            smallerChild = left;
        else
            smallerChild = right;
    }
    else if (left <= Qsize)
        smallerChild = left;
    else if (right <= Qsize)
        smallerChild = right;
    else
        return;
    if (Q[smallerChild].first < Q[k].first)
    {
        indexOfKeys[Q[smallerChild].second] = k;
        indexOfKeys[Q[k].second] = smallerChild;
        pair<int, int> temp = Q[k];
        Q[k] = Q[smallerChild];
        Q[smallerChild] = temp;
        percolateDownBinary(Q, indexOfKeys, Qsize, smallerChild);
    }
}

// combineNodes() helps in merging nodes, based on min-heap property, while performing union of two binomial heaps
void combineNodes(list<struct binomialNode *> &unionHeap, list<struct binomialNode *>::iterator &x, list<struct binomialNode *>::iterator &nextX)
{
    list<struct binomialNode *>::iterator temp;
    if ((*x)->distance <= (*nextX)->distance)
    {
        // Make nextX as child of x
        (*nextX)->parent = (*x);
        (*nextX)->sibling = (*x)->child;
        (*x)->child = (*nextX);
        (*x)->degree += 1;
        temp = nextX;
        nextX++;
        unionHeap.erase(temp);
    }
    else
    {
        // Make x as child of nextX
        (*x)->parent = (*nextX);
        (*x)->sibling = (*nextX)->child;
        (*nextX)->child = (*x);
        (*nextX)->degree += 1;
        temp = x;
        x = nextX;
        nextX++;
        unionHeap.erase(temp);
    }
}

// unionBinomial() merges two binomial heaps to form a union binomial heap.

list<struct binomialNode *> unionBinomial(list<struct binomialNode *> &heap1, list<struct binomialNode *> &heap2)
{
    list<struct binomialNode *>::iterator itr1 = heap1.begin();
    list<struct binomialNode *>::iterator itr2 = heap2.begin();
    list<struct binomialNode *> unionHeap;
    // Merging the nodes of the root lists of the two heaps in a single root list, in non-decreasing order of degrees
    while (itr1 != heap1.end() || itr2 != heap2.end())
    {
        if (itr1 != heap1.end() && itr2 != heap2.end())
        {
            if ((*itr1)->degree <= (*itr2)->degree)
            {
                unionHeap.push_back(*itr1);
                itr1++;
            }
            else
            {
                unionHeap.push_back(*itr2);
                itr2++;
            }
        }
        else if (itr1 != heap1.end())
        {
            unionHeap.push_back(*itr1);
            itr1++;
        }
        else
        {
            unionHeap.push_back(*itr2);
            itr2++;
        }
    }
    // Consolidating nodes, based on their degrees and distance values
    list<struct binomialNode *>::iterator it;
    list<struct binomialNode *>::iterator x, nextX, nextNextX;
    if (unionHeap.size() > 1)
    { // Atleast 2 nodes to perform consolidation
        x = unionHeap.begin();
        nextX = unionHeap.begin();
        nextX++;
        while (nextX != unionHeap.end())
        { // When nextX reaches end, one node remains in the heap, no more consolidation required
            if ((*x)->degree < (*nextX)->degree)
            {
                x++;
                nextX++; // Simply move ahead
            }
            else
            {
                // If degrees of x and nextX are equal, check the degree of nextNextX node(if present)
                nextNextX = nextX;
                nextNextX++;
                if (nextNextX != unionHeap.end())
                {
                    if ((*nextNextX)->degree != (*x)->degree)
                        combineNodes(unionHeap, x, nextX); // Consolidate the nodes based on min-heap property
                    else
                    { // move ahead
                        x = nextX;
                        nextX = nextNextX;
                    }
                }
                else // Only two nodes remaining in the heap, when nextNextNext reaches end.
                    combineNodes(unionHeap, x, nextX);
            }
        }
    }
    return unionHeap;
}

/*insertBinomial() inserts a new node in the binomial heap, by creating a binomial heap
 *on single node and calling union function to merge Q and the new heap.
 */
struct binomialNode *insertBinomial(list<struct binomialNode *> &Q, int dis, int v)
{
    struct binomialNode *newNode = new struct binomialNode;
    newNode->distance = dis;
    newNode->vertex = v;
    newNode->degree = 0;
    newNode->child = nullptr;
    newNode->parent = nullptr;
    newNode->sibling = nullptr;
    list<struct binomialNode *> newNodeList = {newNode}; // binomial heap with single node
    list<struct binomialNode *>::iterator it;
    Q = unionBinomial(Q, newNodeList);
    return newNode;
}

// getMinBinomial() traverses the root list of a binomial heap, to find the iterator to the minimum node
list<struct binomialNode *>::iterator getMinBinomial(list<struct binomialNode *> &Q)
{
    list<struct binomialNode *>::iterator itr, min = Q.begin();
    for (itr = ++Q.begin(); itr != Q.end(); itr++)
    {
        if ((*itr)->distance < (*min)->distance)
            min = itr;
    }
    return min;
}
/* extractMinBinomial() removes the minimum node returned by getMinBinomial(),
 * creates a new binomial heap of its child nodes and uses unionBinomial() to merge
 * the new binomial heap with the existing one.
 */
struct binomialNode *extractMinBinomial(list<struct binomialNode *> &Q)
{
    list<struct binomialNode *>::iterator minNodeIterator = getMinBinomial(Q);
    struct binomialNode *minNode = *minNodeIterator;
    list<struct binomialNode *> newHeap;
    struct binomialNode *temp;
    if (minNode->child != nullptr)
    {
        (minNode->child)->parent = nullptr;
        newHeap.push_back(minNode->child);
        temp = minNode->child;
        while (temp->sibling != nullptr)
        {
            (temp->sibling)->parent = nullptr;
            newHeap.push_back(temp->sibling);
            temp = temp->sibling;
        }
    }
    Q.erase(minNodeIterator);
    Q = unionBinomial(Q, newHeap);
    return minNode;
}

/* decreaseKeyBinomial() reduces the value of the node and compares with the value of the parent.
 * The values are swapped, if min-heap property is not satisfied. This is performed until we reach a
 * node which has a parent of lower value or we reach a node in the root list
 */
void decreaseKeyBinomial(list<struct binomialNode *> &Q, vector<struct binomialNode *> &pointers, int v, int decreasedValue)
{
    struct binomialNode *nodeToDecrease = pointers[v];
    nodeToDecrease->distance = decreasedValue;
    struct binomialNode *temp = nodeToDecrease;
    struct binomialNode *tempParent = temp->parent;
    struct binomialNode *t1;
    int t2, t3;
    while (tempParent != nullptr)
    { // not a node in the root list
        if (temp->distance < tempParent->distance)
        { // min-heap property is violated, values of the nodes are swapped
            t1 = pointers[tempParent->vertex];
            t2 = tempParent->distance;
            t3 = tempParent->vertex;
            pointers[tempParent->vertex] = pointers[temp->vertex];
            tempParent->distance = temp->distance;
            tempParent->vertex = temp->vertex;
            pointers[temp->vertex] = t1;
            temp->distance = t2;
            temp->vertex = t3;
            temp = tempParent;
            tempParent = tempParent->parent;
        }
        else // min-heap property is satisfied, process is terminated
            break;
    }
}

// insertFibonacci() creates a new node and inserts it in the root list
list<struct fiboNode *>::iterator insertFibonacci(list<struct fiboNode *> &Q, int dis, int v, list<struct fiboNode *>::iterator &minNode)
{
    struct fiboNode *newNode = new struct fiboNode;
    newNode->distance = dis;
    newNode->vertex = v;
    newNode->rank = 0;
    newNode->isMarked = false;
    newNode->children = list<struct fiboNode *>();
    newNode->parent = nullptr;
    if (Q.empty())
    { // first node in the heap
        Q.push_front(newNode);
        minNode = Q.begin();
    }
    else
    {
        Q.push_front(newNode);
        if (newNode->distance < (*minNode)->distance)
            minNode = Q.begin();
    }
    return Q.begin(); // returning the iterator of the newly inserted node.
}

/* extractMinFibonacci() deletes the minNode from the heap, pushes its children in the root list
 * and performs consolidation to merge nodes of same rank together.
 */
void extractMinFibonacci(list<struct fiboNode *> &Q, int totalNodes, vector<list<struct fiboNode *>::iterator> &pointers, list<struct fiboNode *>::iterator &minNode)
{ // Updating pointers array
    list<struct fiboNode *>::iterator itr, prevItr;
    if (!Q.empty())
    {
        if (!(*minNode)->children.empty())
        {
            // pushing the children of the minNode into the root list and updating the iterators pointing to them.
            for (itr = (*minNode)->children.begin(); itr != (*minNode)->children.end(); itr++)
            {
                (*itr)->parent = nullptr;
                (*itr)->isMarked = false;
                Q.push_front(*itr);
                pointers[(*itr)->vertex] = Q.begin();
            }
            (*minNode)->children.clear();
            Q.erase(minNode); // erasing the minNode from the list
            itr = Q.begin();
            double PHI = 1.6180339; // Golden ratio(φ)
            // Rank of fibonacci heaps ≤ logφ n
            int rankArrSize = ceil(log(totalNodes) / log(PHI)) + 5; // 5 is added to avoid any chances of error
            list<struct fiboNode *>::iterator rankArr[rankArrSize];
            vector<bool> isRankPresent(rankArrSize, false);
            struct fiboNode *temp;
            while (itr != Q.end())
            {
                if (!isRankPresent[(*itr)->rank])
                {
                    isRankPresent[(*itr)->rank] = true;
                    rankArr[(*itr)->rank] = itr;
                    itr++;
                }
                else
                {
                    prevItr = rankArr[(*itr)->rank]; // Already present iterator
                    if ((*itr)->distance <= (*prevItr)->distance)
                    {
                        // Make prevItr as child of itr
                        ((*itr)->children).push_front(*prevItr);
                        pointers[(*prevItr)->vertex] = ((*itr)->children).begin();
                        isRankPresent[(*itr)->rank] = false;
                        (*itr)->rank++;
                        Q.erase(prevItr);
                    }
                    else
                    {
                        // Make itr as child of prevItr
                        pointers[(*prevItr)->vertex] = itr;
                        pointers[(*itr)->vertex] = prevItr;
                        temp = (*itr);
                        (*itr) = (*prevItr);
                        (*prevItr) = temp;
                        ((*itr)->children).push_front(*prevItr);
                        pointers[(*prevItr)->vertex] = ((*itr)->children).begin();
                        isRankPresent[(*itr)->rank] = false;
                        (*itr)->rank++;
                        Q.erase(prevItr);
                    }
                }
            }
        }
        else
        {
            Q.erase(minNode); // erasing the minNode from the list
        }
        // updating the minNode iterator
        itr = Q.begin();
        minNode = itr;
        itr++;
        while (itr != Q.end())
        {
            if ((*itr)->distance < (*minNode)->distance)
                minNode = itr;
            itr++;
        }
    }
}

// To cut a node from its parent and insert it into the root list
void cutNode(list<struct fiboNode *> &Q, vector<list<struct fiboNode *>::iterator> &pointers, struct fiboNode *&temp, struct fiboNode *&tempParent)
{ // Updating ranks
    (tempParent->children).erase(pointers[temp->vertex]);
    tempParent->rank--;
    temp->parent = nullptr;
    temp->isMarked = false;
    Q.push_front(temp);
}

/* decreaseKeyFibonacci() reduces the key of a node. In case, min-heap property is violated,
 * the node is cut and inserted into the root list. Based on unmarked/marked status of the parent node,
 * cascade cuts are performed.
 */
void decreaseKeyFibonacci(list<struct fiboNode *> &Q, vector<list<struct fiboNode *>::iterator> &pointers, int v, int decreasedValue, list<struct fiboNode *>::iterator &minNode)
{
    list<struct fiboNode *>::iterator nodeToDecrease = pointers[v];
    (*nodeToDecrease)->distance = decreasedValue;
    struct fiboNode *temp = *nodeToDecrease, *tempParent = temp->parent, *tempParentParent;
    if (tempParent != nullptr && temp->distance < tempParent->distance)
    {
        cutNode(Q, pointers, temp, tempParent);
        pointers[temp->vertex] = Q.begin();
        if (!tempParent->isMarked) // Unmarked parent, mark it and return.
            tempParent->isMarked = true;
        else
        { // Marked parent, perform cascade cuts
            while (tempParent->isMarked)
            {
                tempParentParent = tempParent->parent;
                if (tempParentParent == nullptr)
                { // Marked node in the root list, unmark it
                    tempParent->isMarked = false;
                    break;
                }
                else
                { // cut it and recur
                    cutNode(Q, pointers, tempParent, tempParentParent);
                    pointers[tempParent->vertex] = Q.begin();
                    if (!tempParentParent->isMarked)
                    {
                        if (tempParentParent->parent != nullptr)
                            tempParentParent->isMarked = true;
                        break;
                    }
                    else
                        tempParent = tempParentParent;
                }
            }
        }
    }
    if (decreasedValue < (*minNode)->distance)
    {
        minNode = pointers[v]; // Update minNode iterator, if required.
    }
}

int main(int argc, char **argv)
{
    int T;
    cin >> T;
    time_t startTime[T + 1], endTime[T + 1]; // To store the start and end times of execution
    for (int t = 1; t <= T; t++)
    {
        int N, D, i, j, k;
        startTime[t] = clock();
        cin >> N >> D;
        int z;
        vector<vector<int>> adj(N + 1, vector<int>(N + 1));
        for (i = 1; i <= N; i++)
        {
            for (j = 1; j <= N; j++)
            {
                cin >> z;
                if (i != j)
                    adj[i][j] = z;
                else
                    adj[i][j] = 999999;
            }
        }

        // Add a new vertex N+1 in the graph
        vector<vector<int>> upAdj(N + 2, vector<int>(N + 2));
        for (i = 1; i <= N + 1; i++)
        {
            for (j = 1; j <= N + 1; j++)
            {
                if (j == N + 1)
                    upAdj[i][j] = 999999;
                else if (i == N + 1)
                    upAdj[i][j] = 0;
                else
                    upAdj[i][j] = adj[i][j];
            }
        }
        // Apply Bellman ford to the updated graph
        bool isRelaxed;
        int s = N + 1;
        int totalVertices = N + 1;
        vector<int> dummyDistance(totalVertices + 1, 999999);
        dummyDistance[s] = 0;
        for (k = 1; k < totalVertices; k++)
        {
            isRelaxed = false;
            for (i = 1; i <= totalVertices; i++)
            {
                for (j = 1; j <= totalVertices; j++)
                {
                    if (dummyDistance[i] != 999999 && (dummyDistance[j] > dummyDistance[i] + upAdj[i][j]))
                    {
                        dummyDistance[j] = dummyDistance[i] + upAdj[i][j];
                        isRelaxed = true;
                    }
                }
            }
            if (!isRelaxed)
                break;
        }
        if (isRelaxed)
        {
            isRelaxed = false;
            for (i = 1; i <= totalVertices; i++)
            {
                for (j = 1; j <= totalVertices; j++)
                {
                    if (dummyDistance[i] != 999999 && (dummyDistance[j] > dummyDistance[i] + upAdj[i][j]))
                    {
                        isRelaxed = true;
                        break;
                    }
                }
                if (isRelaxed)
                    break;
            }
        }
        if (!isRelaxed)
        { // No negative cycle in the graph
            // dummyDistance can be used to update the edge weights
            for (i = 1; i <= N; i++)
            {
                for (j = 1; j <= N; j++)
                {
                    if (adj[i][j] != 999999)
                        adj[i][j] += dummyDistance[i] - dummyDistance[j];
                }
            }
            vector<vector<int>> shortestPathWeight(N + 1, vector<int>(N + 1));
            for (i = 1; i <= N; i++)
            {
                for (j = 1; j <= N; j++)
                    if (i == j)
                        shortestPathWeight[i][i] = 0;
                    else
                        shortestPathWeight[i][j] = 999999;
            }
            if ((argc > 1 && *argv[1] == '1'))
            {
                // Array
                int u;
                for (k = 1; k <= N; k++)
                {
                    vector<int> Q(N + 1);
                    int Qsize = N;
                    for (i = 1; i <= N; i++)
                    {
                        if (i != k)
                            Q[i] = 999999;
                    }
                    Q[k] = 0;
                    while (Qsize != 0)
                    {
                        u = extractMinArray(Q, N);
                        Q[u] = -1;
                        Qsize--;
                        for (int v = 1; v <= N; v++)
                        {
                            if (adj[u][v] != 999999 && (shortestPathWeight[k][v] > shortestPathWeight[k][u] + adj[u][v]))
                            {
                                shortestPathWeight[k][v] = shortestPathWeight[k][u] + adj[u][v];
                                Q[v] = shortestPathWeight[k][v];
                            }
                        }
                    }
                }
            }
            else if ((argc > 1 && *argv[1] == '2'))
            {
                // Binary heap
                int u;
                for (k = 1; k <= N; k++)
                {
                    vector<pair<int, int>> Q(N + 1);
                    int indexOfKeys[N + 1];
                    int Qsize = N, index = 2;
                    // make heap
                    Q[1] = make_pair(0, k);
                    indexOfKeys[k] = 1;
                    for (i = 1; i <= N; i++)
                    {
                        if (i != k)
                        {
                            indexOfKeys[i] = index;
                            Q[index].first = 999999;
                            Q[index].second = i;
                            index++;
                        }
                    }
                    while (Qsize != 0)
                    {
                        u = Q[1].second;
                        Q[1] = Q[Qsize];
                        indexOfKeys[Q[Qsize].second] = 1;
                        Qsize--;
                        percolateDownBinary(Q, indexOfKeys, Qsize, 1);
                        for (int v = 1; v <= N; v++)
                        {
                            if (adj[u][v] != 999999 && (shortestPathWeight[k][v] > shortestPathWeight[k][u] + adj[u][v]))
                            {
                                shortestPathWeight[k][v] = shortestPathWeight[k][u] + adj[u][v];
                                int indexOfDecreasedKey = indexOfKeys[v];
                                Q[indexOfDecreasedKey].first = shortestPathWeight[k][v];
                                percolateUpBinary(Q, indexOfKeys, indexOfDecreasedKey);
                            }
                        }
                    }
                }
            }
            else if ((argc > 1 && *argv[1] == '3'))
            {
                // Binomial heap
                int u;
                for (k = 1; k <= N; k++)
                {
                    // make Heap
                    list<struct binomialNode *> Q;
                    vector<struct binomialNode *> pointers(N + 1);
                    struct binomialNode *minNode;
                    pointers[k] = insertBinomial(Q, 0, k);
                    for (i = 1; i <= N; i++)
                    {
                        if (i != k)
                            pointers[i] = insertBinomial(Q, 999999, i);
                    }
                    while (!Q.empty())
                    {
                        minNode = extractMinBinomial(Q);
                        u = minNode->vertex;
                        for (int v = 1; v <= N; v++)
                        {
                            if (adj[u][v] != 999999 && (shortestPathWeight[k][v] > shortestPathWeight[k][u] + adj[u][v]))
                            {
                                shortestPathWeight[k][v] = shortestPathWeight[k][u] + adj[u][v];
                                decreaseKeyBinomial(Q, pointers, v, shortestPathWeight[k][v]);
                            }
                        }
                    }
                    for (i = 1; i <= N; i++)
                        delete pointers[i];
                }
            }
            else
            {
                // fibonacci heaps
                int u;
                for (k = 1; k <= N; k++)
                {
                    list<struct fiboNode *> Q;
                    int totalNodes = 0;
                    list<struct fiboNode *>::iterator minNode;
                    vector<list<struct fiboNode *>::iterator> iterators(N + 1);
                    // make heap
                    iterators[k] = insertFibonacci(Q, 0, k, minNode);
                    totalNodes++;
                    for (i = 1; i <= N; i++)
                    {
                        if (i != k)
                        {
                            iterators[i] = insertFibonacci(Q, 999999, i, minNode);
                            totalNodes++;
                        }
                    }
                    while (!Q.empty())
                    {
                        u = (*minNode)->vertex;
                        extractMinFibonacci(Q, totalNodes, iterators, minNode);
                        totalNodes--;
                        for (int v = 1; v <= N; v++)
                        {
                            if (adj[u][v] != 999999 && (shortestPathWeight[k][v] > shortestPathWeight[k][u] + adj[u][v]))
                            {
                                shortestPathWeight[k][v] = shortestPathWeight[k][u] + adj[u][v];
                                decreaseKeyFibonacci(Q, iterators, v, shortestPathWeight[k][v], minNode);
                            }
                        }
                    }
                    for (i = 1; i <= N; i++)
                        delete (*iterators[i]);
                }
            }
            // Adjusting the final distances as per the original graph
            for (i = 1; i <= N; i++)
            {
                for (j = 1; j <= N; j++)
                {
                    if (shortestPathWeight[i][j] != 999999)
                        shortestPathWeight[i][j] += dummyDistance[j] - dummyDistance[i];
                    cout << shortestPathWeight[i][j] << " ";
                }
                cout << endl;
            }
        }
        else
            cout << "-1" << endl; // Negative cycle in the graph
        endTime[t] = clock();
    }
    for (int t = 1; t <= T; t++)
    {
        double time_taken = double(endTime[t] - startTime[t]) / double(CLOCKS_PER_SEC);
        cout << time_taken << " ";
    }
    return 0;
}
