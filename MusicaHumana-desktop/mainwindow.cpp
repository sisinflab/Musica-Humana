#include "mainwindow.h"
#include "ui_mainwindow.h"


using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    face_cascade_name =  String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/cascades/haarcascade_frontalface_alt.xml");

    eyes_cascade_name = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/cascades/haarcascade_eye_tree_eyeglasses.xml");

    nose_cascade_name = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/cascades/haarcascade_mcs_nose.xml");

    mouth_cascade_name = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/cascades/haarcascade_mcs_mouth.xml");

    // Load the cascades
    if( !face_cascade.load( face_cascade_name ) ){
        QMessageBox::warning(this,"Errore face cascade", "face_cascade non caricato!");
    }
    if( !eyes_cascade.load( eyes_cascade_name ) ){
        QMessageBox::warning(this,"Errore eyes cascade", "eyes_cascade non caricato!");
    }
    if( !nose_cascade.load( nose_cascade_name ) ){
        QMessageBox::warning(this,"Errore nose cascade", "nose_cascade non caricato!");
    }
    if( !mouth_cascade.load( mouth_cascade_name ) ){
        QMessageBox::warning(this,"Errore mouth cascade", "mouth_cascade non caricato!");
    }


    ui->setupUi(this);
    this->setWindowTitle("Musica Humana");

    //istanzio il contenitore per la visualizzazione dell'immagine
    imgWidget = new CVImageWidget (ui->widgetImg);


    // Load an image
    image = cv::imread(String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/materelettrica.jpg"), true);




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

            if (!image.data){
                std::cerr << "errore acquisizione immagine" <<endl;
                return;
            }


            imshow("Acquisizione immagine", image);

            // appena premi un tasto salva l'immagine
            if(waitKey(30) >= 0){

                std::cerr << "DEBUG scatta - A" <<endl;


                // scrivo l'immagine acquisita su un file jpg
                String imgAcquisitaPath = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/immagine acquisita.jpg");
                const char *pathImgAcquisitaChar = imgAcquisitaPath.c_str();
                std::cerr << "DEBUG scatta - B" <<endl;
                imwrite(pathImgAcquisitaChar, image);

                std::cerr << "DEBUG scatta - C" <<endl;

                // visualizzo l'immagine nel widget
                visualizzaImmagine(image,imgWidget);
                std::cerr << "DEBUG scatta - D" <<endl;

                //distruggo la finestra aperta per acquisire l'immagine e libero la webcam
                destroyAllWindows();
                //cap.release(); // 20 nov 2019 provo a commentare
                std::cerr << "DEBUG scatta - E" <<endl;

                //cancello le eventuali scritte sull'interfaccia
                ui->labelDistanze->setText("");
                std::cerr << "DEBUG scatta - F" <<endl;

                //esco dal ciclo for(;;)
                break;
            }
        } std::cerr << "DEBUG scatta - G" <<endl;
    } std::cerr << "DEBUG scatta - H" <<endl;
}

void MainWindow::visualizzaImmagine(Mat immagine, CVImageWidget *imgWidget){

    std::cerr << "DEBUG visualizza - A" <<endl;

    if (immagine.data==nullptr){
        std::cerr << "DEBUG visualizza - B" <<endl;
        return;
    }

    std::cerr << "DEBUG visualizza - C" <<endl;

    //Adatta l'immagine prima di visualizzarla

    // dimensione del contenitore
    float larghezza = ui->widgetImg->width();
    float altezza = ui->widgetImg->height();

    std::cerr << "DEBUG visualizza - D" <<endl;

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
    std::cerr << "E" <<endl;

    int larghezzaInt = larghezza;
    int altezzaInt = altezza;
    std::cerr << "DEBUG visualizza - E-BIS" <<endl;
    std::cerr << larghezzaInt <<endl;
    std::cerr << altezzaInt <<endl;
    std::cerr << "DEBUG visualizza - E-BIS" <<endl;

    // adatto l'immagine
    cv::resize(immagine, immagineAdattata, Size(larghezzaInt,altezzaInt));
    std::cerr << "DEBUG visualizza - F" <<endl;
    std::cerr << "DEBUG visualizza - G0" <<endl;
    // aggiorno il widget per la visualizzazione
    imgWidget->showImage(immagineAdattata);
}

void MainWindow::on_pushButtonCalcolaDistanze_clicked()
{

    //cerr << endl << "in mainwindow: " << vettoreIntervalli[3] << endl;

    //contiene la stringa mostrata nell'interfaccia, viene costruita durante le operazioni di detect
    QString caratteristiche;

    //utilizzato dai detector
    Mat frame_gray;
    //copia utilizzata per disegnare i rettangoli intorno alle aree di interesse
    frame = image.clone();

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

        //Non serve perchè nell'immagine cerco il volto più grande
        //caratteristiche = "Facce trovate: " + QString::number( faces.size() ) + "\n";

    } else {
        caratteristiche = "Nessun volto individuato.";
        //scrivo sull'interfaccia grafica
        ui->labelDistanze->setText(caratteristiche);

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

    if (model==nullptr){
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


            //ANGOLI

            //angolo fra retta occhi e retta occhio-bocca
            float angoloOcchiBocca = calcolaAngolo(occhioSS,occhioDD,occhioDD, boccaD);

            //angolo in gradi
            angoloOcchiBocca = angoloOcchiBocca*180/3.1415;

            caratteristiche += "Angolo occhi/bocca: " + QString::number(angoloOcchiBocca) +"\n";

            //angolo fra retta occhi e retta occhio-naso
            float angoloOcchiNaso = calcolaAngolo(occhioSS,occhioDD,occhioDD, naso);

            //angolo in gradi
            angoloOcchiNaso = angoloOcchiNaso*180/3.1415;
            caratteristiche += "Angolo occhi/naso: " + QString::number(angoloOcchiNaso) +"\n";

            //angolo fra linea bocca e naso (lato destro)
            float angoloBoccaNaso = calcolaAngolo(naso, boccaD, boccaD, boccaS);
            //in gradi
            angoloBoccaNaso = angoloBoccaNaso*180/3.1415;
            caratteristiche += "Angolo fra bocca e naso: " + QString::number(angoloBoccaNaso) + "\n";

            //angolo fra linea bocca e naso (lato sinistro)
            float angoloBoccaNaso2 = calcolaAngolo(naso, boccaS, boccaS, boccaD);
            //in gradi
            angoloBoccaNaso2 = angoloBoccaNaso2*180/3.1415;
            caratteristiche += "Angolo fra bocca e naso: " + QString::number(angoloBoccaNaso2) + "\n";

            //angolo fra linea bocca e naso (al naso)
            float angoloNasoBocca = 180-angoloBoccaNaso-angoloBoccaNaso2;
            caratteristiche += "Angolo naso: " + QString::number(angoloNasoBocca) + "\n";

            //disegno le linee che formano gli angoli
            line(frame,occhioDD, occhioSS, Scalar(255,0,0),2);
            line(frame,occhioDD, naso, Scalar(0,255,0),2);
            line(frame,occhioDD, boccaD, Scalar(0,0,255),2);
            line(frame, naso, boccaD, Scalar(255,125,0),2);
            line(frame, boccaD, boccaS, Scalar(255,125,0),2);
            line(frame, naso, boccaS, Scalar(255,125,0),2);


            Mat output (img);

            String pathOutput = String(QCoreApplication::applicationDirPath().toLocal8Bit().data()) + String("/output.jpg");
            const char *pathOutputChar = pathOutput.c_str();
            imwrite(pathOutputChar, output);

            visualizzaImmagine(output,imgWidget);

            //calcolo le note e scrivo il file midi

            int ottavaFronte = OTTAVA_FRONTE;
            int ottavaNaso = OTTAVA_NASO;
            int ottavaBoccaNaso = OTTAVA_BOCCA_NASO;
            int ottavaOcchi = OTTAVA_OCCHI;
            int ottavaBocca = OTTAVA_BOCCA;


            calcolaFondamentale(angoloNasoBocca);
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

float MainWindow::calcolaAngolo(Point a, Point b, Point c, Point d)
{
    float angolo;
    float m1, m2;
    //se le coordinate x sono uguali -> m assume valore infinito, assegno un valore alto
    if (b.x==a.x){
        m1=1000;
    } else {
        m1=((float)b.y-(float)a.y)/((float)b.x-(float)a.x);
    }

    if (d.x==c.x){
        m2=1000;
    } else {
        m2=((float)d.y-(float)c.y)/((float)d.x-(float)c.x);
    }

    cerr << "punti: " << endl << a.x << " - " << a.y << "\n" << b.x << " - "<< b.y << "\n" << c.x << " - "<< c.y << "\n" << d.x << " - " <<d.y << "\n";

    cerr << "m1: " << m1 << endl << "m2: " << m2 << endl;

    float argomentoTangente =(m1-m2)/(1+ (m1*m2) );
    angolo = atan( argomentoTangente );

    cerr <<"argomento tangente: " << argomentoTangente << endl;
    cerr << "angolo: " << angolo << endl;

    if (angolo<0){
        angolo = angolo * (-1);
    }

    return angolo;
}

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
    if (settings.value("midiPath").toString() != nullptr && settings.value("nonChiedereMidiPath").toBool()==true){
        //uso il default
        pathMidiFile += settings.value("midiPath").toString();
    } else if (settings.value("midiPath").toString() != nullptr)
    {
        //chiedo se vuole usare il default
        DialogUsaDefaultPath *defaultWindow = new DialogUsaDefaultPath();
        defaultWindow->setWindowTitle("Usa default");
        // se vuol usare il default
        if (defaultWindow->exec()){
            //se ha selezionato "non chiedere piu" lo scrivo sul file di configurazione e uso il default
            cerr <<defaultWindow->isCheckedNonChiederePiu();
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
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0xA3);


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

    /*    //program (patch) change -> determina quale strumento deve suonare
    outputFile << static_cast <unsigned char> (0xC0) << static_cast <unsigned char> (0x00);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);
*/
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
    for (int i=0; i<4; i++){

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
    outputFile << static_cast <unsigned char> (0x81) << static_cast <unsigned char> (0xB4) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x80);


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
            outputFile << static_cast <unsigned char> (0x84) << static_cast <unsigned char> (0xD8) << static_cast <unsigned char> (0x00);
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
            outputFile << static_cast <unsigned char> (0x8E) << static_cast <unsigned char> (0x7F);
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
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x5A);


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

    /*    //program (patch) change -> determina quale strumento deve suonare
    outputFile << static_cast <unsigned char> (0xC1) << static_cast <unsigned char> (0x00);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);
*/
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
    outputFile << static_cast <unsigned char> (0x81)<< static_cast <unsigned char> (0xF0)<< static_cast <unsigned char> (0x00);


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
            outputFile << static_cast <unsigned char> (0x84) << static_cast <unsigned char> (0xF5) << static_cast <unsigned char> (0x7F);
        }
    }

    //FINE TRACCIA
    //all note off
    outputFile << static_cast <unsigned char> (0xB0) << static_cast <unsigned char> (0x7F) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00);
    //chiusura della traccia
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x2F) << static_cast <unsigned char> (0x00);


    //TRACCIA3
    // MTtr: Midi Track chunk 4D 54 72 6B
    outputFile << static_cast <unsigned char> (0x4D) << static_cast <unsigned char> (0x54) << static_cast <unsigned char> (0x72) << static_cast <unsigned char> (0x6B);

    //byte rimanenti fino alla fine della traccia
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x90);


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

    /*    //program (patch) change -> determina quale strumento deve suonare
    outputFile << static_cast <unsigned char> (0xC2) << static_cast <unsigned char> (0x00);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);
*/
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
    outputFile << static_cast <unsigned char> (0x83)<< static_cast <unsigned char> (0x86)<< static_cast <unsigned char> (0x00);

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
            outputFile << static_cast <unsigned char> (0x83) << static_cast <unsigned char> (0xDF) << static_cast <unsigned char> (0x7F);
        }else{
            //delay time
            outputFile << static_cast <unsigned char> (0x00);
        }
    }

    //FINE TRACCIA
    //all note off
    outputFile << static_cast <unsigned char> (0xB0) << static_cast <unsigned char> (0x7F) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00);
    //chiusura della traccia
    outputFile << static_cast <unsigned char> (0xFF) << static_cast <unsigned char> (0x2F) << static_cast <unsigned char> (0x00);


    //TRACCIA4
    // MTtr: Midi Track chunk 4D 54 72 6B
    outputFile << static_cast <unsigned char> (0x4D) << static_cast <unsigned char> (0x54) << static_cast <unsigned char> (0x72) << static_cast <unsigned char> (0x6B);

    //byte rimanenti fino alla fine della traccia
    outputFile << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0xEA);


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

    /*    //program (patch) change -> determina quale strumento deve suonare
    outputFile << static_cast <unsigned char> (0xC3) << static_cast <unsigned char> (0x00);

    //delay time
    outputFile << static_cast <unsigned char> (0x00);
*/
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
    outputFile << static_cast <unsigned char> (0x84)<< static_cast <unsigned char> (0x9C)<< static_cast <unsigned char> (0x00);


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
            outputFile << static_cast <unsigned char> (0xCB) << static_cast <unsigned char> (0x00);
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
            outputFile << static_cast <unsigned char> (0x8E) << static_cast <unsigned char> (0x7F);
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
    outputFile << static_cast <unsigned char> (0xB0) << static_cast <unsigned char> (0x7F) << static_cast <unsigned char> (0x00) << static_cast <unsigned char> (0x00);
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
        /*cerr << "vettoreIntervalliDaTonica[i-1] " << vettoreIntervalliDaTonica[i-1] << endl;
        cerr << "(tonica+i-1) " << (tonica+i-1) << endl;
        cerr << "(tonica+i-1)%7 " << (tonica+i-1)%7 << endl;
        cerr << "vettoreIntervalli[(tonica+i-1)%7] " << vettoreIntervalli[(tonica+i-1)%7] << endl;
        cerr << "vettoreIntervalli da tonica " << vettoreIntervalliDaTonica[i] << endl;
        */
        /*
        cerr << endl << "porco dio: " << "i=" <<i << endl<< "vettoreIntervalliDaTonica[" <<i<<"] = vettoreIntervalliDaTonica[" <<i-1<<"]+vettoreIntervalli["<<(tonica+i)%7<<"]-vettoreIntervalli["<<(tonica+i-1)%7<<"];";
        cerr << endl << "vettoreIntervalliDaTonica[i]=" << vettoreIntervalliDaTonica[i-1] << "+" << vettoreIntervalli[(tonica+i)%7] << "-" <<vettoreIntervalli[(tonica+i-1)%7]<<";";
        cerr << endl << "intervalloDaTonica = " << vettoreIntervalliDaTonica[i];
        */
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

void MainWindow::calcolaFondamentale(float angolo)
{
    if(angolo < 60){
        tonica=0; //sol
    } else if(angolo < 67){
        tonica=1; //la
    } else if(angolo < 73.5){
        tonica=2; //si
    } else if(angolo < 79){
        tonica=3; //do
    } else if(angolo < 83.4){
        tonica=4; //re
    } else if(angolo < 86.7){
        tonica=5; //mi
    } else if(angolo < 88.9){
        tonica=6; //fa
    } else if(angolo < 91.1){
        tonica=0;
    } else if(angolo < 93.3){
        tonica=1;
    } else if(angolo < 96.6){
        tonica=2;
    } else if(angolo < 101){
        tonica=3;
    } else if(angolo < 106.5){
        tonica=4;
    } else if(angolo < 113.1){
        tonica=5;
    } else if(angolo < 120){
        tonica=6;
    } else {
        tonica=0;
    }
}

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


        //aggiorno il file delle preferenze
        settings.sync();

    }
}

void MainWindow::on_actionSalva_immagine_triggered()
{
    //verifica se è presente un'immagine
    if (image.data==nullptr && frame.data==nullptr){
        QMessageBox::warning(this, "Errore", "Immagine assente!");
    } else if (frame.data==nullptr){
        //salva l'immagine originale
        //l'utente sceglie percorso e nome file
        QString percorsoENome = QFileDialog::getSaveFileName(this);

        //se l'utente ha selezionato OK vado avanti
        if (percorsoENome!=nullptr){
            //se il file non finisce con un estensione di immagine aggiungo l'estensione
            if( ! percorsoENome.endsWith(".jpg") && ! percorsoENome.endsWith(".jpeg") && ! percorsoENome.endsWith(".bpm") && ! percorsoENome.endsWith(".png")){
                percorsoENome += ".jpg";
            }

            //scrivo l'immagine su file
            imwrite(percorsoENome.toStdString(),image);
        }
    }else {
        //chiede se salvare l'immagine con i disegnini o quella pulita
        QMessageBox msgBox;
        msgBox.setText(trUtf8("Salva"));
        msgBox.setInformativeText("Salva immagine: ");
        QAbstractButton *buttonOriginale = msgBox.addButton(trUtf8("originale"), QMessageBox::YesRole);
        QAbstractButton *buttonCompleta = msgBox.addButton(trUtf8("con linee guida"), QMessageBox::NoRole);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.exec();

        if(msgBox.clickedButton() == buttonOriginale)
        {
            //salva l'immagine originale
            //l'utente sceglie percorso e nome file
            QString percorsoENome = QFileDialog::getSaveFileName(this);

            //se l'utente ha selezionato OK vado avanti
            if (percorsoENome!=nullptr){
                //se il file non finisce con un estensione di immagine aggiungo l'estensione
                if( ! percorsoENome.endsWith(".jpg") && ! percorsoENome.endsWith(".jpeg") && ! percorsoENome.endsWith(".bpm") && ! percorsoENome.endsWith(".png")){
                    percorsoENome += ".jpg";
                }

                //scrivo l'immagine su file
                imwrite(percorsoENome.toStdString(),image);
            }
        }

        if(msgBox.clickedButton() == buttonCompleta){
            //salva l'immagine completa
            //l'utente sceglie percorso e nome file
            QString percorsoENome = QFileDialog::getSaveFileName(this);

            //se l'utente ha selezionato OK vado avanti
            if (percorsoENome!=nullptr){
                //se il file non finisce con un estensione di immagine aggiungo l'estensione
                if( ! percorsoENome.endsWith(".jpg") && ! percorsoENome.endsWith(".jpeg") && ! percorsoENome.endsWith(".bmp") && ! percorsoENome.endsWith(".png")){
                    percorsoENome += ".jpg";
                }

                //scrivo l'immagine su file
                imwrite(percorsoENome.toStdString(),frame);
            }
        }

    }
}
