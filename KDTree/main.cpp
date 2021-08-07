#include <iostream>
#include <Eigen/Dense>
#include <vector>
#include <numeric>
#include <stack>

template<typename T>
struct Node
{
	Node() :split(0), left(nullptr), right(nullptr), parent(nullptr), is_leaf(false)
	{
		data = T::Zero();
		//std::cout << "data: " << std::endl << data << std::endl;
	}

	~Node()
	{

	}

	T data;
	int split;
	Node *left;
	Node *right;
	Node *parent;
	bool is_leaf;
};

int count = 0;

template<typename T, int Dimention = 2>
class KDTree 
{
public:
	KDTree()
	{
		root = new Node<T>();
		root->is_leaf = true;
	}

	~KDTree()
	{
		//delete root;
	}

	void buildKDTree() 
	{
		if (!dataVec.size()) {
			return;
		}

		root->is_leaf = true;
		return createTree( dataVec, root );
	}


	void getDataVec(std::vector<T> &dataVec)
	{
		this->dataVec =  dataVec;
	}

	void printDataVec() const
	{
		std::cout << "--------------------" << std::endl;
		for (size_t i = 0; i < dataVec.size(); i++) {
			std::cout << "data[" << i << "]: "<<std::endl<< dataVec[i] << std::endl;
		}
		std::cout << "--------------------" << std::endl;
	}

	void createTree( std::vector<T> dataVec, Node<T> *root )
	{
		count++;

		float maxCov = 0.0;
		int splitDimension = 0;
		// 1. Caculate the covarince of each dimensions
		T initialData = T::Zero();
		T avr = std::accumulate( dataVec.begin(), dataVec.end(), initialData ) / dataVec.size();

		for (size_t i = 0; i < Dimention; i++) {
			float accum = 0.0;
			for (auto it : dataVec) {
				accum += (it(i) - avr(i)) * (it(i) - avr(i));
			}

			// 2. find the dimension which has the maxmum covarince
			if (accum > maxCov) {
				maxCov = accum;
				splitDimension = i;
			}
		}
		std::cout << count << " split dimension "<< splitDimension << std::endl;

		// 3. find the median in this dimension
		std::vector<float> temp;
		for (auto it : dataVec)
			temp.push_back( it(splitDimension) );

		std::sort( temp.begin(), temp.end() );
		float median = temp[ temp.size() >> 1 ];
		std::cout << count << " median: " << median << std::endl;

		// 4. divide the dataset into left and right subtree container
		//leftTreeDataVec.clear();
		//rightTreeDataVec.clear();

		std::vector<T> leftTreeDataVec, rightTreeDataVec;

		for (auto it : dataVec) {
			if (it(splitDimension) < median) {// 
				leftTreeDataVec.push_back( it );
			}
			else if (it(splitDimension) > median) {
				rightTreeDataVec.push_back( it );
			}
			else {
				root->data = it;
				root->split = splitDimension;
			}
		}

		// ----------------- test ----------------//
		/*std::cout << "--------------- leftTreeDataVec --------------" << std::endl;
		for (auto it : leftTreeDataVec) {
			std::cout << it <<std::endl << std::endl;
		}
		std::cout << "------------------------------------------------" << std::endl;
		std::cout << "--------------- rightTreeDataVec --------------" << std::endl;
		for (auto it : rightTreeDataVec) {
			std::cout << it <<std::endl << std::endl;
		}
		std::cout << "------------------------------------------------" << std::endl;
		// ---------------------------------------//
		*/

		if (leftTreeDataVec.size() == 0 && rightTreeDataVec.size() == 0) {
			root->is_leaf = true;
		}

		if (leftTreeDataVec.size() != 0) {
			root->left = new Node<T>();
			root->left->parent = root;
			createTree( leftTreeDataVec, root->left );
		}
		
		if (rightTreeDataVec.size() != 0) {
			root->right = new Node<T>();
			root->right->parent = root;
			createTree( rightTreeDataVec, root->right );
		}
	}

	void insert( const T &newData )
	{
		if (root == nullptr) {
			return;
		}

		Node<T> *tmpNode = root;
			
		/*if (tmpNode == nullptr) { // if the tree is empty
			tmpNode = new Node<T>();
			tmpNode->data = newData;
			tmpNode->is_leaf = true;
			return;
		}*/

		if (tmpNode->is_leaf) { // there is always a node exists,it is root node
			tmpNode->is_leaf = false;
			tmpNode->data = newData;

			float max = 0.0;
			int partialIndex = 0;
			for (size_t i = 0; i < Dimention; i++) {
				float delta = std::abs(tmpNode->data(i) - newData(i));
				if (delta > max) {
					max = delta;
					partialIndex = i;
				}
			}
			tmpNode->split = partialIndex;

			return;
		}

		while (true) {
			int partialIndex = tmpNode->split;
			Node<T> *nextNode;

			if (newData[partialIndex] > tmpNode->data[partialIndex]){
				if (tmpNode->right == nullptr) {
					tmpNode->right = new Node<T>();
					tmpNode->right->parent = tmpNode;
					tmpNode->right->is_leaf = true;
					tmpNode->right->split = 1;
					tmpNode->right->data = newData;
					break;
				}
				else {
					nextNode = tmpNode->right;
				}
			}
			else {
				if (tmpNode->left == nullptr) {
					tmpNode->left = new Node<T>();
					tmpNode->left->parent = tmpNode;
					tmpNode->left->is_leaf = true;
					tmpNode->left->split = 1;
					tmpNode->left->data = newData;
					break;
				}
				else {
					nextNode = tmpNode->left;
				}
			}

			if (nextNode->is_leaf) {
				nextNode->is_leaf = false;
				float max = 0.0f;
				for (size_t i = 0; i < Dimention; i++){
					float delta = std::abs(newData[i] - nextNode->data[i]);
					if (delta > max){
						max = delta;
						nextNode->split = i;
					}
				}
			}

			tmpNode = nextNode;
		}
	}

	void printNodes() const
	{
		preOrderTravel( root );
	}

	void preOrderTravel( Node<T> *root ) const
	{
		if (root != nullptr) {
			std::cout << "node: " << std::endl << root->data << std::endl;
			std::cout << "split: " << root->split << std::endl << std::endl;

			preOrderTravel( root->left );
			preOrderTravel( root->right );
		}
	}

	bool deleteNode( const T &data )
	{
		if (root == nullptr) {
			return false;
		}

		Node<T> **pToCurr = &root;
		Node<T> *pCurr = root;
		Node<T> *pDelete = nullptr;

		while (pCurr != nullptr && pCurr->data != data) {
			if ( data[pCurr->split] <=pCurr->data[pCurr->split] ) { // if data of current less than the delete data
				std::cout << "pCurr->data[pCurr->split]: " << pCurr->data[pCurr->split] << std::endl;
				pToCurr = &pCurr->left;
				pCurr = pCurr->left;
			}
			else {
				std::cout << "pCurr->data[pCurr->split]: " << pCurr->data[pCurr->split] << std::endl;
				pToCurr = &pCurr->right;
				pCurr = pCurr->right;
			}
		}

		if (pCurr == nullptr) { // not find the element
			std::cout << "didn't find the delete node ..." << std::endl;
			return false;
		}

		pDelete = pCurr;

		// leaf node
		if (pCurr->left == nullptr && pCurr->right == nullptr) {
			*pToCurr = nullptr;
		}
		else if (pCurr->left == nullptr) {
			*pToCurr = pCurr->right;
		}
		else if (pCurr->right == nullptr) {
			*pToCurr = pCurr->left;
		}
		else {
			// Replace the deleted node with the left subtree of the deleted node
			*pToCurr = pCurr->left;

			pCurr = pDelete->left;

			while (pCurr->right != nullptr)
				pCurr = pCurr->right;

			pCurr->right = pDelete->right;
		}

		delete pDelete;

		return true;
	}

	const T nearestNeighborSearchRecursive( const T &goalPoint )
	{
		T nearestPoint;
		Node<T> *tmp = root;

		while (!tmp->is_leaf) {
			int partialIndex = tmp->split;
			if (tmp->left != nullptr && goalPoint[partialIndex] < tmp->data[partialIndex]) {
				tmp = tmp->left;
			}
			else if (tmp->right != nullptr) {
				tmp = tmp->right;
			}
		}

		nearestPoint = tmp->data;

		float radius = (goalPoint - nearestPoint).norm();

		bool isLeft = false;

		while (tmp != root) {
			isLeft = (tmp == tmp->parent->left);

			tmp = tmp->parent;
			if ((goalPoint - tmp->data).norm() < radius) {
				nearestPoint = tmp->data;
				radius = (goalPoint - nearestPoint).norm();
			}

			int patialIndex = tmp->split;

			if (radius > std::abs(tmp->data[patialIndex] - goalPoint[patialIndex])) {
				if (isLeft) {
					searchNearest( goalPoint, radius, tmp->right, nearestPoint );
				}
				else {
					searchNearest( goalPoint, radius, tmp->left, nearestPoint );
				}
			}
		}

		return nearestPoint;
	}

	void searchNearest(const T &goalPoint, float radius, const Node<T> *root, T &nearestPoint)
	{
		if (root == nullptr)
			return;

		float newRadius = (goalPoint - root->data).norm();
		if (newRadius < radius) {
			radius = newRadius;
			nearestPoint = root->data;
		}

		searchNearest( goalPoint, radius, root->left, nearestPoint );
		searchNearest( goalPoint, radius, root->right, nearestPoint );
	}

	const T nearestNeighborSearch(const T &goalPoint)
	{
		Node<T> *tmp = root;
		T nerestPoint = tmp->data;
		
		while (tmp != nullptr) {
			std::cout << "tmp = " << std::endl << tmp->data << std::endl;
			searchPath.push( tmp );
	
			int split = tmp->split;
			if (goalPoint[split] <= tmp->data[split]) {
				tmp = tmp->left;
			}
			else {
				tmp = tmp->right;
			}
		}

		nerestPoint = searchPath.top()->data;
		tmp = searchPath.top();
		//nerestPoint = root->data;
		std::cout << "nerestPoint : " << std::endl << nerestPoint << std::endl;

		float maxDist = (nerestPoint - goalPoint).norm();
		std::cout << "max distance = " << maxDist << std::endl;

		while (!searchPath.empty()) {	
			Node<T> *backPoint = searchPath.top();
			searchPath.pop();
			
			std::cout << "backPoint: " << std::endl << backPoint->data << std::endl;

			int split = backPoint->split;

			std::cout << "distance = " << (goalPoint - backPoint->data).norm() << std::endl;

			if ((goalPoint - backPoint->data).norm() < maxDist) {
				if (goalPoint[split] <= backPoint->data[split]) {
					tmp = backPoint->right;
				}
				else {
					tmp = backPoint->left;
				}
				searchPath.push( tmp );
			}

			std::cout << "nerestPoint : " << std::endl << nerestPoint << std::endl;
			if ((nerestPoint - goalPoint).norm() > (tmp->data - goalPoint).norm()) {
				nerestPoint = tmp->data;
				maxDist = (tmp->data - goalPoint).norm();
			}
		}

		return nerestPoint;
	}

private:
	Node<T> *root;
	std::vector<T> dataVec;
	
	std::stack<Node<T>*> searchPath;
};


int main()
{
	KDTree<Eigen::Vector2f> tree;

	std::vector<Eigen::Vector2f> vec;

	Eigen::Vector2f node1( 7, 2 );
	vec.push_back(node1);
	Eigen::Vector2f node2(5, 4);
	vec.push_back(node2);
	Eigen::Vector2f node3(2, 3);
	vec.push_back(node3);
	Eigen::Vector2f node4(4, 7);
	vec.push_back(node4);
	Eigen::Vector2f node5(9, 6);
	vec.push_back(node5);
	Eigen::Vector2f node6(8, 1);
	vec.push_back(node6);

	tree.getDataVec( vec );
	tree.printDataVec();

	tree.buildKDTree(  );
	std::cout << "count : " << count << std::endl;

	tree.printNodes();
	std::cout << " ------------------------------ " << std::endl;

	//tree.insert( Eigen::Vector2f(3, 6) );

//	tree.printNodes();

	/*KDTree<Eigen::Vector2f> tree;

	Eigen::Vector2f node1(7, 2);
	tree.insert( node1 );
	
	Eigen::Vector2f node2(5, 4);
	tree.insert(node2);

	Eigen::Vector2f node3(2, 3);
	tree.insert(node3);

	Eigen::Vector2f node4(4, 7);
	tree.insert(node4);

	Eigen::Vector2f node5(9, 6);
	tree.insert(node5);

	Eigen::Vector2f node6(8, 1);
	tree.insert(node6);

	//tree.insert(Eigen::Vector2f(3, 6));
	//tree.deleteNode( node1 );

	tree.printNodes();*/

	std::cout << "-----------search the kd tree ----------" << std::endl;
	Eigen::Vector2f goal(2, 4.5);
	Eigen::Vector2f nerest = tree.nearestNeighborSearch( goal );

	std::cout << std::endl <<nerest << std::endl;

	return 0;
}