package uk.co.miajo.UGenTest;

/* 
 * A basic class for creating an individual string for the guitar model.
 * This utility class simply stores the pitches of each fret placement,
 * and has methods for setting or returning the frequency values of each pitch. 
 */

public class GuitarString {
	//A floating point array for storing the pitches of each fret placement on the string
	private float[] fretPitches; 
	
	//Class constructor 
	public GuitarString () {
		//Create new array, with as size defined by the number of frets available on instrument
		fretPitches = new float[GuitarUtils.NUM_FRETS];
	}
	//Return the pitches in the above array
	public float[] getPitches() {
		return fretPitches;
	}
	//Return the pitch for specified fret position
	public float getPitchForFret(int fret) {
		return fretPitches[fret];
	}
	//Set the pitch for the specified fret position
	public void setPitchForFret(int fret, float pitch) {
		fretPitches[fret] = pitch;
	}
}
