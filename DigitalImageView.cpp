
// DigitalImageView.cpp : implementation of the CDigitalImageView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DigitalImage.h"
#endif

#include "DigitalImageDoc.h"
#include "DigitalImageView.h"
#include <math.h>
#include "Jpegfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDigitalImageView

IMPLEMENT_DYNCREATE(CDigitalImageView, CView)

BEGIN_MESSAGE_MAP(CDigitalImageView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_BMP_LOAD, &CDigitalImageView::OnBmpLoad)
	ON_COMMAND(ID_RGB_TO_HSI, &CDigitalImageView::OnRgbToHsi)
	ON_COMMAND(ID_JPG_LOAD, &CDigitalImageView::OnJpgLoad)
	ON_COMMAND(ID_HISTOGRAM_EQUALIZATION, &CDigitalImageView::OnHistogramEqualization)
	ON_COMMAND(ID_HISTOGRAM_EQUALIZATION_RGB, &CDigitalImageView::OnHistogramEqualizationRgb)
	ON_COMMAND(ID_HISTOGRAM_SPECIFICATION, &CDigitalImageView::OnHistogramSpecification)
	ON_COMMAND(ID_AVERAGE_3X3, &CDigitalImageView::OnAverage3x3)
	ON_COMMAND(ID_AVERAGE_5X5, &CDigitalImageView::OnAverage5x5)
	ON_COMMAND(ID_MEDIAN_3X3, &CDigitalImageView::OnMedian3x3)
	ON_COMMAND(ID_MEDIAN_5X5, &CDigitalImageView::OnMedian5x5)
	ON_COMMAND(ID_GAUSSIAN_3X3, &CDigitalImageView::OnGaussian3x3)
	ON_COMMAND(ID_GAUSSIAN_5X5, &CDigitalImageView::OnGaussian5x5)
	ON_COMMAND(ID_MASK1_A, &CDigitalImageView::OnMask1A)
	ON_COMMAND(ID_MASK1_A32809, &CDigitalImageView::OnMask1A32809)
	ON_COMMAND(ID_MASK1_A32810, &CDigitalImageView::OnMask1A32810)
	ON_COMMAND(ID_MASK1_A32811, &CDigitalImageView::OnMask1A32811)
	ON_COMMAND(ID_MASK2_A, &CDigitalImageView::OnMask2A)
	ON_COMMAND(ID_MASK2_A32813, &CDigitalImageView::OnMask2A32813)
	ON_COMMAND(ID_MASK2_A32814, &CDigitalImageView::OnMask2A32814)
	ON_COMMAND(ID_MASK2_A32815, &CDigitalImageView::OnMask2A32815)

	ON_COMMAND(ID_Prewitt, &CDigitalImageView::OnPrewitt)
	ON_COMMAND(ID_SOBEL, &CDigitalImageView::OnSobel)
	ON_COMMAND(ID_AVI_LOAD, &CDigitalImageView::OnAviLoad)
END_MESSAGE_MAP()

// CDigitalImageView construction/destruction



RGBQUAD** rgbBuffer; //이미지를 저장할 변수
RGBQUAD** rgbBufferHE; //이미지를 저장할 변수
RGBQUAD** rgbBufferFilter; //이미지를 저장할 변수
int imgHeight; //이미지 높이값(세로길이)
int imgWidth; //이미지 너비값(가로길이)
BITMAPINFOHEADER bmpInfo; //BMP 정보
BITMAPFILEHEADER bmpHeader; //BMP 헤더

float** hueBuffer;
float** satuBuffer;
float** intenBuffer;

RGBQUAD** tmpBufferGF3;
//float** bluringBuffer;
RGBQUAD** rgbFilteringBuffer;
float** intenFilteringBuffer;
float** intenFilteringBuffer2;
float** intenFilteringBuffer3;
float** intenFilteringBuffer4;

//float* histogram;
int histogram[256];
int histogramR[256];
int histogramG[256];
int histogramB[256];

int sum_hist[256];
int sum_histR[256];
int sum_histG[256];
int sum_histB[256];

int inv_hist[256];

int viewType;
CString outText;
CString outText2;


RGBQUAD** videoRGB;





CDigitalImageView::CDigitalImageView()
{
	rgbBuffer = nullptr;
	hueBuffer = nullptr;
	satuBuffer = nullptr;
	intenBuffer = nullptr;
	rgbBufferHE = nullptr;

	//bluringBuffer = nullptr;
	rgbFilteringBuffer = nullptr;

	rgbBufferFilter = nullptr;
	intenFilteringBuffer = nullptr;
	intenFilteringBuffer2 = nullptr;
	intenFilteringBuffer3 = nullptr;
	intenFilteringBuffer4 = nullptr;
	//histogram = nullptr;
	outText = "지정되지 않음";
	outText2 = "지정되지 않음";

	videoRGB = nullptr;
}

CDigitalImageView::~CDigitalImageView()
{
	if (rgbBuffer != nullptr) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbBuffer[i];
		}
		delete[] rgbBuffer;
	}
	if (hueBuffer != nullptr) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] hueBuffer[i];
		}
		delete[] hueBuffer;
	}
	if (satuBuffer != nullptr) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] satuBuffer[i];
		}
		delete[] satuBuffer;
	}
	if (intenBuffer != nullptr) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenBuffer[i];
		}
		delete[] intenBuffer;
	}
	/*if (histogram != nullptr) {
	delete[] histogram;
	}*/
	if (rgbBufferHE != nullptr) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbBufferHE[i];
		}
		delete[] rgbBufferHE;
	}
	if (intenFilteringBuffer != nullptr) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer[i];
		}
		delete[] intenFilteringBuffer;
	}
	if (intenFilteringBuffer2 != nullptr) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer2[i];
		}
		delete[] intenFilteringBuffer2;
	}
	if (intenFilteringBuffer3 != nullptr) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer3[i];
		}
		delete[] intenFilteringBuffer3;
	}
	if (intenFilteringBuffer4 != nullptr) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer4[i];
		}
		delete[] intenFilteringBuffer4;
	}

	if (rgbFilteringBuffer != nullptr) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbFilteringBuffer[i];
		}
		delete[] rgbFilteringBuffer;
	}


	if (rgbBufferFilter != nullptr) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbBufferFilter[i];
		}
		delete[] rgbBufferFilter;
	}

	if (tmpBufferGF3 != nullptr) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] tmpBufferGF3[i];
		}
		delete[] tmpBufferGF3;
	}





}

BOOL CDigitalImageView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CDigitalImageView drawing

void CDigitalImageView::OnDraw(CDC* pDC)
{
	CDigitalImageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	POINT p;
	if (rgbBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			for (int j = 0; j < imgWidth; j++) {
				switch (viewType) {
				case 1:
					p.x = j;
					p.y = i;
					pDC->SetPixel(p, RGB(rgbBuffer[i][j].rgbRed, rgbBuffer[i][j].rgbGreen, rgbBuffer[i][j].rgbBlue));
					break;
				case 2:

					p.x = j + imgWidth + 10;
					p.y = i;
					pDC->SetPixel(p, RGB(hueBuffer[i][j], hueBuffer[i][j], hueBuffer[i][j]));

					p.x = j + imgWidth + 10;
					p.y = i + imgHeight + 30;
					pDC->SetPixel(p, RGB(satuBuffer[i][j], satuBuffer[i][j], satuBuffer[i][j]));

					p.x = j;
					p.y = i + imgHeight + 30;
					pDC->SetPixel(p, RGB(intenBuffer[i][j], intenBuffer[i][j], intenBuffer[i][j]));
					break;

				case 3:

					p.x = j + imgWidth + 10 + imgWidth + 10;
					p.y = i + imgHeight + 30;
					pDC->SetPixel(p, RGB(intenFilteringBuffer[i][j], intenFilteringBuffer[i][j], intenFilteringBuffer[i][j]));
					break;

				case 4:
					p.x = j + imgWidth + 10 + imgWidth + 10;
					p.y = i;
					pDC->SetPixel(p, RGB(rgbBufferHE[i][j].rgbRed, rgbBufferHE[i][j].rgbGreen, rgbBufferHE[i][j].rgbBlue));

					break;

				case 5:
					p.x = j + imgWidth + 10 + imgWidth + 10;
					p.y = i + imgHeight + 30;
					pDC->SetPixel(p, RGB(rgbFilteringBuffer[i][j].rgbRed, rgbFilteringBuffer[i][j].rgbGreen, rgbFilteringBuffer[i][j].rgbBlue));
					break;
				case 6:
					p.x = j + imgWidth + 10 + imgWidth + 10;
					p.y = i + imgHeight + 30;
					pDC->SetPixel(p, RGB(intenFilteringBuffer[i][j], intenFilteringBuffer[i][j], intenFilteringBuffer[i][j]));

					p.x = j + imgWidth + 10 + imgWidth + 10;
					p.y = i;
					pDC->SetPixel(p, RGB(intenFilteringBuffer2[i][j], intenFilteringBuffer2[i][j], intenFilteringBuffer2[i][j]));

					break;
				case 7:
					p.x = j + imgWidth + 10 + imgWidth + 10;
					p.y = i + imgHeight + 30;
					pDC->SetPixel(p, RGB(intenFilteringBuffer3[i][j], intenFilteringBuffer3[i][j], intenFilteringBuffer3[i][j]));

					p.x = j + imgWidth + 10 + imgWidth + 10;
					p.y = i;
					pDC->SetPixel(p, RGB(intenFilteringBuffer4[i][j], intenFilteringBuffer4[i][j], intenFilteringBuffer4[i][j]));

					break;

				}

			}
		}
		switch (viewType) {
		case 1:
			p.x = imgWidth / 2;
			p.y = imgHeight + 5;
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->SetTextAlign(TA_CENTER);
			pDC->TextOut(p.x,p.y, "ORIGINAL IMAGE");
			break;
		case 2:
			p.x = imgWidth + imgWidth/2 + 10;
			p.y = imgHeight + 5;
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->SetTextAlign(TA_CENTER);
			pDC->TextOut(p.x, p.y, "HUE");

			p.x = imgWidth + imgWidth / 2 + 10;
			p.y = imgHeight + 30 + imgHeight+5;
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->SetTextAlign(TA_CENTER);
			pDC->TextOut(p.x, p.y, "SATURATION");

			p.x = imgWidth / 2;
			p.y = imgHeight + 30 + imgHeight + 5;
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->SetTextAlign(TA_CENTER);
			pDC->TextOut(p.x, p.y, "INTENSITY");
			break;
		case 3:
			p.x = imgWidth + imgWidth + 10 + imgWidth/2 + 10;
			p.y = imgHeight + imgHeight + 30;
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->SetTextAlign(TA_CENTER);
			pDC->TextOut(p.x, p.y, outText);
			break;
		case 4:
			p.x = imgWidth + imgWidth + 10 + imgWidth / 2 + 10;
			p.y = imgHeight + 5;
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->SetTextAlign(TA_CENTER);
			pDC->TextOut(p.x, p.y, outText);
			break;
		case 5:
			p.x = imgWidth + imgWidth + 10 + imgWidth / 2 + 10;
			p.y = imgHeight + imgHeight + 30;
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->SetTextAlign(TA_CENTER);
			pDC->TextOut(p.x, p.y, outText);
			break;
		case 6:
			p.x = imgWidth + imgWidth + 10 + imgWidth / 2 + 10;
			p.y = imgHeight + imgHeight + 30;
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->SetTextAlign(TA_CENTER);
			pDC->TextOut(p.x, p.y, outText);

			p.x = imgWidth + imgWidth + 10 + imgWidth / 2 + 10;
			p.y = imgHeight + 5;
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->SetTextAlign(TA_CENTER);
			pDC->TextOut(p.x, p.y, outText2);
			break;
		case 7:
			p.x = imgWidth + imgWidth + 10 + imgWidth / 2 + 10;
			p.y = imgHeight + imgHeight + 30;
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->SetTextAlign(TA_CENTER);
			pDC->TextOut(p.x, p.y, outText);

			p.x = imgWidth + imgWidth + 10 + imgWidth / 2 + 10;
			p.y = imgHeight + 5;
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->SetTextAlign(TA_CENTER);
			pDC->TextOut(p.x, p.y, outText2);
			break;

		}
	}

}


// CDigitalImageView printing

BOOL CDigitalImageView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDigitalImageView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDigitalImageView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CDigitalImageView diagnostics

#ifdef _DEBUG
void CDigitalImageView::AssertValid() const
{
	CView::AssertValid();
}

void CDigitalImageView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDigitalImageDoc* CDigitalImageView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDigitalImageDoc)));
	return (CDigitalImageDoc*)m_pDocument;
}
#endif //_DEBUG


// CDigitalImageView message handlers


void CDigitalImageView::OnBmpLoad()
{
	CFileDialog dlg(TRUE, ".bmp", NULL, NULL, "Bitmap File (*.bmp)|*.bmp||");
	if (IDOK != dlg.DoModal())
		return;
	CString filename = dlg.GetPathName();
	if (rgbBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbBuffer[i];
		}
		delete[] rgbBuffer;
	}

	CFile file;
	file.Open(filename, CFile::modeRead);
	file.Read(&bmpHeader, sizeof(BITMAPFILEHEADER));
	file.Read(&bmpInfo, sizeof(BITMAPINFOHEADER));
	imgWidth = bmpInfo.biWidth;
	imgHeight = bmpInfo.biHeight;

	rgbBuffer = new RGBQUAD*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		rgbBuffer[i] = new RGBQUAD[imgWidth];
	}
	bool b4byte = false;
	int upbyte = 0;
	if ((imgWidth * 3) % 4 == 0) {
		b4byte = true;
		upbyte = 0;
	}
	else {
		b4byte = false;
		upbyte = 4 - (imgWidth * 3) % 4;
	}

	BYTE data[3];
	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			file.Read(&data, 3);
			rgbBuffer[imgHeight - i - 1][j].rgbBlue = data[0];
			rgbBuffer[imgHeight - i - 1][j].rgbGreen = data[1];
			rgbBuffer[imgHeight - i - 1][j].rgbRed = data[2];
		}
		if (b4byte == false) {
			file.Read(&data, upbyte);
		}
	}
	file.Close();
	viewType = 1;
	Invalidate(TRUE);

}


void CDigitalImageView::OnRgbToHsi()
{
	if (rgbBuffer == NULL) {
		OnBmpLoad();
	}

	hueBuffer = new float*[imgHeight];
	satuBuffer = new float*[imgHeight];
	intenBuffer = new float*[imgHeight];
	intenFilteringBuffer = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		hueBuffer[i] = new float[imgWidth];
		satuBuffer[i] = new float[imgWidth];
		intenBuffer[i] = new float[imgWidth];
		intenFilteringBuffer[i] = new float[imgWidth];
	}
	
	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			float r = rgbBuffer[i][j].rgbRed;
			float g = rgbBuffer[i][j].rgbGreen;
			float b = rgbBuffer[i][j].rgbBlue;
			intenBuffer[i][j] = (r + g + b) / (float)(3 * 255);
			float total = r + g + b;

			r = r / total;
			g = g / total;
			b = b / total;

			satuBuffer[i][j] = 1 - (3 * (r>g ? (g>b ? b : g) : (r>b ? b : r))); 

			if (r == g && g == b) {
				hueBuffer[i][j] = 0;
				satuBuffer[i][j] = 0;
			}
			else {
				total = (float)(0.5*(r - g + r - b) / sqrt((r - g)*(r - g) + (r - b)*(r - b)));
				hueBuffer[i][j] = (float)acos((double)total);
				if (b > g) {
					hueBuffer[i][j] = 6.28f - hueBuffer[i][j];
				}
			}
		}
	}
	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			hueBuffer[i][j] = hueBuffer[i][j] * 255.0f / (float)(3.14 * 2);
			satuBuffer[i][j] = satuBuffer[i][j] * 255.0f;
			intenBuffer[i][j] = intenBuffer[i][j] * 255.0f;
		}
	}
	viewType = 2;
	Invalidate(FALSE);


}

BYTE* LoadJpegFromOpenFile(FILE *fp, BITMAPINFOHEADER *pbh, UINT *pWidth, UINT *pHeight) {
	if (pWidth == NULL || pHeight == NULL) {
		return NULL;
	}
	BYTE *tmp = JpegFile::OpenFileToRGB(fp, pWidth, pHeight);
	if (!tmp) {
		return NULL;
	}
	JpegFile::BGRFromRGB(tmp, *pWidth, *pHeight);
	UINT dw;
	BYTE *pbuf = JpegFile::MakeDwordAlignedBuf(tmp, *pWidth, *pHeight, &dw);
	delete[] tmp;
	if (!pbuf) {
		return NULL;
	}
	JpegFile::VertFlipBuf(pbuf, dw, *pHeight);
	pbh->biSize = sizeof(BITMAPINFOHEADER);
	pbh->biWidth = *pWidth;
	pbh->biHeight = *pHeight;
	pbh->biPlanes = 1;
	pbh->biBitCount = 24;
	pbh->biCompression = BI_RGB;
	pbh->biSizeImage = 0;
	pbh->biXPelsPerMeter = 0;
	pbh->biYPelsPerMeter = 0;
	pbh->biClrUsed = 0;
	pbh->biClrImportant = 0;
	return pbuf;
}




void CDigitalImageView::OnJpgLoad()
{
	CFileDialog dlg(TRUE, ".jpg", NULL, NULL, "Jpeg File (*.jpg)|*.jpg||");
	if (IDOK != dlg.DoModal()) {
		return;
	}
	CString filename = dlg.GetPathName();

	//rgbBuffer이 NULL이 아닌 경우 메모리 해제
	if (rgbBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbBuffer[i];
		}
		delete[] rgbBuffer;
	}
	
	FILE *fp = fopen(filename, "rb");
	BITMAPINFOHEADER pbh;
	UINT w, h;
	BYTE *pbuf = LoadJpegFromOpenFile(fp, &pbh, &w, &h);
	imgWidth = (int)w;
	imgHeight = (int)h;

	//rgbBuffer를 이미지 사이즈만큼 메모리 할당
	rgbBuffer = new RGBQUAD*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		rgbBuffer[i] = new RGBQUAD[imgWidth];
	}

	int dw = WIDTHBYTES(imgWidth * 24);
	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			rgbBuffer[imgHeight - i - 1][j].rgbBlue = pbuf[i*dw + j * 3 + 0];
			rgbBuffer[imgHeight - i - 1][j].rgbGreen = pbuf[i*dw + j * 3 + 1];
			rgbBuffer[imgHeight - i - 1][j].rgbRed = pbuf[i*dw + j * 3 + 2];
		}
	}
	delete[] pbuf;
	fclose(fp);
	viewType = 1;
	Invalidate(TRUE);
}




void CDigitalImageView::OnHistogramEqualization()
{
	OnRgbToHsi();
	for (int i = 0; i < 256; i++) {
		histogram[i] = 0;
	}


	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			histogram[(int)intenBuffer[i][j]]++;
		}
	}

	int sum = 0;
	float scale_factor = 255.0f;

	for (int i = 0; i < 256; i++) {
		sum += histogram[i];
		sum_hist[i] = (int)((sum * scale_factor) + 0.5);
	}


	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			intenFilteringBuffer[i][j] = float(sum_hist[(int)intenBuffer[i][j]] / sum);
		}
	}
	outText = "          Histogram Equalization(Intensity)          ";
	viewType = 3;
	Invalidate(false);

}


void CDigitalImageView::OnHistogramEqualizationRgb()
{

	for (int i = 0; i < 256; i++) {
		histogramR[i] = 0;
		histogramG[i] = 0;
		histogramB[i] = 0;
	}

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			histogramR[(int)rgbBuffer[i][j].rgbRed]++;
			histogramG[(int)rgbBuffer[i][j].rgbGreen]++;
			histogramB[(int)rgbBuffer[i][j].rgbBlue]++;
		}
	}

	if (rgbBufferHE != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbBufferHE[i];
		}
		delete[] rgbBufferHE;
	}


	int sumR = 0;
	int sumG = 0;
	int sumB = 0;
	float scale_factor = 255.0f;

	for (int i = 0; i < 256; i++) {
		sumR += histogramR[i];
		sumG += histogramG[i];
		sumB += histogramB[i];
		sum_histR[i] = (int)((sumR * scale_factor) + 0.5);
		sum_histG[i] = (int)((sumG * scale_factor) + 0.5);
		sum_histB[i] = (int)((sumB * scale_factor) + 0.5);
	}

	rgbBufferHE = new RGBQUAD*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		rgbBufferHE[i] = new RGBQUAD[imgWidth];
	}

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			rgbBufferHE[i][j].rgbRed = sum_histR[(int)rgbBuffer[i][j].rgbRed] / sumR;
			rgbBufferHE[i][j].rgbGreen = sum_histG[(int)rgbBuffer[i][j].rgbGreen] / sumG;
			rgbBufferHE[i][j].rgbBlue = sum_histB[(int)rgbBuffer[i][j].rgbBlue] / sumB;
		}
	}
	outText = "          Histogram Equalization(RGB)          ";

	viewType = 4;
	Invalidate(false);
}





void CDigitalImageView::OnHistogramSpecification()
{
	OnHistogramEqualization();
	int difference;
	int min;
	for (int i = 0; i<256; i++)
	{
		difference = (int)fabs(i - sum_hist[0]/255);
		min = 0;
		for (int j = 0; j<256; j++)
		{
			if ((int)fabs(i - sum_hist[j] / 255) < difference)
			{
				difference = (int)fabs(i - sum_hist[j] / 255);
				min = j;
			}
		}
		inv_hist[i] = min;
	}
	//inv_hist에 들어가는 값 잘 확인해서 조절해봐야함.(미완성)1
	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			intenBuffer[i][j] = float(inv_hist[(int)intenBuffer[i][j]]);
		}
	}
	outText = "          Histogram Specification          ";
	viewType = 3;
	Invalidate(false);


}



void CDigitalImageView::OnHsiToRgb() {
	/*
	//rgbBuffer이 NULL이 아닌 경우 메모리 해제
	if (rgbBufferFilter != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbBufferFilter[i];
		}
		delete[] rgbBufferFilter;
	}
	rgbBufferFilter = new RGBQUAD*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		rgbBufferFilter[i] = new RGBQUAD[imgWidth];
	}



	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			rgbBufferFilter[i][j].rgbRed = (bluringBuffer[i][j] * 3.0f) - rgbBuffer[i][j].rgbGreen - rgbBuffer[i][j].rgbBlue;
			rgbBufferFilter[i][j].rgbGreen = (bluringBuffer[i][j] * 3.0f) - rgbBuffer[i][j].rgbRed - rgbBuffer[i][j].rgbBlue;
			rgbBufferFilter[i][j].rgbBlue = (bluringBuffer[i][j] * 3.0f) - rgbBuffer[i][j].rgbRed - rgbBuffer[i][j].rgbGreen;

		}
	}

	*/
}



void CDigitalImageView::OnAverage3x3()
{

	float mask3[3][3] = { { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },{ 1.0f / 9.0f,1.0f / 9.0f,1.0f / 9.0f },{ 1.0f / 9.0f,1.0f / 9.0f,1.0f / 9.0f } };

	if (rgbFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbFilteringBuffer[i];
		}
		delete[] rgbFilteringBuffer;
	}

	rgbFilteringBuffer = new RGBQUAD*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		rgbFilteringBuffer[i] = new RGBQUAD[imgWidth];
	}

	int a, b;

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			rgbFilteringBuffer[i][j].rgbRed = 0;
			rgbFilteringBuffer[i][j].rgbBlue = 0;
			rgbFilteringBuffer[i][j].rgbGreen = 0;
			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					rgbFilteringBuffer[i][j].rgbRed += rgbBuffer[a - 1 + x][b - 1 + y].rgbRed * mask3[x][y];
					rgbFilteringBuffer[i][j].rgbBlue += rgbBuffer[a - 1+ x][b - 1 + y].rgbBlue * mask3[x][y];
					rgbFilteringBuffer[i][j].rgbGreen += rgbBuffer[a - 1 + x][b - 1 + y].rgbGreen * mask3[x][y];
				}
			}

		}
	}
	outText = "          Average Filter 3X3          ";

	viewType = 5;
	Invalidate(false);


}


void CDigitalImageView::OnAverage5x5()
{
	float mask5[5][5];
	for (int i=0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			mask5[i][j] = 1.0f / 25.0f;
		}
	}


	if (rgbFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbFilteringBuffer[i];
		}
		delete[] rgbFilteringBuffer;
	}

	rgbFilteringBuffer = new RGBQUAD*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		rgbFilteringBuffer[i] = new RGBQUAD[imgWidth];
	}

	/*if (intenFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer[i];
		}
		delete[] intenFilteringBuffer;
	}

	intenFilteringBuffer = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		intenFilteringBuffer[i] = new float[imgWidth];
	}*/


	int a, b;


	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			//intenFilteringBuffer[i][j] = 0;
			rgbFilteringBuffer[i][j].rgbRed = 0;
			rgbFilteringBuffer[i][j].rgbBlue = 0;
			rgbFilteringBuffer[i][j].rgbGreen = 0;
			a = i;
			b = j;
			if (i == 0 || i == 1) {
				a = 2;
			}
			if (j == 0 || i == 1) {
				b = 2;
			}
			if (i == imgHeight - 1 || i == imgHeight - 2) {
				a = imgHeight - 3;
			}
			if (j == imgWidth - 1 || j == imgHeight - 2) {
				b = imgWidth - 3;
			}
			
			for (int x = 0; x < 5; x++) {
				for (int y = 0; y < 5; y++) {
					//intenFilteringBuffer[i][j] += intenBuffer[a - 2 + x][b - 2 + y] * mask5[x][y];
					rgbFilteringBuffer[i][j].rgbRed += rgbBuffer[a - 2 + x][b - 2 + y].rgbRed * mask5[x][y];
					rgbFilteringBuffer[i][j].rgbBlue += rgbBuffer[a - 2 + x][b - 2 + y].rgbBlue * mask5[x][y];
					rgbFilteringBuffer[i][j].rgbGreen += rgbBuffer[a - 2 + x][b - 2 + y].rgbGreen * mask5[x][y];
				}
			}
		}
	}
	outText = "          Average Filter 5X5          ";
	viewType = 5;
	Invalidate(false);

}


void CDigitalImageView::OnMedian3x3()
{

	if (rgbFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbFilteringBuffer[i];
		}
		delete[] rgbFilteringBuffer;
	}

	rgbFilteringBuffer = new RGBQUAD*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		rgbFilteringBuffer[i] = new RGBQUAD[imgWidth];
	}

	int a, b;
	int arrR[9];
	int arrG[9];
	int arrB[9];
	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			rgbFilteringBuffer[i][j].rgbRed = 0;
			rgbFilteringBuffer[i][j].rgbBlue = 0;
			rgbFilteringBuffer[i][j].rgbGreen = 0;
			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}
			
			int k = 0;
			int l = 0;
			int m = 0;
			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					arrR[k++] = rgbBuffer[a - 1 + x][b - 1 + y].rgbRed;
					arrG[l++] = rgbBuffer[a - 1 + x][b - 1 + y].rgbGreen;
					arrB[m++] = rgbBuffer[a - 1 + x][b - 1 + y].rgbBlue;
				}
			}
			SelectionSort(arrR, 9);
			SelectionSort(arrG, 9);
			SelectionSort(arrB, 9);

			rgbFilteringBuffer[a][b].rgbRed = arrR[4];
			rgbFilteringBuffer[a][b].rgbGreen = arrG[4];
			rgbFilteringBuffer[a][b].rgbBlue = arrB[4];

		}
	}

	outText = "          Median Filter 3X3          ";
	viewType = 5;
	Invalidate(false);
}


void CDigitalImageView::OnMedian5x5()
{
	if (rgbFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbFilteringBuffer[i];
		}
		delete[] rgbFilteringBuffer;
	}

	rgbFilteringBuffer = new RGBQUAD*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		rgbFilteringBuffer[i] = new RGBQUAD[imgWidth];
	}

	int a, b;
	int arrR[25];
	int arrG[25];
	int arrB[25];
	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			rgbFilteringBuffer[i][j].rgbRed = 0;
			rgbFilteringBuffer[i][j].rgbBlue = 0;
			rgbFilteringBuffer[i][j].rgbGreen = 0;
			a = i;
			b = j;
			if (i == 0 || i == 1) {
				a = 2;
			}
			if (j == 0 || i == 1) {
				b = 2;
			}
			if (i == imgHeight - 1 || i == imgHeight - 2) {
				a = imgHeight - 3;
			}
			if (j == imgWidth - 1 || j == imgHeight - 2) {
				b = imgWidth - 3;
			}

			int k = 0;
			int l = 0;
			int m = 0;
			for (int x = 0; x < 5; x++) {
				for (int y = 0; y < 5; y++) {
					arrR[k++] = rgbBuffer[a - 2 + x][b - 2 + y].rgbRed;
					arrG[l++] = rgbBuffer[a - 2 + x][b - 2 + y].rgbGreen;
					arrB[m++] = rgbBuffer[a - 2 + x][b - 2 + y].rgbBlue;
				}
			}
			SelectionSort(arrR, 25);
			SelectionSort(arrG, 25);
			SelectionSort(arrB, 25);

			rgbFilteringBuffer[a][b].rgbRed = arrR[12];
			rgbFilteringBuffer[a][b].rgbGreen = arrG[12];
			rgbFilteringBuffer[a][b].rgbBlue = arrB[12];

		}
	}

	outText = "          Median Filter 5X5          ";
	viewType = 5;
	Invalidate(false);
}

void CDigitalImageView::SelectionSort(int *arr, int n) {
	int i, j, indexMin, temp;

	for (i = 0; i < n - 1; i++) {
		indexMin = i;
		for (j = i + 1; j < n; j++) {
			if (arr[j] < arr[indexMin]) {
				indexMin = j;
			}
		}
		temp = arr[indexMin];
		arr[indexMin] = arr[i];
		arr[i] = temp;
	}

}

void CDigitalImageView::OnGaussian3x3()
{
	OnRgbToHsi();
	float mask3[3][3];
	float sigma = 0.8f;
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			mask3[i+1][j+1] = ((1.0f / (2.0f * 3.14f*sigma*sigma)) * exp(-1.0f*((float)i*(float)i + (float)j*(float)j )/(2.0f*sigma*sigma)));
		}
	}


	if (rgbFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbFilteringBuffer[i];
		}
		delete[] rgbFilteringBuffer;
	}

	rgbFilteringBuffer = new RGBQUAD*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		rgbFilteringBuffer[i] = new RGBQUAD[imgWidth];
	}

	int a, b;

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			rgbFilteringBuffer[i][j].rgbRed = 0.0f;
			rgbFilteringBuffer[i][j].rgbGreen = 0.0f;
			rgbFilteringBuffer[i][j].rgbBlue = 0.0f;
			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					rgbFilteringBuffer[i][j].rgbRed += rgbBuffer[a - 1 + x][b - 1 + y].rgbRed * mask3[x][y];
					rgbFilteringBuffer[i][j].rgbBlue += rgbBuffer[a - 1 + x][b - 1 + y].rgbBlue * mask3[x][y];
					rgbFilteringBuffer[i][j].rgbGreen += rgbBuffer[a - 1 + x][b - 1 + y].rgbGreen * mask3[x][y];
				}
			}
		}
	}

	outText = "          Gaussian Filter 3X3          ";
	viewType = 5;
	Invalidate(false);

}


void CDigitalImageView::OnGaussian5x5()
{
	OnRgbToHsi();
	float mask5[5][5];
	float sigma = 0.8f;
	for (int i = -2; i < 3; i++) {
		for (int j = -2; j < 3; j++) {
			mask5[i + 2][j + 2] = ((1.0f / (2.0f * 3.14f*sigma*sigma)) * exp(-1.0f*((float)i*(float)i + (float)j*(float)j) / (2.0f*sigma*sigma)));
		}
	}

	if (rgbFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbFilteringBuffer[i];
		}
		delete[] rgbFilteringBuffer;
	}

	rgbFilteringBuffer = new RGBQUAD*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		rgbFilteringBuffer[i] = new RGBQUAD[imgWidth];
	}


	int a, b;
	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			//intenFilteringBuffer[i][j] = 0;
			rgbFilteringBuffer[i][j].rgbRed = 0;
			rgbFilteringBuffer[i][j].rgbBlue = 0;
			rgbFilteringBuffer[i][j].rgbGreen = 0;
			a = i;
			b = j;
			if (i == 0 || i == 1) {
				a = 2;
			}
			if (j == 0 || i == 1) {
				b = 2;
			}
			if (i == imgHeight - 1 || i == imgHeight - 2) {
				a = imgHeight - 3;
			}
			if (j == imgWidth - 1 || j == imgHeight - 2) {
				b = imgWidth - 3;
			}

			for (int x = 0; x < 5; x++) {
				for (int y = 0; y < 5; y++) {
					//intenFilteringBuffer[i][j] += intenBuffer[a - 2 + x][b - 2 + y] * mask5[x][y];
					rgbFilteringBuffer[i][j].rgbRed += rgbBuffer[a - 2 + x][b - 2 + y].rgbRed * mask5[x][y];
					rgbFilteringBuffer[i][j].rgbBlue += rgbBuffer[a - 2 + x][b - 2 + y].rgbBlue * mask5[x][y];
					rgbFilteringBuffer[i][j].rgbGreen += rgbBuffer[a - 2 + x][b - 2 + y].rgbGreen * mask5[x][y];
				}
			}

		}
		
	}

	outText = "          Gaussian Filter 5X5          ";
	viewType = 5;
	Invalidate(false);
}


void CDigitalImageView::OnMask1A()// Mask : 1 // A : 1
{
	OnRgbToHsi();
	float mask3[3][3] = { { 0.0f,-1.0f / 5.0f,0.0f },{ -1.0f / 5.0f,4.0f / 5.0f, -1.0f / 5.0f },{ 0.0f,-1.0f / 5.0f,0.0f } };

	if (intenFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer[i];
		}
		delete[] intenFilteringBuffer;
	}

	intenFilteringBuffer = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		intenFilteringBuffer[i] = new float[imgWidth];
	}

	int a, b;

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			intenFilteringBuffer[i][j] = 0;

			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					intenFilteringBuffer[i][j] += intenBuffer[a - 1 + x][b - 1 + y] * mask3[x][y];
					
				}
			}
			intenFilteringBuffer[i][j] += intenBuffer[a][b];

		}
	}
	outText = "          High-Boost Fileter Mask1, A1          ";

	viewType = 3;
	Invalidate(false);
}


void CDigitalImageView::OnMask1A32809()// Mask : 1 // A : 1.2
{
	OnRgbToHsi();
	float mask3[3][3] = { { 0.0f,-1.0f / 5.0f,0.0f },{ -1.0f / 5.0f,4.0f / 5.0f, -1.0f / 5.0f },{ 0.0f,-1.0f / 5.0f,0.0f } };

	if (intenFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer[i];
		}
		delete[] intenFilteringBuffer;
	}

	intenFilteringBuffer = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		intenFilteringBuffer[i] = new float[imgWidth];
	}

	int a, b;

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			intenFilteringBuffer[i][j] = 0;

			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					intenFilteringBuffer[i][j] += intenBuffer[a - 1 + x][b - 1 + y] * mask3[x][y];

				}
			}
			intenFilteringBuffer[i][j] += intenBuffer[a][b]*1.2f;

		}
	}
	outText = "          High-Boost Fileter Mask1, A1.2          ";

	viewType = 3;
	Invalidate(false);
}


void CDigitalImageView::OnMask1A32810()// Mask : 1 // A : 1.5
{
	OnRgbToHsi();
	float mask3[3][3] = { { 0.0f,-1.0f / 5.0f,0.0f },{ -1.0f / 5.0f,4.0f / 5.0f, -1.0f / 5.0f },{ 0.0f,-1.0f / 5.0f,0.0f } };

	if (intenFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer[i];
		}
		delete[] intenFilteringBuffer;
	}

	intenFilteringBuffer = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		intenFilteringBuffer[i] = new float[imgWidth];
	}

	int a, b;

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			intenFilteringBuffer[i][j] = 0;

			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					intenFilteringBuffer[i][j] += intenBuffer[a - 1 + x][b - 1 + y] * mask3[x][y];

				}
			}
			intenFilteringBuffer[i][j] += intenBuffer[a][b] * 1.5f;

		}
	}
	outText = "          High-Boost Fileter Mask1, A1.5          ";

	viewType = 3;
	Invalidate(false);
}


void CDigitalImageView::OnMask1A32811()// Mask : 1 // A : 1.8
{
	OnRgbToHsi();
	float mask3[3][3] = { { 0.0f,-1.0f / 5.0f,0.0f },{ -1.0f / 5.0f,4.0f / 5.0f, -1.0f / 5.0f },{ 0.0f,-1.0f / 5.0f,0.0f } };

	if (intenFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer[i];
		}
		delete[] intenFilteringBuffer;
	}

	intenFilteringBuffer = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		intenFilteringBuffer[i] = new float[imgWidth];
	}

	int a, b;

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			intenFilteringBuffer[i][j] = 0;

			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					intenFilteringBuffer[i][j] += intenBuffer[a - 1 + x][b - 1 + y] * mask3[x][y];

				}
			}
			intenFilteringBuffer[i][j] += intenBuffer[a][b] * 1.8f;

		}
	}
	outText = "          High-Boost Fileter Mask1, A1.8          ";

	viewType = 3;
	Invalidate(false);
}


void CDigitalImageView::OnMask2A()
{
	OnRgbToHsi();
	float mask3[3][3] = { { -1.0f / 9.0f,-1.0f / 9.0f,-1.0f / 9.0f },{ -1.0f / 9.0f,8.0f / 9.0f,-1.0f / 9.0f },{ -1.0f / 9.0f,-1.0f / 9.0f,-1.0f / 9.0f } };

	if (intenFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer[i];
		}
		delete[] intenFilteringBuffer;
	}

	intenFilteringBuffer = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		intenFilteringBuffer[i] = new float[imgWidth];
	}

	int a, b;

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			intenFilteringBuffer[i][j] = 0;

			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					intenFilteringBuffer[i][j] += intenBuffer[a - 1 + x][b - 1 + y] * mask3[x][y];
				}
			}
			intenFilteringBuffer[i][j] += intenBuffer[a][b];
		}
	}
	outText = "          High-Boost Fileter Mask2, A1          ";

	viewType = 3;
	Invalidate(false);
}


void CDigitalImageView::OnMask2A32813()
{
	OnRgbToHsi();
	float mask3[3][3] = { { -1.0f / 9.0f,-1.0f / 9.0f,-1.0f / 9.0f },{ -1.0f / 9.0f,8.0f / 9.0f,-1.0f / 9.0f },{ -1.0f / 9.0f,-1.0f / 9.0f,-1.0f / 9.0f } };

	if (intenFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer[i];
		}
		delete[] intenFilteringBuffer;
	}

	intenFilteringBuffer = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		intenFilteringBuffer[i] = new float[imgWidth];
	}

	int a, b;

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			intenFilteringBuffer[i][j] = 0;

			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					intenFilteringBuffer[i][j] += intenBuffer[a - 1 + x][b - 1 + y] * mask3[x][y];
				}
			}
			intenFilteringBuffer[i][j] += intenBuffer[a][b] * 1.2f;
		}
	}	outText = "          High-Boost Fileter Mask2, A1.2          ";

	viewType = 3;
	Invalidate(false);
}


void CDigitalImageView::OnMask2A32814()
{
	OnRgbToHsi();
	float mask3[3][3] = { { -1.0f / 9.0f,-1.0f / 9.0f,-1.0f / 9.0f },{ -1.0f / 9.0f,8.0f / 9.0f,-1.0f / 9.0f },{ -1.0f / 9.0f,-1.0f / 9.0f,-1.0f / 9.0f } };

	if (intenFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer[i];
		}
		delete[] intenFilteringBuffer;
	}

	intenFilteringBuffer = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		intenFilteringBuffer[i] = new float[imgWidth];
	}

	int a, b;

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			intenFilteringBuffer[i][j] = 0;

			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					intenFilteringBuffer[i][j] += intenBuffer[a - 1 + x][b - 1 + y] * mask3[x][y];
				}
			}
			intenFilteringBuffer[i][j] += intenBuffer[a][b] * 1.5f;
		}
	}
	outText = "          High-Boost Fileter Mask2, A1.5          ";

	viewType = 3;
	Invalidate(false);
}


void CDigitalImageView::OnMask2A32815()
{
	OnRgbToHsi();
	float mask3[3][3] = { { -1.0f / 9.0f,-1.0f / 9.0f,-1.0f / 9.0f },{ -1.0f / 9.0f,8.0f / 9.0f,-1.0f / 9.0f },{ -1.0f / 9.0f,-1.0f / 9.0f,-1.0f / 9.0f } };

	if (intenFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer[i];
		}
		delete[] intenFilteringBuffer;
	}

	intenFilteringBuffer = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		intenFilteringBuffer[i] = new float[imgWidth];
	}

	int a, b;

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			intenFilteringBuffer[i][j] = 0;

			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					intenFilteringBuffer[i][j] += intenBuffer[a - 1 + x][b - 1 + y] * mask3[x][y];
				}
			}
			intenFilteringBuffer[i][j] += intenBuffer[a][b] * 1.8f;
		}
	}
	outText = "          High-Boost Fileter Mask2, A1.8          ";

	viewType = 3;
	Invalidate(false);
}



void CDigitalImageView::OnPrewitt()
{
	OnRgbToHsi();
	float mask3[3][3] = { {-1.0f,-1.0f,-1.0f },{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f} };


	if (intenFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer[i];
		}
		delete[] intenFilteringBuffer;
	}

	intenFilteringBuffer = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		intenFilteringBuffer[i] = new float[imgWidth];
	}

	int a, b;

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			intenFilteringBuffer[i][j] = 0.0f;
			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					intenFilteringBuffer[i][j] += intenBuffer[a - 1 + x][b - 1 + y] * mask3[x][y];
				}
			}

		}
	}

	outText = "          Prewitt Edge Detect-Y          ";



	float mask[3][3] = { { 1.0f,0.0f,-1.0f },{ 1.0f,0.0f,-1.0f },{ 1.0f,0.0f,-1.0f } };


	if (intenFilteringBuffer2 != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer2[i];
		}
		delete[] intenFilteringBuffer2;
	}

	intenFilteringBuffer2 = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		intenFilteringBuffer2[i] = new float[imgWidth];
	}


	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			intenFilteringBuffer2[i][j] = 0.0f;
			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					intenFilteringBuffer2[i][j] += intenBuffer[a - 1 + x][b - 1 + y] * mask[x][y];
				}
			}

		}
	}

	outText2 = "          Prewitt Edge Detect-X         ";
	viewType =6 ;
	Invalidate(false);


}


void CDigitalImageView::OnSobel()
{
	OnRgbToHsi();
	OnPrewitt();
	float mask3[3][3] = { { -1.0f,-2.0f,-1.0f },{ 0.0f,0.0f,0.0f },{ 1.0f,2.0f,1.0f } };


	if (intenFilteringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer[i];
		}
		delete[] intenFilteringBuffer;
	}

	intenFilteringBuffer = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		intenFilteringBuffer[i] = new float[imgWidth];
	}

	int a, b;

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			intenFilteringBuffer[i][j] = 0.0f;
			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					intenFilteringBuffer[i][j] += intenBuffer[a - 1 + x][b - 1 + y] * mask3[x][y];
				}
			}

		}
	}

	outText = "          Sobel Edge Detect-Y          ";



	float mask[3][3] = { { 1.0f,0.0f,-1.0f },{ 2.0f,0.0f,-2.0f },{ 1.0f,0.0f,-1.0f } };


	if (intenFilteringBuffer2 != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] intenFilteringBuffer2[i];
		}
		delete[] intenFilteringBuffer2;
	}

	intenFilteringBuffer2 = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		intenFilteringBuffer2[i] = new float[imgWidth];
	}


	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			intenFilteringBuffer2[i][j] = 0.0f;
			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight - 2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}

			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					intenFilteringBuffer2[i][j] += intenBuffer[a - 1 + x][b - 1 + y] * mask[x][y];
				}
			}

		}
	}

	outText2 = "          Sobel Edge Detect-X         ";
	viewType = 6;
	Invalidate(false);


}


void CDigitalImageView::OnAviLoad()
{
	CFileDialog dlg(TRUE, ".avi", NULL, NULL, "AVI File (*.avi)|*.avi||");
	if (IDOK != dlg.DoModal())
		return;

	CString cfilename = dlg.GetPathName();
	CT2CA strAtl(cfilename);
	String filename(strAtl);


	//AVI 파일 로드
	VideoCapture Capture;
	Capture.open(filename);

	if (!Capture.isOpened())
		AfxMessageBox("Error Video");

	Mat prevFrame;
	for (;;) {
		Mat currFrame;
		Capture >> currFrame;
		if(prevFrame.data != nullptr)
			imshow("prevvideo", prevFrame);
		if (currFrame.data == nullptr)
			break;
		imshow("currvideo", currFrame);

		//여기다가 블록매칭 구현
		Mat diff;
		diff = currFrame - prevFrame;
		
		

		int x = currFrame.rows;
		int y = currFrame.cols;

		int block_wCnt = x / 16;
		int block_hCnt = y / 16;

		
		
	
		float s = 255;
		int xt;
		int yt;
		int xn;
		int yn;
		uchar a;
		uchar b;

		CStdioFile file;
		file.Open("MotionVector.txt", CFile::modeCreate|CFile::modeWrite);
		std::string buf;
		
		

		if (prevFrame.data != nullptr) {
			for (int i = 0; i < block_wCnt; i++) {
				for (int j = 0; j < block_hCnt; j++) {
					int w = 4;
					buf.append("(");
					int picX = block_wCnt*i + 8;
					int picY = block_hCnt*j + 8;
					int m = 0;
					int c = 0;
					s = 255;
					for (int n = 0; n < 3; n++) {
						if (n == 0) {
							xn = picX;
							yn = picY;
						}
						

						for (int k = -1; k < 2; k++) {
							for (int l = -1; l < 2; l++) {

								a = currFrame.at<uchar>(picX, picY);
								b = prevFrame.at<uchar>(xn + (k*w), yn + (l*w));
								
								if (s > abs(a - b)) {
									s = abs(a - b);
									xt = xn + (k*w);
									yt = yn + (l*w);
									c++;
								}

							}
						}
						w = w / 2;
						xn = xt;
						yn = yt;
						
					}
					if (c == 0) {
						xn = picX;
						yn = picY;
					}



					buf.append(std::to_string(picX - xn));
					buf.append(",");
					buf.append(std::to_string(picY - yn));
					buf.append(")");
				}
				buf.append("\n");
			}

		}
		file.WriteString(buf.c_str());
		file.Close();

		if (diff.data == nullptr)
			break;
		imshow("diffvideo", diff);
		
		prevFrame = currFrame;
		if (waitKey(30) >= 0)
			break;
	}

	AfxMessageBox("Completed");




}
void CDigitalImageView::MinDiff() {



}

