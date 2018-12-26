#include"pch.h"
#include<fstream>
#include<iostream>
#include<string>
#include<math.h>
#include<time.h>
#define debug2024 //測試時定義
using namespace std;

int self = 2;//記錄自己的顏色
struct place
{
	int x;
	int y;
}myPiece[15];//用來記錄自己棋子的位置
int board[17][17];//存取棋盤
struct place target = { 0,4 };//棋子要到達的目標
struct place shortTermTarget;
struct place reachablePoints[15][128] = { 0 };//棋子可以到達的點，[棋子][點，0存取的是棋子目前位置]
int prePaths[15][128];//紀錄每個可以到達點的前一個位置index
int tmpLengh[15];//reachablePoints[棋子]的長度

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
		if (abs(deltaX - z) + abs(deltaY - z) + abs(z) < result)
			result = abs(deltaX - z) + abs(deltaY - z) + abs(z);
	return result;
}
bool jumpable(int x, int y, int sX, int sY)//測試從(sX, sY)是否可跳到(x,y)
{
	if (sX >= 0 && sX < 17 && sY >= 0 && sY < 17)
	{
		if (board[sX][sY] == 1)
		{
			if ((board[(x + sX) / 2][(y + sY) / 2] != 0) && (board[(x + sX) / 2][(y + sY) / 2] != 1))
			{
				return true;
			}
		}
	}
	return false;
}
bool isBackward(int x, int y, int i, int piece)//判斷是否走過
{
	for (; i >= 0; i--)
	{
		if (x == reachablePoints[piece][i].x && y == reachablePoints[piece][i].y)
			return true;
	}
	return false;
}
void setSearch(int x, int y, place *Search, int range)
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
void findReachablePoints(int index, int x, int y)
{
	int i = 0;
	int preIndex;
	int preStart = 0, preEnd = 0;
	int tmpX, tmpY;
	struct place search[6];
	bool isDone = false;

	reachablePoints[index][0].x = x;
	reachablePoints[index][0].y = y;

	while (!isDone)//找出可以跳的點
	{
		for (preIndex = preStart; preIndex <= preEnd; preIndex++)
		{
			tmpX = reachablePoints[index][preIndex].x;
			tmpY = reachablePoints[index][preIndex].y;
			setSearch(tmpX, tmpY, search, 2);
			for (int j = 0; j < 6; j++)
			{
				if (jumpable(tmpX, tmpY, search[j].x, search[j].y))
				{
					if (!isBackward(search[j].x, search[j].y, i, index))
					{
						i++;
						reachablePoints[index][i].x = search[j].x;
						reachablePoints[index][i].y = search[j].y;
						prePaths[index][i] = preIndex;
					}
				}
			}
		}
		preStart = preEnd + 1;//下一輪的起點就是上一輪終點+1
		preEnd = i;
		isDone = (preEnd == preStart - 1) ? true : false;
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
	tmpLengh[index] = i;//rechablePoints資料長度
}
int score(int index, int PathIndex)
{

	int tmpLengh_backup[15];
	struct place reachablePoints_backup[15][128];
	int board_backup[17][17];

	for (int i = 0; i < 17; i++)//backup
		for (int j = 0; j < 17; j++)
			board_backup[i][j] = board[i][j];
	for (int i = 0; i < 15; i++)
		for (int j = 0; j < tmpLengh[i]; j++)
			reachablePoints_backup[i][j] = reachablePoints[i][j];
	for (int i = 0; i < 15; i++)
		tmpLengh_backup[i] = tmpLengh[i];

	board[myPiece[index].x][myPiece[index].y] = 1;
	board[reachablePoints[index][PathIndex].x][reachablePoints[index][PathIndex].y] = self;

	double result = 1;
	int i = 0;
	int preIndex;
	int preStart = 0, preEnd = 0;
	int tmpX, tmpY;
	struct place search[6];
	bool done = false;
	reachablePoints[index][0].x = myPiece[index].x;
	reachablePoints[index][0].y = myPiece[index].y;
	for (int piece = 0; piece < 15; piece++)
	{
		if (piece == index) continue;
		findReachablePoints(piece, myPiece[piece].x, myPiece[piece].y);
		//下了這一步，找到其他棋子可移動的點，並存到reachablePoints[piece]
		double max = 0;
		for (int j = 1; j < tmpLengh[piece]; j++)
		{
			if (100 / (distance(reachablePoints[piece][j].x, reachablePoints[piece][j].y, target.x, target.y) + 1) - 100 / (distance(myPiece[piece].x, myPiece[piece].y, target.x, target.y) + 1) >= max)//用100除以距離來算分,+1避免除以零
			{
				//移動後的距離目標的倒數-移動前距離目標的倒數 最後再乘上100
				max = 100 / (distance(reachablePoints[piece][j].x, reachablePoints[piece][j].y, target.x, target.y) + 1) - 100 / (distance(myPiece[piece].x, myPiece[piece].y, target.x, target.y) + 1);
			}
			//if (distance(myPiece[piece].x, myPiece[piece].y, target.x, target.y) - distance(tmpPaths[piece][j].x, tmpPaths[piece][j].y, target.x, target.y) >= max)
			//{
			//	max = distance(myPiece[piece].x, myPiece[piece].y, target.x, target.y) - distance(tmpPaths[piece][j].x, tmpPaths[piece][j].y, target.x, target.y);
			//	maxPiece = piece;
			//	maxPath = j;
			//}
			if ((distance(reachablePoints[piece][j].x, reachablePoints[piece][j].y, target.x, target.y) < 5) && distance(myPiece[piece].x, myPiece[piece].y, target.x, target.y) < 5)
			{
				//假如跳到目標區域分數乘10
				max *= 10;
			}
		}
		result += max;
	}

	for (int i = 0; i < 15; i++)//還原中間修改的變數
		tmpLengh[i] = tmpLengh_backup[i];
	for (int i = 0; i < 15; i++)
		for (int j = 0; j < tmpLengh[i]; j++)
			reachablePoints[i][j] = reachablePoints_backup[i][j];
	for (int i = 0; i < 17; i++)
		for (int j = 0; j < 17; j++)
			board[i][j] = board_backup[i][j];
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
	saveMove << i << endl;
	for (int j = i; j >= 0; j--)
	{
		saveMove << reachablePoints[piece][path[j]].x;
		saveMove << ' ';
		saveMove << reachablePoints[piece][path[j]].y;
		saveMove << '\n';
	}
	saveMove.close();
}
void findShortTermTarget(int x, int y);
void answer()
{
	//找出目標區域的空位 並設為shortTermTarget
	findShortTermTarget(target.x, target.y);
#ifdef debug2024
	cout << "shortTermTarget:" << shortTermTarget.x << ' ' << shortTermTarget.y << endl;
#endif // 

	for (int i = 0; i < 15; i++)
		findReachablePoints(i, myPiece[i].x, myPiece[i].y);
	double max = -6000;
	int maxPiece;
	int maxPath;
	for (int i = 0; i < 15; i++)
		for (int j = 1; j <= tmpLengh[i]; j++)
		{
			double tmpMax = score(i, j)*(100. / (distance(reachablePoints[i][j].x, reachablePoints[i][j].y, shortTermTarget.x, shortTermTarget.y) + 1) - 100. / (distance(myPiece[i].x, myPiece[i].y, shortTermTarget.x, shortTermTarget.y) + 1));
			tmpMax *= (rand() % 100 + 100) / 100;
			if (distance(myPiece[i].x, myPiece[i].y, target.x, target.y) < 5)
				tmpMax = 0;
			if ((distance(myPiece[i].x, myPiece[i].y, target.x, target.y) >= 5) && (distance(reachablePoints[i][j].x, reachablePoints[i][j].y, target.x, target.y) < 5))//移到目標範圍內
				tmpMax *= 10;//假如跳入目標範圍內 分數乘以10
			if ((distance(myPiece[i].x, myPiece[i].y, target.x, target.y) < 5) && (distance(reachablePoints[i][j].x, reachablePoints[i][j].y, target.x, target.y) >= 5))//移到目標範圍外
				tmpMax = 0;//假如跳出目標範圍內 分數乘以0
			if (tmpMax >= max)
			{
				max = tmpMax;
				maxPiece = i;
				maxPath = j;
			}
		}
	movePiece(maxPiece, maxPath);
#ifdef debug2024
	cout << maxPiece << ' ' << reachablePoints[maxPiece][maxPath].x << ' ' << reachablePoints[maxPiece][maxPath].y << ' ' << max << endl;
	board[myPiece[maxPiece].x][myPiece[maxPiece].y] = 1;
	board[reachablePoints[maxPiece][maxPath].x][reachablePoints[maxPiece][maxPath].y] = self;
	saveBoard();
#endif
}
void debug()
{
#ifdef debug2024

	printBoard();
	//cout << "myPiece" << endl;//列出我的棋子在哪
	//for (int i = 0; i < 15; i++)
	//{
	//	cout << i << ' ';
	//	cout << myPiece[i].x << ' ';
	//	cout << myPiece[i].y << ' ';
	//	cout << distance(myPiece[i].x, myPiece[i].y, target.x, target.y);
	//	cout << endl;
	//}
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
	if (argc >= 2)
		self = *argv[1];
	setTarget(self);
#ifdef debug2024
	int steps = 1;
	while (steps++)
	{
		cout << steps << endl;
		for (self = 2; self <= 4; self++)
		{
			setTarget(self);
			readBoard();
			scanMyPiece(self);
			answer();
			debug();
			//system("pause");
		}
	}
#endif
	readBoard();
	scanMyPiece(self);
	answer();
}
void findShortTermTarget(int x, int y)

{
	struct place tmpTarget[15];
	int index = 0;
	for (int i = 0; i < 17; i++)//找出可以設為短期目標的位置
		for (int j = 0; j < 17; j++)
			if (distance(i, j, target.x, target.y) < 5 && //與目標的距離小於等於5
				board[i][j] != self && board[i][j] != 0)
			{
				tmpTarget[index] = { i,j };
				index++;
			}
	int min = 0;
	for (int i = 0; i <= index; i++)
	{
		if (distance(tmpTarget[i].x, tmpTarget[i].y, target.x, target.y) < distance(tmpTarget[min].x, tmpTarget[min].y, target.x, target.y))
			min = i;
	}
	shortTermTarget = tmpTarget[min];
}
