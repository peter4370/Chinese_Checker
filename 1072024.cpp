#include"pch.h"
#include<fstream>
#include<iostream>
#include<string>
#include<math.h>
#include<time.h>
//#define debug2024 //測試時定義
using namespace std;

int self = 2;
struct place
{
	int x;
	int y;
}myPiece[15];//用來記錄自己棋子的位置
int board[17][17];//存取棋盤
struct place target = { 0,4 };//棋子要到達的目標
struct place reachablePoints[15][128] = {0};//棋子可以到達的點，[棋子][點，0存取的是棋子目前位置]
int prePaths[15][128];
int tmpLengh[15];

void readBoard()//讀取棋盤
{
	ifstream file;
	file.open("board.txt");
	for (int i = 0; i < 17; i++)
		for (int j = 0; j < 17; j++)
			file >> board[i][j];
	file.close();
}
void printBoard()//列出棋盤，測試用
{
	for (int i = 0; i < 17; i++)
	{
		for (int j = 0; j < 17; j++)
		{
			cout << board[i][j];
		}
		cout << endl;
	}
}
void saveBoard()//儲存棋盤，測試用
{
	ofstream file;
	file.open("board.txt");
	for (int i = 0; i < 17; i++)
	{
		for (int j = 0; j < 17; j++)
		{
			file << board[i][j] << ' ';
		}
		file << endl;
	}
	file.close();
}
void scanMyPiece(int self)//尋找自己的棋子並存入myPiece[]
{
	int index = 0;
	for (int i = 0; i < 17; i++)
		for (int j = 0; j < 17; j++)
			if (board[i][j] == self)
			{
				myPiece[index].x = i;
				myPiece[index].y = j;
				index++;
			}
}
int distance(int x1, int y1, int x2, int y2)//用移動所需最短步數來計算兩點之間距離
{
	int deltaX = x2 - x1;
	int deltaY = y2 - y1;
	int result = 17 * 17;
	int z;
	for (z = -17; z < 17; z++)
		if (abs(deltaX-z)+abs(deltaY-z)+abs(z) < result)
			result = abs(deltaX - z) + abs(deltaY - z) + abs(z);
	return result;
}
bool jumpable(int x,int y, int sX, int sY)//測試從(sX, sY)是否可跳到(x,y)
{
	if (sX >= 0 && sX < 17 && sY >= 0 && sY < 17)
	{
		if (board[sX][sY] == 1)
		{
			if ((board[(x + sX) / 2][(y + sY) / 2] != 0) && (board[(x + sX) / 2][(y + sY) / 2] != 1))
				return true;
			else return false;
		}
		else return false;
	}
	else return false;
}
bool backward(int x, int y, int i,int index)//判斷是否走過
{
	for (; i >= 0; i--)
	{
		if (x == reachablePoints[index][i].x && y == reachablePoints[index][i].y)
			return true;
	}
	return false;
}
void setSearch(int x, int y, place *Search,int range)
{
	Search[0].x = x - range;
	Search[0].y = y - range;
	Search[1].x = x - range;
	Search[1].y = y;
	Search[2].x = x;
	Search[2].y = y + range;
	Search[3].x = x + range;
	Search[3].y = y + range;
	Search[4].x = x + range;
	Search[4].y = y;
	Search[5].x = x;
	Search[5].y = y - range;
}
void calculatePath(int index,int x,int y)
{
	int i = 0;
	int preIndex;
	int preStart = 0, preEnd = 0;
	int tmpX, tmpY;
	struct place search[6];
	bool done = false;
	
	reachablePoints[index][0].x = x;
	reachablePoints[index][0].y = y;
	
	while (!done)//找出可以跳的點
	{
		for (preIndex = preStart; preIndex <= preEnd; preIndex++)
		{
			tmpX = reachablePoints[index][preIndex].x;
			tmpY = reachablePoints[index][preIndex].y;
			setSearch(tmpX, tmpY, search,2);
			preStart = i+1;
			for (int j = 0; j < 6; j++)
			{
				if (jumpable(tmpX, tmpY, search[j].x, search[j].y))
				{
					if (!backward(search[j].x, search[j].y, i, index))
					{
						i++;
						reachablePoints[index][i].x = search[j].x;
						reachablePoints[index][i].y = search[j].y;
						prePaths[index][i] = preIndex;
					}
				}
			}
		}
		preEnd = i;
		done = (preEnd == preStart-1) ? true : false;
	}
	tmpX = myPiece[index].x;//搜尋可平移的點
	tmpY = myPiece[index].y;
	for (int j = 0; j < 6; j++)
	{
		setSearch(tmpX, tmpY, search, 1);
		if (search[j].x >= 0 && search[j].x < 17 && search[j].y >= 0 && search[j].y < 17)
		{
			if (board[search[j].x][search[j].y] == 1)
			{
				i++;
				reachablePoints[index][i].x = search[j].x;
				reachablePoints[index][i].y = search[j].y;
				prePaths[index][i] = 0;
			}
		}
	}
	tmpLengh[index] = i;

}
int score(int index, int PathIndex)
{
	
        int tmpLengh_backup[15];
        struct place tmpPaths_backup[15][128];
        int board_backup[17][17];
	for (int i = 0; i < 17; i++)//backup
		for (int j = 0; j < 17; j++)
			board_backup[i][j] = board[i][j];
	for (int i = 0; i < 15; i++)
		for (int j = 0; j < tmpLengh[i]; j++)
			tmpPaths_backup[i][j] = reachablePoints[i][j];
	for (int i = 0; i < 17; i++)
		tmpLengh_backup[i] = tmpLengh[i];
	int result = 0;
	board[myPiece[index].x][myPiece[index].y] = 1;
	board[tmpPaths[index][PathIndex].x][tmpPaths[index][PathIndex].y] = self;

	int i = 0;
	int preIndex;
	int preStart = 0, preEnd = 0;
	int tmpX, tmpY;
	struct place search[6];
	bool done = false;
	reachablePoints[index][0].x = myPiece[index].x;
	reachablePoints[index][0].y = myPiece[index].y;
	for (int testJ = 0; testJ < 15; testJ++)
	{
		if (testJ == index) continue;
		calculatePath(testJ, myPiece[testJ].x, myPiece[testJ].y);
		int max = 0;
		int maxPiece;
		int maxPath;
			for (int j = 1; j<tmpLengh[testJ]; j++)
			{
				if (100 / (distance(reachablePoints[testJ][j].x, reachablePoints[testJ][j].y, target.x, target.y) + 1) - 100 / (distance(myPiece[testJ].x, myPiece[testJ].y, target.x, target.y) + 1) >= max)//用100除以距離來算分,+1避免除以零
				{
					max = 100 / (distance(reachablePoints[testJ][j].x, reachablePoints[testJ][j].y, target.x, target.y) + 1) - 100/(distance(myPiece[testJ].x, myPiece[testJ].y, target.x, target.y)+1);
					maxPiece = testJ;
					maxPath = j;
				}
				//if (distance(myPiece[testJ].x, myPiece[testJ].y, target.x, target.y) - distance(tmpPaths[testJ][j].x, tmpPaths[testJ][j].y, target.x, target.y) >= max)
				//{
				//	max = distance(myPiece[testJ].x, myPiece[testJ].y, target.x, target.y) - distance(tmpPaths[testJ][j].x, tmpPaths[testJ][j].y, target.x, target.y);
				//	maxPiece = testJ;
				//	maxPath = j;
				//}
				if ((distance(reachablePoints[testJ][j].x, reachablePoints[testJ][j].y, target.x, target.y) < 5)&& distance(myPiece[testJ].x, myPiece[testJ].y, target.x, target.y) < 5)
				{
					max *= 10;
					break;
				}
				result += max;
			}
	}

	for (int i = 0; i < 17; i++)//restore
		tmpLengh[i] = tmpLengh_backup[i];
	for (int i = 0; i < 15; i++)
		for (int j = 0; j < tmpLengh[i]; j++)
			reachablePoints[i][j] = tmpPaths_backup[i][j];
	for (int i = 0; i < 17; i++)
		for (int j = 0; j < 17; j++)
			board[i][j] =  board_backup[i][j];
	return result;
}
void movePiece(int piece, int index)
{
	int i = 1;
	int path[128] = { 0 };
	path[0] = index;
	do
	{
		
		path[i] = prePaths[piece][index];
		index = prePaths[piece][index];
		i++;
	} while (index);
	i--;//我不知道為什麼 但有用
	ofstream saveMove;
	saveMove.open("1072024.txt");
	saveMove << i;
	saveMove << '\n';
	for (int j = i; j >=0; j--)
	{
		saveMove << reachablePoints[piece][path[j]].x;
		saveMove << ' ';
		saveMove << reachablePoints[piece][path[j]].y;
		saveMove << '\n';
	}
	saveMove.close();
}
void answer()
{
	for (int i = 0; i < 15; i++)
		calculatePath(i,myPiece[i].x,myPiece[i].y);
	double max=-6000;
	int maxPiece;
	int maxPath;
	for (int i = 0; i < 15; i++)
		for (int j = 1; j < tmpLengh[i]; j++)
			{
			//int tmpMax = score(i,j);
			//double tmpMax = 100. / (distance(tmpPaths[i][j].x, tmpPaths[i][j].y, target.x, target.y) + 1) - 100. / (distance(myPiece[i].x, myPiece[i].y, target.x, target.y) + 1);
			double tmpMax = score(i, j)+6.*(100. / (distance(reachablePoints[i][j].x, reachablePoimts[i][j].y, target.x, target.y) + 1) - 100. / (distance(myPiece[i].x, myPiece[i].y, target.x, target.y) + 1))+rand()%10;
				if (tmpMax >= max)
				{
					if (distance(myPiece[i].x, myPiece[i].y, target.x, target.y) < 5)
						tmpMax /= 5;

					max = tmpMax;
					maxPiece = i;
					maxPath = j;
				}
				if ((distance(myPiece[i].x, myPiece[i].y, target.x, target.y) >= 5)&&(distance(reachablePoints[i][j].x, reachablePoints[i][j].y, target.x, target.y) < 5))//移到目標範圍內
				{
					max += 100;
					maxPiece = i;
					maxPath = j;
				}
			}
	movePiece(maxPiece,maxPath);
#ifdef debug2024
	cout << maxPiece << ' ' << tmpPaths[maxPiece][maxPath].x << ' ' << tmpPaths[maxPiece][maxPath].y << ' ' << max << endl;
	board[myPiece[maxPiece].x][myPiece[maxPiece].y] = 1;
	board[tmpPaths[maxPiece][maxPath].x][tmpPaths[maxPiece][maxPath].y] = self;
	saveBoard();
#endif
}
void debug()
{
#ifdef debug2024

	printBoard();//列出我的棋子在哪
	cout << "myPiece" << endl;
	for (int i = 0; i < 15; i++)
	{
		cout << i << ' ';
		cout << myPiece[i].x << ' ';
		cout << myPiece[i].y << ' ';
		cout << distance(myPiece[i].x, myPiece[i].y, target.x, target.y);
		cout << endl;
	}
#endif
}
void setTarget(int self)
{
	if (self == 2) target = { 0,4 };
	else if (self == 3) target = { 12,16 };
	else if (self == 4) target = { 12,4 };
}
int main(int argc, char *argv[])
{
	srand(time(0));
	if(argc >= 2 )
		self = *argv[1];
	setTarget(self);
#ifdef debug2024
	int steps = 1;
	while(steps++)
	{
		for (self = 2; self <= 4; self++)
		{
			setTarget(self);
			readBoard();
			scanMyPiece(self);
			answer();
			debug();
			cout << steps << endl;
			cout << target.x << ' ' << target.y << endl;
			system("pause");
		}
	}
#endif
	readBoard();
	scanMyPiece(self);
	answer();
}
