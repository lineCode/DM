#include "DmMainAfx.h"
#include "DMMsgThunk.h"

namespace DM
{
	// DMMsgThunk--------------------------------
	void DMMsgThunk::Init(DWORD_PTR proc, void* pThis)
	{
		m_mov = 0x042444C7;// C7 44 24 0C
		m_this = PtrToUlong(pThis);  // mov [esp+4], pThis;��esp+4�����Ƿ�hWnd,���ڱ�͵�ŷŶ���ָ����.
		m_jmp = 0xe9;
		m_relproc = (DWORD)((INT_PTR)proc - ((INT_PTR)this + sizeof(DMMsgThunk)));

		// write block from data cache and
		// flush from instruction cache
		FlushInstructionCache(GetCurrentProcess(), this, sizeof(DMMsgThunk));
	}
	void* DMMsgThunk::GetCodeAddress()
	{
		return this;
	}

	// DMMsgThunkTool-----------------------------
	DMMsgThunkTool::DMMsgThunkTool()
	{
		m_hHeap		 = ::HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 0, 0);
		m_pThunk	 = NULL;
	}

	DMMsgThunkTool::~DMMsgThunkTool()
	{
		MsgThunkUnInit();
		if (m_hHeap)
		{
			::HeapDestroy(m_hHeap);
			m_hHeap = NULL;
		}
	}

	bool DMMsgThunkTool::MsgThunkInit()
	{
		bool bRet = false;
		do 
		{
			if (m_pThunk
				||NULL == m_hHeap)
			{
				break;
			}

			m_pThunk = (DMMsgThunk*)HeapAlloc(m_hHeap, HEAP_ZERO_MEMORY, sizeof(DMMsgThunk));
			bRet = (NULL!=m_pThunk);
		} while (false);
		return bRet;
	}

	bool DMMsgThunkTool::MsgThunkRun(HWND hWnd, DWORD_PTR proc, PVOID pThis)
	{
		bool bRet = false;
		do 
		{
			if (NULL == m_pThunk
				|| NULL == pThis)
			{
				break;
			}

			m_pThunk->Init(proc, pThis);
			WNDPROC pProc = (WNDPROC)m_pThunk->GetCodeAddress();// �õ�Thunkָ��

			// ��������������Ժ���Ϣ���ˣ�����pProc����
			::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)pProc);
			bRet = true;
		} while (false);
		return bRet;
	}

	void DMMsgThunkTool::MsgThunkUnInit()
	{
		if (m_pThunk)
		{
			::HeapFree(m_hHeap,0,m_pThunk);
			m_pThunk = NULL;
		}
	}

	PVOID DMMsgThunkTool::MsgThunkGetCodeAddress()
	{
		return m_pThunk->GetCodeAddress();
	}

}//namespace DM