package uk.co.miajo.UGenTest;

public class GuitarUtils {

	// Constants
	public static float[] stringFundamentals = new float[] { 293.66f, 246.94f,
			196, 146.83f, 98, 73.42f };
	
	public static int NUM_FRETS = 18;
	public static double TWELTH_ROUTE_OF_TWO = 1.05946309436;
	
	// Utility methods
	
	public static float calculatePitchForString(int numString, int fret) {
		return GuitarUtils.stringFundamentals[numString] * (float) Math.pow(TWELTH_ROUTE_OF_TWO, (double) fret + 1);
	}
	
	public static float calculateStringBendFactor(float touchPositionY, float pitch, float originalStringPosition)
	{
		float maxBendInPixels = 30;
		float twoSemitoneUp = pitch * (float) Math.pow(TWELTH_ROUTE_OF_TWO, (double) 2); 
		float bendFactor = Math.abs(touchPositionY - originalStringPosition)/maxBendInPixels;
		return bendFactor * (twoSemitoneUp - pitch);
	}
	
	public static float calculateFrequency(GuitarString[] guitarStrings, float[] fretPositions, float touchPositionX, int stringIndex) {
		int fretBehindTouch = 0;
		boolean touchPointIsOnFret = false;
		float highestPitch = guitarStrings[stringIndex].getPitchForFret(NUM_FRETS - 1);
		for (int c = 0; c < NUM_FRETS; c++) {
			if (touchPositionX > fretPositions[c]
					&& touchPositionX < fretPositions[c + 1]
					&& touchPositionX < fretPositions[NUM_FRETS - 1]) {
				fretBehindTouch = c;
				break;
			} else if (touchPositionX == fretPositions[c]) {
				touchPointIsOnFret = true;
				fretBehindTouch = c;
				break;
			} else if (touchPositionX > fretPositions[NUM_FRETS - 1]) { 
				//Input has reached end of neck
				return highestPitch;
			}
		}
		
		float differenceBetweenFrets = fretPositions[fretBehindTouch + 1]
				- fretPositions[fretBehindTouch];
		float increment = touchPositionX
				- fretPositions[fretBehindTouch];
		float ratio = increment / differenceBetweenFrets;
		
		for(int numString = 0; numString < guitarStrings.length; numString++) {
			if(touchPointIsOnFret && numString == stringIndex) {
				return guitarStrings[numString].getPitchForFret(fretBehindTouch);
			} else if(!touchPointIsOnFret && numString == stringIndex){
				float differenceBetweenFrequencies = guitarStrings[numString].getPitchForFret(fretBehindTouch + 1)
						- guitarStrings[numString].getPitchForFret(fretBehindTouch);
				return (guitarStrings[numString].getPitchForFret(fretBehindTouch) + (ratio * differenceBetweenFrequencies))
						- stringFundamentals[numString];
			}
		}
		return 0;
	}
}
