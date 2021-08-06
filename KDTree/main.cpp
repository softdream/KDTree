#include <iostream>
#include <Eigen/Dense>
#include <vector>
#include <numeric>

template<typename T>
struct Node
{
	Node() :split(0), left(nullptr), right(nullptr), is_leaf(false)
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
			createTree( leftTreeDataVec, root->left );
		}
		
		if (rightTreeDataVec.size() != 0) {
			root->right = new Node<T>();
			createTree( rightTreeDataVec, root->right );
		}
	}

	void insert( T &newData )
	{
		Node<T> *tmpNode = root;
			
		if (tmpNode == nullptr) {
			tmpNode = new Node<T>();
			tmpNode->data = newData;
			tmpNode->is_leaf = true;
			
			return;
		}

		if (tmpNode->is_leaf) {
			tmpNode->is_leaf = false;

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
		}

		while (true) {
			int partialIndex = tmpNode->split;
			Node<T> *nextNode;

			if (newData[partialIndex] > tmpNode->data[partialIndex]){
				if (tmpNode->right == nullptr) {
					tmpNode->right = new Node<T>();
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

	void printNodes()
	{
		preOrderTravel( root );
	}

	void preOrderTravel( Node<T> *root )
	{
		if (root != nullptr) {
			std::cout << "node: " << std::endl << root->data << std::endl;

			preOrderTravel( root->left );
			preOrderTravel( root->right );
		}
	}

private:
	Node<T> *root;
	std::vector<T> dataVec;
	//std::vector<T> leftTreeDataVec;
	//std::vector<T> rightTreeDataVec;
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

	tree.insert( Eigen::Vector2f(8, 7) );

	tree.printNodes();

	return 0;
}