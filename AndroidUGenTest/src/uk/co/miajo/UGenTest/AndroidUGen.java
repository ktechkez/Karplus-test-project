/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package uk.co.miajo.UGenTest;

import uk.co.miajo.UGenTest.GestureView;
import uk.co.miajo.UGen.UGenAudio;
import android.app.Activity;
import android.content.Context;
import android.graphics.Point;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;

public class AndroidUGen extends Activity implements GestureListener, SensorEventListener {
	private UGenAudio audioThread;
	private GestureView mainView;
	private int[] triggers, frequencies, decays; // Store enum equivalent ints in arrays for shorter code
	private float[] decayAmounts;
	private float newPositionY = 0.f, lastPositionY = 0.f, strumVelocity = 0.f, neckLength = 0.f;
	// match the enum in the C++ code
	static final int FreqOne = 0;
	static final int FreqTwo = 1;
	static final int FreqThree = 2;
	static final int FreqFour = 3;
	static final int FreqFive = 4;
	static final int FreqSix = 5;
	static final int Cutoff = 6;
	static final int DecayOne = 7;
	static final int DecayTwo = 8;
	static final int DecayThree = 9;
	static final int DecayFour = 10;
	static final int DecayFive = 11;
	static final int DecaySix = 12;
	static final int TriggerOne = 13;
	static final int TriggerTwo = 14;
	static final int TriggerThree = 15;
	static final int TriggerFour = 16;
	static final int TriggerFive = 17;
	static final int TriggerSix = 18;
	static final int Dampening = 19;
	static final int Vibrato = 20;
	static final int Pressure = 21;
	// Below variables are used for tilt functions
	private float mLastX, mLastY, mLastZ;
	private boolean mInitialized;
	private SensorManager mSensorManager;
	private Sensor mAccelerometer;
	private final float NOISE = (float) 1;
	
	private GuitarString[] guitarStrings = new GuitarString[6]; 
	private int guitarStringsAmount;
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		mainView = (GestureView) findViewById(R.id.main_view);
		/* UGen setup */
		audioThread = new UGenAudio("/data/data/uk.co.miajo.UGen/lib"); // ??Context.getFilesDir().getPath()
		audioThread.start();
		// Fill static arrays with ints etc
		triggers = new int[] { TriggerOne, TriggerTwo, TriggerThree, TriggerFour, TriggerFive, TriggerSix };
		frequencies = new int[] { FreqOne, FreqTwo, FreqThree, FreqFour, FreqFive, FreqSix };
		decays = new int[] { DecayOne, DecayTwo, DecayThree, DecayFour, DecayFive, DecaySix };
		decayAmounts = new float[] { 0, 0, 0, 0, 0, 0 };
		setNeckLength(); // Initialise neckLength variable with is used extensively
		initStringPitches();
		
		// Set sensor stuff up
		mInitialized = false;
		mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
		mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);
	}
	
	private void initStringPitches() {
		guitarStringsAmount = guitarStrings.length;
		for(int numString = 0; numString < guitarStringsAmount; numString++){
			guitarStrings[numString] = new GuitarString();
			audioThread.setParameter(frequencies[numString], GuitarUtils.stringFundamentals[numString]);
			for(int fret = 0; fret < GuitarUtils.NUM_FRETS; fret++) {
				float pitch = GuitarUtils.calculatePitchForFret(numString, fret);
				guitarStrings[numString].setPitchForFret(fret, pitch);
			}
        }
        
	}
	
	protected void onResume() {
		super.onResume();
		mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);
	}

	protected void onPause() {
		super.onPause();
		mSensorManager.unregisterListener(this);
	}

	@Override
	public void onStop() {
		super.onStop();
		audioThread.sendQuit();
		while (!audioThread.isEnded()) {
			try {
				Thread.sleep(50L);
			} catch (InterruptedException err) {
				err.printStackTrace();
				break;
			}
		}
		audioThread.destroyIOHost();
	}

	private Point calculateScreenSize() {// Calculate the screen size as a point variable
		Display display = getWindowManager().getDefaultDisplay();
		Point size = new Point();
		display.getSize(size);
		return size;
	}

	private void setNeckLength() {// Calculate the neck length of the guitar
		Point size = calculateScreenSize();
		neckLength = (size.x / 3) * 2;
		Log.d("Sample rate", "Sample rate is " +audioThread.sampleRateInHz);
	}
	
	/*Calculates whether the string in question should be triggered based on
	 * whether the most recent and newest Y-axis points have crossed the string
	 * location
	 */
	private boolean shouldTrigger(float newPosY, float lastPosY, int stringPosition) {
		return ((stringPosition >= lastPosY && stringPosition < newPosY) || 
				(stringPosition < lastPosY && stringPosition > newPosY));
	}

	public void onTouchDown(MotionEvent event, int touchID) {// Called when a
																// touch down
																// gesture is
																// received
		float xCoordinates = event.getX(mainView.getIndex(event));
		float yCoordinates = event.getY(mainView.getIndex(event));
		float frequency = 0;
		// If the slider isn't being used, calculate the frequency and decay
		// values
		if (xCoordinates < neckLength && mainView.slideIsActive == false) {
			for (int c = 0; c < 6; c++) {// The loop is to check the X-axis
											// position against the position of
											// each string on guitar
				if (yCoordinates > (mainView.stringPositions[c] - 40)
						&& yCoordinates < (mainView.stringPositions[c] + 40)) {
					frequency = GuitarUtils.calculateFrequency(guitarStrings, mainView.fretPositions, xCoordinates, c);
					audioThread.setParameter(frequencies[c], frequency);
					decayAmounts[c] = (xCoordinates / neckLength * 10);
					audioThread.setParameter(decays[c], decayAmounts[c]);
				}
			}
		}
	}

	public void onTouchUp(MotionEvent event, int touchID) {// Called when a
															// touch up gesture
															// is received
		float xCoordinates = event.getX(mainView.getIndex(event));
		float yCoordinates = event.getY(mainView.getIndex(event));
		float decayReset = 0;
		if (xCoordinates < neckLength && mainView.slideIsActive == false) {
			for (int c = 0; c < guitarStringsAmount; c++) {// Loop simply resets the respective string frequency to its fundamental
				if (yCoordinates > (mainView.stringPositions[c] - 40)
						&& yCoordinates < (mainView.stringPositions[c] + 40)) {
					audioThread.setParameter(frequencies[c], GuitarUtils.stringFundamentals[c]);
					audioThread.setParameter(decays[c], decayReset);
				}
			}
		} else if (xCoordinates < neckLength && mainView.slideIsActive == true) {
			for (int c = 0; c < guitarStringsAmount; c++) {// Slide is active but lifted, so set all string frequencies back to fundamental
				audioThread.setParameter(frequencies[c], GuitarUtils.stringFundamentals[c]);
				audioThread.setParameter(decays[c], decayReset);
			}
		}
		if (xCoordinates > neckLength) {// Reset variables used in strumming
										// functions
			lastPositionY = 0;
		}
	}

	public void onMove(int newPointX, int newPointY, int touchID) {// Called
																	// when a
																	// touch
																	// pointer
																	// moves
																	// along any
																	// axis
		if (newPointX > neckLength) { // If movement is a strum or pick action
			newPositionY = newPointY;
			audioThread.setParameter(Pressure, mainView.strumPressure); //Set string pick pressure
			if (mainView.velocityTrackerExists == true)
				strumVelocity = Math.abs(mainView.vTracker.getYVelocity(mainView.strumPointer));
			Log.d("Velocity", "Strum velocity is " +strumVelocity);
			for (int c = 0; c < guitarStringsAmount; c++) { // Trigger the respective strings based on movement across string location
				if (newPositionY != lastPositionY && lastPositionY > 0) {
					if (shouldTrigger(newPositionY, lastPositionY, mainView.stringPositions[c]))
						audioThread.sendTrigger(triggers[c]);
					if(strumVelocity > 1000){
						audioThread.setParameter(Cutoff, strumVelocity+4000);
						audioThread.setParameter(decays[c], decayAmounts[c] + 2);
					}
					else{
						audioThread.setParameter(Cutoff, 4000);
						audioThread.setParameter(decays[c], decayAmounts[c]);
					}
				}
			}
			lastPositionY = newPositionY;
		}
		// The below conditions simply amend the frequency values of each string
		// based on fret hand movement
		else if (newPointX < neckLength && mainView.slideIsActive == false) {
			float frequencyAdd;
			for (int c = 0; c < guitarStringsAmount; c++) {
				if (newPointY > (mainView.stringPositions[c] - 40)
						&& newPointY < (mainView.stringPositions[c] + 40)) {
					frequencyAdd = GuitarUtils.calculateFrequency(guitarStrings, mainView.fretPositions, newPointX, c);
					float stringBendAmount = GuitarUtils.calculateStringBendFactor(newPointY, (frequencyAdd + GuitarUtils.stringFundamentals[c]), mainView.stringPositions[c]);
					audioThread.setParameter(frequencies[c], frequencyAdd + stringBendAmount);
					decayAmounts[c] = (newPointX / neckLength * 10);
					audioThread.setParameter(decays[c], decayAmounts[c]);
				}
			}
		} else if (newPointX < neckLength && mainView.slideIsActive == true) {
			float frequency = 0;
			for (int c = 0; c < guitarStringsAmount; c++) {
				frequency = GuitarUtils.calculateFrequency(guitarStrings, mainView.fretPositions, mainView.slidePoint, c);
				audioThread.setParameter(frequencies[c], frequency);
				decayAmounts[c] = (mainView.slidePoint / neckLength * 10);
				audioThread.setParameter(decays[c], decayAmounts[c]);
			}
		}
	}

	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// can be safely ignored for this demo (Unused sensor listener function)
	}

	@Override
	public void onSensorChanged(SensorEvent event) {
		// When sensor axis values are changed, save new points
		float y = event.values[0];
		float x = event.values[1];
		float z = event.values[2];
		if (!mInitialized) {
			mLastX = x;
			mLastY = y;
			mLastZ = z;
			mInitialized = true;
		} else {
			// Calculate new positions
			float deltaX = Math.abs(mLastX - x);
			float deltaY = Math.abs(mLastY - y);
			float deltaZ = Math.abs(mLastZ - z);
			// If change is less than the NOISE value, then ignore
			if (deltaX < NOISE)
				deltaX = (float) 0.0;
			if (deltaY < NOISE)
				deltaY = (float) 0.0;
			if (deltaZ < NOISE)
				deltaZ = (float) 0.0;
			// Creates a palm dampening affect based on X-axist tilt
			if (deltaY > 9)
				deltaY = 9; // Only allow the decay value to go as low as 1
							// second
			for (int c = 0; c < 6; c++) {
				if ((decayAmounts[c] + deltaY) > 9)
					deltaY = 9 - decayAmounts[c];
			}
			//audioThread.setParameter(Dampening, deltaY);
			float vibrato = deltaX / 10;
			//audioThread.setParameter(Vibrato, vibrato);
		}
	}
}
