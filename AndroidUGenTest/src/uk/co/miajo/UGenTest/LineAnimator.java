package uk.co.miajo.UGenTest;

import android.util.Log;

public class LineAnimator implements Runnable {
	
	public void animateLine(int stringIndex, float decay){
		
	}
	@Override
    public void run() {
		
		// Moves the current Thread into the background
        android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND);
        /*
         * Stores the current Thread in the PhotoTask instance,
         * so that the instance
         * can interrupt the Thread.
         */
	}
//Doesn't work - needs seperate thread
/*public void animateLine(int stringIndex, float decay){
	long triggerTime = System.currentTimeMillis();
	long end = triggerTime + 10000;
	int positionalChange = 5;
	while(System.currentTimeMillis() < end){
		Log.d("Loop", "String position is " +stringPositions[stringIndex]);
		if(animateUp == false){ 
			stringPositions[stringIndex] += positionalChange; 
			animateUp = true;
			positionalChange = 10;
			}
		else { 
			stringPositions[stringIndex] -= positionalChange; 
			animateUp = false;
			positionalChange = 10;
			}
		invalidate();
		try{ Thread.sleep(20); }catch(InterruptedException e){ }
	}
	positionalChange = 5;
	if(animateUp == false) stringPositions[stringIndex] += positionalChange; 
	else stringPositions[stringIndex] -= positionalChange; 
	invalidate();
}*/
}