/*********************************************************************




        File:       FieldContainer.h

        Author:     Robin
        Date:       2017-06-07


**********************************************************************/

#ifndef __FIELDCONTAINER_H__
#define __FIELDCONTAINER_H__

#include "FieldBase.h"


#include <vector>

#include "typedef.h"
#include "field.h"



using namespace std;

class CFieldContainer : public CFieldBase
{
public:

	virtual void addField(CFieldBase *pField);



	CFieldContainer(string name);

	~CFieldContainer();

	virtual u32 parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset);

    virtual void showData();


private:

protected:
	vector<CFieldBase *> m_vecField;

	u32 m_u32CurrentOffset;

};


typedef enum {
    EN_FIELDCONTAINER_TYPE_COMMON,
    EN_FIELDCONTAINER_TYPE_NAME_STR
}EN_FIELDCONTAINER_TYPE;

class CFieldLoopContainer : public CFieldContainer
{
public:
        CFieldLoopContainer(string name);
        CFieldLoopContainer(string name, u32 u32LeftBit);
        CFieldLoopContainer(string name, u32 u32LeftBit, EN_FIELDCONTAINER_TYPE enType);

        ~CFieldLoopContainer();


        virtual u32 parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset);

        virtual void showData();


        u32 bitlen();

protected:
        u32 m_u32LoopCount;

        u32 m_u32BitLen;

        vector<CFieldBase *> m_vecLoopField;

private:
        u32 getOneLoopBits();
        u32 getOneLoopFieldNum();
        u32 m_u32LeftBit;
        EN_FIELDCONTAINER_TYPE m_enType;

};



typedef enum {
    EN_FIELDLOOPN_TYPE_COMMON,
    EN_FIELDLOOPN_TYPE_NAME_STR
}EN_FIELDLOOPN_TYPE;

class CFieldLoopNContainer : public CFieldContainer
{
public:
        CFieldLoopNContainer(string name, u32 u32BitsLen);
        CFieldLoopNContainer(string name, u32 u32BitsLen, EN_FIELDLOOPN_TYPE type);
        ~CFieldLoopNContainer();


        virtual u32 parserData(const u8 *pData, const u32 u32DataLen, const u32 u32BitOffset);

        virtual void showData();

protected:
        u32 m_u32LoopN;

        CFieldBase *m_pLoopNField;

        vector<CFieldBase *> m_vecLoopField;

        EN_FIELDLOOPN_TYPE m_enFieldLoopNType;

private:

};


#endif  // #ifndef __FIELDCONTAINER_H__

