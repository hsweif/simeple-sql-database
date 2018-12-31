#include "RecordModule/RID.h"
using namespace std;
RID::RID(int pageNum, int slotNum) {
	mPageNum = pageNum;
	mSlotNum = slotNum;
}

RID::RID() {
	mPageNum = -1;
	mSlotNum = -1;
}

RID::~RID() {

}

bool RID::operator != (const RID &b)
{
	if(this->mPageNum != b.mPageNum) {
		return true;
	}
	else if(this->mSlotNum != b.mSlotNum) {
		return true;
	}
	else{
		return false;
	}
}

ostream& operator << (ostream &output, RID& rid)
{
	output << "RID: (" << rid.mPageNum << "," << rid.mSlotNum << ")";
	return output;
}

int RID::GetPageNum(int &pageNum) const {
	if (mPageNum == -1 || mSlotNum == -1)
		return 1;
	pageNum = mPageNum;
	return 0;
}

int RID::GetSlotNum(int &slotNum) const {
	if (mPageNum == -1 || mSlotNum == -1)
		return 1;
	slotNum = mSlotNum;
	return 0;
}

BufType RID::Serialize(){
	BufType buf = new uint[2];
	buf[0] = (uint)mPageNum;
	buf[1] = (uint)mSlotNum;
	return buf;
}

int RID::Deserialize(BufType buf){
	mPageNum = (int)buf[0];
	mSlotNum = (int)buf[1];
	return 0;
}void RID::show(){
	cout<<mPageNum<<"|"<<mSlotNum<<endl;
}
