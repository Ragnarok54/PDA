#include <iostream>
#include <fstream>
#include <mpi.h>
#include <vector>
#include <list>
#include <string>

using namespace std;

vector<int> merge(vector<int> left, vector<int> right) {
    vector<int> result;

    while (!left.empty() || !right.empty()) {

        if (!left.empty() && !right.empty()) {
            if (left.front() <= right.front()) {
                result.push_back(left.front());
                left.erase(left.begin());
            } 
            else {
                result.push_back(right.front());
                right.erase(right.begin());
            }
        } 
        else if (!left.empty()) {
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

vector<int> mergeSortRec(int rank, int arraySize, vector<int> input, int worldSize){

    if (arraySize <= 1) {
        return input;
    }

    int middle = arraySize / 2;
    vector<int> left(input.begin(), input.begin() + middle);
    vector<int> right(input.begin() + middle, input.end());

    // Split the array in two in case we have more than two processes available
    if (worldSize >= 2) {
        int child = rank + worldSize / 2;

        // Send half the array to a child
        MPI_Send(right.data(), arraySize - middle, MPI_INT, child, 1, MPI_COMM_WORLD);

        // Sort the other half of the array
        left = mergeSortRec(rank, middle, left, worldSize / 2);

        // Get the sorted array from the child
        MPI_Status status;
        MPI_Recv(right.data(), arraySize - middle, MPI_INT, child, 1, MPI_COMM_WORLD, &status);
    } else {
        // Otherwise sort both halves recursively in the same process
        left = mergeSortRec(rank, middle, left, 1);
        right = mergeSortRec(rank, arraySize - middle, right, 1);
    }

    auto result =  merge(left, right);

    return result;
}

// Worker function for child processes to sort sub arrays recursively
void mergeWorker(int size, int rank, int worldSize){

    // Find the parrent of the current process
    int base = 0;
    int parent;
    int offset = rank;

    while (offset > 0) {
        parent = base;
        int mid = worldSize / 2;
        if (offset < mid) {
            size = size / 2;
            worldSize = worldSize / 2;
        }
        else {
            offset = offset - mid;
            size = size / 2 + size % 2;
            worldSize = worldSize / 2 + worldSize % 2;
            base += mid;
        }
    }

    // Get data from the parent process
    vector<int> v(size);
    MPI_Status status;
    MPI_Recv(v.data(), size, MPI_INT, parent, 1, MPI_COMM_WORLD, &status);

    // Sort the sub array
    auto result = mergeSortRec(rank, v.size(), v, worldSize);

    // Send the sorted array back to the parent
    MPI_Send(result.data(), size, MPI_INT, parent, 1, MPI_COMM_WORLD);
}

int main(int argc, char** argv){

    list<int> dataInputs = { 100, 1000, 10000, 50000, 100000, 300000, 600000, 1000000};

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
        if (worldRank == 0) {
            int number;

            // Read the input data
            for (int index = 0; index < testSize; index++) {
                fin >> number;
                vector.push_back(number);
            }
            
            // Save the start time
            start = clock();
            
            vector = mergeSortRec(0, vector.size(), vector, worldSize);
        } else {
            mergeWorker(testSize, worldRank, worldSize);
        }

        if (worldRank == 0) {
            // Save the end time of the test
            finish = clock();

            for (auto val : vector) {
                fout << val << " ";
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