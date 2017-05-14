#include <QCoreApplication>
#include <QImage>
#include <QColor>
#include <QStringList>
#include <qtconcurrentrun.h>
#include <QThread>
#include <QtCore/qmath.h>
#include <QFile>

/*
By: Reza Farid
Date: 3/April/2017

I had two options to load and traverse image pixels:
1) using img.bits()
	In this option, I needed to check the depth of each pixel to separate R,G, and B.
2) using img->pixel()
	I used this option since extracting R,G and B can be done easily by QColor,
	so checking the depth has been taken care of.
Note: I kept the option 1 code as comments.

3) Some debugging lines are commented out.
4) The program can be tested by few exemplar images (200x100) such as:
4-a) "RGB_red.png": all pixels as Red   (255,0,0)
4-b) "RGB_green.png": all pixels as Green (0,255,0)
4-c) "RGB_blue.png": all pixels as Blue  (0,0,255)
4-d) "RGB_white.png": all pixels as white (255,255,255)
4-e) "RGB_black.png": all pixels as black (0,0,0)
4-f) "RGB_colmajor.png" and "RGB_rowmajor.png": the same number of R,G,B values.

5) Another test is that the (number of pixels*3) must be equal to the summation of
all R,G and B histogram values.

6) I/O
input: input_image
output: a txt file contaning the histogram of R,G and B values in 3 lines. Values in each line are comma separated.
The name of output file is <input_image>_hist.txt where <input_image> is replaced by the value of the input_image file.
*/
// QImage img;
int img_height;
int img_width;
const int RGB_len=768;

void saveHistogram(QString outfile, unsigned int *histogram)
{
    QFile file(outfile);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
		for (int i=0;i<3;i++) {
				for (int j=0;j<255;j++) {
					stream << histogram[i*256+j] << ",";
				}
				stream << histogram[i*256+255] << endl;
		}
    file.close();
    qDebug() << "Result is saved to" << outfile;
	}
}

// void calcHistogram(int tnum, int from, int to, int *RGBs, uchar *bits)
void calcHistogram(int tnum, int from, int to, int *RGBs, QImage *img)
{
    int start=tnum*RGB_len;
    if (to>img_height)
    	to=img_height;
    for(int i = 0; i <= 5; i++)
    {
        qDebug() << "thread# " << tnum << "(" << i << ") from" << QThread::currentThread();
    }
    // qDebug() << "image HxW =" << img_height << "X" << img_width;

    // int currentIndex = from*img_width*4;
    // qDebug() <<  currentIndex;
	for ( int row = from; row < to; ++row )
	    for ( int col = 0; col < img_width; ++col )
	    {
	        QColor clrCurrent( img->pixel( col , row ) );
	        int R=clrCurrent.red();
	        int G=clrCurrent.green();
	        int B=clrCurrent.blue();
	        // currentIndex++;
	        // uchar R = bits[currentIndex++];
	        // uchar G = bits[currentIndex++];
	        // uchar B = bits[currentIndex++];
	        
	        // qDebug() <<  R << G << B;
	        RGBs[start+R]++;
	        RGBs[start+G+256]++;
	        RGBs[start+B+512]++;
	    }
	return;
}

int main(int argc, char *argv[])
{
    QString output_suffix="_hist.txt";
    QCoreApplication a(argc, argv);

    //////////////////
    //// Qt hints ////
    
    // get the commandline arguments
    const QStringList args = a.arguments();
    QString infile("../data/DSC_0278.JPG");
    if (args.count()>1)
    	infile=args[1];
    
    // load an image - option 1
    QImage img;
    if (not(img.load(infile))) {
    	qDebug()<< infile <<"is NOT available.";
    	qDebug()<< "Usage:" << args[0] << "input_image";
    	qDebug()<< "Output:" << QString("input_image"+output_suffix);
    	return 0;
    }
    // load an image - option 2
    // uchar *bits;    
    // if (img.load(infile)) {
    //     bits = img.bits();
    // }
    // else {
    	// qDebug()<< infile <<"is NOT available.";
    	// qDebug()<< "Usage:" << args[0] << "input_image";
    	// qDebug()<< "Output:" << QString("input_image"+output_suffix);
    // 	return 0;
    // }
   
    // find the number of threads
	int num_of_ideal_threads=QThread::idealThreadCount();
	qDebug() <<"num_of_ideal_threads=" << num_of_ideal_threads;
	
	// form the output
	int *RGBs=new int[num_of_ideal_threads*RGB_len];
	for (int i=0;i<num_of_ideal_threads*RGB_len;i++)
		RGBs[i]=0;
	
	// find the image dimension
    img_height = img.height();
    img_width = img.width();
    qDebug() << "image HxW =" << img_height << "X" << img_width;
	qDebug() << "number of bytes" << img.byteCount();    

    // split height to threads
	int step =qCeil(float(img_height)/float(num_of_ideal_threads));
	qDebug() << "step =" << step;
	// form vector of threads
	QVector<QFuture <void> > t;

	for (int i=0;i<num_of_ideal_threads;i++) {
		int stIndex = i*step;
		qDebug() << "For thread#" << i << "::"<< stIndex << "::"<< stIndex+step;
		// QFuture<void> ti = QtConcurrent::run(calcHistogram, i, stIndex, stIndex+step,RGBs,bits); 
    	QFuture<void> ti = QtConcurrent::run(calcHistogram, i, stIndex, stIndex+step,RGBs,&img); 
    	t.push_back(ti);
	}

	// for all threads, wait for finished
	for (int i=0;i<num_of_ideal_threads;i++) {
		t[i].waitForFinished();
	}

	// sum up all histograms, calculate total sum
	unsigned int total=0;
	unsigned int final_RGB[RGB_len]={0}; // 256*3
	for (int i=0;i<t.size();i++) {
		// qDebug() << "For thread#" << i;
		for (int j=0;j<RGB_len;j++) {
			final_RGB[j]+=RGBs[i*RGB_len+j];
			total+=RGBs[i*RGB_len+j];
		}
	}

	// Save the result
	unsigned int *hist=final_RGB;
	saveHistogram(infile+output_suffix, hist);

	// test the numbers
	QString check((unsigned int)(img_height * img_width * 3) == total? "True" : "False");
	qDebug() << img_height * img_width * 3 << "(number of pixels) ==" << total <<"(sum of histograms)" << check;
    //////////////////

    return 0;
}