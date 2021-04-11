#include <iostream>
#include <fstream>
#include <mpi.h>
#include <list>
#include <vector>
#include <string>

using namespace std;

vector<int> merge(vector<int> left, vector<int> right) {
	vector<int> result;

	while (!left.empty() || !right.empty()) {

		if (!left.empty() && !right.empty()) {
			if (left.front() <= right.front()) {
				result.push_back(left.front());
				left.erase(left.begin());
			} else {
				result.push_back(right.front());
				right.erase(right.begin());
			}
		} else if (!left.empty()) {
			for (auto val : left) {
				result.push_back(val);
			}

			break;
		} else if (!right.empty()) {
			for (auto val : right) {
				result.push_back(val);
			}

			break;
		}
	}

	return result;
}

vector<int> mergeSort(vector<int> initial) {
	vector<int> left;
	vector<int> right;
	vector<int> result;

	if (initial.size() <= 1)
		return initial;

	int middle = (initial.size() + 1) / 2;

	for (int i = 0; i < middle; i++) {
		left.push_back(initial[i]);
	}

	for (int i = middle; i < initial.size(); i++) {
		right.push_back(initial[i]);
	}

	left = mergeSort(left);
	right = mergeSort(right);

	return merge(left, right);
}

int main(int argc, char** argv) {
	list<int> dataInputs = { 100, 1000, 10000, 50000, 100000, 300000, 600000, 1000000 };

	// Initialize MPI
	int worldRank;
	int worldSize;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	for (auto testSize : dataInputs) {

		clock_t start;
		clock_t finish;

		// Define the files used for the tests
		auto inputFileName = "..\\Input Data\\data_input_" + to_string(testSize) + ".txt";
		auto outputFileName = "..\\Output Data\\Parallel implementation MPI\\data_output_" + to_string(testSize) + ".txt";

		ifstream fin(inputFileName);
		ofstream fout(outputFileName);

		vector<int> vector;
		int number;

		if (worldRank == 0) {
			// Read the input data
			for (int index = 0; index < testSize; index++) {
				fin >> number;
				vector.push_back(number);
			}
		}

		// Divide the initial vector for each process
		int partialSize = testSize / worldSize;

		std::vector<int> recVector(partialSize);
		
		// Start the timer before scattering the array
		if (worldRank == 0) {
			start = clock();
		}

		// Send each subarray to each process
		MPI_Scatter(vector.data(), partialSize, MPI_INT, recVector.data(), partialSize, MPI_INT, 0, MPI_COMM_WORLD);

		// MergeSort each sub-array of the vector
		recVector = mergeSort(recVector);

		// Gather all subarrays in one vector
		MPI_Gather(recVector.data(), partialSize, MPI_INT, vector.data(), partialSize, MPI_INT, 0, MPI_COMM_WORLD);

		// Sort the combined subarrays
		if (worldRank == 0) {
			vector = mergeSort(vector);
			
			// Save the finishing time
			finish = clock();
			
			// Print the array
			for (auto val: vector) {
				fout << val <<" ";
			}
			
			// Show the timer
			cout << "\nTest on " + to_string(testSize) + " numbers, took " << ((double)finish - (double)start) / CLOCKS_PER_SEC << " seconds.\n";
		}

		// Block processes so all of them start accordingly
		MPI_Barrier(MPI_COMM_WORLD);
	}

	// Finalize MPI
	MPI_Finalize();
}
