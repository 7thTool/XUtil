#ifndef _H_XNODE_HPP_
#define _H_XNODE_HPP_

#pragma once

namespace XUtil {
	class XNode
	{
	public:
	};

	template<class T, class TBase = XNode>
	class XNode2 : public TBase
	{
	protected:
		T* m_pPrev;
		T* m_pNext;
	public:
		XNode2() : m_pPrev(nullptr),m_pNext(nullptr)
		{ 
		}
		~XNode2() 
		{ 
			T* pT = static_cast<T*>(this);
			pT->Empty();
			pT->Free();
		}

		//�����ӽڵ�
		void Empty()
		{
			
		}

		//���ɣ��ӽڵ����з������
		void Free()
		{
			if (m_pPrev) {
				m_pPrev->m_pNext = m_pNext;
			}
			if (m_pNext) {
				m_pNext->m_pPrev = m_pPrev;
			}
			m_pPrev = nullptr;
			m_pNext = nullptr;
		}

		bool IsFree()
		{
			return m_pPrev == nullptr && m_pNext == nullptr;
		}

		T* Prev()
		{
			return m_pPrev;
		}

		T* SetPrev(T* pN)
		{
			m_pPrev = pN;
			return m_pPrev;
		}

		T* Next()
		{
			return m_pNext;
		}

		T* SetNext(T* pN)
		{
			m_pNext = pN;
			return m_pNext;
		}

		bool IsFirst()
		{
			return m_pPrev == nullptr ? true : false;
		}

		bool IsLast()
		{
			return m_pNext == nullptr ? true : false;
		}

		T* Prev(T* pN)
		{
			T* pT = static_cast<T*>(this);
			if (m_pPrev) {
				m_pPrev->SetNext(pN);
			}
			pN->SetPrev(m_pPrev);
			pN->SetNext(pT);
			m_pPrev = pN;
			return m_pPrev;
		}

		T* Next(T* pN)
		{
			T* pT = static_cast<T*>(this);
			if(m_pNext) {
				m_pNext->SetPrev(pN);
			}
			pN->SetPrev(pT);
			pN->SetNext(m_pNext);
			m_pNext = pN;
			return m_pNext;
		}

		T* First()
		{
			T* pT = static_cast<T*>(this);
			if (m_pPrev) {
				T* pPrev = m_pPrev;
				while(pPrev->Prev())
				{
					pPrev = pPrev->Prev();
				}
				return pPrev;
			}
			return pT;
		}

		T* First(T* pN)
		{
			T* pT = static_cast<T*>(this);
			return pT->First()->Prev(pN);
		}

		T* Last()
		{
			T* pT = static_cast<T*>(this);
			if (m_pNext) {
				T* pNext = m_pNext;
				while(pNext->Next())
				{
					pNext = pNext->Next();
				}
				return pNext;
			}
			return pT;
		}

		T* Last(T* pN)
		{
			T* pT = static_cast<T*>(this);
			return pT->Last()->Next(pN);
		}
	};

	template<class T,class TBase>
	class XNode4 : public XNode2<T,TBase>
	{
		typedef XNode2<T,TBase> Base;
	protected:
		T* m_pChild;
		T* m_pParent;
	public:
		XNode4() : Base(), m_pChild(0),m_pParent(0)
		{ 
		}
		~XNode4() 
		{ 
			
		}

		//�����ӽڵ�
		void Empty()
		{
			while (T* pC = Child()) 
			{
				pC->Empty();
				pC->Free();
			}
		}

		//���ɣ��ӽڵ����з������
		void Free()
		{
			T* pT = static_cast<T*>(this);
			if (m_pParent) {
				if (m_pParent->Child() == pT) {
					m_pParent->SetChild(m_pPrev ? m_pPrev : m_pNext);
				}
				m_pParent = nullptr;
			}
			Base::Free();
		}

		T* Child()
		{
			return m_pChild;
		}

		T* SetChild(T* pN)
		{
			m_pChild = pN;
			return m_pChild;
		}

		T* Parent()
		{
			return m_pParent;
		}

		T* SetParent(T* pN)
		{
			m_pParent = pN;
			return m_pParent;
		}

		T* Root()
		{
			if (m_pParent == nullptr) {
				return static_cast<T*>(this);
			}
			return m_pParent->Root();
		}

		bool IsRoot()
		{
			return m_pParent == nullptr ? true : false;
		}

		bool IsLeaf()
		{
			return m_pChild == nullptr ? true : false;
		}

		T* Child(T* pN)
		{
			T* pT = static_cast<T*>(this);
			pN->SetParent(pT);
			if(m_pChild) {
				m_pChild->Prev(pN);
			}
			m_pChild = pN;
			return m_pChild;
		}

		T* FirstChild()
		{
			if (m_pChild) {
				return m_pChild->First();
			}
			return nullptr;
		}

		T* FirstChild(T* pN)
		{
			T* pT = static_cast<T*>(this);
			if (m_pChild) {
				m_pChild->First(pN);
			} else {
				return Child(pN);
			}
		}

		T* LastChild()
		{
			if (m_pChild) {
				return m_pChild->Last();
			}
			return nullptr;
		}

		T* LastChild(T* pN)
		{
			T* pT = static_cast<T*>(this);
			if (m_pChild) {
				return m_pChild->Last(pN);
			} else {
				return Child(pN);
			}
		}

		T* Prev()
		{
			return Base::Prev();
		}

		T* Prev(T* pN)
		{
			T* pT = static_cast<T*>(this);
			pN->SetParent(pT);
			return Base::Prev(pN);
		}

		T* Next()
		{
			return Base::Next();
		}

		T* Next(T* pN)
		{
			T* pT = static_cast<T*>(this);
			pN->SetParent(pT);
			return Base::Next(pN);
		}
	};
}

#endif//_H_XNODE_HPP_
