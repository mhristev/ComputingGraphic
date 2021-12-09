#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <cmath>
#include <string>

using namespace std;
using namespace cv;

void histogramEqualize(Mat &src, Mat &dst) {
	int gray[256] {0}; 

	for (int i = 0; i < src.rows; i++) { 
		for (int k = 0; k < src.cols; k++) { 
			gray[(int)src.at<uchar>(k, i)]++;
		} 
	} 
	
	int minValue; 
	for (int i = 0; i < 255; i++) { 
		gray[i + 1] += gray[i];
	}

	for (int i = 0; i < 255; i++) { 
		if (gray[i] != 0) {
			minValue = gray[i];
			break;
		}
	}

	for (int i = 0; i < src.rows; i++) {
		for (int k = 0; k < src.cols; k++) {
			dst.at<uchar>(i, k) = (uchar)round((((double)gray[(int)src.at<uchar>(i, k)] - minValue) / (double)(src.rows * src.cols - minValue)) * (double)255);
		}
	}
}

void drawHist(Mat &src, Mat &dst) {
	int histSize = 256;
	float histMax = 0;
	for (int i = 0; i < histSize; i++) {
		float temp = src.at<float>(i);
		if (histMax < temp) {
			histMax = temp;
		}
	}

	float scale = (0.9 * 256) / histMax;
	for (int i = 0; i < histSize; i++) {
		int intensity = static_cast<int>(src.at<float>(i)*scale);
		line(dst, Point(i, 255), Point(i, 255 - intensity), Scalar(0));
	}
}


int histogramStart() {
	Mat image; 
	Mat startHist; 
	Mat eqHist;

	int histSize = 256;
	float range[] = { 0, 255 };
	const float* histRange = { range };

	string file;
	cout << "Enter the file name: ";
	cin >> file;
		
	image = imread(file, IMREAD_GRAYSCALE);

	if (!image.data) { 
		cout << "Couldn't find the image" << endl;
		return -1;
	}
	Mat out = Mat::zeros(image.size(), image.type());

	histogramEqualize(image, out);

	
	calcHist(&image, 1, 0, Mat(), startHist, 1, &histSize, &histRange);
	Mat showstartHist(512, 512, CV_8UC1, Scalar(512));
	drawHist(startHist, showstartHist);

	namedWindow("Original Histogram", WINDOW_AUTOSIZE);
	imshow("Original Histogram", showstartHist);

	
	calcHist(&out, 1, 0, Mat(), eqHist, 1, &histSize, &histRange);
	Mat showeqHist(512, 512, CV_8UC1, Scalar(512));
	drawHist(eqHist, showeqHist);

	namedWindow("Equalized Histogram", WINDOW_AUTOSIZE);
	imshow("Equalized Histogram", showeqHist);
	waitKey(0);

	
	imwrite("result.png", out);
	return 0;
}


int saltPepper_filter() {

	Mat src;
	Mat dst;

	string file;
	cout << "Enter the file name: ";
	cin >> file;
		
    src = imread(file, IMREAD_GRAYSCALE);
    if (!src.data) {
		cout << "Couldn't find the image" << endl; 
		return -1; 
	}

      int image[9];
        dst = src.clone();
        for(int y = 0; y < src.rows; y++)
            for(int x = 0; x < src.cols; x++)
                dst.at<uchar>(y,x) = 0.0;
        for(int y = 1; y < src.rows - 1; y++){
            for(int x = 1; x < src.cols - 1; x++){
                image[0] = src.at<uchar>(y - 1 ,x - 1);
                image[1] = src.at<uchar>(y, x - 1);
                image[2] = src.at<uchar>(y + 1, x - 1);
                image[3] = src.at<uchar>(y - 1, x);
                image[4] = src.at<uchar>(y, x);
                image[5] = src.at<uchar>(y + 1, x);
                image[6] = src.at<uchar>(y - 1, x + 1);
                image[7] = src.at<uchar>(y, x + 1);
                image[8] = src.at<uchar>(y + 1, x + 1);
                int test;
				int j;
				for (int i = 0; i < 9; i++) {
					test = image[i];

					for (j = i-1; j >= 0 && test < image[j]; j--) {
						image[j+1] = image[j];
					}
					image[j+1] = test;
				}
                dst.at<uchar>(y,x) = image[4];
            }
        }
        
	namedWindow("Salt&Pepper filter");
	imshow("Salt&Pepper filter", dst);
	
	namedWindow("Original");
	imshow("Original", src);
	waitKey();
    return 0;
}


int contour() {
	Mat image;
	Mat result;

	string file;
	cout << "Enter the file name: ";
	cin >> file;
		
    image = imread(file, IMREAD_GRAYSCALE);

    if (!image.data) {
		cout << "Couldn't find the image" << endl; 
		return -1; 
	}

    image.copyTo(result);
	// Прилагане на алгоритъма за контури
    for (int i = 1; i < image.rows - 1; i++)
        for (int k = 1; k < image.cols - 1; k++) {
            int p[8];
            p[0] = image.at<uchar>(i, k + 1);
            p[1] = image.at<uchar>(i - 1, k + 1);
            p[2] = image.at<uchar>(i - 1, k);
            p[3] = image.at<uchar>(i - 1, k - 1);
            p[4] = image.at<uchar>(i, k - 1);
            p[5] = image.at<uchar>(i + 1, k - 1);
            p[6] = image.at<uchar>(i + 1, k);
            p[7] = image.at<uchar>(i + 1, k + 1);
            
			int a;
			int b;

			a = p[3] + p[4] + p[2] - p[0] - p[6] - p[7];
            b = p[2] + p[1] + p[0] - p[4] - p[5] - p[6];
            
			// проверяваме двете стойности за контур
			int ab = abs(a) + abs(b);
            // 0-черно, няма контур
			// 255 - бяло, ако има контур
			if (ab > 80) result.at<uchar>(i, k) = 255;
            else result.at<uchar>(i, k) = 0;
        }
	
    imwrite("result.png", result);
    return 0;
}

void midPoint(double X1, double Y1, double X2, double Y2) {
   
    double dx = X2 - X1;
    double dy = Y2 - Y1;
   
    if (dy <= dx) {
        double d = dy - (dx/2);
        double x = X1; 
        double y = Y1;
    
        cout << "(x: " << x << ") (y:" << y << ")" << " - Start"<< endl;
    
        while (x < X2) {
            x++;
            if (d < 0) {
                d = d + dy;
                cout << "(x: " << x << ") (y:" << y << ")" << " - East" << endl;
            } else {
                d += (dy - dx);
                y++;
                cout << "(x: " << x << ") (y:" << y << ")" << " - Nord East" << endl;
            }
            
        }
    } else if (dx < dy) {
        double d = dx - (dy/2);
        double x = X1; 
        double y = Y1;
    
        cout << "(x: " << x << ") (y:" << y << ")" << endl;
    
        while (y < Y2) {
            y++;
    
            if (d < 0) {
                d = d + dx;
                cout << "(x: " << x << ") (y:" << y << ")" << " - East" << endl;
            } else {
                d += (dx - dy);
                x++;
                cout << "(x: " << x << ") (y:" << y << ")" << " - Nord East" << endl;
            }
            
        }
    }
}




int main (int argc, char *argv[]) {
	//histogramStart();
	//saltPepper_filter();
	//contour();
    //cout << "(x: " << x << "), (y:" << y << ")" << endl;
	double x1 = 1, y1 = 1, x2 = 9, y2 = 4;
    midPoint(x1, y1, x2, y2);
    
    return 0;
}