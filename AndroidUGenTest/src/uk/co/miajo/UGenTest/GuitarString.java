package uk.co.miajo.UGenTest;

public class GuitarString {
	private float[] fretPitches;
	
	public GuitarString () {
		fretPitches = new float[GuitarUtils.NUM_FRETS];
	}
	
	public float[] getPitches() {
		return fretPitches;
	}
	
	public float getPitchForFret(int fret) {
		return fretPitches[fret];
	}
	
	public void setPitchForFret(int fret, float pitch) {
		fretPitches[fret] = pitch;
	}
}
