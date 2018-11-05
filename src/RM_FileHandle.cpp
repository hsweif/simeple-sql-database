#include "../include/RecordModule/RM_FileHandle.h"

RM_FileHandle::RM_FileHandle()
{
}

RM_FileHandle::RM_FileHandle(int id, int sz)
{
	this->fileId = id;
	this->recordSize = sz;
}

int RM_FileHandle::GetRec(const RID &rid, RM_Record &rec) const
{
	int page, slot;
	if (rid.GetPageNum(page) > 0 || rid.GetSlotNum(slot) > 0)
	{
		return 1;
	}
	BufType buf = new uint[PAGE_INT_NUM];
	BufType result = new uint[this->recordSize];
	FileManager *fm = new FileManager();
	fm->readPage(this->fileId, page, buf, 0);
	result = &buf[slot * this->recordSize];
	rec.SetRecord(result, this->recordSize, rid);
	return 0;
}

int RM_FileHandle::init(int _fileId, int _recordSize, int _recordPP, int _recordSum, int _pageCnt, BufType _pageMap)
{
	fileId = _fileId;
	recordSize = _recordSize;
	recordPP = _recordPP;
	recordSum = _recordSum;
	pageCnt = _pageCnt;
	pageMap = new MyBitMap((PAGE_INT_NUM - 4) << 5, _pageMap);
	return 0;
}

int RM_FileHandle::InsertRec(const RM_Record &pData, RID &rid)
{
	FileManager *fm = new FileManager();
	int page, slot;
	page = this->pageMap->findLeftOne();
	if (rid.GetPageNum(page) > 0 || rid.GetSlotNum(slot) > 0)
	{
		return 1;
	}
	BufType buf = new uint[PAGE_INT_NUM], data;
	fm->readPage(this->fileId, page, buf, 0);
	slot = this->GetSlot(buf);
	int rSize;
	if (pData.getSize(rSize) || pData.GetData(data))
	{
		return 1;
	}
	for (int i = 0; i < rSize; i++)
	{
		buf[slot * rSize + i] = data[i];
	}
	fm->writePage(this->fileId, page, buf, 0);
	return 0;
}

int RM_FileHandle::DeleteRec(const RID &rid)
{
	return 0;
}

int RM_FileHandle::UpdateRec(const RM_Record &rec)
{
	return 0;
}

/**
 * TODO: 改写成二级跳转
 * 解析传入的页，回传最左边的空位offset
 * @page 传入一页 
 */
int RM_FileHandle::GetSlot(BufType page)
{
	int offset = 0;
	bool found = false;
	while(!found && offset < PAGE_INT_NUM)
	{
		int upper = offset + recordSize;
		bool flag = true;
		for(int i = offset; i < upper; i ++)
		{
			if(page[i] != 0)
			{
				flag = false;
				break;
			}
		}
		if(!flag)
		{
			offset = upper;
		}
		else
		{
			found = true;
			break;
		}
	}
}