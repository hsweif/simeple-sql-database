#include <iostream>
using namespace std;
int main(int argc, char const *argv[])
{
	MyBitMap *b = new MyBitMap(10<<5,1);
	b->show();
	b->setBit(1,1);
	b->setBit(3,1);
	b->show();
	b->initConst();
	cout<<b->findLeftOne()<<endl;
	return 0;
}