#include "mainwindow.h"
#include "ui_mainwindow.h"


using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    /*
        String face_cascade_name =  String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/cascades/haarcascade_frontalface_alt.xml");

        String eyes_cascade_name = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/cascades/haarcascade_eye_tree_eyeglasses.xml");

        String nose_cascade_name = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/cascades/haarcascade_mcs_nose.xml");

        String mouth_cascade_name = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/cascades/haarcascade_mcs_mouth.xml");
    */
    face_cascade_name =  String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/cascades/haarcascade_frontalface_alt.xml");

    eyes_cascade_name = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/cascades/haarcascade_eye_tree_eyeglasses.xml");

    nose_cascade_name = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/cascades/haarcascade_mcs_nose.xml");

    mouth_cascade_name = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/cascades/haarcascade_mcs_mouth.xml");

    // Load the cascades
    if( !face_cascade.load( face_cascade_name ) ){
        QMessageBox::warning(this,"Errore face cascade", "face_cascade non caricato!");
    };
    if( !eyes_cascade.load( eyes_cascade_name ) ){
        QMessageBox::warning(this,"Errore eyes cascade", "eyes_cascade non caricato!");
    };
    if( !nose_cascade.load( nose_cascade_name ) ){
        QMessageBox::warning(this,"Errore nose cascade", "nose_cascade non caricato!");
    };
    if( !mouth_cascade.load( mouth_cascade_name ) ){
        QMessageBox::warning(this,"Errore mouth cascade", "mouth_cascade non caricato!");
    };


    ui->setupUi(this);
    this->setWindowTitle("Musica Humana");

    //istanzio il contenitore per la visualizzazione dell'immagine
    imgWidget = new CVImageWidget (ui->widgetImg);


    // Load an image
    image = cv::imread(String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/lena.jpg"), true);




    if (! image.data){
        std::cerr << "immagine non caricata" ;
    }

    visualizzaImmagine(image, imgWidget);

    int larghezza2 = imgWidget->width();
    int altezza2 = imgWidget->height();

    std::cerr << "lena: " << larghezza2 << " - " << altezza2 << endl;

    /* usata inizialmente per provare a scrivere un file midi a cazzo
    int nota[5] = {30,40,50,60,70};
    salvaMIDI(nota, 5);
*/

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonCarica_clicked()
{
    std::cerr << "carica" << endl;

    QString nomeFile = QFileDialog::getOpenFileName(this);
    QByteArray byteArray = nomeFile.toLocal8Bit();
    const char *nomeFileChar = byteArray.data();
    image = imread (nomeFileChar , true);
    if (! image.data){
        QMessageBox::warning(this,"Errore nella fase di lettura", "Lettura del file non riuscita!");
    } else {
        visualizzaImmagine(image, imgWidget);

        // scrivo l'immagine selezionata su un file jpg
        String imgAcquisitaPath = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/immagine acquisita.jpg");
        const char *pathImgAcquisitaChar = imgAcquisitaPath.c_str();
        imwrite(pathImgAcquisitaChar, image);

        //cancello le eventuali scritte sull'interfaccia
        ui->labelDistanze->setText("");
    }
}

void MainWindow::on_pushButtonScatta_clicked()
{
    std::cerr << "scatta" <<endl;

    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        QMessageBox::warning(this,"Errore webcam", "Webcam non attiva!");

    else{
        for(;;)
        {

            cap >> image; // get a new frame from camera

            imshow("Acquisizione immagine", image);

            // appena premi un tasto salva l'immagine
            if(waitKey(30) >= 0){

                // scrivo l'immagine acquisita su un file jpg
                String imgAcquisitaPath = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/immagine acquisita.jpg");
                const char *pathImgAcquisitaChar = imgAcquisitaPath.c_str();
                imwrite(pathImgAcquisitaChar, image);

                // visualizzo l'immagine nel widget
                visualizzaImmagine(image,imgWidget);

                //distruggo la finestra aperta per acquisire l'immagine e libero la webcam
                destroyAllWindows();
                cap.release();

                //cancello le eventuali scritte sull'interfaccia
                ui->labelDistanze->setText("");

                //esco dal ciclo for(;;)
                break;
            }
        }
    }
}

void MainWindow::visualizzaImmagine(Mat immagine, CVImageWidget *imgWidget){

    if (immagine.data==0){
        return;
    }

    //Adatta l'immagine prima di visualizzarla

    // dimensione del contenitore
    float larghezza = ui->widgetImg->width();
    float altezza = ui->widgetImg->height();

    // adatto le dimensioni dell'immagine al contenitore
    // proporzione image.witdh : larghezza = image.height : altezza

    // se una delle dimensioni dell'immagine è più grande di quella del contenitore
    if (immagine.size().width > larghezza || immagine.size().height > altezza){

        //se l'overhead della larghezza è maggiore di quello dell' altezza
        if (immagine.size().width-larghezza > immagine.size().height-altezza){

            //la larghezza rimane al valore massimo, l'altezza cambia proporzionalmente.
            altezza = larghezza * immagine.size().height/immagine.size().width;
        } else{

            //altrimenti si fa il contrario: lunghezza = valore massimo; larghezza cambia proporzionalmente
            larghezza = altezza * immagine.size().width/immagine.size().height;
        }
    }

    // non modifico l'originale, ma solo un'elemento temporaneo che serve solo per la corretta visualizzazione
    Mat immagineAdattata;

    // adatto l'immagine
    cv::resize(immagine, immagineAdattata, Size((int)larghezza,(int)altezza));

    // aggiorno il widget per la visualizzazione
    imgWidget->showImage(immagineAdattata);
}

void MainWindow::on_pushButtonCalcolaDistanze_clicked()
{

    //mostra la finestra per la scelta della forma del volto
    SelezionaFormaVolto *finestraSelezionaVolto = new SelezionaFormaVolto;
    if (finestraSelezionaVolto->exec()){

        int formaVolto = finestraSelezionaVolto->getFormaVolto();

        //imposto vettoreIntervalli
        impostaIntervalli(formaVolto);

        //cerr << endl << "in if: " << vettoreIntervalli[2] << endl;

    } else{
        return;
    }


    //cerr << endl << "in mainwindow: " << vettoreIntervalli[3] << endl;

    //contiene la stringa mostrata nell'interfaccia, viene costruita durante le operazioni di detect
    QString caratteristiche;

    //utilizzato dai detector
    Mat frame_gray;
    //copia utilizzata per disegnare i rettangoli intorno alle aree di interesse
    Mat frame = image.clone();

    //vettore contenente i volti individuati
    std::vector<Rect> faces;

    //QMessageBox::information(this,"cvtColor", "prima di cvtColor( frame, frame_gray, CV_BGR2GRAY);");

    //converto frame in scala di grigi
    cvtColor( frame, frame_gray, CV_BGR2GRAY);
    equalizeHist( frame_gray, frame_gray );

    //distanza minima usata dai detectMultiScale per individuare gli oggetti
    Size dimensioneMinima (image.size().width/10,image.size().height/10);

    //-- Detect faces - find biggest object trova il volto più grande.
    // se si vogliono visualizzare tutte le facce utilizzare scale_image
    //face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, CV_HAAR_FIND_BIGGEST_OBJECT, dimensioneMinima );

    std::cerr << endl << endl << "facce trovate: " << faces.size() << endl;

    if (faces.size()>0){
        /*
         * Non serve perchè nell'immagine cerco il volto più grande
         *
        caratteristiche = "Facce trovate: " + QString::number( faces.size() ) + "\n";
        */
    } else {
        caratteristiche = "Nessun volto individuato.";
        return;
    }

    //scrivo su file l'immagine contenuta nel membro image di mainwindow.cpp
    // serve perchè la libreria flandmark utilizza IplImage al posto di Mat
    String imgAcquisitaPath = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/immagine acquisita.jpg");
    const char *pathImgAcquisitaChar = imgAcquisitaPath.c_str();
    imwrite(pathImgAcquisitaChar, image);


    // load flandmark model structure and initialize
    //FLANDMARK_Model * model = flandmark_init("flandmark_model.dat");

    String flandmarkPath = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/flandmark_model.dat");

    const char *pathFlandChar = flandmarkPath.c_str();

    FLANDMARK_Model * model = flandmark_init(pathFlandChar);

    if (model==NULL){
        QMessageBox::critical(this, "FLANDMARK LIB", "libreria flandmark non caricata correttamente!!");
    }

    //QMessageBox::information(this, "superato", "superato caricamento flandmark model");

    // load input image
    IplImage *img = cvLoadImage(pathImgAcquisitaChar);

    //QMessageBox::information(this, "prima crash", "prima della funzione \nIplImage *img_grayscale = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 1);");

    // convert image to grayscale
    IplImage *img_grayscale = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 1);

    //QMessageBox::information(this, "dopo crash", "superata la funzione IplImage... ");

    cerr << "g1: " << img_grayscale->width << endl;

    //QMessageBox::information(this,"cvtColor", "prima di cvCvtColor(img, img_grayscale, CV_BGR2GRAY);");

    cvCvtColor(img, img_grayscale, CV_BGR2GRAY);

    cerr << "g2: " <<  img_grayscale->width << endl;

    // haarcascade usa Mat
    Mat matGrayImage (img_grayscale);

    cerr << "matGray: " << matGrayImage.size().width << endl;

    equalizeHist(matGrayImage, matGrayImage);


    //se sono stati indivisuati più volti, individuo l'indice del volto più grande
    int i=0;
    if (faces.size()>1){
        for (size_t j=1; j<faces.size(); j++){
            if (faces[j].width > faces[j-1].width){
                i=j;
            }
        }
    }


    // HAAR CASCADE + FLANDMARK
    // per individuare più facce: for( size_t i = 0; i < faces.size(); i++ )
    if (faces.size()!=0)
    {

        //riferimento: altezza del volto individuato con face_cascade diviso 12 (semitoni presenti in un'ottava)
        //tutti i parametri trovati successivamente devono essere riportati a questa misura
        float riferimento = (float)faces[i].height/12;

        caratteristiche += "Altezza del volto: " + QString::number(faces[i].height) +"\n";
        caratteristiche += "Riferimento per le distanze: " + QString::number(riferimento) + "\n";
        caratteristiche += "Riferimento = Altezza / 12 \n";

        //dimensione minima per i punti caratteristici, usata da detectMultiScale
        Size dimensioneMinimaPuntiCaratteristici (faces[i].width/20, faces[i].height/20);

        /*
         * //usato per identificare i parametri dei vari volti. non utilizzato perchè vogliamo individuare un unico volto per ogni immagine
        if (faces.size()>1){
            caratteristiche += "\nVolto n. " + QString::number(i+1) + "\n";
        }

        */

        //disegno un cerchietto nel punto in alto a sinistra rispetto al volto individuato
        Point upperLeft (faces[i].x, faces[i].y);
        //        circle(frame, upperLeft, 5, Scalar (255,255,255));

        //punto in basso a destra del volto
        Point lowerRight (upperLeft.x+faces[i].width, upperLeft.y+faces[i].height);


        std::cerr << "area faccia: " << faces[i].height << " - " << faces[i].width << endl;

        //punto centrale del rettangolo che identifica il volto
        //Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
        //ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 0, 0, 0 ), 4, 8, 0 );

        //        Mat faceROI = frame_gray( faces[i] );
        /*
        Mat hsvFaceROI = frame(faces[i]).clone();

        //QMessageBox::information(this,"cvtColor", "cvtColor(hsvFaceROI,hsvFaceROI,CV_BGR2HSV);");

        //se da errore in esecuzione usa due Mat diversi
        cvtColor(hsvFaceROI,hsvFaceROI,CV_BGR2HSV);

*/
        //individuo le zone di interesse per la rilevazione di occhi, bocca, naso

        CvRect eyesROIRect = cvRect(faces[i].x, faces[i].y + (faces[i].height/6), faces[i].width, faces[i].height/2.5);
        Mat eyesROI = frame_gray(eyesROIRect);

        CvRect nosesROIRect = cvRect(faces[i].x, faces[i].y + (faces[i].height/3), faces[i].width, faces[i].height/2.2);
        Mat nosesROI = frame_gray(nosesROIRect);

        CvRect mouthROIRect = cvRect(faces[i].x, faces[i].y + (faces[i].height/1.5), faces[i].width, faces[i].height/3);
        Mat mouthROI = frame_gray(mouthROIRect);


        // xMin e deltaxMax servono per non rischiare di mettere i rettangoli fuori dall'immagine
        int xMin;
        if ((faces[i].x - (faces[i].width/8)) >0){
            xMin = faces[i].x - (faces[i].width/8);
        } else {
            xMin = 0;
        }

        int deltaxMax;
        if ((faces[i].x + faces[i].width*0.75 + faces[i].width/3) > frame_gray.size().width ){
            deltaxMax = frame_gray.size().width - (faces[i].x + (faces[i].width*0.75));
        } else {
            deltaxMax = faces[i].width/3;
        }

        cerr <<"xmin: " << xMin<< endl;
        cerr << "deltaMax: "<< deltaxMax <<endl;
        /*
 * orecchie
        CvRect rightEarsROIRect = cvRect(xMin, faces[i].y + (faces[i].height/3), faces[i].width/3, faces[i].height/2);
        Mat rightEarsROI = frame_gray(rightEarsROIRect);

        CvRect leftEarsROIRect = cvRect(faces[i].x + (faces[i].width*0.75), faces[i].y + (faces[i].height/3), deltaxMax, faces[i].height/2);
        Mat leftEarsROI = frame_gray(leftEarsROIRect);

        Mat hsvRightEarROI = frame(rightEarsROIRect).clone();
        Mat hsvLeftEarROI = frame(leftEarsROIRect).clone();

        cvtColor (hsvRightEarROI, hsvRightEarROI, CV_BGR2HSV);
        cvtColor (hsvLeftEarROI, hsvLeftEarROI, CV_BGR2HSV);
*/
        //disegno i rettangoli intorno alle zone di interesse
        rectangle(frame, upperLeft, lowerRight, Scalar(255,255,255), 2);
        rectangle(frame, eyesROIRect , Scalar (255,20,20));
        rectangle(frame, nosesROIRect , Scalar (20,20,255));
        rectangle(frame, mouthROIRect, Scalar(20,255,20));
        //rectangle(frame, rightEarsROIRect , Scalar (0,180,255));
        //rectangle(frame, leftEarsROIRect , Scalar (180,0,255));

        std::vector<Rect> eyes;

        //prototipo:
        //void CascadeClassifier::detectMultiScale(const Mat& image, vector<Rect>& objects, double scaleFactor=1.1, int minNeighbors=3, int flags=0, Size minSize=Size(), Size maxSize=Size())

        //-- In each face, detect eyes
        eyes_cascade.detectMultiScale( eyesROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, dimensioneMinimaPuntiCaratteristici );

        std::cerr << "occhi trovati: " << eyes.size() << endl;

        for( size_t j = 0; j < eyes.size(); j++ )
        {
            std::cerr << "area occhio: " << eyes[j].height << " - " << eyes[j].width << endl;

            Point center( eyesROIRect.x + eyes[j].x + eyes[j].width*0.5, eyesROIRect.y + eyes[j].y + eyes[j].height*0.5 );

            //int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
            //circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );

            rectangle(frame, Point (eyesROIRect.x + eyes[j].x, eyesROIRect.y + eyes[j].y ), Point( eyesROIRect.x + eyes[j].x + eyes[j].width, eyesROIRect.y + eyes[j].y +eyes[j].height), Scalar (255,0,0),2);

        }

        /*
         * per la distanza degli occhi utilizzo i punti di flandmarks (occorre calcolare la distanza fra i punti esterni degli occhi)
        if (eyes.size()==2){

            cerr << "distanza fra gli occhi: " << sqrt ((eyes[0].x-eyes[1].x)*(eyes[0].x-eyes[1].x) + (eyes[0].y-eyes[1].y)*(eyes[0].y-eyes[1].y) ) << endl;

            caratteristiche += "Distanza fra gli occhi: " + QString::number(sqrt ((eyes[0].x-eyes[1].x)*(eyes[0].x-eyes[1].x) + (eyes[0].y-eyes[1].y)*(eyes[0].y-eyes[1].y))) + "\n";
        } else {
            caratteristiche += "Distanza fra gli occhi non calcolata\n";
        }
        */


        //riconoscimento naso - CV_HAAR_FIND_BIGGEST_OBJECT resituisce solo l'oggetto più grande

        std::vector<Rect> noses;

        nose_cascade.detectMultiScale ( nosesROI, noses, 1.1, 2, CV_HAAR_FIND_BIGGEST_OBJECT, dimensioneMinimaPuntiCaratteristici );

        std::cerr << "nasi trovati: " << noses.size() << endl;

        Point noseCenter;

        for( size_t j = 0; j < noses.size(); j++ )

        {
            std::cerr << "area naso: " << noses[j].height << " - " << noses[j].width << endl;

            noseCenter.x = (nosesROIRect.x + noses[j].x + noses[j].width*0.5);
            noseCenter.y = (nosesROIRect.y + noses[j].y + noses[j].height*0.5);
            //int radius = cvRound( (noses[j].width + noses[j].height)*0.25 );
            //circle( frame, center, radius, Scalar( 0, 0, 255 ), 4, 8, 0 );
            rectangle(frame, Point (nosesROIRect.x + noses[j].x, nosesROIRect.y + noses[j].y ), Point( nosesROIRect.x + noses[j].x + noses[j].width, nosesROIRect.y + noses[j].y +noses[j].height), Scalar (0,0,255),2);
        }


        //riconoscimento bocca - CV_HAAR_FIND_BIGGEST_OBJECT resituisce solo l'oggetto più grande
        std::vector<Rect> mouths;

        mouth_cascade.detectMultiScale(mouthROI, mouths, 1.1, 2, CV_HAAR_FIND_BIGGEST_OBJECT, dimensioneMinimaPuntiCaratteristici);

        std::cerr << "bocche trovate: " << mouths.size() << endl;

        Point mouthCenter;

        if (mouths.size()!=0) {

            mouthCenter.x = (mouthROIRect.x + mouths[0].x + mouths[0].width*0.5);
            mouthCenter.y = (mouthROIRect.y + mouths[0].y + mouths[0].height*0.5);
            //int radius = cvRound( (mouths[max].width + mouths[max].height)*0.25 );
            //circle( frame, center, radius, Scalar( 0, 255, 0 ), 4, 8, 0 );
            rectangle(frame, Point (mouthROIRect.x + mouths[0].x, mouthROIRect.y + mouths[0].y ), Point( mouthROIRect.x + mouths[0].x + mouths[0].width, mouthROIRect.y + mouths[0].y +mouths[0].height), Scalar (0,255,0),2);
        }

        /*
        if (noses.size()!=0 && mouths.size()!=0){
            caratteristiche += "Distanza fra bocca e naso: " + QString::number(sqrt ((noseCenter.x-mouthCenter.x)*(noseCenter.x-mouthCenter.x) + (noseCenter.y-mouthCenter.y)*(noseCenter.y-mouthCenter.y))) +"\n";
        }
        */

        // FLANDMARK

        //QMessageBox::information(this, "superato", "prima di algoritmo flandmark");

        cerr << "flandmark " << endl;

        // bbox with detected face (format: top_left_col top_left_row bottom_right_col bottom_right_row)
        int bbox[] = {faces[i].x, faces[i].y, faces[i].x+faces[i].width, faces[i].y+faces[i].height};

        cerr << "dopo bbox[] " << bbox[0] << " - " << bbox[1] <<  " - " <<bbox[2] << " - " << bbox[3] << endl;

        Mat mat(img_grayscale);
        String pathGray = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/gray.jpg");
        const char *pathGrayChar = pathGray.c_str();
        imwrite(pathGrayChar , mat);

        // detect facial landmarks (output are x, y coordinates of detected landmarks)
        double * landmarks = (double*)malloc(2*model->data.options.M*sizeof(double));

        cerr << "dopo double *landm " << endl;

        flandmark_detect(img_grayscale, bbox, model, landmarks);

        cerr << "dopo flandmark_detect " << endl;

        /*
         * if (landmarks[0]!=NULL && landmarks[1]!=NULL && landmarks[2]!=NULL && landmarks[3]!=NULL &&
                landmarks[4]!=NULL && landmarks[5]!=NULL && landmarks[6]!=NULL && landmarks[7]!=NULL &&
                landmarks[8]!=NULL && landmarks[9]!=NULL && landmarks[10]!=NULL && landmarks[11]!=NULL &&
                landmarks[12]!=NULL && landmarks[13]!=NULL && landmarks[14]!=NULL && landmarks[15]!=NULL){
         */

        //se sono stati riconosciuti tutti i landmarks vado avanti. per evitare crash dell'applicazione quando non trova tutti i punti
        if ((int)landmarks[0]!=0 && (int)landmarks[1]!=0 && (int)landmarks[2]!=0 && (int)landmarks[3]!=0 &&
                (int)landmarks[4]!=0 && (int)landmarks[5]!=0 && (int)landmarks[6]!=0 && (int)landmarks[7]!=0 &&
                (int)landmarks[8]!=0 && (int)landmarks[9]!=0 && (int)landmarks[10]!=0 && (int)landmarks[11]!=0 &&
                (int)landmarks[12]!=0 && (int)landmarks[13]!=0 && (int)landmarks[14]!=0 && (int)landmarks[15]!=0){

            cerr << "dentro if landmarks[i]!=NULL " << endl;

            //disegno sull'immagine i landmark
            for (int i = 2; i < 2*model->data.options.M; i += 2)
            {
                //IlpImage img non viene mai visualizzato
                //cvCircle(img, Point(int(landmarks[i]), int(landmarks[i+1])), 3, Scalar (255,0,0));
                circle(frame,Point(int (landmarks[i]), int(landmarks[i+1])), 5, Scalar (255,0,0),2);
            }

            cerr << "limite faccia: " << bbox[0] << " - " << bbox[1] << " ... " << bbox[2] << " - " << bbox[3] << endl;

            Point occhioDD = Point (int(landmarks[10]), int(landmarks[11]));

            std::cerr << "occhio DD: " << occhioDD.x << " - " << occhioDD.y << endl ;

            Point occhioDS = Point (int(landmarks[2]), int(landmarks[3]));

            std::cerr << "occhio DS: " << occhioDS.x << " - " << occhioDS.y << endl ;

            Point occhioSD = Point (int(landmarks[4]), int(landmarks[5]));

            std::cerr << "occhio SD: " << occhioSD.x << " - " << occhioSD.y << endl ;

            Point occhioSS = Point (int(landmarks[12]), int(landmarks[13]));

            std::cerr << "occhio SS: " << occhioSS.x << " - " << occhioSS.y  << endl;

            Point boccaD = Point (int(landmarks[6]), int(landmarks[7]));

            std::cerr << "bocca D: " << boccaD.x << " - " << boccaD.y  << endl;

            Point boccaS = Point (int(landmarks[8]), int(landmarks[9]));

            std::cerr << "bocca S: " << boccaS.x << " - " << boccaS.y  << endl;

            Point naso = Point (int(landmarks[14]), int(landmarks[15]));

            std::cerr << "naso: " << naso.x << " - " << naso.y  << endl;

            /*
 * orecchie
            // naso contiene la posizione rispetto all'immagine intera, la funzione segmentazioneColore utilizza solo la porzione con il volto per questo - faces[i].x o y
            // come punto di riferimento prendo un punto sul naso, in verticale prendo la media dell'altezza degli occhi e quella del punto inferiore del naso
            Point nasoRiferitoAlVolto (naso.x - faces[i].x, (naso.y+occhioDD.y)/2 - faces[i].y );
            circle(hsvFaceROI, nasoRiferitoAlVolto,3,Scalar (0,0,0),3 );

            String pathHSVFace = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/hsvFace.jpg");
            const char *pathHSVFaceChar = pathHSVFace.c_str();
            imwrite(pathHSVFaceChar, hsvFaceROI);

            // ERRORE: SE BYPASSO L'IF CON I !=0 DICE CHE nasoRiferitoAlVolto VA OLTRE L'IMMAGINE (PER LE IMMAGINI CON IL VOLTO SU UN LATO)

            Vec3b coloreRiferimento = segmentazioneColore(hsvFaceROI, nasoRiferitoAlVolto);

            lunghezzaOrecchio(hsvRightEarROI, coloreRiferimento);

*/

            /*
 * come distanza uso quella fra i punti esterni
            // calcolo la distanza fra gli occhi considerando il centro dell'occhio come la media fra gli estremi
            Point occhioDMedia = Point ((occhioDD.x+occhioDS.x)/2, (occhioDD.y+occhioDS.y)/2);

            Point occhioSMedia = Point ((occhioSD.x+occhioSS.x)/2, (occhioSD.y+occhioSS.y)/2);

            float distanzaMediaOcchi = sqrt((occhioDMedia.x-occhioSMedia.x)*(occhioDMedia.x-occhioSMedia.x) + (occhioDMedia.y-occhioSMedia.y)*(occhioDMedia.y-occhioSMedia.y));

            cerr << "distanza fra gli occhi: " << distanzaMediaOcchi << endl;

            caratteristiche += "Distanza fra gli occhi: " +QString::number(distanzaMediaOcchi) + "\n";
*/

            //CHIEDERE SE SERVE LA LARGHEZZA DEL SINGOLO OCCHIO
            float larghezzaOcchioD = sqrt((occhioDD.x-occhioDS.x)*(occhioDD.x-occhioDS.x)+(occhioDD.y-occhioDS.y)*(occhioDD.y-occhioDS.y));

            float larghezzaOcchioS = sqrt((occhioSD.x-occhioSS.x)*(occhioSD.x-occhioSS.x)+(occhioSD.y-occhioSS.y)*(occhioSD.y-occhioSS.y));

            caratteristiche += "Ampiezza occhio destro: " + QString::number(larghezzaOcchioD) + "\n";
            caratteristiche += "Ampiezza occhio sinistro: " + QString::number(larghezzaOcchioS) + "\n";

            //PARAMETRI CARATTERISTICI DEL VOLTO

            // VERTICALI

            //Altezza della fronte
            //limite superiore del volto individuato - altezza media degli occhi (flandmarks)
            float altezzaMediaOcchi = (occhioDD.y+occhioDS.y+occhioSD.y+occhioSS.y)/4;
            float altezzaFronte = altezzaMediaOcchi - upperLeft.y;
            // +0.5 è stato aggiunto in modo tale che parametroFronte non sia ottenuto troncando il risultato, ma arrotondandolo. (il parametro è sempre positivo)
            int parametroFronte = altezzaFronte/riferimento + 0.5;

            cerr << occhioDD.y << " " << occhioDS.y << " " << occhioSD.y << " " << occhioSS.y;
            cerr << endl << altezzaMediaOcchi << endl << upperLeft.y;

            caratteristiche += "Ampiezza verticale della fronte: " + QString::number(altezzaFronte) + "\n";
            caratteristiche += "Parametro fronte: " + QString::number(parametroFronte) + "\n";

            //Alteza del naso
            //punta del naso (flandmark) - punto di partenza del naso (utilizzo altezzaMediaOcchi)
            int altezzaNaso = naso.y-altezzaMediaOcchi;
            int parametroNaso = altezzaNaso/riferimento + 0.5;

            caratteristiche += "Ampiezza verticale del naso: " + QString::number(altezzaNaso) + "\n";
            caratteristiche += "Parametro naso: " + QString::number(parametroNaso) + "\n";

            //Distanza naso-bocca
            //Altezza media della bocca (flandmark) - altezza naso (flandmark)
            float altezzaBocca = (boccaS.y+boccaD.y)/2;
            float distanzaBoccaNaso = altezzaBocca - naso.y;
            int parametroBoccaNaso = distanzaBoccaNaso/riferimento + 0.5;

            caratteristiche += "Distanza verticale bocca-naso: " + QString::number(distanzaBoccaNaso) + "\n";
            caratteristiche += "Parametro bocca-naso: " + QString::number(parametroBoccaNaso) + "\n";


            //ORIZZONTALI

            //Distanza degli occhi
            //Calcolata dai punti più esterni di ciascun occhio (flandmark)
            int distanzaOcchi = occhioSS.x-occhioDD.x;
            int parametroOcchi = distanzaOcchi/riferimento + 0.5;

            caratteristiche += "Distanza fra gli occhi: " + QString::number(distanzaOcchi) + "\n";
            caratteristiche += "Parametro occhi: " + QString::number(parametroOcchi) + "\n";

            //Larghezza della bocca (flandmark)
            float larghezzaBocca = sqrt((boccaS.x-boccaD.x)*(boccaS.x-boccaD.x)+(boccaS.y-boccaD.y)*(boccaS.y-boccaD.y));
            int parametroBocca = larghezzaBocca/riferimento + 0.5;

            caratteristiche += "Larghezza bocca: " + QString::number(larghezzaBocca) + "\n";
            caratteristiche += "Parametro bocca: " + QString::number(parametroBocca) + "\n";

            float distanzeDivisoAltezza = (larghezzaBocca + distanzaOcchi + distanzaBoccaNaso + altezzaNaso + altezzaFronte) / faces[i].height;
            // float distanzeDivisoAltezza = (larghezzaBocca + distanzaOcchi + distanzaBoccaNaso + altezzaNaso + altezzaFronte) / riferimento;
            cerr << "distanze tot/alt: " << distanzeDivisoAltezza << endl;
            caratteristiche += "Totale distanze/altezza volto: " + QString::number(distanzeDivisoAltezza) + "\n";
            /*
            // naso contiene la posizione rispetto all'immagine intera, la funzione segmentazioneColore utilizza solo la porzione con il volto per questo - faces[i].x o y
            // come punto di riferimento prendo un punto sul naso, in verticale prendo la media dell'altezza degli occhi e quella del punto inferiore del naso
            Point nasoRiferitoAlVolto (naso.x - faces[i].x, (naso.y+occhioDD.y)/2 - faces[i].y );

            Mat puntoDiRiferimento = hsvFaceROI(Rect(nasoRiferitoAlVolto.x, nasoRiferitoAlVolto.y,1,1));
            //circle(hsvFaceROI, nasoRiferitoAlVolto,3,Scalar (0,0,0),3 );

            String pathHSVFace = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/hsvFace.jpg");
            const char *pathHSVFaceChar = pathHSVFace.c_str();
            imwrite(pathHSVFaceChar, hsvFaceROI);

            //trova il colore sul punto centrale del naso e crea un file contenente la maschhera del volto
            segmentazioneColore(hsvFaceROI, nasoRiferitoAlVolto);

            Vec3b coloreDiRiferimento = puntoDiRiferimento.at<Vec3b>(0,0);
            int H = coloreDiRiferimento.val[0]; // hue
            int S = coloreDiRiferimento.val[1]; // saturation
            int V = coloreDiRiferimento.val[2]; // value

            caratteristiche += "punto di rif: H: " + QString::number(H) + "  S: " + QString::number(S) + "  V: " + QString::number(V) + "\n";

            Vec3b coloreTotale = mediaColoreCompleta(hsvFaceROI, H);
            caratteristiche += "colore del volto: H: " + QString::number(coloreTotale.val[0]) + "  S: " + QString::number(coloreTotale.val[1]) + "  V: " + QString::number(coloreTotale.val[2]) + "\n";

            //media colore su un rettangolo intorno al naso
            Vec3b colore = mediaColore(hsvFaceROI, Point(nasoRiferitoAlVolto.x, nasoRiferitoAlVolto.y),faces[i].height/15, faces[i].width/4, H);
            caratteristiche += "media: H: " + QString::number(colore.val[0]) + "  S: " + QString::number(colore.val[1]) + "  V: " + QString::number(colore.val[2]) + "\n";

*/
            Mat output (img);

            String pathOutput = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/output.jpg");
            const char *pathOutputChar = pathOutput.c_str();
            imwrite(pathOutputChar, output);

            visualizzaImmagine(output,imgWidget);

            //calcolo le note e scrivo il file midi
            /*
            //apro il file settings e recupero i valori delle ottave di riferimento
            QSettings settings("sisinflab", "MusicaHumana");
            int ottavaFronte = settings.value("fronte", 1).toInt();
            int ottavaNaso = settings.value("naso", 2).toInt();
            int ottavaBoccaNaso = settings.value("boccaNaso", 3).toInt();
            int ottavaOcchi = settings.value("occhi", 4).toInt();
            int ottavaBocca = settings.value("bocca", 5).toInt();
*/
            int ottavaFronte = OTTAVA_FRONTE;
            int ottavaNaso = OTTAVA_NASO;
            int ottavaBoccaNaso = OTTAVA_BOCCA_NASO;
            int ottavaOcchi = OTTAVA_OCCHI;
            int ottavaBocca = OTTAVA_BOCCA;


            //calcolo la tonica come somma di tutti i parametri (modulo 7) - poi si calcola l'intervallo partendo da sol
            tonica=((parametroBocca+parametroBoccaNaso+parametroFronte+parametroNaso+parametroOcchi)%7);

            //sol dev'essere associato al resto 1 e non al resto 0   -> tonica = tonica-1
            tonica = (tonica + 6 )%7;

            //più 12 perchè altrimenti va a scrivere all'ottava 0 o -1
            vettoreNote [0] = vettoreIntervalli[tonica] + SOL +12;

            //se la tonica è sol - la - si bisogna spostare di un'altra ottava altrimenti si va oltre la tastiera
            if(tonica<3){
                vettoreNote[0] = vettoreNote[0] + 12;
            }

            cerr << endl << "tonica: " << tonica <<endl;
            cerr << "tonica midi: " << vettoreNote[0] << endl;

            impostaIntervalliDaTonica();

            //si parte da G0, se la tonica è G0, A0, B0 -> le ottave di riferimento sono giuste, altrimenti la tonica sta sull'ottava 1 e bisogna incrementare tutte le altre ottave di riferimento
            if (vettoreNote[0]>2){
                ottavaBocca++;
                ottavaBoccaNaso++;
                ottavaFronte++;
                ottavaNaso++;
                ottavaOcchi++;
            }

            //calcolo le altre note
            vettoreNote[1] = calcolaNumeroNotaMidi(parametroFronte, ottavaFronte);          //1
            vettoreNote[2] = calcolaNumeroNotaMidi(parametroNaso, ottavaNaso);              //2
            vettoreNote[3] = calcolaNumeroNotaMidi(parametroBoccaNaso, ottavaBoccaNaso);    //3
            vettoreNote[4] = calcolaNumeroNotaMidi(parametroOcchi, ottavaOcchi);            //4
            vettoreNote[5] = calcolaNumeroNotaMidi(parametroBocca, ottavaBocca);            //5

            bool duplicati=true;
            while(duplicati==true){
                duplicati=trovaDuplicatiESpostaOttava(vettoreNote,6);
            }

            salvaMIDI(vettoreNote, 6);

        }

        //libero la memoria
        free(landmarks);
        cvReleaseImage(&img);
        cvReleaseImage(&img_grayscale);
        flandmark_free(model);
    }
    //-- Show what you got
    String pathRiconoscimento = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/riconoscimento.jpg");
    const char *pathRiconoscimentoChar = pathRiconoscimento.c_str();

    imwrite(pathRiconoscimentoChar, frame);

    visualizzaImmagine(frame,imgWidget);

    //scrivo sull'interfaccia grafica
    ui->labelDistanze->setText(caratteristiche);

}
/*
float MainWindow::lunghezzaOrecchio(Mat orecchioROI, Vec3b riferimento)
{
    // orecchio ROI è già in hsv

    int H = riferimento.val[0]; // hue
    int S = riferimento.val[1]; // saturation
    int V = riferimento.val[2]; // value

    Mat imgThresholded;

    // CONTROLLA COSA SUCCEDE SE IL VALORE DI H VA SOTTO LO 0

    // sogliatura
    inRange (orecchioROI , Scalar (H-5,S-70,V-70), Scalar (H+5,S+70,V+70), imgThresholded);    //CONTROLLA I PARAMETRI

    imwrite("ear-thresholded.jpg",imgThresholded);

    // eseguo l'erosione dell'immagine ottenuta con la sogliatura per rimouovere outliers
    Mat kernel = getStructuringElement(MORPH_RECT, Size (3,3), Point (1,1));
    Mat imgEroded;
    erode (imgThresholded, imgEroded, kernel);
    imwrite("ear-eroded.jpg", imgEroded);

    // effettuo la ricerca dei blob

    // parametri per il detector
    SimpleBlobDetector::Params params;
    params.minDistBetweenBlobs = 50.0f;
    params.filterByInertia = false;
    params.filterByConvexity = false;
    params.filterByColor = true; //mi interessa estrarre le regioni di colore bianco
    params.filterByCircularity = false;
    params.filterByArea = false;
    params.blobColor=255; //per estrarre le regioni bianche

    // istanzio il detector
    SimpleBlobDetector blobDetector(params);

    //detect
    vector <KeyPoint> keypoints;
    blobDetector.detect(imgEroded, keypoints);

    //stampo i keypoints
    // extract the x y coordinates of the keypoints:
    for (size_t i=0; i<keypoints.size(); i++){
        float x = keypoints[i].pt.x;
        float y = keypoints[i].pt.y;
        cerr << "keypoint: " << x << " - " << y << endl;
        circle(orecchioROI, Point (x,y), 3, Scalar (255,0,0));
    }

    imwrite("keypoints.jpg" , orecchioROI);

    return 0;

} */

void MainWindow::salvaMIDI(int* nota, int numeroNote)
{



    /*
    // ad ogni nota è associato un evento di "note on" e uno di "note off"
    int lunghezzaTraccia = numeroNote*8;   // VEDI PER QUANTO LO DEVI MOLTIPLICARE
*/

    // creo il file midi per l'output
    ofstream outputFile;

    QString pathMidiFile;

    // se è presente un valore del file delle impostazioni e se è selezionato il non chiedere più chiedo all'utente se vuole usare tale impostazione
    //altrimenti apro una finestra per la scelta del percorso
    QSettings settings("sisinflab", "MusicaHumana");
    if (settings.value("midiPath").toString() != NULL && settings.value("nonChiedereMidiPath").toBool()==true){
        //uso il default
        pathMidiFile += settings.value("midiPath").toString();
    } else if (settings.value("midiPath").toString() != NULL)
    {
        //chiedo se vuole usare il default
        DialogUsaDefaultPath *defaultWindow = new DialogUsaDefaultPath();
        defaultWindow->setWindowTitle("Usa default");
        // se vuol usare il default
        if (defaultWindow->exec()){
            //se ha selezionato "non chiedere piu" lo scrivo sul file di configurazione e uso il default
            if (defaultWindow->isCheckedNonChiederePiu()){
                settings.setValue("nonChiedereMidiPath", true);
            }
            //uso il default
            pathMidiFile += settings.value("midiPath").toString();
            // se non vuole usare il default
        }else{
            // chiedo il percorso
            pathMidiFile += QFileDialog::getSaveFileName(this);
        }
    } else {
        // chiedo il percorso
        pathMidiFile += QFileDialog::getSaveFileName(this);
    }

    //se il file non finisce con .mid o .midi aggiungo l'estensione
    if( ! pathMidiFile.endsWith(".mid") && ! pathMidiFile.endsWith(".midi")){
        pathMidiFile += ".mid";
    }


    //String pathMidiFile = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/output.mid");

    //const char *pathMidiFileChar = pathMidiFile.toLocal8Bit().data().c_str();
    outputFile.open(pathMidiFile.toLocal8Bit().data(), ios::binary);

    // MThd: Midi Track header chunk   4D 54 68 64
    outputFile << static_cast <unsigned char> (0x4D) << static_cast <unsigned char> (0x54) << static_cast <unsigned char> (0x68) << static_cast <unsigned char> (0x64);

    // lunghezza in byte della restante parte dell'header 00 00 00 06
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x06);

    // tipo di file midi (traccia singola = 00 00 - multitraccia sincrone = 00 01)
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x01);

    // numero tracce MTkr
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x05);

    // risoluzione per nota da 1/4 (PPQN)
    outputFile << static_cast <unsigned char> (0x03) << static_cast <unsigned char> (0xC0);

    // MTtr: Midi Track chunk 4D 54 72 6B
    outputFile << static_cast <unsigned char> (0x4D) << static_cast <unsigned char> (0x54) << static_cast <unsigned char> (0x72) << static_cast <unsigned char> (0x6B);

    //byte rimanenti fino alla fine della traccia
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x24);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // tempo metronomico
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x51) << static_cast <unsigned char> (0x03) <<
                  static_cast <unsigned char> (0x07) << static_cast <unsigned char> (0xA1) << static_cast <unsigned char> (0x20);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // chiave di tonalità
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x59) << static_cast <unsigned char> (0x02) <<
                  static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // nome della traccia = output = 6f7574707574
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x03) << static_cast <unsigned char> (0x07) <<
                  static_cast <unsigned char> (0x6f) << static_cast <unsigned char> (0x75) << static_cast <unsigned char> (0x74) <<
                  static_cast <unsigned char> (0x70) << static_cast <unsigned char> (0x75) << static_cast <unsigned char> (0x74) <<
                  static_cast <unsigned char> (0x00);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // metro della misura = 4/4
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x58) << static_cast <unsigned char> (0x04) <<
                  static_cast <unsigned char> (0x04) << static_cast <unsigned char> (0x02) << static_cast <unsigned char> (0x18) <<
                  static_cast <unsigned char> (0x08);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    //chiusura della traccia
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x2F) << static_cast <unsigned char> (0x00);



    // MTtr: Midi Track chunk 4D 54 72 6B
    outputFile << static_cast <unsigned char> (0x4D) << static_cast <unsigned char> (0x54) << static_cast <unsigned char> (0x72) << static_cast <unsigned char> (0x6B);

    //byte rimanenti fino alla fine della traccia
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0xAF);


    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // Bank select B0 = 00 (canale 0)
    outputFile << static_cast <unsigned char> (0xB0) << static_cast <unsigned char> (0x00);

    //MSB for Bank select
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00);

    //LSB for Bank select
    outputFile << static_cast <unsigned char> (0x20) << static_cast <unsigned char> (0x00);

    // delay
    outputFile << static_cast <unsigned char> (0x00);

    //program (patch) change -> determina quale strumento deve suonare
    outputFile << static_cast <unsigned char> (0xC0) << static_cast <unsigned char> (0x00);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // nome della traccia = traccia1
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x03) << static_cast <unsigned char> (0x09) <<
                  static_cast <unsigned char> (0x74) << static_cast <unsigned char> (0x72) << static_cast <unsigned char> (0x61) <<
                  static_cast <unsigned char> (0x63) << static_cast <unsigned char> (0x63) << static_cast <unsigned char> (0x69) <<
                  static_cast <unsigned char> (0x61) << static_cast <unsigned char> (0x31) << static_cast <unsigned char> (0x00);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // Control change n.7 (volume)
    outputFile << static_cast <unsigned char> (0xB0) << static_cast <unsigned char> (0x07) << static_cast <unsigned char> (0x5A);  //40=-6db ; 7f=+6db

    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) <<
                  static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x20) << static_cast <unsigned char> (0x00) <<
                  static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x07) << static_cast <unsigned char> (0x5A);

    // Control change n.91 (riverbero)
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x5B) << static_cast <unsigned char> (0x7F);

    // ciclo for parte da 1 perchè sta roba viene scritta n-1 volte (se cè una sola nota non serve) - bugia, sta roba sta tre/quattro volte anche con 5 note (??)
    //bank select?

    //for (int i=0; i<4; i++){
    for (int i=0; i<5; i++){

        outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) <<
                      static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x20) << static_cast <unsigned char> (0x00) <<
                      static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x07) << static_cast <unsigned char> (0x5A);   //ultimo era 7f      era 40
    }

    // ACCORDO 6 BATTUTE

    //delay time + note on
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x90);

    for (int i=0; i<numeroNote; i++){

        //nota
        outputFile << (char)nota[i];

        //velocità
        outputFile << static_cast <unsigned char> (0x5A);

        //l'ultima volta il delay è diverso
        if (i!=numeroNote-1)
            outputFile << static_cast <unsigned char> (0x00);

    }

    //delay time + note off
    //outputFile << static_cast <unsigned char> (0x81) << static_cast <unsigned char> (0xB4) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x80);
    outputFile << static_cast <unsigned char> (0xBC) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x80);


    for (int i=0; i<numeroNote; i++){


        //nota
        outputFile << (char)nota[i];
        //velocità
        outputFile << static_cast <unsigned char> (0x00);

        //delay time
        if(i!=numeroNote-1){
            outputFile << static_cast <unsigned char> (0x00);
        } else {
            //dall'ultima nota c'è una pausa di un po di battute (fino alla melodia finale)
            outputFile << static_cast <unsigned char> (0x83) << static_cast <unsigned char> (0xC2) << static_cast <unsigned char> (0x00);
        }
    }

    //ACCORDO SULLA MELODIA
    //note on
    outputFile << static_cast <unsigned char> (0x90);

    //accordo (senza la nota nell'ottava centrale)
    for (int i=0; i<numeroNote; i++){
        //nota
        outputFile << (char)nota[i];

        //velocità
        outputFile << static_cast <unsigned char> (0x5A);

        //delay time
        if(i!=numeroNote-1){
            outputFile << static_cast <unsigned char> (0x00);
        } else {
            //durata della melodia
            outputFile << static_cast <unsigned char> (0x81) << static_cast <unsigned char> (0xA5) << static_cast <unsigned char> (0x00);
        }
    }
    //chiusura accordo
    //note off
    outputFile << static_cast <unsigned char> (0x80);

    for (int i=0; i<numeroNote; i++){

        //nota
        outputFile << (char)nota[i];
        //velocità
        outputFile << static_cast <unsigned char> (0x00);

        //delay time
        if(i!=numeroNote-1){
            outputFile << static_cast <unsigned char> (0x00);
        } else {
            //outputFile << static_cast <unsigned char> (0x8E) << static_cast <unsigned char> (0x7F);
            outputFile << static_cast <unsigned char> (0x82) << static_cast <unsigned char> (0x9C) << static_cast <unsigned char> (0x7F);
        }

    }

    //FINE TRACCIA
    //all note off
    outputFile << static_cast <unsigned char> (0xB0) << static_cast <unsigned char> (0x7F) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00);
    //chiusura della traccia
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x2F) << static_cast <unsigned char> (0x00);


    //TRACCIA2
    // MTtr: Midi Track chunk 4D 54 72 6B
    outputFile << static_cast <unsigned char> (0x4D) << static_cast <unsigned char> (0x54) << static_cast <unsigned char> (0x72) << static_cast <unsigned char> (0x6B);

    //byte rimanenti fino alla fine della traccia
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x5C);


    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // Bank select B0 = 00 (canale 0)
    outputFile << static_cast <unsigned char> (0xB1) << static_cast <unsigned char> (0x00);

    //MSB for Bank select
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00);

    //LSB for Bank select
    outputFile << static_cast <unsigned char> (0x20) << static_cast <unsigned char> (0x00);

    // delay
    outputFile << static_cast <unsigned char> (0x00);

    //program (patch) change -> determina quale strumento deve suonare
    outputFile << static_cast <unsigned char> (0xC1) << static_cast <unsigned char> (0x00);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // nome della traccia = traccia2
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x03) << static_cast <unsigned char> (0x09) <<
                  static_cast <unsigned char> (0x74) << static_cast <unsigned char> (0x72) << static_cast <unsigned char> (0x61) <<
                  static_cast <unsigned char> (0x63) << static_cast <unsigned char> (0x63) << static_cast <unsigned char> (0x69) <<
                  static_cast <unsigned char> (0x61) << static_cast <unsigned char> (0x32) << static_cast <unsigned char> (0x00);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // Control change n.7 (volume)
    outputFile << static_cast <unsigned char> (0xB1) << static_cast <unsigned char> (0x07) << static_cast <unsigned char> (0x5A);  //40=-6db ; 7f=+6db
/*
    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // Control change n.91 (riverbero)
    outputFile << static_cast <unsigned char> (0xB1) << static_cast <unsigned char> (0x5B) << static_cast <unsigned char> (0x7F);
*/
    //delay time
    //outputFile << static_cast <unsigned char> (0x81)<< static_cast <unsigned char> (0xF0)<< static_cast <unsigned char> (0x00);
    outputFile << static_cast <unsigned char> (0xDA) << static_cast <unsigned char> (0x00);


    //ARPEGGIO ASCENDENTE CON MINIME

    for (int i=0; i<numeroNote; i++){

        //note on
        outputFile << static_cast <unsigned char> (0x91);

        //nota
        outputFile << (char)nota[i];

        //velocità
        outputFile << static_cast <unsigned char> (0x5A);

        //delay time + note off
        outputFile << static_cast <unsigned char> (0x8F) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x81);

        //nota
        outputFile << (char)nota[i];

        //velocità
        outputFile << static_cast <unsigned char> (0x00);

        //delay time
        if(i!=numeroNote-1){
            outputFile << static_cast <unsigned char> (0x00);
        } else {
            //bho
            //outputFile << static_cast <unsigned char> (0x84) << static_cast <unsigned char> (0xF5) << static_cast <unsigned char> (0x7F);
            outputFile << static_cast <unsigned char> (0x86) << static_cast <unsigned char> (0x8B) << static_cast <unsigned char> (0x7F);
        }
    }

    //FINE TRACCIA
    //all note off
    outputFile << static_cast <unsigned char> (0xB1) << static_cast <unsigned char> (0x7F) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00);
    //chiusura della traccia
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x2F) << static_cast <unsigned char> (0x00);


    //TRACCIA3
    // MTtr: Midi Track chunk 4D 54 72 6B
    outputFile << static_cast <unsigned char> (0x4D) << static_cast <unsigned char> (0x54) << static_cast <unsigned char> (0x72) << static_cast <unsigned char> (0x6B);

    //byte rimanenti fino alla fine della traccia
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x93);


    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // Bank select B0 = 00 (canale 0)
    outputFile << static_cast <unsigned char> (0xB2) << static_cast <unsigned char> (0x00);

    //MSB for Bank select
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00);

    //LSB for Bank select
    outputFile << static_cast <unsigned char> (0x20) << static_cast <unsigned char> (0x00);

    // delay
    outputFile << static_cast <unsigned char> (0x00);

    //program (patch) change -> determina quale strumento deve suonare
    outputFile << static_cast <unsigned char> (0xC2) << static_cast <unsigned char> (0x00);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // nome della traccia = traccia3
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x03) << static_cast <unsigned char> (0x09) <<
                  static_cast <unsigned char> (0x74) << static_cast <unsigned char> (0x72) << static_cast <unsigned char> (0x61) <<
                  static_cast <unsigned char> (0x63) << static_cast <unsigned char> (0x63) << static_cast <unsigned char> (0x69) <<
                  static_cast <unsigned char> (0x61) << static_cast <unsigned char> (0x33) << static_cast <unsigned char> (0x00);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // Control change n.7 (volume)
    outputFile << static_cast <unsigned char> (0xB2) << static_cast <unsigned char> (0x07) << static_cast <unsigned char> (0x5A);  //40=-6db ; 7f=+6db
/*
    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // Control change n.91 (riverbero)
    outputFile << static_cast <unsigned char> (0xB2) << static_cast <unsigned char> (0x5B) << static_cast <unsigned char> (0x7F);
*/
    //delay time
    //outputFile << static_cast <unsigned char> (0x83)<< static_cast <unsigned char> (0x86)<< static_cast <unsigned char> (0x00);
    outputFile << static_cast <unsigned char> (0x81)<< static_cast <unsigned char> (0xB4)<< static_cast <unsigned char> (0x00);

    //ARPEGGIO ASCENDENTE E DISCENDENTE CON SEMIMINIME

    //ASCENDENTE
    for (int i=0; i<numeroNote; i++){

        //note on
        outputFile << static_cast <unsigned char> (0x92);

        //nota
        outputFile << (char)nota[i];

        //velocità
        outputFile << static_cast <unsigned char> (0x5A);

        //delay time + note off
        outputFile << static_cast <unsigned char> (0x87) << static_cast <unsigned char> (0x40) << static_cast <unsigned char> (0x82);

        //nota
        outputFile << (char)nota[i];

        //velocità
        outputFile << static_cast <unsigned char> (0x00);

        //delay time
        outputFile << static_cast <unsigned char> (0x00);

    }

    //DISCENDENTE
    for (int i=numeroNote-1; i>-1; i--){

        //note on
        outputFile << static_cast <unsigned char> (0x92);

        //nota
        outputFile << (char)nota[i];

        //velocità
        outputFile << static_cast <unsigned char> (0x5A);

        //delay time + note off
        outputFile << static_cast <unsigned char> (0x87) << static_cast <unsigned char> (0x40) << static_cast <unsigned char> (0x82);

        //nota
        outputFile << (char)nota[i];

        //velocità
        outputFile << static_cast <unsigned char> (0x00);

        if (i==0){
            //delay time prima della melodia
            //outputFile << static_cast <unsigned char> (0x83) << static_cast <unsigned char> (0xDF) << static_cast <unsigned char> (0x7F);
            outputFile << static_cast <unsigned char> (0x85) << static_cast <unsigned char> (0xB1) << static_cast <unsigned char> (0x7F);
        }else{
            //delay time
            outputFile << static_cast <unsigned char> (0x00);
        }
    }

    //FINE TRACCIA
    //all note off
    outputFile << static_cast <unsigned char> (0xB2) << static_cast <unsigned char> (0x7F) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00);
    //chiusura della traccia
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x2F) << static_cast <unsigned char> (0x00);


    //TRACCIA4
    // MTtr: Midi Track chunk 4D 54 72 6B
    outputFile << static_cast <unsigned char> (0x4D) << static_cast <unsigned char> (0x54) << static_cast <unsigned char> (0x72) << static_cast <unsigned char> (0x6B);

    //byte rimanenti fino alla fine della traccia
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0xEE);


    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // Bank select B0 = 00 (canale 0)
    outputFile << static_cast <unsigned char> (0xB3) << static_cast <unsigned char> (0x00);

    //MSB for Bank select
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00);

    //LSB for Bank select
    outputFile << static_cast <unsigned char> (0x20) << static_cast <unsigned char> (0x00);

    // delay
    outputFile << static_cast <unsigned char> (0x00);

    //program (patch) change -> determina quale strumento deve suonare
    outputFile << static_cast <unsigned char> (0xC3) << static_cast <unsigned char> (0x00);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // nome della traccia = traccia4
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x03) << static_cast <unsigned char> (0x09) <<
                  static_cast <unsigned char> (0x74) << static_cast <unsigned char> (0x72) << static_cast <unsigned char> (0x61) <<
                  static_cast <unsigned char> (0x63) << static_cast <unsigned char> (0x63) << static_cast <unsigned char> (0x69) <<
                  static_cast <unsigned char> (0x61) << static_cast <unsigned char> (0x34) << static_cast <unsigned char> (0x00);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // Control change n.7 (volume)
    outputFile << static_cast <unsigned char> (0xB3) << static_cast <unsigned char> (0x07) << static_cast <unsigned char> (0x5A);  //40=-6db ; 7f=+6db
/*
    //delay time
    outputFile << static_cast <unsigned char> (0x00);

    // Control change n.91 (riverbero)
    outputFile << static_cast <unsigned char> (0xB3) << static_cast <unsigned char> (0x5B) << static_cast <unsigned char> (0x7F);
*/
    //delay time
    //outputFile << static_cast <unsigned char> (0x84)<< static_cast <unsigned char> (0x9C)<< static_cast <unsigned char> (0x00);
    outputFile << static_cast <unsigned char> (0x82)<< static_cast <unsigned char> (0xAC)<< static_cast <unsigned char> (0x00);


    //MELODIA CON SEMIMINIME RIPORTATE SULL'OTTAVA CENTRALE
    //fase ascendente
    for (int i=0; i<numeroNote; i++){

        //note on
        outputFile << static_cast <unsigned char> (0x93);

        //nota
        outputFile << (char)notaCorrispondenteOttavaCentrale(nota[i]);

        //velocità
        outputFile << static_cast <unsigned char> (0x5A);

        //delay time + note off
        outputFile << static_cast <unsigned char> (0x8F) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x83);

        //nota
        outputFile << (char)notaCorrispondenteOttavaCentrale(nota[i]);

        //velocità
        outputFile << static_cast <unsigned char> (0x00);

        //delay time
        outputFile << static_cast <unsigned char> (0x00);

    }

    //fase discendente, si parte da numeroNote-2 perchè l'ultima nota non va risuonata
    //esempio di melodia: ABCDEDCBA
    for (int i=numeroNote-2; i>-1; i--){

        //note on
        outputFile << static_cast <unsigned char> (0x93);

        //nota
        outputFile << (char)notaCorrispondenteOttavaCentrale(nota[i]);

        //velocità
        outputFile << static_cast <unsigned char> (0x5A);

        //delay time + note off
        outputFile << static_cast <unsigned char> (0x8F) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x83);

        //nota
        outputFile << (char)notaCorrispondenteOttavaCentrale(nota[i]);

        //velocità
        outputFile << static_cast <unsigned char> (0x00);

        //delay time
        if (i==0){
            //delay time prima della melodia
            //outputFile << static_cast <unsigned char> (0xCB) << static_cast <unsigned char> (0x00);
            outputFile << static_cast <unsigned char> (0xAD) << static_cast <unsigned char> (0x00);
        }else{
            //delay time
            outputFile << static_cast <unsigned char> (0x00);
        }
    }


    //MELODIA  (sincrona all'accordo)

    //melodia
    //fase ascendente
    for (int i=0; i<numeroNote; i++){

        outputFile << static_cast <unsigned char> (0x93);

        //nota
        outputFile << (char)notaCorrispondenteOttavaCentrale(nota[i]);

        //velocità
        outputFile << static_cast <unsigned char> (0x5A);

        //delay time + note off
        outputFile << static_cast <unsigned char> (0x8F) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x83);

        //nota
        outputFile << (char)notaCorrispondenteOttavaCentrale(nota[i]);

        //velocità
        outputFile << static_cast <unsigned char> (0x00);

        //delay time
        outputFile << static_cast <unsigned char> (0x00);

    }

    //fase discendente, si parte da numeroNote-2 perchè l'ultima nota non va risuonata
    //esempio di melodia: ABCDEDCBA
    for (int i=numeroNote-2; i>-1; i--){

        //note on
        outputFile << static_cast <unsigned char> (0x93);

        //nota
        outputFile << (char)notaCorrispondenteOttavaCentrale(nota[i]);

        //velocità
        outputFile << static_cast <unsigned char> (0x5A);

        //delay time + note off
        outputFile << static_cast <unsigned char> (0x8F) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x83);

        //nota
        outputFile << (char)notaCorrispondenteOttavaCentrale(nota[i]);

        //velocità
        outputFile << static_cast <unsigned char> (0x00);

        //delay time
        if (i==0){
            //delay time prima della chiusura?!
            //outputFile << static_cast <unsigned char> (0x8E) << static_cast <unsigned char> (0x7F);
            outputFile << static_cast <unsigned char> (0x82) << static_cast <unsigned char> (0x9C) << static_cast <unsigned char> (0x7F);
        }else{
            //delay time
            outputFile << static_cast <unsigned char> (0x00);
        }
    }



    /* SERVE SOLO QUANDO SCRIVI UNA SOLA NOTA
    // delay o cosa???
    outputFile << static_cast <unsigned char> (0x82) << static_cast <unsigned char> (0xC9) << static_cast <unsigned char> (0x7F) <<
                  static_cast <unsigned char> (0xB0) << static_cast <unsigned char> (0x7F) << static_cast <unsigned char> (0x00);
    */


    //all note off
    outputFile << static_cast <unsigned char> (0xB3) << static_cast <unsigned char> (0x7F) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00);
    //chiusura della traccia
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x2F) << static_cast <unsigned char> (0x00);

}

//intervalli (in semitoni) della scala individuata da ciascun modo
void MainWindow::impostaIntervalli(int volto)
{
    if (volto==VOLTOOVALE){
        //Modo ionico
        vettoreIntervalli[0] = 0;
        vettoreIntervalli[1] = 2;
        vettoreIntervalli[2] = 4;
        vettoreIntervalli[3] = 5;
        vettoreIntervalli[4] = 7;
        vettoreIntervalli[5] = 9;
        vettoreIntervalli[6] = 11;
    } else if (volto==VOLTOROTONDO){
        //modo dorico
        vettoreIntervalli[0] = 0;
        vettoreIntervalli[1] = 2;
        vettoreIntervalli[2] = 3;
        vettoreIntervalli[3] = 5;
        vettoreIntervalli[4] = 7;
        vettoreIntervalli[5] = 9;
        vettoreIntervalli[6] = 10;
    } else if (volto==VOLTOQUADRATO){
        //modo frigio
        vettoreIntervalli[0] = 0;
        vettoreIntervalli[1] = 1;
        vettoreIntervalli[2] = 3;
        vettoreIntervalli[3] = 5;
        vettoreIntervalli[4] = 7;
        vettoreIntervalli[5] = 8;
        vettoreIntervalli[6] = 10;
    } else if (volto==VOLTODIAMANTE){
        //lidio
        vettoreIntervalli[0] = 0;
        vettoreIntervalli[1] = 2;
        vettoreIntervalli[2] = 4;
        vettoreIntervalli[3] = 6;
        vettoreIntervalli[4] = 7;
        vettoreIntervalli[5] = 9;
        vettoreIntervalli[6] = 11;
    } else if (volto==VOLTOCUORE){
        //misolidio
        vettoreIntervalli[0] = 0;
        vettoreIntervalli[1] = 2;
        vettoreIntervalli[2] = 4;
        vettoreIntervalli[3] = 5;
        vettoreIntervalli[4] = 7;
        vettoreIntervalli[5] = 9;
        vettoreIntervalli[6] = 10;
    } else if (volto==VOLTORETTANGOLARE){
        //eolio
        vettoreIntervalli[0] = 0;
        vettoreIntervalli[1] = 2;
        vettoreIntervalli[2] = 3;
        vettoreIntervalli[3] = 5;
        vettoreIntervalli[4] = 7;
        vettoreIntervalli[5] = 8;
        vettoreIntervalli[6] = 10;
    } else if (volto==VOLTOTRIANGOLARE){
        //locrio
        vettoreIntervalli[0] = 0;
        vettoreIntervalli[1] = 1;
        vettoreIntervalli[2] = 3;
        vettoreIntervalli[3] = 5;
        vettoreIntervalli[4] = 6;
        vettoreIntervalli[5] = 8;
        vettoreIntervalli[6] = 10;
    }

}

//intervalli (in semitoni) dalla tonica, sulla scala modale costruita su sol
void MainWindow::impostaIntervalliDaTonica()
{
    vettoreIntervalliDaTonica[0]=0;
    for (int i=1; i<7; i++){
        vettoreIntervalliDaTonica[i]=vettoreIntervalliDaTonica[i-1]+vettoreIntervalli[(tonica+i)%7]-vettoreIntervalli[(tonica+i-1)%7];
        if (vettoreIntervalliDaTonica[i]<0){
            vettoreIntervalliDaTonica[i]=vettoreIntervalliDaTonica[i-1] + 12-vettoreIntervalli[(tonica+i-1)%7];
        }
    }

}

int MainWindow::calcolaNumeroNotaMidi(int intervalloDiPartenza, int ottavaDiPartenza)
{
    int ottava = ottavaDiPartenza;

    // se il parametro vale 1 si suona la tonica, quindi la nota 0.
    int intervallo = intervalloDiPartenza - 1;

    // se l'intervallo è maggiore di una settima, salgo di ottava e sottraggo 8 all'intervallo
    while (intervallo>7){
        ottava++;
        intervallo = intervallo - 8;
    }

    //la nota si ottiene come la tonica + l'intervallo a partire dalla tonica (in semitoni) + l'ottava di riferimento per 12 (semitoni)
    int nota = vettoreNote[0] + vettoreIntervalliDaTonica[intervallo] + ottava*12;

    cerr << "nota: " << nota << endl;

    return nota;
}

int MainWindow::notaCorrispondenteOttavaCentrale(int nota)
{
    //fino a quando la nota è sotto l'ottava centrale, aggiungo 12 (semitoni) e sposto la nota di un ottava in alto
    while (nota<MIN_OTTAVA_CENTRALE){
        nota = nota + 12;
    }

    //fino a quando la nota è sopra l'ottava centrale, sottraggo 12 (semitoni) e sposto la nota di un ottava in basso
    while (nota > (MIN_OTTAVA_CENTRALE+11)){
        nota = nota - 12;
    }

    return nota;
}

bool MainWindow::trovaDuplicatiESpostaOttava(int *vettoreNote, int n)
{
    // ordino il vettore in ordine crescente
    int min, temp, i, j;
    bool noteUguali=false;
    for(i=0; i<n-1; i++)
    {
        min = i;
        for(j=i+1; j<n; j++)
            if(vettoreNote[j] < vettoreNote[min])
                min = j;
        temp=vettoreNote[min];
        vettoreNote[min]=vettoreNote[i];
        vettoreNote[i]=temp;
    }
    //se due note sono uguali, ne sposto una all'ottava superiore (+12 semitoni)
    for (i=0; i<n-1; i++){
        if (vettoreNote[i]==vettoreNote[i+1]){
            vettoreNote[i+1] = vettoreNote[i+1] + 12;
            noteUguali=true;
        }
    }
    return noteUguali;
}

bool MainWindow::notaOttavaCentrale(int nota)
{
    if (nota>=MIN_OTTAVA_CENTRALE && nota<=MIN_OTTAVA_CENTRALE+11){
        return true;
    } else{
        return false;
    }
}
/*
Vec3b MainWindow::mediaColore(Mat immagine, Point centro, int distanzaMaxAltezza, int distanzaMaxLarghezza, int hRiferimento)
{
    Vec3b colore;
    int h=0;
    int s=0;
    int v=0;
    int pixelConsiderati=0;
    //int temp;

    // se il valore di h di riferimento è alto, vuol dire che ci sono alcuni valori molto bassi (h è ciclico)
    if (hRiferimento>160){
        for (int i=-distanzaMaxAltezza; i<distanzaMaxAltezza; i++){
            for (int j=-distanzaMaxLarghezza; j<distanzaMaxLarghezza; j++){
                colore = immagine.at<Vec3b>(centro.y+i,centro.x+j);
                //se il valore è basso, sommo 180, altrimenti sballa la media
                if (colore.val[0]<20){
                    h = h + colore.val[0]+180;
                } else {
                    h = h + colore.val[0];
                }

                s = s + colore.val[1];
                v = v + colore.val[2];
                pixelConsiderati++;

                //temp = colore.val[0];
                //cerr << pixelConsiderati << " - h: " << h << " colore: " << temp << endl;
            }
        }
        //riporto il valore di h nell'intervallo 0-179
        h=(h/pixelConsiderati)%180;
        s=s/pixelConsiderati;
        v=v/pixelConsiderati;

        colore.val[0]=h;
        colore.val[1]=s;
        colore.val[2]=v;
    } else {
        for (int i=-distanzaMaxAltezza; i<distanzaMaxAltezza; i++){
            for (int j=-distanzaMaxLarghezza; j<distanzaMaxLarghezza; j++){
                colore = immagine.at<Vec3b>(centro.y+i,centro.x+j);
                h = h + colore.val[0];
                s = s + colore.val[1];
                v = v + colore.val[2];
                pixelConsiderati++;

                //temp = colore.val[0];
                //cerr << pixelConsiderati << " - h: " << h << " colore: " << temp << endl;
            }
        }
        h=h/pixelConsiderati;
        s=s/pixelConsiderati;
        v=v/pixelConsiderati;

        colore.val[0]=h;
        colore.val[1]=s;
        colore.val[2]=v;
    }



    //disegno un rettangolo intorno alla zona analizzata
    Point upperLeft (centro.x-distanzaMaxLarghezza, centro.y-distanzaMaxAltezza);
    Point lowerRight (centro.x+distanzaMaxLarghezza, centro.y+distanzaMaxAltezza);
    rectangle(immagine, upperLeft, lowerRight, Scalar (0,0,0));
    imwrite("media-colore.jpg", immagine);

    return colore;
}

Vec3b MainWindow::mediaColoreCompleta(Mat immagine, int hRiferimento)
{
    Vec3b colore;
    int h=0;
    int s=0;
    int v=0;
    int pixelConsiderati=0;
    //int temp;
    //Mat imgEroded = imread("eroded.bmp");    quando legge l'img fatta da 0 e 255 da file fa casino - adesso imgeroded sta nella classe mainwindow

    cerr << "imgEroded: " << imgEroded.size().width << " " << imgEroded.size().height << endl;
    cerr << "immagine: " << immagine.size().width << " " << immagine.size().height << endl;

    if (hRiferimento>160){
        for (int i=0; i<imgEroded.size().height; i++){
            for (int j=0; j<imgEroded.size().width; j++){
                if((int)imgEroded.at<uchar>(i,j)==255){
                    colore=immagine.at<Vec3b>(i,j);
                    if (colore.val[0]<20){
                        h = h + colore.val[0] + 180;
                    }else{
                        h = h + colore.val[0];
                    }

                    s = s + colore.val[1];
                    v = v + colore.val[2];
                    pixelConsiderati++;

                    //temp = colore.val[0];
                    //cerr << pixelConsiderati << " - h: " << h << " colore: " << temp << endl;
                }
            }
        }
    }else {
        for (int i=0; i<imgEroded.size().height; i++){
            for (int j=0; j<imgEroded.size().width; j++){
                if((int)imgEroded.at<uchar>(i,j)==255){
                    colore=immagine.at<Vec3b>(i,j);
                    h = h + colore.val[0];
                    s = s + colore.val[1];
                    v = v + colore.val[2];
                    pixelConsiderati++;

                    //temp = colore.val[0];
                    //cerr << pixelConsiderati << " - h: " << h << " colore: " << temp << endl;
                }
            }
        }
    }

    h=(h/pixelConsiderati)%180;
    s=s/pixelConsiderati;
    v=v/pixelConsiderati;

    colore.val[0]=h;
    colore.val[1]=s;
    colore.val[2]=v;
    return colore;
}


Vec3b MainWindow::segmentazioneColore(Mat hsvImage, Point pixelRiferimento)
{

    Mat puntoDiRiferimento = hsvImage(Rect(pixelRiferimento.x, pixelRiferimento.y,1,1));
    Vec3b coloreDiRiferimento = puntoDiRiferimento.at<Vec3b>(0,0);
    int H = coloreDiRiferimento.val[0]; // hue
    int S = coloreDiRiferimento.val[1]; // saturation
    int V = coloreDiRiferimento.val[2]; // value

    cerr << "HSV= " << H << " - " << S << " - " << V << endl;

 // effettua la sogliatura su tutta l'immagine
    Mat imgThresholded;

    // threshold
    inRange (hsvImage , Scalar (H-7,S-100,V-100), Scalar (H+7,S+100,V+100), imgThresholded);

    imwrite("thresholded.jpg",imgThresholded);

    // eseguo l'erosione dell'immagine ottenuta con la sogliatura per rimouovere outliers
    Mat kernel = getStructuringElement(MORPH_RECT, Size (3,3), Point (1,1));

    erode (imgThresholded, imgEroded, kernel);

/*    for (int i=0; i<imgEroded.size().height; i++)
        for (int j=0; j<imgEroded.size().width; j++)
            cerr << (int)imgEroded.at<uchar>(i,j) << " ";
* /
    imwrite("eroded.jpg", imgEroded);
    imwrite("eroded.bmp", imgEroded);

   // circle(hsvImage,pixelRiferimento, 3, Scalar (0,0,0));
   // imwrite("cerchio.jpg",hsvImage);

    return coloreDiRiferimento;

}
*/

void MainWindow::on_actionPreferenze_triggered()
{
    //mostra la finestra delle preferenze
    DialogSettings *settingsWindow = new DialogSettings();
    settingsWindow->setWindowTitle("Preferenze");
    if(settingsWindow->exec()){
        QSettings settings("sisinflab", "MusicaHumana");

        //imposto il valore selezionato nella finestra delle preferenze
        settings.setValue("midiPath",settingsWindow->getMidiPath());
        settings.setValue("nonChiedereMidiPath", settingsWindow->getCheckBoxUsaDefault());
        /*
        settings.setValue("fronte",settingsWindow->getOttavaFronte());
        settings.setValue("naso",settingsWindow->getOttavaNaso());
        settings.setValue("boccaNaso",settingsWindow->getOttavaBoccaNaso());
        settings.setValue("occhi",settingsWindow->getOttavaOcchi());
        settings.setValue("bocca",settingsWindow->getOttavaBocca());
*/
        //aggiorno il file delle preferenze
        settings.sync();

    }
}
