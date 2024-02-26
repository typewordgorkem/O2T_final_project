package com.example.realo2t;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import androidx.appcompat.app.AppCompatActivity;


public class SnakeGameActivity extends AppCompatActivity {

    private Button startPauseButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_snake_game);

        Button startPauseButton = findViewById(R.id.startPauseButton);
        Button upButton = findViewById(R.id.upButton);
        Button downButton = findViewById(R.id.downButton);
        Button leftButton = findViewById(R.id.leftButton);
        Button rightButton = findViewById(R.id.rightButton);

        startPauseButton.setOnClickListener(new View.OnClickListener() {
            private boolean isPaused = true;

            @Override
            public void onClick(View v) {
                isPaused = !isPaused;
                startPauseButton.setText(isPaused ? "Start" : "Pause");
                sendCommand(isPaused ? "snake_pause" : "snake_start");
            }
        });

        upButton.setOnClickListener(v -> sendCommand("snake_up"));

        downButton.setOnClickListener(v -> sendCommand("snake_down"));

        leftButton.setOnClickListener(v -> sendCommand("snake_left"));

        rightButton.setOnClickListener(v -> sendCommand("snake_right"));


    }
    private void sendCommand(String command) {
        command += "\n";
        new MainActivity.SendMessageTask().execute(command);
    }
}
