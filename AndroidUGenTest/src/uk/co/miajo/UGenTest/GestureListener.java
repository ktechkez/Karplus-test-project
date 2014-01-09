package uk.co.miajo.UGenTest;

import android.view.MotionEvent;

//Implements a new listener class that response to multi-touch information
//These are called in the Gesture View and the methods dealt with in main activity 
public interface GestureListener {

    public void onTouchDown(MotionEvent event, int touchID);

    public void onTouchUp(MotionEvent event, int touchID);

    public void onMove(int newPointX, int newPointY, int touchID);
}
