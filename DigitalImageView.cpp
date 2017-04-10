
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
	ON_COMMAND(ID_BLURRING_3X3, &CDigitalImageView::OnBlurring3x3)
	ON_COMMAND(ID_BLURRING_5X5, &CDigitalImageView::OnBlurring5x5)
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

float** bluringBuffer;


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

CDigitalImageView::CDigitalImageView()
{
	rgbBuffer = nullptr;
	hueBuffer = nullptr;
	satuBuffer = nullptr;
	intenBuffer = nullptr;
	rgbBufferHE = nullptr;

	bluringBuffer = nullptr;
	rgbBufferFilter = nullptr;
	//histogram = nullptr;
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
	if (bluringBuffer != nullptr) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] bluringBuffer[i];
		}
		delete[] bluringBuffer;
	}

	if (rgbBufferFilter != nullptr) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] rgbBufferFilter[i];
		}
		delete[] rgbBufferFilter;
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
					p.y = i + imgWidth + 10;
					pDC->SetPixel(p, RGB(satuBuffer[i][j], satuBuffer[i][j], satuBuffer[i][j]));

					p.x = j;
					p.y = i + imgWidth + 10;
					pDC->SetPixel(p, RGB(intenBuffer[i][j], intenBuffer[i][j], intenBuffer[i][j]));
					break;

				case 3:

					p.x = j + imgWidth + 10 + imgWidth + 10;
					p.y = i + imgWidth + 10;
					pDC->SetPixel(p, RGB(intenBuffer[i][j], intenBuffer[i][j], intenBuffer[i][j]));
					break;

				case 4:
					p.x = j + imgWidth + 10 + imgWidth + 10;
					p.y = i;
					pDC->SetPixel(p, RGB(rgbBufferHE[i][j].rgbRed, rgbBufferHE[i][j].rgbGreen, rgbBufferHE[i][j].rgbBlue));

					break;

				case 5:
					p.x = j + imgWidth + 10 + imgWidth + 10;
					p.y = i + imgWidth + 10;
					pDC->SetPixel(p, RGB(bluringBuffer[i][j], bluringBuffer[i][j], bluringBuffer[i][j]));
					//pDC->SetPixel(p, RGB(rgbBufferFilter[i][j].rgbRed, rgbBufferFilter[i][j].rgbGreen, rgbBufferFilter[i][j].rgbBlue));
					break;
					

				}

			}
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
	for (int i = 0; i < imgHeight; i++) {
		hueBuffer[i] = new float[imgWidth];
		satuBuffer[i] = new float[imgWidth];
		intenBuffer[i] = new float[imgWidth];
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
			intenBuffer[i][j] = float(sum_hist[(int)intenBuffer[i][j]] / sum);
		}
	}

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

	viewType = 3;
	Invalidate(false);


}


void CDigitalImageView::OnBlurring3x3()
{
	OnRgbToHsi();

	float mask3[3][3] = { { 1.0f / 16.0f, 1.0f / 8.0f, 1.0f / 16.0f },{ 1.0f / 8.0f,1.0f / 4.0f,1.0f / 8.0f },{ 1.0f / 16.0f,1.0f / 8.0f,1.0f / 16.0f } };
	
/*
	float sigma = 3.0f;
	double s = 2.0*sigma*sigma;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j<=1; j++) {
			mask3[i+1][j+1] = (1 / sqrt(s*3.14))*exp(-(((i)*(i) + (j)*(j)) / s));
		}
	}
	*/



	if (bluringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] bluringBuffer[i];
		}
		delete[] bluringBuffer;
	}

	bluringBuffer = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		bluringBuffer[i] = new float[imgWidth];
	}

	int a, b;

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			a = i;
			b = j;
			if (i == 0) {
				a = 1;
			}
			if (j == 0) {
				b = 1;
			}
			if (i == imgHeight - 1) {
				a = imgHeight-2;
			}
			if (j == imgWidth - 1) {
				b = imgWidth - 2;
			}
			bluringBuffer[i][j] = intenBuffer[a - 1][b - 1] * mask3[0][0] + intenBuffer[a - 1][b] * mask3[0][1] + intenBuffer[a - 1][b + 1] * mask3[0][2] + intenBuffer[a][b - 1] * mask3[1][0] + intenBuffer[a][b] * mask3[1][1] + intenBuffer[a][b + 1] * mask3[1][2] + intenBuffer[a + 1][b - 1] * mask3[2][0] + intenBuffer[a + 1][b] * mask3[2][1] + intenBuffer[a + 1][b + 1] * mask3[2][2];
		}
	}
	OnHsiToRgb();

	viewType = 5;
	Invalidate(false);
}

void CDigitalImageView::OnHsiToRgb() {

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


}

void CDigitalImageView::OnBlurring5x5()
{
	
	OnRgbToHsi();

	//float mask5[5][5] = { { 2.0f / 159.0f, 4.0f / 159.0f ,5.0f / 159.0f , 4.0f / 159.0f , 2.0f / 159.0f },{ 4.0f / 159.0f , 9.0f / 159.0f , 12.0f / 159.0f , 9.0f / 159.0f , 4.0f / 159.0f },{ 5.0f / 159.0f, 12.0f / 159.0f, 15.0f / 159.0f, 12.0f / 159.0f, 5.0f / 159.0f },{ 4.0f / 159.0f , 9.0f / 159.0f , 12.0f / 159.0f , 9.0f / 159.0f , 4.0f / 159.0f } ,{ 2.0f / 159.0f, 4.0f / 159.0f ,5.0f / 159.0f , 4.0f / 159.0f , 2.0f / 159.0f } };
	float mask5[5][5] = { {0.03,0.013,0.022,0.013,0.03},{0.013,0.059,0.097,0.059,0.013},{0.022,0.097,0.159,0.097,0.022},{0.013,0.059,0.097,0.059,0.013},{0.03,0.013,0.022,0.013,0.03} };
	

	/*float sigma =8.0f;
	double s = 2.0*sigma*sigma;
	for (int i = -2; i <= 2; i++) {
		for (int j = -2; j <= 2; j++) {
			mask5[i + 2][j + 2] = (1 / sqrt(s*3.14))*exp(-(((i)*(i)+(j)*(j)) / s));
		}
	}*/

	if (bluringBuffer != NULL) {
		for (int i = 0; i < imgHeight; i++) {
			delete[] bluringBuffer[i];
		}
		delete[] bluringBuffer;
	}

	bluringBuffer = new float*[imgHeight];
	for (int i = 0; i < imgHeight; i++) {
		bluringBuffer[i] = new float[imgWidth];
	}

	int a, b;

	for (int i = 0; i < imgHeight; i++) {
		for (int j = 0; j < imgWidth; j++) {
			a = i;
			b = j;
			if (i == 0 || i==1) {
				a = 2;
			}
			if (j == 0|| i==1) {
				b = 2;
			}
			if (i == imgHeight - 1|| i==imgHeight-2) {
				a = imgHeight - 3;
			}
			if (j == imgWidth - 1 || j == imgHeight - 2) {
				b = imgWidth - 3;
			}
			bluringBuffer[i][j] =
				intenBuffer[a - 2][b - 2] * mask5[0][0] +
				intenBuffer[a - 2][b - 1] * mask5[0][1] +
				intenBuffer[a - 2][b] * mask5[0][2] +
				intenBuffer[a - 2][b + 1] * mask5[0][3] +
				intenBuffer[a - 2][b + 2] * mask5[0][4] +
				intenBuffer[a - 1][b - 2] * mask5[1][0] +
				intenBuffer[a - 1][b - 2] * mask5[1][1] +
				intenBuffer[a - 1][b - 2] * mask5[1][2] +
				intenBuffer[a - 1][b - 2] * mask5[1][3] +
				intenBuffer[a - 1][b - 2] * mask5[1][4] +
				intenBuffer[a][b - 2] * mask5[2][0] +
				intenBuffer[a][b - 2] * mask5[2][1] +
				intenBuffer[a][b - 2] * mask5[2][2] +
				intenBuffer[a][b - 2] * mask5[2][3] +
				intenBuffer[a][b - 2] * mask5[2][4] +
				intenBuffer[a + 1][b - 2] * mask5[3][0] +
				intenBuffer[a + 1][b - 2] * mask5[3][1] +
				intenBuffer[a + 1][b - 2] * mask5[3][2] +
				intenBuffer[a + 1][b - 2] * mask5[3][3] +
				intenBuffer[a + 1][b - 2] * mask5[3][4] +
				intenBuffer[a + 2][b - 2] * mask5[4][0] +
				intenBuffer[a + 2][b - 2] * mask5[4][1] +
				intenBuffer[a + 2][b - 2] * mask5[4][2] +
				intenBuffer[a + 2][b - 2] * mask5[4][3] +
				intenBuffer[a + 2][b - 2] * mask5[4][4];
		}
	}
	//OnHsiToRgb();

	viewType = 5;
	Invalidate(false);
	
}
