package it.poliba.musicahumana;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Environment;
import android.os.Handler;
import android.provider.MediaStore;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.Toast;

import com.example.giuseppe.musicahumana.R;

import org.opencv.android.Utils;
import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import org.opencv.objdetect.CascadeClassifier;
import org.opencv.objdetect.Objdetect;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.Date;


public class MainActivity extends ActionBarActivity {

    public static final int JAVA_DETECTOR = 0;

    private static final int PHOTO_REQUEST_CODE = 111;
    private static final int RESULT_LOAD_IMAGE = 112;
    private static final int FACE_SHAPE_REQUEST = 113;

    public static final int VOLTOOVALE = 0;
    public static final int VOLTOROTONDO = 1;
    public static final int VOLTOQUADRATO = 2;
    public static final int VOLTODIAMANTE = 3;
    public static final int VOLTOCUORE = 4;
    public static final int VOLTORETTANGOLARE = 5;
    public static final int VOLTOTRIANGOLARE = 6;

    private static final int MIN_OTTAVA_CENTRALE = 60;

    private static final int OTTAVA_FRONTE = 0;
    private static final int OTTAVA_NASO = 1;
    private static final int OTTAVA_BOCCA_NASO = 2;
    private static final int OTTAVA_OCCHI = 3;
    private static final int OTTAVA_BOCCA = 4;

    private static final int SOL = 7;
    private static final String NOME_FILE_MIDI = "file-midi.mid";


    private File mCascadeFile;
    private CascadeClassifier face_cascade;
    private CascadeClassifier eyes_cascade;
    private CascadeClassifier nose_cascade;
    private CascadeClassifier mouth_cascade;

    ImageView imgView;
    Bitmap immagine;
    String mCurrentPhotoPath;

    Mat matImg;
    Mat matGray;

    boolean immagineIsScattata;

    static int formaVolto;

    static boolean formaVoltoIsSelected = false;

    //intervalli della scala modale a partire da sol
    int[] vettoreIntervalli = new int[7];

    //intervalli della scala modale di sol, a partire dalla tonica
    int[] vettoreIntervalliDaTonica = new int[7];

    //vettoreNote[0] = TONICA (il numero rappresenta il corrispondente numero in midi)
    int[] vettoreNote = new int[6];

    //0=sol, 1=la, 2=si ecc...
    int tonica;

    MediaPlayer mediaPlayer;

    SeekBar mSeekBar;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        imgView = (ImageView) findViewById(R.id.imageView);

        ImageButton buttonPlay = (ImageButton) findViewById(R.id.buttonPlay);
        ImageButton buttonPause = (ImageButton) findViewById(R.id.buttonPause);
        ImageButton buttonStop = (ImageButton) findViewById(R.id.buttonStop);

        buttonPlay.setEnabled(false);
        buttonStop.setEnabled(false);
        buttonPause.setEnabled(false);

        mSeekBar = (SeekBar) findViewById(R.id.seekBar);
        mSeekBar.setEnabled(false);

        buttonPlay.setOnClickListener(buttonPlayOnClickListener);
        buttonPause.setOnClickListener(buttonPauseOnClickListener);
        buttonStop.setOnClickListener(buttonStopOnClickListener);

        final Handler mHandler = new Handler();
        //Make sure you update Seekbar on UI thread
        MainActivity.this.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                if (mediaPlayer != null) {
                    int mCurrentPosition = mediaPlayer.getCurrentPosition() / 1000;
                    mSeekBar.setProgress(mCurrentPosition);
                }
                mHandler.postDelayed(this, 1000);
            }
        });

        mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (mediaPlayer != null && fromUser) {
                    mediaPlayer.seekTo(progress * 1000);
                }
            }
        });


    }

    Button.OnClickListener buttonPlayOnClickListener
            = new Button.OnClickListener() {
        @Override
        public void onClick(View v) {
            if (mediaPlayer != null) {
                if (!mediaPlayer.isPlaying()) {
                    mediaPlayer.start();
                }
            }

        }
    };

    Button.OnClickListener buttonPauseOnClickListener
            = new Button.OnClickListener() {
        @Override
        public void onClick(View v) {
            if (mediaPlayer != null) {
                if (mediaPlayer.isPlaying()) {
                    mediaPlayer.pause();
                }
            }

        }
    };

    Button.OnClickListener buttonStopOnClickListener
            = new Button.OnClickListener() {
        @Override
        public void onClick(View v) {
            if (mediaPlayer != null) {
                if (mediaPlayer.isPlaying()) {

                    mediaPlayer.pause();
                    mediaPlayer.seekTo(0);

                } else {
                    mediaPlayer.seekTo(0);
                }
            }

        }
    };


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    /**
     * funzione per creare un file in cui salvare la foto scattata
     *
     * @return File immagine
     * @throws IOException
     */
    private File createImageFile() throws IOException {
        // Create an image file name
        String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
        String imageFileName = "JPEG_" + timeStamp + "_";
        File storageDir = Environment.getExternalStoragePublicDirectory(
                Environment.DIRECTORY_PICTURES);
        File image = File.createTempFile(
                imageFileName,  /* prefix */
                ".jpg",         /* suffix */
                storageDir      /* directory */
        );

        // Save a file: path for use with ACTION_VIEW intents
        mCurrentPhotoPath = image.getAbsolutePath();     //"file:" +
        return image;
    }

    /**
     * carica opencv tramite opencvManager
     */
    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS: {
                    Log.i("a", "OpenCV loaded successfully");
                    // mOpenCvCameraView.enableView();

                    //carico face cascade
                    try {
                        // load cascade file from application resources
                        InputStream is = getResources().openRawResource(R.raw.haarcascade_frontalface_alt);
                        File cascadeDir = getDir("cascade", Context.MODE_PRIVATE);
                        mCascadeFile = new File(cascadeDir, "haarcascade_frontalface.xml");
                        FileOutputStream os = new FileOutputStream(mCascadeFile);

                        byte[] buffer = new byte[4096];
                        int bytesRead;
                        while ((bytesRead = is.read(buffer)) != -1) {
                            os.write(buffer, 0, bytesRead);
                        }
                        is.close();
                        os.close();
                        face_cascade = new CascadeClassifier(mCascadeFile.getAbsolutePath());
                        if (face_cascade.empty()) {
                            Log.e("e", "Failed to load cascade classifier");
                            face_cascade = null;
                        } else
                            Log.i("i", "Loaded cascade classifier from " + mCascadeFile.getAbsolutePath());
                        cascadeDir.delete();
                    } catch (IOException e) {
                        e.printStackTrace();
                        Log.e("E", "Errore caricamento face cascade");
                    }

                    //carico cascade occhi
                    try {
                        // load cascade file from application resources
                        InputStream is = getResources().openRawResource(R.raw.haarcascade_eye_tree_eyeglasses);
                        File cascadeDir = getDir("cascade", Context.MODE_PRIVATE);
                        mCascadeFile = new File(cascadeDir, "haarcascade_eye_tree_eyeglasses.xml");
                        FileOutputStream os = new FileOutputStream(mCascadeFile);

                        byte[] buffer = new byte[4096];
                        int bytesRead;
                        while ((bytesRead = is.read(buffer)) != -1) {
                            os.write(buffer, 0, bytesRead);
                        }
                        is.close();
                        os.close();
                        eyes_cascade = new CascadeClassifier(mCascadeFile.getAbsolutePath());
                        if (eyes_cascade.empty()) {
                            Log.e("e", "Failed to load cascade classifier");
                            eyes_cascade = null;
                        } else
                            Log.i("i", "Loaded cascade classifier from " + mCascadeFile.getAbsolutePath());
                        cascadeDir.delete();
                    } catch (IOException e) {
                        e.printStackTrace();
                        Log.e("E", "Errore caricamento eyes cascade");
                    }

                    //carico cascade bocca
                    try {
                        // load cascade file from application resources
                        InputStream is = getResources().openRawResource(R.raw.haarcascade_mcs_mouth);
                        File cascadeDir = getDir("cascade", Context.MODE_PRIVATE);
                        mCascadeFile = new File(cascadeDir, "haarcascade_mcs_mouth.xml");
                        FileOutputStream os = new FileOutputStream(mCascadeFile);

                        byte[] buffer = new byte[4096];
                        int bytesRead;
                        while ((bytesRead = is.read(buffer)) != -1) {
                            os.write(buffer, 0, bytesRead);
                        }
                        is.close();
                        os.close();
                        mouth_cascade = new CascadeClassifier(mCascadeFile.getAbsolutePath());
                        if (mouth_cascade.empty()) {
                            Log.e("e", "Failed to load cascade classifier");
                            mouth_cascade = null;
                        } else
                            Log.i("i", "Loaded cascade classifier from " + mCascadeFile.getAbsolutePath());
                        cascadeDir.delete();
                    } catch (IOException e) {
                        e.printStackTrace();
                        Log.e("E", "Errore caricamento mouth cascade");
                    }

                    //carico cascade naso
                    try {
                        // load cascade file from application resources
                        InputStream is = getResources().openRawResource(R.raw.haarcascade_mcs_nose);
                        File cascadeDir = getDir("cascade", Context.MODE_PRIVATE);
                        mCascadeFile = new File(cascadeDir, "haarcascade_mcs_nose.xml");
                        FileOutputStream os = new FileOutputStream(mCascadeFile);

                        byte[] buffer = new byte[4096];
                        int bytesRead;
                        while ((bytesRead = is.read(buffer)) != -1) {
                            os.write(buffer, 0, bytesRead);
                        }
                        is.close();
                        os.close();
                        nose_cascade = new CascadeClassifier(mCascadeFile.getAbsolutePath());
                        if (nose_cascade.empty()) {
                            Log.e("e", "Failed to load cascade classifier");
                            nose_cascade = null;
                        } else
                            Log.i("i", "Loaded cascade classifier from " + mCascadeFile.getAbsolutePath());
                        cascadeDir.delete();
                    } catch (IOException e) {
                        e.printStackTrace();
                        Log.e("E", "Errore caricamento nose cascade");
                    }

                }
                break;
                default: {
                    super.onManagerConnected(status);
                }
                break;
            }
        }
    };

    @Override
    public void onResume() {
        super.onResume();
        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_9, this, mLoaderCallback);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    /**
     * lancia l'intent per scattare una foto,
     * chiama la funzione createImageFile
     *
     * @param v View
     */
    public void scatta(View v) {
        Intent takePictureIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);

        immagineIsScattata = true;

        // Ensure that there's a camera activity to handle the intent
        if (takePictureIntent.resolveActivity(getPackageManager()) != null) {
            // Create the File where the photo should go
            File photoFile = null;
            try {
                photoFile = createImageFile();
            } catch (IOException ex) {
                // Error occurred while creating the File
                ex.printStackTrace();
            }
            // Continue only if the File was successfully created
            if (photoFile != null) {
                takePictureIntent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(photoFile));

                Log.i("uri", "uri.fromFile: " + Uri.fromFile(photoFile));
                Log.i("uri", "path: " + mCurrentPhotoPath);

                startActivityForResult(takePictureIntent, PHOTO_REQUEST_CODE);
            }
        }
    }

    /**
     * lancia l'intent per caricare una foto
     *
     * @param v View
     */
    public void carica(View v) {
        immagineIsScattata = false;
        Intent loadIntent = new Intent(Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        startActivityForResult(loadIntent, RESULT_LOAD_IMAGE);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        Button buttonCalcolaDistanze = (Button) findViewById(R.id.buttonEsegui);

        if (requestCode == PHOTO_REQUEST_CODE && resultCode == RESULT_OK/* && null != data*/) {

            matImg = Highgui.imread(mCurrentPhotoPath);
//            Mat tmp = Highgui.imread(data.getStringExtra(MediaStore.EXTRA_OUTPUT));

            Log.i("img", "larghezza: " + matImg.size().width);

            visualizzaImmagine(matImg, true);

            buttonCalcolaDistanze.setEnabled(true);

        } else if (requestCode == RESULT_LOAD_IMAGE && resultCode == RESULT_OK && null != data) {
            try {
                InputStream stream = getContentResolver().openInputStream(data.getData());
                immagine = BitmapFactory.decodeStream(stream);
                stream.close();

                // converto l'immagine in Mat e utilizzo la funzione visualizzaImmagine() per evitare che l'immagine sia troppo grande
                matImg = new Mat(immagine.getWidth(), immagine.getHeight(), CvType.CV_8UC1);
                Utils.bitmapToMat(immagine, matImg);

                visualizzaImmagine(matImg, false);

                buttonCalcolaDistanze.setEnabled(true);
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        } else if (requestCode == FACE_SHAPE_REQUEST && resultCode == RESULT_OK) {
            Log.d("INTERVALLI", "int FORMAVOLTO IN ACTIVITY RESULT: " + formaVolto);
            impostaIntervalli(formaVolto);
            calcolaDistanze();
        }
    }

    /**
     * riduce le dimensioni dell'immagine se superano una certa soglia e la visualizza in imgView
     *
     * @param immagine oggetto Mat da visualizzare
     * @param bgr2rgb  indica se effettuare il cambio da BGR a RGB prima della visualizzazione
     */

    private void visualizzaImmagine(Mat immagine, boolean bgr2rgb) {

        double larghezza = 1000;
        double altezza = 1000;

        if (immagine.size().width == 0) {
            return;
        }

        if (immagine.size().width > larghezza || immagine.size().height > altezza) {

            //se l'overhead della larghezza è maggiore di quello dell' altezza
            if (immagine.size().width - larghezza > immagine.size().height - altezza) {

                //la larghezza rimane al valore massimo, l'altezza cambia proporzionalmente.
                altezza = larghezza * immagine.size().height / immagine.size().width;
            } else {

                //altrimenti si fa il contrario: lunghezza = valore massimo; larghezza cambia proporzionalmente
                larghezza = altezza * immagine.size().width / immagine.size().height;
            }
        }

        // non modifico l'originale, ma solo un'elemento temporaneo che serve solo per la corretta visualizzazione
        Mat immagineAdattata = new Mat((int) larghezza, (int) altezza, CvType.CV_8UC1);

        // adatto l'immagine

        Size dimensione = new Size(larghezza, altezza);
        Imgproc.resize(immagine, immagineAdattata, dimensione);

        // Se l'immagine è stata acquisita occorre cambiare il color space
        if (bgr2rgb) {
            Mat tmp = new Mat();

            Imgproc.cvtColor(immagineAdattata, tmp, Imgproc.COLOR_BGR2RGB);

            this.immagine = Bitmap.createBitmap((int) tmp.size().width, (int) tmp.size().height, Bitmap.Config.ARGB_8888);

            Utils.matToBitmap(tmp, this.immagine);
        } else {
            this.immagine = Bitmap.createBitmap((int) immagineAdattata.size().width, (int) immagineAdattata.size().height, Bitmap.Config.ARGB_8888);

            Utils.matToBitmap(immagineAdattata, this.immagine);
        }

        imgView.setImageBitmap(this.immagine);
    }

    /**
     * Verifica che nell'immagine sia presente un volto, chiama l'activity per la selezione del volto, infine chiama la funzione
     * calcolaDistanze() che fa tutti i calcoli sull'immagine
     *
     * @param v
     */
    public void verificaPresenzaVoltoESelezionaForma(View v) {
        Button buttonCalcolaDistanze = (Button) findViewById(R.id.buttonEsegui);
        buttonCalcolaDistanze.setEnabled(false);

        if (matImg == null) {
            return;
        }

        MatOfRect faces = new MatOfRect();

        matGray = new Mat();

        int i = 0;
        int indiceVolto = 0;
        int indiceOcchioD = 0;
        int indiceOcchioS = 0;

        Imgproc.cvtColor(matImg, matGray, Imgproc.COLOR_BGR2GRAY);

        //riconoscimento volto

        if (face_cascade != null) {
            face_cascade.detectMultiScale(matGray, faces, 1.1, 2, Objdetect.CASCADE_FIND_BIGGEST_OBJECT, new Size(matGray.size().width / 10, matGray.size().height / 10), new Size());
        }

        Rect[] facesArray = faces.toArray();

        if (facesArray.length > 0) {
            // OK
        } else {
            Toast.makeText(getApplicationContext(), "Nessun volto individuato... \nProva con un'altra foto!", Toast.LENGTH_LONG).show();
            return;
        }


        //individuo le zone di interesse per la rilevazione di occhi, bocca, naso

        Rect eyesROIRect = new Rect(facesArray[indiceVolto].x, facesArray[indiceVolto].y + (facesArray[indiceVolto].height / 6), facesArray[indiceVolto].width, facesArray[indiceVolto].height / 2);
        Mat eyesROI = new Mat(matGray, eyesROIRect);

        //riconoscimento occhi

        MatOfRect eyes = new MatOfRect();

        if (eyes_cascade != null) {
            eyes_cascade.detectMultiScale(eyesROI, eyes, 1.1, 2, Objdetect.CASCADE_SCALE_IMAGE, new Size(matGray.size().width / 20, matGray.size().height / 20), new Size());
        }

        Rect[] eyesArray = eyes.toArray();

        if (eyesArray.length != 2) {
            Toast.makeText(getApplicationContext(), "Errore nel riconoscimento degli occhi... \nprova con un'altra foto!", Toast.LENGTH_LONG).show();
            return;
        }

        Intent faceShapeIntent = new Intent(this, FaceShapeActivity.class);
        startActivityForResult(faceShapeIntent, FACE_SHAPE_REQUEST);


    }

    /**
     * individua volto, occhi, naso, bocca e calcola le distanze caratteristiche
     *
     */
    public void calcolaDistanze() {

        Button buttonCalcolaDistanze = (Button) findViewById(R.id.buttonEsegui);
        buttonCalcolaDistanze.setEnabled(false);

        if (matImg == null) {
            return;
        }

        MatOfRect faces = new MatOfRect();

        int i = 0;

        //indici del vettore corrispondente, utile nel caso il classificatore riconosca più istanze di ogni cosa
        int indiceVolto = 0;
        int indiceOcchioD = 0;
        int indiceOcchioS = 0;
        int indiceNaso = 0;
        int indiceBocca = 0;

        matGray = new Mat();

        Imgproc.cvtColor(matImg, matGray, Imgproc.COLOR_BGR2GRAY);

        //riconoscimento volto

        if (face_cascade != null) {
            face_cascade.detectMultiScale(matGray, faces, 1.1, 2, Objdetect.CASCADE_FIND_BIGGEST_OBJECT, new Size(matGray.size().width / 10, matGray.size().height / 10), new Size());
        }

        Rect[] facesArray = faces.toArray();

        if (facesArray.length > 0) {
            //se sono stati indivisuati più volti, individuo l'indice del volto più grande
            if (facesArray.length > 1) {
                for (int j = 1; j < facesArray.length; j++) {
                    if (facesArray[j].width > facesArray[j - 1].width) {
                        indiceVolto = j;
                    }
                }
            }

            Core.rectangle(matImg, facesArray[indiceVolto].tl(), facesArray[indiceVolto].br(), new Scalar(0, 255, 0, 255), 3);
        } else {
            Toast.makeText(getApplicationContext(), "Nessun volto individuato... \nProva con un'altra foto!", Toast.LENGTH_LONG).show();
            return;
        }

        /*
        for (int i=0; i< facesArray.length; i++){
            Core.rectangle(matImg, facesArray[i].tl(), facesArray[i].br(), new Scalar(0, 255, 0, 255), 3 );
        }
        */

        //individuo le zone di interesse per la rilevazione di occhi, bocca, naso

        Rect eyesROIRect = new Rect(facesArray[indiceVolto].x, facesArray[indiceVolto].y + (facesArray[indiceVolto].height / 6), facesArray[indiceVolto].width, facesArray[indiceVolto].height / 2);
        Mat eyesROI = new Mat(matGray, eyesROIRect);

        Rect nosesROIRect = new Rect(facesArray[indiceVolto].x, facesArray[indiceVolto].y + (facesArray[indiceVolto].height / 3), facesArray[indiceVolto].width, facesArray[indiceVolto].height / 2);
        Mat nosesROI = new Mat(matGray, nosesROIRect);

        Rect mouthROIRect = new Rect(facesArray[indiceVolto].x, facesArray[indiceVolto].y + (facesArray[indiceVolto].height / 2), facesArray[indiceVolto].width, facesArray[indiceVolto].height / 2);
        Mat mouthROI = new Mat(matGray, mouthROIRect);

        //Core.rectangle(matImg,mouthROIRect.tl(), mouthROIRect.br(), new Scalar (255,255,255),3);
        //Core.rectangle(matImg,nosesROIRect.tl(), nosesROIRect.br(), new Scalar (120,0,255),3);
        //Core.rectangle(matImg,eyesROIRect.tl(), eyesROIRect.br(), new Scalar (255,120,0),3);

        //riconoscimento occhi

        MatOfRect eyes = new MatOfRect();

        if (eyes_cascade != null) {
            eyes_cascade.detectMultiScale(eyesROI, eyes, 1.1, 2, Objdetect.CASCADE_SCALE_IMAGE, new Size(matGray.size().width / 20, matGray.size().height / 20), new Size());
        }

        Rect[] eyesArray = eyes.toArray();
        if (eyesArray.length > 0) {
            for (i = 0; i < eyesArray.length; i++) {
                Core.rectangle(matImg, new Point(eyesArray[i].tl().x + eyesROIRect.tl().x, eyesArray[i].tl().y + eyesROIRect.tl().y),
                        new Point(eyesArray[i].br().x + eyesROIRect.tl().x, eyesArray[i].br().y + eyesROIRect.tl().y),
                        new Scalar(255, 0, 0, 255), 3);
                if (eyesArray[i].tl().x < eyesArray[indiceOcchioD].tl().x) {
                    indiceOcchioD = i;
                } else if (eyesArray[i].tl().x > eyesArray[indiceOcchioS].tl().x) {
                    indiceOcchioS = i;
                }
            }
        }

        if (eyesArray.length != 2) {
            Toast.makeText(getApplicationContext(), "Errore nel riconoscimento degli occhi... \nprova con un'altra foto!", Toast.LENGTH_LONG).show();
            return;
        }

        //riconoscimento naso

        MatOfRect noses = new MatOfRect();

        if (nose_cascade != null) {
            nose_cascade.detectMultiScale(nosesROI, noses, 1.1, 2, Objdetect.CASCADE_FIND_BIGGEST_OBJECT, new Size(matGray.size().width / 20, matGray.size().height / 20), new Size());
        }

        Rect[] nosesArray = noses.toArray();

        if (nosesArray.length > 0) {
            //se sono stati indivisuati più nasi, individuo l'indice del naso più grande
            if (nosesArray.length > 1) {
                for (int j = 1; j < nosesArray.length; j++) {
                    if (nosesArray[j].width > nosesArray[j - 1].width) {
                        indiceNaso = j;
                    }
                }
            }

            Core.rectangle(matImg, new Point(nosesArray[indiceNaso].tl().x + nosesROIRect.tl().x, nosesArray[indiceNaso].tl().y + nosesROIRect.tl().y),
                    new Point(nosesArray[indiceNaso].br().x + nosesROIRect.tl().x, nosesArray[indiceNaso].br().y + nosesROIRect.tl().y),
                    new Scalar(0, 0, 255, 255), 3);
        }


        //riconoscimento bocca

        MatOfRect mouths = new MatOfRect();

        if (mouth_cascade != null) {
            mouth_cascade.detectMultiScale(mouthROI, mouths, 1.1, 2, Objdetect.CASCADE_FIND_BIGGEST_OBJECT, new Size(matGray.size().width / 20, matGray.size().height / 20), new Size());
        }

        Rect[] mouthsArray = mouths.toArray();

        if (mouthsArray.length > 0) {
            //se sono state indivisuate più bocche, individuo l'indice della bocca più grande
            if (mouthsArray.length > 1) {
                for (int j = 1; j < mouthsArray.length; j++) {
                    if (mouthsArray[j].width > mouthsArray[j - 1].width) {
                        indiceBocca = j;
                    }
                }
            }

            Core.rectangle(matImg, new Point(mouthsArray[indiceBocca].tl().x + mouthROIRect.tl().x, mouthsArray[indiceBocca].tl().y + mouthROIRect.tl().y),
                    new Point(mouthsArray[indiceBocca].br().x + mouthROIRect.tl().x, mouthsArray[indiceBocca].br().y + mouthROIRect.tl().y), new Scalar(0, 255, 255, 255), 3);
        }

        //punti fondamentali
        Point occhioDD = new Point(eyesROIRect.tl().x + eyesArray[indiceOcchioD].tl().x,
                eyesROIRect.tl().y + eyesArray[indiceOcchioD].tl().y + eyesArray[indiceOcchioD].height / 2);

        Point occhioDS = new Point(eyesROIRect.tl().x + eyesArray[indiceOcchioD].tl().x + eyesArray[indiceOcchioD].width,
                eyesROIRect.tl().y + eyesArray[indiceOcchioD].tl().y + eyesArray[indiceOcchioD].height / 2);

        Point occhioSD = new Point(eyesROIRect.tl().x + eyesArray[indiceOcchioS].tl().x,
                eyesROIRect.tl().y + eyesArray[indiceOcchioS].tl().y + eyesArray[indiceOcchioS].height / 2);

        Point occhioSS = new Point(eyesROIRect.tl().x + eyesArray[indiceOcchioS].tl().x + eyesArray[indiceOcchioS].width,
                eyesROIRect.tl().y + eyesArray[indiceOcchioS].tl().y + eyesArray[indiceOcchioS].height / 2);

        Point boccaD = new Point(mouthROIRect.tl().x + mouthsArray[indiceBocca].tl().x,
                mouthROIRect.tl().y + mouthsArray[indiceBocca].tl().y + mouthsArray[indiceBocca].height / 2);

        Point boccaS = new Point(mouthROIRect.tl().x + mouthsArray[indiceBocca].tl().x + mouthsArray[indiceBocca].width,
                mouthROIRect.tl().y + mouthsArray[indiceBocca].tl().y + mouthsArray[indiceBocca].height / 2);

        Point naso = new Point(nosesROIRect.tl().x + nosesArray[indiceNaso].tl().x + nosesArray[indiceNaso].width / 2,
                nosesROIRect.tl().y + nosesArray[indiceNaso].tl().y + nosesArray[indiceNaso].height / 2);

        //disegno cerchietti bianchi intorno ai punti
        Core.circle(matImg, occhioDD, eyesArray[indiceOcchioD].width / 5, new Scalar(255, 255, 255), 5);
        Core.circle(matImg, occhioDS, eyesArray[indiceOcchioD].width / 5, new Scalar(255, 255, 255), 5);
        Core.circle(matImg, occhioSD, eyesArray[indiceOcchioD].width / 5, new Scalar(255, 255, 255), 5);
        Core.circle(matImg, occhioSS, eyesArray[indiceOcchioD].width / 5, new Scalar(255, 255, 255), 5);
        Core.circle(matImg, boccaD, eyesArray[indiceOcchioD].width / 5, new Scalar(255, 255, 255), 5);
        Core.circle(matImg, boccaS, eyesArray[indiceOcchioD].width / 5, new Scalar(255, 255, 255), 5);
        Core.circle(matImg, naso, eyesArray[indiceOcchioD].width / 5, new Scalar(255, 255, 255), 5);

        //PARAMETRI

        float riferimento = (float) facesArray[indiceVolto].height / 12;

        //VERTICALI

        //Altezza della fronte
        //limite superiore del volto individuato - altezza media degli occhi
        double altezzaMediaOcchi = (occhioDD.y + occhioDS.y + occhioSD.y + occhioSS.y) / 4;
        double altezzaFronte = altezzaMediaOcchi - facesArray[indiceVolto].tl().y;
        // +0.5 è stato aggiunto in modo tale che parametroFronte non sia ottenuto troncando il risultato, ma arrotondandolo. (il parametro è sempre positivo)
        int parametroFronte = (int) (altezzaFronte / riferimento + 0.5);

        //Alteza del naso
        //punta del naso  - punto di partenza del naso (utilizzo altezzaMediaOcchi)
        double altezzaNaso = naso.y - altezzaMediaOcchi;
        int parametroNaso = (int) (altezzaNaso / riferimento + 0.5);

        //Distanza naso-bocca
        //Altezza media della bocca - altezza naso
        double altezzaBocca = (boccaS.y + boccaD.y) / 2;
        double distanzaBoccaNaso = altezzaBocca - naso.y;
        int parametroBoccaNaso = (int) (distanzaBoccaNaso / riferimento + 0.5);

        //ORIZZONTALI

        //Distanza degli occhi
        //Calcolata dai punti più esterni di ciascun occhio
        double distanzaOcchi = occhioSS.x - occhioDD.x;
        int parametroOcchi = (int) (distanzaOcchi / riferimento + 0.5);

        //Larghezza della bocca
        double larghezzaBocca = Math.sqrt((boccaS.x - boccaD.x) * (boccaS.x - boccaD.x) + (boccaS.y - boccaD.y) * (boccaS.y - boccaD.y));
        int parametroBocca = (int) (larghezzaBocca / riferimento + 0.5);

        //ANGOLI
        //angolo fra linea bocca e naso (lato destro)
        double angoloBoccaNaso = calcolaAngolo(naso, boccaD, boccaD, boccaS);
        //in gradi
        angoloBoccaNaso = angoloBoccaNaso * 180 / 3.1415;

        //angolo fra linea bocca e naso (lato sinistro)
        double angoloBoccaNaso2 = calcolaAngolo(naso, boccaS, boccaS, boccaD);
        //in gradi
        angoloBoccaNaso2 = angoloBoccaNaso2 * 180 / 3.1415;

        //angolo fra linea bocca e naso (al naso)
        double angoloNasoBocca = 180 - angoloBoccaNaso - angoloBoccaNaso2;


        //disegno linee angolo
        Core.line(matImg, naso, boccaD, new Scalar(0, 180, 60), 5);
        Core.line(matImg, naso, boccaS, new Scalar(0, 180, 60), 5);

        visualizzaImmagine(matImg, immagineIsScattata);

        int ottavaFronte = OTTAVA_FRONTE;
        int ottavaNaso = OTTAVA_NASO;
        int ottavaBoccaNaso = OTTAVA_BOCCA_NASO;
        int ottavaOcchi = OTTAVA_OCCHI;
        int ottavaBocca = OTTAVA_BOCCA;

        Log.d("INTERVALLI: ", "formavolto in calcoladistanze(): " + formaVolto);

        impostaIntervalli(formaVolto);


        //scrive la variabile tonica
        calcolaFondamentale(angoloNasoBocca);
        //più 12 perchè altrimenti va a scrivere all'ottava 0 o -1
        vettoreNote[0] = vettoreIntervalli[tonica] + SOL + 12;

        //se la tonica è sol - la - si bisogna spostare di un'altra ottava altrimenti si va oltre la tastiera
        if (tonica < 3) {
            vettoreNote[0] = vettoreNote[0] + 12;
        }

        impostaIntervalliDaTonica();

        //si parte da G0, se la tonica è G0, A0, B0 -> le ottave di riferimento sono giuste, altrimenti la tonica sta sull'ottava 1 e bisogna incrementare tutte le altre ottave di riferimento
        if (vettoreNote[0] > 2) {
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

        boolean duplicati = true;
        while (duplicati == true) {
            duplicati = trovaDuplicatiESpostaOttava(6);
        }

        salvaMIDI(vettoreNote, 6);

        ImageButton buttonPlay = (ImageButton) findViewById(R.id.buttonPlay);
        ImageButton buttonStop = (ImageButton) findViewById(R.id.buttonStop);
        ImageButton buttonPause = (ImageButton) findViewById(R.id.buttonPause);

        buttonPlay.setEnabled(true);
        buttonStop.setEnabled(true);
        buttonPause.setEnabled(true);

        Log.d("RIFERIMENTO: ", "rif: " + riferimento);

        Log.d("DISTANZE: ", "volto: " + facesArray[indiceVolto].height);
        Log.d("DISTANZE: ", "fronte: " + altezzaFronte);
        Log.d("DISTANZE: ", "naso: " + altezzaNaso);
        Log.d("DISTANZE: ", "boccanaso: " + distanzaBoccaNaso);
        Log.d("DISTANZE: ", "occhi: " + distanzaOcchi);
        Log.d("DISTANZE: ", "bocca: " + larghezzaBocca);
        Log.d("ANGOLO: ", "naso: " + angoloNasoBocca);


        Log.d("PARAMETRO: ", "fronte: " + parametroFronte);
        Log.d("PARAMETRO: ", "naso: " + parametroNaso);
        Log.d("PARAMETRO: ", "boccanaso: " + parametroBoccaNaso);
        Log.d("PARAMETRO: ", "occhi: " + parametroOcchi);
        Log.d("PARAMETRO: ", "bocca: " + parametroBocca);

        Log.d("NOTE: ", "0: " + vettoreNote[0]);
        Log.d("NOTE: ", "1: " + vettoreNote[1]);
        Log.d("NOTE: ", "2: " + vettoreNote[2]);
        Log.d("NOTE: ", "3: " + vettoreNote[3]);
        Log.d("NOTE: ", "4: " + vettoreNote[4]);
        Log.d("NOTE: ", "5: " + vettoreNote[5]);


    }

    /**
     * Calcola l'angolo che si forma dalle rette individuate dai punti passati per parametro.
     * La prima retta è individuata dai punti A e B, la seconda dai punti C e D.
     *
     * @param a Punto 1
     * @param b Punto 2
     * @param c Punto 3
     * @param d Punto 4
     * @return angolo formato dalle rette individuate dai punti passati per parametro
     */
    private double calcolaAngolo(Point a, Point b, Point c, Point d) {
        double angolo;
        double m1, m2;
        //se le coordinate x sono uguali -> m assume valore infinito, assegno un valore alto
        if (b.x == a.x) {
            m1 = 1000;
        } else {
            m1 = (b.y - a.y) / (b.x - a.x);
        }

        if (d.x == c.x) {
            m2 = 1000;
        } else {
            m2 = (d.y - c.y) / (d.x - c.x);
        }

        double argomentoTangente = (m1 - m2) / (1 + (m1 * m2));
        angolo = Math.atan(argomentoTangente);

        if (angolo < 0) {
            angolo = angolo * (-1);
        }

        return angolo;
    }

    /**
     * imposta gli intervalli (in semitoni) della scala individuata da ciascun modo
     *
     * @param volto intero che definisce la forma del volto
     */
    private void impostaIntervalli(int volto) {
        if (volto == VOLTOOVALE) {
            //Modo ionico
            vettoreIntervalli[0] = 0;
            vettoreIntervalli[1] = 2;
            vettoreIntervalli[2] = 4;
            vettoreIntervalli[3] = 5;
            vettoreIntervalli[4] = 7;
            vettoreIntervalli[5] = 9;
            vettoreIntervalli[6] = 11;
        } else if (volto == VOLTOROTONDO) {
            //modo dorico
            vettoreIntervalli[0] = 0;
            vettoreIntervalli[1] = 2;
            vettoreIntervalli[2] = 3;
            vettoreIntervalli[3] = 5;
            vettoreIntervalli[4] = 7;
            vettoreIntervalli[5] = 9;
            vettoreIntervalli[6] = 10;
        } else if (volto == VOLTOQUADRATO) {
            //modo frigio
            vettoreIntervalli[0] = 0;
            vettoreIntervalli[1] = 1;
            vettoreIntervalli[2] = 3;
            vettoreIntervalli[3] = 5;
            vettoreIntervalli[4] = 7;
            vettoreIntervalli[5] = 8;
            vettoreIntervalli[6] = 10;
        } else if (volto == VOLTODIAMANTE) {
            //lidio
            vettoreIntervalli[0] = 0;
            vettoreIntervalli[1] = 2;
            vettoreIntervalli[2] = 4;
            vettoreIntervalli[3] = 6;
            vettoreIntervalli[4] = 7;
            vettoreIntervalli[5] = 9;
            vettoreIntervalli[6] = 11;
        } else if (volto == VOLTOCUORE) {
            //misolidio
            vettoreIntervalli[0] = 0;
            vettoreIntervalli[1] = 2;
            vettoreIntervalli[2] = 4;
            vettoreIntervalli[3] = 5;
            vettoreIntervalli[4] = 7;
            vettoreIntervalli[5] = 9;
            vettoreIntervalli[6] = 10;
        } else if (volto == VOLTORETTANGOLARE) {
            //eolio
            vettoreIntervalli[0] = 0;
            vettoreIntervalli[1] = 2;
            vettoreIntervalli[2] = 3;
            vettoreIntervalli[3] = 5;
            vettoreIntervalli[4] = 7;
            vettoreIntervalli[5] = 8;
            vettoreIntervalli[6] = 10;
        } else if (volto == VOLTOTRIANGOLARE) {
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

    /**
     * imposta gli intervalli (in semitoni) dalla tonica, sulla scala modale costruita su sol
     */
    private void impostaIntervalliDaTonica() {
        vettoreIntervalliDaTonica[0] = 0;
        for (int i = 1; i < 7; i++) {
            vettoreIntervalliDaTonica[i] = vettoreIntervalliDaTonica[i - 1] + vettoreIntervalli[(tonica + i) % 7] - vettoreIntervalli[(tonica + i - 1) % 7];
            if (vettoreIntervalliDaTonica[i] < 0) {
                vettoreIntervalliDaTonica[i] = vettoreIntervalliDaTonica[i - 1] + 12 - vettoreIntervalli[(tonica + i - 1) % 7];
            }
        }
    }

    /**
     * calcola l'intero che rappresenta la nota desiderata in MIDI
     *
     * @param intervalloDiPartenza intero che definisce quale nota suonare
     * @param ottavaDiPartenza     intero che definisce l'ottava a partire dalla quale occorre calcolare la nota
     * @return valore della nota in notazione MIDI
     */
    int calcolaNumeroNotaMidi(int intervalloDiPartenza, int ottavaDiPartenza) {
        int ottava = ottavaDiPartenza;

        // se il parametro vale 1 si suona la tonica, quindi la nota 0.
        int intervallo = intervalloDiPartenza - 1;

        // se l'intervallo è maggiore di una settima, salgo di ottava e sottraggo 8 all'intervallo
        while (intervallo > 7) {
            ottava++;
            intervallo = intervallo - 8;
        }

        //la nota si ottiene come la tonica + l'intervallo a partire dalla tonica (in semitoni) + l'ottava di riferimento per 12 (semitoni)
        int nota = vettoreNote[0] + vettoreIntervalliDaTonica[intervallo] + ottava * 12;


        return nota;
    }

    /**
     * Restituisce la nota passata per parametro riportata all'ottava centrale
     *
     * @param nota intero che definisce la nota in MIDI
     * @return intero che definisce la nota in ingresso traslata sull'ottava centrale
     */
    int notaCorrispondenteOttavaCentrale(int nota) {
        //fino a quando la nota è sotto l'ottava centrale, aggiungo 12 (semitoni) e sposto la nota di un ottava in alto
        while (nota < MIN_OTTAVA_CENTRALE) {
            nota = nota + 12;
        }

        //fino a quando la nota è sopra l'ottava centrale, sottraggo 12 (semitoni) e sposto la nota di un ottava in basso
        while (nota > (MIN_OTTAVA_CENTRALE + 11)) {
            nota = nota - 12;
        }

        return nota;
    }

    /**
     * Verifica se la nota appartiene all'ottava centrale del pianoforte
     *
     * @param nota intero che definisce la nota in MIDI
     * @return true se la nota appartiene all'ottava centrale, altrimenti false
     */
    private boolean notaOttavaCentrale(int nota) {
        if (nota >= MIN_OTTAVA_CENTRALE && nota <= MIN_OTTAVA_CENTRALE + 11) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Imposta il valore della tonica (fondamentale) in base all'angolo passato per parametro
     *
     * @param angolo angolo in gradi in base al quale calcolare la tonica
     */
    private void calcolaFondamentale(double angolo) {
        if (angolo < 60) {
            tonica = 0; //sol
        } else if (angolo < 67) {
            tonica = 1; //la
        } else if (angolo < 73.5) {
            tonica = 2; //si
        } else if (angolo < 79) {
            tonica = 3; //do
        } else if (angolo < 83.4) {
            tonica = 4; //re
        } else if (angolo < 86.7) {
            tonica = 5; //mi
        } else if (angolo < 88.9) {
            tonica = 6; //fa
        } else if (angolo < 91.1) {
            tonica = 0;
        } else if (angolo < 93.3) {
            tonica = 1;
        } else if (angolo < 96.6) {
            tonica = 2;
        } else if (angolo < 101) {
            tonica = 3;
        } else if (angolo < 106.5) {
            tonica = 4;
        } else if (angolo < 113.1) {
            tonica = 5;
        } else if (angolo < 120) {
            tonica = 6;
        } else {
            tonica = 0;
        }
    }

    /**
     * Controlla se nel vettore delle note ci sono duplicati, in tal caso sposta la nota duplicata all'ottava superiore
     *
     * @param n numero delle note nel vettore
     * @return true quando trova duplicati, false altrimenti
     */
    private boolean trovaDuplicatiESpostaOttava(int n) {
        // ordino il vettore in ordine crescente
        int min, temp, i, j;
        boolean noteUguali = false;
        for (i = 0; i < n - 1; i++) {
            min = i;
            for (j = i + 1; j < n; j++)
                if (vettoreNote[j] < vettoreNote[min])
                    min = j;
            temp = vettoreNote[min];
            vettoreNote[min] = vettoreNote[i];
            vettoreNote[i] = temp;
        }
        //se due note sono uguali, ne sposto una all'ottava superiore (+12 semitoni)
        for (i = 0; i < n - 1; i++) {
            if (vettoreNote[i] == vettoreNote[i + 1]) {
                vettoreNote[i + 1] = vettoreNote[i + 1] + 12;
                noteUguali = true;
            }
        }
        return noteUguali;
    }

    /**
     * Scrive il file midi e inizializza il mediaPlayer
     *
     * @param nota       vettore delle note
     * @param numeroNote numero di note da scrivere
     */
    private void salvaMIDI(int[] nota, int numeroNote) {


        try {
            File file = new File(getApplicationContext().getFilesDir(), NOME_FILE_MIDI);

            //outputStream = openFileOutput(NOME_FILE_MIDI, Context.MODE_PRIVATE);
            FileOutputStream outputStream = new FileOutputStream(file);


            byte[] byteArray = new byte[]{

                    (byte) 0x4D, (byte) 0x54, (byte) 0x68, (byte) 0x64, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x06, (byte) 0x00, (byte) 0x01, (byte) 0x00, (byte) 0x05, (byte) 0x03, (byte) 0xC0, (byte) 0x4D, (byte) 0x54,
                    (byte) 0x72, (byte) 0x6B, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x24, (byte) 0x00, (byte) 0xFF, (byte) 0x51, (byte) 0x03, (byte) 0x07, (byte) 0xA1, (byte) 0x20, (byte) 0x00, (byte) 0xFF, (byte) 0x59,
                    (byte) 0x02, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0xFF, (byte) 0x03, (byte) 0x07, (byte) 0x6F, (byte) 0x75, (byte) 0x74, (byte) 0x70, (byte) 0x75, (byte) 0x74, (byte) 0x00, (byte) 0x00, (byte) 0xFF, (byte) 0x58, (byte) 0x04, (byte) 0x04,
                    (byte) 0x02, (byte) 0x18, (byte) 0x08, (byte) 0x00, (byte) 0xFF, (byte) 0x2F, (byte) 0x00,

                    (byte) 0x4D, (byte) 0x54, (byte) 0x72, (byte) 0x6B,
                    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0xA3,

                    (byte) 0x00,

                    (byte) 0xB0, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x20, (byte) 0x00, (byte) 0x00,

                    (byte) 0xFF, (byte) 0x03, (byte) 0x09, (byte) 0x74, (byte) 0x72, (byte) 0x61,
                    (byte) 0x63, (byte) 0x63, (byte) 0x69, (byte) 0x61, (byte) 0x31, (byte) 0x00, (byte) 0x00,

                    (byte) 0xB0, (byte) 0x07, (byte) 0x5A,
                    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x20, (byte) 0x00, (byte) 0x00, (byte) 0x07, (byte) 0x5A,
                    (byte) 0x00, (byte) 0x5B, (byte) 0x7F
            };

            outputStream.write(byteArray);


            byte[] byteArray2 = new byte[]{
                    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x20, (byte) 0x00, (byte) 0x00, (byte) 0x07, (byte) 0x5A
            };

            for (int i = 0; i < 4; i++) {
                outputStream.write(byteArray2);
            }

            //ACCORDO

            //note on
            byte[] byteArrayAccordoOn = new byte[]{
                    (byte) 0x00, (byte) 0x90
            };

            outputStream.write(byteArrayAccordoOn);

            //note
            for (int i = 0; i < numeroNote; i++) {
                if (i != numeroNote - 1) {
                    byte[] byteArrayAccordo = new byte[]{
                            (byte) nota[i], (byte) 0x5A, (byte) 0x00
                    };
                    outputStream.write(byteArrayAccordo);
                } else {
                    byte[] byteArrayAccordo = new byte[]{
                            (byte) nota[i], (byte) 0x5A
                    };
                    outputStream.write(byteArrayAccordo);
                }

            }

            //delay più note off
            byte[] byteArrayAccordoOff = new byte[]{
                    (byte) 0x81, (byte) 0xB4, (byte) 0x00, (byte) 0x80
            };

            outputStream.write(byteArrayAccordoOff);

            //note e delay
            for (int i = 0; i < numeroNote; i++) {
                if (i != numeroNote - 1) {
                    byte[] byteArrayAccordo = new byte[]{
                            (byte) nota[i], (byte) 0x00, (byte) 0x00
                    };
                    outputStream.write(byteArrayAccordo);

                } else {
                    //ultimo delay (fra primo accordo e accordo sulla melodia) più note on
                    byte[] byteArrayAccordo = new byte[]{
                            (byte) nota[i], (byte) 0x00, (byte) 0x84, (byte) 0xD8, (byte) 0x00, (byte) 0x90
                    };
                    outputStream.write(byteArrayAccordo);
                }

            }

            //ACCORDO SULLA MELODIA
            //note on sta già messo sopra
            //accordo (senza la nota nell'ottava centrale)
            for (int i = 0; i < numeroNote; i++) {
                if (i != numeroNote - 1) {
                    byte[] byteArrayAccordo = new byte[]{
                            (byte) nota[i], (byte) 0x5A, (byte) 0x00
                    };
                    outputStream.write(byteArrayAccordo);

                } else {
                    //ultimo delay più note off
                    byte[] byteArrayAccordo = new byte[]{
                            (byte) nota[i], (byte) 0x5A, (byte) 0x81, (byte) 0xA5, (byte) 0x00, (byte) 0x80
                    };
                    outputStream.write(byteArrayAccordo);
                }
            }

            //chiusura accordo (note off sta già messo sopra)
            for (int i = 0; i < numeroNote; i++) {
                if (i != numeroNote - 1) {
                    byte[] byteArrayAccordo = new byte[]{
                            (byte) nota[i], (byte) 0x00, (byte) 0x00
                    };
                    outputStream.write(byteArrayAccordo);

                } else {
                    //ultimo delay più note off
                    byte[] byteArrayAccordo = new byte[]{
                            (byte) nota[i], (byte) 0x00, (byte) 0x8E, (byte) 0x7F
                    };
                    outputStream.write(byteArrayAccordo);
                }
            }

            //fine traccia accordi
            byte[] byteArrayFineTraccia1 = new byte[]{
                    (byte) 0xB0, (byte) 0x7F, (byte) 0x00, (byte) 0x00,
                    (byte) 0xFF, (byte) 0x2F, (byte) 0x00
            };
            outputStream.write(byteArrayFineTraccia1);

            //TRACCIA 2

            byte[] byteArrayTraccia2 = new byte[]{

                    (byte) 0x4D, (byte) 0x54, (byte) 0x72, (byte) 0x6B,
                    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x5A,

                    (byte) 0x00,

                    (byte) 0xB1, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x20, (byte) 0x00, (byte) 0x00,

                    (byte) 0xFF, (byte) 0x03, (byte) 0x09, (byte) 0x74, (byte) 0x72, (byte) 0x61,
                    (byte) 0x63, (byte) 0x63, (byte) 0x69, (byte) 0x61, (byte) 0x32, (byte) 0x00, (byte) 0x00,

                    (byte) 0xB1, (byte) 0x07, (byte) 0x5A,
                    (byte) 0x81, (byte) 0xF0, (byte) 0x00
            };

            outputStream.write(byteArrayTraccia2);

            //Arpeggio ascendente con minime

            for (int i = 0; i < numeroNote; i++) {
                if (i != numeroNote - 1) {
                    byte[] byteArrayArpeggioTr2 = new byte[]{
                            (byte) 0x91, (byte) nota[i], (byte) 0x5A,
                            (byte) 0x8F, (byte) 0x00, (byte) 0x81,
                            (byte) nota[i], (byte) 0x00, (byte) 0x00,
                    };
                    outputStream.write(byteArrayArpeggioTr2);
                } else {
                    byte[] byteArrayArpeggioTr2 = new byte[]{
                            (byte) 0x91, (byte) nota[i], (byte) 0x5A,
                            (byte) 0x8F, (byte) 0x00, (byte) 0x81,
                            (byte) nota[i], (byte) 0x00,
                            (byte) 0x84, (byte) 0xF5, (byte) 0x7F,
                    };
                    outputStream.write(byteArrayArpeggioTr2);
                }
            }

            //fine traccia 2
            byte[] byteArrayFineTraccia2 = new byte[]{
                    (byte) 0xB0, (byte) 0x7F, (byte) 0x00, (byte) 0x00,
                    (byte) 0xFF, (byte) 0x2F, (byte) 0x00
            };
            outputStream.write(byteArrayFineTraccia2);

            //TRACCIA 3
            byte[] byteArrayTraccia3 = new byte[]{

                    (byte) 0x4D, (byte) 0x54, (byte) 0x72, (byte) 0x6B,
                    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x90,

                    (byte) 0x00,

                    (byte) 0xB2, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x20, (byte) 0x00, (byte) 0x00,

                    (byte) 0xFF, (byte) 0x03, (byte) 0x09, (byte) 0x74, (byte) 0x72, (byte) 0x61,
                    (byte) 0x63, (byte) 0x63, (byte) 0x69, (byte) 0x61, (byte) 0x33, (byte) 0x00, (byte) 0x00,

                    (byte) 0xB2, (byte) 0x07, (byte) 0x5A,
                    (byte) 0x83, (byte) 0x86, (byte) 0x00
            };

            outputStream.write(byteArrayTraccia3);

            //Arpeggio ascendente (semiminime)
            for (int i = 0; i < numeroNote; i++) {

                byte[] byteArrayArpeggioTr3 = new byte[]{
                        (byte) 0x92, (byte) nota[i], (byte) 0x5A,
                        (byte) 0x87, (byte) 0x40, (byte) 0x82,
                        (byte) nota[i], (byte) 0x00, (byte) 0x00
                };
                outputStream.write(byteArrayArpeggioTr3);

            }

            //Arpeggio discendente (semiminime)
            for (int i = numeroNote - 1; i > -1; i--) {
                if (i == 0) {
                    byte[] byteArrayArpeggioTr3 = new byte[]{
                            (byte) 0x92, (byte) nota[i], (byte) 0x5A,
                            (byte) 0x87, (byte) 0x40, (byte) 0x82,
                            (byte) nota[i], (byte) 0x00,
                            (byte) 0x83, (byte) 0xDF, (byte) 0x7F,
                    };
                    outputStream.write(byteArrayArpeggioTr3);
                } else {
                    byte[] byteArrayArpeggioTr3 = new byte[]{
                            (byte) 0x92, (byte) nota[i], (byte) 0x5A,
                            (byte) 0x87, (byte) 0x40, (byte) 0x82,
                            (byte) nota[i], (byte) 0x00, (byte) 0x00,
                    };
                    outputStream.write(byteArrayArpeggioTr3);
                }

            }

            //fine traccia 3
            byte[] byteArrayFineTraccia3 = new byte[]{
                    (byte) 0xB0, (byte) 0x7F, (byte) 0x00, (byte) 0x00,
                    (byte) 0xFF, (byte) 0x2F, (byte) 0x00
            };
            outputStream.write(byteArrayFineTraccia3);

            //TRACCIA 4
            byte[] byteArrayTraccia4 = new byte[]{

                    (byte) 0x4D, (byte) 0x54, (byte) 0x72, (byte) 0x6B,
                    (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0xEA,

                    (byte) 0x00,

                    (byte) 0xB3, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x20, (byte) 0x00, (byte) 0x00,

                    (byte) 0xFF, (byte) 0x03, (byte) 0x09, (byte) 0x74, (byte) 0x72, (byte) 0x61,
                    (byte) 0x63, (byte) 0x63, (byte) 0x69, (byte) 0x61, (byte) 0x34, (byte) 0x00, (byte) 0x00,

                    (byte) 0xB3, (byte) 0x07, (byte) 0x5A,
                    (byte) 0x84, (byte) 0x9C, (byte) 0x00
            };

            outputStream.write(byteArrayTraccia4);

            //melodia con simiminime riportate sull'ottava centrale

            //fase ascendente
            for (int i = 0; i < numeroNote; i++) {
                byte[] byteArrayMelodiaAscendente = new byte[]{
                        (byte) 0x93, (byte) notaCorrispondenteOttavaCentrale(nota[i]), (byte) 0x5A,
                        (byte) 0x8F, (byte) 0x00, (byte) 0x83,
                        (byte) notaCorrispondenteOttavaCentrale(nota[i]), (byte) 0x00, (byte) 0x00
                };
                outputStream.write(byteArrayMelodiaAscendente);
            }

            //melodia discendente
            for (int i = numeroNote - 2; i > -1; i--) {
                if (i == 0) {
                    byte[] byteArrayMelodiaDiscendente = new byte[]{
                            (byte) 0x93, (byte) notaCorrispondenteOttavaCentrale(nota[i]), (byte) 0x5A,
                            (byte) 0x8F, (byte) 0x00, (byte) 0x83,
                            (byte) notaCorrispondenteOttavaCentrale(nota[i]), (byte) 0x00, (byte) 0xCB, (byte) 0x00
                    };
                    outputStream.write(byteArrayMelodiaDiscendente);
                } else {
                    byte[] byteArrayMelodiaDiscendente = new byte[]{
                            (byte) 0x93, (byte) notaCorrispondenteOttavaCentrale(nota[i]), (byte) 0x5A,
                            (byte) 0x8F, (byte) 0x00, (byte) 0x83,
                            (byte) notaCorrispondenteOttavaCentrale(nota[i]), (byte) 0x00, (byte) 0x00
                    };
                    outputStream.write(byteArrayMelodiaDiscendente);
                }
            }

            //Melodia (sincrona all'accordo)
            for (int i = 0; i < numeroNote; i++) {
                byte[] byteArrayMelodiaAscendente = new byte[]{
                        (byte) 0x93, (byte) notaCorrispondenteOttavaCentrale(nota[i]), (byte) 0x5A,
                        (byte) 0x8F, (byte) 0x00, (byte) 0x83,
                        (byte) notaCorrispondenteOttavaCentrale(nota[i]), (byte) 0x00, (byte) 0x00
                };
                outputStream.write(byteArrayMelodiaAscendente);
            }

            for (int i = numeroNote - 2; i > -1; i--) {
                if (i == 0) {
                    byte[] byteArrayMelodiaDiscendente = new byte[]{
                            (byte) 0x93, (byte) notaCorrispondenteOttavaCentrale(nota[i]), (byte) 0x5A,
                            (byte) 0x8F, (byte) 0x00, (byte) 0x83,
                            (byte) notaCorrispondenteOttavaCentrale(nota[i]), (byte) 0x00, (byte) 0x8E, (byte) 0x7F
                    };
                    outputStream.write(byteArrayMelodiaDiscendente);
                } else {
                    byte[] byteArrayMelodiaDiscendente = new byte[]{
                            (byte) 0x93, (byte) notaCorrispondenteOttavaCentrale(nota[i]), (byte) 0x5A,
                            (byte) 0x8F, (byte) 0x00, (byte) 0x83,
                            (byte) notaCorrispondenteOttavaCentrale(nota[i]), (byte) 0x00, (byte) 0x00
                    };
                    outputStream.write(byteArrayMelodiaDiscendente);
                }
            }

            //Fine traccia 4
            byte[] byteArrayFineTraccia4 = new byte[]{
                    (byte) 0xB0, (byte) 0x7F, (byte) 0x00, (byte) 0x00,
                    (byte) 0xFF, (byte) 0x2F, (byte) 0x00
            };
            outputStream.write(byteArrayFineTraccia4);


            Log.d("FILE MIDI ", "path " + file.getAbsolutePath());
            Log.d("FILE MIDI ", "context " + getApplicationContext().getFilesDir());
            Log.d("FILE MIDI ", "FD " + outputStream.getFD());
            Log.d("FILE MIDI ", "uri " + Uri.parse(getApplicationContext().getFilesDir() + File.separator + NOME_FILE_MIDI));

            mediaPlayer = MediaPlayer.create(this, Uri.parse(getApplicationContext().getFilesDir() + File.separator + NOME_FILE_MIDI));
            //mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);

            mSeekBar.setMax(mediaPlayer.getDuration() / 1000);
            mSeekBar.setEnabled(true);

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}
