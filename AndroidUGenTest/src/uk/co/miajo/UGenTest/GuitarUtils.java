package uk.co.miajo.UGenTest;
/*
 * A utility class with methods for emulating guitar features and characteristics. 
 * These methods and variables are kept within their own class for clarity and separation
 * from the core and user interface code. 
 * Methods are used for calculate string pitch, frequency from the user note input and string bend modulation.
 */
public class GuitarUtils {

	// Constants, including the number of frets and the fundamental string frequencies. 
	//Guitar is tuned to Open G for blues style
	public static float[] stringFundamentals = new float[] { 293.66f, 246.94f,
			196, 146.83f, 98, 73.42f };
	
	public static int NUM_FRETS = 18;
	public static double TWELTH_ROUTE_OF_TWO = 1.05946309436; //This is used for calculating fret position and frequency. 
	
	// Utility methods
	
	//This method simply calculates the returns the pitch for each fret of each string
	// based on string fundamental * 1/12 to the power of fret number. 
	public static float calculatePitchForFret(int numString, int fret) {
		return GuitarUtils.stringFundamentals[numString] * (float) Math.pow(TWELTH_ROUTE_OF_TWO, (double) fret + 1);
	}
	
	//Calculates pitch bend applied when called and adds/takes it away from pitch of note
	public static float calculateStringBendFactor(float touchPositionY, float pitch, float originalStringPosition)
	{
		float maxBendInPixels = 30;//Maximum string bend available in pixels
		//Calculates two semitones higher than original value- This is the max pitch bend in frequency
		float twoSemitoneUp = pitch * (float) Math.pow(TWELTH_ROUTE_OF_TWO, (double) 2); 
		//Calculates the string bend amount as a fator of one
		float bendFactor = Math.abs(touchPositionY - originalStringPosition)/maxBendInPixels;
		return bendFactor * (twoSemitoneUp - pitch); // Return bend factor multiplied by bend amount
	}
	
	//Calculates frequency of note when a string is dampened on fretboard
	public static float calculateFrequency(GuitarString[] guitarStrings, float[] fretPositions, float touchPositionX, int stringIndex) {
		int fretBehindTouch = 0;
		boolean touchPointIsOnFret = false; //Used to check whether the touch point is exactly on fret 
		float highestPitch = guitarStrings[stringIndex].getPitchForFret(NUM_FRETS - 1); //Highest pitcch available on string
		//The loop runs checks up the fret board until the appropriate positioning is found
		for (int c = 0; c < NUM_FRETS; c++) {
			//Code will run when the touch position on x axis aka the string is
			// Above the fret number in loop count and behind the next fret
			// and less than the highest fret on neck
			if (touchPositionX > fretPositions[c]
					&& touchPositionX < fretPositions[c + 1]
					&& touchPositionX < fretPositions[NUM_FRETS - 1]) {
				fretBehindTouch = c; // Fret behind touch point is determined
				break;
			//Code will run here if the touch point is exactly on the fret
			//This is simpler pitch calculation as it is exact
			} else if (touchPositionX == fretPositions[c]) {
				touchPointIsOnFret = true;
				fretBehindTouch = c;
				break;
			} else if (touchPositionX > fretPositions[NUM_FRETS - 1]) { 
				//Input has reached end of neck, return highest pitch
				return highestPitch;
			}
		}
		//Calculate the different between fret positions on screen 
		float differenceBetweenFrets = fretPositions[fretBehindTouch + 1]
				- fretPositions[fretBehindTouch];
		//Calculate increment point from the fret behind touch 
		float increment = touchPositionX
				- fretPositions[fretBehindTouch];
		//Calculate the ratio of touch point between frets
		float ratio = increment / differenceBetweenFrets;
		//Run loop checking each string
		for(int numString = 0; numString < guitarStrings.length; numString++) {
			if(touchPointIsOnFret && numString == stringIndex) {
				//If touch point is on fret, return that frets pitch
				return guitarStrings[numString].getPitchForFret(fretBehindTouch);
			} else if(!touchPointIsOnFret && numString == stringIndex){
				//If touch point is between frets...
				float differenceBetweenFrequencies = guitarStrings[numString].getPitchForFret(fretBehindTouch + 1)
						- guitarStrings[numString].getPitchForFret(fretBehindTouch);
				//Pitch = pitch of fret behind touch point + (Ratio of touch position between frets * difference betwee frequencies
				//This calculates the desired frequency for desired point of contact on string
				return (guitarStrings[numString].getPitchForFret(fretBehindTouch) + (ratio * differenceBetweenFrequencies));
			}
		}
		return 0;
	}
}
