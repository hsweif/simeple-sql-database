#include "../include/RecordModule/RID.h"
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

