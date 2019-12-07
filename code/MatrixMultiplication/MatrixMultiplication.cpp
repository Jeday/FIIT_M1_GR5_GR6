#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <stdlib.h>



// clang++ -Xpreprocessor -fopenmp matrixMultiplication.cpp -o main -lomp -std=c++11
typedef float MatrixType;
const size_t  SIZE = 2880;
const size_t BLOCK_SIZES[] = {720, 480, 360, 240, 180, 160, 144, 120, 96, 80, 72, 60, 40, 36, 30, 24, 20, 15, 10, 6, 1 };//{1};

 size_t BLOCK_SIZE_IND = 0;
 size_t BLOCK_SIZE = BLOCK_SIZES[BLOCK_SIZE_IND];
 size_t BLOCK_COUNT = SIZE / BLOCK_SIZE;
 size_t BLOCK_SIZE2 = BLOCK_SIZE * BLOCK_SIZE;
 size_t SIZE2 = SIZE * SIZE;
 size_t NO_ELEM = (size_t)0 - 1;
MatrixType*MatrixA, *MatrixB, *MatrixC;

std::uniform_real_distribution<MatrixType> unif(-1000, 1000);
std::default_random_engine re;



struct Task {
public:
	size_t i;
	size_t j;

	Task(size_t _i,size_t _j){
		i = _i;
		j = _j;
	}
};

std::vector<Task> tasks;


size_t ** offsetA, ** offsetB, **offsetC;
size_t arraySize;
size_t calcSize() {
	size_t res = 0;
	for (size_t i = 0; i < BLOCK_COUNT; i++)
	{
		for (size_t j = 0; j <= i; j++)
		{
			res += BLOCK_SIZE2;
		}
	}
	return res;
}

size_t ** generateOffsetA() {
	
	size_t ** result = new size_t*[BLOCK_COUNT];
	for (size_t i = 0; i < BLOCK_COUNT; i++)
	{
		result[i] = new size_t[BLOCK_COUNT]{ 0 };
	}

	// columns first
	size_t offset = 0;
	for (size_t j = 0; j < BLOCK_COUNT; j++)
	{
		for (size_t i = 0; i < BLOCK_COUNT; i++)
		{
			if (i < j) {
				result[i][j] = NO_ELEM;
			}
			else {
				result[i][j] = offset;
				offset += BLOCK_SIZE2;
			}
			
		}
	}
	return result;
	
}


size_t ** generateOffsetB() {

	size_t ** result = new size_t*[BLOCK_COUNT];
	for (size_t i = 0; i < BLOCK_COUNT; i++)
	{
		result[i] = new size_t[BLOCK_COUNT]{ 0 };
	}

	
	size_t offset = 0;
	for (size_t i = 0; i < BLOCK_COUNT; i++)
	{
		for (size_t j = 0; j < BLOCK_COUNT; j++)
		{
			if (i < j) {
				result[i][j] = NO_ELEM;
			}
			else {
				result[i][j] = offset;
				offset += BLOCK_SIZE2;
			}

		}
	}
	return result;

}

size_t ** generateOffsetC() {

	size_t ** result = new size_t*[BLOCK_COUNT];
	for (size_t i = 0; i < BLOCK_COUNT; i++)
	{
		result[i] = new size_t[BLOCK_COUNT]{ 0 };
	}

	// columns first
	size_t offset = 0;
	for (size_t i = 0; i < BLOCK_COUNT; i++)
	{
		for (size_t j = 0; j < BLOCK_COUNT; j++)
		{
			
				result[i][j] = offset;
				offset += BLOCK_SIZE2;
			

		}
	}
	return result;

}


inline MatrixType* addresA(const size_t row, const size_t col){
	// shouldn't be needed if proper permutations are generated
	// i guess
	return MatrixA + offsetA[row][col];
}

inline MatrixType* addresB(const size_t row, const size_t col) {
	return MatrixB + offsetB[row][col];
}

inline MatrixType* addresC(const size_t row, const size_t col) {
	return MatrixC + offsetC[row][col];
}

void genereateTasks() {
	for (size_t i = 0; i < BLOCK_COUNT; i++)
		for (size_t j = 0; j < BLOCK_COUNT; j++)
			tasks.push_back(Task(i, j));
}
void printOffset(size_t ** off) {
	for (size_t i = 0; i < BLOCK_COUNT; i++)
	{
		for (size_t j = 0; j < BLOCK_COUNT; j++)
		{
			int pr = off[i][j] == NO_ELEM ? -1 : (int)off[i][j];
			std::cout << pr << "\t";
		}
		std::cout << std::endl;
	}
}
 void printBlocksA(){
	 for (size_t i = 0; i < BLOCK_COUNT; i++)
	 {
		 for (size_t j = 0; j < BLOCK_COUNT; j++)
		 {
			 MatrixType* adr = addresA(i,j);
			 if (adr == nullptr)
				 std::cout << "NO_ELEM";
			 else
				 std::cout << adr[0];
			 std::cout << "  ";
		 }
		 std::cout << std::endl;
	 }
 }

 void printBlocksB() {
	 for (size_t i = 0; i < BLOCK_COUNT; i++)
	 {
		 for (size_t j = 0; j < BLOCK_COUNT; j++)
		 {
			 MatrixType* adr = addresB(i, j);
			 if (adr == nullptr)
				 std::cout << "NO_ELEM";
			 else
				 std::cout << adr[0];
			 std::cout << "  ";
		 }
		 std::cout << std::endl;
	 }
 }

 void printBlocksC() {
	 for (size_t i = 0; i < BLOCK_COUNT; i++)
	 {
		 for (size_t j = 0; j < BLOCK_COUNT; j++)
		 {
			 MatrixType* adr = addresC(i, j);
			 std::cout << adr[0];
			 std::cout << "  ";
		 }
		 std::cout << std::endl;
	 }
 }


 void printBlock(MatrixType* blockStart) {
	 for (size_t i = 0; i < BLOCK_SIZE; i++)
	 {
		 for (size_t j = 0; j < BLOCK_SIZE; j++)
		 {
			 std::cout << blockStart[i*BLOCK_SIZE + j] << "  ";
		 }
		 std::cout << std::endl;
	 }
 }

inline void multBlocks(const MatrixType* const A, const MatrixType* const B,  MatrixType* const C) {
    //#pragma omp parallel for
	for (int i = 0; i < BLOCK_SIZE; i++)
		for (size_t j = 0; j < BLOCK_SIZE; j++)
			for (size_t k = 0; k < BLOCK_SIZE; k++)
				C[i*BLOCK_SIZE + j] += A[i*BLOCK_SIZE + k] * B[k*BLOCK_SIZE + j];
			
}

inline void multBlocksT(const MatrixType* const A, const MatrixType* const B,  MatrixType* const C) {
   //#pragma omp parallel for
	for (int i = 0; i < BLOCK_SIZE; i++)
		for (size_t j = 0; j < BLOCK_SIZE; j++)
			for (size_t k = 0; k < BLOCK_SIZE; k++)
				C[i*BLOCK_SIZE + j] += A[i*BLOCK_SIZE + k] * B[j*BLOCK_SIZE + k];
			
}

void multMatrixes() {
   #pragma omp parallel for
	for (int i = 0; i < tasks.size(); i++)
	{
		Task t = tasks[i];
		//if skipping where no blocks are present and 
		for (size_t k = 0; (k <= t.j)&&(k<=t.i) ; k++)
		{	
			MatrixType* A = addresA(t.i, k);
			MatrixType* B = addresB(k, t.j);
			MatrixType* C = addresC(t.i, t.j);
			multBlocks(A, B, C);
		}
		for(size_t k = t.j+1; k <= t.i; k++){
			MatrixType* A = addresA(t.i, k);
			MatrixType* B = addresB(k, t.j);
			MatrixType* C = addresC(t.i, t.j);
			multBlocksT(A, B, C);
		}
	}
}


void generateRandomBlock(MatrixType* Block) {
	for (size_t i = 0; i < BLOCK_SIZE; i++)
		for (size_t j = 0; j < BLOCK_SIZE; j++) {
			Block[i*BLOCK_SIZE+j]= unif(re);
		}
}

void generateRandomLowerTriangle(MatrixType* Block) {
	for (size_t i = 0; i < BLOCK_SIZE; i++)
		for (size_t j = 0; j < BLOCK_SIZE; j++) {
			if (i < j)
				Block[i*BLOCK_SIZE + j] = 0;
			else
				Block[i*BLOCK_SIZE + j] = unif(re);
		}
}

void generateRandomSym(MatrixType* Block) {
	for (size_t i = 0; i < BLOCK_SIZE; i++)
		for (size_t j = 0; j < BLOCK_SIZE; j++) {
			if (i < j)
				continue;
			else
				Block[i*BLOCK_SIZE + j] = unif(re);
		}
	for (size_t i = 0; i < BLOCK_SIZE; i++)
		for (size_t j = 0; j < BLOCK_SIZE; j++) {
			if (i < j)
				Block[i*BLOCK_SIZE + j] = Block[j*BLOCK_SIZE + i];
			else
				continue;
		}
}


void generateMatrixA() {
	for (size_t j = 0; j < BLOCK_COUNT; j++)
	{
		for (size_t i = 0; i < BLOCK_COUNT; i++)
		{
			if (i<j)
				continue;
			MatrixType* A = addresA(i, j);
			if (A == nullptr)
				continue;
			if (i == j)
				generateRandomLowerTriangle(A);
			else
				generateRandomBlock(A);

		}
	}
}

void generateMatrixB() {
	for (size_t i = 0; i < BLOCK_COUNT; i++)
	{
		for (size_t j = 0; j < BLOCK_COUNT; j++)
		{
			if (i<j)
				continue;
			MatrixType* B = addresB(i, j);
			if (i == j)
				generateRandomSym(B);
			else
				generateRandomBlock(B);

		}
	}
}


void Debug_print(){
	std::cout << "Matrix A blocks starts:" << std::endl;
	printBlocksA();
	std::cout << std::endl << std::endl;
	std::cout << "Matrix A diagonal block:" << std::endl;
	printBlock(addresA(BLOCK_COUNT/2, BLOCK_COUNT / 2));
	std::cout << std::endl << std::endl;
	std::cout << "Matrix B blocks starts:" << std::endl;
	printBlocksB();
	std::cout << std::endl << std::endl;
	std::cout << "Matrix B diagonal block:" << std::endl;
	printBlock(addresB(BLOCK_COUNT / 2, BLOCK_COUNT / 2));
	std::cout << std::endl << std::endl;
	std::cout << "Matrix C blocks starts:" << std::endl;
    printBlocksC();
}

void cleanUP() {
	delete[] MatrixA;
	delete[] MatrixB;
	delete[] MatrixC;
	for (size_t i = 0; i < BLOCK_COUNT; i++)
	{
		delete[] offsetA[i];
		delete[] offsetB[i];
		delete[] offsetC[i];
	}
	tasks.clear();
 }

void setUP(size_t ind) {
	 BLOCK_SIZE_IND = ind;
	 BLOCK_SIZE = BLOCK_SIZES[BLOCK_SIZE_IND];
	 BLOCK_COUNT = SIZE / BLOCK_SIZE;
	 BLOCK_SIZE2 = BLOCK_SIZE * BLOCK_SIZE;
	 SIZE2 = SIZE * SIZE;
	 NO_ELEM = (size_t)0 - 1;
}

int main()
{
	for (size_t i = 0; i < 21; i++)
	{
		setUP(i);
		arraySize = calcSize();
		MatrixA = new MatrixType[arraySize];
		MatrixB = new MatrixType[arraySize];
		MatrixC = new MatrixType[SIZE2];
		for (size_t i = 0; i < SIZE2; i++)
		{
			MatrixC[i] = 0;
		}
		offsetA = generateOffsetA();
		offsetB = generateOffsetB();
		offsetC = generateOffsetC();
		genereateTasks();
		generateMatrixA();
		generateMatrixB();
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		multMatrixes();
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = t2 - t1;
		std::chrono::nanoseconds duration = std::chrono::duration_cast<std::chrono::nanoseconds>(time_span);
		cleanUP();
		std::cout<< BLOCK_SIZE <<"\t" << duration.count() << std::endl;
	}

}
