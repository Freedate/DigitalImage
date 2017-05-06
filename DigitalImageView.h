
// DigitalImageView.h : CDigitalImageView 클래스의 인터페이스
//

#pragma once


class CDigitalImageView : public CView
{
protected: // serialization에서만 만들어집니다.
	CDigitalImageView();
	DECLARE_DYNCREATE(CDigitalImageView)

// 특성입니다.
public:
	CDigitalImageDoc* GetDocument() const;
	
// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CDigitalImageView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBmpLoad();
	afx_msg void OnRgbToHsi();
	afx_msg void OnJpgLoad();
	afx_msg void OnHistogramEqualization();
	afx_msg void OnHistogramEqualizationRgb();
	afx_msg void OnHistogramSpecification();
	void OnHsiToRgb();
	afx_msg void OnAverage3x3();
	afx_msg void OnAverage5x5();
	afx_msg void OnMedian3x3();
	afx_msg void OnMedian5x5();
	void SelectionSort(int *arr, int n);
	afx_msg void OnGaussian3x3();
	afx_msg void OnGaussian5x5();
	afx_msg void OnMask1A();
	afx_msg void OnMask1A32809();
	afx_msg void OnMask1A32810();
	afx_msg void OnMask1A32811();
	afx_msg void OnMask2A();
	afx_msg void OnMask2A32813();
	afx_msg void OnMask2A32814();
	afx_msg void OnMask2A32815();
	afx_msg void OnPrewitt();
	afx_msg void OnSobel();
};

#ifndef _DEBUG  // DigitalImageView.cpp의 디버그 버전
inline CDigitalImageDoc* CDigitalImageView::GetDocument() const
   { return reinterpret_cast<CDigitalImageDoc*>(m_pDocument); }
#endif

