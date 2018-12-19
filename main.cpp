#include <iostream>
#include <queue>
#include <vector>
#include <random>
#include <fstream>
#include <cmath>
#include <sstream>
#include <chrono>
#include <ctime> 

using namespace std;

#pragma region Random numbers generation
random_device generator;
uniform_real_distribution<double> distribution(0.0, 1.0);
#define rn distribution(generator);	
#pragma endregion


constexpr int latticeSize = 40;
unsigned lattice[latticeSize][latticeSize]; // prvni je cislo radku, druhe je cislo sloupce



int howManySteps;
unsigned numberOfStates;
float beta;

struct coord2D
{
	int x, y;
	coord2D(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};


void FillWithRandomNumbers2D()
{
	for (int i = 0; i < latticeSize; i++) // i je cislo radku
	{
		for (int j = 0; j < latticeSize; j++) // j je cislo sloupce
		{
			double random = rn;
			unsigned newState = (unsigned)floor(random * numberOfStates); // zaokrouhli dolu, dle definice uniform_real_distribution
																		  // nikdy nenabyde hodnoty 1.0, takze vse v poradku
			lattice[i][j] = newState;
		}
	}
}

void Initialization2D() // nainicializuje  lattice
{
	FillWithRandomNumbers2D();
}


void OneIteration2D(int i, int j)
{
	vector<double> probabilities (numberOfStates, 0); // initializes the whole vector to zeros
	probabilities[lattice[(latticeSize + i - 1) % latticeSize][j]]++; // up
	probabilities[lattice[(latticeSize + i + 1) % latticeSize][j]]++; // down
	probabilities[lattice[i][(latticeSize + j - 1) % latticeSize]]++; // left
	probabilities[lattice[i][(latticeSize + j + 1) % latticeSize]]++; // right

	for (unsigned q = 0; q < numberOfStates; q++)
	{
		probabilities[q] = exp(beta * probabilities[q]);
	} // mame citatel uplne podminene pravdepodobnosti

	double sum = 0;
	for (unsigned q = 0; q < numberOfStates; q++)
	{
		sum += probabilities[q];
	} // v promenne sum mame jmenovatel uplne podminene pravdepodobnosti

	for (unsigned q = 0; q < numberOfStates; q++)
	{
		probabilities[q] /= sum;
	} // nyni mame v probabilities primo uplne podminene pravdepodobnosti

	double random = rn;

	unsigned q = 0;
	double probability = probabilities[q];
	while (probability < random)
	{
		q++;
		probability += probabilities[q];
	}

	lattice[i][j] = q;
}


void OneStep2D() // Gibbs systematic sampler
{
	for (int i = 0; i < latticeSize; i++)
	{
		for (int j = 0; j < latticeSize; j++)
		{
			OneIteration2D(i, j);
		}
	}
}

void Print2D()
{
	for (int i = 0; i < latticeSize; i++) // i je cislo radku
	{
		for (int j = 0; j < latticeSize; j++) // j je cislo sloupce
		{
			cout << lattice[i][j] << " ";
		}
		cout << endl;
	}
}

void Print2DToFile(int number, int steps)
{
	ofstream saveFile;
	stringstream ss;
	ss << "lattice" << number << ".txt";
	string str = ss.str();
	saveFile.open(str, ios::out | ios::trunc);

	saveFile << "% mode: 2D" << endl;
	saveFile << "% size: " << latticeSize << endl;
	saveFile << "% steps: " << steps << endl;
	saveFile << "% states: " << numberOfStates << endl;
	saveFile << "% beta: " << beta << endl;

	for (int i = 0; i < latticeSize; i++) // i je cislo radku
	{
		for (int j = 0; j < latticeSize; j++) // j je cislo sloupce
		{
			saveFile << lattice[i][j] << " ";
		}
		saveFile << endl;
	}
	saveFile << endl;
	saveFile.close();
}

void Load2DFromFile(string fileName)
{
	ifstream file;
	file.open(fileName, ifstream::in);
	//file.getline();
	//file.get();


	file.close();
}

string startTime;

void SaveNumberOfStates(int number, int steps)
{
	vector<double> frequency(numberOfStates, 0);
	for (int i = 0; i < latticeSize; i++)
	{
		for (int j = 0; j < latticeSize; j++)
		{
			frequency[lattice[i][j]]++;
		}
	}

	ofstream saveFile;
	stringstream ss;
	ss << "frequency" << number << ".txt";
	string str = ss.str();
	saveFile.open(str, ios::out | ios::trunc);
	saveFile << "% Start: " << startTime;
#pragma region TimeStamp
	{
		auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
#pragma warning(suppress : 4996)
		saveFile << "% End: " << ctime(&timenow);
	}
#pragma endregion

	saveFile << "% mode: 2D" << endl;
	saveFile << "% size: " << latticeSize << endl;
	saveFile << "% steps: " << steps << endl;
	saveFile << "% states: " << numberOfStates << endl;
	saveFile << "% beta: " << beta << endl;
	for (unsigned i = 0; i < numberOfStates; i++)
	{
		saveFile << i << " " << frequency[i] << endl;
	}
	saveFile << endl;
	saveFile.close();
}




bool visited2D[latticeSize][latticeSize];
priority_queue<int> grainSizes;
queue<coord2D> remaining;

void ResetVariables2D();
void CallingBFS2D();
void SaveToFile(int number, int steps);
void ProcessAndSave(int number, int steps)
{
	ResetVariables2D();
	CallingBFS2D();
	SaveToFile(number, steps);
}

void ResetVariables2D()
{
	for (int i = 0; i < latticeSize; i++)
	{
		for (int j = 0; j < latticeSize; j++)
		{
			visited2D[i][j] = false;
		}
	}
	while (!grainSizes.empty())
	{
		grainSizes.pop();
	}
	while (!remaining.empty())
	{
		remaining.pop();
	}
}

void BFS2D(int i, int j);
void CallingBFS2D() // Breadth-first search
{
	for (int i = 0; i < latticeSize; i++)
	{
		for (int j = 0; j < latticeSize; j++)
		{
			if (visited2D[i][j] == false)
			{
				BFS2D(i, j);
			}
		}
	}
}

int BFS2DCheck(int otherX, int otherY, unsigned value)
{
	if ((visited2D[otherX][otherY] == false) && (lattice[otherX][otherY] == value))
		// point is always visited2D, so it can not end up in remaining again
	{
		remaining.push(coord2D(otherX, otherY));
		visited2D[otherX][otherY] = true;
		return 1;
	}
	return 0;
}
void BFS2D(int i, int j)
{
	unsigned value = lattice[i][j];
	remaining.push(coord2D(i, j));
	visited2D[i][j] = true;
	int count = 1;
	while (!remaining.empty())
	{
		coord2D point = remaining.front();
		remaining.pop();
		int otherX, otherY;

		otherY = point.y;

		otherX = (latticeSize + point.x + 1) % latticeSize;
		count += BFS2DCheck(otherX, otherY, value);
		otherX = (latticeSize + point.x - 1) % latticeSize;
		count += BFS2DCheck(otherX, otherY, value);

		otherX = point.x;

		otherY = (latticeSize + point.y + 1) % latticeSize;
		count += BFS2DCheck(otherX, otherY, value);
		otherY = (latticeSize + point.y - 1) % latticeSize;
		count += BFS2DCheck(otherX, otherY, value);
	}
	grainSizes.push(count);
}


void SaveToFile(int number, int steps)
{
	ofstream saveFile;
	stringstream ss;
	ss << "data" << number << ".txt";
	string str = ss.str();
	saveFile.open(str, ios::out | ios::trunc);
	saveFile << "% Start: " << startTime;
#pragma region TimeStamp
	{
		auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
#pragma warning(suppress : 4996)
		saveFile << "% End: " << ctime(&timenow);
	}
#pragma endregion

	saveFile << "% mode: 2D" << endl;
	saveFile << "% size: " << latticeSize << endl;
	saveFile << "% steps: " << steps << endl;
	saveFile << "% states: " << numberOfStates << endl;
	saveFile << "% beta: " << beta << endl;
	while (!grainSizes.empty())
	{
		saveFile << grainSizes.top() << endl;
		grainSizes.pop();
	}
	saveFile << endl;
	saveFile.close();
}

void StartTimeStamp()
{
	stringstream ss;
	auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
#pragma warning(suppress : 4996)
	ss << ctime(&timenow);
	startTime = ss.str();
}

#include <iomanip>
void MakeOne2D(int steps, int states, float betaLocal, int dataSavingInterval,
			   int latticesavingInterval, int procentInterval, int frequencySavingInterval)
{
	StartTimeStamp();
	howManySteps = steps;
	numberOfStates = states;
	beta = betaLocal;

	Initialization2D();
	int number = 1;
	int saveNumber = 1;
	int frequencyNumber = 1;
	for (int i = 0; i < howManySteps; i++)
	{
		if ((procentInterval != 0) && (i % procentInterval == 0))
		{
			cout << "Hotovo " << setprecision(6) << 100*(double)(i) / howManySteps << " %." << endl;
		}
		OneStep2D();
		if ((dataSavingInterval != 0) && (i % dataSavingInterval == 0))
		{
			ProcessAndSave(number++, i);
		}
		if ((frequencySavingInterval != 0) && (i % frequencySavingInterval == 0))
		{
			SaveNumberOfStates(frequencyNumber++, i);
		}
		if ((latticesavingInterval != 0) && (i % latticesavingInterval == 0))
		{
			Print2DToFile(saveNumber++, i);
		}
	}
	cout << "Hotovo 100 %." << endl;
	cout << " - Konec simulace ... ";
	
	if (dataSavingInterval != 0)
	{
		ProcessAndSave(number, howManySteps);
	}
	if (latticesavingInterval != 0)
	{
		Print2DToFile(saveNumber, howManySteps);
	}
	if (frequencySavingInterval != 0)
	{ 
		SaveNumberOfStates(frequencyNumber, howManySteps);
	}
	cout << "Konec zapisu " << endl;
	//Print2D();
	
	//getchar();
}

void InitializeByConsole()
{
	int steps, states, dataSavingInterval, latticeSavingInterval, procentInterval, frequencySavingInterval;
	float beta;
	cout << "Rezim: 2D" << endl << "Velikost mrizky: " << latticeSize << endl << "Dalsi parametry zadejte. " << endl;
	cout << "Pocet iteraci: ";
	cin >> steps;
	cout << "Po kolika iteracich pocitat zrna: ";
	cin >> dataSavingInterval;
	cout << "Po kolika iteracich spocitat frekvenci barev: ";
	cin >> frequencySavingInterval;
	cout << "Po kolika iteracich ukladat mrizku: ";
	cin >> latticeSavingInterval;
	cout << "Po kolika iteracich vypisovat procenta: ";
	cin >> procentInterval;
	cout << "Pocet stavu (= Q): ";
	cin >> states;
	cout << "Parametr beta: ";
	cin >> beta;
	cout << "Pracuji ..." << endl;
	MakeOne2D(steps, states, beta, dataSavingInterval, latticeSavingInterval, procentInterval, frequencySavingInterval);
}

int main()
{
	InitializeByConsole();
	//getchar();
	//getchar();
}