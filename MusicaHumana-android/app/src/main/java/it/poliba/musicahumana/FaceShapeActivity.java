package it.poliba.musicahumana;

import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.View;

import com.example.giuseppe.musicahumana.R;

public class FaceShapeActivity extends ActionBarActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_face_shape);
    }

    public void voltoOvale(View v){
        MainActivity.formaVolto = MainActivity.VOLTOOVALE;
        MainActivity.formaVoltoIsSelected=true;
        setResult(RESULT_OK);
        finish();
    }

    public void voltoQuadrato(View v){
        MainActivity.formaVolto = MainActivity.VOLTOQUADRATO;
        MainActivity.formaVoltoIsSelected=true;
        setResult(RESULT_OK);
        finish();
    }

    public void voltoRotondo(View v){
        MainActivity.formaVolto = MainActivity.VOLTOROTONDO;
        MainActivity.formaVoltoIsSelected=true;
        setResult(RESULT_OK);
        finish();
    }

    public void voltoRettangolare(View v){
        MainActivity.formaVolto = MainActivity.VOLTORETTANGOLARE;
        MainActivity.formaVoltoIsSelected=true;
        setResult(RESULT_OK);
        finish();
    }

    public void voltoTriangolare(View v){
        MainActivity.formaVolto = MainActivity.VOLTOTRIANGOLARE;
        MainActivity.formaVoltoIsSelected=true;
        setResult(RESULT_OK);
        finish();
    }

    public void voltoCuore(View v){
        MainActivity.formaVolto = MainActivity.VOLTOCUORE;
        MainActivity.formaVoltoIsSelected=true;
        setResult(RESULT_OK);
        finish();
    }

    public void voltoDiamante(View v){
        MainActivity.formaVolto = MainActivity.VOLTODIAMANTE;
        MainActivity.formaVoltoIsSelected=true;
        setResult(RESULT_OK);
        finish();
    }

}
