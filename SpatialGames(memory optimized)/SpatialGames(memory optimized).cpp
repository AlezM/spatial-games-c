#include <iostream>
#include <math.h>
#include <vector>
#include <ctime>
#include <fstream>

#include <time.h>

using namespace std;

//Type: fasle - Cooperator, true - Deffector
class Tile {
public:
	bool type;
	double score;
	Tile() : type(false), score(0) {};
	Tile(bool _type) : type(_type), score(0) {};

	void InterractWith(Tile member, double b) {
		if (member.type == 0) {
			if (type == 0)
				score += 1;
			else
				score += b;
		}
	}

	void SetType(bool _type) {
		type = _type;
	}
};

int main() {
	FILE *file;
	clock_t start, finish;
	char file_name[64];

// Main program
	int mapSize = 4096;
	double b = 1.81;
	vector<Tile> map;
	vector<Tile> newMap;

	double defectorsSpawnPercentage = 21;
	string seed;

	int borderMapScale = 10;
	vector<bool> borderMap;

	int startSize = 1;
	int finishSize = 500;
	int step = 1;

	sprintf(file_name, "%i_%0.2f.txt", mapSize, b);
	file = fopen(file_name, "w");

	map.resize(mapSize*mapSize, Tile(0));
	borderMap.resize(mapSize*mapSize, false);

// SetUp
	srand(time(0));
	for (int i = 0; i < mapSize*mapSize; i++) {
		map[i] = (1 + rand() % 100 < 21) ? Tile(true) : Tile(false);
	}
	cout << "> Map set up." << endl;

////////////////////////////////

// Generations
	for (int i = 0; i < 1000; i++) {
		start = clock();


		//Scores
		for (int i = 0; i < mapSize*mapSize; i++) {
			for (int x = -1; x < 2; x++) {
				for (int y = -1; y < 2; y++) {
					int memberX = ((i + x) + mapSize) % mapSize;
					int memberY = (((i / mapSize) + y) + mapSize) % mapSize;

					map[i].InterractWith(map[memberX + memberY * mapSize], b);
				}
			}
		}

		newMap.insert(newMap.begin(), map.begin(), map.end());

		for (int i = 0; i < mapSize; i++)
		{
			for (int j = 0; j < mapSize; j++)
			{
				//Search for the reachest member
				int newType = map[i + j * mapSize].type;
				double maxMemberScore = 0;
				bool uncertainty = false;
				for (int x = -1; x <= 1; x++)
				{
					for (int y = -1; y <= 1; y++)
					{
						int memberIndex = (i + x + mapSize) % mapSize + (j + y + mapSize) % mapSize * mapSize;

						double memberScore = map[memberIndex].score;
						int memberType = map[memberIndex].type;

						if (memberScore > maxMemberScore)
						{
							maxMemberScore = memberScore;
							newType = memberType;
							uncertainty = false;
						}
						else if (memberScore == maxMemberScore && newType != memberType)
						{
							uncertainty = true;
						}
					}
				}
				newMap[i + j * mapSize] = Tile(newType);
			}
		}

		for (int i = 0; i < mapSize*mapSize; i++) {
			newMap.at(i).score = 0;
		}

		map.clear();
		map = newMap;
		newMap.clear();

		finish = clock();
		printf("Time for step %i: %f\n", i, ((double)(finish - start)) / CLOCKS_PER_SEC);
		fprintf(file, "%f,", ((double)(finish - start)) / CLOCKS_PER_SEC);
	}

	cout << "> Map ready." << endl;

	return 0;

////////////////////////////////

// BorderMap

	int borderMapSize = borderMapScale * mapSize;

	borderMap.resize(borderMapSize * borderMapSize);

	for (int i = 0; i < mapSize; i++)
	{
		for (int j = 0; j < mapSize; j++)
		{
			if (map[i + j * mapSize].type != map[(i - 1 + mapSize) % mapSize + j * mapSize].type)
			{
				for (int k = borderMapScale * j; k < borderMapScale * (j + 1) + 1; k++)
				{
					borderMap[i * borderMapScale + k % borderMapSize * mapSize] = true;
				}
			}
		}
	}

	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			if (map[j + i * mapSize].type != map[j + (i - 1 + mapSize) % mapSize * mapSize].type) {
				for (int k = borderMapScale * j; k < borderMapScale * (j + 1) + 1; k++) {
					borderMap[k % borderMapSize + i * borderMapScale * borderMapScale] = true;
				}
			}
		}
	}

	cout << "> Border map ready." << endl;


	map.clear();

////////////////////////////////



	vector<bool> filledBoxes;
	ofstream fout("out_179_20k.txt", ios_base::trunc);

	filledBoxes.resize(borderMapSize * borderMapSize);

	for (int b = startSize; b <= finishSize; b += step)
	{
		// Filling Boxes
		size_t hCount = borderMapSize / b; //Hight
		size_t wCount = borderMapSize / b; //Width	
								  

		for (size_t x = 0; x < borderMapSize; x++) {
			for (size_t y = 0; y < borderMapSize; y++) {
				if (borderMap[x + y * borderMapSize]) {
					size_t xBox = x / b;
					size_t yBox = y / b;
					filledBoxes[xBox + yBox * borderMapSize] = true;
				}
			}
		}

		// Counting Boxes
		size_t a = 0;
		size_t fbSize = sqrt(filledBoxes.size());
		for (size_t i = 0; i < fbSize; i++) {
			for (size_t j = 0; j < fbSize; j++) {
				if (filledBoxes[i + j * fbSize]) {
					a++;
				}
			}
		}

		for (size_t x = 0; x < borderMapSize; x++) {
			for (size_t y = 0; y < borderMapSize; y++) {
				filledBoxes[x + y * borderMapSize] = false;
			}
		}

		cout << "|";

		fout << "{" << b << ", " << a << "}," << endl;
	}
	cout << "\nComplete.\n";
	fout.close();


	return 0;
}