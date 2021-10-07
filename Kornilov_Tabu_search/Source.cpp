	
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;


const double EPS = 1E-7;

int TabuS = 100;
int MaxIt = 600;

void InputList(vector<double> & V, int & n)
{
	string filename;
	cout << "Write name of file\n";
	cin >> filename;
	ifstream inputFile(filename+".txt");
	inputFile >> n;
	V.resize(n*n);

	for (int i = 0; i < n-1; ++i)
		for (int j = 1; j < n+1; ++j)
		{
			inputFile >> V[i * (n + 1) + j];
		}
	inputFile.close();

	for (int i = 0; i < n; ++i) { V[i * (n + 1)] = 1e300; };
}


double Price(const vector<int>& Solution, const vector<double>& PMatr) 
{
	double sum = 0.;
	int n = Solution.size();
	int val1 = Solution[0];
	int val2;
	for (int i = 1; i < n; ++i)
	{
		val2 = Solution[i];
		sum += PMatr[(val1 - 1) * n + val2 - 1];
		val1 = val2;
	}
	sum += PMatr[(val1 - 1) * n + Solution[0] - 1];
	return sum;
}


bool BelongsTabuQ(const vector<int>& Solution, const list< vector<int> >& TabuN)
{
	auto it = TabuN.begin();
	while (it != TabuN.end()) 
	{
		int k = 0;
		for (int j = 0; j < Solution.size(); ++j)
		{
			if (Solution[j] != (*it)[j]) { ++k; }
		};
		if (k == 2 || k == 0 ) { return true; };
		++it;
	}
	return false;
}


void AddTabu(list< vector<int> >& TabuN, vector<int>& AddV)
{
	TabuN.push_front(AddV);
	if (TabuN.size() > TabuS) { TabuN.pop_back(); }
}

void CreateSolution(vector<int>& solution,  vector<double> Pmatr) 
{
	int n = solution.size();
	solution[0] = 1;

	int El = 1;
	int MinEl = 1 ;

	for (int i = 1; i < n; ++i)//номер вставляемой позиции
	{
				double MinZn = 1e300;
		for (int j = 1; j < n; ++j)
		{
			if (Pmatr[(El - 1) * n + j] < MinZn) { MinZn = Pmatr[(El - 1) * n + j]; MinEl = j + 1; };
		}
		
		solution[i] = MinEl;
		El = MinEl;

		for (int j = 0; j < solution.size(); ++j)
		{
			Pmatr[n * j + MinEl - 1] = 1e300;
		}
	}
}

vector<int> TabuSearch(const vector<double>& Pmatr, const int& n)
{
	ofstream outFile("coord3.txt");

	vector<int> solution(n);
	list< vector<int> > tabuN;
	
	int LastMinIt = 0;
	//for (int i = 0; i < n; ++i) { solution[i] = i + 1; };
	CreateSolution(solution, Pmatr);
	vector<int> GminSolution { solution };
	double Gmin{ Price(GminSolution, Pmatr) };

	do
	{
		++LastMinIt;
		double Lmin = 1e300;
		vector<int> LminSolution{ solution };
		vector<int> Lsolution{ solution };

		for (int i = 0; i < n - 1; ++i) 
		{
			for (int j = n-1; j > i; --j)
			{
				Lsolution = solution;
				swap(Lsolution[i], Lsolution[j]);
				if (Price(Lsolution, Pmatr) < Lmin + EPS)
				{
					if (not(BelongsTabuQ(Lsolution, tabuN)))
					{
						Lmin = Price(Lsolution, Pmatr);
						LminSolution = Lsolution;
					}
				}
			}
		}

		if (Lmin < Gmin)
		{
			Gmin = Lmin;
			GminSolution = LminSolution;
			LastMinIt = 0;
		}
		outFile << Lmin << endl;
		AddTabu(tabuN, solution);
		solution = LminSolution;

	} while (LastMinIt != MaxIt);



	outFile.close();

	return GminSolution;

}


int main(int arg, char** argv)
{
	cout << "TabuSearch method \n";
	cout << "---------------------------------- \n";
	vector<double> Pmatr;
	int n;
	InputList(Pmatr, n);
	auto clockS = clock();
	vector<int> sol = TabuSearch(Pmatr, n);
	cout <<"time: " << (float)(clock() - clockS) / CLOCKS_PER_SEC << endl;
	for (auto val : sol) { cout << val << " -> "; };
	cout << sol[0] <<endl <<"Price: " << Price(sol,Pmatr);

}