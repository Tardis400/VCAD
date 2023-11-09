#include "stdafx.h"
#include "math.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "MainFrm.h"
#include "Entity.h"
#include "Command.h"
#include "CreateCmd.h"
#include "Windows.h"
#include "TextInputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDeleteComb::CDeleteComb()
	: m_LeftTop(0, 0), m_RightBottom(0, 0)
{
	m_nStep = 0; // ��ʼ��������Ϊ 0
}

CDeleteComb::~CDeleteComb()
{
}

int CDeleteComb::GetType()
{
	return ctCreateComb;
}

int	CDeleteComb::OnLButtonDown(UINT nFlags, const Position& pos)
{
	CDC* pDC = g_pView->GetDC(); // �õ��豸����ָ�� 
	CTextInputDlg dlg1;
	dlg1.DoModal();

	CComb* oldComb = NULL;    // �½�һ��CComb���͵�ָ���������ֵΪNULL
	int ListLength_1 = g_pDoc->m_EntityList.GetCount();
	MEntity* SYentityHead_beginning = (MEntity*)g_pDoc->m_EntityList.GetHead();
	POSITION position = g_pDoc->m_EntityList.GetHeadPosition();  // position���ڴ���ͼԪ����Ŀ�ʼ��ַ
	while (position != NULL) {  // ��position��Ϊ�գ���ͼԪ�����д���������ʱ������whileѭ��
		MEntity* entity = (MEntity*)g_pDoc->m_EntityList.GetAt(position);  // ����ͼԪ�ĵ�ַѭ������ͼԪ
		if (entity->GetType() == etComb) {  // ���entity����������etComb�����if
			CComb* comb = (CComb*)entity;  // Ϊ�˱���entity����entity������comb
			if (!comb->GetName().Compare(dlg1.m_text)) // ��comb��name��dlg1�Ի�����������ֽ��жԱ�
			{
				oldComb = (CComb*)entity;   // ��comb��name��dlg1�Ի������������һ��ʱ����entity������oldComb
				break;
			}
		}
		g_pDoc->m_EntityList.GetNext(position); // ǰ����һ��ͼԪ�ĵ�ַ

	}
	if (oldComb == NULL) {
		char msg[256];
		sprintf(msg, "���������: %s", dlg1.m_text);
		MessageBox(NULL, msg, "����", MB_OK);
		return 0;
	}

	std::vector<MEntity*> entities = oldComb->GetEntities();
	for (MEntity* entity : entities) {
		MEntity* newEntity = entity->Copy();
		newEntity->Draw(pDC, dmNormal);
		g_pDoc->m_EntityList.AddTail(newEntity);
		newEntity->m_nOperationNum = g_pView->m_nCurrentOperation;
	}
	g_pDoc->m_EntityList.RemoveAt(position);  // ����Ի�������ƥ���ָ����ϴ�ͼԪ�б�ɾ��
	g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;

	g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��

	return 0;
}

int	CDeleteComb::OnMouseMove(UINT nFlags, const Position& pos)
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_DRAW_RECT));

	// ��һ��̬����nPreRefresh��¼����OnMouseMove״̬ʱ��ˢ�´���
	static	int nPreRefresh = g_nRefresh;
	// ��������bRefresh˵����OnMouseMove�������Ӵ��Ƿ�ˢ��
	BOOL	bRefresh = FALSE;
	// nCurRefresh���ڼ�¼��ǰ��ˢ�´���
	int		nCurRefresh = g_nRefresh;
	// ���nCurRefresh��nPreRefresh����ȣ�˵���Ӵ�����ˢ�¹�
	if (nCurRefresh != nPreRefresh) {
		bRefresh = TRUE;
		nPreRefresh = nCurRefresh;
	}

	switch (m_nStep)
	{
	case 0:
		::Prompt("��������ε����Ͻǵ㣺");
		break;
	case 1:
	{
		Position	prePos, curPos;
		prePos = m_RightBottom; // ���������ڵ�ǰһ��λ��

		curPos = pos;

		CDC* pDC = g_pView->GetDC(); // �õ��豸����ָ��

		// ������ʱ���������һ����Ƥ��
		MRectangle* pTempRect = new MRectangle(m_LeftTop, prePos);
		if (!bRefresh) // ���Ӵ�û�б�ˢ��ʱ���ػ�ԭ������Ƥ��ʹ�䱻����
			pTempRect->Draw(pDC, dmDrag);
		delete pTempRect;
		// ������ʱ���󣬸��ݵ�ǰλ�û���һ����Ƥ��
		MRectangle* pTempRect2 = new MRectangle(m_LeftTop, curPos);
		pTempRect2->Draw(pDC, dmDrag);

		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��			

		m_RightBottom = curPos; // ����ǰλ������Ϊֱ���յ㣬�Ա���һ������ƶ�ʱ��
		break;
	}
	}
	return 0;
}
// ��������Ҽ�ȡ����ǰ�Ĳ���
int	CDeleteComb::OnRButtonDown(UINT nFlags, const Position& pos)
{
	// �����ǰ�Ĳ�����Ϊ 1 ����ôҪ�ڽ������β���ǰ�����ϴ�����ƶ�ʱ���Ƶ���Ƥ��
	if (m_nStep == 1) {
		CDC* pDC = g_pView->GetDC(); // �õ��豸����ָ��
		Position	prePos = m_RightBottom; // ���������ڵ�ǰһ��λ��
		MRectangle* pTempRect = new MRectangle(m_LeftTop, m_RightBottom);
		pTempRect->Draw(pDC, dmDrag); // ������һ�λ��Ƶ���Ƥ��
		delete pTempRect;
		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	}
	m_nStep = 0; // ������������Ϊ 0 
	::Prompt("��������ε����Ͻǵ㣺");
	return 0;
}
// ����Cancel ����ȡ�����β���
int CDeleteComb::Cancel()
{
	// �����ǰ�Ĳ�����Ϊ 1 ����ôҪ�ڽ������β���ǰ�����ϴ�����ƶ�ʱ���Ƶ���Ƥ��
	if (m_nStep == 1) {
		CDC* pDC = g_pView->GetDC(); // �õ��豸����ָ��
		Position	prePos = m_RightBottom; // ���������ڵ�ǰһ��λ��
		MRectangle* pTempRect = new MRectangle(m_LeftTop, m_RightBottom);
		pTempRect->Draw(pDC, dmDrag); // ������һ�λ��Ƶ���Ƥ��
		delete pTempRect;
		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	}
	m_nStep = 0; // ������������Ϊ 0 
	::Prompt("����"); // �ȴ���ʾ�����͵��������
	return 0;
}

