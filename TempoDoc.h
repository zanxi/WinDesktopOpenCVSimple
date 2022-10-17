
// TempoDoc.h : interface of the CTempoDoc class
//


#pragma once

#include <opencv2/opencv.hpp>

#include "dav_class.h"


#define SHARED_HANDLERS

#define FRAME_SCALE							1	// divide frame dimentions by this number

class CTempoDoc : public CDocument
{
	int CV_CAP_PROP_FRAME_COUNT = 4;
	int CV_CAP_PROP_POS_FRAMES = 7;
	int CV_CAP_PROP_FPS = 1;

protected: // create from serialization only
	CTempoDoc();
	DECLARE_DYNCREATE(CTempoDoc)

public:
	cv::VideoCapture m_Video;
	    DAV * m_VideoDav;
		int cadrCount=1;

		std::map<int, std::vector< std::vector<cv::Point>>> naborCadrs;
		std::vector<int> numCadrs;
		int numcalccadrCount = 1;

// Attributes
public:
	BOOL m_bErase;
	BITMAPINFO* m_pBmi;
	cv::Mat m_Mat;

    LPCTSTR lpszPathName;

// Operations
public:
	void ShowNextFrame();
	void ShowNextFrameNazad();
	void ShowNextFrameVpered();

	void ShowNextFrameNazad_Jump();
	void ShowNextFrameVpered_Jump();

	void ShowNextFrameDav();

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CTempoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void ResizeMat(cv::Mat& mat);
	void SetupBitmapInfo(cv::Mat& mat);
	void debugSquares(std::vector<std::vector<cv::Point> > squares, cv::Mat image);
	   

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
