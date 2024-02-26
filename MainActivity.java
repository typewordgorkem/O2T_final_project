package com.example.realo2t;
import android.content.Intent;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import yuku.ambilwarna.AmbilWarnaDialog;

public class MainActivity extends AppCompatActivity {

    private static final String RASPBERRY_PI_IP = "192.168.78.102";
    private static final int RASPBERRY_PI_PORT = 9600;
    private boolean ledsOn = false;
    private EditText editText;
    private int selectedColor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button snakeGameButton = findViewById(R.id.snakeGameButton);
        editText = findViewById(R.id.editText);
        Button sendButton = findViewById(R.id.sendButton);
        Button colorPickerButton = findViewById(R.id.colorPickerButton);
        Button animationButton = findViewById(R.id.animationButton);
        selectedColor = Color.BLACK;

        snakeGameButton.setOnClickListener(v -> openSnakeGame());

        sendButton.setOnClickListener(v -> {
            String textToSend = editText.getText().toString();
            sendTextToLEDs(textToSend);
        });

        colorPickerButton.setOnClickListener(v -> openColorPicker());
        animationButton.setOnClickListener(v -> startAnimation());

    }
    private void openSnakeGame() {
        try {
            Intent intent = new Intent(this, SnakeGameActivity.class);
            startActivity(intent);
        } catch (Exception e) {
            Log.e("MainActivity", "Error opening SnakeControlActivity", e);
            Toast.makeText(this, "Error opening SnakeControlActivity: " + e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    private void openColorPicker() {
        AmbilWarnaDialog colorDialog = new AmbilWarnaDialog(this, selectedColor, new AmbilWarnaDialog.OnAmbilWarnaListener() {
            @Override
            public void onCancel(AmbilWarnaDialog dialog) {
                // Do nothing
            }

            @Override
            public void onOk(AmbilWarnaDialog dialog, int color) {
                selectedColor = color;
                Toast.makeText(MainActivity.this, "Color selected: " + color, Toast.LENGTH_SHORT).show();
            }
        });
        colorDialog.show();
    }

    private void sendTextToLEDs(String text) {
        String colorHex = String.format("#%06X", (0xFFFFFF & selectedColor));
        String command = "display_text:" + text + ":"+  colorHex + "\n";
        new SendMessageTask().execute(command);
    }

    public void toggleLEDs(View view) {
        ledsOn = !ledsOn;
        String command = ledsOn ? "all_on\n" : "all_off\n";
        new SendMessageTask().execute(command);
    }
    private void startAnimation() {
        String command = "start_animation\n";
        new SendMessageTask().execute(command);
    }

    public static class SendMessageTask extends AsyncTask<String, Void, Void> {

        @Override
        protected Void doInBackground(String... strings) {
            try (Socket socket = new Socket(RASPBERRY_PI_IP, RASPBERRY_PI_PORT);
                 OutputStream outputStream = socket.getOutputStream()) {
                outputStream.write(strings[0].getBytes());
                outputStream.flush();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }
    }
}